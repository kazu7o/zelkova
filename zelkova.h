#ifndef ZELKOVA_H_
#define ZELKOVA_H_

#include "common.h"

NODE *insert_znode(KEY key);

extern int z_rotations;
extern NODE *z_root;
extern pthread_mutex_t mutex_root;

#endif