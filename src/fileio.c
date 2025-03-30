#include "fileio.h"
#include "logger.h"
#include "memory.h"
#include "bfi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <sys/stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
char *ReadFileContent(const char *fileName, size_t *fileSize) {
    if (!fileName || !fileSize)
        TerminateWithErrorCode(ERR_IO, 6, "Invalid file name or size parameter"); 
    *fileSize = 0;
    struct stat fileStats;
    if (stat(fileName, &fileStats) != 0)
        TerminateWithErrorCode(ERR_IO, 1, FormatError("Cannot open file '%s'", fileName));
    if (!S_ISREG(fileStats.st_mode))
        TerminateWithErrorCode(ERR_IO, 7, FormatError("'%s' is not a regular file", fileName));
    size_t size = fileStats.st_size;
    if (size == 0)
        TerminateWithErrorCode(ERR_IO, 8, FormatError("File '%s' is empty", fileName));
    if (size > gMaxFileSizeAllowed)
        TerminateWithErrorCode(ERR_IO, 2, FormatError("File '%s' exceeds maximum size limit (%zu bytes)", fileName, gMaxFileSizeAllowed));
    FILE *fileHandle = fopen(fileName, "rb");
    if (!fileHandle)
        TerminateWithErrorCode(ERR_IO, 3, FormatError("Cannot read file '%s'", fileName));
    char *buffer = AllocateMemory(size + 1);
    size_t bytesRead = fread(buffer, 1, size, fileHandle);
    if (bytesRead != size) {
        fclose(fileHandle);
        free(buffer);
        TerminateWithErrorCode(ERR_IO, 4, FormatError("Read error in file '%s'", fileName));
    }
    fclose(fileHandle);
    buffer[size] = '\0';
    *fileSize = size;
    return buffer;
}