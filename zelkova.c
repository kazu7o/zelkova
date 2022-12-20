#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "zelkova.h"

int z_rotations = 0;
NODE *z_root = NULL;
pthread_mutex_t mutex_root;

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

  /* 待機スレッドの注目ノードをpivotにするための処理 */
  if (p_root->mutex.__data.__lock >= 2) {
    p_root->rotated = pivot;
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

  /* 待機スレッドの注目ノードをpivotにするための処理 */
  if (p_root->mutex.__data.__lock >= 2) {
    p_root->rotated = pivot;
  }

  return pivot;
}

/*
  LR_Rotate -- 左->右回転
    root: 回転する部分木の根ノード
*/
static NODE *LR_Rotate(NODE *p_root, NODE **root, NODE *parent) {
  p_root->left = L_Rotate(p_root->left, root, p_root);
  p_root = R_Rotate(p_root, root, parent);
  // printf("data: %d p_root.lock: %d\n", p_root->data, p_root->mutex.__data.__lock);
  return p_root;
}

/*
  RL_Rotate -- 右->左回転
    root: 回転する部分木の根ノード
*/
static NODE *RL_Rotate(NODE *p_root, NODE **root, NODE *parent) {
  p_root->right = R_Rotate(p_root->right, root, p_root);
  p_root = L_Rotate(p_root, root, parent);
  // printf("data: %d p_root.lock: %d\n", p_root->data, p_root->mutex.__data.__lock);
  return p_root;
}

