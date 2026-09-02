#include <stdbool.h>

#include "game.h"
#include "panel.h"
#include "sound.h"
#include "timer.h"

int main(void) {
  Game game = {0};
  Panel panel = {0};
  SequencePlayer sequence_player = {0};
  MelodyPlayer melody_player = {0};

  timer_init();
  game_init(&game);
  panel_init(&panel);

  while (true) {
    panel_update(&panel);
    melody_update(&melody_player);
    sequence_update(&sequence_player, &panel);
    game_update(&game, &panel, &sequence_player);
  }

  return 0;
}
