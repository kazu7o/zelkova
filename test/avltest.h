#ifndef FTREE_H_
#define FTREE_H_

#include "common.h"

typedef struct _F_NODE {
  struct _F_NODE *left;
  struct _F_NODE *right;
  KEY data;
  int passnum;
} F_NODE;

int get_height_ftree(F_NODE *root);
F_NODE *insert_fnode(F_NODE *root, KEY key);
void clear_ftree(F_NODE *root);

#endif