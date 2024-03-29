#include <stdlib.h>
#include "avltree.h"

int rotations = 0;

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