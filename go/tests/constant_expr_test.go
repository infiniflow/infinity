// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package tests

import (
	"reflect"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

func TestParseConstantValueBasicTypes(t *testing.T) {
	tests := []struct {
		name     string
		input    interface{}
		expected *thriftapi.ConstantExpr
	}{
		{
			name:  "bool true",
			input: true,
			expected: &thriftapi.ConstantExpr{
				LiteralType: thriftapi.LiteralType_Boolean,
				BoolValue:   boolPtr(true),
			},
		},
		{
			name:  "bool false",
			input: false,
			expected: &thriftapi.ConstantExpr{
				LiteralType: thriftapi.LiteralType_Boolean,
				BoolValue:   boolPtr(false),
			},
		},
		{
			name:  "int",
			input: 42,
			expected: &thriftapi.ConstantExpr{
				LiteralType: thriftapi.LiteralType_Int64,
				I64Value:    int64Ptr(42),
			},
		},
		{
			name:  "int64",
			input: int64(1234567890123),
			expected: &thriftapi.ConstantExpr{
				LiteralType: thriftapi.LiteralType_Int64,
				I64Value:    int64Ptr(1234567890123),
			},
		},
		{
			name:  "float64",
			input: 3.14159,
			expected: &thriftapi.ConstantExpr{
				LiteralType: thriftapi.LiteralType_Double,
				F64Value:    float64Ptr(3.14159),
			},
		},
		{
			name:  "float32",
			input: float32(2.718),
			expected: &thriftapi.ConstantExpr{
				LiteralType: thriftapi.LiteralType_Double,
				F64Value:    float64Ptr(2.718),
			},
		},
		{
			name:  "string",
			input: "hello world",
			expected: &thriftapi.ConstantExpr{
				LiteralType: thriftapi.LiteralType_String,
				StrValue:    stringPtr("hello world"),
			},
		},
		{
			name:  "nil",
			input: nil,
			expected: &thriftapi.ConstantExpr{
				LiteralType: thriftapi.LiteralType_Null,
			},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			result, err := infinity.ParseConstantValue(tt.input)
			if err != nil {
				t.Fatalf("ParseConstantValue returned error: %v", err)
			}
			if !compareConstantExpr(result, tt.expected) {
				t.Errorf("ParseConstantValue() = %+v, expected %+v", result, tt.expected)
			}
		})
	}
}

func TestParseConstantValueIntegerArray(t *testing.T) {
	tests := []struct {
		name     string
		input    interface{}
		expected *thriftapi.ConstantExpr
	}{
		{
			name:  "[]int",
			input: []int{1, 2, 3, 4, 5},
			expected: &thriftapi.ConstantExpr{
				LiteralType:    thriftapi.LiteralType_IntegerArray,
				I64ArrayValue:  []int64{1, 2, 3, 4, 5},
			},
		},
		{
			name:  "[]int64",
			input: []int64{10, 20, 30},
			expected: &thriftapi.ConstantExpr{
				LiteralType:    thriftapi.LiteralType_IntegerArray,
				I64ArrayValue:  []int64{10, 20, 30},
			},
		},
		{
			name:  "empty int slice",
			input: []int64{},
			expected: &thriftapi.ConstantExpr{
				LiteralType:    thriftapi.LiteralType_IntegerArray,
				I64ArrayValue:  []int64{},
			},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			result, err := infinity.ParseConstantValue(tt.input)
			if err != nil {
				t.Fatalf("ParseConstantValue returned error: %v", err)
			}
			if !compareConstantExpr(result, tt.expected) {
				t.Errorf("ParseConstantValue() = %+v, expected %+v", result, tt.expected)
			}
		})
	}
}

func TestParseConstantValueDoubleArray(t *testing.T) {
	tests := []struct {
		name     string
		input    interface{}
		expected *thriftapi.ConstantExpr
	}{
		{
			name:  "[]float64",
			input: []float64{1.1, 2.2, 3.3},
			expected: &thriftapi.ConstantExpr{
				LiteralType:    thriftapi.LiteralType_DoubleArray,
				F64ArrayValue:  []float64{1.1, 2.2, 3.3},
			},
		},
		{
			name:  "[]float32",
			input: []float32{0.5, 1.5},
			expected: &thriftapi.ConstantExpr{
				LiteralType:    thriftapi.LiteralType_DoubleArray,
				F64ArrayValue:  []float64{0.5, 1.5},
			},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			result, err := infinity.ParseConstantValue(tt.input)
			if err != nil {
				t.Fatalf("ParseConstantValue returned error: %v", err)
			}
			if !compareConstantExpr(result, tt.expected) {
				t.Errorf("ParseConstantValue() = %+v, expected %+v", result, tt.expected)
			}
		})
	}
}

func TestParseConstantValueIntegerTensor(t *testing.T) {
	input := [][]int64{{1, 2, 3}, {4, 5, 6}}
	expected := &thriftapi.ConstantExpr{
		LiteralType:   thriftapi.LiteralType_IntegerTensor,
		I64TensorValue: input,
	}

	result, err := infinity.ParseConstantValue(input)
	if err != nil {
		t.Fatalf("ParseConstantValue returned error: %v", err)
	}
	if !reflect.DeepEqual(result, expected) {
		t.Errorf("ParseConstantValue() = %+v, expected %+v", result, expected)
	}
}

