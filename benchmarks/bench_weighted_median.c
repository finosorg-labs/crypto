#include "bench_framework.h"
#include "weighted_median.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    double* values;
    double* weights;
    double* values_backup;
    double* weights_backup;
    size_t* offsets;
    size_t* sizes;
    double* results;
    size_t n;
    size_t batch_size;
} bench_wm_ctx_t;

static void bench_wm_setup(bench_wm_ctx_t* ctx, size_t n) {
    ctx->n = n;
    ctx->values = (double*)malloc(n * sizeof(double));
    ctx->weights = (double*)malloc(n * sizeof(double));
    ctx->values_backup = (double*)malloc(n * sizeof(double));
    ctx->weights_backup = (double*)malloc(n * sizeof(double));
    ctx->results = (double*)malloc(sizeof(double));

    for (size_t i = 0; i < n; i++) {
        ctx->values[i] = 40000.0 + (double)(i % 20000);
        ctx->weights[i] = 1.0 + (double)(i % 1000);
    }
    memcpy(ctx->values_backup, ctx->values, n * sizeof(double));
    memcpy(ctx->weights_backup, ctx->weights, n * sizeof(double));
}

static void bench_wm_batch_setup(bench_wm_ctx_t* ctx, size_t n, size_t batch_size) {
    ctx->n = n;
    ctx->batch_size = batch_size;
    size_t total_size = batch_size * n;

    ctx->values = (double*)malloc(total_size * sizeof(double));
    ctx->weights = (double*)malloc(total_size * sizeof(double));
    ctx->values_backup = (double*)malloc(total_size * sizeof(double));
    ctx->weights_backup = (double*)malloc(total_size * sizeof(double));
    ctx->offsets = (size_t*)malloc(batch_size * sizeof(size_t));
    ctx->sizes = (size_t*)malloc(batch_size * sizeof(size_t));
    ctx->results = (double*)malloc(batch_size * sizeof(double));

    for (size_t b = 0; b < batch_size; b++) {
        ctx->offsets[b] = b * n;
        ctx->sizes[b] = n;
        for (size_t i = 0; i < n; i++) {
            size_t idx = b * n + i;
            ctx->values[idx] = 40000.0 + (double)(idx % 20000);
            ctx->weights[idx] = 1.0 + (double)(idx % 1000);
        }
    }
    memcpy(ctx->values_backup, ctx->values, total_size * sizeof(double));
    memcpy(ctx->weights_backup, ctx->weights, total_size * sizeof(double));
}

static void bench_wm_teardown(bench_wm_ctx_t* ctx) {
    free(ctx->values);
    free(ctx->weights);
    free(ctx->values_backup);
    free(ctx->weights_backup);
    free(ctx->results);
}

static void bench_wm_batch_teardown(bench_wm_ctx_t* ctx) {
    bench_wm_teardown(ctx);
    free(ctx->offsets);
    free(ctx->sizes);
}

static void bench_wm_reset(bench_wm_ctx_t* ctx) {
    memcpy(ctx->values, ctx->values_backup, ctx->n * sizeof(double));
    memcpy(ctx->weights, ctx->weights_backup, ctx->n * sizeof(double));
}

static void bench_wm_batch_reset(bench_wm_ctx_t* ctx) {
    size_t total_size = ctx->batch_size * ctx->n;
    memcpy(ctx->values, ctx->values_backup, total_size * sizeof(double));
    memcpy(ctx->weights, ctx->weights_backup, total_size * sizeof(double));
}

static void bench_weighted_median_single(void* user_data) {
    bench_wm_ctx_t* ctx = (bench_wm_ctx_t*)user_data;
    bench_wm_reset(ctx);
    fc_crypto_weighted_median_f64(ctx->values, ctx->weights, ctx->n, ctx->results);
}

static void bench_weighted_median_batch(void* user_data) {
    bench_wm_ctx_t* ctx = (bench_wm_ctx_t*)user_data;
    bench_wm_batch_reset(ctx);
    fc_crypto_weighted_median_batch_f64(ctx->values, ctx->weights, ctx->offsets, ctx->sizes,
                                        ctx->batch_size, ctx->results);
}

static void run_bench_suite_single(const char* name, size_t n) {
    bench_wm_ctx_t ctx;
    bench_wm_setup(&ctx, n);

    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    config.name = name;
    config.data_size = n * sizeof(double) * 2;
    config.min_time_ms = 200.0;
    config.warmup_ms = 50.0;

    fc_bench_result_t result;
    fc_bench_run(&config, bench_weighted_median_single, &ctx, &result);

    bench_wm_teardown(&ctx);
}

static void run_bench_suite_batch(const char* name, size_t n, size_t batch_size) {
    bench_wm_ctx_t ctx;
    bench_wm_batch_setup(&ctx, n, batch_size);

    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    config.name = name;
    config.data_size = batch_size * n * sizeof(double) * 2;
    config.min_time_ms = 200.0;
    config.warmup_ms = 50.0;

    fc_bench_result_t result;
    fc_bench_run(&config, bench_weighted_median_batch, &ctx, &result);

    bench_wm_batch_teardown(&ctx);
}

void bench_weighted_median_run(void) {
    printf("\nWeighted Median Benchmarks\n");
    printf("==========================\n\n");

    printf("Single dataset:\n");
    run_bench_suite_single("wm_5", 5);
    run_bench_suite_single("wm_10", 10);
    run_bench_suite_single("wm_50", 50);
    run_bench_suite_single("wm_100", 100);

    printf("\nBatch processing:\n");
    run_bench_suite_batch("wm_batch_5x100", 5, 100);
    run_bench_suite_batch("wm_batch_5x1K", 5, 1000);
    run_bench_suite_batch("wm_batch_5x10K", 5, 10000);
    run_bench_suite_batch("wm_batch_10x1K", 10, 1000);
}
