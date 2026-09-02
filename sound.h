#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>
#include <stdint.h>

#define RED_TONE 262
#define YELLOW_TONE 330
#define GREEN_TONE 392
#define BLUE_TONE 523

#define MELODY_END {0, 0}

typedef struct {
  uint16_t frequency;
  uint16_t duration_ms;
} Note;

typedef struct {
  const Note *melody;
  uint8_t curr_note;
  uint32_t started_at;
} MelodyPlayer;

extern uint16_t button_tones[];
extern Note lose_melody[];
extern Note win_melody[];

void sound_start(uint16_t frequency);
void sound_stop(void);
bool sound_toggle(void);
bool sound_is_enabled(void);

void melody_start(MelodyPlayer *player, const Note *melody);
void melody_update(MelodyPlayer *player);

#endif
