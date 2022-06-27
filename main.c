#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SFMT.h"
#include "avltree.h"
#include "ftree.h"

#define NUM_ADDNODE 100000
#define SEED 0

/*
  random_keys -- ランダムなキー配列を生成する
    keys: 生成したキーを格納する配列のアドレス
    count: 生成するキーの数
    seed: シード値
*/
// static void random_keys(int *keys, int count, int seed) {
//   sfmt_t sfmt;
//   int i;
  
//   sfmt_init_gen_rand(&sfmt, seed);
//   for (i = 0; i < count; i++) {
//     uint32_t key = sfmt_genrand_uint32(&sfmt) % count;
//     keys[i] = key;
//   }
// }
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
  random_keys2 - rand関数によるランダムなキー配列の生成
    keys: 生成したキーを格納する配列のアドレス
    count: 生成するキーの数
    seed: シード値
*/
static void random_keys2(int *keys, int count, int seed) {
  int i;

  srand(seed);
  for (i = 0; i < count; i++) {
    int key = rand() % count;
    keys[i] = key;
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
  int i;
  NODE *root;

  keys = (KEY *)malloc(sizeof(KEY) * NUM_ADDNODE);
  root = NULL;
  random_keys(keys, NUM_ADDNODE, SEED);
  // random_keys2(keys, count, SEED);
  // dumpkeys(keys, count);
  for (i = 0; i < count; i++) {
    root = insert(root, keys[i]);
  }
  printf("nodes: %d\nrotations: %d\nheight: %d\nmin_depth: %d\n", count, rotations, getHeight(root), getminDepth(root));
  dumpTree(root, NULL);
  clearTree(root);
}

static void test1(int count, char *fname) {
  KEY *keys;
  int i;
  NODE *root;

  keys = (KEY *)malloc(sizeof(KEY) * NUM_ADDNODE);
  root = NULL;
  random_keys(keys, NUM_ADDNODE, SEED);
  // dumpkeys(keys, count);
  for (i = 0; i < count; i++) {
    root = insert_fnode(root, keys[i]);
  }
  printf("nodes: %d\nrotations: %d\nheight: %d\nmin_depth: %d\n", count, f_rotations, getHeight(root), getminDepth(root));
  FILE *of = fopen(fname, "w");
  fdumpTree(root, NULL, of);
  clearTree(root);
}

static void test2(int count, char *fname) {
  KEY *keys;
  int i;
  NODE *root;

  keys = (KEY *)malloc(sizeof(KEY) * NUM_ADDNODE);
  root = NULL;
  random_keys(keys, NUM_ADDNODE, SEED);
  // dumpkeys(keys, count);
  for (i = 0; i < count; i++) {
    root = insert(root, keys[i]);
  }
  printf("nodes: %d\nrotations: %d\nheight: %d\nmin_depth: %d\n", count, rotations, getHeight(root), getminDepth(root));
  FILE *of = fopen(fname, "w");
  fdumpTree(root, NULL, of);
  clearTree(root);
}

void test3(void) {
  KEY *keys;
  keys = (KEY *)malloc(sizeof(KEY) * NUM_ADDNODE);
  random_keys(keys, NUM_ADDNODE, SEED);
  dumpkeys(keys, NUM_ADDNODE);
}

int main(int argc, char *argv[]) {
  char *fname = "hoge.dot";
  clock_t begin, end;

  if (argc == 2) {
    fname = argv[1];
  }
  begin = clock();
  // benchmark(NUM_ADDNODE);
  // benchmark(50);
  // benchmark(100);
  // test1(NUM_ADDNODE, fname);
  test2(NUM_ADDNODE, fname);
  // test3();
  end = clock();
  printf("Execution Time = %lf [s]\n", (double)(end - begin) / CLOCKS_PER_SEC);
  return 0;
}