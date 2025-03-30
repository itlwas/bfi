#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <stdlib.h>
#include <stdbool.h>
extern size_t execMemPeak;
extern unsigned long long execInstrCount;
void ExecuteBrainfuck(const char *programCode,
    size_t codeLength,
    const size_t *bracketMapping,
    bool debugMode,
    size_t snapshotWidth,
    size_t initialTapeCapacity,
    size_t maxTapeCapacity,
    unsigned long long maxIterations,
    int eofHandlingMode,
    unsigned char **tapeRef,
    size_t *tapeCapacity,
    size_t *dataIndex,
    bool autoReleaseTape);
#endif