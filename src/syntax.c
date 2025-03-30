#include "syntax.h"
#include "memory.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
size_t *CreateBracketMapping(const char *programCode, size_t codeLength) {
    if (!programCode || codeLength == 0) {
        TerminateWithErrorCode(ERR_SYNTAX, 4, "Invalid program code for bracket mapping");
        return NULL;
    }
    size_t *bracketMapping = (size_t *)AllocateMemory(sizeof(size_t) * codeLength);
    size_t *bracketStack = (size_t *)AllocateMemory(sizeof(size_t) * codeLength);
    memset(bracketMapping, 0, sizeof(size_t) * codeLength);
    memset(bracketStack, 0, sizeof(size_t) * codeLength);
    size_t stackTop = 0;
    size_t bracketCount = 0;
    for (size_t pos = 0; pos < codeLength; pos++) {
        if (programCode[pos] == '[') {
            bracketStack[stackTop++] = pos;
            bracketCount++;
        }
        else if (programCode[pos] == ']') {
            if (stackTop == 0) {
                free(bracketStack);
                free(bracketMapping);
                TerminateWithErrorCode(ERR_SYNTAX, 1, FormatError("Unmatched closing bracket ']' at position %zu", pos));
                return NULL;
            }
            size_t openPos = bracketStack[--stackTop];
            bracketMapping[openPos] = pos;
            bracketMapping[pos] = openPos;
            bracketCount++;
        }
    }
    if (stackTop != 0) {
        char *errMsg = FormatError("Missing closing bracket for '[' at position %zu", bracketStack[stackTop - 1]);
        free(bracketStack);
        free(bracketMapping);
        TerminateWithErrorCode(ERR_SYNTAX, 2, errMsg);
        return NULL;
    }
    if (bracketCount == 0) {
        free(bracketStack);
        free(bracketMapping);
        LogWarning("No brackets found in program - program may not contain loops");
        return NULL;
    }
    free(bracketStack);
    return bracketMapping;
}