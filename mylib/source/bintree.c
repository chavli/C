#include <stdio.h>
#include <stdlib.h>
#include "../headers/bintree.h"


BinaryNode *bintree_init(){
  BinaryNode *root = malloc(sizeof(BinaryNode));
  return root;
}

BinaryNode *bintree_insert(BinaryNode **root, void *data, int (*comp)(void *, void *)){
  BinaryNode **next;

  //base case, root is null
  if(*root == 0){
    *root = malloc(sizeof(BinaryNode));
    (*root)->data = data;
    return *root;
  }
  else{
    //if a comparison function is given, use it.
    //otherwise just compare the values at the addresses
    if(comp)
      *next = ( comp((*root)->data, data ) > 0 ) ? (*root)->right : (*root)->left;
    else  //omg pointer madness
      *next = ( *(int *)((*root)->data) > *(int *)(data) ) ? (*root)->right : (*root)->left;

    return bintree_insert(next, data, comp);
  }
}

void bintree_remove(BinaryNode **root, void *data, int (*comp)(void *, void *)){

}

void bintree_inorder(BinaryNode *root, char *(* stringify)(void *)){
  if(root == 0){
    printf("%s\n", "X");
    return;
  }

  printf("%s\n", "A");
  bintree_inorder(root->left, stringify);
  //use the given stringify function if possible 
  if(stringify)
    printf("##%s\n", stringify(root->data));
  else
    printf(">>%d\n", *(int *)(root->data));
  bintree_inorder(root->right, stringify);
  printf("%s\n", "D");
}


