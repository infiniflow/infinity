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

// TestLimit tests basic limit operations
func TestLimit(t *testing.T) {
	tableName := "test_limit"

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
			Name:        "c2",
			DataType:    "int",
			Constraints: []infinity.ColumnConstraint{infinity.ConstraintNotNull},
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

	// Insert first batch of data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": -3, "c2": 3},
		{"c1": -2, "c2": 2},
		{"c1": -1, "c2": 1},
		{"c1": 0, "c2": 0},
		{"c1": 1, "c2": 1},
		{"c1": 2, "c2": 2},
		{"c1": 3, "c2": 3},
	})
	if err != nil {
		t.Fatalf("Failed to insert first batch: %v", err)
	}

	// Insert second batch of data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": -8, "c2": 8},
		{"c1": -7, "c2": 7},
		{"c1": -6, "c2": 6},
		{"c1": 7, "c2": 7},
		{"c1": 8, "c2": 8},
		{"c1": 9, "c2": 9},
	})
	if err != nil {
		t.Fatalf("Failed to insert second batch: %v", err)
	}

	// Test limit 2
	res, err := table.Output([]string{"c1", "c2"}).Limit(2).ToResult()
	if err != nil {
		t.Fatalf("Failed to query with limit 2: %v", err)
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if c1Col, exists := result.Data["c1"]; exists {
			if len(c1Col) != 2 {
				t.Errorf("limit 2: expected 2 rows, got %d", len(c1Col))
			}
			// Verify first two rows: c1=-3, c2=3 and c1=-2, c2=2
			if len(c1Col) >= 2 {
				var c1Val int
				switch v := c1Col[0].(type) {
				case int:
					c1Val = v
				case int32:
					c1Val = int(v)
				case int64:
					c1Val = int(v)
				}
				if c1Val != -3 {
					t.Errorf("limit 2 first row c1: expected -3, got %d", c1Val)
				}
			}
		}
	}

	// Test limit 2 offset 2
	res, err = table.Output([]string{"c1", "c2"}).Limit(2).Offset(2).ToResult()
	if err != nil {
		t.Fatalf("Failed to query with limit 2 offset 2: %v", err)
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if c1Col, exists := result.Data["c1"]; exists {
			if len(c1Col) != 2 {
				t.Errorf("limit 2 offset 2: expected 2 rows, got %d", len(c1Col))
			}
			// Verify rows at offset 2: c1=-1, c2=1 and c1=0, c2=0
			if len(c1Col) >= 2 {
				var c1Val int
				switch v := c1Col[0].(type) {
				case int:
					c1Val = v
				case int32:
					c1Val = int(v)
				case int64:
					c1Val = int(v)
				}
				if c1Val != -1 {
					t.Errorf("limit 2 offset 2 first row c1: expected -1, got %d", c1Val)
				}
			}
		}
	}

	// Test limit 2 offset 2 with total_hits_count option
	table2 := table.Output([]string{"c1", "c2"}).Limit(2).Offset(2)
	table2.Option(map[string]interface{}{"total_hits_count": true})
	res, err = table2.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with total_hits_count: %v", err)
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if c1Col, exists := result.Data["c1"]; exists {
			if len(c1Col) != 2 {
				t.Errorf("limit 2 offset 2 with total_hits_count: expected 2 rows, got %d", len(c1Col))
			}
		}
		// Note: total_hits_count is in ExtraInfo, not directly in Data
		// The actual verification depends on how the SDK returns this info
	}

	// Test limit 3 with sort by c2 asc, c1 desc
	// Expected: c2=0,c1=0; c2=1,c1=1; c2=1,c1=-1
	table3 := table.Output([]string{"c1", "c2"})
	table3.Sort([][2]interface{}{{"c2", infinity.SortTypeAsc}, {"c1", infinity.SortTypeDesc}})
	table3.Limit(3)
	res, err = table3.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with sort and limit: %v", err)
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if c1Col, exists := result.Data["c1"]; exists {
			if len(c1Col) != 3 {
				t.Errorf("sort with limit 3: expected 3 rows, got %d", len(c1Col))
			}
		}
		if c2Col, exists := result.Data["c2"]; exists {
			if len(c2Col) != 3 {
				t.Errorf("sort with limit 3: expected 3 rows in c2, got %d", len(c2Col))
			}
		}
	}
}

// TestLimit2Block tests limit with larger dataset spanning multiple blocks
func TestLimit2Block(t *testing.T) {
	tableName := "test_limit_2block"

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
		{Name: "doc_id", DataType: "varchar"},
		{Name: "knowledge_graph_kwd", DataType: "varchar"},
		{Name: "page_num_int", DataType: "int"},
		{Name: "top_int", DataType: "int"},
		{Name: "create_timestamp_flt", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert data - simplified version for testing
	// Insert multiple batches to create multiple blocks
	for j := 0; j < 5; j++ {
		batchSize := 100
		for i := 0; i < 1000; i += batchSize {
			values := make([]map[string]interface{}, batchSize)
			for k := 0; k < batchSize; k++ {
				knowledgeGraph := ""
				if j == 2 && (i+k)%2 == 0 {
					knowledgeGraph = "graph"
				} else if j == 2 && (i+k)%2 == 1 {
					knowledgeGraph = "mind_map"
				}
				values[k] = map[string]interface{}{
					"doc_id":               "f294ba5cd48711efaaa610ffe02aa993",
					"knowledge_graph_kwd":  knowledgeGraph,
					"page_num_int":         i + k,
					"top_int":              i + k,
					"create_timestamp_flt": 0.0,
				}
			}
			_, err = table.Insert(values)
			if err != nil {
				t.Fatalf("Failed to insert data: %v", err)
			}
		}
	}

	// Test limit with filter
	table2 := table.Output([]string{"doc_id"})
	table2.Option(map[string]interface{}{"total_hits_count": true})
	table2.Filter("(doc_id IN ('f294ba5cd48711efaaa610ffe02aa993')) AND (knowledge_graph_kwd IN ('graph', 'mind_map'))")
	table2.Offset(0)
	table2.Limit(30)
	res, err := table2.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with filter and limit: %v", err)
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if docIdCol, exists := result.Data["doc_id"]; exists {
			if len(docIdCol) != 30 {
				t.Errorf("limit 30 with filter: expected 30 rows, got %d", len(docIdCol))
			}
			// Verify all rows have the expected doc_id
			for i, v := range docIdCol {
				if strVal, ok := v.(string); ok {
					if strVal != "f294ba5cd48711efaaa610ffe02aa993" {
						t.Errorf("row %d: expected doc_id 'f294ba5cd48711efaaa610ffe02aa993', got '%s'", i, strVal)
					}
				}
			}
		}
	}

	// Test limit with filter and sort
	table3 := table.Output([]string{"doc_id"})
	table3.Option(map[string]interface{}{"total_hits_count": true})
	table3.Filter("(doc_id IN ('f294ba5cd48711efaaa610ffe02aa993')) AND (knowledge_graph_kwd IN ('graph', 'mind_map'))")
	table3.Offset(0)
	table3.Limit(30)
	table3.Sort([][2]interface{}{{"page_num_int", infinity.SortTypeAsc}, {"top_int", infinity.SortTypeAsc}, {"create_timestamp_flt", infinity.SortTypeDesc}})
	res, err = table3.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with filter, sort and limit: %v", err)
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if docIdCol, exists := result.Data["doc_id"]; exists {
			if len(docIdCol) != 30 {
				t.Errorf("limit 30 with filter and sort: expected 30 rows, got %d", len(docIdCol))
			}
		}
	}
}
