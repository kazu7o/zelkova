#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "avltree.h"

int rotations = 0;

/*
  error -- エラーメッセージを表示する
    mes: 表示するエラーメッセージ
*/
void error(char *mes) {
  fprintf(stderr, "\nERROR: %s\n", mes);
  exit(1);
}

/*
  max -- 大きいほうの値を返す
    a: 比較する値1
    b: 比較する値2
*/
int max(int a, int b) {
  return a > b ? a : b;
}

/*
  getHeight -- 木の高さを取得する
               空の木の高さは-1とする
    root: 部分木の根ノード
*/
int getHeight(NODE *root) {
  int left_height;
  int right_height;

  if (root == NULL) {
    return -1;
  }
  left_height = getHeight(root->left);
  right_height = getHeight(root->right);
  return max(left_height, right_height) + 1;
}

int getminDepth(NODE *root) {
  if (root == NULL) {
    return -1;
  }
  if (root->left == NULL && root->right == NULL) {
    return 0;
  }
  int l = INT_MAX;
  int r = INT_MAX;

  if (root->left != NULL) {
    l = getminDepth(root->left);
  }
  if (root->right != NULL) {
    r = getminDepth(root->right);
  }
  return ((l > r) ? r : l) + 1;
}

/*
  R_Rotate -- 右回転
    root: 回転する部分木の根ノード
*/
static NODE *R_Rotate(NODE *root) {
  NODE *pivot = root->left;
  root->left = pivot->right;
  pivot->right = root;

  rotations++;

  return pivot;
}

/*
  L_Rotate -- 左回転
    root: 回転する部分木の根ノード
*/
static NODE *L_Rotate(NODE *root) {
  NODE *pivot = root->right;
  root->right = pivot->left;
  pivot->left = root;

  rotations++;

  return pivot;
}

/*
  LR_Rotate -- 左->右回転
    root: 回転する部分木の根ノード
*/
static NODE *LR_Rotate(NODE *root) {
  root->left = L_Rotate(root->left);
  return R_Rotate(root);
}

/*
  RL_Rotate -- 右->左回転
    root: 回転する部分木の根ノード
*/
static NODE *RL_Rotate(NODE *root) {
  root->right = R_Rotate(root->right);
  return L_Rotate(root);
}

/*
  insert -- AVL木に要素を挿入する（再帰的）
    root: 挿入する部分木の根ノード
    key: 挿入するデータ
*/
NODE *insert(NODE *root, KEY key) {
  NODE *p;
  NODE *new;
  int balance;

  p = root;
  if (p == NULL) {
    if ((new = (NODE *)malloc(sizeof(NODE))) == NULL) {
      error("out of memory!");
    }
    new->left   = NULL;
    new->right  = NULL;
    new->data   = key;
    new->passnum = 1;
    p = new;
    return p;
  } else if (key < p->data) {
    p->passnum++;
    p->left = insert(p->left, key);
  } else {  // key >= p->data
    p->passnum++;
    p->right = insert(p->right, key);
  }
  
  // 平衡化
  balance = getHeight(p->left) - getHeight(p->right);
  if (balance >= 2) { // 左部分木が高く、非平衡
    if (getHeight(p->left->left) > getHeight(p->left->right)) { // Left Left Case
      p = R_Rotate(p);
    } else {  // Left Right Case
      p = LR_Rotate(p);
    }
  } else if (balance <= -2) { // 右部分木が高く、非平衡
    if (getHeight(p->right->left) > getHeight(p->right->right)) { // Right Left Case
      p = RL_Rotate(p);
    } else {  // Right Right Case
      p = L_Rotate(p);
    }
  }
  return p;
}

/*
  clearTree -- 木に含まれるノードを全て削除する
    root: 削除する木の根ノード
*/
void clearTree(NODE *root) {
  if (root == NULL) {
    return;
  }
  if (root->left != NULL) {
    clearTree(root->left);
  }
  if (root->right != NULL) {
    clearTree(root->right);
  }
  free(root);
}

/*
  dumpTree -- Graphvizに読み込ませる木構造をDOT言語のスクリプトで出力
    node: 表示する木の根ノード
    par: nodeの親ノード
*/
void dumpTree(NODE *node, NODE *par) {
  static int i = 0;
  if (node == NULL) {
    printf("%d%d[label=%d];\n", par->data, par->passnum, par->data);
    printf("nullNode%d[style=invis];\n", i);
    printf("%d%d -> nullNode%d[style=invis];\n", par->data, par->passnum, i++);
    return;
  }
  if (par == NULL) {
    printf("digraph G{\n");
    printf("graph [ordering=out];\n");
  } else {
    printf("%d%d[label=%d];\n", par->data, par->passnum, par->data);
    printf("%d%d -> %d%d", par->data, par->passnum, node->data, node->passnum);
    if (node->data >= par->data) {
      printf("[style=dotted]");
    }
    printf(";\n");
  }
  dumpTree(node->left, node);
  dumpTree(node->right, node);
  if (par == NULL) {
    printf("}\n");
  }
}

/*
  fdumpTree -- Graphvizに読み込ませる木構造をDOT言語のスクリプトでファイルに出力
    node: 表示する木の根ノード
    par: nodeの親ノード

*/
void fdumpTree(NODE *node, NODE *par, FILE *of) {
  static int i = 0;

  if (node == NULL) {
    fprintf(of, "%d%d[label=%d];\n", par->data, par->passnum, par->data);
    fprintf(of, "nullNode%d[style=invis];\n", i);
    fprintf(of, "%d%d -> nullNode%d[style=invis];\n", par->data, par->passnum, i++);
    return;
  }
  if (par == NULL) {
    fprintf(of, "digraph G{\n");
    fprintf(of, "graph [ordering=out];\n");
  } else {
    fprintf(of, "%d%d[label=%d];\n", par->data, par->passnum, par->data);
    fprintf(of, "%d%d -> %d%d", par->data, par->passnum, node->data, node->passnum);
    if (node->data > par->data) {
      fprintf(of, "[style=dotted]");
    }
    fprintf(of, ";\n");
  }
  fdumpTree(node->left, node, of);
  fdumpTree(node->right, node, of);
  if (par == NULL) {
    fprintf(of, "}\n");
    fclose(of);
  }
}