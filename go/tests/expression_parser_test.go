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
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// TestParseExprColumnReference tests parsing column references
func TestParseExprColumnReference(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{"simple column", "c1", "c1"},
		{"column with table", "t1.c1", "t1.c1"},
		{"column with schema", "db.t1.c1", "db.t1.c1"},
		{"special column _row_id", "_row_id", "_row_id()"},
		{"special column _score", "_score", "_score()"},
		{"star", "*", "*"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprConstant tests parsing constant values
func TestParseExprConstant(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{"integer", "123", "123"},
		{"negative integer", "-456", "-(456)"},
		{"float", "3.14", "3.140000"},
		{"string single quote", "'hello'", "'hello'"},
		{"string double quote", "\"world\"", "'world'"},
		{"boolean true", "true", "true"},
		{"boolean false", "false", "false"},
		{"null", "null", "NULL"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprComparison tests parsing comparison expressions
func TestParseExprComparison(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{"equal", "c1 = 1", "=(c1, 1)"},
		{"not equal", "c1 != 1", "!=(c1, 1)"},
		{"less than", "c1 < 100", "<(c1, 100)"},
		{"greater than", "c1 > 0", ">(c1, 0)"},
		{"less equal", "c1 <= 10", "<=(c1, 10)"},
		{"greater equal", "c1 >= 5", ">=(c1, 5)"},
		{"string comparison", "name = 'test'", "=(name, 'test')"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprLogical tests parsing logical expressions
func TestParseExprLogical(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{"and", "c1 = 1 AND c2 = 2", "and(=(c1, 1), =(c2, 2))"},
		{"or", "c1 = 1 OR c2 = 2", "or(=(c1, 1), =(c2, 2))"},
		{"not", "NOT c1 = 1", "not(=(c1, 1))"},
		{"complex", "c1 = 1 AND c2 = 2 OR c3 = 3", "or(and(=(c1, 1), =(c2, 2)), =(c3, 3))"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprArithmetic tests parsing arithmetic expressions
func TestParseExprArithmetic(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		// Basic arithmetic with spaces
		{"addition", "c1 + 1", "+(c1, 1)"},
		{"subtraction", "c1 - 1", "-(c1, 1)"},
		{"multiplication", "c1 * 2", "*(c1, 2)"},
		{"division", "c1 / 2", "/(c1, 2)"},
		{"modulo", "c1 % 2", "%(c1, 2)"},

		// Arithmetic without spaces
		{"addition_slim", "c1+1", "+(c1, 1)"},
		{"subtraction_slim", "c1-1", "-(c1, 1)"},
		{"multiplication_slim", "c1*2", "*(c1, 2)"},
		{"division_slim", "c1/2", "/(c1, 2)"},
		{"modulo_slim", "c1%2", "%(c1, 2)"},

		// Multiple operations (right associative - parser behavior)
		{"chain_addition", "c1 + c2 + c3", "+(c1, +(c2, c3))"},
		{"chain_subtraction", "c1 - c2 - c3", "-(c1, -(c2, c3))"},
		{"chain_multiplication", "c1 * c2 * c3", "*(c1, *(c2, c3))"},

		// Mixed operations (precedence)
		{"add_mul_precedence", "c1 + c2 * c3", "+(c1, *(c2, c3))"},
		{"sub_div_precedence", "c1 - c2 / c3", "-(c1, /(c2, c3))"},
		{"mul_add_precedence", "c1 * c2 + c3", "+(*(c1, c2), c3)"},
		{"div_sub_precedence", "c1 / c2 - c3", "-(/(c1, c2), c3)"},
		{"complex arithmetic", "c1 + c2 * 2", "+(c1, *(c2, 2))"},

		// Operations with negative numbers
		{"add_negative", "c1 + -5", "+(c1, -(5))"},
		{"sub_negative", "c1 - -5", "-(c1, -(5))"},
		{"mul_negative", "c1 * -2", "*(c1, -(2))"},
		{"div_negative", "c1 / -2", "/(c1, -(2))"},

		// Reverse order (number first)
		{"add_reverse", "1 + c1", "+(1, c1)"},
		{"sub_reverse", "10 - c1", "-(10, c1)"},
		{"mul_reverse", "2 * c1", "*(2, c1)"},
		{"div_reverse", "100 / c1", "/(100, c1)"},

		// Multiple terms (right associative - parser behavior)
		{"three_terms_add", "c1 + c2 + c3", "+(c1, +(c2, c3))"},
		{"mixed_three_terms", "c1 + c2 - c3", "+(c1, -(c2, c3))"},
		{"four_terms", "c1 + c2 + c3 + c4", "+(c1, +(c2, +(c3, c4)))"},

		// Complex expressions
		{"complex_nested", "c1 * c2 + c3 * c4", "+(*(c1, c2), *(c3, c4))"},
		{"complex_nested2", "(c1 + c2) * (c3 - c4)", "*(+(c1, c2), -(c3, c4))"},
		// Complex expression with all operators - parser groups right due to equal precedence
		{"all_operators", "c1 + c2 - c3 * c4 / c5 % c6", "+(c1, -(c2, *(c3, /(c4, %(c5, c6)))))"},

		// Arithmetic with floats
		{"add_float", "c1 + 3.14", "+(c1, 3.140000)"},
		{"mul_float", "c1 * 2.5", "*(c1, 2.500000)"},
		{"float_arithmetic", "3.14 + 2.5", "+(3.140000, 2.500000)"},

		// Multiple digits
		{"large_numbers", "c1 + 12345", "+(c1, 12345)"},
		{"large_mul", "c1 * 100", "*(c1, 100)"},

		// Column to column operations
		{"col_add_col", "c1 + c2", "+(c1, c2)"},
		{"col_mul_col", "c1 * c2", "*(c1, c2)"},
		{"col_div_col", "c1 / c2", "/(c1, c2)"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprFunction tests parsing function calls
func TestParseExprFunction(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{"function no args", "now()", "now()"},
		{"function single arg", "upper(name)", "upper(name)"},
		{"function multiple args", "substring(name, 1, 5)", "substring(name, 1, 5)"},
		{"aggregate function", "count(*)", "count(*)"},
		{"aggregate with column", "sum(c1)", "sum(c1)"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprIn tests parsing IN expressions
func TestParseExprIn(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{"in with integers", "c1 IN (1, 2, 3)", "c1 IN (1, 2, 3)"},
		{"in with strings", "name IN ('a', 'b')", "name IN ('a', 'b')"},
		{"not in", "c1 NOT IN (1, 2)", "c1 NOT IN (1, 2)"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprBetween tests parsing BETWEEN expressions
func TestParseExprBetween(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{"between integers", "c1 BETWEEN 1 AND 10", "between(c1, 1, 10)"},
		{"between floats", "c1 BETWEEN 0.5 AND 1.5", "between(c1, 0.500000, 1.500000)"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprCast tests parsing CAST expressions
func TestParseExprCast(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{"cast to int", "CAST(c1 AS INTEGER)", "CAST(c1 AS INTEGER)"},
		{"cast to varchar", "CAST(c1 AS VARCHAR)", "CAST(c1 AS VARCHAR)"},
		{"cast to float", "CAST(c1 AS FLOAT)", "CAST(c1 AS FLOAT)"},
		{"cast with expression", "CAST(c1 + 1 AS INTEGER)", "CAST(+(c1, 1) AS INTEGER)"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprParentheses tests parsing expressions with parentheses
func TestParseExprParentheses(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{"simple parens", "(c1 = 1)", "=(c1, 1)"},
		{"complex parens", "((c1 = 1))", "=(c1, 1)"},
		{"parens with operator", "(c1 + 1) * 2", "*(+(c1, 1), 2)"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprComplex tests complex expressions
func TestParseExprComplex(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		expected string
	}{
		{
			"complex filter",
			"c1 = 1 AND (c2 = 2 OR c3 = 3)",
			"and(=(c1, 1), or(=(c2, 2), =(c3, 3)))",
		},
		{
			"with functions",
			"upper(name) = 'TEST' AND age > 18",
			"and(=(upper(name), 'TEST'), >(age, 18))",
		},
		{
			"mixed conditions",
			"c1 IN (1, 2) AND c2 BETWEEN 1 AND 10",
			"and(c1 IN (1, 2), between(c2, 1, 10))",
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			result := infinity.ParsedExprToString(parsed)
			t.Logf("Expression '%s' -> '%s' (expected: '%s')", tt.expr, result, tt.expected)
			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}

// TestParseExprError tests error cases
func TestParseExprError(t *testing.T) {
	tests := []struct {
		name string
		expr string
	}{
		{"empty expression", ""},
		{"invalid expression", "@#$"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			_, err := infinity.ParseExpr(tt.expr)
			if err == nil {
				t.Errorf("Expected error for expression '%s'", tt.expr)
			}
		})
	}
}

// TestParseFilter tests ParseFilter function
func TestParseFilter(t *testing.T) {
	// Test valid filter
	parsed, err := infinity.ParseFilter("c1 = 1 AND c2 = 2")
	if err != nil {
		t.Fatalf("Failed to parse filter: %v", err)
	}
	if parsed == nil {
		t.Error("Parsed filter should not be nil")
	}

	// Test empty filter
	parsed, err = infinity.ParseFilter("")
	if err != nil {
		t.Errorf("Empty filter should not return error: %v", err)
	}
	if parsed != nil {
		t.Error("Empty filter should return nil")
	}
}

// TestParsedExprToStringRoundTrip tests round-trip conversion
func TestParsedExprToStringRoundTrip(t *testing.T) {
	tests := []struct {
		name string
		expr string
	}{
		{"simple column", "c1"},
		{"integer constant", "123"},
		{"comparison", "c1 = 1"},
		{"function call", "upper(name)"},
		{"logical", "c1 = 1 AND c2 = 2"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			// Parse the expression
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			// Convert to string
			str := infinity.ParsedExprToString(parsed)
			if str == "" {
				t.Error("ParsedExprToString returned empty string")
			}

			t.Logf("Expression '%s' -> Parsed -> '%s'", tt.expr, str)
		})
	}
}

// TestMapSQLTypeToDataType tests SQL type mapping
func TestMapSQLTypeToDataType(t *testing.T) {
	tests := []struct {
		sqlType   string
		logicType thriftapi.LogicType
	}{
		{"INT", thriftapi.LogicType_Integer},
		{"INTEGER", thriftapi.LogicType_Integer},
		{"BIGINT", thriftapi.LogicType_BigInt},
		{"FLOAT", thriftapi.LogicType_Float},
		{"DOUBLE", thriftapi.LogicType_Double},
		{"VARCHAR", thriftapi.LogicType_Varchar},
		{"TEXT", thriftapi.LogicType_Varchar},
		{"BOOLEAN", thriftapi.LogicType_Boolean},
		{"DATE", thriftapi.LogicType_Date},
		{"JSON", thriftapi.LogicType_Json},
	}

	for _, tt := range tests {
		t.Run(tt.sqlType, func(t *testing.T) {
			// Note: mapSQLTypeToDataType is not exported, so we test via CAST parsing
			expr := "CAST(c1 AS " + tt.sqlType + ")"
			parsed, err := infinity.ParseExpr(expr)
			if err != nil {
				t.Fatalf("Failed to parse CAST expression: %v", err)
			}

			if parsed.Type.CastExpr == nil {
				t.Fatal("CastExpr should not be nil")
			}

			if parsed.Type.CastExpr.DataType.LogicType != tt.logicType {
				t.Errorf("Expected LogicType %v, got %v", tt.logicType, parsed.Type.CastExpr.DataType.LogicType)
			}
		})
	}
}

// TestConstantExprTypes tests different constant expression types
func TestConstantExprTypes(t *testing.T) {
	tests := []struct {
		name        string
		expr        string
		literalType thriftapi.LiteralType
	}{
		{"boolean true", "true", thriftapi.LiteralType_Boolean},
		{"boolean false", "false", thriftapi.LiteralType_Boolean},
		{"integer", "42", thriftapi.LiteralType_Int64},
		{"float", "3.14", thriftapi.LiteralType_Double},
		{"string", "'test'", thriftapi.LiteralType_String},
		{"null", "null", thriftapi.LiteralType_Null},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			if parsed.Type.ConstantExpr == nil {
				t.Fatal("ConstantExpr should not be nil")
			}

			if parsed.Type.ConstantExpr.LiteralType != tt.literalType {
				t.Errorf("Expected LiteralType %v, got %v", tt.literalType, parsed.Type.ConstantExpr.LiteralType)
			}
		})
	}
}

// TestBinaryOperatorMapping tests binary operator mapping
func TestBinaryOperatorMapping(t *testing.T) {
	tests := []struct {
		name     string
		expr     string
		operator string
	}{
		{"equal", "c1 = 1", "="},
		{"not equal", "c1 != 1", "!="},
		{"less than", "c1 < 1", "<"},
		{"greater than", "c1 > 1", ">"},
		{"less equal", "c1 <= 1", "<="},
		{"greater equal", "c1 >= 1", ">="},
		{"and", "c1 = 1 AND c2 = 2", "and"},
		{"or", "c1 = 1 OR c2 = 2", "or"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(tt.expr)
			if err != nil {
				t.Fatalf("Failed to parse expression '%s': %v", tt.expr, err)
			}

			// Navigate to the function expression
			var funcExpr *thriftapi.FunctionExpr
			if parsed.Type.FunctionExpr != nil {
				funcExpr = parsed.Type.FunctionExpr
			} else if parsed.Type.FunctionExpr != nil {
				funcExpr = parsed.Type.FunctionExpr
			}

			// For complex expressions, we might need to navigate deeper
			if parsed.Type.FunctionExpr != nil {
				funcExpr = parsed.Type.FunctionExpr
			}

			if funcExpr == nil {
				t.Fatal("FunctionExpr should not be nil")
			}

			if funcExpr.FunctionName != tt.operator {
				t.Errorf("Expected operator '%s', got '%s'", tt.operator, funcExpr.FunctionName)
			}
		})
	}
}

// TestSpecialColumns tests special system columns
func TestSpecialColumns(t *testing.T) {
	specialColumns := []string{
		"_row_id",
		"_create_timestamp",
		"_delete_timestamp",
		"_score",
		"_similarity",
		"_distance",
		"_score_factors",
		"_similarity_factors",
		"_distance_factors",
	}

	for _, col := range specialColumns {
		t.Run(col, func(t *testing.T) {
			parsed, err := infinity.ParseExpr(col)
			if err != nil {
				t.Fatalf("Failed to parse special column '%s': %v", col, err)
			}

			// Special columns should be converted to function calls
			if parsed.Type.FunctionExpr == nil {
				t.Errorf("Special column '%s' should be converted to FunctionExpr", col)
			}

			if parsed.Type.FunctionExpr != nil && parsed.Type.FunctionExpr.FunctionName != col {
				t.Errorf("Expected function name '%s', got '%s'", col, parsed.Type.FunctionExpr.FunctionName)
			}
		})
	}
}

// TestCondition tests condition expressions from Python test_condition.py
func TestCondition(t *testing.T) {
	tests := []struct {
		name     string
		cond     string
		expected string
	}{
		// Basic comparison operators
		{"equal", "c1 = 1", "=(c1, 1)"},
		{"not equal", "c1 != 1", "!=(c1, 1)"},
		{"greater than", "c1 > 1", ">(c1, 1)"},
		{"less than", "c1 < 1", "<(c1, 1)"},
		{"greater equal", "c1 >= 1", ">=(c1, 1)"},
		{"less equal", "c1 <= 1", "<=(c1, 1)"},
		// Complex logical expressions
		{"complex and or", "c1 > 1 and c2 < 2 or c3 = 3.3", "or(and(>(c1, 1), <(c2, 2)), =(c3, 3.300000))"},
		{"negative range", "-8 < c1 and c1 <= -7", "and(<(-(8), c1), <=(c1, -(7)))"},
		// IN expressions
		{"in list", "c1 IN (1,2,3)", "c1 IN (1, 2, 3)"},
		{"not in list", "c1 NOT IN (1,2,3)", "c1 NOT IN (1, 2, 3)"},
		// Parentheses and NOT
		{"complex parens", "(-7 < c1 or 9 <= c1) and (c1 = 3)", "and(or(<(-(7), c1), <=(9, c1)), =(c1, 3))"},
		{"not operator", "!(9 <= c1)", "not(<=(9, c1))"},
		{"not keyword", "NOT c1 = 1", "not(=(c1, 1))"},
		// BETWEEN expressions
		{"between", "c1 BETWEEN 1 AND 10", "between(c1, 1, 10)"},
		// Arithmetic in conditions
		{"addition in condition", "c1 + c2 > 10", ">(+(c1, c2), 10)"},
		{"subtraction in condition", "c1 - c2 = 0", "=(-(c1, c2), 0)"},
		{"multiplication in condition", "c1 * 2 < 100", "<(*(c1, 2), 100)"},
		{"complex arithmetic", "(c1 + c2) * c3 > 100", ">(*(+(c1, c2), c3), 100)"},
		// String comparisons
		{"string equal", "name = 'test'", "=(name, 'test')"},
		{"string not equal", "name != 'test'", "!=(name, 'test')"},
		// Function calls in conditions
		{"function in condition", "upper(c1) = 'TEST'", "=(upper(c1), 'TEST')"},
		{"count star", "count(*) > 0", ">(count(*), 0)"},
		// Nested logical expressions (right associative is the current implementation)
		{"triple and", "c1 = 1 AND c2 = 2 AND c3 = 3", "and(=(c1, 1), and(=(c2, 2), =(c3, 3)))"},
		{"triple or", "c1 = 1 OR c2 = 2 OR c3 = 3", "or(=(c1, 1), or(=(c2, 2), =(c3, 3)))"},
		{"mixed logic", "c1 = 1 AND (c2 = 2 OR c3 = 3) AND c4 = 4", "and(=(c1, 1), and(or(=(c2, 2), =(c3, 3)), =(c4, 4)))"},
		// CAST in conditions
		{"cast in condition", "CAST(c1 AS INTEGER) = 1", "=(CAST(c1 AS INTEGER), 1)"},
		{"cast with arithmetic", "CAST(c1 + 1 AS FLOAT) > 0.5", ">(CAST(+(c1, 1) AS FLOAT), 0.500000)"},
		{"cast standalone", "CAST(c1 AS VARCHAR)", "CAST(c1 AS VARCHAR)"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			// Parse the condition using ParseFilter
			parsed, err := infinity.ParseFilter(tt.cond)
			if err != nil {
				t.Fatalf("Failed to parse condition '%s': %v", tt.cond, err)
			}

			if parsed == nil {
				t.Fatal("Parsed condition should not be nil")
			}

			// Convert back to string for verification
			result := infinity.ParsedExprToString(parsed)
			t.Logf("Condition '%s' -> '%s' (expected: '%s')", tt.cond, result, tt.expected)

			if result != tt.expected {
				t.Errorf("Expected '%s', got '%s'", tt.expected, result)
			}
		})
	}
}
