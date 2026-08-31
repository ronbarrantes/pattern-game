#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 1000000UL

#define SHORT_DELAY 50
#define MID_DELAY 300
#define LONG_DELAY 500

#define RED_LED PB2
#define BLUE_LED PB1
#define YELLOW_LED PB3
#define GREEN_LED PB4

#define RED_TONE 262
#define YELLOW_TONE 330
#define GREEN_TONE 392
#define BLUE_TONE 523

#define MELODY_END {0, 0}

/*
rough plan

// main.c
main()

// game.c
game_init()
game_update()
game_reset()
game_next_turn()
game_handle_press()
game_win()
game_lose()
sequence_generate()

// panel.c
panel_init()
panel_update()
panel_set_led()
panel_all_off()
panel_get_pressed()
panel_wait_release()
panel_start_pattern()
panel_pattern_update()

// utils.c
delay_ms()
*/

// I want something like this in the end

/*

int main(void) {
    Game game;
    Panel panel;
    MelodyPlayer player;

    game_init(&game);
    panel_init(&panel);
    melody_init(&player);

    while (1) {
        panel_update(&panel);
        melody_update(&player);
        game_update(&game, &panel, &player);
    }
}

*/

typedef struct {
  uint8_t led;
  uint16_t duration;
} LightPattern;

typedef struct {
  uint16_t frequency;
  uint16_t duration_ms;
} Note;

typedef struct {
  const Note *melody;
  uint8_t curr_note;
  // the function will contain a sentinel to ensure the
  // note ends {0, 0}
} MelodyPlayer;

typedef struct {
  uint8_t led;
  uint16_t duration_ms;
} Light;

typedef struct {
  const Light *sequence;
  uint8_t curr_led;
  uint8_t sequence_length;
  uint32_t started_at;
} SequencePlayer;

uint8_t led_arr[] = {RED_LED, YELLOW_LED, GREEN_LED, BLUE_LED};

uint16_t button_tones[] = {
  0,
  BLUE_TONE,
  RED_TONE,
  YELLOW_TONE,
  GREEN_TONE,
};

static bool buzzer_enabled = true;
static bool sound_playing = false;

