#include "utils.h"
#include "logger.h"
#include "bfi.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
unsigned long long ParseULL(const char *str) {
    unsigned long long result = 0;
    if (!str) return 0;
    while ((unsigned)(*str - '0') < 10)
        result = result * 10 + (*str++ - '0');
    return result;
}
unsigned long long ConvertStringToULL(const char *str, char **endPtr, int base) {
    unsigned long long number = 0;
    while (*str == ' ' || (*str >= 9 && *str <= 13))
        str++;
    if (!base) {
        if (str[0] == '0') {
            if (str[1] == 'x' || str[1] == 'X') { base = 16; str += 2; }
            else { base = 8; str++; }
        } else {
            base = 10;
        }
    }
    while (*str) {
        int digit;
        char ch = *str;
        if (ch >= '0' && ch <= '9')
            digit = ch - '0';
        else if (ch >= 'A' && ch <= 'Z')
            digit = ch - 'A' + 10;
        else if (ch >= 'a' && ch <= 'z')
            digit = ch - 'a' + 10;
        else break;
        if (digit >= base)
            break;
        number = number * base + digit;
        str++;
    }
    if (endPtr)
        *endPtr = (char *)str;
    return number;
}
const char *FetchNextArgument(int argc, char *argv[], int *index, const char *errorMessage) {
    if (++(*index) < argc)
        return argv[*index];
    TerminateWithError(errorMessage);
    return NULL;
}
int FindOption(const CmdOption* options, int optCount, const char* arg) {
    if (!arg || arg[0] != '-' || arg[1] == '\0')
        return -1;
    if (arg[1] == '-') {
        const char *key = arg + 2;
        for (int i = 0; i < optCount; i++) {
            const char *opt = options[i].longOpt;
            if (!opt)
                continue;
            const char *a = key, *b = opt;
            while (*a && *b && *a == *b)
                a++, b++;
            if (!*a && !*b)
                return i;
        }
    } else if (arg[2] == '\0') {
        for (int i = 0; i < optCount; i++)
            if (options[i].shortOpt == arg[1])
                return i;
    }
    return -1;
}
void PrintHelp(const char *programName, const CmdOption *options, int optCount) {
    printf("Usage: %s [options] filename\n\nOptions:\n", programName);
    for (int i = 0; i < optCount; i++) {
        char buf[256];
        int off = options[i].shortOpt 
                  ? (options[i].longOpt 
                        ? sprintf(buf, "  -%c, --%s", options[i].shortOpt, options[i].longOpt) 
                        : sprintf(buf, "  -%c", options[i].shortOpt))
                  : sprintf(buf, "    --%s", options[i].longOpt);
        if (options[i].hasArg)
            off += sprintf(buf + off, " <arg>");
        int pad = off < 30 ? 30 - off : 0;
        printf("%s%*s%s\n", buf, pad, "", options[i].description);
    }
    printf("\nBrainfuck Interpreter %s\n", BFI_VERSION);
}