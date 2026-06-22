#
# tests/CMakeLists.txt - Test configuration for crypto module
#

# Test executable
add_executable(crypto_tests
    test_framework.c
    test_main.c
    test_weighted_median.c
)

# Link with crypto library
target_link_libraries(crypto_tests
    PRIVATE
        finkit_crypto_static_base
     finkit::ds
        finkit::platform
        m
)

target_compile_definitions(crypto_tests PRIVATE
    FC_STATIC_BUILD
)

# Include directories
target_include_directories(crypto_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/modules/platform/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler flags for tests
target_compile_options(crypto_tests PRIVATE -Wall -Wextra -Wpedantic)

# Run tests with ctest
add_test(
    NAME "crypto_tests"
    COMMAND crypto_tests
)

# Installation
install(TARGETS crypto_tests
    RUNTIME DESTINATION bin
)
