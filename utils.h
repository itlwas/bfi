#ifndef UTILS_H
#define UTILS_H
unsigned long long ConvertStringToULL(const char *str, char **endPtr, int base);
const char *FetchNextArgument(int argc, char *argv[], int *index, const char *errorMessage);
#endif