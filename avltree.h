#ifndef AVLTREE_H_
#define AVLTREE_H_

#include "common.h"

int getHeight(NODE *root);
int getminDepth(NODE *root);
NODE *insert(NODE *root, KEY key);
void clearTree(NODE *root);
void dumpTree(NODE *node, NODE *par);
void fdumpTree(NODE *node, NODE *par, FILE *of);
void preorder(NODE *);

extern int rotations;

#endif