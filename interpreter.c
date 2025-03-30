#include "interpreter.h"
#include "executor.h"
#include "memory.h"
#include "logger.h"
#include "bfi.h"
#include <stdio.h>
#include <string.h>
size_t execMemPeak = 0;
unsigned long long execInstrCount = 0;
void ExecuteBrainfuck(const char *programCode, size_t codeLength, const size_t *bracketMapping, 
                      bool debugMode, size_t snapshotWidth, size_t initialTapeCapacity, 
                      size_t maxTapeCapacity, unsigned long long maxIterations, 
                      int eofHandlingMode, unsigned char **tapeRef, size_t *tapeCapacity, 
                      size_t *dataIndex, bool autoReleaseTape) {
    BFState *state = NULL;
    if (!programCode || !bracketMapping || !tapeRef || !tapeCapacity || !dataIndex)
        return;
    if (!*tapeRef) {
        state = CreateExecutor(initialTapeCapacity);
    } else {
        state = AllocateMemory(sizeof(BFState));
        state->tape = *tapeRef;
        state->capacity = *tapeCapacity;
        state->dataPtr = *dataIndex;
    }
    size_t ip = 0;
    unsigned long long instrCount = 0;
    while (ip < codeLength && instrCount < maxIterations) {
        char cmd = programCode[ip];
        if (debugMode) {
            size_t curIdx = state->dataPtr;
            size_t winStart = (curIdx >= snapshotWidth) ? curIdx - snapshotWidth : 0;
            size_t winEnd = (curIdx + snapshotWidth < state->capacity) ? curIdx + snapshotWidth : state->capacity - 1;
            printf("%sIteration: %s%llu%s | IP: %s%zu%s | Command: '%s%c%s'\n", 
                CLR_BOLD, CLR_YELLOW, instrCount, CLR_RESET, 
                CLR_YELLOW, ip, CLR_RESET,
                CLR_CYAN, cmd, CLR_RESET);
            printf("Pointer: %s%zu%s | Cell: %s%d%s | Capacity: %s%zu%s\n", 
                CLR_GREEN, curIdx, CLR_RESET, 
                CLR_MAGENTA, state->tape[state->dataPtr], CLR_RESET,
                CLR_BLUE, state->capacity, CLR_RESET);
            printf("%sTape: %s", CLR_BOLD, CLR_RESET);
            for (size_t pos = winStart; pos <= winEnd; pos++) {
                char disp = (state->tape[pos] >= 32 && state->tape[pos] <= 126) ? state->tape[pos] : '.';
                if (pos == curIdx)
                    printf("%s->[%s%zu%s: %s%d%s/%s%c%s] ", 
                        CLR_RED, CLR_GREEN, pos, CLR_RESET, 
                        CLR_MAGENTA, state->tape[pos], CLR_RESET,
                        CLR_CYAN, disp, CLR_RESET);
                else
                    printf("[%s%zu%s: %s%d%s/%s%c%s] ", 
                        CLR_GREEN, pos, CLR_RESET, 
                        CLR_MAGENTA, state->tape[pos], CLR_RESET,
                        CLR_CYAN, disp, CLR_RESET);
            }
            printf("\n%s------------------------%s\n", CLR_BOLD, CLR_RESET);
        }
        switch (cmd) {
            case '>': 
            case '<': {
                int offset = 0;
                do {
                    offset += (programCode[ip] == '>') ? 1 : -1;
                    ip++;
                    instrCount++;
                } while (ip < codeLength && (programCode[ip] == '>' || programCode[ip] == '<'));
                
                if (offset != 0) {
                    MovePtr(state, offset, maxTapeCapacity);
                }
                continue;
            }
            case '+': 
            case '-': {
                int delta = 0;
                do {
                    delta += (programCode[ip] == '+') ? 1 : -1;
                    ip++;
                    instrCount++;
                } while (ip < codeLength && (programCode[ip] == '+' || programCode[ip] == '-'));
                if (delta != 0) {
                    Increment(state, delta);
                }
                continue;
            }
            case '.': 
                OutputValue(state); 
                break;
            case ',':
                InputValue(state, eofHandlingMode);
                break;
            case '[': 
                if (IsZero(state)) 
                    ip = bracketMapping[ip]; 
                break;
            case ']': 
                if (!IsZero(state)) 
                    ip = bracketMapping[ip]; 
                break;
            case '!': 
                printf("%sDebug: Pointer: %s%zu%s, Value: %s%d%s\n", 
                    CLR_BOLD, CLR_GREEN, state->dataPtr, CLR_RESET, 
                    CLR_MAGENTA, state->tape[state->dataPtr], CLR_RESET); 
                break;
            case '$': 
                fprintf(stderr, "%sState: IP: %s%zu%s, Pointer: %s%zu%s, Cell: %s%d%s\n", 
                    CLR_BOLD, CLR_YELLOW, ip, CLR_RESET,
                    CLR_GREEN, state->dataPtr, CLR_RESET,
                    CLR_MAGENTA, state->tape[state->dataPtr], CLR_RESET); 
                break;
        }
        ip++;
        instrCount++;
    }
    *tapeCapacity = state->capacity;
    *dataIndex = state->dataPtr;
    *tapeRef = state->tape;
    execMemPeak = state->capacity;
    execInstrCount = instrCount;
    if (autoReleaseTape) {
        free(state->tape);
        *tapeRef = NULL;
        *tapeCapacity = 0;
        *dataIndex = 0;
    }
    free(state);
}