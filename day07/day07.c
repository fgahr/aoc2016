#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/lib.h"

#define INSIDE_BRACKETS 0
#define OUTSIDE_BRACKETS 1

/* Used to store information about ABA letter sequences. */
typedef struct {
  char outer;
  char inner;
} aba_seq;

/* Whether str starts with an ABBA quartet. */
bool starts_with_abba(const char *str) {
  if (strlen(str) < 4) {
    return false;
  }
  /* Disregard if it contains opening or closing brackets. */
  for (int i = 0; i < 4; i++) {
    if (str[i] == '[' || str[i] == ']') {
      return false;
    }
  }
  /* ABBA check. */
  return str[0] != str[1] && str[0] == str[3] && str[1] == str[2];
}

/* Whether the given IPv7 string supports TLS according to the puzzle rules. */
bool supports_tls(const char *ip) {
  /* Assume for now that brackets are not nested. */
  int state = OUTSIDE_BRACKETS;
  int abba_encountered_outside = false;
  for (int i = 0; ip[i] != '\n' && ip[i] != '\0'; i++) {
    if (ip[i] == '[') {
      state = INSIDE_BRACKETS;
    } else if (ip[i] == ']') {
      state = OUTSIDE_BRACKETS;
    } else {
      if (starts_with_abba(&ip[i])) {
        if (state == OUTSIDE_BRACKETS) {
          abba_encountered_outside = true;
        } else {
          return false;
        }
      }
    }
  }
  return abba_encountered_outside;
}

/* Whether str starts with an ABA sequence. */
bool starts_with_aba(const char *str) {
  if (strlen(str) < 3) {
    return false;
  }
  /* Disregard if it contains opening or closing brackets. */
  for (int i = 0; i < 3; i++) {
    if (str[i] == '[' || str[i] == ']') {
      return false;
    }
  }
  /* ABA check. */
  return str[0] != str[1] && str[0] == str[2];
}

/* Whether the given IPv7 string supports SSL according to the puzzle rules. */
bool supports_ssl(const char *ip) {
  int state = OUTSIDE_BRACKETS;
  /* Realistic upper bound of sequences to be found. */
  aba_seq sequences[128];
  int seq_idx = 0;
  for (int i = 0; ip[i] != '\n' && ip[i] != '\0'; i++) {
    if (ip[i] == '[') {
      state = INSIDE_BRACKETS;
    } else if (ip[i] == ']') {
      state = OUTSIDE_BRACKETS;
    } else {
      if (state == OUTSIDE_BRACKETS && starts_with_aba(&ip[i])) {
        sequences[seq_idx] = (aba_seq){ip[i], ip[i + 1]};
        ++seq_idx;
      }
    }
  }

  for (int i = 0; ip[i] != '\n' && ip[i] != '\0'; i++) {
    if (ip[i] == '[') {
      state = INSIDE_BRACKETS;
    } else if (ip[i] == ']') {
      state = OUTSIDE_BRACKETS;
    } else {
      if (state == INSIDE_BRACKETS && starts_with_aba(&ip[i])) {
        for (int j = 0; j < seq_idx; j++) {
          if (sequences[j].inner == ip[i] &&
              sequences[j].outer == ip[i + 1]) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

/* The number of IPv7 IPs supporting TLS. */
void part_one(FILE *f) {
  rewind(f);
  int num_tls_ips = 0;
  /* Allocate buffer. */
  size_t llen = BUFSIZ;
  char *lbuf = malloc(BUFSIZ * sizeof(char));
  while (getline(&lbuf, &llen, f) != EOF) {
    if (supports_tls(lbuf)) {
      ++num_tls_ips;
    }
  }
  free(lbuf);
  printf("%d\n", num_tls_ips);
}

/* The number of IPv7 IPs supporting SSL. */
void part_two(FILE *f) {
  rewind(f);
  int num_ssl_ips = 0;
  /* Allocate buffer. */
  size_t llen = BUFSIZ;
  char *lbuf = malloc(BUFSIZ * sizeof(char));
  while (getline(&lbuf, &llen, f) != EOF) {
    if (supports_ssl(lbuf)) {
      ++num_ssl_ips;
    }
  }
  free(lbuf);
  printf("%d\n", num_ssl_ips);
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
