#ifndef COMMON_H_
#define COMMON_H_
#include <stdio.h>

typedef int KEY;
typedef struct _NODE {
  struct _NODE *left;
  struct _NODE *right;
  KEY data;
  int passnum;
  pthread_mutex_t mutex;
  struct _NODE *rotated;
} NODE;

void error(char *mes);
int max(int a, int b);
int getHeight(NODE *root);
int getminDepth(NODE *root);
NODE *malloc_node(KEY key);
void clearTree(NODE *root);
void dumpTree(NODE *node, NODE *par, FILE *of);
NODE *search(NODE *root, KEY key);
void node_mutex_destroy(NODE *p);

#endif