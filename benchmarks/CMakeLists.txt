#
# benchmarks/CMakeLists.txt - Benchmark configuration for crypto module
#

# Benchmark framework library
add_library(bench_framework OBJECT
    bench_framework.c
)

target_include_directories(bench_framework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/modules/platform/include
        ${CMAKE_SOURCE_DIR}/modules/ds/include
        ${CMAKE_SOURCE_DIR}/modules/sort/include
)

target_link_libraries(bench_framework
    PUBLIC
        finkit_crypto_static_base
        finkit::platform
      finkit::ds
)

# Benchmark executable
add_executable(crypto_benchmarks
    bench_main.c
    bench_crypto.c
    bench_xxx.c
)

# Link with benchmark framework and crypto library
target_link_libraries(crypto_benchmarks
    PRIVATE
        bench_framework
        finkit_crypto_static_base
        finkit::sort
        finkit::platform
        finkit::ds
        m
)

target_compile_definitions(crypto_benchmarks PRIVATE
    FC_STATIC_BUILD
)

# Include directories
target_include_directories(crypto_benchmarks
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/modules/platform/include
        ${CMAKE_SOURCE_DIR}/modules/ds/include
        ${CMAKE_SOURCE_DIR}/modules/sort/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler flags for benchmarks
target_compile_options(crypto_benchmarks PRIVATE -Wall -Wextra -Wpedantic -ffp-contract=off)

# Installation
install(TARGETS crypto_benchmarks
    RUNTIME DESTINATION bin
)
