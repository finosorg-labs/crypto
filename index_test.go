package crypto

import (
	"math"
	"testing"
)

const testTolerance = 1e-10

func almostEqual(a, b, tolerance float64) bool {
	return math.Abs(a-b) < tolerance
}

func TestCalculate(t *testing.T) {
	prices := []float64{50000.0, 3000.0, 100.0}
	weights := []float64{0.5, 0.3, 0.2}

	indexValue, err := Calculate(prices, weights, 1.0)
	if err != nil {
		t.Fatalf("Calculate failed: %v", err)
	}

	expected := 50000.0*0.5 + 3000.0*0.3 + 100.0*0.2
	if !almostEqual(indexValue, expected, testTolerance) {
		t.Errorf("Expected %f, got %f", expected, indexValue)
	}
}

func TestCalculateWithDivisor(t *testing.T) {
	prices := []float64{50000.0, 3000.0, 100.0}
	weights := []float64{0.5, 0.3, 0.2}

	indexValue, err := Calculate(prices, weights, 100.0)
	if err != nil {
		t.Fatalf("Calculate failed: %v", err)
	}

	expected := (50000.0*0.5 + 3000.0*0.3 + 100.0*0.2) / 100.0
	if !almostEqual(indexValue, expected, testTolerance) {
		t.Errorf("Expected %f, got %f", expected, indexValue)
	}
}

func TestCalculateEmpty(t *testing.T) {
	_, err := Calculate([]float64{}, []float64{}, 1.0)
	if err == nil {
		t.Error("Expected error for empty slices")
	}
}

func TestCalculateMismatchedLengths(t *testing.T) {
	prices := []float64{50000.0, 3000.0}
	weights := []float64{0.5, 0.3, 0.2}

	_, err := Calculate(prices, weights, 1.0)
	if err == nil {
		t.Error("Expected error for mismatched lengths")
	}
}

func TestCalculateBatch(t *testing.T) {
	pricesMatrix := [][]float64{
		{50000.0, 3000.0, 100.0},
		{51000.0, 3100.0, 105.0},
		{49000.0, 2900.0, 95.0},
	}
	weights := []float64{0.5, 0.3, 0.2}

	indexValues, err := CalculateBatch(pricesMatrix, weights, 1.0)
	if err != nil {
		t.Fatalf("CalculateBatch failed: %v", err)
	}

	if len(indexValues) != 3 {
		t.Fatalf("Expected 3 index values, got %d", len(indexValues))
	}

	for i, prices := range pricesMatrix {
		expected := 0.0
		for j, price := range prices {
			expected += price * weights[j]
		}
		if !almostEqual(indexValues[i], expected, testTolerance) {
			t.Errorf("Timestamp %d: expected %f, got %f", i, expected, indexValues[i])
		}
	}
}

func TestWeightsEqual(t *testing.T) {
	weights, err := WeightsEqual(5)
	if err != nil {
		t.Fatalf("WeightsEqual failed: %v", err)
	}

	if len(weights) != 5 {
		t.Fatalf("Expected 5 weights, got %d", len(weights))
	}

	for i, w := range weights {
		if !almostEqual(w, 0.2, testTolerance) {
			t.Errorf("Weight %d: expected 0.2, got %f", i, w)
		}
	}

	sum := 0.0
	for _, w := range weights {
		sum += w
	}
	if !almostEqual(sum, 1.0, testTolerance) {
		t.Errorf("Weights sum: expected 1.0, got %f", sum)
	}
}

func TestWeightsMarketCap(t *testing.T) {
	marketCaps := []float64{1000.0, 500.0, 250.0, 250.0}

	weights, err := WeightsMarketCap(marketCaps)
	if err != nil {
		t.Fatalf("WeightsMarketCap failed: %v", err)
	}

	if len(weights) != 4 {
		t.Fatalf("Expected 4 weights, got %d", len(weights))
	}

	expected := []float64{0.5, 0.25, 0.125, 0.125}
	for i, w := range weights {
		if !almostEqual(w, expected[i], testTolerance) {
			t.Errorf("Weight %d: expected %f, got %f", i, expected[i], w)
		}
	}

	sum := 0.0
	for _, w := range weights {
		sum += w
	}
	if !almostEqual(sum, 1.0, testTolerance) {
		t.Errorf("Weights sum: expected 1.0, got %f", sum)
	}
}

func TestWeightsLiquidity(t *testing.T) {
	liquidity := []float64{1000.0, 2000.0, 3000.0, 4000.0}

	weights, err := WeightsLiquidity(liquidity)
	if err != nil {
		t.Fatalf("WeightsLiquidity failed: %v", err)
	}

	if len(weights) != 4 {
		t.Fatalf("Expected 4 weights, got %d", len(weights))
	}

	expected := []float64{0.1, 0.2, 0.3, 0.4}
	for i, w := range weights {
		if !almostEqual(w, expected[i], testTolerance) {
			t.Errorf("Weight %d: expected %f, got %f", i, expected[i], w)
		}
	}

	sum := 0.0
	for _, w := range weights {
		sum += w
	}
	if !almostEqual(sum, 1.0, testTolerance) {
		t.Errorf("Weights sum: expected 1.0, got %f", sum)
	}
}

