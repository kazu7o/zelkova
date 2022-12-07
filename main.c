#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include "SFMT.h"
#include "avltree.h"
#include "ftree.h"
#include "zelkova.h"
#include "common.h"

#define NUM_ADDNODE 100000
#define NUM_THREAD 8
#define SEED 0

KEY *keys;
typedef struct _THREAD_DATA {
  int start;
  int end;
} THREAD_DATA;

/*
  random_keys -- ランダムなキー配列を生成する
    keys: 生成したキーを格納する配列のアドレス
    count: 生成するキーの数
    seed: シード値
*/
static void random_keys(int *keys, int count, int seed) {
  sfmt_t sfmt;
  int i;
  int tmp;
  
  // 重複なしのキー配列を生成
  for (i = 0; i < count; i++) {
    keys[i] = i + 1;
  }

  // キー配列をシャッフル
  i = count - 1;
  sfmt_init_gen_rand(&sfmt, seed);
  while (i > 0) {
    uint32_t j = sfmt_genrand_uint32(&sfmt) % (i + 1);
    tmp = keys[j];
    keys[j] = keys[i];
    keys[i] = tmp;
    i--;
  }
}

/*
  dumpkeys -- キーの一覧表示
*/
static void dumpkeys(int *keys, int count) {
  int i;
  for (i = 0; i < count;) {
    printf("%5d ", keys[i]);
    if (++i % 10 == 0) {
      printf("\n");
    }
  }
  printf("\n");
}

static void th_insert(void *_args) {
  int i, end;
  THREAD_DATA *args = (THREAD_DATA *)_args;
  i = args->start;
  end = args->end;
  while (i < end && i < NUM_ADDNODE) {
    printf("key: %d\n", keys[i]);
    insert_znode(keys[i]);
    i++;
  }
}

/*
  benchmark -- 木構造の性能評価テスト
*/
static void benchmark(void) {
  int i, dw;
  pthread_t thread[NUM_THREAD];
  THREAD_DATA th_data[NUM_THREAD];

  pthread_mutex_init(&mutex_root, NULL);
  keys = (KEY *)malloc(sizeof(KEY) * NUM_ADDNODE);
  random_keys(keys, NUM_ADDNODE, SEED);

  dw = (int)ceil((double)NUM_ADDNODE / (double)NUM_THREAD);
  printf("dw: %d\n", dw);

  for (i = 0; i < NUM_THREAD; i++) {
    th_data[i].start = i * dw;
    th_data[i].end = th_data[i].start + dw;
    if (pthread_create(&thread[i], NULL, (void *)th_insert, (void *)&th_data[i]) != 0) {
      error("can not create thread");
    }
  }
  for (i = 0; i < NUM_THREAD; i++) {
    if (pthread_join(thread[i], NULL) != 0) {
      error("can not join thread");
    }
  }

  // for (i = 0; i < NUM_ADDNODE;) {
  //   for (j = 0; j < NUM_THREAD; j++) {
  //     if (i + j >= NUM_ADDNODE) {
  //       break;
  //     }
  //     if (pthread_create(&thread[j], NULL, (void *)insert_znode, (void *)&keys[i+j]) != 0) {
  //       error("can not create thread");
  //     }
  //   }
  //   for (j = 0; j < NUM_THREAD; j++) {
  //     if (i + j >= NUM_ADDNODE) {
  //       break;
  //     }
  //     if (pthread_join(thread[j], NULL) != 0) {
  //       error("can not join thread");
  //     }
  //   }
  //   i += NUM_THREAD;
  // }

  pthread_mutex_destroy(&mutex_root);

  printf("nodes: %d\nrotations: %d\nheight: %d\nmin_depth: %d\n", NUM_ADDNODE, 
  z_rotations, getHeight(z_root), getminDepth(z_root));
  FILE *of = fopen("zelkova.dot", "w");
  dumpTree(z_root, NULL, of);
}

int main(void) {
  clock_t begin, end;

  begin = clock();
  benchmark();
  end = clock();
  node_mutex_destroy(z_root);
  clearTree(z_root);
  printf("Execution Time = %lf [s]\n", (double)(end - begin) / CLOCKS_PER_SEC);
  return 0;
}
