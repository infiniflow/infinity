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

// TestUpdate tests basic update operations
func TestUpdate(t *testing.T) {
	tableName := "test_update"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{
			Name:        "c1",
			DataType:    "int",
			Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey, infinity.ConstraintNotNull},
		},
		{Name: "c2", DataType: "int"},
		{Name: "c3", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 10, "c3": 100},
		{"c1": 2, "c2": 20, "c3": 200},
		{"c1": 3, "c2": 30, "c3": 300},
		{"c1": 4, "c2": 40, "c3": 400},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Update with condition
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 90, "c3": 900})
	if err != nil {
		t.Errorf("Failed to update with condition: %v", err)
	}

	// Test update with nil condition (should fail)
	_, err = table.Update("", map[string]interface{}{"c2": 90, "c3": 900})
	if err == nil {
		t.Error("Expected error when updating with empty condition")
	}
}

// TestUpdateEmptyTable tests update on empty table
func TestUpdateEmptyTable(t *testing.T) {
	tableName := "test_update_empty"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
		{Name: "c3", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Update on empty table should succeed
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 90, "c3": 900})
	if err != nil {
		t.Errorf("Failed to update empty table: %v", err)
	}
}

// TestUpdateNoRowMetCondition tests update when no rows match condition
func TestUpdateNoRowMetCondition(t *testing.T) {
	tableName := "test_update_no_match"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert data
	_, err = table.Insert(map[string]interface{}{"c1": 1, "c2": 1})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Update with condition that doesn't match any row
	_, err = table.Update("1=0", map[string]interface{}{"c2": 100})
	if err != nil {
		t.Errorf("Failed to update with no match: %v", err)
	}
}

// TestUpdateAllRowMetCondition tests update when all rows match condition
func TestUpdateAllRowMetCondition(t *testing.T) {
	tableName := "test_update_all_match"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert data
	_, err = table.Insert(map[string]interface{}{"c1": 1, "c2": 1})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Update with condition that matches all rows
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 100})
	if err != nil {
		t.Errorf("Failed to update all rows: %v", err)
	}
}

// TestUpdateTableWithOneBlock tests update on table with one block
func TestUpdateTableWithOneBlock(t *testing.T) {
	tableName := "test_update_one_block"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert 8192 rows
	values := make([]map[string]interface{}, 8192)
	for i := range values {
		values[i] = map[string]interface{}{"c1": 1, "c2": 2}
	}
	_, err = table.Insert(values)
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Update
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 20})
	if err != nil {
		t.Errorf("Failed to update: %v", err)
	}
}

// TestUpdateTableWithTwoBlocks tests update on table with two blocks
func TestUpdateTableWithTwoBlocks(t *testing.T) {
	tableName := "test_update_two_blocks"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert 6000 rows
	values := make([]map[string]interface{}, 6000)
	for i := range values {
		values[i] = map[string]interface{}{"c1": 1, "c2": 2}
	}
	_, err = table.Insert(values)
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Insert 5000 more rows
	values = make([]map[string]interface{}, 5000)
	for i := range values {
		values[i] = map[string]interface{}{"c1": 1, "c2": 2}
	}
	_, err = table.Insert(values)
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Update
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 20})
	if err != nil {
		t.Errorf("Failed to update: %v", err)
	}
}

// TestUpdateBeforeDelete tests update after delete
func TestUpdateBeforeDelete(t *testing.T) {
	tableName := "test_update_before_delete"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert 8 rows
	values := make([]map[string]interface{}, 8)
	for i := range values {
		values[i] = map[string]interface{}{"c1": 1, "c2": 2}
	}
	_, err = table.Insert(values)
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Delete
	_, err = table.Delete("c1 = 1")
	if err != nil {
		t.Fatalf("Failed to delete: %v", err)
	}

	// Update after delete
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 20})
	if err != nil {
		t.Errorf("Failed to update after delete: %v", err)
	}
}

// TestUpdateDroppedTable tests update on dropped table
func TestUpdateDroppedTable(t *testing.T) {
	tableName := "test_update_dropped"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	// Update on dropped table should fail
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 21})
	if err == nil {
		t.Error("Expected error when updating dropped table")
	}
}

