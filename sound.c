#define F_CPU 1000000UL
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "sound.h"
#include "timer.h"

uint16_t button_tones[] = {
  0,
  BLUE_TONE,
  RED_TONE,
  YELLOW_TONE,
  GREEN_TONE,
};

Note startup_melody[] = {
  {RED_TONE, 100},
  {YELLOW_TONE, 100},
  {GREEN_TONE, 100},
  {BLUE_TONE, 100},
  {GREEN_TONE, 100},
  {YELLOW_TONE, 100},
  {RED_TONE, 100},
  MELODY_END,
};

Note lose_melody[] = {
  {BLUE_TONE, 150},
  {GREEN_TONE, 150},
  {YELLOW_TONE, 150},
  {RED_TONE, 400},
  MELODY_END,
};

Note win_melody[] = {
  {RED_TONE, 100},
  {YELLOW_TONE, 100},
  {GREEN_TONE, 100},
  {BLUE_TONE, 300},
  MELODY_END,
};

static bool buzzer_enabled = true;

void sound_start(uint16_t frequency) {
  if (!buzzer_enabled) {
    return;
  }

  DDRB |= (1 << PB0);
  TCCR0A = (1 << COM0A0) | (1 << WGM01);
  TCCR0B = (1 << CS01);
  OCR0A = (F_CPU / (2UL * 8 * frequency)) - 1;
}

void sound_stop(void) {
  TCCR0A = 0;
  TCCR0B = 0;

  PORTB &= ~(1 << PB0);
}

bool sound_toggle(void) {
  buzzer_enabled = !buzzer_enabled;

  if (!buzzer_enabled) {
    sound_stop();
  }

  return buzzer_enabled;
}

bool sound_is_enabled(void) { return buzzer_enabled; }

void melody_start(MelodyPlayer *player, const Note *melody) {
  player->melody = melody;
  player->curr_note = 0;
  player->started_at = timer_now();

  Note note = player->melody[0];

  if (note.frequency == 0 && note.duration_ms == 0) {
    player->melody = NULL;
    return;
  }

  sound_start(note.frequency);
}

void melody_update(MelodyPlayer *player) {
  if (player->melody == NULL)
    return;

  Note note = player->melody[player->curr_note];
  uint32_t now = timer_now();

  if ((uint32_t)(now - player->started_at) >= note.duration_ms) {
    sound_stop();
    player->curr_note++;

    Note next = player->melody[player->curr_note];

    if (next.frequency == 0 && next.duration_ms == 0) {
      player->melody = NULL;
      return;
    }

    player->started_at = now;
    sound_start(next.frequency);
  }
}

bool melody_is_playing(const MelodyPlayer *player) {
  return player->melody != NULL;
}
