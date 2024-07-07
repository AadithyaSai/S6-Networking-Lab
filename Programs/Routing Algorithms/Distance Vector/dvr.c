#include <stdio.h>

// Program to implement basic DVR routing

void print_table(int table[][256], int n);

int main() {
  int n;
  int routing_table[256][256];

  printf("Enter number of routers: ");
  scanf("%d", &n);

  for (int i = 0; i < n; i++) {
    printf("Enter initial table for router %d: ", i);
    for (int j = 0; j < n; j++) {
      scanf("%d", &routing_table[i][j]);
    }
  }

  char choice;
  int i = 0;
  int prev, next;
  do {
    for (int j = 0; j < n; j++) {
      prev = j != 0 ? j - 1 : n - 1;
      next = (j + 1) % n;
      for (int k = 0; k < n; k++) {
        if (routing_table[j][k] >
            routing_table[j][prev] + routing_table[prev][k]) {
          routing_table[j][k] = routing_table[j][prev] + routing_table[prev][k];
        } else if (routing_table[j][k] >
                   routing_table[j][next] + routing_table[next][k]) {
          routing_table[j][k] = routing_table[j][next] + routing_table[next][k];
        }
      }
    }
    i++;
    printf("Iteration - %d\n", i);
    print_table(routing_table, n);

    printf("Do you want to continue?(y/n): ");
    scanf(" %c", &choice);
  } while (choice != 'n');

  return 0;
}

void print_table(int table[][256], int n) {
  for (int i = 0; i < n; i++) {
    printf("|");
    for (int j = 0; j < n; j++) {
      printf(" %d ", table[i][j]);
    }
    printf("|\n");
  }
}
