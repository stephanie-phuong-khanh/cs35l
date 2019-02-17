#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
      if (from[i] == from[j]) {
	fprintf(stderr, "Cannot have duplicate characters in from argument.\n");
	exit(1);
      }
    }
  }

  //ssize_t read(int fildes, void *buf, size_t nbyte);
  //ssize_t write(int fildes, const void *buf, size_t nbyte);
  //nbyte = 1 always
  char buf;  
  while (1) {
    int read_ret = read(0, &buf, 1);
    if (read_ret < 0) {
      fprintf(stderr, "Error reading from input.\n");
      exit(1);
    } else if (read_ret == 0) {
      break;  //end of file
    }
    //Check for replacement
    int n;
    int replaced = 0;
    for (n=0; from[n] != '\0'; n++) {
      if (buf == from[n]) {
	buf = to[n];
	break;
      }
    }
    int write_ret = write(1, &buf, 1);
    if (write_ret == -1) {
      fprintf(stderr, "Error writing to output.\n");
      exit(1);
    }
  }

  return 0;
}
