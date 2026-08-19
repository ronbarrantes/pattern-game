#include <avr/io.h>
#include <util/delay.h>

int main(void) {
  // PB0 is output
  DDRB |= (1 << PB0);

  while (1) {
    // LED ON
    PORTB |= (1 << PB0);
    _delay_ms(500);

    // LED OFF
    PORTB &= ~(1 << PB0);
    _delay_ms(500);
  }
  return 0;
}
