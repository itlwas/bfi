#include "bfi.h"
#include "memory.h"
#include "logger.h"
#include "fileio.h"
#include "syntax.h"
#include "interpreter.h"
#include "utils.h"
#include "benchmark.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
static inline void BenchRun(const char *srcCode, size_t codeSize, const size_t *brackets, bool debug, size_t snapWidth, size_t initTape, size_t maxTape, unsigned long long maxIter, int eofMode) {
    unsigned char *tape = NULL;
    size_t tapeSize = 0, ptrPos = 0;
    BenchData bench;
    bench.initTapeSize = initTape;
    BenchStart(&bench);
    ExecuteBrainfuck(srcCode, codeSize, brackets, debug, snapWidth, initTape, maxTape, maxIter, eofMode, &tape, &tapeSize, &ptrPos, true);
    BenchStop(&bench, execInstrCount, execMemPeak, ptrPos, tapeSize);
    BenchReport(&bench);
}
static inline void Run(const char *srcCode, size_t codeSize, const size_t *brackets, bool debug, size_t snapWidth, size_t initTape, size_t maxTape, unsigned long long maxIter, int eofMode) {
    unsigned char *tape = NULL;
    size_t tapeSize = 0, ptrPos = 0;
    ExecuteBrainfuck(srcCode, codeSize, brackets, debug, snapWidth, initTape, maxTape, maxIter, eofMode, &tape, &tapeSize, &ptrPos, true);
}
int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
    setvbuf(stdin, NULL, _IOLBF, BUFSIZ);
    if (argc < 2) 
        TerminateWithErrorCode(ERR_SYSTEM, 1, "No source file specified");
    bool debugEnabled = false;
    bool benchEnabled = false;
    const char *srcFile = NULL;
    size_t snapWidth = SNAPSHOT_WIDTH, initTape = INITIAL_TAPE_CAPACITY, maxTape = MAX_TAPE_CAPACITY;
    unsigned long long maxIter = MAX_ITERATIONS;
    int eofMode = 0;
    const CmdOption options[] = {
        {'d', "debug", 0, "Enable debug output"},
        {'v', "version", 0, "Display version information and exit"},
        {'i', "iterations", 1, "Maximum number of iterations to execute"},
        {'w', "snapshot-width", 1, "Number of cells to display in each direction during debugging"},
        {'t', "initial-tape", 1, "Initial tape capacity in cells"},
        {'M', "max-tape", 1, "Maximum tape capacity in cells"},
        {'e', "eof-mode", 1, "EOF handling mode: 0=no change, 1=set to 0, 2=set to 255"},
        {'b', "benchmark", 0, "Enable benchmark mode"},
        {'h', "help", 0, "Display this help message and exit"}
    };
    const int optCount = sizeof(options) / sizeof(options[0]);
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            int optIdx = FindOption(options, optCount, argv[i]);
            if (optIdx >= 0) {
                switch (options[optIdx].shortOpt) {
                    case 'd': debugEnabled = true; break;
                    case 'v': printf("BFI %s\n", BFI_VERSION); return EXIT_SUCCESS;
                    case 'h': PrintHelp(argv[0], options, optCount); return EXIT_SUCCESS;
                    case 'b': benchEnabled = true; break;
                    case 'i': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing iteration limit parameter");
                        maxIter = ParseULL(arg);
                        if (maxIter == 0) 
                            TerminateWithErrorCode(ERR_SYSTEM, 2, "Invalid iteration limit");
                        break;
                    }
                    case 'w': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing snapshot width parameter");
                        snapWidth = (size_t)ParseULL(arg);
                        if (snapWidth == 0) 
                            TerminateWithErrorCode(ERR_SYSTEM, 3, "Invalid snapshot width");
                        break;
                    }
                    case 't': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing initial tape capacity parameter");
                        initTape = (size_t)ParseULL(arg);
                        if (initTape == 0) 
                            TerminateWithErrorCode(ERR_SYSTEM, 4, "Invalid initial tape capacity");
                        break;
                    }
                    case 'M': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing maximum tape capacity parameter");
                        maxTape = (size_t)ParseULL(arg);
                        if (maxTape < initTape) 
                            TerminateWithErrorCode(ERR_SYSTEM, 5, "Maximum tape capacity must be >= initial capacity");
                        break;
                    }
                    case 'e': {
                        const char *arg = FetchNextArgument(argc, argv, &i, "Missing EOF mode parameter");
                        eofMode = (int)ParseULL(arg);
                        if (eofMode < 0 || eofMode > 2) 
                            TerminateWithErrorCode(ERR_SYSTEM, 6, "Invalid EOF mode (valid values: 0, 1, 2)");
                        break;
                    }
                }
            } else {
                TerminateWithErrorCode(ERR_SYSTEM, 7, FormatError("Unknown option: %s", argv[i]));
            }
        } else if (!srcFile) {
            srcFile = argv[i];
        } else {
            TerminateWithErrorCode(ERR_SYSTEM, 8, "Multiple input files specified");
        }
    }
    if (!srcFile) 
        TerminateWithErrorCode(ERR_SYSTEM, 9, "No source file specified");
    size_t codeSize = 0;
    char *srcCode = ReadFileContent(srcFile, &codeSize);
    size_t *brackets = CreateBracketMapping(srcCode, codeSize);
    
    if (benchEnabled) {
        BenchRun(srcCode, codeSize, brackets, debugEnabled, snapWidth, initTape, maxTape, maxIter, eofMode);
    } else {
        Run(srcCode, codeSize, brackets, debugEnabled, snapWidth, initTape, maxTape, maxIter, eofMode);
    }
    free(srcCode);
    free(brackets);
    return EXIT_SUCCESS;
}