LightPattern startup_pattern[] = {
  {RED_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {BLUE_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {RED_LED, SHORT_DELAY},
};

LightPattern lose_pattern[] = {
  {BLUE_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {RED_LED, MID_DELAY},
  {RED_LED, MID_DELAY},
  {RED_LED, MID_DELAY},
  {RED_LED, MID_DELAY},
};

LightPattern win_pattern[] = {
  {RED_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {BLUE_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
};

Note lose_melody[] = {
  {BLUE_TONE, 150},
  {GREEN_TONE, 150},
  {YELLOW_TONE, 150},
  {RED_TONE, 400},
  MELODY_END,
};

Note win_melody[] = {
  {RED_TONE, 100},
  {YELLOW_TONE, 100},
  {GREEN_TONE, 100},
  {BLUE_TONE, 300},
  MELODY_END,
};

void play_sound(uint16_t frequency, uint16_t duration_ms) {
  if (!buzzer_enabled) {
    sound_playing = false;
    return;
  }

  sound_playing = true;

  DDRB |= (1 << PB0);

  // CTC mode
  // Toggle OC0A (PB0) every time timer matches OCR0A
  TCCR0A = (1 << COM0A0) | (1 << WGM01);

  // Prescaler = 8
  TCCR0B = (1 << CS01);

  // Calculate how high Timer0 should count
  OCR0A = (F_CPU / (2UL * 8 * frequency)) - 1;

  // Timer hardware is now generating the tone on PB0.
  while (duration_ms > 0) {
    _delay_ms(1);
    duration_ms--;
  }

  // Stop Timer0
  TCCR0A = 0;
  TCCR0B = 0;

  // PB0 LOW
  sound_playing = false;
  PORTB &= ~(1 << PB0);
}

void sound_start(uint16_t frequency) {
  if (!buzzer_enabled) {
    sound_playing = false;
    return;
  }

  DDRB |= (1 << PB0);

  // CTC mode
  // Toggle OC0A (PB0) every time timer matches OCR0A
  TCCR0A = (1 << COM0A0) | (1 << WGM01);

  // Prescaler = 8
  TCCR0B = (1 << CS01);

  // Calculate how high Timer0 should count
  OCR0A = (F_CPU / (2UL * 8 * frequency)) - 1;
  sound_playing = true;
}

void sound_stop(void) {
  // Stop Timer0
  TCCR0A = 0;
  TCCR0B = 0;

  // PB0 LOW

  PORTB &= ~(1 << PB0);

  sound_playing = false;
}

void play_melody(Note melody[], uint8_t melody_length) {
  for (uint8_t i = 0; i < melody_length; i++) {
    play_sound(melody[i].frequency, melody[i].duration_ms);
  }
}

// PANEL
void set_led(uint8_t pin, bool state) {
  if (state) {
    PORTB &= ~(1 << pin);
    DDRB |= (1 << pin);
  } else {
    DDRB &= ~(1 << pin);
    PORTB |= (1 << pin);
  }
}

// I'll try not to use this one too much
void delay_ms(uint16_t duration_ms) {
  while (duration_ms > 0) {
    _delay_ms(1);
    duration_ms--;
  }
}

void blink_led(uint8_t pin, uint8_t count, uint16_t duration_ms) {
  for (uint8_t i = 0; i < count; i++) {
    set_led(pin, true);
    delay_ms(duration_ms);

    set_led(pin, false);
    delay_ms(duration_ms);
  }
}

bool check_pressed(uint8_t pin) {
  set_led(pin, false);
  delay_ms(5);

  return !(PINB & (1 << pin));
}

void melody_start(MelodyPlayer *player, const Note *melody) {
  player->melody = melody;
  player->curr_note = 0;

  Note note = player->melody[0];

  if (note.frequency == 0 && note.duration_ms == 0) {
    player->melody = NULL;
    return;
  }

  play_sound(note.frequency, note.duration_ms);
}

void sequence_start(SequencePlayer *player, const Light *sequence,
                    uint8_t sequence_length) {
  player->sequence = sequence;
  player->sequence_length = sequence_length;
  player->curr_led = 0;

  set_led(player->sequence->led, true);
}

void sequence_update(SequencePlayer *player) {
  if (player->sequence == NULL)
    return;

  Light light = player->sequence[player->curr_led];

  if (player->curr_led >= player->sequence_length) {
    player->sequence = NULL;
    set_led(light.led, false);
    return;
  }

  set_led(light.led, true);
  _delay_ms(light.duration_ms);
  set_led(light.led, false);

  player->curr_led++;
}

void sequence_stop(SequencePlayer *player) {
  if (player->sequence == NULL)
    return;

  Light light = player->sequence[player->curr_led];

  set_led(light.led, false);
  player->sequence = NULL;
}

void melody_update(MelodyPlayer *player) {
  if (player->melody == NULL)
    return;

  if (sound_playing) {
    return;
  }

  Note note = player->melody[player->curr_note];

  if (note.frequency == 0 && note.duration_ms == 0) {
    player->melody = NULL;
    sound_stop();
    return;
  }

  play_sound(note.frequency, note.duration_ms);
}

void startup_sequence(uint8_t pin_arr[], uint8_t led_length) {
  for (int i = 0; i < led_length; i++) {
    set_led(pin_arr[i], true);
    delay_ms(SHORT_DELAY);

    set_led(pin_arr[i], false);
    delay_ms(SHORT_DELAY);
  }

  delay_ms(SHORT_DELAY * 3);

  for (int i = led_length - 1; i >= 0; i--) {
    set_led(pin_arr[i], true);
    delay_ms(SHORT_DELAY);

    set_led(pin_arr[i], false);
    delay_ms(SHORT_DELAY);
  }
}

void light_pattern(LightPattern pattern[], uint8_t pattern_length) {
  for (int i = 0; i < pattern_length; i++) {
    set_led(pattern[i].led, true);
    delay_ms(pattern[i].duration);

    set_led(pattern[i].led, false);
    delay_ms(pattern[i].duration);
  }
}

// do something with this.
void sequence_light(uint8_t pattern_list[], uint8_t curr_turn) {
  for (int i = 0; i < curr_turn; i++) {
    set_led(pattern_list[i], true);
    if (buzzer_enabled) {
      play_sound(button_tones[pattern_list[i]], LONG_DELAY);
    } else {
      delay_ms(LONG_DELAY);
    }

    set_led(pattern_list[i], false);
    if (i + 1 < curr_turn) {
      delay_ms(LONG_DELAY);
    }
  }
}

void game_init(uint8_t game_length, uint8_t pattern_list[], uint8_t led_arr[],
               uint8_t led_length) {
  for (int l_item = 0; l_item < game_length; l_item++) {
    uint8_t idx = rand() % led_length;
    pattern_list[l_item] = led_arr[idx];
  }
}

uint8_t choose_level(uint8_t led_arr[], uint8_t led_length, uint16_t *seed) {
  // Red toggles the buzzer. Yellow, green, and blue choose the level.
  uint8_t levels_arr[] = {0, 5, 10, 20};

  for (uint8_t blink = 0; blink < 20; blink++) {

    // ON phase
    for (uint8_t scan = 0; scan < 10; scan++) {
      (*seed)++;

      for (uint8_t i = 0; i < led_length; i++) {
        bool pressed = check_pressed(led_arr[i]);

        if (pressed) {
          while (check_pressed(led_arr[i])) {
            (*seed)++;
          }

          if (led_arr[i] == RED_LED) {
            buzzer_enabled = !buzzer_enabled;
            play_sound(RED_TONE, 100);
            set_led(RED_LED, buzzer_enabled);
            continue;
          }

          uint8_t level = levels_arr[i];
          play_sound(button_tones[led_arr[i]], 100);

          // Turn everything off before leaving
          for (uint8_t j = 0; j < led_length; j++) {
            set_led(led_arr[j], false);
          }

          blink_led(led_arr[i], 3, SHORT_DELAY);
          return level;
        }

        // check_pressed() turns the LED off,
        // so restore it during the ON phase
        set_led(led_arr[i], led_arr[i] == RED_LED ? buzzer_enabled : true);
      }
    }

    // The red LED shows the buzzer state while the other LEDs blink off.
    for (uint8_t i = 0; i < led_length; i++) {
      set_led(led_arr[i], led_arr[i] == RED_LED && buzzer_enabled);
    }

    // OFF phase
    for (uint8_t scan = 0; scan < 10; scan++) {
      (*seed)++;

      for (uint8_t i = 0; i < led_length; i++) {
        if (check_pressed(led_arr[i])) {
          while (check_pressed(led_arr[i])) {
            (*seed)++;
          }

          if (led_arr[i] == RED_LED) {
            buzzer_enabled = !buzzer_enabled;
            play_sound(RED_TONE, 100);
            set_led(RED_LED, buzzer_enabled);
            continue;
          }

          uint8_t level = levels_arr[i];
          play_sound(button_tones[led_arr[i]], 100);

          // Turn everything off before leaving
          for (uint8_t j = 0; j < led_length; j++) {
            set_led(led_arr[j], false);
          }

          blink_led(led_arr[i], 3, SHORT_DELAY);
          return level;
        }

        set_led(led_arr[i], led_arr[i] == RED_LED && buzzer_enabled);
      }
    }
  }

  // Nothing selected
  return 0;
}

bool game_play(uint8_t pattern_list[], uint8_t curr_turn, uint8_t led_arr[],
               uint8_t led_length) {
  uint8_t curr_guess;

  sequence_light(pattern_list, curr_turn);

  for (uint8_t idx = 0; idx < curr_turn; idx++) {
    bool got_press = false;

    while (!got_press) {
      uint8_t buttons = PINB;

      for (uint8_t j = 0; j < led_length; j++) {
        if (!(buttons & (1 << led_arr[j]))) {
          curr_guess = led_arr[j];
          got_press = true;
          break;
        }
      }
    }

    // START THE TONE
    sound_start(button_tones[curr_guess]);
    while (check_pressed(curr_guess)) {
      // wait for release
    }

    // stop the sound
    sound_stop();

    if (curr_guess != pattern_list[idx]) {
      return false;
    }
  }

  return true;
}

//// MAIN FUNCTION
int main(void) {
  while (1) {
    uint8_t game_length;
    uint8_t curr_turn = 1;
    uint8_t pattern_list[20];

    uint8_t led_length = sizeof(led_arr) / sizeof(led_arr[0]);
    uint8_t startup_pattern_length =
      sizeof(startup_pattern) / sizeof(startup_pattern[0]);
    uint8_t win_pattern_length = sizeof(win_pattern) / sizeof(win_pattern[0]);
    uint8_t lose_pattern_length =
      sizeof(lose_pattern) / sizeof(lose_pattern[0]);
    uint8_t win_melody_length = sizeof(win_melody) / sizeof(win_melody[0]);
    uint8_t lose_melody_length = sizeof(lose_melody) / sizeof(lose_melody[0]);

    uint16_t seed = 0;

    for (int i = 0; i < led_length; i++) {
      set_led(led_arr[i], false);
    }

    delay_ms(100);

    // Play startup sequence
    light_pattern(startup_pattern, startup_pattern_length);

    // Wait a bit before choosing level
    delay_ms(1000);

    // Choose difficulty and use the timing of that press as the random seed
    game_length = choose_level(led_arr, led_length, &seed);

    // No level was selected
    if (game_length == 0) {
      // later: enter sleep mode here
      return 0;
    }

    srand(seed);

    // Small pause before the game begins
    delay_ms(1000);

    game_init(game_length, pattern_list, led_arr, led_length);

    bool is_pattern_correct;

    for (int i = 0; i < led_length; i++) {
      set_led(led_arr[i], false);
    }

    while (curr_turn <= game_length) {
      is_pattern_correct =
        game_play(pattern_list, curr_turn, led_arr, led_length);

      if (!is_pattern_correct) {
        play_melody(lose_melody, lose_melody_length);
        light_pattern(lose_pattern, lose_pattern_length);

        break;
      }

      curr_turn++;

      if (curr_turn <= game_length) {
        delay_ms(1000);
      }
    }

    if (is_pattern_correct) {
      play_melody(win_melody, win_melody_length);
      light_pattern(win_pattern, win_pattern_length);
    }

    delay_ms(2000);
  }

  return 0;
}
