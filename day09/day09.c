#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../lib/lib.h"

/* Determine the decompressed size of the input file. */
void part_one(FILE *f) {
  rewind(f);
  int a;
  int size = 0;
  while ((a = fgetc(f)) != EOF) {
    if (a == '(') {
      /* Read the decompression marker. */
      int x, y;
      fscanf(f, "%dx%d)", &x, &y);
      size += x * y;
      for (int i = 0; i < x; i++) {
        /* Ignore the next x characters. */
        fgetc(f);
      }
    } else if (a != '\n') {
      /* Just one character is being read, ignore newline. */
      ++size;
    }
  }
  printf("%d\n", size);
}

/* Determine the decompressed size of the given fragment of specified length. */
long decompressed_size(const char *fragment, int len) {
  long size = 0;
  for (int i = 0; i < len;) {
    if (fragment[i] == '(') {
      ++i;
      int x, y;
      if (sscanf(&fragment[i], "%dx%d)", &x, &y) < 2) {
        fprintf(stderr, "Unable to read decompression marker: %s\n",
                &fragment[i]);
        exit(EXIT_FAILURE);
      }
      /* Fast-forward just past the closing paren. */
      while (fragment[i] != ')' && i < len) {
        ++i;
      }
      ++i;
      /* Abbreviated error handling. */
      assert(i < len);
      /* Recursively decompress the fragment of size x. */
      char *buffer = malloc(x * sizeof(char));
      if (strncpy(buffer, &fragment[i], x) == NULL) {
        fprintf(stderr, "Unable to read fragment of length %d\n", x);
        exit(EXIT_FAILURE);
      }
      size += y * decompressed_size(buffer, x);
      free(buffer);
      /* Skip the already decompressed bit. */
      i += x;
    } else if (fragment[i] != '\n') {
      /* Just one character, no decompression. */
      ++i;
      ++size;
    }
  }
  /* printf("Decompressed "); */
  /* for (int i = 0; i < len; i++) { */
  /*   printf("%c", fragment[i]); */
  /* } */
  /* printf(": %d\n", size); */
  return size;
}

/* Determine the decompressed size of the input file, using version two. */
void part_two(FILE *f) {
  rewind(f);
  int a;
  long size = 0;
  while ((a = fgetc(f)) != EOF) {
    if (a == '(') {
      /* Read the decompression marker. */
      int x = 0;
      int y = 0;
      if (fscanf(f, "%dx%d)", &x, &y) < 2) {
        fprintf(stderr, "Unable to read decompression marker.");
        exit(EXIT_FAILURE);
      }
      /* Recursively decompress. */
      char *fragment = malloc((x + 1) * sizeof(char));
      fgets(fragment, x + 1, f);
      size += y * decompressed_size(fragment, x);
      free(fragment);
    } else if (a != '\n') {
      /* Just one character is being read, ignore newline. */
      ++size;
    }
  }
  printf("%ld\n", size);
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
