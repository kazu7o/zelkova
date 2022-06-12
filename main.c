#include <stdio.h>
#include <stdlib.h>
#include "SFMT.h"
#include "avltree.h"

#define NUM_ADDNODE 10000
#define SEED 0

/*
  random_keys -- ランダムなキー配列を生成する
    keys: 生成したキーを格納する配列のアドレス
    count: 生成するキーの数
    seed: シード値
*/
static void random_keys(int *keys, int count, int seed) {
  sfmt_t sfmt;
  int i;

  sfmt_init_gen_rand(&sfmt, seed);
  for (i = 0; i < count; i++) {
    uint32_t key = sfmt_genrand_uint32(&sfmt) % count;
    keys[i] = key;
  }
}

// static void dumpkeys(int *keys, int count) {
//   int i;
//   for (i = 0; i < count;) {
//     printf("%5d ", keys[i]);
//     if (++i % 10 == 0) {
//       printf("\n");
//     }
//   }
// }

/*
  benchmark -- 木構造の性能評価テスト
    count: 挿入するデータの数
*/
static void benchmark(int count) {
  int *keys;
  int i;
  NODE *root;

  keys = (int *)malloc(sizeof(int) * count);
  root = NULL;
  random_keys(keys, count, SEED);
  // dumpkeys(keys, count);
  for (i = 0; i < count; i++) {
    root = insert(root, keys[i]);
  }
  printf("Height of Tree: %d\n", getHeight(root));
  clearTree(root);
}

int main(void) {
  benchmark(NUM_ADDNODE);
  return 0;
}