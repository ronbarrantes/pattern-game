#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

#define STARTUP_DELAY 100

/*
DDRB   → Should this pin be INPUT or OUTPUT?
PORTB  → Set output / enable input pull-up
PINB   → What is currently coming INTO the pin?
*/

void startup_sequence(void) {
  for (int i = 0; i < 3; i++) {
    // // LED ON
    // PB0 sinks current
    PORTB &= ~(1 << PB0);
    DDRB |= (1 << PB0);
    _delay_ms(STARTUP_DELAY);

    // // LED OFF
    // PB0 is an input with its pull-up enabled
    DDRB &= ~(1 << PB0);
    PORTB |= (1 << PB0);
    _delay_ms(STARTUP_DELAY);
  }
}

bool check_pressed(uint8_t pin) {
  DDRB &= ~(1 << pin); // input
  PORTB |= (1 << pin); // pull-up
  _delay_ms(5);

  return !(PINB & (1 << pin));
}

void set_led(uint8_t pin, bool state) {
  if (state) {
    PORTB &= ~(1 << pin);
    DDRB |= (1 << pin); // output
  } else {
    DDRB &= ~(1 << pin);
    PORTB |= (1 << pin);
  }
}

int main(void) {
  // PB0 is output

  DDRB |= (1 << PB0);

  bool is_pressed = false;
  // uint8_t pb_arr[4] = {PB0}; // will have PB1, PB2, PB3
  startup_sequence();

  while (true) {
    //    for (uint8_t i = 0; i < 4; i++) {
    is_pressed = check_pressed(PB0);
    set_led(PB0, is_pressed);
    //   }
  }
  return 0;
}
