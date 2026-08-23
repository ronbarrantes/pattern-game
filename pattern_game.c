#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <avr/io.h>
#include <stdbool.h>
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

LightPattern win_pattern[] {
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

bool check_pressed(uint8_t pin) {
  set_led(pin, false);
  _delay_ms(5);

  return !(PINB & (1 << pin));
}

void startup_sequence(uint8_t pin_arr[], uint8_t pb_lenght) {
  for (int i = 0; i < pb_lenght; i++) {
    // LED on
    set_led(pin_arr[i], true);
    _delay_ms(SHORT_DELAY);

    // LED off
    set_led(pin_arr[i], false);
    _delay_ms(SHORT_DELAY);
  }

  // a tiny little break
  _delay_ms(SHORT_DELAY * 3);

  for (int i = pb_lenght - 1; i >= 0; i--) {
    // LED on
    set_led(pin_arr[i], true);
    _delay_ms(SHORT_DELAY);

    // LED off
    set_led(pin_arr[i], false);
    _delay_ms(SHORT_DELAY);
  }
}

void light_pattern(LightPattern pattern, uint8_t pattern_length) {
  for (int i = 0 ; i < pattern_length; i++) {
    // LED on
    set_led(pattern[i].led, true);
    _delay_ms(pattern[i].duration);

    // LED off
    set_led(pattern[i].led, false);
    _delay_ms(pattern[i].duration);
  }
}

// for current sequence
void sequence_light(uint8_t pattern_list[], uint8_t curr_turn) {
  for (int i = 0; i < curr_turn; i++) {
    // LED on
    set_led(pattern_list[i], true);
    _delay_ms(LONG_DELAY);

    // LED off
    set_led(pattern_list[i], false);
    _delay_ms(LONG_DELAY);
  }
}

/// game_setup
void game_init(uint8_t game_length, uint8_t pattern_list[], uint8_t pb_arr[],
               uint8_t pb_lenght) {

  for (int l_item = 0; l_item < game_length; l_item++) {
    int idx = rand() % pb_lenght;
    pattern_list[l_item] = pb_arr[idx];
  }
}

bool pattern(uint8_t pattern_list[], uint8_t curr_turn, uint8_t pb_arr[],
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
  }

  return true;
}

int main(void) {
  bool is_pressed = false;
  uint8_t pb_length = sizeof(pb_arr) / sizeof(pb_arr[0]);

  // SET PINS TO OUTPUT
  for (int i = 0; i < pb_length; i++) {
    set_led(pb_arr[i], false);
  }

  _delay_ms(100);
  light_pattern(startup_pattern);
  

  uint8_t game_length = 3;
  uint8_t curr_turn = 1;
  uint8_t pattern_list[game_length];

  // setup
  srand((unsigned)time(NULL));
  game_init(game_length, pattern_list, pb_arr, pb_length);

  // the loop
  while (curr_turn <= game_length) {
    bool is_pattern_correct =
        pattern(pattern_list, curr_turn, pb_arr, pb_length);
    if (!is_pattern_correct) {
      printf("you lose\n");
      light_pattern(lose_pattern);
      return 0;
    } else {

      printf("next\n");
    }
    curr_turn++;
  }
  light_pattern(win_pattern);
  return 0;
}
