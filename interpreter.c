#include "interpreter.h"
#include "memory.h"
#include "logger.h"
#include "bfi.h"
#include <stdio.h>
#include <string.h>
void ExecuteBrainfuck(const char *programCode, size_t codeLength, const size_t *bracketMapping, bool debugMode, size_t snapshotWidth, size_t initialTapeCapacity, size_t maxTapeCapacity, unsigned long long maxIterations, int eofHandlingMode, unsigned char **tapeRef, size_t *tapeCapacity, size_t *dataIndex, bool autoReleaseTape) {
    size_t currentCapacity;
    unsigned char *tapeBuffer;
    unsigned char *dataPtr;
    if (!*tapeRef) {
        currentCapacity = initialTapeCapacity;
        tapeBuffer = (unsigned char *)calloc(currentCapacity, 1);
        if (!tapeBuffer)
            TerminateWithError("Critical Error: Unable to allocate tape memory.");
        dataPtr = tapeBuffer;
        *dataIndex = 0;
    } else {
        tapeBuffer = *tapeRef;
        currentCapacity = *tapeCapacity;
        dataPtr = tapeBuffer + *dataIndex;
    }
    size_t instructionPtr = 0;
    unsigned long long iterationCount = 0;
    while (instructionPtr < codeLength && iterationCount < maxIterations) {
        char command = programCode[instructionPtr];
        if (debugMode) {
            size_t curIdx = dataPtr - tapeBuffer;
            size_t winStart = (curIdx >= snapshotWidth) ? curIdx - snapshotWidth : 0;
            size_t winEnd = (curIdx + snapshotWidth < currentCapacity) ? curIdx + snapshotWidth : currentCapacity - 1;
            printf("%sIteration: %s%llu%s | IP: %s%zu%s | Command: '%s%c%s'\n", 
                CLR_BOLD, CLR_YELLOW, iterationCount, CLR_RESET, 
                CLR_YELLOW, instructionPtr, CLR_RESET,
                CLR_CYAN, command, CLR_RESET);
            printf("Pointer: %s%zu%s | Cell: %s%d%s | Capacity: %s%zu%s\n", 
                CLR_GREEN, curIdx, CLR_RESET, 
                CLR_MAGENTA, *dataPtr, CLR_RESET,
                CLR_BLUE, currentCapacity, CLR_RESET);
            printf("%sTape: %s", CLR_BOLD, CLR_RESET);
            for (size_t pos = winStart; pos <= winEnd; pos++) {
                char disp = (tapeBuffer[pos] >= 32 && tapeBuffer[pos] <= 126) ? tapeBuffer[pos] : '.';
                if (pos == curIdx)
                    printf("%s->[%s%zu%s: %s%d%s/%s%c%s] ", 
                        CLR_RED, CLR_GREEN, pos, CLR_RESET, 
                        CLR_MAGENTA, tapeBuffer[pos], CLR_RESET,
                        CLR_CYAN, disp, CLR_RESET);
                else
                    printf("[%s%zu%s: %s%d%s/%s%c%s] ", 
                        CLR_GREEN, pos, CLR_RESET, 
                        CLR_MAGENTA, tapeBuffer[pos], CLR_RESET,
                        CLR_CYAN, disp, CLR_RESET);
            }
            printf("\n%s------------------------%s\n", CLR_BOLD, CLR_RESET);
        }
        if (command == '>' || command == '<') {
            int offset = 0;
            while (instructionPtr < codeLength && (programCode[instructionPtr] == '>' || programCode[instructionPtr] == '<')) {
                offset += (programCode[instructionPtr] == '>') ? 1 : -1;
                instructionPtr++;
                iterationCount++;
            }
            dataPtr += offset;
            size_t newIndex = dataPtr - tapeBuffer;
            if (newIndex >= currentCapacity) {
                size_t newCapacity = currentCapacity + currentCapacity / 2;
                while (newCapacity <= newIndex)
                    newCapacity += newCapacity / 2;
                if (newCapacity > maxTapeCapacity)
                    TerminateWithError("Runtime Error: Maximum tape capacity exceeded.");
                unsigned char *newBuffer = (unsigned char *)ReallocateMemory(tapeBuffer, newCapacity);
                memset(newBuffer + currentCapacity, 0, newCapacity - currentCapacity);
                dataPtr = newBuffer + newIndex;
                tapeBuffer = newBuffer;
                currentCapacity = newCapacity;
            }
            continue;
        }
        if (command == '+' || command == '-') {
            int delta = 0;
            while (instructionPtr < codeLength && (programCode[instructionPtr] == '+' || programCode[instructionPtr] == '-')) {
                delta += (programCode[instructionPtr] == '+') ? 1 : -1;
                instructionPtr++;
                iterationCount++;
            }
            *dataPtr += delta;
            continue;
        }
        switch (command) {
            case '.': putchar(*dataPtr); break;
            case ',':
                {
                    int inputChar = getchar();
                    *dataPtr = (inputChar == EOF) ? ((eofHandlingMode == 2) ? 255 : 0) : (unsigned char)inputChar;
                    break;
                }
            case '[': if (!*dataPtr) instructionPtr = bracketMapping[instructionPtr]; break;
            case ']': if (*dataPtr) instructionPtr = bracketMapping[instructionPtr]; break;
            case '!': printf("%sDebug: Pointer: %s%zu%s, Value: %s%d%s\n", 
                        CLR_BOLD, CLR_GREEN, dataPtr - tapeBuffer, CLR_RESET, 
                        CLR_MAGENTA, *dataPtr, CLR_RESET); break;
            case '$': fprintf(stderr, "%sState: IP: %s%zu%s, Pointer: %s%zu%s, Cell: %s%d%s\n", 
                        CLR_BOLD, CLR_YELLOW, instructionPtr, CLR_RESET,
                        CLR_GREEN, dataPtr - tapeBuffer, CLR_RESET,
                        CLR_MAGENTA, *dataPtr, CLR_RESET); break;
            default: break;
        }
        instructionPtr++;
        iterationCount++;
    }
    *dataIndex = dataPtr - tapeBuffer;
    *tapeCapacity = currentCapacity;
    *tapeRef = tapeBuffer;
    if (autoReleaseTape) {
        free(tapeBuffer);
        *tapeRef = NULL;
        *tapeCapacity = 0;
        *dataIndex = 0;
    }
}