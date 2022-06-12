#ifndef AVLTREE_H_
#define AVLTREE_H_

typedef int KEY;

typedef struct _NODE {
  struct _NODE *left;
  struct _NODE *right;
  KEY data;
} NODE;

void error(char *mes);
int getHeight(NODE *root);
NODE *R_Rotate(NODE *root);
NODE *L_Rotate(NODE *root);
NODE *LR_Rotate(NODE *root);
NODE *RL_Rotate(NODE *root);
NODE *insert(NODE *root, KEY key);
void clearTree(NODE *root);

#endif