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

void startup_sequence(uint8_t pin_arr[], uint8_t pb_length) {
  for (int i = 0; i < pb_length; i++) {
    // // LED ON
    // pin sinks current
    PORTB &= ~(1 << pin_arr[i]);
    DDRB |= (1 << pin_arr[i]);
    _delay_ms(STARTUP_DELAY * (i + 1));

    // // LED OFF
    // pin is an input with its pull-up enabled
    DDRB &= ~(1 << pin_arr[i]);
    PORTB |= (1 << pin_arr[i]);
    _delay_ms(STARTUP_DELAY * (i + 1));
  }

  for (int i = pb_length - 1; i >= 0; i--) {
    // // LED ON
    // pin sinks current
    PORTB &= ~(1 << pin_arr[i]);
    DDRB |= (1 << pin_arr[i]);
    _delay_ms(STARTUP_DELAY * (i + 1));

    // // LED OFF
    // pin is an input with its pull-up enabled
    DDRB &= ~(1 << pin_arr[i]);
    PORTB |= (1 << pin_arr[i]);
    _delay_ms(STARTUP_DELAY * (i + 1));
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
  bool is_pressed = false;
  uint8_t pb_arr[4] = {PB0, PB1, PB2, PB3};
  uint8_t pb_length = sizeof(pb_arr) / sizeof(pb_arr[0]);

  // SET EVERYTHING TO OUTPUT
  for (int i = 0; i < pb_length; i++) {
    DDRB |= (1 << pb_arr[i]);
  }

  // level 0 the lobby
  // level 1 the havital zone parking garage
  // level 2 is where thing start to get crippy
  // //  level fun :)
  // level 3 the abandoned hotel
  // level 4 the pipe room
  // // gold pipe room (secret)
  // level 94 modern
  // level 103 oi chips and vinegar
  // level 900 the end
  // level 999999999999 staairs
  // level reality??

  startup_sequence(pb_arr, pb_length);

  while (true) {
    for (uint8_t i = 0; i < pb_length; i++) {
      is_pressed = check_pressed(pb_arr[i]);
      set_led(pb_arr[i], is_pressed);
    }
  }
  return 0;
}
