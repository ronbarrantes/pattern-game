#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "panel.h"
#include "timer.h"

/*
// panel.c
panel_init()
panel_update()
panel_set_led()
panel_all_off()
panel_get_pressed()
panel_wait_release()
panel_start_pattern()
panel_pattern_update()
*/

uint8_t led_arr[] = {RED_LED, YELLOW_LED, GREEN_LED, BLUE_LED};

Light startup_pattern[] = {
  {RED_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {BLUE_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {RED_LED, SHORT_DELAY},
};

Light lose_pattern[] = {
  {BLUE_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {RED_LED, MID_DELAY},
  {RED_LED, MID_DELAY},
  {RED_LED, MID_DELAY},
  {RED_LED, MID_DELAY},
};

Light win_pattern[] = {
  {RED_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {BLUE_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
};

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

void sequence_start(SequencePlayer *player, const Light *sequence,
                    uint8_t sequence_length) {
  player->sequence = sequence;
  player->sequence_length = sequence_length;
  player->curr_led = 0;
  player->started_at = timer_now();

  set_led(player->sequence->led, true);
}

// sequence update
void sequence_update(SequencePlayer *player) {
  if (player->sequence == NULL)
    return;

  if (player->curr_led >= player->sequence_length) {
    player->sequence = NULL;
    return;
  }

  Light light = player->sequence[player->curr_led];
  uint32_t now = timer_now();

  if ((uint32_t)(now - player->started_at) >= light.duration_ms) {
    set_led(light.led, false);
    player->curr_led++;
    player->started_at = now;

    if (player->curr_led < player->sequence_length) {
      Light next = player->sequence[player->curr_led];
      set_led(next.led, true);
    } else {
      player->sequence = NULL;
    }
  }
}

void sequence_stop(SequencePlayer *player) {
  if (player->sequence == NULL)
    return;

  Light light = player->sequence[player->curr_led];

  set_led(light.led, false);
  player->sequence = NULL;
}

// LED and button share a pin. Turning the LED off makes the pin an input and
// enables its pull-up resistor, so a pressed button reads low.
bool check_pressed(uint8_t pin) {
  set_led(pin, false);
  return !(PINB & (1 << pin));
}
