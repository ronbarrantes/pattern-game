#include <stdbool.h>
#include <stdio.h>

/** IO LOGIC **/

/** GAME LOGIC **/
// game starts
// start sequence (lights up the LEDs 3 TIMES)
//
// there is sequence that starts with 3 colors
// then 4 colors, then 5 colors and so on and so forth
//
// maybe I can have up to 32
// if player loses then it displays a "sad" sequece of colors
// if player wins the 32 different sequence then it diplays a "happy" sequence
//
//
//
//
//
//

/// game_setup
void game_setup(int *game_length, char *char_list) {
  for (int l_item = 0; l_item < *game_length; l_item++) {
    char_list[l_item] = l_item;
  }
}

/// pattern
bool pattern(int *curr_turn, char *char_list) {
  int curr_guess;

  printf("Nums: ");
  for (int itr = 0; itr < 5; itr++) {
    printf("%d ", char_list[itr]);
  }
  printf("\n\n");

  printf("gimme the numbers: ");
  for (int itr = 0; itr < *curr_turn; itr++) {
    scanf("%d", &curr_guess);

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }

    if (curr_guess != char_list[itr]) {
      return false;
    }

    printf("Num: %d\n", curr_guess);
  }

  return true;
}

/// game_loop

// bool game_loop(int *i) {
//   int game_length = 5;
//   char char_list[game_length];

//   for (int l_item = 0; l_item < game_length; l_item++) {
//     char_list[l_item] = l_item;
//   }

//   while (*i < game_length) {
//     printf("gimme the number: ");
//     scanf("%d", i);

//     int ch;
//     while ((ch = getchar()) != '\n' && ch != EOF) {
//     }

//     printf("\nthe num is %d\n", *i);
//   }

//   return true;
// }

int main(void) {
  int game_length = 5;
  int curr_turn = 0;
  char char_list[game_length];

  // setup
  game_setup(&game_length, char_list);
  pattern(&curr_turn, char_list);

  return 0;
}
