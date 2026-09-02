#include <stdint.h>
#include <stdlib.h>

/*

// game.c
game_init()
game_update()
game_reset()
game_next_turn()
game_handle_press()
game_win()
game_lose()
sequence_generate()
  */

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
            // play_sound(RED_TONE, 100);
            set_led(RED_LED, buzzer_enabled);
            continue;
          }

          uint8_t level = levels_arr[i];
          // play_sound(button_tones[led_arr[i]], 100);

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
            // play_sound(RED_TONE, 100);
            set_led(RED_LED, buzzer_enabled);
            continue;
          }

          uint8_t level = levels_arr[i];
          // play_sound(button_tones[led_arr[i]], 100);

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
