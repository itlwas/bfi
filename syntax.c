#include "syntax.h"
#include "memory.h"
#include "logger.h"
#include <stdlib.h>
size_t *CreateBracketMapping(const char *programCode, size_t codeLength) {
    size_t *bracketMapping = (size_t *)AllocateMemory(sizeof(size_t) * codeLength);
    size_t *bracketStack = (size_t *)AllocateMemory(sizeof(size_t) * codeLength);
    size_t stackTop = 0;
    for (size_t pos = 0; pos < codeLength; pos++) {
        if (programCode[pos] == '[')
            bracketStack[stackTop++] = pos;
        else if (programCode[pos] == ']') {
            if (stackTop == 0) {
                free(bracketStack);
                free(bracketMapping);
                TerminateWithErrorCode(ERR_SYNTAX, 1, "Unmatched closing bracket ']'");
            }
            size_t openPos = bracketStack[--stackTop];
            bracketMapping[openPos] = pos;
            bracketMapping[pos] = openPos;
        }
    }
    if (stackTop != 0) {
        char *errMsg = FormatError("Missing closing bracket for '[' at position %zu", bracketStack[stackTop - 1]);
        free(bracketStack);
        free(bracketMapping);
        TerminateWithErrorCode(ERR_SYNTAX, 2, errMsg);
    }
    free(bracketStack);
    return bracketMapping;
}