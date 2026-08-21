#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define STARTUP_DELAY 50

void set_led(uint8_t pin, bool state) {
  if (state) {
    PORTB &= ~(1 << pin);
    DDRB |= (1 << pin);
  } else {
    DDRB &= ~(1 << pin);
    PORTB |= (1 << pin);
  }
}

void startup_sequence(uint8_t pin_arr[], uint8_t pb_lenght) {
  for (int i = 0; i < pb_lenght; i++) {
    // LED on
    set_led(pin_arr[i], true);
    _delay_ms(STARTUP_DELAY);

    // LED off
    set_led(pin_arr[i], true);
    _delay_ms(STARTUP_DELAY);
  }

  _delay_ms(STARTUP_DELAY * 3);

  for (int i = pb_lenght - 1; i >= 0; i--) {
    // LED on
    set_led(pin_arr[i], true);
    _delay_ms(STARTUP_DELAY);

    // LED off
    set_led(pin_arr[i], true);
    _delay_ms(STARTUP_DELAY);
  }
}

bool check_pressed(uint8_t pin) {
  set_led(pin, false);
  _delay_ms(5);

  return !(PINB & (1 << pin));
}

// CONTINUE HERE

/// game_setup
void game_init(int *game_length, char *num_list) {
  int num_pool[4] = {1, 2, 3, 4};

  for (int l_item = 0; l_item < *game_length; l_item++) {
    int idx = rand() % 4;
    num_list[l_item] = num_pool[idx];
  }
}

/// pattern
bool pattern(int *curr_turn, char *num_list) {
  int curr_guess;

  printf("Nums: ");
  for (int idx = 0; idx < *curr_turn; idx++) {
    printf("%d ", num_list[idx]);
  }
  printf("\n\n");

  printf("gimme the numbers: ");
  for (int idx = 0; idx < *curr_turn; idx++) {
    scanf("%d", &curr_guess);

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }

    if (curr_guess != num_list[idx]) {
      return false;
    }

    printf("Num: %d\n", curr_guess);
  }
  return true;
}

int main(void) {

  int game_length = 3;
  int curr_turn = 1;
  char num_list[game_length];

  // setup
  srand((unsigned)time(NULL));
  game_init(&game_length, num_list);

  // the loop
  while (curr_turn <= game_length) {
    bool is_pattern_correct = pattern(&curr_turn, num_list);
    if (!is_pattern_correct) {
      printf("you lose\n");
      return 0;
    } else {
      printf("next\n");
    }
    curr_turn++;
  }
  printf("you win\n");

  return 0;
}
