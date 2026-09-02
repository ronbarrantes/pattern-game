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

typedef enum {
  PANEL_PHASE_DISPLAY = 0,
  PANEL_PHASE_SCAN = 1,
} PanelPhase;

typedef struct {
  uint8_t led_mask;
  uint8_t pressed_mask;
  PanelPhase phase;
  uint32_t started_at;
} Panel;

// Panel
typedef struct {
  uint8_t led;
  uint16_t duration_ms;
} Light;

// Panel
typedef struct {
  const Light *sequence;
  uint8_t curr_led;
  uint8_t sequence_length;
  uint32_t started_at;
} SequencePlayer;

extern uint8_t led_arr[];
extern Light startup_pattern[];
extern Light lose_pattern[];
extern Light win_pattern[];

void panel_init(Panel *panel);
void panel_update(Panel *panel);
void panel_set_led(Panel *panel, uint8_t led, bool is_on);
bool panel_is_pressed(const Panel *panel, uint8_t button);

void sequence_start(SequencePlayer *player, Panel *panel,
                    const Light *sequence, uint8_t sequence_length);

void sequence_update(SequencePlayer *player, Panel *panel);
void sequence_stop(SequencePlayer *player, Panel *panel);

#endif
