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

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
        if (routing_table[j][k] >
            routing_table[j][i] + routing_table[i][k]) {
          routing_table[j][k] = routing_table[j][i] + routing_table[i][k];
        }
      }
    }
  }
  printf("Result\n");
  print_table(routing_table, n);

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
