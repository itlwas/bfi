#include "logger.h"
#include "memory.h"
#include "bfi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
size_t gMaxFileSizeAllowed = MAX_FILE_SIZE;
static inline void WriteLog(const char *level, const char *message) {
    time_t now = time(NULL);
    char buffer[128];
    const char *color = CLR_RESET;
    if (strcmp(level, "INFO") == 0) {
        color = CLR_GREEN;
    } else if (strcmp(level, "WARN") == 0) {
        color = CLR_YELLOW;
    } else if (strcmp(level, "ERR") == 0) {
        color = CLR_RED;
    }
    int len = snprintf(buffer, sizeof(buffer), "%s[%s]%s [%ld] ", color, level, CLR_RESET, (long)now);
    fwrite(buffer, 1, (size_t)len, stderr);
    fputs(message, stderr);
    fputc('\n', stderr);
}
void LogInfo(const char *message) { WriteLog("INFO", message); }
void LogWarning(const char *message) { WriteLog("WARN", message); }
void LogError(const char *message) { WriteLog("ERR", message); }
void TerminateWithError(const char *message) {
    char output[256];
    snprintf(output, sizeof(output), "%sInterpreter Error: %s%s", CLR_RED, message, CLR_RESET);
    fputs(output, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}
char *FormatError(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    int required = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    char *errString = AllocateMemory(required + 1);
    if (required < (int)sizeof(buffer))
        memcpy(errString, buffer, required + 1);
    else {
        va_start(args, format);
        vsnprintf(errString, required + 1, format, args);
        va_end(args);
    }
    return errString;
}