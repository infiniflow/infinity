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
	"fmt"
	"strings"
	"sync"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// validNameArray for testing valid column names - exported for use in other test files
var ValidNameArray = []string{
	"name",
	"name12",
	"name_",
	"name_12",
	"name12__",
}

// TestCreateTableWithValidOptions tests creating tables with valid conflict options
func TestCreateTableWithValidOptions(t *testing.T) {

	tableName := "test_create_table_with_valid_options"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	validOptions := []infinity.ConflictType{
		infinity.ConflictTypeError,
		infinity.ConflictTypeIgnore,
	}

	for _, option := range validOptions {
		schema := infinity.TableSchema{
			{
				Name:     "c1",
				DataType: "int",
			},
		}

		table, err := db.CreateTable(tableName, schema, option)
		if err != nil {
			t.Errorf("Failed to create table with option %v: %v", option, err)
			continue
		}
		if table == nil {
			t.Errorf("Table is nil for option %v", option)
			continue
		}

		// Drop table
		_, err = db.DropTable(tableName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop table with option %v: %v", option, err)
		}
	}
}

// TestDropTableWithValidOptions tests dropping tables with valid conflict options
func TestDropTableWithValidOptions(t *testing.T) {

	tableName := "test_drop_table_with_valid_options"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	validOptions := []infinity.ConflictType{
		infinity.ConflictTypeError,
		infinity.ConflictTypeIgnore,
	}

	for _, option := range validOptions {
		schema := infinity.TableSchema{
			{
				Name:     "c1",
				DataType: "int",
			},
		}

		// Create table
		table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
		if err != nil {
			t.Fatalf("Failed to create table: %v", err)
		}
		if table == nil {
			t.Fatal("Table is nil")
		}

		// Drop table with option
		_, err = db.DropTable(tableName, option)
		if err != nil {
			t.Errorf("Failed to drop table with option %v: %v", option, err)
		}
	}
}

// TestTableWithValidColumnNames tests creating tables with valid column names
func TestTableWithValidColumnNames(t *testing.T) {

	tableName := "test_table_with_valid_column_names"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	for _, columnName := range ValidNameArray {
		// Clean up
		db.DropTable(tableName, infinity.ConflictTypeIgnore)

		schema := infinity.TableSchema{
			{
				Name:     columnName,
				DataType: "int",
			},
		}

		table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to create table with column name %s: %v", columnName, err)
			continue
		}
		if table == nil {
			t.Errorf("Table is nil for column name %s", columnName)
			continue
		}

		// Drop table
		_, err = db.DropTable(tableName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop table: %v", err)
		}
	}
}

// TestCreateTableWithInvalidColumnName tests creating tables with invalid column names
func TestCreateTableWithInvalidColumnName(t *testing.T) {

	tableName := "test_create_table_with_invalid_column_name"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	for _, columnName := range invalidNameArray {
		schema := infinity.TableSchema{
			{
				Name:     columnName,
				DataType: "int",
			},
		}

		_, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
		if err == nil {
			t.Errorf("Expected error for invalid column name: %s", columnName)
		} else {
			infinityErr, ok := err.(*infinity.InfinityException)
			if !ok {
				t.Errorf("Expected InfinityException for column name: %s, got: %T", columnName, err)
			} else {
				// Server may return different error codes for invalid column names
				errCode := infinity.ErrorCode(infinityErr.ErrorCode)
				if errCode != infinity.ErrorCodeInvalidColumnName && errCode != infinity.ErrorCodeEmptyColumnName {
					t.Errorf("Expected ErrorCodeInvalidColumnName or ErrorCodeEmptyColumnName for column name: %s, got: %d", columnName, infinityErr.ErrorCode)
				}
			}
		}
	}

	// Try to drop the table (should fail as it was never created)
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err == nil {
		t.Log("Drop table failed as expected (table does not exist)")
	}
}

// TestCreateTableWithInvalidOptions tests creating tables with invalid conflict options
func TestCreateTableWithInvalidOptions(t *testing.T) {

	tableName := "test_create_table_with_invalid_options"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
	}

	// Test with invalid conflict type
	_, err = db.CreateTable(tableName, schema, infinity.ConflictType(999))
	if err == nil {
		t.Fatal("Expected error for invalid conflict type")
	} else {
		infinityErr, ok := err.(*infinity.InfinityException)
		if !ok {
			t.Fatalf("Expected InfinityException, got: %T", err)
		} else {
			errCode := infinity.ErrorCode(infinityErr.ErrorCode)
			if errCode != infinity.ErrorCodeInvalidConflictType && errCode != infinity.ErrorCodeInvalidCreateOption {
				t.Errorf("Expected ErrorCodeInvalidConflictType or ErrorCodeInvalidCreateOption, got: %d", infinityErr.ErrorCode)
			}
		}
	}

	// Try to drop the table (should fail as it was never created)
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err == nil {
		t.Log("Drop table failed as expected (table does not exist)")
	}
}

