#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>

/* Defined in header file for simplicity as ever-new main files make proper
   library handling a bit of a hassle. */
FILE *read_input(int argc, const char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "%s -- Error: No input file given.\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    fprintf(stderr, "%s -- Error: File does not exist: %s\n", argv[0], argv[1]);
    exit(EXIT_FAILURE);
  }
  char *inbuf = malloc(BUFSIZ);
  setbuf(f, inbuf);

  return f;
}

#endif /* LIB_H */
