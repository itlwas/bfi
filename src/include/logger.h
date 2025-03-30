#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
typedef enum {
    ERR_SYNTAX = 0x100,
    ERR_RUNTIME = 0x200,
    ERR_MEMORY = 0x300,
    ERR_IO = 0x400,
    ERR_SYSTEM = 0x500
} ErrorCategory;
typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;
void LogMessage(LogLevel level, const char *message);
void TerminateWithError(const char *message);
void TerminateWithErrorCode(ErrorCategory category, int code, const char *message);
char *FormatError(const char *format, ...);
#define LogInfo(msg) LogMessage(LOG_INFO, (msg))
#define LogWarning(msg) LogMessage(LOG_WARN, (msg))
#define LogError(msg) LogMessage(LOG_ERROR, (msg))
#endif