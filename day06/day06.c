#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "../lib/lib.h"

#define INPUT_LINE_LENGTH 8
#define ALPHABET_SIZE 26

typedef struct {
  int count[ALPHABET_SIZE];
} char_counts;

/* Determine the letter frequency table from the file contents. */
char_counts *read_letter_frequencies(FILE *f) {
  /* Initialize letter count table. */
  char_counts *letter_freqs = malloc(INPUT_LINE_LENGTH * sizeof(char_counts));
  for (int i = 0; i < INPUT_LINE_LENGTH; i++) {
    for (int j = 0; j < ALPHABET_SIZE; j++) {
      letter_freqs[i].count[j] = 0;
    }
  }
  size_t llen = INPUT_LINE_LENGTH + 1;
  char *lbuf = malloc(llen * sizeof(char));
  /* Collect letter frequencies from the input. */
  while (getline(&lbuf, &llen, f) != EOF) {
    for (int i = 0; i < INPUT_LINE_LENGTH; i++) {
      int c = lbuf[i] - 'a';
      letter_freqs[i].count[c]++;
    }
  }
  free(lbuf);
  return letter_freqs;
}

/* Determine and print the most frequent letter for each position. */
void part_one(char_counts *letter_freqs) {
  for (int i = 0; i < INPUT_LINE_LENGTH; i++) {
    int max_pos = 0;
    int max_count = 0;
    for (int j = 0; j < ALPHABET_SIZE; j++) {
      if (letter_freqs[i].count[j] > max_count) {
        max_pos = j;
        max_count = letter_freqs[i].count[j];
      }
    }
    printf("%c", max_pos + 'a');
  }
  printf("\n");
}

/* Determine and print the least frequent letter for each position. */
void part_two(char_counts *letter_freqs) {
  for (int i = 0; i < INPUT_LINE_LENGTH; i++) {
    int max_pos = 0;
    int max_count = INT_MAX;
    for (int j = 0; j < ALPHABET_SIZE; j++) {
      if (letter_freqs[i].count[j] < max_count) {
        max_pos = j;
        max_count = letter_freqs[i].count[j];
      }
    }
    printf("%c", max_pos + 'a');
  }
  printf("\n");
}

int main(int argc, const char *argv[]) {
  FILE *f = read_input(argc, argv);
  char buffer[BUFSIZ];
  setbuf(f, buffer);

  char_counts *letter_freqs = read_letter_frequencies(f);

  part_one(letter_freqs);
  part_two(letter_freqs);

  fclose(f);
  free(letter_freqs);
  exit(EXIT_SUCCESS);
}
