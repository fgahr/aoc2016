#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lib/lib.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))
/* Should be plenty for all inputs. */
#define POSITION_LIST_SIZE 16384

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

/* Represents a direction along the path. */
typedef int dir;

/* Facing the given direction, move ahead from the x, y coordinates. */
void move_ahead(int *pos_x, int *pos_y, dir direction, int distance) {
  if (direction == NORTH) {
    *pos_y += distance;
  } else if (direction == EAST) {
    *pos_x += distance;
  } else if (direction == SOUTH) {
    *pos_y -= distance;
  } else if (direction == WEST) {
    *pos_x -= distance;
  }
}

int dist_from_start(int x, int y) { return ABS(x) + ABS(y); }

/* Facing in the given direction, adjust it to a right turn. */
void turn_right(dir *direction) { *direction = (*direction + 1) % 4; }

/* Facing in the given direction, adjust it to a left turn. */
void turn_left(dir *direction) { *direction = (*direction + 3) % 4; }

/* Represents a place along the way that is being visited. */
typedef struct {
  int x;
  int y;
} location;

/* Determine the distance from the start after following the instructions. */
void part_one(FILE *f) {
  rewind(f);
  char c;
  int distance;

  int pos_x = 0;
  int pos_y = 0;
  dir direction = NORTH;

  while ((c = fgetc(f)) != EOF) {
    if (c != 'R' && c != 'L') {
      continue;
    }
    if (c == 'R') {
      turn_right(&direction);
    } else {
      turn_left(&direction);
    }
    fscanf(f, "%d", &distance);
    move_ahead(&pos_x, &pos_y, direction, distance);
  }

  printf("%d\n", dist_from_start(pos_x, pos_y));
}

/* Determine the distance of the first self-intersection of the path. */
void part_two(FILE *f) {
  rewind(f);
  char turn;
  int distance;

  int pos_x = 0;
  int pos_y = 0;
  int direction = 0;

  /* Memorize all visited positions. */
  location places[POSITION_LIST_SIZE];
  places[0] = (location) {0, 0};
  int place_idx = 1;

  while (fscanf(f, "%c%d%*2[, ]", &turn, &distance) != EOF) {
    switch (turn) {
    case 'L': {
      turn_left(&direction);
      break;
    }
    case 'R': {
      turn_right(&direction);
      break;
    }
    default:
      fprintf(stderr, "Unknown direction encountered: %c\n", turn);
      exit(EXIT_FAILURE);
      break;
    }

    for (int i = 0; i < distance; i++) {
      move_ahead(&pos_x, &pos_y, direction, 1);
      places[place_idx] = (location){pos_x, pos_y};
      ++place_idx;
      if (place_idx >= POSITION_LIST_SIZE) {
        fprintf(stderr, "Position list size exceeded. Increase the limit or "
                        "use dynamic re-allocation.");
      }
      /* Omit the current place. */
      for (int j = 0; j < place_idx - 1; j++) {
        if (pos_x == places[j].x && pos_y == places[j].y) {
          printf("%d\n", dist_from_start(pos_x, pos_y));
          return;
        }
      }
    }
  }

  printf("%d\n", dist_from_start(pos_x, pos_y));
}

int main(int argc, const char *argv[]) {
  FILE *f = read_input(argc, argv);
  char buffer[BUFSIZ];
  setbuf(f, buffer);

  part_one(f);
  part_two(f);

  fclose(f);
  exit(EXIT_SUCCESS);
}
