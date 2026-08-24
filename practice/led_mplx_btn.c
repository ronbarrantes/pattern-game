#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

#define STARTUP_DELAY 50

/*
DDRB   → Should this pin be INPUT or OUTPUT?
PORTB  → Set output / enable input pull-up
PINB   → What is currently coming INTO the pin?
*/

void set_led(uint8_t pin, bool state) {
  if (state) {
    PORTB &= ~(1 << pin);
    DDRB |= (1 << pin); // output
  } else {
    DDRB &= ~(1 << pin);
    PORTB |= (1 << pin);
  }
}

void startup_sequence(uint8_t pin_arr[], uint8_t pb_length) {
  for (int i = 0; i < pb_length; i++) {
    // // LED ON
    // pin sinks current
    set_led(pin_arr[i], true);
    _delay_ms(STARTUP_DELAY);

    // // LED OFF
    // pin is an input with its pull-up enabled
    set_led(pin_arr[i], false);
    _delay_ms(STARTUP_DELAY);
  }

  for (int i = pb_length; i >= 0; i--) {
    // // LED ON
    // pin sinks current
    set_led(pin_arr[i], true);
    _delay_ms(STARTUP_DELAY);

    // // LED OFF
    // pin is an input with its pull-up enabled
    set_led(pin_arr[i], false);
    _delay_ms(STARTUP_DELAY);
  }
}

bool check_pressed(uint8_t pin) {
  set_led(pin, false);
  _delay_ms(5);

  return !(PINB & (1 << pin));
}

int main(void) {
  bool is_pressed = false;
  uint8_t pb_arr[4] = {PB0, PB1, PB2, PB3};
  uint8_t pb_length = sizeof(pb_arr) / sizeof(pb_arr[0]);

  // SET EVERYTHING TO OUTPUT
  for (int i = 0; i < pb_length; i++) {
    set_led(pb_arr[i], false);
  }

  _delay_ms(100);
  startup_sequence(pb_arr, pb_length);

  while (true) {
    for (uint8_t i = 0; i < pb_length; i++) {
      is_pressed = check_pressed(pb_arr[i]);
      set_led(pb_arr[i], is_pressed);
    }
  }
  return 0;
}
