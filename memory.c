#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
void *AllocateMemory(size_t numBytes) {
    void *ptr = malloc(numBytes);
    if (!ptr) {
        fprintf(stderr, "Critical Error: Memory allocation of %zu bytes failed.\n", numBytes);
        exit(EXIT_FAILURE);
    }
    return ptr;
}
void *ReallocateMemory(void *pointer, size_t numBytes) {
    void *newPtr = realloc(pointer, numBytes);
    if (!newPtr) {
        fprintf(stderr, "Critical Error: Memory reallocation to %zu bytes failed.\n", numBytes);
        exit(EXIT_FAILURE);
    }
    return newPtr;
}