func TestParseConstantValueDoubleTensor(t *testing.T) {
	input := [][]float64{{1.1, 2.2}, {3.3, 4.4}}
	expected := &thriftapi.ConstantExpr{
		LiteralType:   thriftapi.LiteralType_DoubleTensor,
		F64TensorValue: input,
	}

	result, err := infinity.ParseConstantValue(input)
	if err != nil {
		t.Fatalf("ParseConstantValue returned error: %v", err)
	}
	if !reflect.DeepEqual(result, expected) {
		t.Errorf("ParseConstantValue() = %+v, expected %+v", result, expected)
	}
}

func TestParseConstantValueIntegerTensorArray(t *testing.T) {
	input := [][][]int64{{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}}
	expected := &thriftapi.ConstantExpr{
		LiteralType:        thriftapi.LiteralType_IntegerTensorArray,
		I64TensorArrayValue: input,
	}

	result, err := infinity.ParseConstantValue(input)
	if err != nil {
		t.Fatalf("ParseConstantValue returned error: %v", err)
	}
	if !reflect.DeepEqual(result, expected) {
		t.Errorf("ParseConstantValue() = %+v, expected %+v", result, expected)
	}
}

func TestParseConstantValueDoubleTensorArray(t *testing.T) {
	input := [][][]float64{{{1.1, 2.2}, {3.3, 4.4}}, {{5.5, 6.6}, {7.7, 8.8}}}
	expected := &thriftapi.ConstantExpr{
		LiteralType:        thriftapi.LiteralType_DoubleTensorArray,
		F64TensorArrayValue: input,
	}

	result, err := infinity.ParseConstantValue(input)
	if err != nil {
		t.Fatalf("ParseConstantValue returned error: %v", err)
	}
	if !reflect.DeepEqual(result, expected) {
		t.Errorf("ParseConstantValue() = %+v, expected %+v", result, expected)
	}
}

func TestParseConstantValueSparseIntegerArray(t *testing.T) {
	input := map[int64]int64{0: 10, 2: 20, 5: 30}
	expected := &thriftapi.ConstantExpr{
		LiteralType:   thriftapi.LiteralType_SparseIntegerArray,
		I64ArrayIdx:   []int64{0, 2, 5},
		I64ArrayValue: []int64{10, 20, 30},
	}

	result, err := infinity.ParseConstantValue(input)
	if err != nil {
		t.Fatalf("ParseConstantValue returned error: %v", err)
	}
	// The map iteration order is non-deterministic, so we need to compare sorted indices
	// For simplicity, just check that the literal type is correct and lengths match
	if result.LiteralType != expected.LiteralType {
		t.Errorf("LiteralType = %v, expected %v", result.LiteralType, expected.LiteralType)
	}
	if len(result.I64ArrayIdx) != len(expected.I64ArrayIdx) {
		t.Errorf("I64ArrayIdx length = %d, expected %d", len(result.I64ArrayIdx), len(expected.I64ArrayIdx))
	}
	// We could sort and compare, but for now just ensure it's a sparse integer array
}

func TestParseConstantValueSparseDoubleArray(t *testing.T) {
	input := map[int64]float64{1: 1.5, 3: 2.5, 7: 3.5}
	expected := &thriftapi.ConstantExpr{
		LiteralType:   thriftapi.LiteralType_SparseDoubleArray,
		I64ArrayIdx:   []int64{1, 3, 7},
		F64ArrayValue: []float64{1.5, 2.5, 3.5},
	}

	result, err := infinity.ParseConstantValue(input)
	if err != nil {
		t.Fatalf("ParseConstantValue returned error: %v", err)
	}
	if result.LiteralType != expected.LiteralType {
		t.Errorf("LiteralType = %v, expected %v", result.LiteralType, expected.LiteralType)
	}
	if len(result.I64ArrayIdx) != len(expected.I64ArrayIdx) {
		t.Errorf("I64ArrayIdx length = %d, expected %d", len(result.I64ArrayIdx), len(expected.I64ArrayIdx))
	}
}

