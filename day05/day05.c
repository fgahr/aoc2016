#include <ctype.h>
#include <gcrypt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lib/lib.h"

#define MD5_DIGEST_BYTES 16

#define PASSWORD_LENGTH 8

bool starts_with_five_zeros(const char *str) {
  for (int i = 0; i < 5; i++) {
    if (str[i] != '0') {
      return false;
    }
  }
  return true;
}

/* The resulting hex string from digesting the input. */
void md5_hex(gcry_md_hd_t handle, const char *input, size_t input_size,
             char out_buf[]) {
  /* Create md5 digest. */
  gcry_md_write(handle, input, input_size);
  unsigned char *d = gcry_md_read(handle, GCRY_MD_MD5);
  /* Create (32-digit) hex output. */
  for (int i = 0; i < MD5_DIGEST_BYTES; i++) {
    sprintf(&out_buf[2 * i], "%02x", d[i]);
  }
  gcry_md_reset(handle);
}

/* Find the password for the given door ID. */
void part_one(const char *door_id) {
  /* Create crypt handle. */
  gcry_md_hd_t handle;
  gcry_md_open(&handle, GCRY_MD_MD5, 0x0);
  if (handle == NULL) {
    fprintf(stderr, "Creating gcrypt handle failed.\n");
    exit(EXIT_FAILURE);
  }

  /* Sufficient margin for all possible pre-hashes. */
  char pre_hash_buf[40];
  char password[PASSWORD_LENGTH + 1] = {'\0'};
  int passwd_char_idx = 0;
  /* Output buffer for resulting hashes. */
  char out_buf[2 * MD5_DIGEST_BYTES + 1];

  /* Find the password characters. */
  for (int i = 0; passwd_char_idx < PASSWORD_LENGTH; i++) {
    size_t buf_size = sprintf(pre_hash_buf, "%s%d", door_id, i);
    md5_hex(handle, pre_hash_buf, buf_size, out_buf);
    if (starts_with_five_zeros(out_buf)) {
      password[passwd_char_idx] = out_buf[5];
      ++passwd_char_idx;
    }
  }

  gcry_md_close(handle);
  printf("%s\n", password);
}

/* Whether all of the password positions have been filled. */
bool all_filled(bool filled[]) {
  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    if (!filled[i]) {
      return false;
    }
  }
  return true;
}

/* Find the password for the given door ID -- improved version. */
void part_two(const char *door_id) {
  /* Create crypt handle. */
  gcry_md_hd_t handle;
  gcry_md_open(&handle, GCRY_MD_MD5, 0x0);
  if (handle == NULL) {
    fprintf(stderr, "Creating gcrypt handle failed.\n");
    exit(EXIT_FAILURE);
  }

  /* Sufficient margin for all possible pre-hashes. */
  char pre_hash_buf[40];
  char password[PASSWORD_LENGTH + 1] = {'\0'};
  bool filled[PASSWORD_LENGTH] = {false};
  /* Output buffer for resulting hashes. */
  char out_buf[2 * MD5_DIGEST_BYTES + 1];

  /* Find the password characters. */
  for (int i = 0; !all_filled(filled); i++) {
    size_t buf_size = sprintf(pre_hash_buf, "%s%d", door_id, i);
    md5_hex(handle, pre_hash_buf, buf_size, out_buf);
    if (starts_with_five_zeros(out_buf)) {
      if (isdigit(out_buf[5])) {
        int pos = out_buf[5] - '0';
        if (pos > PASSWORD_LENGTH - 1 || filled[pos]) {
          continue;
        }
        password[pos] = out_buf[6];
        filled[pos] = true;
      }
    }
  }

  gcry_md_close(handle);
  printf("%s\n", password);
}

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "No door ID given.\n");
    exit(EXIT_FAILURE);
  }
  const char *door_id = argv[1];

  part_one(door_id);
  part_two(door_id);

  exit(EXIT_SUCCESS);
}
