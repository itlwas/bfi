#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
typedef struct {
    unsigned char *tape;
    size_t capacity;
    size_t dataPtr;
} BFState;
BFState* CreateExecutor(size_t initialCapacity);
void DestroyExecutor(BFState *state);
void MovePtr(BFState *state, int offset, size_t maxCapacity);
static inline void Increment(BFState *state, int delta) {
    if (!state) return;
    state->tape[state->dataPtr] += delta;
}
static inline void OutputValue(BFState *state) {
    if (!state) return;
    putchar(state->tape[state->dataPtr]);
    fflush(stdout);
}
static inline bool IsZero(BFState *state) {
    if (!state) return true;
    return state->tape[state->dataPtr] == 0;
}
static inline void InputValue(BFState *state, int eofHandlingMode) {
    if (!state) return;
    int c = getchar();
    if (c == EOF) {
        switch (eofHandlingMode) {
            case 0: break;
            case 1: state->tape[state->dataPtr] = 0; break;
            case 2: state->tape[state->dataPtr] = 255; break;
        }
    } else {
        state->tape[state->dataPtr] = (unsigned char)c;
    }
}
#endif 