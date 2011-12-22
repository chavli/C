#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/util.h"
#include "../headers/bintree.h"


int main(int argc, char *argv[]){
  printf("%s\n", (2 > 1) ? "hello" : "world");
 
  /* 
  int *test = malloc(sizeof(int));
  int j = 33;
  *test = j;

  for(j = 0; j < 33; ++j){
    *test = j;
    printf("%p, %p, %i\n", &test, test, *test);
    memview(test, sizeof(int));
    printf("------\n");
  }

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

  BinaryNode *root = 0; 

  int i = 10;
  bintree_insert(&root, &i, 0);
  printf("%p\n", root);

  i = 20;
  bintree_insert(&root, &i, 0);
  printf("%p\n", root);

  i = 5;
  bintree_insert(&root, &i, 0);
  printf("%p\n", root);

  i = 7;
  bintree_insert(&root, &i, 0);
  printf("%p\n", root);

  bintree_inorder(root, 0);
  printf("%s\n", "done");
}