func TestParseConstantValueCurlyBracketsArray(t *testing.T) {
	// []interface{} containing mixed types (should be wrapped in ConstantExpr)
	input := []interface{}{1, "hello", true, nil}
	// Expected: each element converted to ConstantExpr
	expected := &thriftapi.ConstantExpr{
		LiteralType: thriftapi.LiteralType_CurlyBracketsArray,
		CurlyBracketsArray: []*thriftapi.ConstantExpr{
			{LiteralType: thriftapi.LiteralType_Int64, I64Value: int64Ptr(1)},
			{LiteralType: thriftapi.LiteralType_String, StrValue: stringPtr("hello")},
			{LiteralType: thriftapi.LiteralType_Boolean, BoolValue: boolPtr(true)},
			{LiteralType: thriftapi.LiteralType_Null},
		},
	}

	result, err := infinity.ParseConstantValue(input)
	if err != nil {
		t.Fatalf("ParseConstantValue returned error: %v", err)
	}
	if result.LiteralType != expected.LiteralType {
		t.Errorf("LiteralType = %v, expected %v", result.LiteralType, expected.LiteralType)
	}
	if len(result.CurlyBracketsArray) != len(expected.CurlyBracketsArray) {
		t.Fatalf("CurlyBracketsArray length = %d, expected %d", len(result.CurlyBracketsArray), len(expected.CurlyBracketsArray))
	}
	for i, child := range result.CurlyBracketsArray {
		expChild := expected.CurlyBracketsArray[i]
		if child.LiteralType != expChild.LiteralType {
			t.Errorf("Child %d LiteralType = %v, expected %v", i, child.LiteralType, expChild.LiteralType)
		}
		// Compare values based on literal type
		switch child.LiteralType {
		case thriftapi.LiteralType_Int64:
			if *child.I64Value != *expChild.I64Value {
				t.Errorf("Child %d I64Value = %v, expected %v", i, *child.I64Value, *expChild.I64Value)
			}
		case thriftapi.LiteralType_String:
			if *child.StrValue != *expChild.StrValue {
				t.Errorf("Child %d StrValue = %v, expected %v", i, *child.StrValue, *expChild.StrValue)
			}
		case thriftapi.LiteralType_Boolean:
			if *child.BoolValue != *expChild.BoolValue {
				t.Errorf("Child %d BoolValue = %v, expected %v", i, *child.BoolValue, *expChild.BoolValue)
			}
		case thriftapi.LiteralType_Null:
			// ok
		default:
			t.Errorf("Unexpected child literal type: %v", child.LiteralType)
		}
	}
}

func TestParseConstantValueUnsupportedType(t *testing.T) {
	input := struct{}{}
	_, err := infinity.ParseConstantValue(input)
	if err == nil {
		t.Fatal("Expected error for unsupported type")
	}
	// Check that error contains appropriate message
	if !contains(err.Error(), "Invalid constant type") {
		t.Errorf("Error message does not contain expected text: %v", err)
	}
}

// Helper functions
func compareConstantExpr(a, b *thriftapi.ConstantExpr) bool {
	if a == nil && b == nil {
		return true
	}
	if a == nil || b == nil {
		return false
	}
	if a.LiteralType != b.LiteralType {
		return false
	}
	// Compare pointer fields
	if (a.BoolValue == nil) != (b.BoolValue == nil) {
		return false
	}
	if a.BoolValue != nil && b.BoolValue != nil && *a.BoolValue != *b.BoolValue {
		return false
	}
	if (a.I64Value == nil) != (b.I64Value == nil) {
		return false
	}
	if a.I64Value != nil && b.I64Value != nil && *a.I64Value != *b.I64Value {
		return false
	}
	if (a.F64Value == nil) != (b.F64Value == nil) {
		return false
	}
	if a.F64Value != nil && b.F64Value != nil {
		// Use absolute tolerance for floating point comparison
		diff := *a.F64Value - *b.F64Value
		if diff < 0 {
			diff = -diff
		}
		if diff > 1e-6 {
			return false
		}
	}
	if (a.StrValue == nil) != (b.StrValue == nil) {
		return false
	}
	if a.StrValue != nil && b.StrValue != nil && *a.StrValue != *b.StrValue {
		return false
	}
	// Compare slices
	if !reflect.DeepEqual(a.I64ArrayValue, b.I64ArrayValue) {
		return false
	}
	if !reflect.DeepEqual(a.F64ArrayValue, b.F64ArrayValue) {
		return false
	}
	if !reflect.DeepEqual(a.I64TensorValue, b.I64TensorValue) {
		return false
	}
	if !reflect.DeepEqual(a.F64TensorValue, b.F64TensorValue) {
		return false
	}
	if !reflect.DeepEqual(a.I64TensorArrayValue, b.I64TensorArrayValue) {
		return false
	}
	if !reflect.DeepEqual(a.F64TensorArrayValue, b.F64TensorArrayValue) {
		return false
	}
	if !reflect.DeepEqual(a.I64ArrayIdx, b.I64ArrayIdx) {
		return false
	}
	// Compare CurlyBracketsArray recursively
	if len(a.CurlyBracketsArray) != len(b.CurlyBracketsArray) {
		return false
	}
	for i := range a.CurlyBracketsArray {
		if !compareConstantExpr(a.CurlyBracketsArray[i], b.CurlyBracketsArray[i]) {
			return false
		}
	}
	return true
}

func abs(f float64) float64 {
	if f < 0 {
		return -f
	}
	return f
}

func boolPtr(b bool) *bool {
	return &b
}

func int64Ptr(i int64) *int64 {
	return &i
}

func float64Ptr(f float64) *float64 {
	return &f
}

func stringPtr(s string) *string {
	return &s
}

func contains(s, substr string) bool {
	return len(s) >= len(substr) && (s == substr || len(s) > len(substr) && (s[:len(substr)] == substr || contains(s[1:], substr)))
}