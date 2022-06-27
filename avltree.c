#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "avltree.h"

// #define DEBUG

float min_rate = 1.0;
float max_rate = 0.0;
float min_lc, min_rc;
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
// static NODE *R_Rotate(NODE *root) {
//   NODE *pivot = root->left;
//   root->left = pivot->right;
//   pivot->right = root;
//   return pivot;
// }
static NODE *R_Rotate(NODE *root) {
  NODE *pivot = root->left;
  root->left = pivot->right;
  pivot->right = root;

  rotations++;

  /* passnumの更新 */
  int pivot_right_left = pivot->right->left == NULL ? 0 : pivot->right->left->passnum;
  int pivot_right_right = pivot->right->right == NULL ? 0 : pivot->right->right->passnum;
  int pivot_left = pivot->left == NULL ? 0 : pivot->left->passnum;
  pivot->right->passnum = pivot_right_left + pivot_right_right + 1;
  pivot->passnum = pivot_left + pivot->right->passnum + 1;

  return pivot;
}

/*
  L_Rotate -- 左回転
    root: 回転する部分木の根ノード
*/
// static NODE *L_Rotate(NODE *root) {
//   NODE *pivot = root->right;
//   root->right = pivot->left;
//   pivot->left = root;
//   return pivot;
// }
static NODE *L_Rotate(NODE *root) {
  NODE *pivot = root->right;
  root->right = pivot->left;
  pivot->left = root;

  rotations++;

  /* passnumの更新 */
  int pivot_left_left = pivot->left->left == NULL ? 0 : pivot->left->left->passnum;
  int pivot_left_right = pivot->left->right == NULL ? 0 : pivot->left->right->passnum;
  int pivot_right = pivot->right == NULL ? 0 : pivot->right->passnum;
  pivot->left->passnum = pivot_left_left + pivot_left_right + 1;
  pivot->passnum = pivot->left->passnum + pivot_right + 1;

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
  
  // #ifdef DEBUG
  // float lc = p->left == NULL ? 0 : p->left->passnum;
  // float rc = p->right == NULL ? 0 : p->right->passnum;
  // float lr = lc / rc;
  // if (lc != 0 && rc != 0) {
  //   if (lr >= 4 || lr <= 0.25) {
  //     printf("[DEBUG] left childs: %.0f right childs: %.0f\n", lc, rc);
  //   }
  // }
  // #endif

  #ifdef DEBUG
    static int j = 1;
  #endif

  #ifdef DEBUG
    static int j = 1;
  #endif

  // 平衡化
  balance = getHeight(p->left) - getHeight(p->right);
  #ifdef DEBUG
    printf("balance: %d\n", balance);
  #endif
  if (balance >= 2) { // 左部分木が高く、非平衡
    #ifdef DEBUG
    float lc = p->left == NULL ? 0 : p->left->passnum;
    float rc = p->right == NULL ? 0 : p->right->passnum;
    float rate = fabs(lc-rc)/(lc+rc);
    if (lc != 0 && rc != 0) {
      if ((lc >= rc*2+1 && lc < 4*rc) || (lc*2+1 <= rc && rc < 4*lc)) {
        if ((lc==1||lc==3) && (rc==1||rc==3)) {
          printf("[ROTATE %5d] left childs: %.f right childs: %.f rate: %.5f\n", j++, lc, rc, rate);
        }
      }
      if (rate < min_rate) {
        min_rate = rate;
        min_lc = lc;
        min_rc = rc;
      }
    }
    #endif
    if (getHeight(p->left->left) > getHeight(p->left->right)) { // Left Left Case
      p = R_Rotate(p);
    } else {  // Left Right Case
      p = LR_Rotate(p);
    }
  } else if (balance <= -2) { // 右部分木が高く、非平衡
    #ifdef DEBUG
    float lc = p->left == NULL ? 0 : p->left->passnum;
    float rc = p->right == NULL ? 0 : p->right->passnum;
    float rate = fabs(lc-rc)/(lc+rc);
    if (lc != 0 && rc != 0) {
      if ((lc >= rc*2+1 && lc < 4*rc) || (lc*2+1 <= rc && rc < 4*lc)) {
        if ((lc==1||lc==3) && (rc==1||rc==3)) {
          printf("[ROTATE %5d] left childs: %.f right childs: %.f rate: %.5f\n", j++, lc, rc, rate);
        }
      }
      if (rate < min_rate) {
        min_rate = rate;
        min_lc = lc;
        min_rc = rc;
      }
    }
    #endif
    if (getHeight(p->right->left) > getHeight(p->right->right)) { // Right Left Case
      p = RL_Rotate(p);
    } else {  // Right Right Case
      p = L_Rotate(p);
    }
  }
  #ifdef DEBUG
  else {
    static int i = 1;
    float lc = p->left == NULL ? 0 : p->left->passnum;
    float rc = p->right == NULL ? 0 : p->right->passnum;
    float rate = fabs(lc-rc)/(lc+rc);
    float _rate = lc/rc;
    if (lc != 0 && rc != 0) {
      if ((lc >= rc*2+1 && lc < 4*rc) || (lc*2+1 <= rc && rc < 4*lc)) {
        if (lc!=1 && rc!=1) {
          if (lc==3 || rc==3) {
            printf("[NO ROTATE %5d] left childs: %.f right childs: %.f rate: %.5f balance: %d\n", i++, lc, rc, rate, balance);
          }
        }
        if (rate > max_rate) {
          max_rate = rate;
        }
      }
    }
  }
  #endif

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

void preorder(NODE *node) {
  if (node != NULL) {
    printf("%d ", node->data);
    preorder(node->left);
    preorder(node->right);
  }
}