#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/lib.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define BOT_TYPE 0
#define OUT_TYPE 1

/* Chips have positive values. Negative values indicate an empty chip slot. */
#define NO_CHIP (-1)
/* Type for chips which will be passed around. */
typedef int chip;

/* An object which can receive a chip. */
typedef struct receiver {
  /* BOT_TYPE or OUT_TYPE */
  int rec_type;
  /* Outputs and bots both have IDs. */
  int id;
  /* Outputs are a no-op for now, hence only add bot-specifics. */
  chip chip_one;
  chip chip_two;
  struct receiver *high_rec;
  struct receiver *low_rec;
} receiver;

/* Print a description of a receiver for debugging purposes. */
void print_receiver(receiver *rec) {
  if (rec == NULL) {
    printf("NULL");
    return;
  }
  char *description = "bot";
  if (rec->rec_type == OUT_TYPE) {
    description = "output";
  }
  printf("%s #%d: high_rec: %p, low_rec: %p, chips: [%d, %d]\n", description,
         rec->id, rec->high_rec, rec->low_rec, rec->chip_one, rec->chip_two);
}

/* Create a new heap-allocated receiver of given type and ID and return a
   pointer to it. */
receiver *new_receiver(int rec_type, int id) {
  receiver *rec = malloc(sizeof(receiver));
  rec->rec_type = rec_type;
  rec->id = id;
  rec->chip_one = NO_CHIP;
  rec->chip_two = NO_CHIP;
  rec->high_rec = NULL;
  rec->low_rec = NULL;
  return rec;
}

/* Global variables to keep track of all objects. The scope of this program
   doesn't warrant having a state object for these. */
receiver **recs;
uint rec_num;
uint rec_cap;

/* Initialize the receiver storage. */
void init_receivers(void) {
  rec_cap = 64;
  recs = malloc(rec_cap * sizeof(receiver *));
  rec_num = 0;
}

/* Add a new receiver with given type and ID to the receiver pool. */
receiver *register_receiver(int rec_type, int id) {
  if (rec_num == (rec_cap - 1)) {
    /* Enlarge capacity. */
    rec_cap *= 2;
    receiver **more_recs = realloc(recs, rec_cap * sizeof(receiver *));
    if (more_recs == NULL) {
      fprintf(stderr, "Allocating more receivers failed.");
      free(recs);
      exit(EXIT_FAILURE);
    }
    recs = more_recs;
  }
  receiver *new_rec = recs[rec_num] = new_receiver(rec_type, id);
  ++rec_num;
  return new_rec;
}

/* Give chip c to the receiver rec. */
void receive_chip(receiver *rec, chip c) {
  if (rec == NULL) {
    fprintf(stderr, "Receiver is NULL.");
    exit(EXIT_FAILURE);
  }
  /* Outputs are assumed to only ever receive one chip. */
  if (rec->rec_type == OUT_TYPE) {
    if (rec->chip_one != NO_CHIP) {
      fprintf(stderr, "Attempting to give a second chip to output %d\n",
              rec->id);
      exit(EXIT_FAILURE);
    }
    rec->chip_one = c;
    return;
  }
  /* Bots can occupy both slots. Always fill slot one first. */
  if (rec->chip_one == NO_CHIP) {
    rec->chip_one = c;
  } else if (rec->chip_two == NO_CHIP) {
    rec->chip_two = c;
  } else {
    fprintf(
        stderr,
        "Receiver (type %d, id %d) cannot receive chip: %d. No empty slot.\n",
        rec->rec_type, rec->id, c);
  }
}

/* Let the given bot transfer its chips to their respective receivers.
   Returns whether a transfer was actually performed. */
bool init_transfer(receiver *bot) {
  /* Outputs cannot transfer. For simplicity we allow this as a no-op. */
  if (bot->rec_type != BOT_TYPE) {
    return false;
  }
  /* Not ready for transfer yet or transfer already done. */
  if ((bot->chip_one == NO_CHIP) || (bot->chip_two == NO_CHIP)) {
    return false;
  }
  /* Illegal state at this point. */
  if ((bot->high_rec == NULL) || (bot->low_rec == NULL)) {
    fprintf(stderr, "Bot #%d doesn't have both receivers set up.\n", bot->id);
    print_receiver(bot);
    exit(EXIT_FAILURE);
  }
  /* Transfer chips. */
  chip high = MAX(bot->chip_one, bot->chip_two);
  chip low = MIN(bot->chip_one, bot->chip_two);
  receive_chip(bot->high_rec, high);
  receive_chip(bot->low_rec, low);
  bot->chip_one = NO_CHIP;
  bot->chip_two = NO_CHIP;
  return true;
}

