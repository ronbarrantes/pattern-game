
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
  LIGHT_END,
};

//// MAIN FUNCTION
int main(void) {
  Panel panel = {0};
  SequencePlayer sp = {0};
  MelodyPlayer mp = {0};

  timer_init();
  panel_init(&panel);

  sequence_start(&sp, &panel, test_sequence);
  melody_start(&mp, win_melody);

  while (true) {
    panel_update(&panel);
    melody_update(&mp);
    sequence_update(&sp, &panel);
  }

  return 0;

  //   while (1) {
  //     uint8_t game_length;
  //     uint8_t curr_turn = 1;
  //     uint8_t pattern_list[20];

  //     uint8_t led_length = sizeof(led_arr) / sizeof(led_arr[0]);
  //     uint8_t startup_pattern_length =
  //       sizeof(startup_pattern) / sizeof(startup_pattern[0]);
  //     uint8_t win_pattern_length = sizeof(win_pattern) /
  //     sizeof(win_pattern[0]); uint8_t lose_pattern_length =
  //       sizeof(lose_pattern) / sizeof(lose_pattern[0]);
  //     uint8_t win_melody_length = sizeof(win_melody) / sizeof(win_melody[0]);
  //     uint8_t lose_melody_length = sizeof(lose_melody) /
  //     sizeof(lose_melody[0]);

  //     uint16_t seed = 0;

  //     for (int i = 0; i < led_length; i++) {
  //       set_led(led_arr[i], false);
  //     }

  //     delay_ms(100);

  //     // Play startup sequence
  //     light_pattern(startup_pattern, startup_pattern_length);

  //     // Wait a bit before choosing level
  //     delay_ms(1000);

  //     // Choose difficulty and use the timing of that press as the random
  //     seed game_length = choose_level(led_arr, led_length, &seed);

  //     // No level was selected
  //     if (game_length == 0) {
  //       // later: enter sleep mode here
  //       return 0;
  //     }

  //     srand(seed);

  //     // Small pause before the game begins
  //     delay_ms(1000);

  //     game_init(game_length, pattern_list, led_arr, led_length);

  //     bool is_pattern_correct;

  //     for (int i = 0; i < led_length; i++) {
  //       set_led(led_arr[i], false);
  //     }

  //     while (curr_turn <= game_length) {
  //       is_pattern_correct =
  //         game_play(pattern_list, curr_turn, led_arr, led_length);

  //       if (!is_pattern_correct) {
  //         play_melody(lose_melody, lose_melody_length);
  //         light_pattern(lose_pattern, lose_pattern_length);

  //         break;
  //       }

  //       curr_turn++;

  //       if (curr_turn <= game_length) {
  //         delay_ms(1000);
  //       }
  //     }

  //     if (is_pattern_correct) {
  //       play_melody(win_melody, win_melody_length);
  //       light_pattern(win_pattern, win_pattern_length);
  //     }

  //     delay_ms(2000);
  //   }

  //   return 0;
}
