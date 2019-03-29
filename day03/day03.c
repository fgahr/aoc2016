#include <stdio.h>
#include <stdlib.h>

int is_valid_triangle(int a, int b, int c) {
  return a < b + c && b < a + c && c < a + b;
}

void part_one(FILE *f) {
  rewind(f);
  char *lbuf;
  size_t llen;
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

void part_two(FILE *f) {
  rewind(f);
  char *lbuf;
  size_t llen;
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
  if (argc < 2) {
    fprintf(stderr, "%s -- Error: No input file given.\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    fprintf(stderr, "%s -- Error: File does not exist: %s\n", argv[0], argv[1]);
    exit(EXIT_FAILURE);
  }
  char inbuf[BUFSIZ];
  setbuf(f, inbuf);

  part_one(f);
  part_two(f);

  fclose(f);
  exit(EXIT_SUCCESS);
}
