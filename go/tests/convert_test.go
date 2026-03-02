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
)

// TestWithoutOutputSelectList tests that empty select list returns error
func TestWithoutOutputSelectList(t *testing.T) {
	// Connect to infinity
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Create table
	tableName := "test_without_output_select_list"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
		return
	}

	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int32",
		},
		{
			Name:     "c2",
			DataType: "float",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 2.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test empty output list - should return error
	_, err = table.Output([]string{}).ToResult()
	if err == nil {
		t.Errorf("Expected error for empty output list, but got nil")
	} else {
		infinityErr, ok := err.(*infinity.InfinityException)
		if !ok {
			t.Errorf("Expected InfinityException, got %T", err)
		} else if infinityErr.ErrorCode != int(infinity.ErrorCodeEmptySelectFields) {
			t.Errorf("Expected ErrorCodeEmptySelectFields (%d), got %d", infinity.ErrorCodeEmptySelectFields, infinityErr.ErrorCode)
		}
		t.Logf("Got expected error: %v", err)
	}
}

// TestConvertWithValidSelectListOutput tests various valid select list conditions
func TestConvertWithValidSelectListOutput(t *testing.T) {
	// Connect to infinity
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Create table
	tableName := "test_with_valid_select_list_output"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
		return
	}

	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int32",
		},
		{
			Name:     "c2",
			DataType: "float",
		},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 2.0},
		{"c1": 10, "c2": 2.0},
		{"c1": 100, "c2": 2.0},
		{"c1": 1000, "c2": 2.0},
		{"c1": 10000, "c2": 2.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test cases with valid expressions in output list
	tests := []struct {
		name    string
		columns []string
	}{
		{"condition_and", []string{"c1", "c1 > 0.1 and c2 < 3.0"}},
		{"condition_and_false", []string{"c1", "c1 > 0.1 and c2 < 1.0"}},
		{"condition_and_both_false", []string{"c1", "c1 < 0.1 and c2 < 1.0"}},
		{"column_only", []string{"c1", "c1"}},
		{"column_equals_zero", []string{"c1", "c1 = 0"}},
		{"row_id", []string{"c1", "_row_id"}},
		{"star", []string{"c1", "*"}},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			result, err := table.Output(tt.columns).ToResult()
			if err != nil {
				t.Errorf("Failed to query with columns %v: %v", tt.columns, err)
			} else {
				t.Logf("Query with columns %v succeeded, result: %v", tt.columns, result)
			}
		})
	}
}

// TestConvertWithInvalidSelectListOutput tests various invalid select list conditions
func TestConvertWithInvalidSelectListOutput(t *testing.T) {
	// Connect to infinity
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Create table
	tableName := "test_with_invalid_select_list_output"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
		return
	}

	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int32",
		},
		{
			Name:     "c2",
			DataType: "float",
		},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 2.0},
		{"c1": 10, "c2": 2.0},
		{"c1": 100, "c2": 2.0},
		{"c1": 1000, "c2": 2.0},
		{"c1": 10000, "c2": 2.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test cases with invalid expressions in output list
	tests := []struct {
		name    string
		columns []string
	}{
		{"invalid_add_and", []string{"c1", "c1 + 0.1 and c2 - 1.0"}},
		{"invalid_mul_div", []string{"c1", "c1 * 0.1 and c2 / 1.0"}},
		{"invalid_syntax", []string{"c1", "c1 > 0.1 %@#$sf c2 < 1.0"}},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			_, err := table.Output(tt.columns).ToResult()
			if err == nil {
				t.Errorf("Expected error for invalid columns %v, but got nil", tt.columns)
			} else {
				t.Logf("Got expected error for invalid columns %v: %v", tt.columns, err)
			}
		})
	}
}

// TestConvertOutputWithValidFilterFunction tests various valid filter conditions
func TestConvertOutputWithValidFilterFunction(t *testing.T) {
	// Connect to infinity
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Create table
	tableName := "test_output_with_valid_filter_function"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
		return
	}

	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int32",
		},
		{
			Name:     "c2",
			DataType: "float",
		},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 2.0},
		{"c1": 10, "c2": 2.0},
		{"c1": 100, "c2": 2.0},
		{"c1": 1000, "c2": 2.0},
		{"c1": 10000, "c2": 2.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test cases with valid filter conditions
	tests := []struct {
		name   string
		filter string
	}{
		{"c1_gt_10", "c1 > 10"},
		{"c2_gt_1", "c2 > 1"},
		{"condition_and", "c1 > 0.1 and c2 < 3.0"},
		{"condition_and_false", "c1 > 0.1 and c2 < 1.0"},
		{"condition_both_false", "c1 < 0.1 and c2 < 1.0"},
		{"condition_contradiction", "c1 < 0.1 and c1 > 1.0"},
		{"c1_equals_zero", "c1 = 0"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			result, err := table.Output([]string{"*"}).Filter(tt.filter).ToResult()
			if err != nil {
				t.Errorf("Failed to query with filter '%s': %v", tt.filter, err)
			} else {
				t.Logf("Query with filter '%s' succeeded, result: %v", tt.filter, result)
			}
		})
	}
}

// TestConvertOutputWithInvalidFilterFunction tests various invalid filter conditions
func TestConvertOutputWithInvalidFilterFunction(t *testing.T) {
	// Connect to infinity
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Create table
	tableName := "test_output_with_invalid_filter_function"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
		return
	}

	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int32",
		},
		{
			Name:     "c2",
			DataType: "float",
		},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 2.0},
		{"c1": 10, "c2": 2.0},
		{"c1": 100, "c2": 2.0},
		{"c1": 1000, "c2": 2.0},
		{"c1": 10000, "c2": 2.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test cases with invalid filter conditions
	tests := []struct {
		name   string
		filter string
	}{
		{"column_only", "c1"},
		{"row_id_only", "_row_id"},
		{"star_only", "*"},
		{"invalid_syntax", "#@$%@#f"},
		{"invalid_add_sub", "c1 + 0.1 and c2 - 1.0"},
		{"invalid_mul_div", "c1 * 0.1 and c2 / 1.0"},
		{"invalid_mixed_syntax", "c1 > 0.1 %@#$sf c2 < 1.0"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			_, err := table.Output([]string{"*"}).Filter(tt.filter).ToResult()
			if err == nil {
				t.Errorf("Expected error for invalid filter '%s', but got nil", tt.filter)
			} else {
				t.Logf("Got expected error for invalid filter '%s': %v", tt.filter, err)
			}
		})
	}
}
