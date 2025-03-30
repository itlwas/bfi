#ifndef UTILS_H
#define UTILS_H
typedef struct {
    char shortOpt;
    const char* longOpt;
    int hasArg;
    const char* description;
} CmdOption;
unsigned long long ParseULL(const char *str);
unsigned long long ConvertStringToULL(const char *str, char **endPtr, int base);
const char *FetchNextArgument(int argc, char *argv[], int *index, const char *errorMessage);
int FindOption(const CmdOption* options, int optCount, const char* arg);
void PrintHelp(const char* programName, const CmdOption* options, int optCount);
#endif