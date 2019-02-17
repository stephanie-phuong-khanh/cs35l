#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int InputErrCheck() {
  if (ferror(stdin)) {
    fprintf(stderr, "Error reading from standard input.\n");
    exit(1);
  }
}

int OutputErrCheck() {
  if (ferror(stdout)) {
    fprintf(stderr, "Error writing to standard output.\n");
    exit(1);
  }
}

int main(int argc, char** argv) {
  //Error if more or less than two string arguments
  if (argc != 3) {
    fprintf(stderr, "Must have two string arguments.\n");
    exit(1);
  }

  char* from = argv[1];
  char* to = argv[2];

  //String lengths must be same
  if (strlen(from) != strlen(to)) {
    fprintf(stderr, "Must have string arguments of same length.\n");
    exit(1);
  }

  //Check for duplicates in from - brute force comparison
  int i;
  int length = strlen(from);
  for (i=0; i<length; i++) {
    int j;
    for (j=i+1; j<length; j++) {
      if (from[j] == from[i]) {
	fprintf(stderr, "Cannot have duplicate characters in from argument.\n");
	exit(1);
      }
    }
  }

  char curr = getchar();
  InputErrCheck();
  if (curr == EOF) //Exit if empty file
    exit(0);

  while(curr != EOF) {
    int n;
    int replaced = 0; //False by default
    for (n=0; from[n]!='\0'; n++) {
      if (curr == from[n]) {
	putchar(to[n]);
	OutputErrCheck();
	replaced = 1;
	break;
      }
    }
    if (!replaced) {
      putchar(curr);
      OutputErrCheck();
    }
    curr = getchar();
    InputErrCheck();
  }

  return 0;
}


