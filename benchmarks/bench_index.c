/**
 * @file bench_index.c
 * @brief Performance benchmarks for crypto index calculation
 *
 * Measures index calculation performance at various component counts
 * and compares SIMD vs scalar implementations.
 */

#include "bench_framework.h"
#include "index.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Helper: initialize test data */
static void init_test_data(double* prices, double* weights, int32_t n) {
    for (int32_t i = 0; i < n; i++) {
        prices[i] = 1000.0 + i * 10.0;
        weights[i] = 1.0 / n;
    }
}

/* Benchmark: Single index calculation */
static void bench_index_calculate_func(void* data) {
    double** arrays = (double**)data;
    double* prices = arrays[0];
    double* weights = arrays[1];
    int32_t n = (int32_t)(intptr_t)arrays[2];
    double index_value;
    fc_index_calculate(prices, weights, n, 1.0, &index_value);
}

static void bench_index_calculate(int32_t n) {
    double* prices = (double*)malloc(n * sizeof(double));
    double* weights = (double*)malloc(n * sizeof(double));

    init_test_data(prices, weights, n);

    void* data[3] = {prices, weights, (void*)(intptr_t)n};

    char name[64];
    snprintf(name, sizeof(name), "index_calculate_n%d", n);

    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    config.name = name;
    config.data_size = n * sizeof(double) * 2;

    fc_bench_result_t result;
    fc_bench_run(&config, bench_index_calculate_func, data, &result);
    fc_bench_result_print(&result);

    free(prices);
    free(weights);
}

/* Benchmark: Market cap weights */
static void bench_weights_market_cap_func(void* data) {
    double** arrays = (double**)data;
    double* market_caps = arrays[0];
    double* weights = arrays[1];
    int32_t n = (int32_t)(intptr_t)arrays[2];
    fc_index_weights_market_cap(market_caps, n, weights);
}

static void bench_weights_market_cap(int32_t n) {
    double* market_caps = (double*)malloc(n * sizeof(double));
    double* weights = (double*)malloc(n * sizeof(double));

    for (int32_t i = 0; i < n; i++) {
        market_caps[i] = 1000000.0 + i * 10000.0;
    }

    void* data[3] = {market_caps, weights, (void*)(intptr_t)n};

    char name[64];
    snprintf(name, sizeof(name), "weights_market_cap_n%d", n);

    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    config.name = name;
    config.data_size = n * sizeof(double) * 2;

    fc_bench_result_t result;
    fc_bench_run(&config, bench_weights_market_cap_func, data, &result);
    fc_bench_result_print(&result);

    free(market_caps);
    free(weights);
}

/* Benchmark: Real-world scenario - DeFi index with 50 components */
static void bench_defi_index_scenario(void) {
    const int32_t n = 50;
    const int32_t num_updates = 1000;

    double* prices = (double*)malloc(n * sizeof(double));
    double* weights = (double*)malloc(n * sizeof(double));
    double* market_caps = (double*)malloc(n * sizeof(double));
    double index_value;

    printf("\n=== DeFi Index Scenario (50 components, 1000 updates) ===\n");

    for (int32_t i = 0; i < n; i++) {
        market_caps[i] = 100000000.0 * (n - i);
    }

    fc_bench_time_t start = fc_bench_time_now();

    fc_index_weights_market_cap(market_caps, n, weights);

    for (int32_t update = 0; update < num_updates; update++) {
        for (int32_t i = 0; i < n; i++) {
            prices[i] = 100.0 + i * 5.0 + (update % 10) * 0.5;
        }
        fc_index_calculate(prices, weights, n, 1.0, &index_value);
    }

    fc_bench_time_t end = fc_bench_time_now();

    double elapsed_ms = fc_bench_time_elapsed_ms(&start, &end);
    double updates_per_sec = (num_updates / elapsed_ms) * 1000.0;
    double latency_us = (elapsed_ms / num_updates) * 1000.0;

    printf("Total time: %.3f ms\n", elapsed_ms);
    printf("Updates/sec: %.0f\n", updates_per_sec);
    printf("Latency per update: %.3f μs\n", latency_us);
    printf("Final index value: %.2f\n", index_value);

    free(prices);
    free(weights);
    free(market_caps);
}

/* Main benchmark entry point */
void bench_index_run(void) {
    printf("\n--- Single Index Calculation (varying components) ---\n");
    bench_index_calculate(10);
    bench_index_calculate(50);
    bench_index_calculate(100);
    bench_index_calculate(500);
    bench_index_calculate(1000);

    printf("\n--- Weight Calculation ---\n");
    bench_weights_market_cap(50);
    bench_weights_market_cap(100);
    bench_weights_market_cap(500);
    bench_weights_market_cap(1000);

    bench_defi_index_scenario();
}
