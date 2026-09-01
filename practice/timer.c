#define F_CPU 1000000UL

#include <stdbool.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#define RED_LED PB2

volatile uint32_t system_ticks = 0;

void timer_init(void) {
  TCNT1 = 0;

  // CTC mode
  TCCR1 = (1 << CTC1);

  // 1 MHz / 8 = 125 kHz
  // 125 timer counts = 1 ms
  OCR1A = 124;
  OCR1C = 124;

  TIMSK |= (1 << OCIE1A);

  // Start Timer1, prescaler /8
  TCCR1 |= (1 << CS12);

  // Global interrupts on
  sei();
}

ISR(TIMER1_COMPA_vect) { system_ticks++; }

uint32_t timer_now(void) {
  uint32_t ticks;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { ticks = system_ticks; }

  return ticks;
}

// PANEL
void set_led(uint8_t pin, bool state) {
  if (state) {
    PORTB &= ~(1 << pin);
    DDRB |= (1 << pin);
  } else {
    DDRB &= ~(1 << pin);
    PORTB |= (1 << pin);
  }
}

//// MAIN FUNCTION
int main(void) {
  uint32_t last_tick = 0;
  bool led_state = false;

  timer_init();

  while (1) {
    uint32_t now = timer_now();

    if ((uint32_t)(now - last_tick) >= 500) {
      last_tick += 500;

      led_state = !led_state;
      set_led(RED_LED, led_state);
    }
  }
  return 0;
}
