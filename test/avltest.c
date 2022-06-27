#include <stdio.h>
#include <stdlib.h>
#include "avltest.h"

/*
  get_height_ftree -- 木の高さを取得する
                       空の木の高さは-1とする
    root: 部分木の根ノード
*/
int get_height_ftree(F_NODE *root) {
  int left_height;
  int right_height;

  if (root == NULL) {
    return -1;
  }
  left_height = get_height_ftree(root->left);
  right_height = get_height_ftree(root->right);
  return max(left_height, right_height) + 1;
}

/*
  R_Rotate -- 右回転
    root: 回転する部分木の根ノード
*/
static F_NODE *R_Rotate(F_NODE *root) {
  F_NODE *pivot = root->left;
  root->left = pivot->right;
  pivot->right = root;

  /* passnumの更新 */
  int pivot_right_left = (pivot->right->left == NULL ? 0 : pivot->right->left->passnum);
  int pivot_right_right = (pivot->right->right == NULL ? 0 : pivot->right->right->passnum);
  int pivot_left = (pivot->left == NULL ? 0 : pivot->left->passnum);
  pivot->right->passnum = pivot_right_left + pivot_right_right + 1;
  pivot->passnum = pivot_left + pivot->right->passnum + 1;

  return pivot;
}

/*
  L_Rotate -- 左回転
    root: 回転する部分木の根ノード
*/
static F_NODE *L_Rotate(F_NODE *root) {
  F_NODE *pivot = root->right;
  root->right = pivot->left;
  pivot->left = root;

  /* passnumの更新 */
  int pivot_left_left = (pivot->left->left == NULL ? 0 : pivot->left->left->passnum);
  int pivot_left_right = (pivot->left->right == NULL ? 0 : pivot->left->right->passnum);
  int pivot_right = (pivot->right == NULL ? 0 : pivot->right->passnum);
  pivot->left->passnum = pivot_left_left + pivot_left_right + 1;
  pivot->passnum = pivot->left->passnum + pivot_right + 1;

  return pivot;
}

/*
  LR_Rotate -- 左->右回転
    root: 回転する部分木の根ノード
*/
static F_NODE *LR_Rotate(F_NODE *root) {
  root->left = L_Rotate(root->left);
  return R_Rotate(root);
}

/*
  RL_Rotate -- 右->左回転
    root: 回転する部分木の根ノード
*/
static F_NODE *RL_Rotate(F_NODE *root) {
  root->right = R_Rotate(root->right);
  return L_Rotate(root);
}

/*
  malloc_fnode -- 新しいノードに必要な領域を確保する
    key: ノードのデータ
*/
F_NODE *malloc_fnode(KEY key) {
  F_NODE *new;
  if ((new = (F_NODE *)malloc(sizeof(F_NODE))) == NULL) {
    error("out of memory!");
  }
  new->left    = NULL;
  new->right   = NULL;
  new->data    = key;
  new->passnum = 1;
  return new;
}

/*
  insert_fnode -- 要素を挿入する（再帰的）
    root: 挿入する部分木の根ノード
    key: 挿入するデータ
*/
F_NODE *insert_fnode(F_NODE *root, KEY key) {
  F_NODE *p;

  p = root;

  if (p == NULL) {
    p = malloc_fnode(key);
    return p;
  } else if (key < p->data) {
    p->passnum++;
    p->left = insert_fnode(p->left, key);
  } else {  // key >= p->data
    p->passnum++;
    p->right = insert_fnode(p->right, key);
  }

  // 平衡化
  balance = getHeight(p->left) - getHeight(p->right);
  // printf("balance: %d\n", balance);
  if (balance >= 2) { // 左部分木が高く、非平衡
    printf("left childs: %d right childs: %d\n", p->left->passnum, p->right->passnum);
    if (getHeight(p->left->left) > getHeight(p->left->right)) { // Left Left Case
      p = R_Rotate(p);
    } else {  // Left Right Case
      p = LR_Rotate(p);
    }
  } else if (balance <= -2) { // 右部分木が高く、非平衡
    printf("left childs: %d right childs: %d\n", p->left->passnum, p->right->passnum);
    if (getHeight(p->right->left) > getHeight(p->right->right)) { // Right Left Case
      p = RL_Rotate(p);
    } else {  // Right Right Case
      p = L_Rotate(p);
    }
  }
  return p;
}

/*
  clear_ftree -- 木に含まれるノードを全て削除する
    root: 削除する木の根ノード
*/
void clear_ftree(F_NODE *root) {
  if (root == NULL) {
    return;
  }
  if (root->left != NULL) {
    clear_ftree(root->left);
  }
  if (root->right != NULL) {
    clear_ftree(root->right);
  }
  free(root);
}

int main(void) {
  int i;
  F_NODE *root;
  KEY keys[] = {12, 7, 15, 4, 10, 14, 16, 2, 6, 9, 11, 13, 1, 3, 5, 8, 0};
  int count = sizeof(keys) / sizeof(keys[0]);
  root = NULL;

  for (i = 0; i < count; i++) {
    root = insert_fnode(root, keys[i]);
  }
  return 0;
}