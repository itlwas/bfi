#include "logger.h"
#include "memory.h"
#include "bfi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
size_t gMaxFileSizeAllowed = MAX_FILE_SIZE;
static const char* const LOG_LEVEL_NAMES[] = {"INFO", "WARN", "ERR"};
static const char* const LOG_LEVEL_COLORS[] = {CLR_GREEN, CLR_YELLOW, CLR_RED};
static const char* const ERROR_CATEGORY_NAMES[] = {"SYNTAX", "RUNTIME", "MEMORY", "IO", "SYSTEM", "UNKNOWN"};
void LogMessage(LogLevel level, const char *message) {
    if (level < LOG_INFO || level > LOG_ERROR) return;
    fputs(LOG_LEVEL_COLORS[level], stderr);
    fputs("[", stderr);
    fputs(LOG_LEVEL_NAMES[level], stderr);
    fputs("]", stderr);
    fputs(CLR_RESET, stderr);
    fputs(" ", stderr);
    fputs(message, stderr);
    fputc('\n', stderr);
}
void TerminateWithError(const char *message) {
    fputs(CLR_RED, stderr);
    fputs("ERROR: ", stderr);
    fputs(message, stderr);
    fputs(CLR_RESET, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}
void TerminateWithErrorCode(ErrorCategory category, int code, const char *message) {
    int categoryIndex = (category >= ERR_SYNTAX && category <= ERR_SYSTEM) ? 
                        (category / 0x100) - 1 : 5;
    int errorCode = category + code;
    fputs(CLR_RED, stderr);
    fputs(ERROR_CATEGORY_NAMES[categoryIndex], stderr);
    fputs(" ERROR [", stderr);
    char codeStr[8];
    snprintf(codeStr, sizeof(codeStr), "%04X", errorCode);
    fputs(codeStr, stderr);
    fputs("]: ", stderr);
    fputs(message, stderr);
    fputs(CLR_RESET, stderr);
    fputc('\n', stderr);
    bool shouldFree = message != NULL;
    if (shouldFree && message[0] != '\0' && 
        ((category == ERR_SYSTEM && (code == 7 || code == 8 || code == 9 || code == 1)) || 
         (message[0] == 'N' && strcmp(message, "No brackets found") == 0))) {
        shouldFree = false;
    }
    if (shouldFree) {
        free((void*)message);
    }
    exit(EXIT_FAILURE);
}
char *FormatError(const char *format, ...) {
    va_list args, argsCopy;
    va_start(args, format);
    va_copy(argsCopy, args);
    int required = vsnprintf(NULL, 0, format, args);
    va_end(args);
    if (required < 0) {
        va_end(argsCopy);
        return NULL;
    }
    char *errString = AllocateMemory(required + 1);
    vsnprintf(errString, required + 1, format, argsCopy);
    va_end(argsCopy);
    return errString;
}
void FreeError(char *errorString) {
    if (errorString) {
        free(errorString);
    }
}