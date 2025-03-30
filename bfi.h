#ifndef BFI_H
#define BFI_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define INITIAL_TAPE_CAPACITY 30000
#define MAX_FILE_SIZE (10 * 1024 * 1024)
#define MAX_TAPE_CAPACITY (1024 * 1024)
#define SNAPSHOT_WIDTH 5
#define MAX_ITERATIONS 1000000000ULL
extern size_t gMaxFileSizeAllowed;
#endif