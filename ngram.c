/*
  Name: Shreyas Agrawal (Shrey)
  Date: 6th Sept 2021
  N-Gram generator program written in C
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  // Make sure the user provided an N parameter on the command line
  if (argc != 2) {
    fprintf(stderr, "Usage: %s N (N must be >= 1)\n", argv[0]);
    exit(1);
  }

  // Convert the N parameter to an integer
  int N = atoi(argv[1]);

  // Make sure N is >= 1
  if (N < 1) {
    fprintf(stderr, "Invalid N value %d\n", N);
    exit(1);
  }

  // Reading from standard input and print out ngrams until you reach the end of the input
  // N is the number given by the user for how many chars you want on each line

  // Allocating a dynamic array for the size of N
  char* arr = (char*)malloc(N * sizeof(char) + 1);
  char c;

  // Read N number of chars from the input
  fgets(arr, N + 1, stdin);

  // making the last index of the array the null terminator
  arr[N] = '\0';
  if (strlen(arr) == N) {
    printf("%s\n", arr);
  }
  // The program exits here, and doesn't go into while loop if length of input = N

  // While loop which checks if reached EOF, if not then rearrange array according to ngram.
  while ((c = fgetc(stdin)) != EOF) {
    for (int j = 0; j < N; j++) {
      arr[j] = arr[j + 1];
    }
    // Adding new char to end of array
    arr[N - 1] = c;
    // Printing the entire array
    printf("%s\n", arr);
  }

  free(arr);

  return 0;
}
