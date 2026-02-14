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

// TestUnnest tests unnest functionality with array columns
func TestUnnest(t *testing.T) {
	tableName := "test_unnest"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with array column
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "array,int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert data with arrays using infinity.Array
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": infinity.NewArray(0, 1)},
		{"c1": 2, "c2": infinity.NewArray(2, 3)},
		{"c1": 3, "c2": infinity.NewArray(0, 1, 2)},
		{"c1": 4, "c2": infinity.NewArray(0, 2, 3)},
		{"c1": 5, "c2": infinity.NewArray()},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test unnest(c2) - output unnested array values
	res, err := table.Output([]string{"unnest(c2)"}).ToResult()
	if err != nil {
		t.Errorf("Failed to query unnest(c2): %v", err)
	}
	if res == nil {
		t.Error("Query result is nil")
	}

	// Test c1, unnest(c2) - output column with unnested array
	res, err = table.Output([]string{"c1", "unnest(c2)"}).ToResult()
	if err != nil {
		t.Errorf("Failed to query c1, unnest(c2): %v", err)
	}
	if res == nil {
		t.Error("Query result is nil")
	}

	// Test c1, c2, unnest(c2) - output multiple columns with unnest
	res, err = table.Output([]string{"c1", "c2", "unnest(c2)"}).ToResult()
	if err != nil {
		t.Errorf("Failed to query c1, c2, unnest(c2): %v", err)
	}
	if res == nil {
		t.Error("Query result is nil")
	}

	// Test c1, unnest(c2) as uc2 with filter c1 > 2
	table2 := table.Output([]string{"c1", "unnest(c2) as uc2"})
	table2.Filter("c1 > 2")
	res, err = table2.ToResult()
	if err != nil {
		t.Errorf("Failed to query with filter c1 > 2: %v", err)
	}
	if res == nil {
		t.Error("Query result is nil")
	}

	// Test c1, unnest(c2) as uc2 with filter uc2 > 1
	table3 := table.Output([]string{"c1", "unnest(c2) as uc2"})
	table3.Filter("uc2 > 1")
	res, err = table3.ToResult()
	if err != nil {
		t.Errorf("Failed to query with filter uc2 > 1: %v", err)
	}
	if res == nil {
		t.Error("Query result is nil")
	}

	// Test unnest(c2) as uc2 with group by and sum
	table4 := table.Output([]string{"unnest(c2) as uc2", "sum(c1)"})
	table4.GroupBy([]string{"uc2"})
	res, err = table4.ToResult()
	if err != nil {
		t.Errorf("Failed to query with group by: %v", err)
	}
	if res == nil {
		t.Error("Query result is nil")
	}
}
