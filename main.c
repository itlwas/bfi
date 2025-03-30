#include "bfi.h"
#include "memory.h"
#include "logger.h"
#include "fileio.h"
#include "syntax.h"
#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
static inline unsigned long long ParseULL(const char *str) {
    unsigned long long value = 0;
    while (*str >= '0' && *str <= '9') {
        value = value * 10 + (*str - '0');
        str++;
    }
    return value;
}
int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
    setvbuf(stdin, NULL, _IOLBF, BUFSIZ);
    bool debugEnabled = false;
    const char *sourceFileName = NULL;
    size_t snapshotWidth = SNAPSHOT_WIDTH;
    size_t initialTapeCapacity = INITIAL_TAPE_CAPACITY;
    size_t maxTapeCapacity = MAX_TAPE_CAPACITY;
    unsigned long long maxIterations = MAX_ITERATIONS;
    int eofMode = 0;
    if (argc < 2) {
        TerminateWithError("No Brainfuck source file specified.");
    }
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1]) {
            char opt = argv[i][1];
            if (opt == 'd')
                debugEnabled = true;
            else if (opt == 'v' || strcmp(argv[i], "--version") == 0) {
                printf("BFI %s\n", BFI_VERSION);
                return EXIT_SUCCESS;
            } else if (opt == 'i') {
                if (!argv[i][2]) {
                    if (++i >= argc)
                        TerminateWithError("Missing iteration limit parameter.");
                    maxIterations = ParseULL(argv[i]);
                    if (maxIterations == 0)
                        TerminateWithError("Invalid iteration limit provided.");
                } else
                    TerminateWithError("Interactive mode is not supported.");
            } else if (opt == 'w') {
                if (++i >= argc)
                    TerminateWithError("Missing snapshot width parameter.");
                snapshotWidth = (size_t)strtol(argv[i], NULL, 10);
                if (snapshotWidth == 0)
                    TerminateWithError("Invalid snapshot width provided for debugging.");
            } else if (opt == 't') {
                if (++i >= argc)
                    TerminateWithError("Missing initial tape capacity parameter.");
                initialTapeCapacity = (size_t)strtol(argv[i], NULL, 10);
                if (initialTapeCapacity == 0)
                    TerminateWithError("Invalid initial tape capacity provided.");
            } else if (opt == 'M') {
                if (++i >= argc)
                    TerminateWithError("Missing maximum tape capacity parameter.");
                maxTapeCapacity = (size_t)strtol(argv[i], NULL, 10);
                if (maxTapeCapacity < initialTapeCapacity)
                    TerminateWithError("Maximum tape capacity must be greater than or equal to initial tape capacity.");
            } else if (opt == 'e') {
                if (++i >= argc)
                    TerminateWithError("Missing EOF mode parameter.");
                eofMode = (int)strtol(argv[i], NULL, 10);
                if (eofMode < 0 || eofMode > 2)
                    TerminateWithError("Invalid EOF mode. Acceptable values are 0, 1, or 2.");
            } else
                TerminateWithError("Unknown option provided.");
        } else if (!sourceFileName)
            sourceFileName = argv[i];
        else
            TerminateWithError("Multiple input files specified. Provide only one source file.");
    }
    if (!sourceFileName)
        TerminateWithError("No Brainfuck source file specified.");
    size_t sourceSize = 0;
    char *sourceContent = ReadFileContent(sourceFileName, &sourceSize);
    size_t *bracketMapping = CreateBracketMapping(sourceContent, sourceSize);
    unsigned char *tapeBuffer = NULL;
    size_t tapeCapacity = 0;
    size_t dataIndex = 0;
    ExecuteBrainfuck(sourceContent, sourceSize, bracketMapping, debugEnabled, snapshotWidth, initialTapeCapacity, maxTapeCapacity, maxIterations, eofMode, &tapeBuffer, &tapeCapacity, &dataIndex, true);
    free(sourceContent);
    free(bracketMapping);
    return EXIT_SUCCESS;
}