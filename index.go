package crypto

/*
#include <index.h>
*/
import "C"
import (
	"errors"
	"unsafe"
)

// IndexWeightMethod represents the weighting method for index construction
type IndexWeightMethod int

const (
	IndexWeightEqual     IndexWeightMethod = C.FC_INDEX_WEIGHT_EQUAL
	IndexWeightMarketCap IndexWeightMethod = C.FC_INDEX_WEIGHT_MARKET_CAP
	IndexWeightLiquidity IndexWeightMethod = C.FC_INDEX_WEIGHT_LIQUIDITY
	IndexWeightCustom    IndexWeightMethod = C.FC_INDEX_WEIGHT_CUSTOM
)

// Calculate computes the index value from component prices and weights.
// Formula: index_value = Σ(prices[i] × weights[i]) / divisor
//
// Parameters:
//   - prices: slice of component prices
//   - weights: slice of component weights (must sum to 1.0)
//   - divisor: index divisor (must be > 0)
//
// Returns the calculated index value or an error.
func Calculate(prices, weights []float64, divisor float64) (float64, error) {
	if len(prices) == 0 || len(weights) == 0 {
		return 0, errors.New("prices and weights cannot be empty")
	}
	if len(prices) != len(weights) {
		return 0, errors.New("prices and weights must have the same length")
	}
	if divisor <= 0 {
		return 0, errors.New("divisor must be positive")
	}

	var indexValue C.double
	ret := C.fc_index_calculate(
		(*C.double)(unsafe.Pointer(&prices[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.int32_t(len(prices)),
		C.double(divisor),
		&indexValue,
	)

	if ret != C.FC_OK {
		return 0, errors.New("index calculation failed")
	}

	return float64(indexValue), nil
}

// CalculateBatch computes index values for multiple timestamps with the same weights.
// Useful for backtesting or historical analysis.
//
// Parameters:
//   - pricesMatrix: 2D slice of prices [numTimestamps][numComponents]
//   - weights: slice of component weights
//   - divisor: index divisor
//
// Returns a slice of index values for each timestamp or an error.
func CalculateBatch(pricesMatrix [][]float64, weights []float64, divisor float64) ([]float64, error) {
	if len(pricesMatrix) == 0 || len(weights) == 0 {
		return nil, errors.New("pricesMatrix and weights cannot be empty")
	}

	numTimestamps := len(pricesMatrix)
	numComponents := len(weights)

	flatPrices := make([]float64, numTimestamps*numComponents)
	for t := 0; t < numTimestamps; t++ {
		if len(pricesMatrix[t]) != numComponents {
			return nil, errors.New("all price rows must match the number of components")
		}
		copy(flatPrices[t*numComponents:(t+1)*numComponents], pricesMatrix[t])
	}

	indexValues := make([]float64, numTimestamps)

	ret := C.fc_index_calculate_batch(
		(*C.double)(unsafe.Pointer(&flatPrices[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.int32_t(numComponents),
		C.int32_t(numTimestamps),
		C.double(divisor),
		(*C.double)(unsafe.Pointer(&indexValues[0])),
	)

	if ret != C.FC_OK {
		return nil, errors.New("batch index calculation failed")
	}

	return indexValues, nil
}

// WeightsEqual calculates equal weights for index components.
// Each weight is set to 1/n.
func WeightsEqual(numComponents int) ([]float64, error) {
	if numComponents <= 0 {
		return nil, errors.New("numComponents must be positive")
	}

	weights := make([]float64, numComponents)

	ret := C.fc_index_weights_equal(
		C.int32_t(numComponents),
		(*C.double)(unsafe.Pointer(&weights[0])),
	)

	if ret != C.FC_OK {
		return nil, errors.New("equal weights calculation failed")
	}

	return weights, nil
}

// WeightsMarketCap calculates market-cap weighted index weights.
// Formula: weights[i] = marketCaps[i] / Σ(marketCaps)
func WeightsMarketCap(marketCaps []float64) ([]float64, error) {
	if len(marketCaps) == 0 {
		return nil, errors.New("marketCaps cannot be empty")
	}

	weights := make([]float64, len(marketCaps))

	ret := C.fc_index_weights_market_cap(
		(*C.double)(unsafe.Pointer(&marketCaps[0])),
		C.int32_t(len(marketCaps)),
		(*C.double)(unsafe.Pointer(&weights[0])),
	)

	if ret != C.FC_OK {
		return nil, errors.New("market cap weights calculation failed")
	}

	return weights, nil
}

// WeightsLiquidity calculates liquidity weighted index weights.
// Formula: weights[i] = liquidity[i] / Σ(liquidity)
func WeightsLiquidity(liquidity []float64) ([]float64, error) {
	if len(liquidity) == 0 {
		return nil, errors.New("liquidity cannot be empty")
	}

	weights := make([]float64, len(liquidity))

	ret := C.fc_index_weights_liquidity(
		(*C.double)(unsafe.Pointer(&liquidity[0])),
		C.int32_t(len(liquidity)),
		(*C.double)(unsafe.Pointer(&weights[0])),
	)

	if ret != C.FC_OK {
		return nil, errors.New("liquidity weights calculation failed")
	}

	return weights, nil
}

// WeightsNormalize normalizes weights to sum to 1.0.
func WeightsNormalize(weights []float64) error {
	if len(weights) == 0 {
		return errors.New("weights cannot be empty")
	}

	ret := C.fc_index_weights_normalize(
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.int32_t(len(weights)),
	)

	if ret != C.FC_OK {
		return errors.New("weight normalization failed")
	}

	return nil
}

// RebalanceTrades calculates the trades needed to transition from old weights to new weights.
//
// Parameters:
//   - oldWeights: current component weights
//   - newWeights: target component weights
//   - prices: current component prices
//   - indexValue: current index value
//
// Returns a slice of trade amounts (positive = buy, negative = sell) or an error.
func RebalanceTrades(oldWeights, newWeights, prices []float64, indexValue float64) ([]float64, error) {
	if len(oldWeights) == 0 || len(newWeights) == 0 || len(prices) == 0 {
		return nil, errors.New("oldWeights, newWeights, and prices cannot be empty")
	}
	if len(oldWeights) != len(newWeights) || len(oldWeights) != len(prices) {
		return nil, errors.New("oldWeights, newWeights, and prices must have the same length")
	}

	tradeAmounts := make([]float64, len(oldWeights))

	ret := C.fc_index_rebalance_trades(
		(*C.double)(unsafe.Pointer(&oldWeights[0])),
		(*C.double)(unsafe.Pointer(&newWeights[0])),
		(*C.double)(unsafe.Pointer(&prices[0])),
		C.int32_t(len(oldWeights)),
		C.double(indexValue),
		(*C.double)(unsafe.Pointer(&tradeAmounts[0])),
	)

	if ret != C.FC_OK {
		return nil, errors.New("rebalance trades calculation failed")
	}

	return tradeAmounts, nil
}

// RebalanceCost estimates the total cost of rebalancing based on trade amounts and slippage.
//
// Parameters:
//   - tradeAmounts: slice of trade amounts (from RebalanceTrades)
//   - prices: current component prices
//   - slippageRates: slice of slippage rates per component
//
// Returns the total rebalancing cost or an error.
func RebalanceCost(tradeAmounts, prices, slippageRates []float64) (float64, error) {
	if len(tradeAmounts) == 0 || len(prices) == 0 || len(slippageRates) == 0 {
		return 0, errors.New("tradeAmounts, prices, and slippageRates cannot be empty")
	}
	if len(tradeAmounts) != len(prices) || len(tradeAmounts) != len(slippageRates) {
		return 0, errors.New("tradeAmounts, prices, and slippageRates must have the same length")
	}

	var totalCost C.double

	ret := C.fc_index_rebalance_cost(
		(*C.double)(unsafe.Pointer(&tradeAmounts[0])),
		(*C.double)(unsafe.Pointer(&prices[0])),
		(*C.double)(unsafe.Pointer(&slippageRates[0])),
		C.int32_t(len(tradeAmounts)),
		&totalCost,
	)

	if ret != C.FC_OK {
		return 0, errors.New("rebalance cost calculation failed")
	}

	return float64(totalCost), nil
}

// AdjustDivisor calculates a new divisor after component changes.
// This maintains index continuity when components are added/removed.
//
// Parameters:
//   - oldPrices: old component prices
//   - oldWeights: old component weights
//   - newPrices: new component prices
//   - newWeights: new component weights
//   - oldDivisor: current divisor
//
// Returns the new divisor or an error.
func AdjustDivisor(oldPrices, oldWeights, newPrices, newWeights []float64, oldDivisor float64) (float64, error) {
	if len(oldPrices) == 0 || len(oldWeights) == 0 || len(newPrices) == 0 || len(newWeights) == 0 {
		return 0, errors.New("all price and weight slices cannot be empty")
	}
	if len(oldPrices) != len(oldWeights) {
		return 0, errors.New("oldPrices and oldWeights must have the same length")
	}
	if len(newPrices) != len(newWeights) {
		return 0, errors.New("newPrices and newWeights must have the same length")
	}

	var newDivisor C.double

	ret := C.fc_index_adjust_divisor(
		(*C.double)(unsafe.Pointer(&oldPrices[0])),
		(*C.double)(unsafe.Pointer(&oldWeights[0])),
		C.int32_t(len(oldPrices)),
		(*C.double)(unsafe.Pointer(&newPrices[0])),
		(*C.double)(unsafe.Pointer(&newWeights[0])),
		C.int32_t(len(newPrices)),
		C.double(oldDivisor),
		&newDivisor,
	)

	if ret != C.FC_OK {
		return 0, errors.New("divisor adjustment failed")
	}

	return float64(newDivisor), nil
}
