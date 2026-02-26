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
	"sort"
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
	// Expected: [0, 0, 0, 1, 1, 2, 2, 2, 3, 3] (sorted)
	res, err := table.Output([]string{"unnest(c2)"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query unnest(c2): %v", err)
	}
	if res == nil {
		t.Fatal("Query result is nil")
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if unnestCol, exists := result.Data["unnest(c2)"]; exists {
			// Convert to int values and sort for comparison
			values := make([]int, len(unnestCol))
			for i, v := range unnestCol {
				switch val := v.(type) {
				case int:
					values[i] = val
				case int32:
					values[i] = int(val)
				case int64:
					values[i] = int(val)
				default:
					t.Fatalf("Unexpected type for unnest(c2): %T", v)
				}
			}
			sort.Ints(values)
			expected := []int{0, 0, 0, 1, 1, 2, 2, 2, 3, 3}
			if len(values) != len(expected) {
				t.Errorf("unnest(c2): expected %v, got %v", expected, values)
			}
			for i := range expected {
				if values[i] != expected[i] {
					t.Errorf("unnest(c2)[%d]: expected %d, got %d", i, expected[i], values[i])
					break
				}
			}
		} else {
			t.Error("unnest(c2) column not found in result")
		}
	} else {
		t.Fatalf("Unexpected result type: %T", res)
	}

	// Test c1, unnest(c2) - output column with unnested array
	// Expected: c1=[1,1,2,2,3,3,3,4,4,4], unnest(c2)=[0,1,2,3,0,1,2,0,2,3]
	res, err = table.Output([]string{"c1", "unnest(c2)"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query c1, unnest(c2): %v", err)
	}
	if res == nil {
		t.Fatal("Query result is nil")
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if c1Col, exists := result.Data["c1"]; exists {
			if len(c1Col) != 10 {
				t.Errorf("c1 column: expected 10 rows, got %d", len(c1Col))
			}
		} else {
			t.Error("c1 column not found in result")
		}
		if unnestCol, exists := result.Data["unnest(c2)"]; exists {
			if len(unnestCol) != 10 {
				t.Errorf("unnest(c2) column: expected 10 rows, got %d", len(unnestCol))
			}
		} else {
			t.Error("unnest(c2) column not found in result")
		}
	}

	// Test c1, c2, unnest(c2) - output multiple columns with unnest
	res, err = table.Output([]string{"c1", "c2", "unnest(c2)"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query c1, c2, unnest(c2): %v", err)
	}
	if res == nil {
		t.Fatal("Query result is nil")
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if c1Col, exists := result.Data["c1"]; exists {
			if len(c1Col) != 10 {
				t.Errorf("c1 column: expected 10 rows, got %d", len(c1Col))
			}
		}
		if c2Col, exists := result.Data["c2"]; exists {
			if len(c2Col) != 10 {
				t.Errorf("c2 column: expected 10 rows, got %d", len(c2Col))
			}
		}
		if unnestCol, exists := result.Data["unnest(c2)"]; exists {
			if len(unnestCol) != 10 {
				t.Errorf("unnest(c2) column: expected 10 rows, got %d", len(unnestCol))
			}
		}
	}

	// Test c1, unnest(c2) as uc2 with filter c1 > 2
	// Expected: c1=[3,3,3,4,4,4], uc2=[0,1,2,0,2,3]
	table2 := table.Output([]string{"c1", "unnest(c2) as uc2"})
	table2.Filter("c1 > 2")
	res, err = table2.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with filter c1 > 2: %v", err)
	}
	if res == nil {
		t.Fatal("Query result is nil")
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if c1Col, exists := result.Data["c1"]; exists {
			if len(c1Col) != 6 {
				t.Errorf("c1 > 2: expected 6 rows, got %d", len(c1Col))
			}
		}
		if uc2Col, exists := result.Data["uc2"]; exists {
			if len(uc2Col) != 6 {
				t.Errorf("uc2 with c1 > 2: expected 6 rows, got %d", len(uc2Col))
			}
		} else {
			t.Error("uc2 column not found in result")
		}
	}

	// Test c1, unnest(c2) as uc2 with filter uc2 > 1
	// Expected: c1=[2,2,3,4,4], uc2=[2,3,2,2,3]
	table3 := table.Output([]string{"c1", "unnest(c2) as uc2"})
	table3.Filter("uc2 > 1")
	res, err = table3.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with filter uc2 > 1: %v", err)
	}
	if res == nil {
		t.Fatal("Query result is nil")
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if c1Col, exists := result.Data["c1"]; exists {
			if len(c1Col) != 5 {
				t.Errorf("uc2 > 1: expected 5 rows, got %d", len(c1Col))
			}
		}
		if uc2Col, exists := result.Data["uc2"]; exists {
			if len(uc2Col) != 5 {
				t.Errorf("uc2 > 1: expected 5 rows, got %d", len(uc2Col))
			}
		}
	}

	// Test unnest(c2) as uc2 with group by and sum
	// Expected: uc2=[0,1,2,3], sum(c1)=[8,4,9,6]
	table4 := table.Output([]string{"unnest(c2) as uc2", "sum(c1)"})
	table4.GroupBy([]string{"uc2"})
	res, err = table4.ToResult()
	if err != nil {
		t.Fatalf("Failed to query with group by: %v", err)
	}
	if res == nil {
		t.Fatal("Query result is nil")
	}
	if result, ok := res.(*infinity.QueryResult); ok {
		if uc2Col, exists := result.Data["uc2"]; exists {
			if len(uc2Col) != 4 {
				t.Errorf("group by uc2: expected 4 rows, got %d", len(uc2Col))
			}
		}
		if sumCol, exists := result.Data["sum(c1)"]; exists {
			if len(sumCol) != 4 {
				t.Errorf("sum(c1): expected 4 rows, got %d", len(sumCol))
			}
			// Verify sum values: uc2=0->sum=1+3+4=8, uc2=1->sum=1+3=4, uc2=2->sum=2+3+4=9, uc2=3->sum=2+4=6
			expectedSums := map[int]int64{
				0: 8,
				1: 4,
				2: 9,
				3: 6,
			}
			uc2Col := result.Data["uc2"]
			for i := 0; i < len(sumCol); i++ {
				var uc2 int
				switch val := uc2Col[i].(type) {
				case int:
					uc2 = val
				case int32:
					uc2 = int(val)
				case int64:
					uc2 = int(val)
				default:
					continue
				}
				var sumVal int64
				switch val := sumCol[i].(type) {
				case int64:
					sumVal = val
				case int:
					sumVal = int64(val)
				case int32:
					sumVal = int64(val)
				default:
					continue
				}
				if expected, ok := expectedSums[uc2]; ok {
					if sumVal != expected {
						t.Errorf("sum(c1) for uc2=%d: expected %d, got %d", uc2, expected, sumVal)
					}
				}
			}
		} else {
			t.Error("sum(c1) column not found in result")
		}
	}
}
