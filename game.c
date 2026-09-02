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
  game->phase_started = false;
  game->started_at = timer_now();
}

void game_update(Game *game, Panel *panel, SequencePlayer *sequence_player) {
  switch (game->phase) {
  case GAME_PHASE_STARTUP:
    if (!game->phase_started) {
      sequence_start(sequence_player, panel, startup_pattern);
      game->phase_started = true;
      return;
    }

    if (sequence_is_playing(sequence_player)) {
      return;
    }

    game->phase = GAME_PHASE_CHOOSE_LENGTH;
    game->phase_started = false;
    game->started_at = timer_now();
    return;

  default:
    return;
  }
}
