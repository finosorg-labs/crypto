/**
 * @file test_index.c
 * @brief Unit tests for crypto index construction and calculation
 */

#include "test_framework.h"
#include "index.h"
#include <math.h>

#define TEST_TOLERANCE 1e-10

/* Test: Basic index calculation */
TEST(index_calculate_basic) {
    double prices[] = {50000.0, 3000.0, 100.0};
    double weights[] = {0.5, 0.3, 0.2};
    double index_value;

    ASSERT_EQ(fc_index_calculate(prices, weights, 3, 1.0, &index_value), FC_OK);

    double expected = (50000.0 * 0.5 + 3000.0 * 0.3 + 100.0 * 0.2);
    FC_TEST_ASSERT_DOUBLE_EQ(index_value, expected, TEST_TOLERANCE);
}

/* Test: Index calculation with divisor */
TEST(index_calculate_with_divisor) {
    double prices[] = {50000.0, 3000.0, 100.0};
    double weights[] = {0.5, 0.3, 0.2};
    double index_value;

    ASSERT_EQ(fc_index_calculate(prices, weights, 3, 100.0, &index_value), FC_OK);

    double expected = (50000.0 * 0.5 + 3000.0 * 0.3 + 100.0 * 0.2) / 100.0;
    FC_TEST_ASSERT_DOUBLE_EQ(index_value, expected, TEST_TOLERANCE);
}

/* Test: Single component index */
TEST(index_calculate_single) {
    double prices[] = {50000.0};
    double weights[] = {1.0};
    double index_value;

    ASSERT_EQ(fc_index_calculate(prices, weights, 1, 1.0, &index_value), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(index_value, 50000.0, TEST_TOLERANCE);
}

/* Test: Large number of components */
TEST(index_calculate_many_components) {
    double prices[100];
    double weights[100];

    for (int i = 0; i < 100; i++) {
        prices[i] = 100.0 + i;
        weights[i] = 1.0 / 100.0;
    }

    double index_value;
    ASSERT_EQ(fc_index_calculate(prices, weights, 100, 1.0, &index_value), FC_OK);

    double expected = 0.0;
    for (int i = 0; i < 100; i++) {
        expected += prices[i] * weights[i];
    }
    FC_TEST_ASSERT_DOUBLE_EQ(index_value, expected, TEST_TOLERANCE);
}

/* Test: NULL pointer checks */
TEST(index_calculate_null) {
    double prices[] = {50000.0};
    double weights[] = {1.0};
    double index_value;

    ASSERT_EQ(fc_index_calculate(NULL, weights, 1, 1.0, &index_value), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_calculate(prices, NULL, 1, 1.0, &index_value), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_calculate(prices, weights, 1, 1.0, NULL), FC_ERR_INVALID_ARG);
}

/* Test: Invalid arguments */
TEST(index_calculate_invalid) {
    double prices[] = {50000.0};
    double weights[] = {1.0};
    double index_value;

    ASSERT_EQ(fc_index_calculate(prices, weights, 0, 1.0, &index_value), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_calculate(prices, weights, -1, 1.0, &index_value), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_calculate(prices, weights, 1, 0.0, &index_value), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_calculate(prices, weights, 1, -1.0, &index_value), FC_ERR_INVALID_ARG);
}

/* Test: Batch calculation */
TEST(index_calculate_batch) {
    double prices_matrix[] = {
        50000.0, 3000.0, 100.0,
        51000.0, 3100.0, 105.0,
        49000.0, 2900.0, 95.0
    };
    double weights[] = {0.5, 0.3, 0.2};
    double index_values[3];

    ASSERT_EQ(fc_index_calculate_batch(prices_matrix, weights, 3, 3, 1.0, index_values), FC_OK);

    for (int t = 0; t < 3; t++) {
        double expected = 0.0;
        for (int i = 0; i < 3; i++) {
            expected += prices_matrix[t * 3 + i] * weights[i];
        }
        FC_TEST_ASSERT_DOUBLE_EQ(index_values[t], expected, TEST_TOLERANCE);
    }
}

/* Test: Batch calculation NULL checks */
TEST(index_calculate_batch_null) {
    double prices_matrix[] = {50000.0, 3000.0, 100.0};
    double weights[] = {0.5, 0.3, 0.2};
    double index_values[1];

    ASSERT_EQ(fc_index_calculate_batch(NULL, weights, 3, 1, 1.0, index_values), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_calculate_batch(prices_matrix, NULL, 3, 1, 1.0, index_values), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_calculate_batch(prices_matrix, weights, 3, 1, 1.0, NULL), FC_ERR_INVALID_ARG);
}

/* Test: Equal weights */
TEST(index_weights_equal) {
    double weights[5];

    ASSERT_EQ(fc_index_weights_equal(5, weights), FC_OK);

    for (int i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(weights[i], 0.2, TEST_TOLERANCE);
    }

    double sum = 0.0;
    for (int i = 0; i < 5; i++) {
        sum += weights[i];
    }
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 1.0, TEST_TOLERANCE);
}

