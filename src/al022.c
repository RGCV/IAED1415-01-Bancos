/*******************************************************************************
**----------------------------------------------------------------------------**
*'                1st IAED project | 2nd semester - 2014/2015                 '*
*'                       BANK NETWORK MANAGEMENT TERMINAL                     '*
*'     Group: 81045-Rui Ventura; 81338-Pedro Cerejo; 81670-Joao Oliveira      '*
**----------------------------------------------------------------------------**
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Bank rating. */
enum bank_rating {
  /** Represents a unqualified ("bad") bank. */
  UNQUALIFIED = 0,
  /** Represents a qualified ("good") bank. */
  QUALIFIED
};
typedef enum bank_rating bank_rating_t;

bank_rating_t rating_from_int(int i) {
  return i == 0 ? UNQUALIFIED : QUALIFIED;
}

/** Maximum number of registries in the bank network. */
#define MAX_BANKS 1000

/** Bank name maximum length (40 chars + 1 '\0'). */
#define MAX_NAME_LEN 41

/** A Bank. */
struct bank {
  /** Bank reference identifier. */
  int id;
  /** Bank's name. */
  char name[MAX_NAME_LEN];
  /** Bank's rating. */
  bank_rating_t rating;
  /** Number of partners with credit. */
  int inP;
  /** Number of partners with debt. */
  int outP;
  /** Total amount loaned. */
  int outV;
  /** Amount loaned to unqualified banks. */
  int outVM;
  /** Total amortized amount. */
  int inV;
  /** Amount amortized from unqualified banks. */
  int inVM;
};
typedef struct bank bank_t;

int find_bank(int id);
int qualified_banks();

void add_bank();
void loan();
void disqualify_worst();
void disqualify_bank();
void qualify_bank();
void listing();
void amortize();

/** Vector of registered banks. */
bank_t g_banks[MAX_BANKS];
/** Total number of banks. */
int g_banks_size;
/** Transaction adjacency transactions. */
int transactions[MAX_BANKS][MAX_BANKS];

int main() {
  int command;

  while (1) {
    switch (command = getchar()) {
      case 'a': add_bank();         break;
      case 'e': loan();             break;
      case 'K': disqualify_worst(); break;
      case 'k': disqualify_bank();  break;
      case 'r': qualify_bank();     break;
      case 'l': listing();          break;
      case 'p': amortize();         break;
      case 'x':
      case EOF:
        printf("%d %d\n", g_banks_size, qualified_banks());
        return EXIT_SUCCESS;
      default:
        fprintf(stderr, "Error: unrecognized command '%c'\n", command);
    }

    while((command = getchar()) != '\n' && command != EOF && command != 0);
  }

  return EXIT_FAILURE;
}

int qualified_banks() {
  int i = 0, qualified = 0;
  while (i < g_banks_size) {
    if (g_banks[i++].rating == QUALIFIED) ++qualified;
  }
  return qualified;
}

int find_bank(int id) {
  int i;
  for (i = 0; i < g_banks_size; ++i) {
    if (g_banks[i].id == id) return i;
  }
  return -1;
}

void add_bank() {
  int i, id;
  short rating;
  char name[MAX_NAME_LEN];

  if (g_banks_size == MAX_BANKS) {
    fprintf(stderr, "Error: couldn't add bank. Number of registries maxed.\n");
    return;
  }

  if (scanf("%s%hd%d", name, &rating, &id) != 3) {
    perror("scanf");
    return;
  }

  for (i = 0; i < g_banks_size; ++i) {
    if (g_banks[i].id == id) {
      fprintf(stderr, "Error: bank with id '%d' already exists.\n", id);
      break;
    }
    if (!strncmp(g_banks[i].name, name, MAX_NAME_LEN)) {
      fprintf(stderr, "Error: bank '%s' already exists.\n", g_banks[i].name);
      break;
    }
  }

  if (i == g_banks_size) {
    g_banks[g_banks_size].id = id;
    g_banks[g_banks_size].rating = rating_from_int(rating);
    strncpy(g_banks[g_banks_size++].name, name, MAX_NAME_LEN);
  }
}

void qualify_bank() {
  int i, id;

  if (scanf("%d", &id) != 1) {
    perror("scanf");
    return;
  }

  if ((i = find_bank(id)) == -1) {
    fprintf(stderr, "Error: no bank with id '%d' exists.\n", id);
    return;
  }

  if (g_banks[i].rating != QUALIFIED) {
    int j;
    for (j = 0; j < MAX_BANKS; j++) {
      g_banks[j].inVM  -= transactions[i][j];
      g_banks[j].outVM -= transactions[j][i];
    }
    g_banks[i].rating = QUALIFIED;
  }
}

void disqualify_bank() {
  int i, id;

  if (scanf("%d", &id) != 1) {
    perror("scanf");
    return;
  }

  if ((i = find_bank(id)) == -1) {
    fprintf(stderr, "Error: bank with id '%d' not found.\n", id);
    return;
  }

  if (g_banks[i].rating != UNQUALIFIED) {
    int j;
    for (j = 0; j < MAX_BANKS; j++){
      g_banks[j].inVM  += transactions[i][j];
      g_banks[j].outVM += transactions[j][i];
    }
    g_banks[i].rating = UNQUALIFIED;
  }
}

