#include <stdio.h>
#include <stdlib.h>

#include "../lib/lib.h"

/* 1 if the side lengths a, b, and c can make up a valid triangle, else 0.

   NOTE: Since the result is used in a sum, treat it as a number rather
   than true/false. */
int is_valid_triangle(int a, int b, int c) {
  return a < b + c && b < a + c && c < a + b;
}

/* The number of possible triangles in the file. */
void part_one(FILE *f) {
  rewind(f);
  char *lbuf = NULL;
  size_t llen = 0;
  ssize_t nread;
  int num_triangles = 0;
  int a, b, c;
  while ((nread = getline(&lbuf, &llen, f)) != EOF) {
    sscanf(lbuf, "%5d%5d%5d", &a, &b, &c);
    num_triangles += is_valid_triangle(a, b, c);
  }
  printf("%d\n", num_triangles);

  free(lbuf);
}

/* The number of possible triangles in the file, assuming columnar data. */
void part_two(FILE *f) {
  rewind(f);
  char *lbuf = NULL;
  size_t llen = 0;
  int num_triangles = 0;
  /* Read three triangles at once, hence store all sides at the same time. */
  int a1, a2, a3;
  int b1, b2, b3;
  int c1, c2, c3;
  /* Assume that the number of input lines is a multiple of 3. */
  while (1) {
    if (getline(&lbuf, &llen, f) == -1) {
      break;
    }

    sscanf(lbuf, "%5d%5d%5d", &a1, &b1, &c1);
    getline(&lbuf, &llen, f);
    sscanf(lbuf, "%5d%5d%5d", &a2, &b2, &c2);
    getline(&lbuf, &llen, f);
    sscanf(lbuf, "%5d%5d%5d", &a3, &b3, &c3);
    num_triangles += is_valid_triangle(a1, a2, a3) +
                     is_valid_triangle(b1, b2, b3) +
                     is_valid_triangle(c1, c2, c3);
  }
  printf("%d\n", num_triangles);

  free(lbuf);
}

int main(int argc, const char *argv[]) {
  FILE *f = read_input(argc, argv);
  char inbuf[BUFSIZ];
  setbuf(f, inbuf);

  part_one(f);
  part_two(f);

  fclose(f);
  exit(EXIT_SUCCESS);
}