/*
  insert_fnode -- 要素を挿入する（再帰的）
    root: 挿入する部分木の根ノード
    key: 挿入するデータ
*/
// NODE *insert_znode(NODE *root, KEY key) {
// NODE *insert_znode(void *_key) {
NODE *insert_znode(KEY key) {
  NODE **p;
  NODE *parent;

  pthread_mutex_lock(&mutex_root);
  printf("key: %d\n", key);
  p = &z_root;
  parent = NULL;
  // key = *(KEY *)_key;
  if (*p == NULL) {
    *p = malloc_node(key);
    z_root = *p;
    pthread_mutex_unlock(&mutex_root);
    return *p;
  }

  pthread_mutex_unlock(&mutex_root);
  
  /* 根ノードから葉ノードへ順にノードを挿入する場所を探索 */
  while (1) {
    // printf("[ws] p.data: %d p.lock: %d adr: %p\n", (*p)->data, (*p)->mutex.__data.__lock, *p);
    pthread_mutex_lock(&(*p)->mutex);
    if ((*p)->rotated != NULL) {
      NODE **temp = p;
      p = &(*p)->rotated;
      if ((*p)->mutex.__data.__lock <= 1) {
        (*temp)->rotated = NULL;
      }
      pthread_mutex_unlock(&(*temp)->mutex);
      continue;
    }
    /* 部分木に含まれるノード数が1のとき、回転は行わない */
    if ((*p)->passnum == 1) {
      (*p)->passnum++;
      if (key < (*p)->data) {
        (*p)->left = malloc_node(key);
      } else {
        (*p)->right = malloc_node(key);
      } 
      pthread_mutex_unlock(&(*p)->mutex);
      return z_root;
    }

    /* 部分木に含まれるノード数が2のとき、回転の条件を確認 */
    if ((*p)->passnum == 2) {
      (*p)->passnum++;
      /* 左部分木が無いとき */
      if ((*p)->left == NULL) {
        if (key < (*p)->data) {
          (*p)->left = malloc_node(key);
          pthread_mutex_unlock(&(*p)->mutex);
          return z_root;
        }
        if (key < (*p)->right->data) {
          (*p)->right->passnum++;
          (*p)->right->left = malloc_node(key);
          pthread_mutex_lock(&(*p)->right->left->mutex);
          *p = RL_Rotate(*p, &z_root, parent);
          pthread_mutex_unlock(&(*p)->left->mutex);
          pthread_mutex_unlock(&(*p)->mutex);
          printf("rl1data: %d lock: %d adr: %p\n", (*p)->left->data, (*p)->left->mutex.__data.__lock, (*p)->left);
          return z_root; 
        } else if (key >= (*p)->right->data) {
          (*p)->right->passnum++;
          (*p)->right->right = malloc_node(key);
          pthread_mutex_lock(&(*p)->right->mutex);
          *p = L_Rotate(*p, &z_root, parent);
          pthread_mutex_unlock(&(*p)->left->mutex);
          pthread_mutex_unlock(&(*p)->mutex);
          printf("l1data: %d lock: %d\n", (*p)->left->data, (*p)->left->mutex.__data.__lock);
          return z_root;
        }
      /* 右部分木が無いとき */
      } else {
        if (key >= (*p)->data) {
          (*p)->right = malloc_node(key);
          pthread_mutex_unlock(&(*p)->mutex);
          return z_root;
        }
        if (key < (*p)->left->data) {
          (*p)->left->passnum++;
          (*p)->left->left = malloc_node(key);
          pthread_mutex_lock(&(*p)->left->mutex);
          *p = R_Rotate(*p, &z_root, parent);
          pthread_mutex_unlock(&(*p)->right->mutex);
          pthread_mutex_unlock(&(*p)->mutex);
          printf("r1data: %d lock: %d\n", (*p)->right->data, (*p)->right->mutex.__data.__lock);
          return z_root;
        } else if (key >= (*p)->left->data) {
          (*p)->left->passnum++;
          (*p)->left->right = malloc_node(key);
          pthread_mutex_lock(&(*p)->left->right->mutex);
          *p = LR_Rotate(*p, &z_root, parent);
          pthread_mutex_unlock(&(*p)->right->mutex);
          pthread_mutex_unlock(&(*p)->mutex);
          printf("lr1data: %d lock: %d adr: %p\n", (*p)->right->data, (*p)->right->mutex.__data.__lock, (*p)->right);
          return z_root;
        }
      }
    }

    /* 部分木に含まれるノード数が2より大きく、回転の条件を満たす場合 */
    if ((*p)->left->passnum >= 2*(*p)->right->passnum+2) {  // 左部分木が高く、非平衡
      if ((*p)->left->right->passnum > (*p)->left->left->passnum) {
        pthread_mutex_lock(&(*p)->left->right->mutex);
        *p = LR_Rotate(*p, &z_root, parent);
        pthread_mutex_unlock(&(*p)->right->mutex);
        printf("lr2data: %d lock: %d\n", (*p)->right->data, (*p)->right->mutex.__data.__lock);
      } else {
        pthread_mutex_lock(&(*p)->left->mutex);
        *p = R_Rotate(*p, &z_root, parent);
        pthread_mutex_unlock(&(*p)->right->mutex);
        printf("r2data: %d lock: %d\n", (*p)->right->data, (*p)->right->mutex.__data.__lock);
      }
    } else if (2*(*p)->left->passnum+2 <= (*p)->right->passnum) { // 右部分木が高く、非平衡
      if ((*p)->right->right->passnum < (*p)->right->left->passnum) {
        pthread_mutex_lock(&(*p)->right->left->mutex);
        *p = RL_Rotate(*p, &z_root, parent);
        pthread_mutex_unlock(&(*p)->left->mutex);
        printf("rl2data: %d lock: %d\n", (*p)->left->data, (*p)->left->mutex.__data.__lock);
      } else {
        pthread_mutex_lock(&(*p)->right->mutex);
        *p = L_Rotate(*p, &z_root, parent);
        pthread_mutex_unlock(&(*p)->left->mutex);
        printf("l2data: %d lock: %d\n", (*p)->left->data, (*p)->left->mutex.__data.__lock);
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
    pthread_mutex_unlock(&parent->mutex);
    printf("p.data: %d p.lock: %d adr: %p\n", (*p)->data, (*p)->mutex.__data.__lock, *p);
  }
  return z_root;
}