#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>

#include "panel.h"
#include "sound.h"

#define GAME_MAX_PATTERN_LENGTH 20

typedef enum {
  GAME_PHASE_STARTUP = 0,
  GAME_PHASE_SELECT,
  GAME_PHASE_CONFIRM_LEVEL,
  GAME_PHASE_ROUND_PAUSE,
  GAME_PHASE_SHOW_PATTERN,
  GAME_PHASE_PLAYER_TURN,
  GAME_PHASE_WIN,
  GAME_PHASE_LOSE,
  GAME_PHASE_RESTART,
} GamePhase;

typedef struct {
  uint8_t pattern[GAME_MAX_PATTERN_LENGTH];
  uint8_t game_length;
  uint8_t curr_turn;
  uint8_t playback_index;
  uint8_t guess_index;
  uint8_t active_button;
  uint8_t flash_count;
  uint16_t seed;
  uint16_t step_duration_ms;
  GamePhase phase;
  bool phase_started;
  bool light_on;
  uint32_t started_at;
} Game;

void game_init(Game *game);
void game_update(Game *game, Panel *panel, SequencePlayer *sequence_player,
                 MelodyPlayer *melody_player);

#endif