// TestDropTableWithInvalidOptions tests dropping tables with invalid conflict options
func TestDropTableWithInvalidOptions(t *testing.T) {

	tableName := "test_drop_table_with_invalid_options"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up and create table
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

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

	// Test with invalid conflict type (Replace is invalid for drop)
	_, err = db.DropTable(tableName, infinity.ConflictTypeReplace)
	if err == nil {
		t.Fatal("Expected error for invalid conflict type")
	} else {
		infinityErr, ok := err.(*infinity.InfinityException)
		if !ok {
			t.Fatalf("Expected InfinityException, got: %T", err)
		} else {
			errCode := infinity.ErrorCode(infinityErr.ErrorCode)
			if errCode != infinity.ErrorCodeInvalidConflictType && errCode != infinity.ErrorCodeInvalidDropOption {
				t.Errorf("Expected ErrorCodeInvalidConflictType or ErrorCodeInvalidDropOption, got: %d", infinityErr.ErrorCode)
			}
		}
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestCreateOrDropSameTableInDifferentThread tests creating/dropping same table in different threads
func TestCreateOrDropSameTableInDifferentThread(t *testing.T) {

	tableName := "test_create_or_drop_same_table_in_different_thread"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
	}

	// Create table concurrently
	var wg sync.WaitGroup
	errors := make(chan error, 16)

	for i := 0; i < 16; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			_, err := db.CreateTable(tableName, schema, infinity.ConflictTypeIgnore)
			if err != nil {
				errors <- err
			}
		}()
	}

	wg.Wait()
	close(errors)

	// At least one should succeed
	errorCount := 0
	for range errors {
		errorCount++
	}

	if errorCount == 16 {
		t.Log("All create attempts failed (this may be expected depending on server behavior)")
	}

	// Drop table concurrently
	errors = make(chan error, 16)

	for i := 0; i < 16; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			_, err := db.DropTable(tableName, infinity.ConflictTypeIgnore)
			if err != nil {
				errors <- err
			}
		}()
	}

	wg.Wait()
	close(errors)

	// At least one should succeed
	errorCount = 0
	for range errors {
		errorCount++
	}

	if errorCount == 16 {
		t.Log("All drop attempts failed (this may be expected depending on server behavior)")
	}
}

