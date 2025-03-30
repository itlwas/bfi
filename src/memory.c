#include "memory.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
void *AllocateMemory(size_t numBytes) {
    void *ptr = malloc(numBytes);
    if (!ptr) {
        TerminateWithErrorCode(ERR_MEMORY, 1, FormatError("Memory allocation failed (%zu bytes)", numBytes));
    }
    return ptr;
}
void *ReallocateMemory(void *pointer, size_t numBytes) {
    void *newPtr = realloc(pointer, numBytes);
    if (!newPtr) {
        TerminateWithErrorCode(ERR_MEMORY, 2, FormatError("Memory reallocation failed (%zu bytes)", numBytes));
    }
    return newPtr;
}