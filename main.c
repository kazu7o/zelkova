#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SFMT.h"
#include "avltree.h"
#include "ftree.h"
#include "zelkova.h"

#define NUM_ADDNODE 100000
#define SEED 5

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
    uint64_t j = sfmt_genrand_uint64(&sfmt) % (i + 1);
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
  test_zelkova -- zelkovaの性能評価テスト
    num_addnode: 挿入するデータの数
*/
static double test_zelkova(int num_addnode, char *fname) {
  clock_t begin, end;
  KEY *keys;
  int i;
  NODE *root;

  keys = (KEY *)malloc(sizeof(KEY) * num_addnode);
  root = NULL;
  random_keys(keys, num_addnode, SEED);

  begin = clock();
  for (i = 0; i < num_addnode; i++) {
    root = insert_znode(root, keys[i]);
  }
  end = clock();
  printf("nodes: %d\nrotations: %d\nheight: %d\nmin_depth: %d\n", num_addnode, z_rotations, getHeight(root), getminDepth(root));
  //printf("nodes: %ld\nrotations: %ld\n", num_addnode, z_rotations);
  FILE *of = fopen(fname, "w");
  inorder(root, of);
  clearTree(root);
  return (double)(end - begin) / CLOCKS_PER_SEC;
}

/*
  test_ftree -- F木の性能評価テスト
    count: 挿入するデータの数
*/
static double test_ftree(int num_addnode, char *fname) {
  clock_t begin, end;
  KEY *keys;
  int i;
  NODE *root;

  keys = (KEY *)malloc(sizeof(KEY) * num_addnode);
  root = NULL;
  random_keys(keys, num_addnode, SEED);

  begin = clock();
  for (i = 0; i < num_addnode; i++) {
    root = insert_fnode(root, keys[i]);
  }
  end = clock();
  printf("nodes: %d\nrotations: %d\nheight: %d\nmin_depth: %d\n", num_addnode, f_rotations, getHeight(root), getminDepth(root));
  FILE *of = fopen(fname, "w");
  inorder(root, of);
  clearTree(root);
  return (double)(end - begin) / CLOCKS_PER_SEC;
}

/*
  test_avl -- AVL木の性能評価テスト
    count: 挿入するデータの数
*/
static double test_avl(int num_addnode, char *fname) {
  clock_t begin, end;
  KEY *keys;
  int i;
  NODE *root;

  keys = (KEY *)malloc(sizeof(KEY) * num_addnode);
  root = NULL;
  random_keys(keys, num_addnode, SEED);

  begin = clock();
  for (i = 0; i < num_addnode; i++) {
    root = insert(root, keys[i]);
  }
  end = clock();
  printf("nodes: %d\nrotations: %d\nheight: %d\nmin_depth: %d\n", num_addnode, rotations, getHeight(root), getminDepth(root));
  FILE *of = fopen(fname, "w");
  inorder(root, of);
  clearTree(root);
  return (double)(end - begin) / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[]) {
  double et;
  int num_addnode;
  char *fname = "hoge.dot";

  if (argc != 3) {
    fprintf(stderr, "Error: too few arguments\n");
    return EXIT_FAILURE;
  }
  num_addnode = atoi(argv[1]);
  fname = argv[2];
  et = test_zelkova(num_addnode, fname);
  printf("Execution Time = %lf [s]\n", et);
  return 0;
}
