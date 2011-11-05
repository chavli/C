#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"


int main(int argc, char *argv[]){
  int *test = malloc(sizeof(int));
  int j;

  for(j = 0; j < 33; ++j){
    *test = j;
    printf("%p, %p, %i\n", &test, test, *test);
    memview(test, sizeof(int));
    printf("------\n");
  }
  

  /*

  char *word = malloc(24 * sizeof(char));
  word = "hello world";

  char *word2 = malloc(24 * sizeof(char));
  word2 = "goodbye world";

  printf("%p, %p, %i\n", &test, test, *test);
  printf("%p, %p, %i\n", &word, word, *word);
  printf("%p, %p, %i\n", &word2, word2, *word2);
  memview(test, 100);
  memview(word, 200);
  memview(word2, 200);
  */
}
