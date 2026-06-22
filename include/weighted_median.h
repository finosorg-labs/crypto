#ifndef FC_CRYPTO_WEIGHTED_MEDIAN_H
#define FC_CRYPTO_WEIGHTED_MEDIAN_H

#include <error.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute weighted median for a single dataset
 *
 * Weighted median is the value at which cumulative weight reaches 50% of total weight.
 * Used for anti-manipulation price aggregation from multiple exchanges.
 *
 * Algorithm: Sort by value, compute prefix sum of weights, find first position where
 * cumulative weight >= total_weight/2.
 *
 * @param values Array of values (e.g., prices from multiple exchanges)
 * @param weights Array of corresponding weights (e.g., volume or credibility)
 * @param n Number of elements (must be > 0)
 * @param result Output weighted median value
 * @return FC_OK on success, error code otherwise:
 *         FC_ERR_INVALID_ARG: NULL pointer or n == 0
 *         FC_ERR_NAN_INPUT: Weights contain NaN, Inf, or negative values
 *         FC_ERR_DIVIDE_BY_ZERO: All weights are zero
 *         FC_ERR_OUT_OF_MEMORY: Memory allocation failed
 *
 * @note Time complexity: O(n log n) due to sorting
 * @note Space complexity: O(n) for temporary index array
 * @note Thread safety: Thread-safe if different threads use different input arrays
 */
fc_status_t fc_crypto_weighted_median_f64(
    const double* values,
    const double* weights,
    size_t n,
    double* result
);

/**
 * @brief Compute weighted median for multiple datasets in batch
 *
 * Batch processing for computing weighted medians across multiple independent datasets.
 * Each dataset can have different size. Used for computing mark prices for multiple contracts.
 *
 * @param values Concatenated array of all value arrays
 * @param weights Concatenated array of all weight arrays
 * @param offsets Array of starting offsets for each dataset (length = batch_size)
 * @param sizes Array of sizes for each dataset (length = batch_size)
 * @param batch_size Number of datasets
 * @param results Output array of weighted medians (length = batch_size)
 * @return FC_OK on success, error code otherwise:
 *         FC_ERR_INVALID_ARG: NULL pointer, batch_size == 0, or dataset size == 0
 *         FC_ERR_NAN_INPUT: Invalid weights in any dataset
 *         FC_ERR_DIVIDE_BY_ZERO: All weights zero in any dataset
 *         FC_ERR_OUT_OF_MEMORY: Memory allocation failed
 *
 * @note Each dataset is processed independently
 * @note Thread safety: Thread-safe if different threads use different input arrays
 *
 * Example:
 *   Dataset 0: values[0..2], weights[0..2], size=3
 *   Dataset 1: values[3..7], weights[3..7], size=5
 *   offsets = {0, 3}, sizes = {3, 5}, batch_size = 2
 */
fc_status_t fc_crypto_weighted_median_batch_f64(
    const double* values,
    const double* weights,
    const size_t* offsets,
    const size_t* sizes,
    size_t batch_size,
    double* results
);

#ifdef __cplusplus
}
#endif

#endif // FC_CRYPTO_WEIGHTED_MEDIAN_H
