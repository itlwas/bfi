#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
void LogInfo(const char *message);
void LogWarning(const char *message);
void LogError(const char *message);
void TerminateWithError(const char *message);
char *FormatError(const char *format, ...);
#endif