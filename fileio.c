#include "fileio.h"
#include "logger.h"
#include "memory.h"
#include "bfi.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <sys/stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
char *ReadFileContent(const char *fileName, size_t *fileSize) {
    struct stat fileStats;
    if (stat(fileName, &fileStats) != 0)
        TerminateWithError(FormatError("Unable to open file '%s'. Check file existence and permissions.", fileName));
    size_t size = fileStats.st_size;
    if (size > gMaxFileSizeAllowed)
        TerminateWithError(FormatError("File '%s' exceeds permitted size of %zu bytes.", fileName, gMaxFileSizeAllowed));
    FILE *fileHandle = fopen(fileName, "rb");
    if (!fileHandle)
        TerminateWithError(FormatError("Unable to open file '%s'. Check file existence and permissions.", fileName));
    char *buffer = AllocateMemory(size + 1);
    if (fread(buffer, 1, size, fileHandle) != size) {
        fclose(fileHandle);
        free(buffer);
        TerminateWithError(FormatError("Incomplete file read from '%s'. File may be corrupted.", fileName));
    }
    fclose(fileHandle);
    buffer[size] = '\0';
    if (fileSize)
        *fileSize = size;
    return buffer;
}