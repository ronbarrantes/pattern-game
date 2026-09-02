
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>

#include "panel.h"
#include "sound.h"
#include "timer.h"

// Panel
// I need to make a clock using Timer1

/*
rough plan

// main.c
main()

// utils.c
delay_ms()
*/

// I want something like this in the end

/*

int main(void) {
    Game game;
    Panel panel;
    MelodyPlayer player;

    game_init(&game);
    panel_init(&panel);
    melody_init(&player);

    while (1) {
        panel_update(&panel);
        melody_update(&player);
        game_update(&game, &panel, &player);
    }
}

*/

Light test_sequence[] = {
  {RED_LED, 500},
  {YELLOW_LED, 500},
  {GREEN_LED, 500},
  {BLUE_LED, 500},
};

//// MAIN FUNCTION
int main(void) {
  SequencePlayer sp = {0};
  MelodyPlayer mp = {0};

  timer_init();

  sequence_start(
    &sp, test_sequence, sizeof(test_sequence) / sizeof(test_sequence[0]));
  melody_start(&mp, win_melody);

  while (true) {
    melody_update(&mp);
    sequence_update(&sp);
  }

  return 0;
}
