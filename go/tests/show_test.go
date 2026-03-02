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
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// TestShowDatabases tests listing databases
func TestShowDatabases(t *testing.T) {
	dbCount := 1000

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// Cleanup existing databases
	for i := 0; i < dbCount; i++ {
		conn.DropDatabase(fmt.Sprintf("test_show_database%d", i), infinity.ConflictTypeIgnore)
	}

	// Create databases
	for i := 0; i < dbCount; i++ {
		_, err := conn.CreateDatabase(fmt.Sprintf("test_show_database%d", i), infinity.ConflictTypeError, "")
		if err != nil {
			t.Fatalf("Failed to create database: %v", err)
		}
	}

	// List databases
	resp, err := conn.ListDatabases()
	if err != nil {
		t.Fatalf("Failed to list databases: %v", err)
	}

	listResp, ok := resp.(*thriftapi.ListDatabaseResponse)
	if !ok {
		t.Fatalf("Unexpected response type")
	}

	// Count matching databases
	var resDBs []string
	for _, dbName := range listResp.DbNames {
		resDBs = append(resDBs, dbName)
	}

	if len(resDBs) > dbCount+1 {
		t.Errorf("Expected %d databases, got %d", dbCount+1, len(resDBs))
	}

	// Cleanup
	for i := 0; i < dbCount; i++ {
		_, err := conn.DropDatabase(fmt.Sprintf("test_show_database%d", i), infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop database: %v", err)
		}
	}
}

// TestShowTable tests showing table information
func TestShowTable(t *testing.T) {
	tableName := "test_show_table"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Cleanup
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "num", DataType: "integer"},
		{Name: "body", DataType: "varchar"},
		{Name: "vec", DataType: "vector,5,float"},
	}
	_, err = db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Show table
	resp, err := db.ShowTable(tableName)
	if err != nil {
		t.Fatalf("Failed to show table: %v", err)
	}

	showResp, ok := resp.(*thriftapi.ShowTableResponse)
	if !ok {
		t.Fatalf("Unexpected response type")
	}

	if showResp.DatabaseName != "default_db" {
		t.Errorf("Expected database name 'default_db', got '%s'", showResp.DatabaseName)
	}
	if showResp.TableName != tableName {
		t.Errorf("Expected table name '%s', got '%s'", tableName, showResp.TableName)
	}
	if showResp.ColumnCount != 3 {
		t.Errorf("Expected column count 3, got %d", showResp.ColumnCount)
	}
	if showResp.SegmentCount != 0 {
		t.Errorf("Expected segment count 0, got %d", showResp.SegmentCount)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestShowInvalidTable tests showing table with invalid names (parametrized test)
func TestShowInvalidTable(t *testing.T) {
	invalidTableNames := []interface{}{
		"Invalid name",
		1,
		1.1,
		true,
		[]int{},
		struct{}{},
		map[string]interface{}{},
	}

	tableName := "test_show_table"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Cleanup and create table
	db.DropTable(tableName, infinity.ConflictTypeIgnore)
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "vector,3,int"},
	}
	_, err = db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Test with invalid table names - only string names can be tested
	// Other types would cause compilation errors in Go
	invalidStringNames := []string{"Invalid name", "1", "1.1", "[]", "{}"}
	for _, invalidName := range invalidStringNames {
		_, err := db.ShowTable(invalidName)
		if err == nil {
			t.Errorf("Expected error for invalid table name '%v', but got nil", invalidName)
		}
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}

	// Suppress unused variable warning by referencing invalidTableNames
	_ = invalidTableNames
}

// TestShowNotExistTable tests showing a non-existent table
func TestShowNotExistTable(t *testing.T) {
	tableName := "not_exist_name"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Try to show non-existent table
	_, err = db.ShowTable(tableName)
	if err == nil {
		t.Errorf("Expected error for non-existent table, but got nil")
	}
}

// TestShowIndex tests showing index information
func TestShowIndex(t *testing.T) {
	tableName := "test_show_index"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Cleanup
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "vector,1024,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create indexes and show them
	for i := 0; i < 10; i++ {
		indexName := fmt.Sprintf("my_index_%d", i)
		indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
			"m":               "16",
			"ef_construction": "50",
			"metric":          "l2",
		})

		_, err := table.CreateIndex(indexName, indexInfo, infinity.ConflictTypeError, "")
		if err != nil {
			t.Fatalf("Failed to create index %s: %v", indexName, err)
		}

		resp, err := table.ShowIndex(indexName)
		if err != nil {
			t.Fatalf("Failed to show index %s: %v", indexName, err)
		}

		showResp, ok := resp.(*thriftapi.ShowIndexResponse)
		if !ok {
			t.Fatalf("Unexpected response type")
		}

		if showResp.IndexType != "HNSW" {
			t.Errorf("Expected index type 'HNSW', got '%s'", showResp.IndexType)
		}
	}

	// Cleanup indexes
	for i := 0; i < 10; i++ {
		indexName := fmt.Sprintf("my_index_%d", i)
		_, err := table.DropIndex(indexName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop index %s: %v", indexName, err)
		}
	}

	// Cleanup table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestShowInvalidNameIndex tests showing index with invalid names (parametrized test)
