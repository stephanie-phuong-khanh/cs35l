#include <stdio.h>
#include <stdlib.h>

int frobcmp (char const * a, char const * b) {
  while (1) {
    if (*a == ' ' && *b == ' ')
      return 0;  //reached end of word with both being lexicographically equal
    else if (*a == ' ' || (*a ^ 42) < (*b ^ 42))
      return -1;  //a is a prefix of b, or a is lexicographically less than b
    else if (*b == ' ' || (*a ^ 42) > (*b ^ 42))
      return 1;  //b is a prefix of a, or b is lexicrographically less than a
    a++; b++;
  }
  return 0;
}


int compareWithFrob(const void* word1, const void* word2) {
  return frobcmp(*((char**) word1), *((char**) word2));
}


void IOErrCheck() {
  if (ferror(stdin)) {
    fprintf(stderr, "Error reading input from file.");
    exit(1);
  }
}


int main(int argc, char** argv) {
  //Array to hold letters of the word currently being read
  char *lettersArr = (char*) malloc(sizeof(char));

  //Array of words read
  char **wordsArr = (char**) malloc(sizeof(char*));

  if (lettersArr == NULL || wordsArr == NULL) {
    fprintf(stderr, "Memory allocation error.");
    exit(1);
  }

  int letterIndex = 0;
  int wordIndex = 0;
  
  char currChar = getchar();
  IOErrCheck();
  char nextChar = getchar();
  IOErrCheck();

  if (currChar == EOF) 
    exit(0); //Exit if empty file

  while (currChar != EOF) {
    int isEndWord = 0; //False until changed true
    
    lettersArr[letterIndex] = currChar;
    //Allocate more space for more characters in word
    char *tempLettersArr = realloc(lettersArr, (letterIndex + 2)*sizeof(char));
    if(tempLettersArr == NULL) {
      fprintf(stderr, "Memory allocation error.");
      exit(1);
    }
    lettersArr = tempLettersArr;

    //End of word, add word to wordsArr array, free lettersArray memory
    if (currChar == ' ') {
      isEndWord = 1;	//Denote end of word
      char **tempWordsArr = realloc(wordsArr, (wordIndex + 1)*sizeof(char*));
      if (tempWordsArr == NULL) {
	fprintf(stderr, "Memory allocation error.");
	exit(1);
      }
      wordsArr = tempWordsArr;
      wordsArr[wordIndex] = lettersArr;
      wordIndex++;
      lettersArr = (char*) malloc(sizeof(char)); //Allocate new memory for next word
      letterIndex = 0;

      //File ends in space -> exit file scanning loop
      if (nextChar == EOF)
	break;

      //Contiguous spaces -> continue reading until nonspace character
      if (nextChar == ' ') {
	while (currChar == ' ') {
	  currChar = getchar();
	  IOErrCheck();
	  if (currChar == EOF)
	    break;
	}
	nextChar = getchar();
	IOErrCheck();
	letterIndex++;
	continue;
      }
    } else if (nextChar == EOF) { //End of file without trailing space
      currChar = ' ';
      letterIndex++;
      continue;
    }

    //Continue scanning file
    currChar = nextChar;
    nextChar = getchar();
    IOErrCheck();
    if (!isEndWord)
    	letterIndex++;
  }

  qsort(wordsArr, wordIndex, sizeof(char*), compareWithFrob);

  //Print
  int i;
  for (i=0; i<wordIndex; i++) {
    int j;
    for (j=0; ; j++) {
      putchar(wordsArr[i][j]);
      if (ferror(stdout)) {
	fprintf(stderr, "Error writing output.");
	exit(1);
      }
      if (wordsArr[i][j] == ' ')
	break;  //end of word
    }
  } 


  //Free all memory
  free(lettersArr);
  int n;
  for (n=0; n<wordIndex; n++)
    free(wordsArr[n]);
  free(wordsArr);
  exit(0);
}

