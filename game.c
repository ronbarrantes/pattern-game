#include <stddef.h>

#include "game.h"
#include "sound.h"
#include "timer.h"

typedef struct {
  uint8_t button;
  uint8_t length;
  uint16_t speed_ms;
} GameOption;

static const GameOption game_options[] = {
  {YELLOW_LED, 5, 500},
  {GREEN_LED, 10, 400},
  {BLUE_LED, GAME_MAX_PATTERN_LENGTH, 300},
};

static void enter_phase(Game *game, GamePhase phase);
static void update_startup(Game *game, Panel *panel,
                           SequencePlayer *sequence_player);
static void update_select(Game *game, Panel *panel);

void game_init(Game *game) {
  *game = (Game){
    .curr_turn = 1,
    .phase = GAME_PHASE_STARTUP,
    .started_at = timer_now(),
  };
}

void game_update(Game *game, Panel *panel, SequencePlayer *sequence_player) {
  switch (game->phase) {
  case GAME_PHASE_STARTUP:
    update_startup(game, panel, sequence_player);
    return;

  case GAME_PHASE_SELECT:
    update_select(game, panel);
    return;

  default:
    return;
  }
}

static void enter_phase(Game *game, GamePhase phase) {
  game->phase = phase;
  game->phase_started = false;
  game->started_at = timer_now();
}

static void update_startup(Game *game, Panel *panel,
                           SequencePlayer *sequence_player) {
  if (!game->phase_started) {
    sequence_start(sequence_player, panel, startup_pattern);
    game->phase_started = true;
    return;
  }

  if (sequence_is_playing(sequence_player)) {
    return;
  }

  enter_phase(game, GAME_PHASE_SELECT);
}

static void update_select(Game *game, Panel *panel) {
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

  const GameOption *selected_option = NULL;

  for (uint8_t i = 0; i < sizeof(game_options) / sizeof(game_options[0]); i++) {
    if (panel_take_press(panel, game_options[i].button)) {
      selected_option = &game_options[i];
      break;
    }
  }

  if (selected_option == NULL) {
    return;
  }

  game->game_length = selected_option->length;
  game->step_duration_ms = selected_option->speed_ms;
  game->seed = (uint16_t)timer_now();

  panel_all_off(panel);
  panel_clear_press_events(panel);

  enter_phase(game, GAME_PHASE_SHOW_PATTERN);
}