// TestUpdateNewValue tests update with various new values
func TestUpdateNewValue(t *testing.T) {
	tableName := "test_update_new_value"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Test int type
	db.DropTable(tableName+"_int", infinity.ConflictTypeIgnore)
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
	}
	table, _ := db.CreateTable(tableName+"_int", schema, infinity.ConflictTypeError)
	table.Insert(map[string]interface{}{"c1": 1, "c2": 2})

	// Update with int
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 1})
	if err != nil {
		t.Errorf("Failed to update int: %v", err)
	}
	// Update with float (should work for int column)
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 1.333})
	if err != nil {
		t.Errorf("Failed to update with float: %v", err)
	}
	// Update with string
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": "1"})
	if err != nil {
		t.Errorf("Failed to update with string: %v", err)
	}
	db.DropTable(tableName+"_int", infinity.ConflictTypeError)

	// Test float type
	db.DropTable(tableName+"_float", infinity.ConflictTypeIgnore)
	schema = infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "float"},
	}
	table, _ = db.CreateTable(tableName+"_float", schema, infinity.ConflictTypeError)
	table.Insert(map[string]interface{}{"c1": 1, "c2": 2})

	// Update with int
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 1})
	if err != nil {
		t.Errorf("Failed to update float with int: %v", err)
	}
	// Update with float
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": 1.333})
	if err != nil {
		t.Errorf("Failed to update float: %v", err)
	}
	// Update with string
	_, err = table.Update("c1 = 1", map[string]interface{}{"c2": "1"})
	if err != nil {
		t.Errorf("Failed to update float with string: %v", err)
	}
	db.DropTable(tableName+"_float", infinity.ConflictTypeError)
}

// TestUpdateValidFilterExpression tests update with valid filter expressions
func TestUpdateValidFilterExpression(t *testing.T) {
	tableName := "test_update_valid_filter"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert data
	for i := 0; i < 10; i++ {
		values := make([]map[string]interface{}, 10)
		for j := range values {
			values[j] = map[string]interface{}{"c1": i, "c2": 3.0}
		}
		_, err = table.Insert(values)
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Test various valid filter expressions
	filters := []string{
		"c1 > 10",
		"c2 > 1",
		"c1 > 0.1 and c2 < 3.0",
		"c1 > 0.1 and c2 < 1.0",
		"c1 < 0.1 and c2 < 1.0",
		"c1 < 0.1 and c1 > 1.0",
		"c1 = 0",
	}

	for _, filter := range filters {
		_, err = table.Update(filter, map[string]interface{}{"c2": 1.0})
		if err != nil {
			t.Errorf("Failed to update with filter '%s': %v", filter, err)
		}
	}
}

// TestUpdateInvalidFilterExpression tests update with invalid filter expressions
func TestUpdateInvalidFilterExpression(t *testing.T) {
	tableName := "test_update_invalid_filter"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert data
	for i := 0; i < 10; i++ {
		values := make([]map[string]interface{}, 10)
		for j := range values {
			values[j] = map[string]interface{}{"c1": i, "c2": 3.0}
		}
		_, err = table.Insert(values)
		if err != nil {
			t.Fatalf("Failed to insert data: %v", err)
		}
	}

	// Test various invalid filter expressions
	filters := []string{
		"c1",
		"_row_id",
		"*",
		"#@$%@#f",
		"c1 + 0.1 and c2 - 1.0",
		"c1 * 0.1 and c2 / 1.0",
		"c1 > 0.1 %@#$sf c2 < 1.0",
	}

	for _, filter := range filters {
		_, err = table.Update(filter, map[string]interface{}{"c2": 1.0})
		if err == nil {
			t.Errorf("Expected error for invalid filter '%s'", filter)
		}
	}
}

// TestUpdateEmbedding tests update with embedding column
func TestUpdateEmbedding(t *testing.T) {
	tableName := "test_update_embedding"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "vector,3,int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": []int{1, 2, 3}},
		{"c1": []int{4, 5, 6}},
		{"c1": []int{7, 8, 9}},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Update all rows
	_, err = table.Update("1 = 1", map[string]interface{}{"c1": []int{10, 20, 30}})
	if err != nil {
		t.Errorf("Failed to update embedding: %v", err)
	}
}
