#include "utils.h"
#include "logger.h"
#include <stdlib.h>
#include <ctype.h>
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