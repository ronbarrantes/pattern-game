#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/delay.h>

#define SHORT_DELAY 50
#define MID_DELAY 300
#define LONG_DELAY 500

#define RED_LED PB1
#define YELLOW_LED PB2
#define GREEN_LED PB3
#define BLUE_LED PB4

typedef struct {
  uint8_t led;
  uint16_t duration;
} LightPattern;

uint8_t pb_arr[] = {RED_LED, YELLOW_LED, GREEN_LED, BLUE_LED};

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

void set_led(uint8_t pin, bool state) {
  if (state) {
    PORTB &= ~(1 << pin);
    DDRB |= (1 << pin);
  } else {
    DDRB &= ~(1 << pin);
    PORTB |= (1 << pin);
  }
}

void delay_ms(uint16_t duration_ms) {
  while (duration_ms > 0) {
    _delay_ms(1);
    duration_ms--;
  }
}

bool check_pressed(uint8_t pin) {
  set_led(pin, false);
  delay_ms(5);

  return !(PINB & (1 << pin));
}

void startup_sequence(uint8_t pin_arr[], uint8_t pb_length) {
  for (int i = 0; i < pb_length; i++) {
    set_led(pin_arr[i], true);
    delay_ms(SHORT_DELAY);

    set_led(pin_arr[i], false);
    delay_ms(SHORT_DELAY);
  }

  delay_ms(SHORT_DELAY * 3);

  for (int i = pb_length - 1; i >= 0; i--) {
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
    delay_ms(LONG_DELAY);

    set_led(pattern_list[i], false);
    delay_ms(LONG_DELAY);
  }
}

void game_init(uint8_t game_length, uint8_t pattern_list[], uint8_t pb_arr[],
               uint8_t pb_length) {
  for (int l_item = 0; l_item < game_length; l_item++) {
    uint8_t idx = rand() % pb_length;
    pattern_list[l_item] = pb_arr[idx];
  }
}

uint8_t choose_level(uint8_t pb_arr[], uint8_t pb_length, uint16_t *seed) {
  uint8_t levels_arr[] = {5, 10, 15, 20};

  for (uint8_t blink = 0; blink < 20; blink++) {

    // ON phase
    for (uint8_t scan = 0; scan < 10; scan++) {
      (*seed)++;

      for (uint8_t i = 0; i < pb_length; i++) {
        bool pressed = check_pressed(pb_arr[i]);

        if (pressed) {
          uint8_t level = levels_arr[i];

          while (check_pressed(pb_arr[i])) {
            (*seed)++;
          }

          // Turn everything off before leaving
          for (uint8_t j = 0; j < pb_length; j++) {
            set_led(pb_arr[j], false);
          }

          return level;
        }

        // check_pressed() turns the LED off,
        // so restore it during the ON phase
        set_led(pb_arr[i], true);
      }
    }

    // Turn all LEDs off
    for (uint8_t i = 0; i < pb_length; i++) {
      set_led(pb_arr[i], false);
    }

    // OFF phase
    for (uint8_t scan = 0; scan < 10; scan++) {
      (*seed)++;

      for (uint8_t i = 0; i < pb_length; i++) {
        if (check_pressed(pb_arr[i])) {
          uint8_t level = levels_arr[i];

          while (check_pressed(pb_arr[i])) {
            (*seed)++;
          }

          // Turn everything off before leaving
          for (uint8_t j = 0; j < pb_length; j++) {
            set_led(pb_arr[j], false);
          }

          return level;
        }

        set_led(pb_arr[i], false);
      }
    }
  }

  // Nothing selected
  return 0;
}

bool game_play(uint8_t pattern_list[], uint8_t curr_turn, uint8_t pb_arr[],
               uint8_t pb_length) {
  uint8_t curr_guess;

  sequence_light(pattern_list, curr_turn);

  for (uint8_t idx = 0; idx < curr_turn; idx++) {
    bool got_press = false;

    while (!got_press) {
      for (uint8_t j = 0; j < pb_length; j++) {
        if (check_pressed(pb_arr[j])) {
          curr_guess = pb_arr[j];
          got_press = true;
          break;
        }
      }
    }

    while (check_pressed(curr_guess)) {
      // wait for release
    }

    if (curr_guess != pattern_list[idx]) {
      return false;
    }

    delay_ms(1000);
  }

  return true;
}

//// MAIN FUNCTION
int main(void) {
  uint8_t game_length;
  uint8_t curr_turn = 1;
  uint8_t pattern_list[20];

  uint8_t pb_length = sizeof(pb_arr) / sizeof(pb_arr[0]);

  uint8_t startup_pattern_length =
    sizeof(startup_pattern) / sizeof(startup_pattern[0]);
  uint8_t win_pattern_length = sizeof(win_pattern) / sizeof(win_pattern[0]);
  uint8_t lose_pattern_length = sizeof(lose_pattern) / sizeof(lose_pattern[0]);

  uint16_t seed = 0;

  for (int i = 0; i < pb_length; i++) {
    set_led(pb_arr[i], false);
  }

  delay_ms(100);

  // Play startup sequence
  light_pattern(startup_pattern, startup_pattern_length);

  // Wait a bit before choosing level
  delay_ms(1000);

  // Choose difficulty and use the timing of that press as the random seed
  game_length = choose_level(pb_arr, pb_length, &seed);

  // No level was selected
  if (game_length == 0) {
    // later: enter sleep mode here
    return 0;
  }

  srand(seed);

  // Small pause before the game begins
  delay_ms(1000);

  game_init(game_length, pattern_list, pb_arr, pb_length);

  for (int i = 0; i < pb_length; i++) {
    set_led(pb_arr[i], false);
  }

  while (curr_turn <= game_length) {
    bool is_pattern_correct =
      game_play(pattern_list, curr_turn, pb_arr, pb_length);

    if (!is_pattern_correct) {
      light_pattern(lose_pattern, lose_pattern_length);

      return 0;
    }

    curr_turn++;
  }

  light_pattern(win_pattern, win_pattern_length);

  return 0;
}
