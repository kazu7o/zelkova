#ifndef COMMON_H_
#define COMMON_H_

typedef int KEY;
typedef struct _NODE {
  struct _NODE *left;
  struct _NODE *right;
  KEY data;
  int passnum;
} NODE;

NODE *malloc_fnode(KEY key);
void error(char *mes);
int max(int a, int b);

#endif