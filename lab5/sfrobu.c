#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

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

int compare(const void* word1, const void* word2) {
  return frobcmp(*((char**) word1), *((char**) word2));
}

int frobcmp_f(char const *a, char const*b) {
  while (1) {
    char a_decrypted = *a ^ 42;
    char b_decrypted = *b ^ 42;

    //toupper each byte before comparison
    if (a_decrypted <= UCHAR_MAX && a_decrypted >= 0)
      a_decrypted = toupper(a_decrypted);
    if (b_decrypted <= UCHAR_MAX && b_decrypted >= 0)
      b_decrypted = toupper(b_decrypted);

    if (*a == ' ' && *b == ' ')
      return 0;  //reached end of word with both being lexicographically equal                                                      
    else if (*a == ' ' || a_decrypted < b_decrypted)
      return -1;  //a is a prefix of b, or a is lexicographically less than b                                                       
    else if (*b == ' ' || a_decrypted > b_decrypted)
      return 1;  //b is a prefix of a, or b is lexicrographically less than a                                                       
    a++; b++;
  }
}

int compare_f(const void* word1, const void* word2) {
  return frobcmp_f(*((char**) word1), *((char**) word2));
}

void readErrCheck(int status) {
  if (status < 0) {
    char err[] = "Error reading file\n";
    write(2, &err, sizeof(err));
    exit(1);
  }
}

void memAllocCheck(void *ptr) {
  if (ptr == NULL) {
    char err[] = "Error allocating memory\n";
    write(2, &err, sizeof(err));
    exit(1);
  }
}

int main(int argc, char** argv) {
  //parse arguments
  if (argc > 2) {
    char err[] = "Too many arguments\n";
    write(2, &err, sizeof(err));
    exit(1);
  }

  int ignoreCase = 0; //false unless -f flag
  if (argc==2 && argv[1][0]=='-' && argv[1][1]=='f')
    ignoreCase = 1;

  //read file
  struct stat finfo;
  if (fstat(0, &finfo) < 0) {
    char err[] = "Error opening file";
    write(2, &err, sizeof(err));
    exit(1);
  }

  int fileSize = finfo.st_size;
  char *fileArr;
  char **wordsArr;
  int wordCount = 0;

  if (!S_ISREG(finfo.st_mode)) { 
    //allocate space if file opens unsuccessfully
    wordsArr = (char**)malloc(sizeof(char*));
  } else {  //successful open
    fileArr = (char*) malloc(fileSize * sizeof(char));
    readErrCheck(read(0, fileArr, fileSize));

    //Count words in file therefore word pointers needed
    int i = 0;
    if (fileSize>0 && fileArr[fileSize-1] != ' ') {
      fileArr[fileSize-1] = ' ';
    }
    while (i<fileSize) {
      if (fileArr[i] == ' ') {
	if (i == 0) {
	  i++;
	  while(fileArr[i] == ' ' && i<fileSize)
	    i++;
	} else {
	  wordCount++; //printf("INCREMENT\n");
     	  i++;
	  while(fileArr[i] == ' ' && i<fileSize)
	    i++;
	}
      } else {
	//printf("Letter: "); printchar(fileArr[i]); printf("\n");
	i++;
      }
    }

    wordsArr = (char**) malloc(wordCount * sizeof(char*));
    int readingWord = 0; //initially false
    int wordsArrIndex = 0;
    int fileIndex;
    for (fileIndex=0; fileIndex<fileSize && wordsArrIndex<wordCount; fileIndex++) {
      if (!readingWord && fileArr[fileIndex] != ' ') {
	readingWord = 1; //now reading word
	wordsArr[wordsArrIndex] = &fileArr[fileIndex];
	wordsArrIndex++;
      } else if (readingWord && fileArr[fileIndex] == ' ') {
	readingWord = 0; //word has ended
      }
    } 
  }

  //allocate more memory if file grows as we read it
  int letterIndex = 0;

  char *word = (char*) malloc(sizeof(char));
  char currChar;
  int currReadStat = read(0, &currChar, 1);
  readErrCheck(currReadStat);
  char nextChar;
  int nextReadStat = read(0, &nextChar, 1);
  readErrCheck(nextReadStat);

  while (currReadStat > 0) { //not end of file
    int isEndWord = 0;

    word[letterIndex] = currChar;
    char *tempWord = realloc(word, (letterIndex + 2)*sizeof(char));
    memAllocCheck(tempWord);
    word = tempWord;

    if (currChar == ' ') {
      isEndWord = 1; //denote end of word
      char **tempWordsArr = realloc(wordsArr, (wordCount + 1)*sizeof(char*));
      memAllocCheck(tempWordsArr);
      wordsArr = tempWordsArr;
      wordsArr[wordCount] = word;
      wordCount++;
      word = (char*) malloc(sizeof(char));
      letterIndex = 0;
	
      //File ends in space -> exit file scanning loop
      if (nextReadStat == 0)
	break;

      //Contiguous spaces -> continue reading until nonspace character
      if (nextChar == ' ') {
	while (currChar == ' ') {
	  currReadStat = read(0, &currChar, 1);
	  readErrCheck(currReadStat);
	  if (currReadStat == 0)
	    break;  //end of file
	}
	nextReadStat = read(0, &nextChar, 1);
	readErrCheck(nextReadStat);
	letterIndex++;
	continue;
      }
    } else if (nextReadStat == 0) { //end of file w/o trailing space
      currChar = ' ';
      letterIndex++;
      continue;
    }

    //continue scanning
    if (!isEndWord)
      letterIndex++;
    currChar = nextChar;
    nextReadStat = read(0, &nextChar, 1);
    readErrCheck(nextReadStat);
  }

  if (ignoreCase)
    qsort(wordsArr, wordCount, sizeof(char*), compare_f);
  else
    qsort(wordsArr, wordCount, sizeof(char*), compare);

  //print
  int x;
  for (x=0; x<wordCount; x++) {
    int y;
    for (y=0; ; y++) {
      if (write(1, &wordsArr[x][y], 1) < 0) {
	char err[] = "Error writing to standard output\n";
	write(2, &err, sizeof(err));
	exit(1);
      }
      if (wordsArr[x][y] == ' ')
	break; //end of word
    }
  }

  //free all memory
  free(word);
  free(wordsArr);
  free(fileArr);
  exit(0);
}
