#ifndef MEMORY_H
#define MEMORY_H
#include <stdlib.h>
void *AllocateMemory(size_t numBytes);
void *ReallocateMemory(void *pointer, size_t numBytes);
#endif