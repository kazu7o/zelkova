#include <stdio.h>
#include <stdlib.h>
#include "avltree.h"

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
  R_Rotate -- 右回転
    root: 回転する部分木の根ノード
*/
NODE *R_Rotate(NODE *root) {
  NODE *pivot = root->left;
  root->left = pivot->right;
  pivot->right = root;
  return pivot;
}

/*
  L_Rotate -- 左回転
    root: 回転する部分木の根ノード
*/
NODE *L_Rotate(NODE *root) {
  NODE *pivot = root->right;
  root->right = pivot->left;
  pivot->left = root;
  return pivot;
}

/*
  LR_Rotate -- 左->右回転
    root: 回転する部分木の根ノード
*/
NODE *LR_Rotate(NODE *root) {
  root->left = L_Rotate(root->left);
  return R_Rotate(root);
}

/*
  RL_Rotate -- 右->左回転
    root: 回転する部分木の根ノード
*/
NODE *RL_Rotate(NODE *root) {
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
    p = new;
    return new;
  } else if (key < p->data) {
    p->left = insert(p->left, key);
  } else {  // key >= p->data
    p->right = insert(p->right, key);
  }

  // 平衡化
  balance = getHeight(p->left) - getHeight(p->right);
  // printf("balance: %d\n", balance);
  if (balance >= 2) { // 左部分木が高く、非平衡
    if (getHeight(p->left->left) > getHeight(p->left->right)) { // Left Left Case
      p = R_Rotate(p);
    } else {  // Left Right Case
      p = LR_Rotate(p);
    }
  } else if (balance <= -2) { // 右部分木が高く、非平衡
    if (getHeight(p->right->right) > getHeight(p->right->left)) { // Right Right Case
      p = L_Rotate(p);
    } else {  // Right Left Case
      p = RL_Rotate(p);
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