#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "panel.h"
#include "timer.h"

#define PANEL_DISPLAY_MS 5
#define PANEL_SCAN_SETTLE_MS 1

static void set_led_pin(uint8_t pin, bool state);

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

void panel_init(Panel *panel) {
  panel->led_mask = 0;
  panel->pressed_mask = 0;
  panel->phase = PANEL_PHASE_DISPLAY;
  panel->started_at = timer_now();

  for (uint8_t i = 0; i < sizeof(led_arr) / sizeof(led_arr[0]); i++) {
    set_led_pin(led_arr[i], false);
  }
}

void panel_update(Panel *panel) {
  uint32_t now = timer_now();
  uint32_t elapsed = now - panel->started_at;

  if (panel->phase == PANEL_PHASE_DISPLAY) {
    for (uint8_t i = 0; i < sizeof(led_arr) / sizeof(led_arr[0]); i++) {
      uint8_t led = led_arr[i];
      bool is_on = (panel->led_mask & (1U << led)) != 0;
      set_led_pin(led, is_on);
    }

    if (elapsed < PANEL_DISPLAY_MS) {
      return;
    }

    for (uint8_t i = 0; i < sizeof(led_arr) / sizeof(led_arr[0]); i++) {
      set_led_pin(led_arr[i], false);
    }

    panel->phase = PANEL_PHASE_SCAN;
    panel->started_at = now;
    return;
  }

  if (elapsed < PANEL_SCAN_SETTLE_MS) {
    return;
  }

  uint8_t pins = PINB;
  panel->pressed_mask = 0;

  for (uint8_t i = 0; i < sizeof(led_arr) / sizeof(led_arr[0]); i++) {
    uint8_t led = led_arr[i];
    uint8_t mask = (uint8_t)(1U << led);

    if (!(pins & mask)) {
      panel->pressed_mask |= mask;
    }
  }

  for (uint8_t i = 0; i < sizeof(led_arr) / sizeof(led_arr[0]); i++) {
    uint8_t led = led_arr[i];
    uint8_t mask = (uint8_t)(1U << led);
    set_led_pin(led, (panel->led_mask & mask) != 0);
  }

  panel->phase = PANEL_PHASE_DISPLAY;
  panel->started_at = now;
}

void panel_set_led(Panel *panel, uint8_t led, bool is_on) {
  uint8_t mask = (uint8_t)(1U << led);

  if (is_on) {
    panel->led_mask |= mask;
  } else {
    panel->led_mask &= (uint8_t)~mask;
  }
}

bool panel_is_pressed(const Panel *panel, uint8_t button) {
  uint8_t mask = (uint8_t)(1U << button);
  return (panel->pressed_mask & mask) != 0;
}

// PANEL
static void set_led_pin(uint8_t pin, bool state) {
  if (state) {
    PORTB &= ~(1 << pin);
    DDRB |= (1 << pin);
  } else {
    DDRB &= ~(1 << pin);
    PORTB |= (1 << pin);
  }
}

void sequence_start(SequencePlayer *player, Panel *panel,
                    const Light *sequence, uint8_t sequence_length) {
  player->sequence = sequence;
  player->sequence_length = sequence_length;
  player->curr_led = 0;
  player->started_at = timer_now();

  panel_set_led(panel, player->sequence->led, true);
}

// sequence update
void sequence_update(SequencePlayer *player, Panel *panel) {
  if (player->sequence == NULL)
    return;

  if (player->curr_led >= player->sequence_length) {
    player->sequence = NULL;
    return;
  }

  Light light = player->sequence[player->curr_led];
  uint32_t now = timer_now();

  if ((uint32_t)(now - player->started_at) >= light.duration_ms) {
    panel_set_led(panel, light.led, false);
    player->curr_led++;
    player->started_at = now;

    if (player->curr_led < player->sequence_length) {
      Light next = player->sequence[player->curr_led];
      panel_set_led(panel, next.led, true);
    } else {
      player->sequence = NULL;
    }
  }
}

void sequence_stop(SequencePlayer *player, Panel *panel) {
  if (player->sequence == NULL)
    return;

  Light light = player->sequence[player->curr_led];

  panel_set_led(panel, light.led, false);
  player->sequence = NULL;
}
