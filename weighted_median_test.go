package crypto

import (
	"math"
	"testing"
)

func TestWeightedMedianBasic(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	weights := []float64{1.0, 1.0, 1.0, 1.0, 1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 3.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianSkewed(t *testing.T) {
	values := []float64{100.0, 200.0, 300.0}
	weights := []float64{10.0, 1.0, 1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 100.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianTwoElements(t *testing.T) {
	values := []float64{10.0, 20.0}
	weights := []float64{3.0, 1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 10.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianSingleElement(t *testing.T) {
	values := []float64{42.0}
	weights := []float64{1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 42.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianUnsorted(t *testing.T) {
	values := []float64{5.0, 1.0, 3.0, 4.0, 2.0}
	weights := []float64{1.0, 1.0, 1.0, 1.0, 1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 3.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianZeroWeights(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	weights := []float64{0.0, 0.0, 1.0, 0.0, 0.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 3.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianErrorZeroSize(t *testing.T) {
	values := []float64{}
	weights := []float64{}

	_, err := WeightedMedian(values, weights)
	if err == nil {
		t.Errorf("Expected error for zero size input, got nil")
	}
}

func TestWeightedMedianErrorMismatchedSizes(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0}

	_, err := WeightedMedian(values, weights)
	if err == nil {
		t.Errorf("Expected error for mismatched sizes, got nil")
	}
}

func TestWeightedMedianErrorAllWeightsZero(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0}
	weights := []float64{0.0, 0.0, 0.0}

	_, err := WeightedMedian(values, weights)
	if err == nil {
		t.Errorf("Expected error for all weights zero, got nil")
	}
}

func TestWeightedMedianErrorNegativeWeight(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, -1.0, 1.0}

	_, err := WeightedMedian(values, weights)
	if err == nil {
		t.Errorf("Expected error for negative weight, got nil")
	}
}

func TestWeightedMedianCryptoScenario(t *testing.T) {
	exchangePrices := []float64{50000.0, 50050.0, 49900.0, 50020.0, 51000.0}
	volumes := []float64{100.0, 150.0, 120.0, 130.0, 10.0}

	result, err := WeightedMedian(exchangePrices, volumes)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	t.Logf("Aggregated price (volume-weighted median): %.2f", result)

	if result < 49000.0 || result > 52000.0 {
		t.Errorf("Result out of expected range: %f", result)
	}
}

func TestWeightedMedianBatchBasic(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0, 10.0, 20.0}
	weights := []float64{1.0, 1.0, 1.0, 3.0, 1.0}
	offsets := []int{0, 3}
	sizes := []int{3, 2}

	results, err := WeightedMedianBatch(values, weights, offsets, sizes)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	if len(results) != 2 {
		t.Fatalf("Expected 2 results, got %d", len(results))
	}

	if math.Abs(results[0]-2.0) > 1e-10 {
		t.Errorf("First batch: expected 2.0, got %f", results[0])
	}
	if math.Abs(results[1]-10.0) > 1e-10 {
		t.Errorf("Second batch: expected 10.0, got %f", results[1])
	}
}

func TestWeightedMedianBatchErrorZeroSize(t *testing.T) {
	values := []float64{1.0, 2.0}
	weights := []float64{1.0, 1.0}
	offsets := []int{}
	sizes := []int{}

	_, err := WeightedMedianBatch(values, weights, offsets, sizes)
	if err == nil {
		t.Errorf("Expected error for zero batch size, got nil")
	}
}

func TestWeightedMedianBatchErrorMismatchedSizes(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 1.0}
	offsets := []int{0, 2}
	sizes := []int{2}

	_, err := WeightedMedianBatch(values, weights, offsets, sizes)
	if err == nil {
		t.Errorf("Expected error for mismatched batch sizes, got nil")
	}
}

func BenchmarkWeightedMedianSmall(b *testing.B) {
	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	weights := []float64{1.0, 1.0, 1.0, 1.0, 1.0}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedian(values, weights)
	}
}

func BenchmarkWeightedMedianMedium(b *testing.B) {
	values := make([]float64, 50)
	weights := make([]float64, 50)
	for i := 0; i < 50; i++ {
		values[i] = float64(i + 1)
		weights[i] = 1.0
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedian(values, weights)
	}
}

func BenchmarkWeightedMedianLarge(b *testing.B) {
	values := make([]float64, 1000)
	weights := make([]float64, 1000)
	for i := 0; i < 1000; i++ {
		values[i] = float64(i + 1)
		weights[i] = 1.0
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedian(values, weights)
	}
}

func BenchmarkWeightedMedianBatch1000Contracts(b *testing.B) {
	numContracts := 1000
	exchangesPerContract := 5
	totalSize := numContracts * exchangesPerContract

	values := make([]float64, totalSize)
	weights := make([]float64, totalSize)
	offsets := make([]int, numContracts)
	sizes := make([]int, numContracts)

	for i := 0; i < totalSize; i++ {
		values[i] = 50000.0 + float64(i%10000)
		weights[i] = 1.0 + float64(i%100)
	}
	for i := 0; i < numContracts; i++ {
		offsets[i] = i * exchangesPerContract
		sizes[i] = exchangesPerContract
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedianBatch(values, weights, offsets, sizes)
	}
}

func BenchmarkWeightedMedianBatch10000Contracts(b *testing.B) {
	numContracts := 10000
	exchangesPerContract := 5
	totalSize := numContracts * exchangesPerContract

	values := make([]float64, totalSize)
	weights := make([]float64, totalSize)
	offsets := make([]int, numContracts)
	sizes := make([]int, numContracts)

	for i := 0; i < totalSize; i++ {
		values[i] = 50000.0 + float64(i%10000)
		weights[i] = 1.0 + float64(i%100)
	}
	for i := 0; i < numContracts; i++ {
		offsets[i] = i * exchangesPerContract
		sizes[i] = exchangesPerContract
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedianBatch(values, weights, offsets, sizes)
	}
}
