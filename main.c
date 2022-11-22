#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "SFMT.h"
#include "avltree.h"
#include "ftree.h"
#include "zelkova.h"
#include "common.h"

#define NUM_ADDNODE 1000000
#define NUM_THREAD 4
#define SEED 5

pthread_mutex_t mutex;

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

/*
  benchmark -- 木構造の性能評価テスト
    count: 挿入するデータの数
*/
static void benchmark(int count) {
  KEY *keys;
  int i, j;
  pthread_t thread[NUM_THREAD];

  keys = (KEY *)malloc(sizeof(KEY) * NUM_ADDNODE);
  random_keys(keys, NUM_ADDNODE, SEED);

  for (i = 0; i < count;) {
    for (j = 0; j < NUM_THREAD; j++) {
      if (i + j >= count) {
        break;
      }
      if (pthread_create(&thread[j], NULL, (void *)insert_znode, (void *)&keys[i+j]) != 0) {
        error("can not create thread");
      }
    }
    for (j = 0; j < NUM_THREAD; j++) {
      if (i + j >= count) {
        break;
      }
      if (pthread_join(thread[j], NULL)!= 0) {
        error("can not join thread");
      }
    }
    i += NUM_THREAD;
  }

  pthread_mutex_destroy(&mutex);

  printf("nodes: %d\nrotations: %d\nheight: %d\nmin_depth: %d\n", count, 
  z_rotations, getHeight(z_root), getminDepth(z_root));
  FILE *of = fopen("zelkova.dot", "w");
  dumpTree(z_root, NULL, of);
  clearTree(z_root);
}

int main(void) {
  clock_t begin, end;

  begin = clock();
  benchmark(NUM_ADDNODE);
  end = clock();
  printf("Execution Time = %lf [s]\n", (double)(end - begin) / CLOCKS_PER_SEC);
  return 0;
}
