#ifndef BENCHMARK_H
#define BENCHMARK_H
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
typedef struct {
    clock_t startTime;
    clock_t endTime;
} Timer;
typedef struct {
    Timer timer;
    unsigned long long instructions;
    size_t memUsage;
    size_t ptrPos;
    size_t initTapeSize;
    size_t finalTapeSize;
} BenchData;
void BenchStart(BenchData *data);
void BenchStop(BenchData *data, unsigned long long instructions, size_t memUsed, size_t ptrPos, size_t finalTapeSize);
void BenchReport(const BenchData *data);
#endif 