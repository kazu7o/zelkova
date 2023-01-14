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
    parent: 親ノード
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
    p_root: 回転する部分木の根ノード
    root: 根ノード
    parent: 親ノード
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
    p_root: 回転する部分木の根ノード
    root: 根ノード
    parent: 親ノード
*/
static NODE *LR_Rotate(NODE *p_root, NODE **root, NODE *parent) {
  p_root->left = L_Rotate(p_root->left, root, p_root);
  p_root = R_Rotate(p_root, root, parent);
  return p_root;
}

/*
  RL_Rotate -- 右->左回転
    p_root: 回転する部分木の根ノード
    root: 根ノード
    parent: 親ノード
*/
static NODE *RL_Rotate(NODE *p_root, NODE **root, NODE *parent) {
  p_root->right = R_Rotate(p_root->right, root, p_root);
  p_root = L_Rotate(p_root, root, parent);
  return p_root;
}

/*
  insert_fnode -- 要素を挿入する（再帰的）
    key: 挿入するデータ
*/
NODE *insert_znode(KEY key) {
  NODE **p;
  NODE *parent;
  NODE *parent_parent;
  pthread_mutex_t *mutex_cur;
  pthread_mutex_t *mutex_pre;

  pthread_mutex_lock(&mutex_root);
  p = &z_root;
  parent = NULL;
  if (*p == NULL) {
    *p = malloc_node(key);
    z_root = *p;
    pthread_mutex_unlock(&mutex_root);
    return z_root;
  }
  
  /* 根ノードから葉ノードへ順にノードを挿入する場所を探索 */
  while (1) {
    /* 部分木に含まれるノード数が1のとき、回転は行わない */
    if ((*p)->passnum == 1) {
      (*p)->passnum++;
      if (key < (*p)->data) {
        pthread_mutex_t *m1 = &(*p)->mutex_left;
        pthread_mutex_lock(m1);
        (*p)->left = malloc_node(key);
        pthread_mutex_unlock(m1);
      } else {
        pthread_mutex_t *m2 = &(*p)->mutex_right;
        pthread_mutex_lock(m2);
        (*p)->right = malloc_node(key);
        pthread_mutex_unlock(m2);
      } 
      if (parent == NULL) {
        pthread_mutex_unlock(&mutex_root);
      } else {
        pthread_mutex_unlock(mutex_cur);
      }
      return z_root;
    }

    /* 部分木に含まれるノード数が2のとき、回転の条件を確認 */
    if ((*p)->passnum == 2) {
      /* 左部分木が無いとき */
      pthread_mutex_t *m3 = &(*p)->mutex_left;
      pthread_mutex_lock(m3);
      if ((*p)->left == NULL) {
        if (key < (*p)->data) {
          (*p)->passnum++;
          (*p)->left = malloc_node(key);
          pthread_mutex_unlock(m3);
          if (parent == NULL) {
            pthread_mutex_unlock(&mutex_root);
          } else {
            pthread_mutex_unlock(mutex_cur);
          }
          return z_root;
        }
        pthread_mutex_t *m4 = &(*p)->mutex_right;
        pthread_mutex_lock(m4);
        if (key < (*p)->right->data) {
          (*p)->passnum++;
          pthread_mutex_t *m5 = &(*p)->right->mutex_left;
          pthread_mutex_lock(m5);
          (*p)->right->left = malloc_node(key);
          (*p)->right->passnum++;
          *p = RL_Rotate(*p, &z_root, parent);
          pthread_mutex_unlock(m3);
          pthread_mutex_unlock(m4);
          pthread_mutex_unlock(m5);
          if (parent == NULL) {
            pthread_mutex_unlock(&mutex_root);
          } else {
            pthread_mutex_unlock(mutex_cur);
          }
          return z_root; 
        } else if (key >= (*p)->right->data) {
          (*p)->passnum++;
          pthread_mutex_t *m6 = &(*p)->right->mutex_right;
          pthread_mutex_lock(m6);
          (*p)->right->right = malloc_node(key);
          (*p)->right->passnum++;
          *p = L_Rotate(*p, &z_root, parent);
          pthread_mutex_unlock(m3);
          pthread_mutex_unlock(m4);
          pthread_mutex_unlock(m6);
          if (parent == NULL) {
            pthread_mutex_unlock(&mutex_root);
          } else {
            pthread_mutex_unlock(mutex_cur);
          }
          return z_root;
        }
        pthread_mutex_unlock(m4);
      /* 右部分木が無いとき */
      } else {
        pthread_mutex_t *m7 = &(*p)->mutex_right;
        pthread_mutex_lock(m7);
        if (key >= (*p)->data) {
          (*p)->passnum++;
          (*p)->right = malloc_node(key);
          pthread_mutex_unlock(m7);
          pthread_mutex_unlock(m3);
          if (parent == NULL) {
            pthread_mutex_unlock(&mutex_root);
          } else {
            pthread_mutex_unlock(mutex_cur);
          }
          return z_root;
        }
        if (key < (*p)->left->data) {
          (*p)->passnum++;
          pthread_mutex_t *m8 = &(*p)->left->mutex_left;
          pthread_mutex_lock(m8);
          (*p)->left->left = malloc_node(key);
          (*p)->left->passnum++;
          *p = R_Rotate(*p, &z_root, parent);
          pthread_mutex_unlock(m3);
          pthread_mutex_unlock(m7);
          pthread_mutex_unlock(m8);
          if (parent == NULL) {
            pthread_mutex_unlock(&mutex_root);
          } else {
            pthread_mutex_unlock(mutex_cur);
          }
          return z_root;
        } else if (key >= (*p)->left->data) {
          (*p)->passnum++;
          pthread_mutex_t *m9 = &(*p)->left->mutex_right;
          pthread_mutex_lock(m9);
          (*p)->left->right = malloc_node(key);
          (*p)->left->passnum++;
          *p = LR_Rotate(*p, &z_root, parent);
          pthread_mutex_unlock(m3);
          pthread_mutex_unlock(m7);
          pthread_mutex_unlock(m9);
          if (parent == NULL) {
            pthread_mutex_unlock(&mutex_root);
          } else {
            pthread_mutex_unlock(mutex_cur);
          }
          return z_root;
        }
      }
    }


    pthread_mutex_t *m10 = &(*p)->mutex_left; 
    pthread_mutex_t *m11 = &(*p)->mutex_right; 
    pthread_mutex_lock(m10);
    pthread_mutex_lock(m11);
    /* 部分木に含まれるノード数が2より大きく、回転の条件を満たす場合 */
    if ((*p)->left->passnum >= 2*(*p)->right->passnum+2) {  // 左部分木が高く、非平衡
      pthread_mutex_t *m12 = &(*p)->left->mutex_left; 
      pthread_mutex_t *m13 = &(*p)->left->mutex_right; 
      pthread_mutex_lock(m12);
      pthread_mutex_lock(m13);
      if ((*p)->left->right->passnum > (*p)->left->left->passnum) {
        pthread_mutex_t *m14 = &(*p)->left->right->mutex_left;
        pthread_mutex_t *m15 = &(*p)->left->right->mutex_right;
        pthread_mutex_lock(m14);
        pthread_mutex_lock(m15);
        *p = LR_Rotate(*p, &z_root, parent);
        pthread_mutex_unlock(m14);
        pthread_mutex_unlock(m15);
      } else {
        pthread_mutex_t *m16 = &(*p)->left->left->mutex_left;
        pthread_mutex_t *m17 = &(*p)->left->left->mutex_right;
        pthread_mutex_lock(m16);
        pthread_mutex_lock(m17);
        *p = R_Rotate(*p, &z_root, parent);
        pthread_mutex_unlock(m16);
        pthread_mutex_unlock(m17);
      }
      pthread_mutex_unlock(m12);
      pthread_mutex_unlock(m13);
    } else if (2*(*p)->left->passnum+2 <= (*p)->right->passnum) { // 右部分木が高く、非平衡
      pthread_mutex_t *m18 = &(*p)->right->mutex_left; 
      pthread_mutex_t *m19 = &(*p)->right->mutex_right; 
      pthread_mutex_lock(m18);
      pthread_mutex_lock(m19);
      if ((*p)->right->right->passnum < (*p)->right->left->passnum) {
        pthread_mutex_t *m20 = &(*p)->right->left->mutex_left; 
        pthread_mutex_t *m21 = &(*p)->right->left->mutex_right; 
        pthread_mutex_lock(m20);
        pthread_mutex_lock(m21);
        *p = RL_Rotate(*p, &z_root, parent);
        pthread_mutex_unlock(m20);
        pthread_mutex_unlock(m21);
      } else {
        pthread_mutex_t *m22 = &(*p)->right->right->mutex_left; 
        pthread_mutex_t *m23 = &(*p)->right->right->mutex_right; 
        pthread_mutex_lock(m22);
        pthread_mutex_lock(m23);
        *p = L_Rotate(*p, &z_root, parent);
        pthread_mutex_unlock(m22);
        pthread_mutex_unlock(m23);
      }
      pthread_mutex_unlock(m18);
      pthread_mutex_unlock(m19);
    }
    pthread_mutex_unlock(m10);
    pthread_mutex_unlock(m11);

    mutex_pre = mutex_cur;
    parent_parent = parent;
    parent = *p;
    (*p)->passnum++;
    /* ノード挿入位置の探索 */
    if (key < (*p)->data) {
      mutex_cur = &(*p)->mutex_left;
      pthread_mutex_lock(mutex_cur);
      p = &(*p)->left;
    } else {
      mutex_cur = &(*p)->mutex_right;
      pthread_mutex_lock(mutex_cur);
      p = &(*p)->right;
    }
    if (parent_parent == NULL) {
      pthread_mutex_unlock(&mutex_root);
    } else {
      pthread_mutex_unlock(mutex_pre);
    }
  }
  return z_root;
}
