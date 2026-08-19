#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

/*
DDRB   → Should this pin be INPUT or OUTPUT?
PORTB  → Set output / enable input pull-up
PINB   → What is currently coming INTO the pin?
*/

void startup_sequence(void) {
  int delay_in_ms = 100;
  for (int i = 0; i < 3; i++) {
    // // LED ON
    PORTB |= (1 << PB0);
    _delay_ms(delay_in_ms);

    // // LED OFF
    PORTB &= ~(1 << PB0);
    _delay_ms(delay_in_ms);
  }
}

bool check_pressed(uint8_t pin) {
  // check if button is pressed
  DDRB &= ~(1 << pin);
  PORTB |= (1 << pin);
  _delay_ms(10);

  return !(PINB & (1 << pin));
}

void set_led(uint8_t pin, bool is_pressed) {
  DDRB |= (1 << pin);
  if (is_pressed) {
    PORTB |= (1 << pin);
  } else {
    PORTB &= ~(1 << pin);
  }
}

int main(void) {
  // PB0 is output
  DDRB |= (1 << PB0);
  DDRB &= ~(1 << PB1);
  PORTB |= (1 << PB1);
  bool is_pressed = 0;
  uint8_t pb_arr[4] = {PB0, PB1, PB2, PB3};

  startup_sequence();

  while (1) {
    for (uint8_t i = 0; i < 4; i++) {
      is_pressed = check_pressed(pb_arr[i]);
      set_led(pb_arr[i], is_pressed);
    }
  }
  return 0;
}
