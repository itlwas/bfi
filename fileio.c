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
        TerminateWithErrorCode(ERR_IO, 1, FormatError("Cannot open file '%s'", fileName));
    size_t size = fileStats.st_size;
    if (size > gMaxFileSizeAllowed)
        TerminateWithErrorCode(ERR_IO, 2, FormatError("File '%s' exceeds maximum size limit (%zu bytes)", fileName, gMaxFileSizeAllowed));
    FILE *fileHandle = fopen(fileName, "rb");
    if (!fileHandle)
        TerminateWithErrorCode(ERR_IO, 3, FormatError("Cannot read file '%s'", fileName));
    char *buffer = AllocateMemory(size + 1);
    if (fread(buffer, 1, size, fileHandle) != size) {
        fclose(fileHandle);
        free(buffer);
        TerminateWithErrorCode(ERR_IO, 4, FormatError("Read error in file '%s'", fileName));
    }
    fclose(fileHandle);
    buffer[size] = '\0';
    if (fileSize)
        *fileSize = size;
    return buffer;
}