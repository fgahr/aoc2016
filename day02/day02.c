#include <stdio.h>
#include <stdlib.h>
#include "../lib/lib.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define SIMPLE_PAD_SIZE 3
#define FANCY_PAD_SIZE 5
#define NOP '\0'

char enter_simple_pad(const char *command) {
  /* Start at '5'. */
  static int pos_x = 1;
  static int pos_y = 1;
  static int simple_pad[SIMPLE_PAD_SIZE][SIMPLE_PAD_SIZE] = {
      {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}};

  char c;
  for (int i = 0; (c = command[i]) != '\0' && c != '\n'; i++) {
    switch (c) {
    case 'U': {
      pos_y = MAX(pos_y - 1, 0);
      break;
    }
    case 'D': {
      pos_y = MIN(pos_y + 1, SIMPLE_PAD_SIZE - 1);
      break;
    }
    case 'L': {
      pos_x = MAX(pos_x - 1, 0);
      break;
    }
    case 'R': {
      pos_x = MIN(pos_x + 1, SIMPLE_PAD_SIZE - 1);
      break;
    }
    default:
      fprintf(stderr, "Invalid command: %c\n", c);
      exit(EXIT_FAILURE);
      break;
    }
  }
  return simple_pad[pos_y][pos_x];
}

char enter_fancy_pad(const char *command) {
  /* Start at '5'. */
  static int pos_x = 0;
  static int pos_y = 2;
  /* NOP marks fields which do not exist on the pad. */
  static int fancy_pad[FANCY_PAD_SIZE][FANCY_PAD_SIZE] = {
      {NOP, NOP, '1', NOP, NOP},
      {NOP, '2', '3', '4', NOP},
      {'5', '6', '7', '8', '9'},
      {NOP, 'A', 'B', 'C', NOP},
      {NOP, NOP, 'D', NOP, NOP}};

  char c;
  int newpos;
  for (int i = 0; (c = command[i]) != '\0' && c != '\n'; i++) {
    switch (c) {
    case 'U': {
      newpos = MAX(pos_y - 1, 0);
      if (fancy_pad[newpos][pos_x] != NOP) {
        pos_y = newpos;
      }
      break;
    }
    case 'D': {
      newpos = MIN(pos_y + 1, FANCY_PAD_SIZE - 1);
      if (fancy_pad[newpos][pos_x] != NOP) {
        pos_y = newpos;
      }
      break;
    }
    case 'L': {
      newpos = MAX(pos_x - 1, 0);
      if (fancy_pad[pos_y][newpos] != NOP) {
        pos_x = newpos;
      }
      break;
    }
    case 'R': {
      newpos = MIN(pos_x + 1, FANCY_PAD_SIZE - 1);
      if (fancy_pad[pos_y][newpos] != NOP) {
        pos_x = newpos;
      }
      break;
    }
    default:
      fprintf(stderr, "Invalid command: %c\n", c);
      exit(EXIT_FAILURE);
      break;
    }
  }
  return fancy_pad[pos_y][pos_x];
}

void unlock(FILE *f, char (*enter_pad)(const char *)) {
  char *lbuf;
  size_t llen;
  ssize_t nread;
  while ((nread = getline(&lbuf, &llen, f)) != EOF) {
    printf("%c", enter_pad(lbuf));
  }
  printf("\n");

  free(lbuf);
}

void part_one(FILE *f) {
  rewind(f);
  unlock(f, &enter_simple_pad);
}

void part_two(FILE *f) {
  rewind(f);
  unlock(f, &enter_fancy_pad);
}

int main(int argc, const char *argv[]) {
  FILE *f = read_input(argc, argv);

  part_one(f);
  part_two(f);
  fclose(f);
  exit(EXIT_SUCCESS);
}
