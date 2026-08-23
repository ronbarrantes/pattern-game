#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define F_CPU 1000000UL

/*
DDRB   → Should this pin be INPUT or OUTPUT?
PORTB  → Set output / enable input pull-up
PINB   → What is currently coming INTO the pin?

TCCR0A = behavior
TCCR0B = speed
OCR0A  = target
TCNT0  = counter
OC0A   = output signal

*/

void play_sound(uint16_t frequency, uint16_t duration_ms) {
  // PB0 / OC0A → output
  DDRB |= (1 << PB0);

  // CTC mode
  // Toggle OC0A (PB0) every time timer matches OCR0A
  TCCR0A = (1 << COM0A0) | (1 << WGM01);

  // Prescaler = 8
  TCCR0B = (1 << CS01);

  // Calculate how high Timer0 should count
  OCR0A = (F_CPU / (2UL * 8 * frequency)) - 1;

  // Timer hardware is now generating the tone on PB0.
  while (duration_ms > 0) {
    _delay_ms(1);
    duration_ms--;
  }

  // Stop Timer0
  TCCR0A = 0;
  TCCR0B = 0;

  // PB0 LOW
  PORTB &= ~(1 << PB0);
}

int main(void) {
  // PB0 is output
  DDRB |= (1 << PB0);
  //  DDRB &= ~(1 << PB1);
  PORTB |= (1 << PB0);

  uint16_t tones[] = {262, 294, 330, 350, 392, 440, 494, 523};
  uint16_t duration_ms[] = {10, 5, 5, 5, 5, 5, 5, 10};

  typedef struct {
    uint16_t frequency;
    uint16_t duration_ms;
  } Note;

  Note melody[] = {
      {262, 500}, {294, 250}, {330, 250}, {350, 250},
      {392, 250}, {440, 250}, {494, 250}, {523, 500},
  };

  uint8_t tone_length = sizeof(tones) / sizeof(tones[0]);

  while (1) {
    for (uint8_t i = 0; i < tone_length; i++) {
      play_sound(melody[i].frequency, melody[i].duration_ms);
    }

    for (uint8_t i = tone_length - 2; i >= 0; i--) {
      play_sound(melody[i].frequency, melody[i].duration_ms);
    }

    _delay_ms(3000);
  }

  return 0;
}
