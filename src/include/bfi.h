#ifndef BFI_H
#define BFI_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define BFI_VERSION "v1.0"
#define INITIAL_TAPE_CAPACITY 30000
#define MAX_FILE_SIZE (10 * 1024 * 1024)
#define MAX_TAPE_CAPACITY (1024 * 1024)
#define SNAPSHOT_WIDTH 5
#define MAX_ITERATIONS 1000000000ULL
#define CLR_RESET "\x1b[0m"
#define CLR_RED "\x1b[31m"
#define CLR_GREEN "\x1b[32m"
#define CLR_YELLOW "\x1b[33m"
#define CLR_BLUE "\x1b[34m"
#define CLR_MAGENTA "\x1b[35m"
#define CLR_CYAN "\x1b[36m"
#define CLR_BOLD "\x1b[1m"
extern size_t gMaxFileSizeAllowed;
#endif