void disqualify_worst() {
  int i, wv, wi = -1;

  for (i = wv = 0; i < g_banks_size; ++i) {
    if (g_banks[i].rating == QUALIFIED) {
      int v = g_banks[i].outVM;
      if (v >= wv) wv = g_banks[wi = i].outVM;
    }
  }

  if (wv > 0) {
    int j;
    g_banks[wi].rating = UNQUALIFIED;

    for (j = 0; j < g_banks_size; j++){
      g_banks[j].inVM  += transactions[wi][j];
      g_banks[j].outVM += transactions[j][wi];
    }

    printf("*%d %s %d %d %d %d %d %d %d\n",
      g_banks[wi].id, g_banks[wi].name, (int)g_banks[wi].rating,
      g_banks[wi].inP, g_banks[wi].outP, g_banks[wi].outV, g_banks[wi].outVM,
      g_banks[wi].inV, g_banks[wi].inVM);
  }

  printf("%d %d\n", g_banks_size, qualified_banks());
}

void listing() {
  short mode;

  if (scanf("%hd", &mode) != 1) {
    perror("scanf");
    return;
  }

  switch (mode) {
    case 0:
    case 1: {
        int i;
        for (i = 0; i < g_banks_size ; i++) {
          bank_t bank = g_banks[i];
          printf("%d %s %d", bank.id, bank.name, (int)bank.rating);
          if (mode) {
            printf(" %d %d %d %d %d %d",
              bank.inP, bank.outP, bank.outV, bank.outVM, bank.inV, bank.inVM);
          }
          printf("\n");
        }
      }
      break;
    case 2: {
        int i, g_banks_hist[MAX_BANKS] = {0};

        for (i = 0; i < g_banks_size; i++) {
          int j, partners = 0;
          for (j = 0; j < g_banks_size; j++) {
            if (i != j && (transactions[i][j] > 0 || transactions[j][i] > 0)) {
              ++partners;
            }
          }
          ++g_banks_hist[partners];
        }

        for(i = 0; i < g_banks_size ; i++) {
          if (g_banks_hist[i] != 0) printf("%d %d\n", i, g_banks_hist[i]);
        }
      }
      break;
    default:
      fprintf(stderr, "Error: unknown listing mode.\n");
  }
}

void loan() {
  int i = -1, j = i, k;
  int id_src, id_dst, amount;

  if (scanf("%d%d%d", &id_src, &id_dst, &amount) != 3) {
    perror("scanf");
    return;
  }

  if (amount < 0) {
    fprintf(stderr, "Error: invalid amount (%d)\n", amount);
    return;
  }

  if (amount == 0) return;

  for (k = 0; k < g_banks_size && (i < 0 || j < 0); k++) {
    if (g_banks[k].id == id_src) i = k;
    if (g_banks[k].id == id_dst) j = k;
  }

  if (i == j) {
    fprintf(stderr, "Error: the id's are the same.\n");
    return;
  }

  if (i == -1 || j == -1) {
    fprintf(stderr, "Error: invalid id(s).\n");
    return;
  }

  if (transactions[i][j] == 0) {
    ++g_banks[i].outP;
    ++g_banks[j].inP;
  }
  transactions[i][j] += amount;

  g_banks[i].outV += amount;
  if (g_banks[j].rating == UNQUALIFIED) g_banks[i].outVM += amount;

  g_banks[j].inV += amount;
  if (g_banks[i].rating == UNQUALIFIED) g_banks[j].inVM += amount;
}

void amortize() {
  int i = -1, j = i, k;
  int id_src, id_dst, amount;

  if (scanf("%d%d%d", &id_src, &id_dst, &amount) != 3) {
    perror("scanf");
    return;
  }

  if (amount < 0) {
    fprintf(stderr, "Error: invalid amount (%d)\n", amount);
    return;
  }

  if (amount == 0) return;

  for (k = 0; k < g_banks_size && (i < 0 || j < 0); k++) {
    if (g_banks[k].id == id_src) j = k;
    if (g_banks[k].id == id_dst) i = k;
  }

  if (i == j) {
    fprintf(stderr, "Error: the id's are the same.\n");
    return;
  }

  if (i == -1 || j == -1) {
    fprintf(stderr, "Error: invalid id(s).\n");
    return;
  }

  if (transactions[i][j] < amount) {
    fprintf(stderr, "Error: amount must be lesser than or equal to that of the "
                    "debt.\n");
    return;
  }

  transactions[i][j] -= amount;
  if (transactions[i][j] == 0) {
    g_banks[i].outP--;
    g_banks[j].inP--;
  }

  g_banks[i].outV -= amount;
  if (g_banks[j].rating == UNQUALIFIED) g_banks[i].outVM -= amount;

  g_banks[j].inV -= amount;
  if (g_banks[i].rating == UNQUALIFIED) g_banks[j].inVM -= amount;
}
