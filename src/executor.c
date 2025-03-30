#include "executor.h"
#include "memory.h"
#include "logger.h"
#include "bfi.h"
#include <stdio.h>
#include <string.h>
BFState* CreateExecutor(size_t initialCapacity) {
    if (initialCapacity == 0)
        initialCapacity = INITIAL_TAPE_CAPACITY;
    BFState *state = AllocateMemory(sizeof(BFState));
    state->tape = AllocateMemory(initialCapacity);
    memset(state->tape, 0, initialCapacity);
    state->capacity = initialCapacity;
    state->dataPtr = 0;
    return state;
}
void DestroyExecutor(BFState *state) {
    if (state) {
        if (state->tape)
            free(state->tape);
        free(state);
    }
}
void MovePtr(BFState *state, int offset, size_t maxCapacity) {
    if (!state || offset == 0)
        return; 
    if (offset < 0 && (size_t)(-offset) > state->dataPtr) {
        TerminateWithErrorCode(ERR_RUNTIME, 1, "Tape underflow - pointer moved below position 0");
    }
    size_t newPos = offset < 0 ? state->dataPtr - (size_t)(-offset) : state->dataPtr + (size_t)offset;
    if (newPos >= state->capacity) {
        size_t newCap = state->capacity * 2;
        newCap = newCap > newPos ? newCap : newPos + 1;
        if (newCap > maxCapacity) {
            if (newPos >= maxCapacity) {
                TerminateWithErrorCode(ERR_RUNTIME, 2, "Maximum tape capacity exceeded");
            }
            newCap = maxCapacity;
        }
        state->tape = ReallocateMemory(state->tape, newCap);
        memset(state->tape + state->capacity, 0, newCap - state->capacity);
        state->capacity = newCap;
    }
    state->dataPtr = newPos;
}