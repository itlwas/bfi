#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
typedef struct {
    unsigned char *tape;
    size_t capacity;
    size_t dataPtr;
    unsigned char currentValue;
} BFState;
BFState* CreateExecutor(size_t initialCapacity);
void DestroyExecutor(BFState *state);
void MovePtr(BFState *state, int offset, size_t maxCapacity);
static inline void Increment(BFState *state, int delta) {
    state->currentValue += delta;
}
static inline void OutputValue(BFState *state) {
    putchar(state->currentValue);
}
static inline bool IsZero(BFState *state) {
    return !state->currentValue;
}
static inline void InputValue(BFState *state, int eofHandlingMode) {
    int c = getchar();
    if (c == EOF) {
        state->currentValue = eofHandlingMode == 1 ? 0 : 
                             eofHandlingMode == 2 ? 255 : state->currentValue;
    } else state->currentValue = c;
}
static inline void SetZero(BFState *state) {
    state->currentValue = 0;
}
static inline void SetValue(BFState *state, unsigned char value) {
    state->currentValue = value;
}
#endif 