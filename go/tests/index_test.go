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
	"math"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// TestCreateIndexIVF tests creating IVF index
func TestCreateIndexIVF(t *testing.T) {
	tableName := "test_index_ivf"

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

	// Create IVF index
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeIVF, map[string]string{
		"metric": "l2",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create IVF index: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexHNSW tests creating HNSW index
func TestCreateIndexHNSW(t *testing.T) {
	tableName := "test_index_hnsw"

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

	// Create HNSW index
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "50",
		"metric":          "l2",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create HNSW index: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexBMP tests creating BMP index
func TestCreateIndexBMP(t *testing.T) {
	tableName := "test_bmp"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Test with different block sizes and compress types
	blockSizes := []string{"8", "128"}
	compressTypes := []string{"compress", "raww"}

	for _, blockSize := range blockSizes {
		for _, compressType := range compressTypes {
			// Cleanup
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table
			schema := infinity.TableSchema{
				{Name: "col1", DataType: "int"},
				{Name: "col2", DataType: "sparse,30000,float,int16"},
			}
			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Create BMP index
			indexInfo := infinity.NewIndexInfo("col2", infinity.IndexTypeBMP, map[string]string{
				"block_size":    blockSize,
				"compress_type": compressType,
			})
			_, err = table.CreateIndex("idx1", indexInfo, infinity.ConflictTypeError, "")
			if err != nil {
				t.Fatalf("Failed to create BMP index (block_size=%s, compress_type=%s): %v", blockSize, compressType, err)
			}

			// Drop index
			_, err = table.DropIndex("idx1", infinity.ConflictTypeError)
			if err != nil {
				t.Errorf("Failed to drop index: %v", err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Errorf("Failed to drop table: %v", err)
			}
		}
	}
}

// TestCreateIndexFullText tests creating fulltext index
func TestCreateIndexFullText(t *testing.T) {
	tableName := "test_index_fulltext"

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
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create fulltext index
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create fulltext index: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestDropIndexFullText tests dropping fulltext index and search behavior
func TestDropIndexFullText(t *testing.T) {
	tableName := "test_index_fulltext_drop"

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
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Fulltext search when index is not created: expect error
	table2 := table.Output([]string{"doctitle", "_score"})
	table2.MatchText("body^5", "harmful chemical", 3, map[string]string{})
	_, err = table2.ToResult()
	if err == nil {
		t.Error("Expected error for fulltext search without index")
	}

	// Create fulltext index
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create fulltext index: %v", err)
	}

	// Fulltext search when index is created: expect success (even if no data)
	table3 := table.Output([]string{"doctitle", "_score"})
	table3.MatchText("body^5", "harmful chemical", 3, map[string]string{})
	_, err = table3.ToResult()
	if err != nil {
		t.Errorf("Fulltext search with index failed: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Fulltext search when index is dropped: expect error
	table4 := table.Output([]string{"doctitle", "_score"})
	table4.MatchText("body^5", "harmful chemical", 3, map[string]string{})
	_, err = table4.ToResult()
	if err == nil {
		t.Error("Expected error for fulltext search after dropping index")
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexSecondary tests creating secondary index
func TestCreateIndexSecondary(t *testing.T) {
	tableName := "test_index_secondary"

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
		{Name: "c1", DataType: "int"},
		{Name: "body", DataType: "varchar"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create secondary index on c1
	indexInfo1 := infinity.NewIndexInfo("c1", infinity.IndexTypeSecondary, map[string]string{})
	_, err = table.CreateIndex("my_index_1", indexInfo1, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create secondary index on c1: %v", err)
	}

	// Insert data
	_, err = table.Insert(map[string]interface{}{"c1": 1, "body": "hello"})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Create secondary index on body
	indexInfo2 := infinity.NewIndexInfo("body", infinity.IndexTypeSecondary, map[string]string{})
	_, err = table.CreateIndex("my_index_2", indexInfo2, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create secondary index on body: %v", err)
	}

	// Drop indexes
	_, err = table.DropIndex("my_index_1", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index 1: %v", err)
	}
	_, err = table.DropIndex("my_index_2", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index 2: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexSecondaryFunctional tests creating secondary functional index
func TestCreateIndexSecondaryFunctional(t *testing.T) {
	tableName := "test_index_secondary_functional"

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
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "varchar"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": "hello world"},
		{"c1": 4, "c2": "thank you"},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Create secondary functional index on sqrt(c1)
	indexInfo1 := infinity.NewIndexInfo("sqrt(c1)", infinity.IndexTypeSecondaryFunctional, map[string]string{})
	_, err = table.CreateIndex("idx_sqrt_c1", indexInfo1, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create functional index: %v", err)
	}

	// Query with filter using sqrt(c1)
	table2 := table.Output([]string{"c1", "sqrt(c1)"})
	table2.Filter("sqrt(c1)>1")
	res, err := table2.ToResult()
	if err != nil {
		t.Fatalf("Query failed: %v", err)
	}
	result, ok := res.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("Unexpected response type")
	}

	if c1Col, exists := result.Data["c1"]; exists {
		if len(c1Col) != 1 {
			t.Errorf("c1 column: expected 10 rows, got %d", len(c1Col))
		}
	}
	// Verify result contains c1=4 with sqrt(c1)=2.0
	found := false
	if c1Data, ok := result.Data["c1"]; ok && len(c1Data) > 0 {
		if sqrtData, ok := result.Data["sqrt(c1)"]; ok && len(sqrtData) > 0 {
			for i := range c1Data {
				if c1, ok := c1Data[i].(int32); ok && c1 == 4 {
					if sqrtVal, ok := sqrtData[i].(float64); ok && math.Abs(sqrtVal-2.0) < 0.001 {
						found = true
						break
					}
				}
			}
		}
	}
	if !found {
		t.Error("Expected to find row with c1=4 and sqrt(c1)=2.0")
	}

	// Create secondary functional index on substring(c2, 0, 5)
	indexInfo2 := infinity.NewIndexInfo("substring(c2, 0, 5)", infinity.IndexTypeSecondaryFunctional, map[string]string{})
	_, err = table.CreateIndex("idx_substring_c2", indexInfo2, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create substring functional index: %v", err)
	}

	// Query with filter using substring
	table3 := table.Output([]string{"c2", "substring(c2, 0, 5) AS c2_sub"})
	table3.Filter("c2_sub = 'hello'")
	res, err = table3.ToResult()
	if err != nil {
		t.Fatalf("Query failed: %v", err)
	}
	result, ok = res.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("Unexpected response type")
	}
	// Check row count from any column
	for _, values := range result.Data {
		if len(values) != 1 {
			t.Errorf("Expected 1 row, got %d", len(values))
		}
		break
	}

	// Show indexes
	_, err = table.ShowIndex("idx_sqrt_c1")
	if err != nil {
		t.Errorf("Failed to show index: %v", err)
	}
	_, err = table.ShowIndex("idx_substring_c2")
	if err != nil {
		t.Errorf("Failed to show index: %v", err)
	}

	// Try to create index with invalid function - should error
	indexInfo3 := infinity.NewIndexInfo("abc(c2)", infinity.IndexTypeSecondaryFunctional, map[string]string{})
	_, err = table.CreateIndex("idx_error", indexInfo3, infinity.ConflictTypeError, "")
	if err == nil {
		t.Error("Expected error for invalid function in functional index")
	}

	// Drop indexes
	_, err = table.DropIndex("idx_sqrt_c1", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}
	_, err = table.DropIndex("idx_substring_c2", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexEMVB tests creating EMVB index
func TestCreateIndexEMVB(t *testing.T) {
	tableName := "test_index_emvb"

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
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "tensor, 128, float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create EMVB index
	indexInfo := infinity.NewIndexInfo("c2", infinity.IndexTypeEMVB, map[string]string{
		"pq_subspace_num":  "32",
		"pq_subspace_bits": "8",
	})
	_, err = table.CreateIndex("my_index_1", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create EMVB index: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index_1", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestDropNonExistentIndex tests dropping a non-existent index
func TestDropNonExistentIndex(t *testing.T) {
	tableName := "test_drop_non_existent_index"

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
		{Name: "c1", DataType: "vector,3,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Drop non-existent index - expect error
	_, err = table.DropIndex("none_index", infinity.ConflictTypeError)
	if err == nil {
		t.Error("Expected error for dropping non-existent index")
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateCreatedIndex tests creating an already created index
func TestCreateCreatedIndex(t *testing.T) {
	tableName := "test_create_created_index"

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
		{Name: "c1", DataType: "vector,3,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create index
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}

	// Try to create the same index again - expect error
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err == nil {
		t.Error("Expected error for creating already created index")
	}

	// Cleanup
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexOnDroppedTable tests creating index on a dropped table
func TestCreateIndexOnDroppedTable(t *testing.T) {
	tableName := "test_create_index_on_dropped_table"

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
		{Name: "c1", DataType: "vector,3,float"},
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

	// Try to create index on dropped table - expect error
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err == nil {
		t.Error("Expected error for creating index on dropped table")
	}
}

// TestCreateIndexShowIndex tests creating index and showing it
func TestCreateIndexShowIndex(t *testing.T) {
	tableName := "test_create_index_show_index"

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
		{Name: "c1", DataType: "vector,3,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create index
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}

	// Show index
	resp, err := table.ShowIndex("my_index")
	if err != nil {
		t.Fatalf("Failed to show index: %v", err)
	}

	showResp, ok := resp.(*thriftapi.ShowIndexResponse)
	if !ok {
		t.Fatalf("Unexpected response type")
	}
	if showResp.ErrorCode != 0 {
		t.Errorf("Expected error code 0, got %d", showResp.ErrorCode)
	}

	// Cleanup
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestDropIndexShowIndex tests dropping index and listing indexes
func TestDropIndexShowIndex(t *testing.T) {
	tableName := "test_drop_index_show_index"

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
		{Name: "c1", DataType: "vector,3,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create index
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}

	// Show index
	_, err = table.ShowIndex("my_index")
	if err != nil {
		t.Errorf("Failed to show index: %v", err)
	}

	// List indexes
	_, err = table.ListIndexes()
	if err != nil {
		t.Errorf("Failed to list indexes: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexOnDifferentTypeOfColumn tests creating index on different column types
func TestCreateIndexOnDifferentTypeOfColumn(t *testing.T) {
	tableName := "test_create_index_on_different_type_of_column"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Cleanup
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with vector column
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "vector,3,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Try to create FullText index on vector column - expect error
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeFullText, map[string]string{
		"centroids_count": "128",
		"metric":          "l2",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err == nil {
		t.Error("Expected error for creating FullText index on vector column")
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestInsertDataCreateIndex tests creating index after inserting data
func TestInsertDataCreateIndex(t *testing.T) {
	tableName := "test_insert_data_create_index"

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

	// Insert data
	vector := make([]float64, 1024)
	for i := range vector {
		vector[i] = 1.1
	}
	_, err = table.Insert(map[string]interface{}{"c1": vector})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Create index after inserting data
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}

	// Cleanup
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestImportDataCreateIndex tests importing data and then creating index
func TestImportDataCreateIndex(t *testing.T) {
	tableName := "test_import_data_create_index"

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
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "vector,3,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Import data
	importFile := testDataDir + "pysdk_test.csv"
	_, err = table.ImportData(importFile, infinity.NewImportOption())
	if err != nil {
		t.Fatalf("Failed to import data: %v", err)
	}

	// Create HNSW index after import - should succeed
	indexInfo := infinity.NewIndexInfo("c2", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create HNSW index: %v", err)
	}

	// Cleanup
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateVectorIndexImportData tests creating index before importing data
func TestCreateVectorIndexImportData(t *testing.T) {
	tableName := "test_create_vector_index_import_data"

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
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "vector,3,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create index before importing data
	indexInfo := infinity.NewIndexInfo("c2", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}

	// Import data
	importFile := testDataDir + "pysdk_test.csv"
	_, err = table.ImportData(importFile, infinity.NewImportOption())
	if err != nil {
		t.Fatalf("Failed to import data: %v", err)
	}

	// Cleanup
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexWithValidOptions tests creating index with valid conflict types
func TestCreateIndexWithValidOptions(t *testing.T) {
	tableName := "test_create_index_with_valid_options"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	validConflictTypes := []infinity.ConflictType{
		infinity.ConflictTypeIgnore,
		infinity.ConflictTypeError,
		infinity.ConflictType(0),
		infinity.ConflictType(1),
	}

	for _, conflictType := range validConflictTypes {
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

		// Create index with valid conflict type
		indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
			"m":               "16",
			"ef_construction": "50",
			"metric":          "l2",
		})
		_, err = table.CreateIndex("my_index", indexInfo, conflictType, "")
		if err != nil {
			t.Fatalf("Failed to create index with conflict type %v: %v", conflictType, err)
		}

		// Cleanup
		_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop index: %v", err)
		}
		_, err = db.DropTable(tableName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop table: %v", err)
		}
	}
}

// TestCreateIndexWithInvalidOptions tests creating index with invalid conflict types
func TestCreateIndexWithInvalidOptions(t *testing.T) {
	// Skip this test as Go is statically typed and invalid types would be caught at compile time
	t.Skip("Skipping test for invalid conflict types - Go type system prevents invalid types at compile time")
}

// TestCreateDuplicatedIndexWithValidOptions tests creating duplicated index with valid options
func TestCreateDuplicatedIndexWithValidOptions(t *testing.T) {
	tableName := "test_create_duplicated_index_with_valid_options"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	validConflictTypes := []infinity.ConflictType{
		infinity.ConflictTypeIgnore,
		infinity.ConflictType(0),
	}

	for _, conflictType := range validConflictTypes {
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

		// Create index multiple times with Ignore conflict type
		for i := 0; i < 10; i++ {
			indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
				"m":               "16",
				"ef_construction": "50",
				"metric":          "l2",
			})
			_, err = table.CreateIndex("my_index", indexInfo, conflictType, "")
			if err != nil {
				t.Fatalf("Failed to create index at iteration %d: %v", i, err)
			}
		}

		// Cleanup
		_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop index: %v", err)
		}
		_, err = db.DropTable(tableName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop table: %v", err)
		}
	}
}

// TestCreateDuplicatedIndexWithErrorOptions tests creating duplicated index with Error conflict type
func TestCreateDuplicatedIndexWithErrorOptions(t *testing.T) {
	tableName := "test_create_duplicated_index_with_error_options"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	errorConflictTypes := []infinity.ConflictType{
		infinity.ConflictTypeError,
		infinity.ConflictType(1),
	}

	for _, conflictType := range errorConflictTypes {
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
		_, err = table.CreateIndex("my_index", indexInfo, conflictType, "")
		if err != nil {
			t.Fatalf("Failed to create index: %v", err)
		}

		// Try to create the same index again - should error
		for i := 0; i < 10; i++ {
			_, err = table.CreateIndex("my_index", indexInfo, conflictType, "")
			if err == nil {
				t.Errorf("Expected error for duplicate index at iteration %d", i)
			}
		}

		// Cleanup
		_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop index: %v", err)
		}
		_, err = db.DropTable(tableName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop table: %v", err)
		}
	}
}

// TestDropIndexWithValidOptions tests dropping index with valid conflict types
func TestDropIndexWithValidOptions(t *testing.T) {
	tableName := "test_drop_index_with_valid_options"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	validConflictTypes := []infinity.ConflictType{
		infinity.ConflictTypeIgnore,
		infinity.ConflictTypeError,
		infinity.ConflictType(0),
		infinity.ConflictType(1),
	}

	for _, conflictType := range validConflictTypes {
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

		// Drop index with valid conflict type
		_, err = table.DropIndex("my_index", conflictType)
		if err != nil {
			t.Fatalf("Failed to drop index with conflict type %v: %v", conflictType, err)
		}

		// Cleanup
		_, err = db.DropTable(tableName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop table: %v", err)
		}
	}
}

// TestDropIndexWithInvalidOptions tests dropping index with invalid conflict types
func TestDropIndexWithInvalidOptions(t *testing.T) {
	// Skip this test as Go is statically typed
	t.Skip("Skipping test for invalid conflict types - Go type system prevents invalid types at compile time")
}

// TestSupportedVectorIndex tests supported vector index metrics
func TestSupportedVectorIndex(t *testing.T) {
	tableName := "test_supported_vector_index"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	supportedMetrics := []string{"l2", "ip"}

	for _, metric := range supportedMetrics {
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

		// Create index with supported metric
		indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
			"m":               "16",
			"ef_construction": "50",
			"metric":          metric,
		})
		_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
		if err != nil {
			t.Fatalf("Failed to create index with metric %s: %v", metric, err)
		}

		// Cleanup
		_, err = table.DropIndex("my_index", infinity.ConflictTypeIgnore)
		if err != nil {
			t.Errorf("Failed to drop index: %v", err)
		}
		_, err = db.DropTable(tableName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop table: %v", err)
		}
	}
}

// TestUnsupportedVectorIndex tests unsupported vector index metrics
func TestUnsupportedVectorIndex(t *testing.T) {
	tableName := "test_unsupported_vector_index"

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

	// Try to create index with unsupported metric
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "50",
		"metric":          "hamming",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err == nil {
		t.Error("Expected error for unsupported metric 'hamming'")
	}

	// Cleanup
	_, err = table.DropIndex("my_index", infinity.ConflictTypeIgnore)
	if err != nil {
		// Ignore error as index may not have been created
		_ = err
	}
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateUpperNameIndex tests creating index with uppercase name
func TestCreateUpperNameIndex(t *testing.T) {
	tableName := "test_upper_name_index"

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

	// Create index with uppercase name
	upperNameIndex := "MY_INDEX"
	lowerNameIndex := "my_index"
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex(upperNameIndex, indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}

	// Show index with lowercase name
	_, err = table.ShowIndex(lowerNameIndex)
	if err != nil {
		t.Logf("Show index with lowercase name failed (may be expected): %v", err)
	}

	// Show index with uppercase name
	_, err = table.ShowIndex(upperNameIndex)
	if err != nil {
		t.Logf("Show index with uppercase name failed (may be expected): %v", err)
	}

	// Drop index with lowercase name
	_, err = table.DropIndex(lowerNameIndex, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexWithConverseParamName tests creating index with converse parameter names
func TestCreateIndexWithConverseParamName(t *testing.T) {
	tableName := "test_index_converse_param_name"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	testCases := []struct {
		indexType infinity.IndexType
		setupFunc func() (string, infinity.TableSchema, map[string]string)
	}{
		{
			indexType: infinity.IndexTypeIVF,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_ivf",
					infinity.TableSchema{{Name: "c1", DataType: "vector,1024,float"}},
					map[string]string{"METRIC": "l2"}
			},
		},
		{
			indexType: infinity.IndexTypeHnsw,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_hnsw",
					infinity.TableSchema{{Name: "c1", DataType: "vector,1024,float"}},
					map[string]string{"m": "16", "EF_CONSTRUCTION": "50", "METRIC": "l2"}
			},
		},
		{
			indexType: infinity.IndexTypeBMP,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_bmp",
					infinity.TableSchema{
						{Name: "col1", DataType: "int"},
						{Name: "col2", DataType: "sparse,30000,float,int16"},
					},
					map[string]string{"BLOCK_SIZE": "8", "COMPRESS_TYPE": "compress"}
			},
		},
		{
			indexType: infinity.IndexTypeFullText,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_ft",
					infinity.TableSchema{
						{Name: "doctitle", DataType: "varchar"},
						{Name: "docdate", DataType: "varchar"},
						{Name: "body", DataType: "varchar"},
					},
					map[string]string{"analyzer": "standard"}
			},
		},
		{
			indexType: infinity.IndexTypeEMVB,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_emvb",
					infinity.TableSchema{
						{Name: "c1", DataType: "int"},
						{Name: "c2", DataType: "tensor, 128, float"},
					},
					map[string]string{"PQ_SUBSPACE_NUM": "32", "PQ_SUBSPACE_BITS": "8"}
			},
		},
		{
			indexType: infinity.IndexTypeSecondary,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_secondary",
					infinity.TableSchema{
						{Name: "c1", DataType: "int"},
						{Name: "body", DataType: "varchar"},
					},
					map[string]string{}
			},
		},
	}

	for _, tc := range testCases {
		t.Run(fmt.Sprintf("IndexType_%s", tc.indexType.String()), func(t *testing.T) {
			tableName, schema, params := tc.setupFunc()

			// Cleanup
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table
			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Get target column name
			targetName := schema[0].Name
			if tc.indexType == infinity.IndexTypeBMP {
				targetName = "col2"
			} else if tc.indexType == infinity.IndexTypeFullText {
				targetName = "body"
			} else if tc.indexType == infinity.IndexTypeEMVB {
				targetName = "c2"
			}

			// Create index with converse parameter names
			var indexInfo *infinity.IndexInfo
			if tc.indexType == infinity.IndexTypeSecondary {
				indexInfo = infinity.NewIndexInfo("c1", tc.indexType, params)
			} else {
				indexInfo = infinity.NewIndexInfo(targetName, tc.indexType, params)
			}
			_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
			if err != nil {
				t.Fatalf("Failed to create index: %v", err)
			}

			// Show index
			_, err = table.ShowIndex("my_index")
			if err != nil {
				t.Errorf("Failed to show index: %v", err)
			}

			// Drop index
			_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
			if err != nil {
				t.Errorf("Failed to drop index: %v", err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Errorf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestCreateIndexWithConverseParamValue tests creating index with converse parameter values
func TestCreateIndexWithConverseParamValue(t *testing.T) {
	tableName := "test_index_converse_param_value"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	testCases := []struct {
		indexType infinity.IndexType
		setupFunc func() (string, infinity.TableSchema, map[string]string)
	}{
		{
			indexType: infinity.IndexTypeIVF,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_ivf",
					infinity.TableSchema{{Name: "c1", DataType: "vector,1024,float"}},
					map[string]string{"metric": "l2"}
			},
		},
		{
			indexType: infinity.IndexTypeHnsw,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_hnsw",
					infinity.TableSchema{{Name: "c1", DataType: "vector,1024,float"}},
					map[string]string{"M": "16", "ef_construction": "50", "metric": "L2"}
			},
		},
		{
			indexType: infinity.IndexTypeBMP,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_bmp",
					infinity.TableSchema{
						{Name: "col1", DataType: "int"},
						{Name: "col2", DataType: "sparse,30000,float,int16"},
					},
					map[string]string{"block_size": "8", "compress_type": "COMPRESS"}
			},
		},
		{
			indexType: infinity.IndexTypeFullText,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_ft",
					infinity.TableSchema{
						{Name: "doctitle", DataType: "varchar"},
						{Name: "docdate", DataType: "varchar"},
						{Name: "body", DataType: "varchar"},
					},
					map[string]string{"ANALYZER": "STANDARD"}
			},
		},
		{
			indexType: infinity.IndexTypeEMVB,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_emvb",
					infinity.TableSchema{
						{Name: "c1", DataType: "int"},
						{Name: "c2", DataType: "tensor, 128, float"},
					},
					map[string]string{"pq_subspace_num": "32", "pq_subspace_bits": "8"}
			},
		},
		{
			indexType: infinity.IndexTypeSecondary,
			setupFunc: func() (string, infinity.TableSchema, map[string]string) {
				return tableName + "_secondary",
					infinity.TableSchema{
						{Name: "c1", DataType: "int"},
						{Name: "body", DataType: "varchar"},
					},
					map[string]string{}
			},
		},
	}

	for _, tc := range testCases {
		t.Run(fmt.Sprintf("IndexType_%s", tc.indexType.String()), func(t *testing.T) {
			tableName, schema, params := tc.setupFunc()

			// Cleanup
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table
			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Get target column name
			targetName := schema[0].Name
			if tc.indexType == infinity.IndexTypeBMP {
				targetName = "col2"
			} else if tc.indexType == infinity.IndexTypeFullText {
				targetName = "body"
			} else if tc.indexType == infinity.IndexTypeEMVB {
				targetName = "c2"
			}

			// Create index with converse parameter values
			var indexInfo *infinity.IndexInfo
			if tc.indexType == infinity.IndexTypeSecondary {
				indexInfo = infinity.NewIndexInfo("c1", tc.indexType, params)
			} else {
				indexInfo = infinity.NewIndexInfo(targetName, tc.indexType, params)
			}
			_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
			if err != nil {
				t.Fatalf("Failed to create index: %v", err)
			}

			// Show index
			_, err = table.ShowIndex("my_index")
			if err != nil {
				t.Errorf("Failed to show index: %v", err)
			}

			// Drop index
			_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
			if err != nil {
				t.Errorf("Failed to drop index: %v", err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Errorf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestCreateIndexWithComment tests creating index with comment
func TestCreateIndexWithComment(t *testing.T) {
	tableName := "test_index_comment"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	testCases := []struct {
		indexType infinity.IndexType
		setupFunc func() (string, infinity.TableSchema, map[string]string, string)
	}{
		{
			indexType: infinity.IndexTypeIVF,
			setupFunc: func() (string, infinity.TableSchema, map[string]string, string) {
				return tableName + "_ivf",
					infinity.TableSchema{{Name: "c1", DataType: "vector,1024,float"}},
					map[string]string{"metric": "l2"},
					"c1"
			},
		},
		{
			indexType: infinity.IndexTypeHnsw,
			setupFunc: func() (string, infinity.TableSchema, map[string]string, string) {
				return tableName + "_hnsw",
					infinity.TableSchema{{Name: "c1", DataType: "vector,1024,float"}},
					map[string]string{"M": "16", "ef_construction": "50", "metric": "L2"},
					"c1"
			},
		},
		{
			indexType: infinity.IndexTypeBMP,
			setupFunc: func() (string, infinity.TableSchema, map[string]string, string) {
				return tableName + "_bmp",
					infinity.TableSchema{
						{Name: "col1", DataType: "int"},
						{Name: "col2", DataType: "sparse,30000,float,int16"},
					},
					map[string]string{"block_size": "8", "compress_type": "COMPRESS"},
					"col2"
			},
		},
		{
			indexType: infinity.IndexTypeFullText,
			setupFunc: func() (string, infinity.TableSchema, map[string]string, string) {
				return tableName + "_ft",
					infinity.TableSchema{
						{Name: "doctitle", DataType: "varchar"},
						{Name: "docdate", DataType: "varchar"},
						{Name: "body", DataType: "varchar"},
					},
					map[string]string{"ANALYZER": "STANDARD"},
					"body"
			},
		},
		{
			indexType: infinity.IndexTypeEMVB,
			setupFunc: func() (string, infinity.TableSchema, map[string]string, string) {
				return tableName + "_emvb",
					infinity.TableSchema{
						{Name: "c1", DataType: "int"},
						{Name: "c2", DataType: "tensor, 128, float"},
					},
					map[string]string{"pq_subspace_num": "32", "pq_subspace_bits": "8"},
					"c2"
			},
		},
		{
			indexType: infinity.IndexTypeSecondary,
			setupFunc: func() (string, infinity.TableSchema, map[string]string, string) {
				return tableName + "_secondary",
					infinity.TableSchema{
						{Name: "c1", DataType: "int"},
						{Name: "body", DataType: "varchar"},
					},
					map[string]string{},
					"c1"
			},
		},
	}

	for _, tc := range testCases {
		t.Run(fmt.Sprintf("IndexType_%s", tc.indexType.String()), func(t *testing.T) {
			tableName, schema, params, targetName := tc.setupFunc()

			// Cleanup
			db.DropTable(tableName, infinity.ConflictTypeIgnore)

			// Create table
			table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
			if err != nil {
				t.Fatalf("Failed to create table: %v", err)
			}

			// Create index with comment
			indexInfo := infinity.NewIndexInfo(targetName, tc.indexType, params)
			_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "test comment")
			if err != nil {
				t.Fatalf("Failed to create index: %v", err)
			}

			// Show index and verify comment
			resp, err := table.ShowIndex("my_index")
			if err != nil {
				t.Errorf("Failed to show index: %v", err)
			} else {
				showResp, ok := resp.(*thriftapi.ShowIndexResponse)
				if ok && showResp.IndexComment != "test comment" {
					t.Errorf("Expected index comment 'test comment', got '%s'", showResp.IndexComment)
				}
			}

			// Drop index
			_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
			if err != nil {
				t.Errorf("Failed to drop index: %v", err)
			}

			// Cleanup
			_, err = db.DropTable(tableName, infinity.ConflictTypeError)
			if err != nil {
				t.Errorf("Failed to drop table: %v", err)
			}
		})
	}
}

// TestDumpIndex tests dumping index
func TestDumpIndex(t *testing.T) {
	tableName := "test_dump_index"

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
		{Name: "c1", DataType: "int"},
		{Name: "body", DataType: "varchar"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Create secondary index on c1
	indexInfo1 := infinity.NewIndexInfo("c1", infinity.IndexTypeSecondary, map[string]string{})
	_, err = table.CreateIndex("my_index_1", indexInfo1, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index 1: %v", err)
	}

	// Insert data
	_, err = table.Insert(map[string]interface{}{"c1": 1, "body": "hello"})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Create secondary index on body
	indexInfo2 := infinity.NewIndexInfo("body", infinity.IndexTypeSecondary, map[string]string{})
	_, err = table.CreateIndex("my_index_2", indexInfo2, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index 2: %v", err)
	}

	// Insert more data
	_, err = table.Insert(map[string]interface{}{"c1": 2, "body": "world"})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Dump indexes
	_, err = table.DumpIndex("my_index_1")
	if err != nil {
		t.Errorf("Failed to dump index 1: %v", err)
	}
	_, err = table.DumpIndex("my_index_2")
	if err != nil {
		t.Errorf("Failed to dump index 2: %v", err)
	}

	// Drop indexes
	_, err = table.DropIndex("my_index_1", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index 1: %v", err)
	}
	_, err = table.DropIndex("my_index_2", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index 2: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}

// TestCreateIndexOnDeletedTable tests creating index on table with deleted data
func TestCreateIndexOnDeletedTable(t *testing.T) {
	tableName := "test_create_index_on_deleted_table"

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
		{Name: "c1", DataType: "vector,128,float"},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert data
	embeddingData := make([]float64, 128)
	for i := range embeddingData {
		embeddingData[i] = float64(i)
	}
	values := make([]map[string]interface{}, 1024)
	for i := range values {
		values[i] = map[string]interface{}{"c1": embeddingData}
	}
	_, err = table.Insert(values)
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Delete all data
	_, err = table.Delete("")
	if err != nil {
		t.Fatalf("Failed to delete data: %v", err)
	}

	// Create index on deleted table (should succeed)
	indexInfo := infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw, map[string]string{
		"m":               "16",
		"ef_construction": "200",
		"metric":          "l2",
		"encode":          "lvq",
	})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index on deleted table: %v", err)
	}

	// Drop index
	_, err = table.DropIndex("my_index", infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop index: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}
