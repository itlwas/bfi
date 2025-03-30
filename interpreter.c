#include "interpreter.h"
#include "memory.h"
#include "logger.h"
#include "bfi.h"
#include <stdio.h>
#include <string.h>
size_t execMemPeak = 0;
unsigned long long execInstrCount = 0;
void ExecuteBrainfuck(const char *programCode, size_t codeLength, const size_t *bracketMapping, bool debugMode, size_t snapshotWidth, size_t initialTapeCapacity, size_t maxTapeCapacity, unsigned long long maxIterations, int eofHandlingMode, unsigned char **tapeRef, size_t *tapeCapacity, size_t *dataIndex, bool autoReleaseTape) {
    size_t curCapacity;
    unsigned char *tape;
    unsigned char *dataPtr;
    size_t peakMem = 0;
    if (!*tapeRef) {
        curCapacity = initialTapeCapacity;
        tape = (unsigned char *)calloc(curCapacity, 1);
        if (!tape)
            TerminateWithError("Critical Error: Unable to allocate tape memory.");
        dataPtr = tape;
        *dataIndex = 0;
        peakMem = curCapacity;
    } else {
        tape = *tapeRef;
        curCapacity = *tapeCapacity;
        dataPtr = tape + *dataIndex;
        peakMem = curCapacity;
    }
    size_t ip = 0;
    unsigned long long instrCount = 0;
    while (ip < codeLength && instrCount < maxIterations) {
        char cmd = programCode[ip];
        if (debugMode) {
            size_t curIdx = dataPtr - tape;
            size_t winStart = (curIdx >= snapshotWidth) ? curIdx - snapshotWidth : 0;
            size_t winEnd = (curIdx + snapshotWidth < curCapacity) ? curIdx + snapshotWidth : curCapacity - 1;
            printf("%sIteration: %s%llu%s | IP: %s%zu%s | Command: '%s%c%s'\n", 
                CLR_BOLD, CLR_YELLOW, instrCount, CLR_RESET, 
                CLR_YELLOW, ip, CLR_RESET,
                CLR_CYAN, cmd, CLR_RESET);
            printf("Pointer: %s%zu%s | Cell: %s%d%s | Capacity: %s%zu%s\n", 
                CLR_GREEN, curIdx, CLR_RESET, 
                CLR_MAGENTA, *dataPtr, CLR_RESET,
                CLR_BLUE, curCapacity, CLR_RESET);
            printf("%sTape: %s", CLR_BOLD, CLR_RESET);
            for (size_t pos = winStart; pos <= winEnd; pos++) {
                char disp = (tape[pos] >= 32 && tape[pos] <= 126) ? tape[pos] : '.';
                if (pos == curIdx)
                    printf("%s->[%s%zu%s: %s%d%s/%s%c%s] ", 
                        CLR_RED, CLR_GREEN, pos, CLR_RESET, 
                        CLR_MAGENTA, tape[pos], CLR_RESET,
                        CLR_CYAN, disp, CLR_RESET);
                else
                    printf("[%s%zu%s: %s%d%s/%s%c%s] ", 
                        CLR_GREEN, pos, CLR_RESET, 
                        CLR_MAGENTA, tape[pos], CLR_RESET,
                        CLR_CYAN, disp, CLR_RESET);
            }
            printf("\n%s------------------------%s\n", CLR_BOLD, CLR_RESET);
        }    
        if (cmd == '>' || cmd == '<') {
            int offset = 0;
            while (ip < codeLength && (programCode[ip] == '>' || programCode[ip] == '<')) {
                offset += (programCode[ip] == '>') ? 1 : -1;
                ip++;
                instrCount++;
            }
            dataPtr += offset;
            size_t newIdx = dataPtr - tape;   
            if (newIdx >= curCapacity) {
                size_t newCap = curCapacity << 1;
                while (newCap <= newIdx)
                    newCap <<= 1;
                if (newCap > maxTapeCapacity) {
                    if (maxTapeCapacity <= newIdx)
                        TerminateWithError("Runtime Error: Maximum tape capacity exceeded.");
                    newCap = maxTapeCapacity;
                }
                unsigned char *newTape = (unsigned char *)ReallocateMemory(tape, newCap);
                memset(newTape + curCapacity, 0, newCap - curCapacity);
                dataPtr = newTape + newIdx;
                tape = newTape;
                curCapacity = newCap;
                if (curCapacity > peakMem)
                    peakMem = curCapacity;
            }
            continue;
        }
        if (cmd == '+' || cmd == '-') {
            int delta = 0;
            while (ip < codeLength && (programCode[ip] == '+' || programCode[ip] == '-')) {
                delta += (programCode[ip] == '+') ? 1 : -1;
                ip++;
                instrCount++;
            }
            *dataPtr += delta;
            continue;
        }
        switch (cmd) {
            case '.': putchar(*dataPtr); break;
            case ',':
                {
                    int inputChar = getchar();
                    *dataPtr = (inputChar == EOF) ? ((eofHandlingMode == 2) ? 255 : 0) : (unsigned char)inputChar;
                    break;
                }
            case '[': if (!*dataPtr) ip = bracketMapping[ip]; break;
            case ']': if (*dataPtr) ip = bracketMapping[ip]; break;
            case '!': printf("%sDebug: Pointer: %s%zu%s, Value: %s%d%s\n", 
                        CLR_BOLD, CLR_GREEN, dataPtr - tape, CLR_RESET, 
                        CLR_MAGENTA, *dataPtr, CLR_RESET); break;
            case '$': fprintf(stderr, "%sState: IP: %s%zu%s, Pointer: %s%zu%s, Cell: %s%d%s\n", 
                        CLR_BOLD, CLR_YELLOW, ip, CLR_RESET,
                        CLR_GREEN, dataPtr - tape, CLR_RESET,
                        CLR_MAGENTA, *dataPtr, CLR_RESET); break;
            default: break;
        }
        ip++;
        instrCount++;
    }
    *dataIndex = dataPtr - tape;
    *tapeCapacity = curCapacity;
    *tapeRef = tape;
    execMemPeak = peakMem;
    execInstrCount = instrCount;
    if (autoReleaseTape) {
        free(tape);
        *tapeRef = NULL;
        *tapeCapacity = 0;
        *dataIndex = 0;
    }
}