#include "game.h"
#include "sound.h"
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

  case GAME_PHASE_CHOOSE_LENGTH: {
    if (!game->phase_started) {
      panel_clear_press_events(panel);
      panel_set_led(panel, RED_LED, sound_is_enabled());
      panel_set_led(panel, YELLOW_LED, true);
      panel_set_led(panel, GREEN_LED, true);
      panel_set_led(panel, BLUE_LED, true);
      game->phase_started = true;
    }

    if (panel_take_press(panel, RED_LED)) {
      bool sound_enabled = sound_toggle();
      panel_set_led(panel, RED_LED, sound_enabled);
      return;
    }

    uint8_t selected_length = 0;

    if (panel_take_press(panel, YELLOW_LED)) {
      selected_length = 5;
    } else if (panel_take_press(panel, GREEN_LED)) {
      selected_length = 10;
    } else if (panel_take_press(panel, BLUE_LED)) {
      selected_length = 20;
    }

    if (selected_length == 0) {
      return;
    }

    game->game_length = selected_length;
    game->seed = (uint16_t)timer_now();

    panel_set_led(panel, RED_LED, false);
    panel_set_led(panel, YELLOW_LED, false);
    panel_set_led(panel, GREEN_LED, false);
    panel_set_led(panel, BLUE_LED, false);
    panel_clear_press_events(panel);

    game->phase = GAME_PHASE_CHOOSE_SPEED;
    game->phase_started = false;
    game->started_at = timer_now();
    return;
  }

  default:
    return;
  }
}
