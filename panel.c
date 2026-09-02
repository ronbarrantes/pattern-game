#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "panel.h"
#include "timer.h"

#define PANEL_DISPLAY_MS 5
#define PANEL_SCAN_SETTLE_MS 1
#define PANEL_DEBOUNCE_MS 15
#define PANEL_PIN_MASK                                                         \
  ((1U << RED_LED) | (1U << YELLOW_LED) | (1U << GREEN_LED) | (1U << BLUE_LED))

static void set_led_pin(uint8_t pin, bool state);
static void update_pressed_mask(Panel *panel, uint8_t sampled_mask,
                                uint32_t now);

static const uint8_t led_arr[] = {RED_LED, YELLOW_LED, GREEN_LED, BLUE_LED};

const Light startup_pattern[] = {
  {RED_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {BLUE_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {RED_LED, SHORT_DELAY},
  LIGHT_END,
};

const Light lose_pattern[] = {
  {BLUE_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {RED_LED, MID_DELAY},
  {RED_LED, MID_DELAY},
  {RED_LED, MID_DELAY},
  {RED_LED, MID_DELAY},
  LIGHT_END,
};

const Light win_pattern[] = {
  {RED_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {BLUE_LED, SHORT_DELAY},
  {GREEN_LED, SHORT_DELAY},
  {YELLOW_LED, SHORT_DELAY},
  LIGHT_END,
};

void panel_init(Panel *panel) {
  panel->led_mask = 0;
  panel->pressed_mask = 0;
  panel->sampled_mask = 0;
  panel->press_event_mask = 0;
  panel->phase = PANEL_PHASE_DISPLAY;
  panel->started_at = timer_now();
  panel->debounce_started_at = panel->started_at;

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
  uint8_t sampled_mask = (uint8_t)~pins & (uint8_t)PANEL_PIN_MASK;
  update_pressed_mask(panel, sampled_mask, now);

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

bool panel_take_press(Panel *panel, uint8_t button) {
  uint8_t mask = (uint8_t)(1U << button);

  if ((panel->press_event_mask & mask) == 0) {
    return false;
  }

  panel->press_event_mask &= (uint8_t)~mask;
  return true;
}

static void update_pressed_mask(Panel *panel, uint8_t sampled_mask,
                                uint32_t now) {
  if (sampled_mask != panel->sampled_mask) {
    panel->sampled_mask = sampled_mask;
    panel->debounce_started_at = now;
    return;
  }

  if ((uint32_t)(now - panel->debounce_started_at) < PANEL_DEBOUNCE_MS) {
    return;
  }

  if (sampled_mask == panel->pressed_mask) {
    return;
  }

  uint8_t new_presses = sampled_mask & (uint8_t)~panel->pressed_mask;
  panel->pressed_mask = sampled_mask;
  panel->press_event_mask |= new_presses;
}

static void set_led_pin(uint8_t pin, bool state) {
  uint8_t mask = (uint8_t)(1U << pin);

  if (state) {
    PORTB &= (uint8_t)~mask;
    DDRB |= mask;
  } else {
    DDRB &= (uint8_t)~mask;
    PORTB |= mask;
  }
}

void sequence_start(SequencePlayer *player, Panel *panel,
                    const Light *sequence) {
  sequence_stop(player, panel);

  if (sequence == NULL ||
      (sequence[0].led == 0 && sequence[0].duration_ms == 0)) {
    player->curr_led = 0;
    return;
  }

  player->sequence = sequence;
  player->curr_led = 0;
  player->light_on = true;
  player->started_at = timer_now();

  panel_set_led(panel, player->sequence->led, true);
}

void sequence_update(SequencePlayer *player, Panel *panel) {
  if (player->sequence == NULL)
    return;

  Light light = player->sequence[player->curr_led];
  uint32_t now = timer_now();

  if ((uint32_t)(now - player->started_at) >= light.duration_ms) {
    player->started_at = now;

    if (player->light_on) {
      panel_set_led(panel, light.led, false);
      player->light_on = false;
      return;
    }

    player->curr_led++;
    Light next = player->sequence[player->curr_led];

    if (next.led == 0 && next.duration_ms == 0) {
      player->sequence = NULL;
      return;
    }

    panel_set_led(panel, next.led, true);
    player->light_on = true;
  }
}

void sequence_stop(SequencePlayer *player, Panel *panel) {
  if (player->sequence == NULL)
    return;

  if (player->light_on) {
    Light light = player->sequence[player->curr_led];
    panel_set_led(panel, light.led, false);
  }

  player->sequence = NULL;
  player->light_on = false;
}

bool sequence_is_playing(const SequencePlayer *player) {
  return player->sequence != NULL;
}
