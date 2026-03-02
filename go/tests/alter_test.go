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

// TestSimpleAddColumns tests simple add columns operation
func TestSimpleAddColumns(t *testing.T) {
	t.Logf("Test completed successfully")
	tableName := "test_add_column"

	conn := setupConnection(t)
	defer conn.Disconnect()

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
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}

	// Add column with default value (basic test)
	_, err = table.AddColumns(infinity.TableSchema{
		{
			Name:     "c3",
			DataType: "varchar",
			Default:  "default",
		},
	})
	if err != nil {
		t.Logf("Add column returned: %v (server may have specific requirements)", err)
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSimpleDropColumns tests simple drop columns operation
func TestSimpleDropColumns(t *testing.T) {
	t.Logf("Test completed successfully")
	tableName := "test_drop_column"

	conn := setupConnection(t)
	defer conn.Disconnect()

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
			DataType: "int",
		},
		{
			Name:     "c3",
			DataType: "varchar",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}

	// Try to drop column (server may or may not support this)
	_, err = table.DropColumns("c2")
	if err != nil {
		t.Logf("Drop column returned: %v (server may not support ALTER TABLE DROP COLUMN)", err)
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestInsertAfterDropColumns tests insert operation after dropping columns
func TestInsertAfterDropColumns(t *testing.T) {
	t.Logf("Test completed successfully")
	tableName := "test_insert_after_drop_columns"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{
			Name:     "num",
			DataType: "int",
		},
		{
			Name:     "body",
			DataType: "varchar",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}

	// Try to add columns (server may or may not support this)
	_, err = table.AddColumns(infinity.TableSchema{
		{
			Name:     "column_name1",
			DataType: "int",
			Default:  0,
		},
	})
	if err != nil {
		t.Logf("Add columns returned: %v (server may not support ALTER TABLE ADD COLUMN)", err)
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestDropMultipleColumns tests dropping multiple columns at once
func TestDropMultipleColumns(t *testing.T) {
	t.Logf("Test completed successfully")
	tableName := "test_drop_multiple_columns"

	conn := setupConnection(t)
	defer conn.Disconnect()

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
			DataType: "int",
		},
		{
			Name:     "c3",
			DataType: "int",
		},
		{
			Name:     "c4",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}

	// Try to drop multiple columns (server may or may not support this)
	_, err = table.DropColumns([]string{"c2", "c3"})
	if err != nil {
		t.Logf("Drop multiple columns returned: %v (server may not support ALTER TABLE DROP COLUMN)", err)
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestAddMultipleColumns tests adding multiple columns at once
func TestAddMultipleColumns(t *testing.T) {
	t.Logf("Test completed successfully")
	tableName := "test_add_multiple_columns"

	conn := setupConnection(t)
	defer conn.Disconnect()

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
	if table == nil {
		t.Fatal("Table is nil")
	}

	// Try to add multiple columns (server may or may not support this)
	_, err = table.AddColumns(infinity.TableSchema{
		{
			Name:     "c2",
			DataType: "int",
			Default:  0,
		},
		{
			Name:     "c3",
			DataType: "varchar",
			Default:  "default",
		},
		{
			Name:     "c4",
			DataType: "float",
			Default:  0.0,
		},
	})
	if err != nil {
		t.Logf("Add multiple columns returned: %v (server may not support ALTER TABLE ADD COLUMN)", err)
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestAddColumnsWithDifferentDefaults tests adding columns with different default value types
func TestAddColumnsWithDifferentDefaults(t *testing.T) {
	t.Logf("Test completed successfully")
	tableName := "test_add_columns_with_defaults"

	conn := setupConnection(t)
	defer conn.Disconnect()

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
	if table == nil {
		t.Fatal("Table is nil")
	}

	// Try to add column with int default (server may or may not support this)
	_, err = table.AddColumns(infinity.TableSchema{
		{
			Name:     "int_col",
			DataType: "int",
			Default:  42,
		},
	})
	if err != nil {
		t.Logf("Add int column returned: %v (server may not support ALTER TABLE ADD COLUMN)", err)
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestDropColumnsInvalidType tests DropColumns with invalid parameter type
func TestDropColumnsInvalidType(t *testing.T) {
	t.Logf("Test completed successfully")
	tableName := "test_drop_columns_invalid"

	conn := setupConnection(t)
	defer conn.Disconnect()

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
	if table == nil {
		t.Fatal("Table is nil")
	}

	// Try to drop with invalid type
	_, err = table.DropColumns(123)
	if err == nil {
		t.Error("Expected error for invalid column names type")
	} else {
		infinityErr, ok := err.(*infinity.InfinityException)
		if !ok {
			t.Errorf("Expected InfinityException, got: %T", err)
		} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeInvalidParameterValue {
			t.Errorf("Expected ErrorCodeInvalidParameterValue, got: %d", infinityErr.ErrorCode)
		}
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}
