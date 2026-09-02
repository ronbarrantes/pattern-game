#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define GAME_MAX_PATTERN_LENGTH 20

#define GAME_SPEED_SLOW_MS 500
#define GAME_SPEED_NORMAL_MS 400
#define GAME_SPEED_FAST_MS 300

typedef enum {
  GAME_PHASE_STARTUP = 0,
  GAME_PHASE_CHOOSE_LENGTH,
  GAME_PHASE_CHOOSE_SPEED,
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
  uint8_t guess_index;
  uint16_t seed;
  uint16_t step_duration_ms;
  GamePhase phase;
  uint32_t started_at;
} Game;

void game_init(Game *game);

#endif