/* Find or create the receiver with given type and ID. */
receiver *select_receiver(int rec_type, int id) {
  for (uint i = 0; i < rec_num; i++) {
    if (recs[i]->rec_type == rec_type && recs[i]->id == id) {
      return recs[i];
    }
  }
  return register_receiver(rec_type, id);
}

/* Find or create a bot with the given ID. */
receiver *select_bot(int id) { return select_receiver(BOT_TYPE, id); }

/* Find or create an output with the given ID. */
receiver *select_output(int id) { return select_receiver(OUT_TYPE, id); }

/* The receiver type based on the description given in the input. */
int receiver_type(const char *name) {
  if (strcmp(name, "bot") == 0) {
    return BOT_TYPE;
  } else if (strcmp(name, "output") == 0) {
    return OUT_TYPE;
  } else {
    fprintf(stderr, "Unknown receiver type name: %s\n", name);
    exit(EXIT_FAILURE);
  }
}

/* Setup the receivers and their connections. */
void init_transfer_network(FILE *f) {
  rewind(f);
  char *lbuf = NULL;
  size_t llen = 0;
  /* Buffers for parsing. */
  char low_type[8];
  char high_type[8];
  while (getline(&lbuf, &llen, f) != EOF) {
    if (strncmp(lbuf, "val", 3) == 0) {
      chip c;
      int id;
      if (sscanf(lbuf, "value %d goes to bot %d", &c, &id) < 2) {
        fprintf(stderr, "Unable to parse: %s", lbuf);
        exit(EXIT_FAILURE);
      }
      receive_chip(select_bot(id), c);
    } else if (strncmp(lbuf, "bot", 3) == 0) {
      /* Strictly speaking, 7 would be long enough ("output" + '\0'). */
      int giver_id, low_id, high_id;
      if (sscanf(lbuf, "bot %d gives low to %s %d and high to %s %d\n",
                 &giver_id, low_type, &low_id, high_type, &high_id) < 5) {
        fprintf(stderr, "Unable to parse: %s", lbuf);
        exit(EXIT_FAILURE);
      }
      receiver *bot = select_bot(giver_id);
      bot->low_rec = select_receiver(receiver_type(low_type), low_id);
      bot->high_rec = select_receiver(receiver_type(high_type), high_id);
    } else {
      fprintf(stderr, "Unable to parse: %s", lbuf);
      exit(EXIT_FAILURE);
    }
  }
  /* Done with input. */
  free(lbuf);
}

/* Whether the given bot is the bot sought for in part one. */
bool part_one_winner(receiver *bot) {
  return (bot->chip_one == 61 && bot->chip_two == 17) ||
         (bot->chip_one == 17 && bot->chip_two == 61);
}

/* Find the bot comparing chips of values 61 and 17. */
void part_one(FILE *f) {
  init_transfer_network(f);

  /* Continue initiating transfers until the right bot is found.

     NOTE: A recursive approach with just one list iteration would be more
           elegant but is unfeasible due to the exit condition. */
  bool something_happened = true;
  while (something_happened) {
    something_happened = false;
    for (uint i = 0; i < rec_num; i++) {
      /* Skip outputs. */
      if (recs[i]->rec_type != BOT_TYPE) {
        continue;
      }
      if (part_one_winner(recs[i])) {
        printf("%d\n", recs[i]->id);
      }
      /* Continue while transfers are still being performed. */
      something_happened = (init_transfer(recs[i]) || something_happened);
    }
  }
}

/* Multiply the values of chips in outputs 0, 1, and 2. */
void part_two(void) {
  int product = 1;
  for (int i = 0; i < 3; i++) {
    chip c = select_output(i)->chip_one;
    product *= c;
  }
  printf("%d\n", product);
}

int main(int argc, const char *argv[]) {
  FILE *f = read_input(argc, argv);
  char buffer[BUFSIZ];
  setbuf(f, buffer);

  init_receivers();

  part_one(f);
  part_two();

  /* Cleanup. */
  fclose(f);
  for (uint i = 0; i < rec_num; i++) {
    free(recs[i]);
  }
  free(recs);
  exit(EXIT_SUCCESS);
}
