#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26
#define NUM_MOST_FREQUENT 5

typedef struct {
  char letter;
  int frequency;
} elem;

/* Create a new heap-allocated element. */
elem *new_elem(char letter) {
  elem *e = malloc(sizeof(elem));
  e->letter = letter;
  e->frequency = 0;
  return e;
}

/* Comparison function for frequency-letter-elements. */
int by_frequency_desc(const void *a, const void *b) {
  elem *a_elem = (elem * )a;
  elem *b_elem = (elem * )b;
  if (a_elem->frequency > b_elem->frequency) {
    return -1;
  } else if (a_elem->frequency < b_elem->frequency) {
    return 1;
  }
  return a_elem->letter < b_elem->letter ? -1 : 1;
}

/* Whether the most frequent of letter_freqs are the same as letters. */
bool frequencies_match(char letters[], elem letter_freqs[]) {
  qsort(letter_freqs, ALPHABET_SIZE, sizeof(elem*), &by_frequency_desc);
  for (int i = 0; i < NUM_MOST_FREQUENT; i++) {
    /* It is assumed that letters are by descending frequency the most
       frequent letters of the input line. */
    if (letters[i] != letter_freqs[i].letter) {
      return false;
    }
  }
  return true;
}

/* The sector ID if the line represents a real room, 0 otherwise. */
int is_real_room(const char *line) {
  elem letter_freqs[ALPHABET_SIZE];
  int i;
  /* Initialize. */
  for (i = 0; i < ALPHABET_SIZE; i++) {
    letter_freqs[i] = *new_elem('a' + i);
  }
  /* Collect frequencies. */
  int letter_index;
  for (i = 0; !isdigit(line[i]); i++) {
    if (line[i] == '-') {
      continue;
    }
    letter_index = line[i] - 'a';
    letter_freqs[letter_index].frequency++;
  }
  /* Extract sector ID and allegedly frequent letters. */
  int sector_id;
  int result;
  char letters[NUM_MOST_FREQUENT + 1];
  sscanf(&line[i], "%d[%[a-z]s]\n", &sector_id, letters);
  if (frequencies_match(letters, letter_freqs)) {
    result = sector_id;
  } else {
    result = 0;
  }
  return result;
}

/* The sum of sector IDs for all real rooms in the file. */
void part_one(FILE *f) {
  rewind(f);
  int sector_id_sum = 0;
  char *line;
  size_t len;
  while (getline(&line, &len, f) != EOF) {
    sector_id_sum += is_real_room(line);
  }
  free(line);
  printf("%d\n", sector_id_sum);
}

/* Decipher the text of the line with room_id. Result is heap-allocated. */
char *cleartext(const char *line, int room_id) {
  int len = strlen(line);
  char *buffer = malloc(sizeof(char) * (len + 1));
  int i;
  for (i = 0; !isdigit(line[i]) && i < len; i++) {
    if (line[i] == '-') {
      buffer[i] = ' ';
    } else {
      buffer[i] = 'a' + ((line[i] - 'a' + room_id) % ALPHABET_SIZE);
    }
  }
  buffer[i-1] = '\0';
  return buffer;
}

/* Whether text marks the storage of north pole objects. */
bool contains_north_pole_objects(const char * text) {
  if (strstr(text, "north") && strstr(text, "pole") && strstr(text, "stor")) {
    return true;
  }
  return false;
}

/* Decipher the text for all real rooms. */
void part_two(FILE *f) {
  rewind(f);
  char *line = malloc(BUFSIZ);
  size_t len;
  int room_id = 0;
  while (getline(&line, &len, f) != EOF) {
    room_id = is_real_room(line);
    if (room_id) {
      char *text = cleartext(line, room_id);
      if (contains_north_pole_objects(text)) {
        printf("%d\n", room_id);
        free(text);
        return;
      }
      free(text);
    }
  }
  free(line);
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
