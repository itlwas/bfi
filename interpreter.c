#include "interpreter.h"
#include "memory.h"
#include "logger.h"
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
            size_t currentIndex = dataPtr - tapeBuffer;
            size_t windowStart = (currentIndex >= snapshotWidth) ? currentIndex - snapshotWidth : 0;
            size_t windowEnd = (currentIndex + snapshotWidth < currentCapacity) ? currentIndex + snapshotWidth : currentCapacity - 1;
            printf("Iteration: %llu | IP: %zu | Command: '%c'\nPointer: %zu | Cell: %d | Capacity: %zu\nTape: ", iterationCount, instructionPtr, command, currentIndex, *dataPtr, currentCapacity);
            for (size_t pos = windowStart; pos <= windowEnd; pos++) {
                char disp = (tapeBuffer[pos] >= 32 && tapeBuffer[pos] <= 126) ? tapeBuffer[pos] : '.';
                if (pos == currentIndex)
                    printf("->[%zu: %d/%c] ", pos, tapeBuffer[pos], disp);
                else
                    printf("[%zu: %d/%c] ", pos, tapeBuffer[pos], disp);
            }
            printf("\n------------------------\n");
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
            case '!': printf("Debug: Pointer: %zu, Value: %d\n", dataPtr - tapeBuffer, *dataPtr); break;
            case '$': fprintf(stderr, "State: IP: %zu, Pointer: %zu, Cell: %d\n", instructionPtr, dataPtr - tapeBuffer, *dataPtr); break;
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