/**
 * @file bench_main.c
 * @brief Benchmark runner entry point for crypto module
 */

#include "bench_framework.h"
#include <platform.h>
#include <simd_detect.h>
#include <stdio.h>

/* External benchmark suites */
extern void bench_crypto_run(void);

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    fc_init();

    fc_bench_init();

    printf("crypto performance benchmarks v%s\n", FC_BENCH_VERSION);

    bench_crypto_run();

    fc_bench_cleanup();
    return 0;
}
