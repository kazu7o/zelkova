#include <stdio.h>
#include <stdlib.h>
#include "zelkova.h"

int z_rotations = 0;

/*
  R_Rotate -- 右回転
    p_root: 回転する部分木の根ノード
    root: 根ノード
*/
static NODE *R_Rotate(NODE *p_root, NODE **root, NODE *parent) {
  NODE *pivot = p_root->left;
  p_root->left = pivot->right;
  pivot->right = p_root;

  z_rotations++;

  /* passnumの更新 */
  int pivot_right_left = (pivot->right->left == NULL ? 0 : pivot->right->left->passnum);
  int pivot_right_right = (pivot->right->right == NULL ? 0 : pivot->right->right->passnum);
  int pivot_left = (pivot->left == NULL ? 0 : pivot->left->passnum);
  pivot->right->passnum = pivot_right_left + pivot_right_right + 1;
  pivot->passnum = pivot_left + pivot->right->passnum + 1;

  /* 根ノードの更新 */
  if (parent == NULL) {
    *root = pivot;
  }

  return pivot;
}

/*
  L_Rotate -- 左回転
    root: 回転する部分木の根ノード
*/
static NODE *L_Rotate(NODE *p_root, NODE **root, NODE *parent) {
  NODE *pivot = p_root->right;
  p_root->right = pivot->left;
  pivot->left = p_root;

  z_rotations++;

  /* passnumの更新 */
  int pivot_left_left = (pivot->left->left == NULL ? 0 : pivot->left->left->passnum);
  int pivot_left_right = (pivot->left->right == NULL ? 0 : pivot->left->right->passnum);
  int pivot_right = (pivot->right == NULL ? 0 : pivot->right->passnum);
  pivot->left->passnum = pivot_left_left + pivot_left_right + 1;
  pivot->passnum = pivot->left->passnum + pivot_right + 1;

  /* 根ノードの更新 */
  if (parent == NULL) {
    *root = pivot;
  }

  return pivot;
}

/*
  LR_Rotate -- 左->右回転
    root: 回転する部分木の根ノード
*/
static NODE *LR_Rotate(NODE *p_root, NODE **root, NODE *parent) {
  p_root->left = L_Rotate(p_root->left, root, p_root);
  return R_Rotate(p_root, root, parent);
}

/*
  RL_Rotate -- 右->左回転
    root: 回転する部分木の根ノード
*/
static NODE *RL_Rotate(NODE *p_root, NODE **root, NODE *parent) {
  p_root->right = R_Rotate(p_root->right, root, p_root);
  p_root = L_Rotate(p_root, root, parent);
  return p_root;
}

/*
  insert_fnode -- 要素を挿入する（再帰的）
    root: 挿入する部分木の根ノード
    key: 挿入するデータ
*/
NODE *insert_znode(NODE *root, KEY key) {
  NODE **p;
  NODE *parent;

  p = &root;
  parent = NULL;
  if (*p == NULL) {
    *p = malloc_node(key);
    return *p;
  }
  
  /* 根ノードから葉ノードへ順にノードを挿入する場所を探索 */
  while (1) {
    /* 部分木に含まれるノード数が1のとき、回転は行わない */
    if ((*p)->passnum == 1) {
      (*p)->passnum++;
      if (key < (*p)->data) {
        (*p)->left = malloc_node(key);
      } else {
        (*p)->right = malloc_node(key);
      } 
      return root;
    }

    /* 部分木に含まれるノード数が2のとき、回転の条件を確認 */
    if ((*p)->passnum == 2) {
      (*p)->passnum++;
      /* 左部分木が無いとき */
      if ((*p)->left == NULL) {
        if (key < (*p)->data) {
          (*p)->left = malloc_node(key);
          return root;
        }
        if (key < (*p)->right->data) {
          (*p)->right->passnum++;
          (*p)->right->left = malloc_node(key);
          *p = RL_Rotate(*p, &root, parent);
          return root; 
        } else if (key >= (*p)->right->data) {
          (*p)->right->passnum++;
          (*p)->right->right = malloc_node(key);
          *p = L_Rotate(*p, &root, parent);
          return root;
        }
      /* 右部分木が無いとき */
      } else {
        if (key >= (*p)->data) {
          (*p)->right = malloc_node(key);
          return root;
        }
        if (key < (*p)->left->data) {
          (*p)->left->passnum++;
          (*p)->left->left = malloc_node(key);
          *p = R_Rotate(*p, &root, parent);
          return root;
        } else if (key >= (*p)->left->data) {
          (*p)->left->passnum++;
          (*p)->left->right = malloc_node(key);
          *p = LR_Rotate(*p, &root, parent);
          return root;
        }
      }
    }

    /* 部分木に含まれるノード数が2より大きく、回転の条件を満たす場合 */
    if ((*p)->left->passnum >= 2*(*p)->right->passnum+2) {  // 左部分木が高く、非平衡
      if ((*p)->left->right->passnum > (*p)->left->left->passnum) {
        *p = LR_Rotate(*p, &root, parent);
      } else {
        *p = R_Rotate(*p, &root, parent);
      }
    } else if (2*(*p)->left->passnum+2 <= (*p)->right->passnum) { // 右部分木が高く、非平衡
      if ((*p)->right->right->passnum < (*p)->right->left->passnum) {
        *p = RL_Rotate(*p, &root, parent);
      } else {
        *p = L_Rotate(*p, &root, parent);
      }
    }

    /* ノード挿入位置の探索 */
    parent = *p;
    (*p)->passnum++;
    if (key < (*p)->data) {
      p = &(*p)->left;
    } else {
      p = &(*p)->right;
    }
  }
  return root;
}