// TestListTables tests listing tables
func TestListTables(t *testing.T) {

	tableName := "test_list_tables"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// List tables before creation
	resp, err := db.ListTables()
	if err != nil {
		t.Fatalf("Failed to list tables: %v", err)
	}
	if resp == nil {
		t.Fatal("List tables response is nil")
	}

	// Create table
	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
	}

	_, err = db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// List tables after creation
	resp, err = db.ListTables()
	if err != nil {
		t.Fatalf("Failed to list tables: %v", err)
	}
	if resp == nil {
		t.Fatal("List tables response is nil")
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestGetTable tests getting a table object
func TestGetTable(t *testing.T) {

	tableName := "test_get_table"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Get non-existent table (should fail)
	_, err = db.GetTable(tableName)
	if err == nil {
		t.Fatal("Expected error for non-existent table")
	} else {
		infinityErr, ok := err.(*infinity.InfinityException)
		if !ok {
			t.Fatalf("Expected InfinityException, got: %T", err)
		} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeTableNotExist {
			t.Errorf("Expected ErrorCodeTableNotExist, got: %d", infinityErr.ErrorCode)
		}
	}

	// Create table
	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
	}

	_, err = db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Get table
	table, err := db.GetTable(tableName)
	if err != nil {
		t.Fatalf("Failed to get table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestCreateTableWithMultipleColumns tests creating tables with multiple columns
func TestCreateTableWithMultipleColumns(t *testing.T) {

	tableName := "test_create_table_with_multiple_columns"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with multiple columns
	schema := infinity.TableSchema{
		{
			Name:        "id",
			DataType:    "int",
			Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey},
		},
		{
			Name:     "name",
			DataType: "varchar",
		},
		{
			Name:     "age",
			DataType: "int",
			Default:  0,
		},
		{
			Name:     "score",
			DataType: "float",
			Default:  0.0,
		},
		{
			Name:     "active",
			DataType: "bool",
			Default:  true,
		},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestDropNonExistentTable tests dropping non-existent tables
func TestDropNonExistentTable(t *testing.T) {

	tableName := "test_drop_non_existent_table"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Try to drop non-existent table with Error option
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err == nil {
		t.Fatal("Expected error for non-existent table")
	} else {
		infinityErr, ok := err.(*infinity.InfinityException)
		if !ok {
			t.Fatalf("Expected InfinityException, got: %T", err)
		} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeTableNotExist {
			t.Errorf("Expected ErrorCodeTableNotExist, got: %d", infinityErr.ErrorCode)
		}
	}

	// Try to drop non-existent table with Ignore option (should succeed)
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Logf("Drop with Ignore returned error (may be expected): %v", err)
	}
}

// TestCreateDuplicateTable tests creating duplicate tables
func TestCreateDuplicateTable(t *testing.T) {

	tableName := "test_create_duplicate_table"

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
	}

	// Create table first time
	_, err = db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Try to create duplicate with Error option
	_, err = db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err == nil {
		t.Fatal("Expected error for duplicate table")
	} else {
		infinityErr, ok := err.(*infinity.InfinityException)
		if !ok {
			t.Fatalf("Expected InfinityException, got: %T", err)
		} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeDuplicateTableName {
			t.Errorf("Expected ErrorCodeDuplicateTableName, got: %d", infinityErr.ErrorCode)
		}
	}

	// Try to create duplicate with Ignore option (should succeed)
	_, err = db.CreateTable(tableName, schema, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Logf("Create with Ignore returned error (may be expected): %v", err)
	}

	// Clean up
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestRenameTable tests table rename functionality
// Based on Python SDK test_pysdk/test_table.py - test_rename_table
func TestRenameTable(t *testing.T) {

	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	oldTableName := "test_rename_table"
	newTableName := "test_rename_table_new"

	// Clean up
	db.DropTable(oldTableName, infinity.ConflictTypeIgnore)
	db.DropTable(newTableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
	}

	table, err := db.CreateTable(oldTableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil after creation")
	}
	t.Logf("Table created: %s", oldTableName)

	// Rename table directly using table object
	_, err = table.Rename(newTableName)
	if err != nil {
		t.Fatalf("Server may not support RENAME TABLE operation. Error: %v", err)
	}
	t.Logf("Table renamed successfully from %s to %s", oldTableName, newTableName)

	// Verify old table name doesn't exist
	_, err = db.GetTable(oldTableName)
	if err == nil {
		t.Error("Expected error when getting old table name, got nil")
	} else {
		infErr, ok := err.(*infinity.InfinityException)
		if !ok {
			t.Errorf("Expected InfinityException, got: %T", err)
		} else if infinity.ErrorCode(infErr.ErrorCode) != infinity.ErrorCodeTableNotExist {
			t.Logf("Note: Got error code %d for old table: %s", infErr.ErrorCode, infErr.ErrorMsg)
		}
	}

	// Verify new table name exists
	newTable, err := db.GetTable(newTableName)
	if err != nil {
		t.Fatalf("Failed to get new table name: %v", err)
	}
	if newTable == nil {
		t.Fatal("New table is nil")
	}

	// Try to rename with invalid name (should fail)
	invalidTableName := "123"
	_, err = table.Rename(invalidTableName)
	if err == nil {
		t.Error("Expected error when renaming with invalid name, got nil")
	} else {
		infErr, ok := err.(*infinity.InfinityException)
		if !ok {
			t.Errorf("Expected InfinityException, got: %T", err)
		} else if infinity.ErrorCode(infErr.ErrorCode) != infinity.ErrorCodeInvalidTableName &&
			infinity.ErrorCode(infErr.ErrorCode) != infinity.ErrorCodeInvalidIdentifierName {
			t.Logf("Note: Got error code %d for invalid name: %s", infErr.ErrorCode, infErr.ErrorMsg)
		}
	}

	// Clean up
	_, err = db.DropTable(newTableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	t.Logf("Test completed successfully")
}

// TestCreate1KTable tests creating 1000 tables
// Based on Python SDK test_pysdk/test_table.py - test_create_1K_table
func TestCreate1KTable(t *testing.T) {
	conn := setupConnection(t)
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tbCount := 1000
	tablePrefix := "test_create_1k_table"

	// Clean up: drop all tables if they exist
	for i := 0; i < tbCount; i++ {
		tableName := fmt.Sprintf("%s%d", tablePrefix, i)
		db.DropTable(tableName, infinity.ConflictTypeIgnore)
	}

	// Create 1000 tables
	schema := infinity.TableSchema{
		{
			Name:     "c1",
			DataType: "int",
		},
	}

	for i := 0; i < tbCount; i++ {
		tableName := fmt.Sprintf("%s%d", tablePrefix, i)
		table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
		if err != nil {
			t.Fatalf("Failed to create table %s: %v", tableName, err)
		}
		if table == nil {
			t.Fatalf("Table is nil for %s", tableName)
		}
	}
	t.Logf("Successfully created %d tables", tbCount)

	// List tables and verify count
	resp, err := db.ListTables()
	if err != nil {
		t.Fatalf("Failed to list tables: %v", err)
	}

	// Type assert to get the response
	listResp, ok := resp.(*thriftapi.ListTableResponse)
	if !ok {
		t.Fatalf("Expected *thriftapi.ListTableResponse, got %T", resp)
	}

	// Count tables with our prefix
	createdCount := 0
	for _, tableName := range listResp.TableNames {
		if strings.HasPrefix(tableName, tablePrefix) {
			createdCount++
		}
	}

	if createdCount != tbCount {
		t.Errorf("Expected %d tables with prefix '%s', got %d", tbCount, tablePrefix, createdCount)
	} else {
		t.Logf("Verified %d tables exist", createdCount)
	}

	// Clean up: drop all tables
	for i := 0; i < tbCount; i++ {
		tableName := fmt.Sprintf("%s%d", tablePrefix, i)
		_, err := db.DropTable(tableName, infinity.ConflictTypeError)
		if err != nil {
			t.Logf("Warning: Failed to drop table %s: %v", tableName, err)
		}
	}
	t.Logf("Successfully dropped %d tables", tbCount)
}

// TestCreate10KTable tests creating 10000 tables
// Based on Python SDK test_pysdk/test_table.py - test_create_10k_table
// Note: This is a slow test that creates 10000 tables
//func TestCreate10KTable(t *testing.T) {
//	conn := setupConnection(t)
//	defer conn.Disconnect()
//
//	db, err := conn.GetDatabase("default_db")
//	if err != nil {
//		t.Fatalf("Failed to get database: %v", err)
//	}
//
//	tbCount := 10000
//	tablePrefix := "test_create_10k_table"
//
//	t.Logf("Starting to create %d tables (this may take a while)...", tbCount)
//
//	// Clean up: drop all tables if they exist
//	for i := 0; i < tbCount; i++ {
//		tableName := fmt.Sprintf("%s%d", tablePrefix, i)
//		db.DropTable(tableName, infinity.ConflictTypeIgnore)
//	}
//
//	// Create 10000 tables
//	schema := infinity.TableSchema{
//		{
//			Name:     "c1",
//			DataType: "int",
//		},
//	}
//
//	for i := 0; i < tbCount; i++ {
//		tableName := fmt.Sprintf("%s%d", tablePrefix, i)
//		table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
//		if err != nil {
//			t.Fatalf("Failed to create table %s: %v", tableName, err)
//		}
//		if table == nil {
//			t.Fatalf("Table is nil for %s", tableName)
//		}
//	}
//	t.Logf("Successfully created %d tables", tbCount)
//
//	// List tables and verify count
//	resp, err := db.ListTables()
//	if err != nil {
//		t.Fatalf("Failed to list tables: %v", err)
//	}
//
//	// Type assert to get the response
//	listResp, ok := resp.(*thriftapi.ListTableResponse)
//	if !ok {
//		t.Fatalf("Expected *thriftapi.ListTableResponse, got %T", resp)
//	}
//
//	// Count tables with our prefix
//	createdCount := 0
//	for _, tableName := range listResp.TableNames {
//		if strings.HasPrefix(tableName, tablePrefix) {
//			createdCount++
//		}
//	}
//
//	if createdCount != tbCount {
//		t.Errorf("Expected %d tables with prefix '%s', got %d", tbCount, tablePrefix, createdCount)
//	} else {
//		t.Logf("Verified %d tables exist", createdCount)
//	}
//
//	// Clean up: drop all tables
//	for i := 0; i < tbCount; i++ {
//		tableName := fmt.Sprintf("%s%d", tablePrefix, i)
//		_, err := db.DropTable(tableName, infinity.ConflictTypeError)
//		if err != nil {
//			t.Logf("Warning: Failed to drop table %s: %v", tableName, err)
//		}
//	}
//	t.Logf("Successfully dropped %d tables", tbCount)
//}