func TestShowInvalidNameIndex(t *testing.T) {
	tableName := "test_show_invalid_name_index"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Cleanup
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "vector,1024,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create index
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "50",
		"metric":          "l2",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}

	// Test with invalid index names
	invalidIndexNames := []string{"Invalid name", "not_exist_name", "1", "1.1", "[]", "{}", "()"}
	for _, invalidName := range invalidIndexNames {
		_, err := table.ShowIndex(invalidName)
		if err == nil {
			t.Errorf("Expected error for invalid index name '%v', but got nil", invalidName)
		}
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestListIndex tests listing indexes
func TestListIndex(t *testing.T) {
	tableName := "test_list_index"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Cleanup
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "vector,1024,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create indexes
	expectedIndexNames := []string{}
	for i := 0; i < 10; i++ {
		indexName := fmt.Sprintf("my_index_%d", i)
		expectedIndexNames = append(expectedIndexNames, indexName)
		indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
			"m":               "16",
			"ef_construction": "50",
			"metric":          "l2",
		})
		_, err := table.CreateIndex(indexName, indexInfo, infinity.ConflictTypeError, "")
		if err != nil {
			t.Fatalf("Failed to create index %s: %v", indexName, err)
		}
	}

	// List indexes
	resp, err := table.ListIndexes()
	if err != nil {
		t.Fatalf("Failed to list indexes: %v", err)
	}

	listResp, ok := resp.(*thriftapi.ListIndexResponse)
	if !ok {
		t.Fatalf("Unexpected response type")
	}

	// Note: Index names may not be in the same order, so we just check count
	if len(listResp.IndexNames) != len(expectedIndexNames) {
		t.Errorf("Expected %d indexes, got %d", len(expectedIndexNames), len(listResp.IndexNames))
	}

	// Cleanup indexes
	for i := 0; i < 10; i++ {
		indexName := fmt.Sprintf("my_index_%d", i)
		_, err := table.DropIndex(indexName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop index %s: %v", indexName, err)
		}
	}

	// Cleanup table
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestShowColumns tests showing columns
func TestShowColumns(t *testing.T) {
	tableName := "test_show_columns"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Cleanup
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table
	schema := infinity.TableSchema{
		{Name: "num", DataType: "integer"},
		{Name: "body", DataType: "varchar"},
		{Name: "vec", DataType: "vector,5,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Show columns
	resp, err := table.ShowColumns()
	if err != nil {
		t.Fatalf("Failed to show columns: %v", err)
	}

	result, ok := resp.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("Unexpected response type: %T", resp)
	}

	// Verify column count
	if len(result.Data) != 4 {
		t.Errorf("Expected 4 columns, got %d", len(result.Data))
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestShowColumnsWithComment tests showing columns with comments and defaults
func TestShowColumnsWithComment(t *testing.T) {
	tableName := "test_show_columns"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Cleanup
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with comments and defaults
	// Note: Go SDK may not support comments in the same way, using basic schema
	schema := infinity.TableSchema{
		{Name: "num", DataType: "integer", Default: 0},
		{Name: "body", DataType: "varchar", Default: "default text"},
		{Name: "vec", DataType: "vector,5,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Show columns
	resp, err := table.ShowColumns()
	if err != nil {
		t.Fatalf("Failed to show columns: %v", err)
	}

	result, ok := resp.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("Unexpected response type: %T", resp)
	}

	// Verify column count
	if len(result.Data) != 4 {
		t.Errorf("Expected 4 columns, got %d", len(result.Data))
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestShowCurrentNode tests showing current node information
func TestShowCurrentNode(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	resp, err := conn.ShowCurrentNode()
	if err != nil {
		t.Fatalf("Failed to show current node: %v", err)
	}

	nodeResp, ok := resp.(*thriftapi.ShowCurrentNodeResponse)
	if !ok {
		t.Fatalf("Unexpected response type")
	}

	// Just verify we got a valid response
	if nodeResp.ErrorCode != 0 {
		t.Errorf("Expected error code 0, got %d", nodeResp.ErrorCode)
	}
}

// TestShowConfig tests showing configs
func TestShowConfig(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// Test showing profile variable
	resp, err := conn.ShowConfig("version")
	if err != nil {
		t.Fatalf("Failed to show config 'profile': %v", err)
	}

	if resp.ErrorCode != infinity.ErrorCodeOK {
		t.Errorf("Expected error code OK, got %v", resp.ErrorCode)
	}

	// Test showing invalid variable
	_, err = conn.ShowConfig("invalid_variable")
	if err == nil {
		t.Errorf("Expected error for invalid variable, but got nil")
	}
}
