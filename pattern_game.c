#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define STARTUP_DELAY 50
#define GAMEPLAY_DELAY 250

void set_led(uint8_t pin, bool state) {
  if (state) {
    PORTB &= ~(1 << pin);
    DDRB |= (1 << pin);
  } else {
    DDRB &= ~(1 << pin);
    PORTB |= (1 << pin);
  }
}

bool check_pressed(uint8_t pin) {
  set_led(pin, false);
  _delay_ms(5);

  return !(PINB & (1 << pin));
}

void startup_sequence(uint8_t pin_arr[], uint8_t pb_lenght) {
  for (int i = 0; i < pb_lenght; i++) {
    // LED on
    set_led(pin_arr[i], true);
    _delay_ms(STARTUP_DELAY);

    // LED off
    set_led(pin_arr[i], false);
    _delay_ms(STARTUP_DELAY);
  }

  // a tiny little break
  _delay_ms(STARTUP_DELAY * 3);

  for (int i = pb_lenght - 1; i >= 0; i--) {
    // LED on
    set_led(pin_arr[i], true);
    _delay_ms(STARTUP_DELAY);

    // LED off
    set_led(pin_arr[i], false);
    _delay_ms(STARTUP_DELAY);
  }
}

// CONTINUE HERE

/// game_setup
void game_init(uint8_t game_length, uint8_t pattern_list[], uint8_t pb_arr[],
               uint8_t pb_lenght) {

  for (int l_item = 0; l_item < game_length; l_item++) {
    int idx = rand() % pb_lenght;
    pattern_list[l_item] = pb_arr[idx];
  }
}

// for current sequence
void sequence_light(uint8_t pattern_list[], uint8_t curr_turn) {
  for (int i = 0; i < curr_turn; i++) {
    // LED on
    set_led(pattern_list[i], true);
    _delay_ms(GAMEPLAY_DELAY);

    // LED off
    set_led(pattern_list[i], false);
    _delay_ms(GAMEPLAY_DELAY);
  }
}

/// pattern
bool pattern(uint8_t pattern_list[], uint8_t curr_turn, uint8_t pb_arr[],
             uint8_t pb_length) {
  uint8_t curr_guess;
  bool is_pressed = false;

  // PLAY CURRENT SEQUENCE
  sequence_light(pattern_list, curr_turn);

  // READ THE BUTTONS

  for (int idx = 0; idx < curr_turn; idx++) {

    scanf("%c", &curr_guess);

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }

    if (curr_guess != pattern_list[idx]) {
      return false;
    }

    for (uint8_t j = 0; j < pb_length; j++) {
      is_pressed = check_pressed(pb_arr[j]);

      set_led(pb_arr[i], is_pressed);

      if (pb_arr[i] != pattern_list[i]) {
        return false;
      }
    }
  }
  return true;
}

int main(void) {
  bool is_pressed = false;
  uint8_t pb_arr[4] = {PB0, PB1, PB2, PB3};
  uint8_t pb_length = sizeof(pb_arr) / sizeof(pb_arr[0]);

  // SET PINS TO OUTPUT
  for (int i = 0; i < pb_length; i++) {
    set_led(pb_arr[i], false);
  }

  _delay_ms(100);
  startup_sequence(pb_arr, pb_length);

  uint8_t game_length = 3;
  uint8_t curr_turn = 1;
  uint8_t pattern_list[game_length];

  // setup
  srand((unsigned)time(NULL));
  game_init(game_length, pattern_list, pb_arr, pb_length);

  // the loop
  while (curr_turn <= game_length) {
    bool is_pattern_correct = pattern(pattern_list, curr_turn);
    if (!is_pattern_correct) {
      printf("you lose\n");
      /// PLAY SAD ANIMATION
      return 0;
    } else {

      printf("next\n");
    }
    curr_turn++;
  }
  printf("you win\n");

  return 0;
}
