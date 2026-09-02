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

void set_led(uint8_t pin, bool state);
bool check_pressed(uint8_t pin);

void sequence_start(SequencePlayer *player, const Light *sequence,
                    uint8_t sequence_length);

void sequence_update(SequencePlayer *player);
void sequence_stop(SequencePlayer *player);

#endif
