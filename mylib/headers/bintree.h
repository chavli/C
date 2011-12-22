#ifndef BINNODE_H
#define BINNODE_H

typedef struct binnode BinaryNode;

struct binnode{
  void *data;
  BinaryNode *left;
  BinaryNode *right;
};

//initialize the binary tree. returns the root node
BinaryNode *bintree_init(void);

//insert and remove functions for the binary tree. the arguments are:
//the root of the tree
//the value to be inserted (NOT A NODE)
//the comparison function
//
//the functions will handle manipulating binary nodes
BinaryNode *bintree_insert(BinaryNode **, void *, int (*)(void *, void *));
void bintree_remove(BinaryNode **, void *, int (*)(void *, void *));

//functions for printing the values contained in the binary tree
//a pointer to a stringify function is needed
void bintree_inorder(BinaryNode *, char * (*)(void *));
void bintree_preorder(BinaryNode *, char * (*)(void *));
void bintree_postorder(BinaryNode *, char * (*)(void *));

//the toString function
char * bintree_stringify(BinaryNode *);

#endif
