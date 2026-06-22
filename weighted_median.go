package crypto

/*
#include "weighted_median.h"
#include "error.h"
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// WeightedMedian computes the weighted median for a single dataset.
//
// The weighted median is the value at which cumulative weight reaches 50% of total weight.
// This is useful for anti-manipulation price aggregation from multiple exchanges.
//
// Example:
//
//	prices := []float64{50000, 50050, 49900, 50020, 51000}
//	volumes := []float64{100, 150, 120, 130, 10}
//	median, err := WeightedMedian(prices, volumes)
func WeightedMedian(values, weights []float64) (float64, error) {
	if len(values) == 0 {
		return 0, fmt.Errorf("%s", C.GoString(C.fc_status_string(C.FC_ERR_INVALID_ARG)))
	}
	if len(values) != len(weights) {
		return 0, fmt.Errorf("%s", C.GoString(C.fc_status_string(C.FC_ERR_INVALID_ARG)))
	}

	var result C.double
	ret := C.fc_crypto_weighted_median_f64(
		(*C.double)(unsafe.Pointer(&values[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(len(values)),
		&result,
	)

	if ret != C.FC_OK {
		return 0, fmt.Errorf("%s", C.GoString(C.fc_status_string(ret)))
	}

	return float64(result), nil
}

// WeightedMedianBatch computes weighted medians for multiple datasets in batch.
//
// Each dataset can have different size. The function processes all datasets efficiently.
//
// Example:
//
//	// Dataset 0: 3 exchanges
//	// Dataset 1: 5 exchanges
//	values := []float64{100, 200, 300, 10, 20, 30, 40, 50}
//	weights := []float64{1, 1, 1, 2, 2, 2, 2, 2}
//	offsets := []int{0, 3}
//	sizes := []int{3, 5}
//	results, err := WeightedMedianBatch(values, weights, offsets, sizes)
func WeightedMedianBatch(values, weights []float64, offsets, sizes []int) ([]float64, error) {
	if len(offsets) == 0 || len(sizes) == 0 {
		return nil, fmt.Errorf("%s", C.GoString(C.fc_status_string(C.FC_ERR_INVALID_ARG)))
	}
	if len(offsets) != len(sizes) {
		return nil, fmt.Errorf("%s", C.GoString(C.fc_status_string(C.FC_ERR_INVALID_ARG)))
	}

	batchSize := len(offsets)
	results := make([]float64, batchSize)

	cOffsets := make([]C.size_t, batchSize)
	cSizes := make([]C.size_t, batchSize)
	for i := 0; i < batchSize; i++ {
		cOffsets[i] = C.size_t(offsets[i])
		cSizes[i] = C.size_t(sizes[i])
	}

	ret := C.fc_crypto_weighted_median_batch_f64(
		(*C.double)(unsafe.Pointer(&values[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		(*C.size_t)(unsafe.Pointer(&cOffsets[0])),
		(*C.size_t)(unsafe.Pointer(&cSizes[0])),
		C.size_t(batchSize),
		(*C.double)(unsafe.Pointer(&results[0])),
	)

	if ret != C.FC_OK {
		return nil, fmt.Errorf("%s", C.GoString(C.fc_status_string(ret)))
	}

	return results, nil
}
