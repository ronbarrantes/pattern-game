#ifndef PANEL_H
#define PANEL_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define RED_LED PB2
#define BLUE_LED PB1
#define YELLOW_LED PB3
#define GREEN_LED PB4

#define SHORT_DELAY 50
#define MID_DELAY 300
#define LONG_DELAY 500

#define LIGHT_END {0, 0}

typedef enum {
  PANEL_PHASE_DISPLAY = 0,
  PANEL_PHASE_SCAN = 1,
} PanelPhase;

typedef struct {
  uint8_t led_mask;
  uint8_t pressed_mask;
  uint8_t sampled_mask;
  uint8_t press_event_mask;
  PanelPhase phase;
  uint32_t started_at;
  uint32_t debounce_started_at;
} Panel;

typedef struct {
  uint8_t led;
  uint16_t duration_ms;
} Light;

typedef struct {
  const Light *sequence;
  uint8_t curr_led;
  bool light_on;
  uint32_t started_at;
} SequencePlayer;

extern const Light startup_pattern[];
extern const Light lose_pattern[];
extern const Light win_pattern[];

void panel_init(Panel *panel);
void panel_update(Panel *panel);
void panel_set_led(Panel *panel, uint8_t led, bool is_on);
void panel_all_off(Panel *panel);
bool panel_is_pressed(const Panel *panel, uint8_t button);
bool panel_take_press(Panel *panel, uint8_t button);
void panel_clear_press_events(Panel *panel);

void sequence_start(SequencePlayer *player, Panel *panel,
                    const Light *sequence);

void sequence_update(SequencePlayer *player, Panel *panel);
void sequence_stop(SequencePlayer *player, Panel *panel);
bool sequence_is_playing(const SequencePlayer *player);

#endif
