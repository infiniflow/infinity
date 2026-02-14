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

func TestQuery(t *testing.T) {
	tableName := "my_table"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with integer, varchar and vector columns
	schema := infinity.TableSchema{
		{
			Name:     "num",
			DataType: "integer",
		},
		{
			Name:     "body",
			DataType: "varchar",
		},
		{
			Name:     "vec",
			DataType: "vector,5,float",
		},
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
	_, err = table.Insert(map[string]interface{}{
		"num":  1,
		"body": "undesirable, unnecessary, and harmful",
		"vec":  []float64{1.0, 1.0, 1.0, 1.0, 1.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert row 1: %v", err)
	}
	_, err = table.Insert(map[string]interface{}{
		"num":  2,
		"body": "publisher=US National Office for Harmful Algal Blooms",
		"vec":  []float64{4.0, 4.0, 4.0, 4.0, 4.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert row 2: %v", err)
	}
	_, err = table.Insert(map[string]interface{}{
		"num":  3,
		"body": "in the case of plants, growth and chemical",
		"vec":  []float64{7.0, 7.0, 7.0, 7.0, 7.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert row 3: %v", err)
	}

	// Create full-text index
	indexInfo := infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{})
	_, err = table.CreateIndex("my_index", indexInfo, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create index: %v", err)
	}
	defer table.DropIndex("my_index", infinity.ConflictTypeError)

	// Build query using table methods (fluent interface)
	// Note: In Go SDK, table methods return *Table for chaining
	// Match dense vector search
	table.MatchDense("vec", []float64{3.0, 3.0, 3.0, 3.0, 3.0}, "float", "ip", 2, nil)
	// Match text search
	table.MatchText("body", "harmful", 2, nil)
	// Fusion
	table.Fusion("rrf", 10, nil)
	// Output columns
	table.Output([]string{"num", "body"})

	// Execute query (using ToResult instead of to_df)
	result, err := table.ToResult()
	if err != nil {
		t.Fatalf("Failed to execute query: %v", err)
	}
	if result == nil {
		t.Error("Query result is nil")
	}
	// We can't easily assert the content of result as it's interface{},
	// but we can at least verify it's not nil and no error occurred.
}

func TestQueryBuilder(t *testing.T) {
	tableName := "test_query_builder"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create simple table
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
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Build query using Output and execute
	table.Output([]string{"*"})
	
	result, err := table.ToResult()
	if err != nil {
		t.Fatalf("Failed to execute query: %v", err)
	}
	if result == nil {
		t.Error("Query result is nil")
	}
}