func TestWeightsNormalize(t *testing.T) {
	weights := []float64{10.0, 20.0, 30.0, 40.0}

	err := WeightsNormalize(weights)
	if err != nil {
		t.Fatalf("WeightsNormalize failed: %v", err)
	}

	expected := []float64{0.1, 0.2, 0.3, 0.4}
	for i, w := range weights {
		if !almostEqual(w, expected[i], testTolerance) {
			t.Errorf("Weight %d: expected %f, got %f", i, expected[i], w)
		}
	}

	sum := 0.0
	for _, w := range weights {
		sum += w
	}
	if !almostEqual(sum, 1.0, testTolerance) {
		t.Errorf("Weights sum: expected 1.0, got %f", sum)
	}
}

func TestRebalanceTrades(t *testing.T) {
	oldWeights := []float64{0.5, 0.3, 0.2}
	newWeights := []float64{0.4, 0.4, 0.2}
	prices := []float64{50000.0, 3000.0, 100.0}
	indexValue := 26920.0

	tradeAmounts, err := RebalanceTrades(oldWeights, newWeights, prices, indexValue)
	if err != nil {
		t.Fatalf("RebalanceTrades failed: %v", err)
	}

	if len(tradeAmounts) != 3 {
		t.Fatalf("Expected 3 trade amounts, got %d", len(tradeAmounts))
	}

	expected := []float64{
		(0.4 - 0.5) * indexValue / 50000.0,
		(0.4 - 0.3) * indexValue / 3000.0,
		(0.2 - 0.2) * indexValue / 100.0,
	}

	for i, amount := range tradeAmounts {
		if !almostEqual(amount, expected[i], testTolerance) {
			t.Errorf("Trade amount %d: expected %f, got %f", i, expected[i], amount)
		}
	}
}

func TestRebalanceCost(t *testing.T) {
	tradeAmounts := []float64{-0.1, 0.15, 0.0}
	prices := []float64{50000.0, 3000.0, 100.0}
	slippageRates := []float64{0.001, 0.002, 0.001}

	totalCost, err := RebalanceCost(tradeAmounts, prices, slippageRates)
	if err != nil {
		t.Fatalf("RebalanceCost failed: %v", err)
	}

	expected := math.Abs(-0.1)*50000.0*0.001 +
		math.Abs(0.15)*3000.0*0.002 +
		math.Abs(0.0)*100.0*0.001

	if !almostEqual(totalCost, expected, testTolerance) {
		t.Errorf("Expected cost %f, got %f", expected, totalCost)
	}
}

func TestAdjustDivisor(t *testing.T) {
	oldPrices := []float64{50000.0, 3000.0, 100.0}
	oldWeights := []float64{0.5, 0.3, 0.2}
	newPrices := []float64{51000.0, 3100.0}
	newWeights := []float64{0.6, 0.4}

	newDivisor, err := AdjustDivisor(oldPrices, oldWeights, newPrices, newWeights, 1.0)
	if err != nil {
		t.Fatalf("AdjustDivisor failed: %v", err)
	}

	oldSum := 50000.0*0.5 + 3000.0*0.3 + 100.0*0.2
	newSum := 51000.0*0.6 + 3100.0*0.4
	expected := 1.0 * newSum / oldSum

	if !almostEqual(newDivisor, expected, testTolerance) {
		t.Errorf("Expected divisor %f, got %f", expected, newDivisor)
	}
}

func TestAdjustDivisorMismatchedLengths(t *testing.T) {
	oldPrices := []float64{50000.0, 3000.0}
	oldWeights := []float64{0.5, 0.3, 0.2}
	newPrices := []float64{51000.0, 3100.0}
	newWeights := []float64{0.6, 0.4}

	_, err := AdjustDivisor(oldPrices, oldWeights, newPrices, newWeights, 1.0)
	if err == nil {
		t.Error("Expected error for mismatched old prices and weights")
	}
}

// Benchmark tests

func BenchmarkCalculate10(b *testing.B) {
	prices := make([]float64, 10)
	weights := make([]float64, 10)
	for i := 0; i < 10; i++ {
		prices[i] = 1000.0 + float64(i)*10.0
		weights[i] = 0.1
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Calculate(prices, weights, 1.0)
	}
}

func BenchmarkCalculate50(b *testing.B) {
	prices := make([]float64, 50)
	weights := make([]float64, 50)
	for i := 0; i < 50; i++ {
		prices[i] = 1000.0 + float64(i)*10.0
		weights[i] = 1.0 / 50.0
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Calculate(prices, weights, 1.0)
	}
}

func BenchmarkCalculate100(b *testing.B) {
	prices := make([]float64, 100)
	weights := make([]float64, 100)
	for i := 0; i < 100; i++ {
		prices[i] = 1000.0 + float64(i)*10.0
		weights[i] = 0.01
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Calculate(prices, weights, 1.0)
	}
}

func BenchmarkWeightsMarketCap50(b *testing.B) {
	marketCaps := make([]float64, 50)
	for i := 0; i < 50; i++ {
		marketCaps[i] = 1000000.0 + float64(i)*10000.0
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightsMarketCap(marketCaps)
	}
}

func BenchmarkRebalanceTrades50(b *testing.B) {
	oldWeights := make([]float64, 50)
	newWeights := make([]float64, 50)
	prices := make([]float64, 50)

	for i := 0; i < 50; i++ {
		oldWeights[i] = 1.0 / 50.0
		newWeights[i] = (1.0 + (float64(i%2)*0.2 - 0.1)) / 50.0
		prices[i] = 1000.0 + float64(i)*10.0
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = RebalanceTrades(oldWeights, newWeights, prices, 10000.0)
	}
}
