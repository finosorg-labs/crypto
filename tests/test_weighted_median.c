#include "test_framework.h"
#include "weighted_median.h"
#include <math.h>

TEST(weighted_median_basic) {
    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double weights[] = {1.0, 1.0, 1.0, 1.0, 1.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 5, &result);
    ASSERT_EQ(ret, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 3.0, 1e-10);
}

TEST(weighted_median_skewed) {
    double values[] = {100.0, 200.0, 300.0};
    double weights[] = {10.0, 1.0, 1.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 3, &result);
    ASSERT_EQ(ret, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 100.0, 1e-10);
}

TEST(weighted_median_two_elements) {
    double values[] = {10.0, 20.0};
    double weights[] = {3.0, 1.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 2, &result);
    ASSERT_EQ(ret, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 10.0, 1e-10);
}

TEST(weighted_median_single_element) {
    double values[] = {42.0};
    double weights[] = {1.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 1, &result);
    ASSERT_EQ(ret, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 42.0, 1e-10);
}

TEST(weighted_median_unsorted) {
    double values[] = {5.0, 1.0, 3.0, 4.0, 2.0};
    double weights[] = {1.0, 1.0, 1.0, 1.0, 1.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 5, &result);
    ASSERT_EQ(ret, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 3.0, 1e-10);
}

TEST(weighted_median_zero_weights) {
    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double weights[] = {0.0, 0.0, 1.0, 0.0, 0.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 5, &result);
    ASSERT_EQ(ret, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 3.0, 1e-10);
}

TEST(weighted_median_error_null_values) {
    double weights[] = {1.0, 1.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(NULL, weights, 2, &result);
    ASSERT_EQ(ret, FC_ERR_INVALID_ARG);
}

TEST(weighted_median_error_null_weights) {
    double values[] = {1.0, 2.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, NULL, 2, &result);
    ASSERT_EQ(ret, FC_ERR_INVALID_ARG);
}

TEST(weighted_median_error_null_result) {
    double values[] = {1.0, 2.0};
    double weights[] = {1.0, 1.0};

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 2, NULL);
    ASSERT_EQ(ret, FC_ERR_INVALID_ARG);
}

TEST(weighted_median_error_zero_size) {
    double values[] = {1.0};
    double weights[] = {1.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 0, &result);
    ASSERT_EQ(ret, FC_ERR_INVALID_ARG);
}

TEST(weighted_median_error_all_weights_zero) {
    double values[] = {1.0, 2.0, 3.0};
    double weights[] = {0.0, 0.0, 0.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 3, &result);
    ASSERT_EQ(ret, FC_ERR_DIVIDE_BY_ZERO);
}

TEST(weighted_median_error_negative_weight) {
    double values[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, -1.0, 1.0};
    double result;

    fc_status_t ret = fc_crypto_weighted_median_f64(values, weights, 3, &result);
    ASSERT_EQ(ret, FC_ERR_NAN_INPUT);
}

TEST(weighted_median_batch_basic) {
    double values[] = {1.0, 2.0, 3.0, 10.0, 20.0};
    double weights[] = {1.0, 1.0, 1.0, 3.0, 1.0};
    size_t offsets[] = {0, 3};
    size_t sizes[] = {3, 2};
    double results[2];

    fc_status_t ret = fc_crypto_weighted_median_batch_f64(values, weights, offsets, sizes, 2, results);
    ASSERT_EQ(ret, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(results[0], 2.0, 1e-10);
    FC_TEST_ASSERT_DOUBLE_EQ(results[1], 10.0, 1e-10);
}

TEST(weighted_median_batch_error_null) {
    double weights[] = {1.0, 1.0};
    size_t offsets[] = {0};
    size_t sizes[] = {2};
    double results[1];

    fc_status_t ret = fc_crypto_weighted_median_batch_f64(NULL, weights, offsets, sizes, 1, results);
    ASSERT_EQ(ret, FC_ERR_INVALID_ARG);
}

TEST(weighted_median_batch_error_zero_batch) {
    double values[] = {1.0};
    double weights[] = {1.0};
    size_t offsets[] = {0};
    size_t sizes[] = {1};
    double results[1];

    fc_status_t ret = fc_crypto_weighted_median_batch_f64(values, weights, offsets, sizes, 0, results);
    ASSERT_EQ(ret, FC_ERR_INVALID_ARG);
}

TEST(weighted_median_crypto_scenario) {
    double exchange_prices[] = {50000.0, 50050.0, 49900.0, 50020.0, 51000.0};
    double volumes[] = {100.0, 150.0, 120.0, 130.0, 10.0};
    double result;

    int ret = fc_crypto_weighted_median_f64(exchange_prices, volumes, 5, &result);
    ASSERT_EQ(ret, 0);
    ASSERT_TRUE(result >= 49000.0 && result <= 52000.0);
}

void register_weighted_median_tests(void) {
    RUN_TEST(weighted_median_basic);
    RUN_TEST(weighted_median_skewed);
    RUN_TEST(weighted_median_two_elements);
    RUN_TEST(weighted_median_single_element);
    RUN_TEST(weighted_median_unsorted);
    RUN_TEST(weighted_median_zero_weights);

    RUN_TEST(weighted_median_error_null_values);
    RUN_TEST(weighted_median_error_null_weights);
    RUN_TEST(weighted_median_error_null_result);
    RUN_TEST(weighted_median_error_zero_size);
    RUN_TEST(weighted_median_error_all_weights_zero);
    RUN_TEST(weighted_median_error_negative_weight);

    RUN_TEST(weighted_median_batch_basic);
    RUN_TEST(weighted_median_batch_error_null);
    RUN_TEST(weighted_median_batch_error_zero_batch);

    RUN_TEST(weighted_median_crypto_scenario);
}
