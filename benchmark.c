#include "benchmark.h"
#include "bfi.h"
#include <stdio.h>
#include <time.h>
static inline double TimeDelta(const Timer *timer) {
    return ((double)(timer->endTime - timer->startTime)) / CLOCKS_PER_SEC;
}
void BenchStart(BenchData *data) {
    if (!data) return;
    data->timer.startTime = clock();
    data->timer.endTime = 0;
    data->instructions = 0;
    data->memUsage = 0;
    data->ptrPos = 0;
    data->initTapeSize = 0;
    data->finalTapeSize = 0;
}
void BenchStop(BenchData *data, unsigned long long instructions, size_t memUsed, size_t ptrPos, size_t finalTapeSize) {
    if (!data) return;
    data->timer.endTime = clock();
    data->instructions = instructions;
    data->memUsage = memUsed;
    data->ptrPos = ptrPos;
    data->finalTapeSize = finalTapeSize;
}
void BenchReport(const BenchData *data) {
    if (!data) return;
    double execTime = TimeDelta(&data->timer);
    double ips = data->instructions / execTime;
    printf("\n%s===== Benchmark Results =====%s\n", CLR_BOLD, CLR_RESET);
    printf("Execution Time: %s%.3f seconds%s\n", CLR_YELLOW, execTime, CLR_RESET);
    printf("Instructions Executed: %s%llu%s\n", CLR_GREEN, data->instructions, CLR_RESET);
    printf("Instructions Per Second: %s%.2f%s\n", CLR_CYAN, ips, CLR_RESET);
    printf("Maximum Memory Used: %s%zu bytes%s\n", CLR_MAGENTA, data->memUsage, CLR_RESET);
    printf("%s============================%s\n\n", CLR_BOLD, CLR_RESET);
}