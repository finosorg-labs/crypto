/**
 * @file bench_crypto.c
 * @brief crypto module benchmark entry point
 *
 * This file serves as the main benchmark registration point for the crypto module.
 * Individual benchmark modules are in separate files:
 */

#include "bench_framework.h"
#include <simd_detect.h>
#include <stdio.h>

/* External benchmark functions from sub-modules */
extern void bench_weighted_median_run(void);

/* Entry point for crypto benchmarks */
void bench_crypto_run(void) {
    printf("\n");
    printf("============================================================\n");
    printf("  crypto Module Performance Benchmarks\n");
    printf("  SIMD level: %s\n", fc_simd_level_string(fc_get_simd_level()));
    printf("============================================================\n");

    /* Run all sub-module benchmarks */
    bench_weighted_median_run();

    printf("\n");
    printf("============================================================\n");
    printf("  crypto benchmarks complete\n");
    printf("============================================================\n");
}
