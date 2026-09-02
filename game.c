#include "game.h"
#include "timer.h"

void game_init(Game *game) {
  for (uint8_t i = 0; i < GAME_MAX_PATTERN_LENGTH; i++) {
    game->pattern[i] = 0;
  }

  game->game_length = 0;
  game->curr_turn = 1;
  game->guess_index = 0;
  game->seed = 0;
  game->step_duration_ms = GAME_SPEED_NORMAL_MS;
  game->phase = GAME_PHASE_STARTUP;
  game->started_at = timer_now();
}
