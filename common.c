#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include "common.h"

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

/*
  getminDepth -- 根から葉までの経路のうち最短の経路長を取得する
    root: 部分木の根ノード
*/
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
  malloc_fnode -- 新しいノードに必要な領域を確保する
    key: ノードのデータ
*/
NODE *malloc_node(KEY key) {
  NODE *new;
  if ((new = (NODE *)malloc(sizeof(NODE))) == NULL) {
    error("out of memory!");
  }
  new->left    = NULL;
  new->right   = NULL;
  new->data    = key;
  new->passnum = 1;
  pthread_mutex_init(&new->mutex, NULL);
  new->rotated = NULL;
  return new;
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
  dumpTree -- Graphvizに読み込ませる木構造をDOT言語のスクリプトでファイルに出力
    node: 表示する木の根ノード
    par: nodeの親ノード
    of: 出力先のファイルポインタ
*/
void dumpTree(NODE *node, NODE *par, FILE *of) {
  static int i = 0;

  if (node == NULL) {
    fprintf(of, "%d[label=%d];\n", par->data, par->data);
    fprintf(of, "nullNode%d[style=invis];\n", i);
    fprintf(of, "%d -> nullNode%d[style=invis];\n", par->data, i++);
    return;
  }
  if (par == NULL) {
    fprintf(of, "digraph G{\n");
    fprintf(of, "graph [ordering=out];\n");
  } else {
    fprintf(of, "%d[label=%d];\n", par->data, par->data);
    fprintf(of, "%d -> %d", par->data, node->data);
    if (node->data > par->data) {
      fprintf(of, "[style=dotted]");
    }
    fprintf(of, ";\n");
  }
  dumpTree(node->left, node, of);
  dumpTree(node->right, node, of);
  if (par == NULL) {
    fprintf(of, "}\n");
    fclose(of);
  }
}

/*
  search -- 指定されたキーを持つノードを探索する
    root: 探索を開始するノードのアドレス
    key: 探索するキー
*/
NODE *search(NODE *root, KEY key) {
  NODE *p;
  p = root;

  while(p != NULL) {
    if (key < p->data) {
      p = p->left;
    } else if (key > p->data) {
      p = p->right;
    } else {
      return p;
    }
  }
  return NULL;
}

/*
  node_mutex_destroy -- 各ノードが持つMutexオブジェクトを破棄
  p: 木構造の根ノードを指すポインタ
*/
void node_mutex_destroy(NODE *p) {
  if (p != NULL) {
    pthread_mutex_destroy(&p->mutex);
    node_mutex_destroy(p->left);
    node_mutex_destroy(p->right);
  }
}