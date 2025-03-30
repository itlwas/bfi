#include "bfi.h"
#include "memory.h"
#include "logger.h"
#include "fileio.h"
#include "syntax.h"
#include "interpreter.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
    setvbuf(stdin, NULL, _IOLBF, BUFSIZ);
    if (argc < 2) TerminateWithError("No Brainfuck source file specified.");
    bool debugEnabled = false;
    const char *sourceFileName = NULL;
    size_t snapshotWidth = SNAPSHOT_WIDTH, initialTapeCapacity = INITIAL_TAPE_CAPACITY, maxTapeCapacity = MAX_TAPE_CAPACITY;
    unsigned long long maxIterations = MAX_ITERATIONS;
    int eofMode = 0;
    const CmdOption options[] = {
        {'d', "debug", 0, "Enable debug output"},
        {'v', "version", 0, "Display version information and exit"},
        {'i', "iterations", 1, "Maximum number of iterations to execute"},
        {'w', "snapshot-width", 1, "Number of cells to display in each direction during debugging"},
        {'t', "initial-tape", 1, "Initial tape capacity in cells"},
        {'M', "max-tape", 1, "Maximum tape capacity in cells"},
        {'e', "eof-mode", 1, "EOF handling mode: 0=no change, 1=set to 0, 2=set to 255"},
        {'h', "help", 0, "Display this help message and exit"}
    };
    const int optCount = sizeof(options) / sizeof(options[0]);
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            int optIndex = FindOption(options, optCount, argv[i]);
            if (optIndex >= 0) {
                switch (options[optIndex].shortOpt) {
                    case 'd': debugEnabled = true; break;
                    case 'v': printf("BFI %s\n", BFI_VERSION); return EXIT_SUCCESS;
                    case 'h': PrintHelp(argv[0], options, optCount); return EXIT_SUCCESS;
                    case 'i': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing iteration limit parameter.");
                        maxIterations = ParseULL(arg);
                        if (maxIterations == 0) TerminateWithError("Invalid iteration limit provided.");
                        break;
                    }
                    case 'w': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing snapshot width parameter.");
                        snapshotWidth = (size_t)ParseULL(arg);
                        if (snapshotWidth == 0) TerminateWithError("Invalid snapshot width provided for debugging.");
                        break;
                    }
                    case 't': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing initial tape capacity parameter.");
                        initialTapeCapacity = (size_t)ParseULL(arg);
                        if (initialTapeCapacity == 0) TerminateWithError("Invalid initial tape capacity provided.");
                        break;
                    }
                    case 'M': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing maximum tape capacity parameter.");
                        maxTapeCapacity = (size_t)ParseULL(arg);
                        if (maxTapeCapacity < initialTapeCapacity) TerminateWithError("Maximum tape capacity must be greater than or equal to initial tape capacity.");
                        break;
                    }
                    case 'e': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing EOF mode parameter.");
                        eofMode = (int)ParseULL(arg);
                        if (eofMode < 0 || eofMode > 2) TerminateWithError("Invalid EOF mode. Acceptable values are 0, 1, or 2.");
                        break;
                    }
                }
            } else {
                TerminateWithError(FormatError("Unknown option: %s", argv[i]));
            }
        } else if (!sourceFileName) {
            sourceFileName = argv[i];
        } else {
            TerminateWithError("Multiple input files specified. Provide only one source file.");
        }
    }
    if (!sourceFileName) TerminateWithError("No Brainfuck source file specified.");
    size_t sourceSize = 0;
    char *sourceContent = ReadFileContent(sourceFileName, &sourceSize);
    size_t *bracketMapping = CreateBracketMapping(sourceContent, sourceSize);
    unsigned char *tapeBuffer = NULL;
    size_t tapeCapacity = 0, dataIndex = 0;
    ExecuteBrainfuck(sourceContent, sourceSize, bracketMapping, debugEnabled, snapshotWidth, initialTapeCapacity, maxTapeCapacity, maxIterations, eofMode, &tapeBuffer, &tapeCapacity, &dataIndex, true);
    free(sourceContent);
    free(bracketMapping);
    return EXIT_SUCCESS;
}