/**
 * @file test_crypto.c
 * @brief crypto module test entry point
 *
 * This file serves as the main test registration point for the crypto module.
 * Individual test modules are in separate files:
 */

#include "test_framework.h"

/* External test registration functions from sub-modules */
extern void register_xxx_tests(void);

/* Entry point for crypto tests */
void register_crypto_tests(void) {
    /* Register all sub-module tests */
    register_xxx_tests();
}
