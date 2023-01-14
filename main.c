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

#define SEED 0

KEY *keys;
typedef struct _THREAD_DATA {
  int thread_no;
  int start;
  int end;
  int num_addnode;
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
  int i, end, num_addnode;
  THREAD_DATA *args = (THREAD_DATA *)_args;
  i = args->start;
  end = args->end;
  num_addnode = args->num_addnode;
  while (i < end && i < num_addnode) {
    insert_znode(keys[i]);
    i++;
  }
}

/*
  benchmark -- 木構造の性能評価テスト
*/
static double benchmark(int num_thread, int num_addnode) {
  clock_t begin, end;
  int i, dw;
  pthread_t thread[num_thread];
  THREAD_DATA th_data[num_thread];

  pthread_mutex_init(&mutex_root, NULL);
  keys = (KEY *)malloc(sizeof(KEY) * num_addnode);
  random_keys(keys, num_addnode, SEED);

  dw = (int)ceil((double)num_addnode / (double)num_thread);

  begin = clock();
  for (i = 0; i < num_thread; i++) {
    th_data[i].start = i * dw;
    th_data[i].end = th_data[i].start + dw;
    th_data[i].num_addnode = num_addnode;
    if (pthread_create(&thread[i], NULL, (void *)th_insert, (void *)&th_data[i]) != 0) {
      error("can not create thread");
    }
  }
  for (i = 0; i < num_thread; i++) {
    if (pthread_join(thread[i], NULL) != 0) {
      error("can not join thread");
    }
  }

  end = clock();
  pthread_mutex_destroy(&mutex_root);

  printf("nodes: %d\nthreads: %d\nrotations: %d\nheight: %d\nmin_depth: %d\n", num_addnode, num_thread, z_rotations, getHeight(z_root), getminDepth(z_root));
  // FILE *of = fopen("zelkova.dot", "w");
  // dumpTree(z_root, NULL, of);

  return (double)(end - begin) / CLOCKS_PER_SEC;
}

int main(int argc, char **argv) {
  double et;
  int num_thread, num_addnode;

  if (argc != 4) {
    fprintf(stderr, "Error: too few arguments\n");
    return EXIT_FAILURE;
  }

  num_thread = atoi(argv[1]);
  num_addnode = atoi(argv[2]);

  et = benchmark(num_thread, num_addnode);
  FILE *of = fopen(argv[3], "w");
  inorder(z_root, of);
  node_mutex_destroy(z_root);
  clearTree(z_root);
  printf("Execution Time = %lf [s]\n", et);
  return 0;
}