/* Test: Market cap weights */
TEST(index_weights_market_cap) {
    double market_caps[] = {1000.0, 500.0, 250.0, 250.0};
    double weights[4];

    ASSERT_EQ(fc_index_weights_market_cap(market_caps, 4, weights), FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(weights[0], 0.5, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(weights[1], 0.25, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(weights[2], 0.125, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(weights[3], 0.125, TEST_TOLERANCE);

    double sum = 0.0;
    for (int i = 0; i < 4; i++) {
        sum += weights[i];
    }
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 1.0, TEST_TOLERANCE);
}

/* Test: Market cap weights with invalid values */
TEST(index_weights_market_cap_invalid) {
    double market_caps[] = {1000.0, -500.0, 250.0};
    double weights[3];

    ASSERT_EQ(fc_index_weights_market_cap(market_caps, 3, weights), FC_ERR_INVALID_ARG);

    double zero_caps[] = {0.0, 0.0, 0.0};
    ASSERT_EQ(fc_index_weights_market_cap(zero_caps, 3, weights), FC_ERR_INVALID_ARG);
}

/* Test: Liquidity weights */
TEST(index_weights_liquidity) {
    double liquidity[] = {1000.0, 2000.0, 3000.0, 4000.0};
    double weights[4];

    ASSERT_EQ(fc_index_weights_liquidity(liquidity, 4, weights), FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(weights[0], 0.1, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(weights[1], 0.2, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(weights[2], 0.3, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(weights[3], 0.4, TEST_TOLERANCE);

    double sum = 0.0;
    for (int i = 0; i < 4; i++) {
        sum += weights[i];
    }
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 1.0, TEST_TOLERANCE);
}

/* Test: Weight normalization */
TEST(index_weights_normalize) {
    double weights[] = {10.0, 20.0, 30.0, 40.0};

    ASSERT_EQ(fc_index_weights_normalize(weights, 4), FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(weights[0], 0.1, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(weights[1], 0.2, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(weights[2], 0.3, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(weights[3], 0.4, TEST_TOLERANCE);

    double sum = 0.0;
    for (int i = 0; i < 4; i++) {
        sum += weights[i];
    }
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 1.0, TEST_TOLERANCE);
}

/* Test: Rebalancing trades */
TEST(index_rebalance_trades) {
    double old_weights[] = {0.5, 0.3, 0.2};
    double new_weights[] = {0.4, 0.4, 0.2};
    double prices[] = {50000.0, 3000.0, 100.0};
    double trade_amounts[3];
    double index_value = 26920.0;

    ASSERT_EQ(fc_index_rebalance_trades(old_weights, new_weights, prices, 3, index_value, trade_amounts), FC_OK);

    double expected_0 = (0.4 - 0.5) * index_value / 50000.0;
    double expected_1 = (0.4 - 0.3) * index_value / 3000.0;
    double expected_2 = (0.2 - 0.2) * index_value / 100.0;

    FC_TEST_ASSERT_DOUBLE_EQ(trade_amounts[0], expected_0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(trade_amounts[1], expected_1, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(trade_amounts[2], expected_2, TEST_TOLERANCE);
}

/* Test: Rebalancing cost */
TEST(index_rebalance_cost) {
    double trade_amounts[] = {-0.1, 0.15, 0.0};
    double prices[] = {50000.0, 3000.0, 100.0};
    double slippage_rates[] = {0.001, 0.002, 0.001};
    double total_cost;

    ASSERT_EQ(fc_index_rebalance_cost(trade_amounts, prices, slippage_rates, 3, &total_cost), FC_OK);

    double expected = fabs(-0.1) * 50000.0 * 0.001 +
                     fabs(0.15) * 3000.0 * 0.002 +
                     fabs(0.0) * 100.0 * 0.001;

    FC_TEST_ASSERT_DOUBLE_EQ(total_cost, expected, TEST_TOLERANCE);
}

/* Test: Divisor adjustment */
TEST(index_adjust_divisor) {
    double old_prices[] = {50000.0, 3000.0, 100.0};
    double old_weights[] = {0.5, 0.3, 0.2};
    double new_prices[] = {51000.0, 3100.0};
    double new_weights[] = {0.6, 0.4};
    double new_divisor;

    ASSERT_EQ(fc_index_adjust_divisor(old_prices, old_weights, 3,
                                       new_prices, new_weights, 2,
                                       1.0, &new_divisor), FC_OK);

    double old_sum = 50000.0 * 0.5 + 3000.0 * 0.3 + 100.0 * 0.2;
    double new_sum = 51000.0 * 0.6 + 3100.0 * 0.4;
    double expected = 1.0 * new_sum / old_sum;

    FC_TEST_ASSERT_DOUBLE_EQ(new_divisor, expected, TEST_TOLERANCE);
}

/* Test: Divisor adjustment NULL checks */
TEST(index_adjust_divisor_null) {
    double old_prices[] = {50000.0};
    double old_weights[] = {1.0};
    double new_prices[] = {51000.0};
    double new_weights[] = {1.0};
    double new_divisor;

    ASSERT_EQ(fc_index_adjust_divisor(NULL, old_weights, 1, new_prices, new_weights, 1, 1.0, &new_divisor),
              FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_adjust_divisor(old_prices, NULL, 1, new_prices, new_weights, 1, 1.0, &new_divisor),
              FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_adjust_divisor(old_prices, old_weights, 1, NULL, new_weights, 1, 1.0, &new_divisor),
              FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_adjust_divisor(old_prices, old_weights, 1, new_prices, NULL, 1, 1.0, &new_divisor),
              FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_index_adjust_divisor(old_prices, old_weights, 1, new_prices, new_weights, 1, 1.0, NULL),
              FC_ERR_INVALID_ARG);
}

void register_index_tests(void) {
    RUN_TEST(index_calculate_basic);
    RUN_TEST(index_calculate_with_divisor);
    RUN_TEST(index_calculate_single);
    RUN_TEST(index_calculate_many_components);
    RUN_TEST(index_calculate_null);
    RUN_TEST(index_calculate_invalid);
    RUN_TEST(index_calculate_batch);
    RUN_TEST(index_calculate_batch_null);
    RUN_TEST(index_weights_equal);
    RUN_TEST(index_weights_market_cap);
    RUN_TEST(index_weights_market_cap_invalid);
    RUN_TEST(index_weights_liquidity);
    RUN_TEST(index_weights_normalize);
    RUN_TEST(index_rebalance_trades);
    RUN_TEST(index_rebalance_cost);
    RUN_TEST(index_adjust_divisor);
    RUN_TEST(index_adjust_divisor_null);
}
