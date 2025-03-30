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
    if (!programCode || !bracketMapping) return;
    BFState *state = !*tapeRef ? CreateExecutor(initialTapeCapacity) : 
                    (BFState*)memcpy(AllocateMemory(sizeof(BFState)), 
                    &(BFState){*tapeRef, *tapeCapacity, *dataIndex, (*tapeRef)[*dataIndex]}, 
                    sizeof(BFState));
    size_t ip = 0;
    unsigned long long instrCount = 0;
    static unsigned long long outputCount = 0;
    setvbuf(stdout, NULL, _IOFBF, BUFSIZ * 16);
    while (ip < codeLength && instrCount < maxIterations) {
        char cmd = programCode[ip];
        if (debugMode) {
            size_t winStart = state->dataPtr >= snapshotWidth ? state->dataPtr - snapshotWidth : 0;
            size_t winEnd = state->dataPtr + snapshotWidth < state->capacity ? 
                            state->dataPtr + snapshotWidth : state->capacity - 1;
            state->tape[state->dataPtr] = state->currentValue;
            printf("%sIteration: %s%llu%s | IP: %s%zu%s | Command: '%s%c%s'\n", 
                CLR_BOLD, CLR_YELLOW, instrCount, CLR_RESET, CLR_YELLOW, ip, CLR_RESET,
                CLR_CYAN, cmd, CLR_RESET);
            printf("Pointer: %s%zu%s | Cell: %s%d%s | Capacity: %s%zu%s\n", 
                CLR_GREEN, state->dataPtr, CLR_RESET, CLR_MAGENTA, state->currentValue, CLR_RESET,
                CLR_BLUE, state->capacity, CLR_RESET);
            printf("%sTape: %s", CLR_BOLD, CLR_RESET);
            for (size_t pos = winStart; pos <= winEnd; pos++) {
                printf(pos == state->dataPtr ? 
                    "%s->[%s%zu%s: %s%d%s/%s%c%s] " : "[%s%zu%s: %s%d%s/%s%c%s] ", 
                    CLR_RED, CLR_GREEN, pos, CLR_RESET, 
                    CLR_MAGENTA, pos == state->dataPtr ? state->currentValue : state->tape[pos], CLR_RESET,
                    CLR_CYAN, state->tape[pos] >= 32 && state->tape[pos] <= 126 ? state->tape[pos] : '.', 
                    CLR_RESET);
            }
            printf("\n%s------------------------%s\n", CLR_BOLD, CLR_RESET);
        }
        if (cmd == '[') {
            if (ip + 2 < codeLength && 
                (programCode[ip+1] == '-' || programCode[ip+1] == '+') && 
                programCode[ip+2] == ']') {
                SetZero(state);
                ip += 3;
                instrCount += 3;
                continue;
            }
            else if (ip + 5 < codeLength && programCode[ip+1] == '-') {
                if ((programCode[ip+2] == '>' && programCode[ip+3] == '+' && programCode[ip+4] == '<' && 
                    programCode[ip+5] == ']') || 
                   (ip + 7 < codeLength && programCode[ip+2] == '>' && programCode[ip+3] == '>' && 
                    programCode[ip+4] == '+' && programCode[ip+5] == '<' && programCode[ip+6] == '<' && 
                    programCode[ip+7] == ']')) {
                    
                    bool isPattern1 = programCode[ip+3] == '+' && programCode[ip+4] == '<';
                    if (state->currentValue > 0) {
                        unsigned char val = state->currentValue;
                        SetZero(state);
                        MovePtr(state, isPattern1 ? 1 : 2, maxTapeCapacity);
                        state->currentValue += val;
                        state->tape[state->dataPtr] = state->currentValue;
                        MovePtr(state, isPattern1 ? -1 : -2, maxTapeCapacity);
                    }
                    ip += isPattern1 ? 6 : 8;
                    instrCount += isPattern1 ? 6 : 8;
                    continue;
                }
            }
            else if (ip + 2 < codeLength && 
                    (programCode[ip+1] == '>' || programCode[ip+1] == '<') && 
                    programCode[ip+2] == ']') {
                if (!IsZero(state)) {
                    int dir = programCode[ip+1] == '>' ? 1 : -1;
                    while (!IsZero(state) && 
                          (dir > 0 ? state->dataPtr < state->capacity : 
                           state->dataPtr > (state->dataPtr > 1000 ? state->dataPtr - 1000 : 0))) {
                        MovePtr(state, dir, maxTapeCapacity);
                    }
                }
                ip += 3;
                instrCount += 3;
                continue;
            }
        }
        switch (cmd) {
            case '>': 
            case '<': {
                int offset = 0;
                size_t nextIp = ip;
                while (nextIp < codeLength && (programCode[nextIp] == '>' || programCode[nextIp] == '<'))
                    offset += programCode[nextIp++] == '>' ? 1 : -1;
                
                if (offset) MovePtr(state, offset, maxTapeCapacity);
                instrCount += nextIp - ip;
                ip = nextIp;
                continue;
            }
            case '+': 
            case '-': {
                int delta = 0;
                size_t nextIp = ip;
                while (nextIp < codeLength && (programCode[nextIp] == '+' || programCode[nextIp] == '-'))
                    delta += programCode[nextIp++] == '+' ? 1 : -1;
                
                if ((delta %= 256)) Increment(state, delta);
                instrCount += nextIp - ip;
                ip = nextIp;
                continue;
            }
            case '.': 
                OutputValue(state);
                if (++outputCount % 4096 == 0) fflush(stdout);
                break;
            case ',':
                fflush(stdout);
                InputValue(state, eofHandlingMode);
                break;
            case '[': 
                if (IsZero(state)) ip = bracketMapping[ip];
                break;
            case ']': 
                if (!IsZero(state)) ip = bracketMapping[ip];
                break;
            case '!': 
                printf("%sDebug: Pointer: %s%zu%s, Value: %s%d%s\n", 
                    CLR_BOLD, CLR_GREEN, state->dataPtr, CLR_RESET, 
                    CLR_MAGENTA, state->currentValue, CLR_RESET); 
                break;
            case '$': 
                fprintf(stderr, "%sState: IP: %s%zu%s, Pointer: %s%zu%s, Cell: %s%d%s\n", 
                    CLR_BOLD, CLR_YELLOW, ip, CLR_RESET,
                    CLR_GREEN, state->dataPtr, CLR_RESET,
                    CLR_MAGENTA, state->currentValue, CLR_RESET); 
                break;
        }
        ip++;
        instrCount++;
    }
    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
    state->tape[state->dataPtr] = state->currentValue;
    execMemPeak = state->capacity;
    execInstrCount = instrCount;
    if (autoReleaseTape) {
        DestroyExecutor(state);
        *tapeRef = NULL;
        *tapeCapacity = *dataIndex = 0;
    } else {
        *tapeRef = state->tape;
        *tapeCapacity = state->capacity;
        *dataIndex = state->dataPtr;
        free(state);
    }
}