/**
 * @file index.h
 * @brief Crypto index construction and real-time calculation
 *
 * Provides high-performance calculation for cryptocurrency indices:
 * - Component weighting (market-cap, equal-weight, liquidity-weighted)
 * - Real-time price aggregation
 * - Index value calculation: Index = Σ(Price_i × Weight_i) / Divisor
 * - Rebalancing computation
 *
 * Design:
 * - Batch processing for multiple indices/timestamps
 * - Zero-copy weight updates
 * - SIMD-accelerated weighted summation
 * - Support for both static and dynamic divisor adjustment
 *
 * Time complexity: O(n) for n components per index calculation
 * Space complexity: O(1) auxiliary space
 *
 * Thread safety: All functions are thread-safe for different index instances.
 * Multiple threads can calculate different indices concurrently.
 */

#ifndef FC_INDEX_H
#define FC_INDEX_H

#include "error.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Weighting method for index construction
 */
typedef enum {
    FC_INDEX_WEIGHT_EQUAL      = 0, /**< Equal weight: 1/n for each component */
    FC_INDEX_WEIGHT_MARKET_CAP = 1, /**< Market capitalization weighted */
    FC_INDEX_WEIGHT_LIQUIDITY  = 2, /**< Liquidity weighted (e.g., trading volume) */
    FC_INDEX_WEIGHT_CUSTOM     = 3, /**< User-provided custom weights */
} fc_index_weight_method_t;

/**
 * @brief Calculate index value from component prices and weights
 *
 * Computes: index_value = Σ(prices[i] × weights[i]) / divisor
 *
 * @param prices Array of component prices (must not be NULL)
 * @param weights Array of component weights (must not be NULL, sum should be 1.0)
 * @param n Number of components (must be > 0)
 * @param divisor Index divisor (must be > 0, typically starts at 1.0)
 * @param index_value Output index value (must not be NULL)
 * @return FC_OK on success, error code otherwise
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 *
 * Example:
 * @code
 * double prices[] = {50000.0, 3000.0, 100.0};
 * double weights[] = {0.5, 0.3, 0.2};
 * double index_value;
 * fc_index_calculate(prices, weights, 3, 1.0, &index_value);
 * @endcode
 */
fc_status_t fc_index_calculate(
    const double* prices,
    const double* weights,
    int32_t n,
    double divisor,
    double* index_value
);

/**
 * @brief Batch calculate index values for multiple timestamps
 *
 * Computes index values for multiple sets of prices with the same weights.
 * Useful for backtesting or historical analysis.
 *
 * @param prices_matrix Matrix of prices [num_timestamps × n], row-major order
 * @param weights Array of component weights (must not be NULL)
 * @param n Number of components per timestamp
 * @param num_timestamps Number of timestamps to calculate
 * @param divisor Index divisor
 * @param index_values Output array of index values [num_timestamps] (must not be NULL)
 * @return FC_OK on success, error code otherwise
 *
 * Time complexity: O(num_timestamps × n)
 * Space complexity: O(1)
 */
fc_status_t fc_index_calculate_batch(
    const double* prices_matrix,
    const double* weights,
    int32_t n,
    int32_t num_timestamps,
    double divisor,
    double* index_values
);

/**
 * @brief Calculate equal weights for index components
 *
 * Sets weights[i] = 1.0 / n for all components.
 *
 * @param n Number of components (must be > 0)
 * @param weights Output array of weights [n] (must not be NULL)
 * @return FC_OK on success, error code otherwise
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 */
fc_status_t fc_index_weights_equal(int32_t n, double* weights);

/**
 * @brief Calculate market-cap weighted index weights
 *
 * Computes: weights[i] = market_caps[i] / Σ(market_caps)
 *
 * @param market_caps Array of market capitalizations (must not be NULL, all > 0)
 * @param n Number of components (must be > 0)
 * @param weights Output array of weights [n] (must not be NULL)
 * @return FC_OK on success, error code otherwise
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 */
fc_status_t fc_index_weights_market_cap(const double* market_caps, int32_t n, double* weights);

/**
 * @brief Calculate liquidity weighted index weights
 *
 * Computes: weights[i] = liquidity[i] / Σ(liquidity)
 *
 * @param liquidity Array of liquidity measures (e.g., 30-day volume) (must not be NULL, all >= 0)
 * @param n Number of components (must be > 0)
 * @param weights Output array of weights [n] (must not be NULL)
 * @return FC_OK on success, error code otherwise
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 */
fc_status_t fc_index_weights_liquidity(const double* liquidity, int32_t n, double* weights);

/**
 * @brief Normalize weights to sum to 1.0
 *
 * Adjusts weights so that Σ(weights) = 1.0
 *
 * @param weights Array of weights to normalize (must not be NULL)
 * @param n Number of components (must be > 0)
 * @return FC_OK on success, error code otherwise
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 */
fc_status_t fc_index_weights_normalize(double* weights, int32_t n);

/**
 * @brief Calculate rebalancing trades for index adjustment
 *
 * Computes the trades needed to transition from old weights to new weights
 * given the current index value.
 *
 * @param old_weights Current component weights [n] (must not be NULL)
 * @param new_weights Target component weights [n] (must not be NULL)
 * @param prices Current component prices [n] (must not be NULL)
 * @param n Number of components (must be > 0)
 * @param index_value Current index value (must be > 0)
 * @param trade_amounts Output array of trade amounts [n] (must not be NULL)
 *                      Positive = buy, negative = sell
 * @return FC_OK on success, error code otherwise
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 */
fc_status_t fc_index_rebalance_trades(
    const double* old_weights,
    const double* new_weights,
    const double* prices,
    int32_t n,
    double index_value,
    double* trade_amounts
);

/**
 * @brief Calculate rebalancing cost estimate
 *
 * Estimates the total cost of rebalancing based on trade amounts and slippage.
 *
 * @param trade_amounts Array of trade amounts [n] (must not be NULL)
 * @param prices Current component prices [n] (must not be NULL)
 * @param slippage_rates Array of slippage rates per component [n] (must not be NULL, all >= 0)
 * @param n Number of components (must be > 0)
 * @param total_cost Output total rebalancing cost (must not be NULL)
 * @return FC_OK on success, error code otherwise
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 */
fc_status_t fc_index_rebalance_cost(
    const double* trade_amounts,
    const double* prices,
    const double* slippage_rates,
    int32_t n,
    double* total_cost
);

/**
 * @brief Calculate new divisor after component changes
 *
 * When components are added/removed or undergo corporate actions,
 * the divisor must be adjusted to maintain index continuity.
 *
 * Formula: new_divisor = old_divisor × (Σ new_prices × new_weights) / (Σ old_prices × old_weights)
 *
 * @param old_prices Old component prices [old_n] (must not be NULL)
 * @param old_weights Old component weights [old_n] (must not be NULL)
 * @param old_n Number of old components (must be > 0)
 * @param new_prices New component prices [new_n] (must not be NULL)
 * @param new_weights New component weights [new_n] (must not be NULL)
 * @param new_n Number of new components (must be > 0)
 * @param old_divisor Current divisor (must be > 0)
 * @param new_divisor Output new divisor (must not be NULL)
 * @return FC_OK on success, error code otherwise
 *
 * Time complexity: O(old_n + new_n)
 * Space complexity: O(1)
 */
fc_status_t fc_index_adjust_divisor(
    const double* old_prices,
    const double* old_weights,
    int32_t old_n,
    const double* new_prices,
    const double* new_weights,
    int32_t new_n,
    double old_divisor,
    double* new_divisor
);

#ifdef __cplusplus
}
#endif

#endif /* FC_INDEX_H */
