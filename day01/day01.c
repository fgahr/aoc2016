#include <stdio.h>
#include <stdlib.h>

#define ABS(x) ((x) > 0 ? (x) : -(x))

#define INFILE "input.txt"

void move(int *pos_x, int *pos_y, int dir, int distance) {
  if (dir == 0) {
    *pos_y += distance;
  } else if (dir == 1) {
    *pos_x += distance;
  } else if (dir == 2) {
    *pos_y -= distance;
  } else if (dir == 3) {
    *pos_x -= distance;
  }
}

void part_one() {
  FILE *f = fopen(INFILE, "r");

  char c;
  int distance;

  int pos_x = 0;
  int pos_y = 0;
  int dir = 0;

  while ((c = fgetc(f)) != EOF) {
    if (c != 'R' && c != 'L') {
      continue;
    }
    if (c == 'R') {
      dir = (dir + 1) % 4;
    } else {
      dir = (dir + 3) % 4;
    }
    fscanf(f, "%d", &distance);
    move(&pos_x, &pos_y, dir, distance);
  }
  fclose(f);

  printf("Part one: %d\n", ABS(pos_x) + ABS(pos_y));
}

typedef struct place {
  int x;
  int y;
  struct place *next;
} place;

place *new_place(int x, int y) {
  place *p = malloc(sizeof(place));
  p->x = x;
  p->y = y;
  p->next = NULL;
  return p;
}

int seen(int pos_x, int pos_y, place *visited) {
  if (visited == NULL) {
    return 0;
  }
  if (pos_x == visited->x && pos_y == visited->y) {
    return 1;
  }
  return seen(pos_x, pos_y, visited->next);
}

void part_two() {
  FILE *f = fopen(INFILE, "r");

  char c;
  int distance;

  int pos_x = 0;
  int pos_y = 0;
  int dir = 0;
  place visited = {pos_x, pos_y, NULL};
  place *current = &visited;
  int skip_check = 1;

  while ((c = fgetc(f)) != EOF) {
    if (c != 'R' && c != 'L') {
      continue;
    }
    if (c == 'R') {
      dir = (dir + 1) % 4;
    } else {
      dir = (dir + 3) % 4;
    }
    fscanf(f, "%d", &distance);
    for (int i = 0; i < distance; i++) {
      move(&pos_x, &pos_y, dir, 1);
      if (skip_check) {
        skip_check = 0;
        continue;
      }
      if (seen(pos_x, pos_y, &visited)) {
        fclose(f);
        printf("Part two: %d\n", ABS(pos_x) + ABS(pos_y));
        return;
      }
      current->next = new_place(pos_x, pos_y);
      current = current->next;
    }
  }
  fclose(f);
  printf("Part two: %d\n", ABS(pos_x) + ABS(pos_y));
}

int main() {
  part_one();
  part_two();
}
