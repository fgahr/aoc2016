#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/lib.h"

#define LIGHT_OFF 0
#define LIGHT_ON 1

/* NOTE: Convention for this file:
   x represents horizontal (0 to width)
   y represents vertical (0 to height) */
#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 6

typedef struct {
  int lights[SCREEN_WIDTH * SCREEN_HEIGHT];
} screen;

/* Turn of all lights on the screen, i.e. initialize to dark. */
void turn_off_all(screen *s) {
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    s->lights[i] = LIGHT_OFF;
  }
}

/* The state of the light at x, y on the screen s. */
int light_at(screen *s, int x, int y) {
  return s->lights[x + y * SCREEN_WIDTH];
}

/* Set the light at x, y on screen to the given state. */
void set_light(screen *s, int x, int y, int light_state) {
  s->lights[x + y * SCREEN_WIDTH] = light_state;
}

/* Shift the y-th row on screen s by shift amount. */
void shift_row(screen *s, int y, int shift) {
  int row_shifted[SCREEN_WIDTH];
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    row_shifted[(x + shift) % SCREEN_WIDTH] = light_at(s, x, y);
  }

  for (int x = 0; x < SCREEN_WIDTH; x++) {
    set_light(s, x, y, row_shifted[x]);
  }
}

/* Shift the x-th column on screen s by shift amount. */
void shift_column(screen *s, int x, int shift) {
  int col_shifted[SCREEN_HEIGHT];
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    col_shifted[(y + shift) % SCREEN_HEIGHT] = light_at(s, x, y);
  }

  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    set_light(s, x, y, col_shifted[y]);
  }
}

/* Set all lights in the rectangular top-left corner of s to on. */
void turn_on_rectangle(screen *s, int rect_w, int rect_h) {
  for (int x = 0; x < rect_w; x++) {
    for (int y = 0; y < rect_h; y++) {
      set_light(s, x, y, LIGHT_ON);
    }
  }
}

/* The number of burning lights on the screen s. */
int num_burning_lights(screen *s) {
  int num = 0;
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    /* State is only 0 or 1 anyway, hence this is good enough for aoc. */
    num += s->lights[i];
  }
  return num;
}

/* Print a representation of the screen s. */
void print_screen(screen *s) {
  /* Note that we have to iterate x in the inner loop. */
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      if (light_at(s, x, y) == LIGHT_ON) {
        printf("#");
      } else {
        printf(".");
      }
    }
    printf("\n");
  }
}

/* Determine the number of burning lights after following the instructions.
   In addition, print the screen to determine the writing. */
void part_one_and_two(FILE *f) {
  /* Initialize screen. */
  screen s;
  turn_off_all(&s);
  /* Prepare to read input. */
  char *lbuf = NULL;
  size_t llen = 0;
  while (getline(&lbuf, &llen, f) != EOF) {
    /* Follow next instruction. */
    if (strncmp(lbuf, "rect", 4) == 0) {
      int w, h;
      sscanf(lbuf, "rect %dx%d", &w, &h);
      turn_on_rectangle(&s, w, h);
    } else if (strncmp(lbuf, "rotate column", 13) == 0) {
      int x, shift;
      sscanf(lbuf, "rotate column x=%d by %d", &x, &shift);
      shift_column(&s, x, shift);
    } else if (strncmp(lbuf, "rotate row", 10) == 0) {
      int y, shift;
      sscanf(lbuf, "rotate row y=%d by %d", &y, &shift);
      shift_row(&s, y, shift);
    } else {
      fprintf(stderr, "Unrecognized line: %s", lbuf);
      exit(EXIT_FAILURE);
    }
  }

  free(lbuf);
  printf("%d\n\n", num_burning_lights(&s));
  print_screen(&s);
}

int main(int argc, const char *argv[]) {
  FILE *f = read_input(argc, argv);
  char buffer[BUFSIZ];
  setbuf(f, buffer);

  part_one_and_two(f);

  fclose(f);
  exit(EXIT_SUCCESS);
}
