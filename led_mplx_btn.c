#include <avr/io.h>
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

void check_pressed(uint8_t *is_pressed) {
  // check if button is pressed
  DDRB &= ~(1 << PB0);
  PORTB |= (1 << PB0);
  _delay_ms(10);

  *is_pressed = !(PINB & (1 << PB0));

  DDRB |= (1 << PB0);
  PORTB |= (1 << PB0);
  _delay_ms(10);

  if (is_pressed) {

    // // LED ON
    PORTB |= (1 << PB0);
  } else {
    // // LED OFF
    PORTB &= ~(1 << PB0);
  }
}

int main(void) {
  // PB0 is output
  DDRB |= (1 << PB0);
  DDRB &= ~(1 << PB1);
  PORTB |= (1 << PB1);

  startup_sequence();

  uint8_t is_pressed = 0;

  while (1) {
    // // LED ON
    // PORTB |= (1 << PB0);
    // _delay_ms(500);

    // // LED OFF
    // PORTB &= ~(1 << PB0);
    // _delay_ms(500);

    if (!(PINB & (1 << PB1))) {
      // // LED ON
      PORTB |= (1 << PB0);
    } else {
      // // LED OFF
      PORTB &= ~(1 << PB0);
    }
  }
  return 0;
}
