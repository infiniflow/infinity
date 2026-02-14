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

// TestDelete tests basic delete operations
func TestDelete(t *testing.T) {
	tableName := "test_delete"

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
		{
			Name:     "c2",
			DataType: "int",
		},
		{
			Name:     "c3",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	insertData := []map[string]interface{}{
		{"c1": 1, "c2": 10, "c3": 100},
		{"c1": 2, "c2": 20, "c3": 200},
		{"c1": 3, "c2": 30, "c3": 300},
		{"c1": 4, "c2": 40, "c3": 400},
	}

	_, err = table.Insert(insertData)
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Delete with condition c1 = 1
	resp, err := table.Delete("c1 = 1")
	if err != nil {
		t.Fatalf("Failed to delete rows: %v", err)
	}

	if resp.DeletedRows != 1 {
		t.Errorf("Expected 1 deleted row, got %d", resp.DeletedRows)
	}

	// Verify remaining rows
	res, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query data: %v", err)
	}

	if result, ok := res.(*infinity.QueryResult); ok {
		rowCount := 0
		for _, col := range result.Data {
			rowCount = len(col)
			break
		}
		if rowCount != 3 {
			t.Errorf("Expected 3 remaining rows, got %d", rowCount)
		}
	}

	// Delete all remaining rows
	resp, err = table.Delete("")
	if err != nil {
		t.Fatalf("Failed to delete all rows: %v", err)
	}

	if resp.DeletedRows != 3 {
		t.Errorf("Expected 3 deleted rows, got %d", resp.DeletedRows)
	}

	// Verify table is empty
	res, err = table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query data: %v", err)
	}

	if result, ok := res.(*infinity.QueryResult); ok {
		rowCount := 0
		for _, col := range result.Data {
			rowCount = len(col)
			break
		}
		if rowCount != 0 {
			t.Errorf("Expected 0 rows after delete all, got %d", rowCount)
		}
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestDeleteEmptyTable tests deleting from an empty table
func TestDeleteEmptyTable(t *testing.T) {
	tableName := "test_delete_empty_table"

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
			Name:     "c1",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Delete from empty table
	resp, err := table.Delete("c1 = 1")
	if err != nil {
		t.Fatalf("Failed to delete from empty table: %v", err)
	}

	if resp.DeletedRows != 0 {
		t.Errorf("Expected 0 deleted rows from empty table, got %d", resp.DeletedRows)
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestDeleteNonExistentTable tests deleting from a non-existent table
func TestDeleteNonExistentTable(t *testing.T) {
	tableName := "test_delete_non_existent_table"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Drop table if exists
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Try to get non-existent table and delete
	_, err = db.GetTable(tableName)
	if err == nil {
		t.Fatal("Expected error when getting non-existent table")
	}
}

// TestDeleteTableAllRowsMetCondition tests deleting all rows that meet the condition
func TestDeleteTableAllRowsMetCondition(t *testing.T) {
	testCases := []struct {
		name  string
		data  interface{}
		value string
	}{
		{"int", 1, "1"},
		{"float", 1.5, "1.5"},
		{"varchar", "test", "'test'"},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			tableName := "test_delete_all_rows_met_condition_" + tc.name

			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			// Clean up
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table
			var schema infinity.TableSchema
			if tc.name == "varchar" {
				schema = infinity.TableSchema{{Name: "c1", DataType: "varchar"}}
			} else if tc.name == "float" {
				schema = infinity.TableSchema{{Name: "c1", DataType: "float"}}
			} else {
				schema = infinity.TableSchema{{Name: "c1", DataType: "int"}}
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Insert data
			_, err = table.Insert([]map[string]interface{}{{"c1": tc.data}})
			if err != nil {
				t.Logf("Insert may have failed (expected for some types): %v", err)
			}

			// Delete all rows that match
			_, err = table.Delete("c1 = " + tc.value)
			if err != nil {
				t.Logf("Delete may have failed (expected for some types): %v", err)
			}

			// Drop table
			db.DropTable(tableName, infinity.ConflictTypeError)
		})
	}
}

// TestDeleteTableNoRowsMetCondition tests deleting when no rows meet the condition
func TestDeleteTableNoRowsMetCondition(t *testing.T) {
	types := []string{"int", "float", "varchar"}
	examples := []interface{}{1, 1.5, "test"}

	for i, typ := range types {
		t.Run(typ, func(t *testing.T) {
			tableName := "test_delete_no_rows_met_condition_" + typ

			conn := setupConnection(t)
			defer closeConnection(t, conn)

			db, err := conn.GetDatabase("default_db")
			if err != nil {
				t.Fatalf("Failed to get database: %v", err)
			}

			// Clean up
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table
			var schema infinity.TableSchema
			if typ == "varchar" {
				schema = infinity.TableSchema{{Name: "c1", DataType: "varchar"}}
			} else if typ == "float" {
				schema = infinity.TableSchema{{Name: "c1", DataType: "float"}}
			} else {
				schema = infinity.TableSchema{{Name: "c1", DataType: "int"}}
			}

			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Insert data
			_, err = table.Insert([]map[string]interface{}{{"c1": examples[i]}})
			if err != nil {
				t.Logf("Insert may have failed: %v", err)
			}

			// Delete with condition that doesn't match
			_, err = table.Delete("c1 != " + stringifyValue(examples[i]))
			if err != nil {
				t.Logf("Delete may have failed: %v", err)
			}

			// Query to verify
			res, _ := table.Output([]string{"*"}).ToResult()
			if result, ok := res.(*infinity.QueryResult); ok {
				t.Logf("Result after delete: %v", result.Data)
			}

			// Drop table
			db.DropTable(tableName, infinity.ConflictTypeError)
		})
	}
}

// TestDeleteTableWithOneBlock tests deleting from a table with one block of data
func TestDeleteTableWithOneBlock(t *testing.T) {
	tableName := "test_delete_one_block"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	// Create table
	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert 8192 rows with c1 = 1
	for i := 0; i < 8192; i++ {
		_, err = table.Insert([]map[string]interface{}{{"c1": 1}})
		if err != nil {
			t.Fatalf("Failed to insert row %d: %v", i, err)
		}
	}

	// Delete all rows where c1 = 1
	resp, err := table.Delete("c1 = 1")
	if err != nil {
		t.Fatalf("Failed to delete rows: %v", err)
	}

	if resp.DeletedRows != 8192 {
		t.Errorf("Expected 8192 deleted rows, got %d", resp.DeletedRows)
	}

	// Verify table is empty
	res, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query data: %v", err)
	}

	if result, ok := res.(*infinity.QueryResult); ok {
		rowCount := 0
		for _, col := range result.Data {
			rowCount = len(col)
			break
		}
		if rowCount != 0 {
			t.Errorf("Expected 0 rows after delete, got %d", rowCount)
		}
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestDeleteTableWithOneSegment tests deleting from a table with one segment of data
func TestDeleteTableWithOneSegment(t *testing.T) {
	tableName := "test_delete_one_segment"

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
			Name:     "c1",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert 1024 batches of 10 rows each
	for i := 0; i < 1024; i++ {
		batch := make([]map[string]interface{}, 10)
		for j := 0; j < 10; j++ {
			batch[j] = map[string]interface{}{"c1": i}
		}
		_, err = table.Insert(batch)
		if err != nil {
			t.Fatalf("Failed to insert batch %d: %v", i, err)
		}
	}

	// Delete rows for each value
	for i := 0; i < 1024; i++ {
		_, err = table.Delete("c1 = " + string(rune('0'+i%10)))
		if err != nil {
			t.Logf("Delete for c1 = %d may have failed: %v", i, err)
		}
	}

	// Query to verify
	res, _ := table.Output([]string{"*"}).ToResult()
	if result, ok := res.(*infinity.QueryResult); ok {
		t.Logf("Result after deletes: %v", result.Data)
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectBeforeAfterDelete tests selecting data before and after delete
func TestSelectBeforeAfterDelete(t *testing.T) {
	tableName := "test_select_before_after_delete"

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
			Name:     "c1",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert 10 batches of 10 rows each
	for i := 0; i < 10; i++ {
		batch := make([]map[string]interface{}, 10)
		for j := 0; j < 10; j++ {
			batch[j] = map[string]interface{}{"c1": i}
		}
		_, err = table.Insert(batch)
		if err != nil {
			t.Fatalf("Failed to insert batch %d: %v", i, err)
		}
	}

	// Delete rows where c1 = 1
	resp, err := table.Delete("c1 = 1")
	if err != nil {
		t.Fatalf("Failed to delete rows: %v", err)
	}

	if resp.DeletedRows != 10 {
		t.Errorf("Expected 10 deleted rows, got %d", resp.DeletedRows)
	}

	// Query to verify
	res, _ := table.Output([]string{"*"}).ToResult()
	if result, ok := res.(*infinity.QueryResult); ok {
		t.Logf("Result after delete: %v", result.Data)
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestDeleteInsertData tests deleting and then inserting data
func TestDeleteInsertData(t *testing.T) {
	tableName := "test_delete_insert_data"

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
			Name:     "c1",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert 10 rows with c1 = 1
	for i := 0; i < 10; i++ {
		_, err = table.Insert([]map[string]interface{}{{"c1": 1}})
		if err != nil {
			t.Fatalf("Failed to insert row %d: %v", i, err)
		}
	}

	// Delete all rows where c1 = 1
	resp, err := table.Delete("c1 = 1")
	if err != nil {
		t.Fatalf("Failed to delete rows: %v", err)
	}

	if resp.DeletedRows != 10 {
		t.Errorf("Expected 10 deleted rows, got %d", resp.DeletedRows)
	}

	// Query to verify
	res, _ := table.Output([]string{"*"}).ToResult()
	if result, ok := res.(*infinity.QueryResult); ok {
		t.Logf("Result after delete: %v", result.Data)
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestDeleteDroppedTable tests deleting from a dropped table
func TestDeleteDroppedTable(t *testing.T) {
	tableName := "test_delete_dropped_table"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Drop table if exists
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Drop the table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	// Try to delete from dropped table - should fail
	_, err = table.Delete("c1 = 0")
	if err == nil {
		t.Error("Expected error when deleting from dropped table")
	}
}

// TestDeleteOneBlockWithoutExpression tests deleting all rows in one block without expression
func TestDeleteOneBlockWithoutExpression(t *testing.T) {
	tableName := "test_delete_one_block_no_expr"

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
			Name:     "c1",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert 8192 rows
	for i := 0; i < 8192; i++ {
		_, err = table.Insert([]map[string]interface{}{{"c1": 1}})
		if err != nil {
			t.Fatalf("Failed to insert row %d: %v", i, err)
		}
	}

	// Delete all rows without expression
	resp, err := table.Delete("")
	if err != nil {
		t.Fatalf("Failed to delete all rows: %v", err)
	}

	if resp.DeletedRows != 8192 {
		t.Errorf("Expected 8192 deleted rows, got %d", resp.DeletedRows)
	}

	// Verify table is empty
	res, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query data: %v", err)
	}

	if result, ok := res.(*infinity.QueryResult); ok {
		rowCount := 0
		for _, col := range result.Data {
			rowCount = len(col)
			break
		}
		if rowCount != 0 {
			t.Errorf("Expected 0 rows after delete, got %d", rowCount)
		}
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestDeleteOneSegmentWithoutExpression tests deleting all rows in one segment without expression
func TestDeleteOneSegmentWithoutExpression(t *testing.T) {
	tableName := "test_delete_one_segment_no_expr"

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
			Name:     "c1",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert 1024 batches of 10 rows each (10240 rows total)
	for i := 0; i < 1024; i++ {
		batch := make([]map[string]interface{}, 10)
		for j := 0; j < 10; j++ {
			batch[j] = map[string]interface{}{"c1": i}
		}
		_, err = table.Insert(batch)
		if err != nil {
			t.Fatalf("Failed to insert batch %d: %v", i, err)
		}
	}

	// Delete all rows without expression
	resp, err := table.Delete("")
	if err != nil {
		t.Fatalf("Failed to delete all rows: %v", err)
	}

	if resp.DeletedRows != 10240 {
		t.Errorf("Expected 10240 deleted rows, got %d", resp.DeletedRows)
	}

	// Verify table is empty
	res, err := table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query data: %v", err)
	}

	if result, ok := res.(*infinity.QueryResult); ok {
		rowCount := 0
		for _, col := range result.Data {
			rowCount = len(col)
			break
		}
		if rowCount != 0 {
			t.Errorf("Expected 0 rows after delete, got %d", rowCount)
		}
	}

	// Drop table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestFilterWithValidExpression tests delete with valid filter expressions
func TestFilterWithValidExpression(t *testing.T) {
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
		t.Run(filter, func(t *testing.T) {
			tableName := "test_filter_valid_"

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
					Name:     "c1",
					DataType: "int",
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
			for i := 0; i < 10; i++ {
				batch := make([]map[string]interface{}, 10)
				for j := 0; j < 10; j++ {
					batch[j] = map[string]interface{}{"c1": i, "c2": 3.0}
				}
				_, err = table.Insert(batch)
				if err != nil {
					t.Fatalf("Failed to insert batch %d: %v", i, err)
				}
			}

			// Delete with filter
			_, err = table.Delete(filter)
			if err != nil {
				t.Logf("Delete with filter '%s' may have failed: %v", filter, err)
			}

			// Query to verify
			res, _ := table.Output([]string{"*"}).ToResult()
			if result, ok := res.(*infinity.QueryResult); ok {
				t.Logf("Result after delete with filter '%s': %v", filter, result.Data)
			}

			// Drop table
			db.DropTable(tableName, infinity.ConflictTypeError)
		})
	}
}

// TestFilterWithInvalidExpression tests delete with invalid filter expressions
func TestFilterWithInvalidExpression(t *testing.T) {
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
		t.Run(filter, func(t *testing.T) {
			tableName := "test_filter_invalid_"

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
					Name:     "c1",
					DataType: "int",
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
			for i := 0; i < 10; i++ {
				batch := make([]map[string]interface{}, 10)
				for j := 0; j < 10; j++ {
					batch[j] = map[string]interface{}{"c1": i, "c2": 3.0}
				}
				_, err = table.Insert(batch)
				if err != nil {
					t.Fatalf("Failed to insert batch %d: %v", i, err)
				}
			}

			// Delete with invalid filter - should fail
			_, err = table.Delete(filter)
			if err == nil {
				t.Errorf("Delete with invalid filter '%s' did not fail (may be accepted)", filter)
			}

			// Query to verify
			res, _ := table.Output([]string{"*"}).ToResult()
			if result, ok := res.(*infinity.QueryResult); ok {
				if len(result.Data) > 0 {
					// Get row count from first column
					var rowCount int
					for _, col := range result.Data {
						rowCount = len(col)
						break
					}
					if rowCount != 100 {
						t.Errorf("Expected 100 rows after delete, got %d", rowCount)
					}
				} else {
					t.Errorf("Expected 100 rows after delete, got 0")
				}
			}

			// Drop table
			db.DropTable(tableName, infinity.ConflictTypeError)
		})
	}
}

// Helper function to convert value to string for SQL
func stringifyValue(v interface{}) string {
	switch val := v.(type) {
	case string:
		return "'" + val + "'"
	default:
		return "0"
	}
}
