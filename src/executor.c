#include "executor.h"
#include "memory.h"
#include "logger.h"
#include "bfi.h"
#include <stdio.h>
#include <string.h>
BFState* CreateExecutor(size_t initialCapacity) {
    if (!initialCapacity) initialCapacity = INITIAL_TAPE_CAPACITY;
    BFState *state = AllocateMemory(sizeof(BFState));
    state->tape = AllocateMemory(initialCapacity);
    memset(state->tape, 0, initialCapacity);
    state->capacity = initialCapacity;
    state->dataPtr = 0;
    state->currentValue = 0;
    return state;
}
void DestroyExecutor(BFState *state) {
    if (state) {
        free(state->tape);
        free(state);
    }
}
void MovePtr(BFState *state, int offset, size_t maxCapacity) {
    if (!state || !offset) return;
    state->tape[state->dataPtr] = state->currentValue;
    if (offset < 0 && (size_t)(-offset) > state->dataPtr)
        TerminateWithErrorCode(ERR_RUNTIME, 1, "Tape underflow - pointer moved below position 0");
    size_t newPos = state->dataPtr + offset;
    if (newPos >= state->capacity) {
        if (newPos >= maxCapacity)
            TerminateWithErrorCode(ERR_RUNTIME, 2, "Maximum tape capacity exceeded");
        size_t newCap = state->capacity < 1024 
                       ? state->capacity << 1 
                       : state->capacity + (state->capacity >> 1);
        newCap = newPos >= newCap ? newPos + 1024 : newCap;
        if (newCap > maxCapacity) {
            newCap = maxCapacity;
        }
        state->tape = ReallocateMemory(state->tape, newCap);
        memset(state->tape + state->capacity, 0, newCap - state->capacity);
        state->capacity = newCap;
    }
    state->dataPtr = newPos;
    state->currentValue = state->tape[state->dataPtr];
}