#include <stddef.h>
#include <stdlib.h>

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

static const uint8_t game_leds[] = {
  RED_LED,
  YELLOW_LED,
  GREEN_LED,
  BLUE_LED,
};

enum { GAME_LED_COUNT = sizeof(game_leds) / sizeof(game_leds[0]) };

static void enter_phase(Game *game, GamePhase phase);
static void generate_pattern(Game *game);
static void update_startup(Game *game, Panel *panel,
                           SequencePlayer *sequence_player);
static void update_select(Game *game, Panel *panel);
static void update_show_pattern(Game *game, Panel *panel);
static void update_player_turn(Game *game, Panel *panel);

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

  case GAME_PHASE_SHOW_PATTERN:
    update_show_pattern(game, panel);
    return;

  case GAME_PHASE_PLAYER_TURN:
    update_player_turn(game, panel);
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

static void generate_pattern(Game *game) {
  srand(game->seed);

  for (uint8_t i = 0; i < game->game_length; i++) {
    uint8_t led_index = (uint8_t)(rand() % GAME_LED_COUNT);
    game->pattern[i] = game_leds[led_index];
  }
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
  generate_pattern(game);

  panel_all_off(panel);
  panel_clear_press_events(panel);

  enter_phase(game, GAME_PHASE_SHOW_PATTERN);
}

static void update_show_pattern(Game *game, Panel *panel) {
  if (!game->phase_started) {
    game->playback_index = 0;
    game->light_on = true;
    game->started_at = timer_now();

    uint8_t led = game->pattern[game->playback_index];
    panel_set_led(panel, led, true);
    sound_start(button_tones[led]);
    game->phase_started = true;
    return;
  }

  uint32_t now = timer_now();

  if ((uint32_t)(now - game->started_at) < game->step_duration_ms) {
    return;
  }

  game->started_at = now;
  uint8_t led = game->pattern[game->playback_index];

  if (game->light_on) {
    panel_set_led(panel, led, false);
    sound_stop();
    game->light_on = false;
    return;
  }

  game->playback_index++;

  if (game->playback_index >= game->curr_turn) {
    panel_clear_press_events(panel);
    game->guess_index = 0;
    game->light_on = false;
    enter_phase(game, GAME_PHASE_PLAYER_TURN);
    return;
  }

  led = game->pattern[game->playback_index];
  panel_set_led(panel, led, true);
  sound_start(button_tones[led]);
  game->light_on = true;
}

static void update_player_turn(Game *game, Panel *panel) {
  if (!game->phase_started) {
    game->guess_index = 0;
    game->active_button = 0;
    panel_clear_press_events(panel);
    game->phase_started = true;
  }

  if (game->active_button != 0) {
    if (panel_is_pressed(panel, game->active_button)) {
      return;
    }

    uint8_t guessed_button = game->active_button;

    panel_set_led(panel, guessed_button, false);
    panel_clear_press_events(panel);
    sound_stop();
    game->active_button = 0;

    if (guessed_button != game->pattern[game->guess_index]) {
      enter_phase(game, GAME_PHASE_LOSE);
      return;
    }

    game->guess_index++;

    if (game->guess_index < game->curr_turn) {
      return;
    }

    game->curr_turn++;

    if (game->curr_turn > game->game_length) {
      enter_phase(game, GAME_PHASE_WIN);
      return;
    }

    enter_phase(game, GAME_PHASE_SHOW_PATTERN);
    return;
  }

  for (uint8_t i = 0; i < GAME_LED_COUNT; i++) {
    uint8_t button = game_leds[i];

    if (!panel_take_press(panel, button)) {
      continue;
    }

    panel_clear_press_events(panel);
    game->active_button = button;
    panel_set_led(panel, button, true);
    sound_start(button_tones[button]);
    return;
  }
}
