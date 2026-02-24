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

// TestSelectBasic tests basic select operations with various filter conditions
func TestSelectBasic(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with c1 int primary key, c2 int not null
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int", Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey, infinity.ConstraintNotNull}},
		{Name: "c2", DataType: "int", Constraints: []infinity.ColumnConstraint{infinity.ConstraintNotNull}},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert first batch of data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": -3, "c2": -3},
		{"c1": -2, "c2": -2},
		{"c1": -1, "c2": -1},
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
		{"c1": -8, "c2": -8},
		{"c1": -7, "c2": -7},
		{"c1": -6, "c2": -6},
		{"c1": 7, "c2": 7},
		{"c1": 8, "c2": 8},
		{"c1": 9, "c2": 9},
	})
	if err != nil {
		t.Fatalf("Failed to insert second batch: %v", err)
	}

	// Test select *
	_, err = table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Select * failed: %v", err)
	}

	// Test select c1, c2
	_, err = table.Output([]string{"c1", "c2"}).ToResult()
	if err != nil {
		t.Fatalf("Select c1, c2 failed: %v", err)
	}

	// Test select with filter: c1 = 3
	_, err = table.Output([]string{"c1 + c2"}).Filter("c1 = 3").ToResult()
	if err != nil {
		t.Fatalf("Select with filter c1 = 3 failed: %v", err)
	}

	// Test select with filter: c1 > 2 and c2 < 4
	_, err = table.Output([]string{"c1"}).Filter("c1 > 2 and c2 < 4").ToResult()
	if err != nil {
		t.Fatalf("Select with filter c1 > 2 and c2 < 4 failed: %v", err)
	}

	// Test select with complex filter conditions
	_, err = table.Output([]string{"c2"}).Filter("(-7 < c1 or 9 <= c1) and (c1 = 3)").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 1 failed: %v", err)
	}

	_, err = table.Output([]string{"c2"}).Filter("(-8 < c1 and c1 <= -7) or (c1 >= 1 and 2 > c1)").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 2 failed: %v", err)
	}

	_, err = table.Output([]string{"c2"}).Filter("((c1 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c1)) and ((c1 > 0 and c1 <= 1) or (c1 > -8 and c1 < -6))").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 3 failed: %v", err)
	}

	_, err = table.Output([]string{"c2"}).Filter("(-7 < c1 or 9 <= c1) and (c2 = 3)").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 4 failed: %v", err)
	}

	_, err = table.Output([]string{"c2"}).Filter("(-8 < c1 and c2 <= -7) or (c1 >= 1 and 2 > c2)").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 5 failed: %v", err)
	}

	_, err = table.Output([]string{"c2"}).Filter("((c2 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c2)) and ((c2 > 0 and c1 <= 1) or (c1 > -8 and c2 < -6))").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 6 failed: %v", err)
	}

	_, err = table.Output([]string{"c2"}).Filter("(not(c2 < -8 or -4 < c1) or not(c1 < 0 or 5 <= c2)) and not((c2 <= 0 or c1 > 1) and (c1 <= -8 or c2 >= -6))").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 7 failed: %v", err)
	}

	// Test IN operator
	_, err = table.Output([]string{"*"}).Filter("c1 in (1, 2, 3)").ToResult()
	if err != nil {
		t.Fatalf("Select with IN operator failed: %v", err)
	}

	_, err = table.Output([]string{"*"}).Filter("c1 in (1, 2, 3) and c2 in (1, 2, 3)").ToResult()
	if err != nil {
		t.Fatalf("Select with IN operators failed: %v", err)
	}

	// Test NOT IN operator
	_, err = table.Output([]string{"*"}).Filter("c1 not in (1, 2, 3)").ToResult()
	if err != nil {
		t.Fatalf("Select with NOT IN operator failed: %v", err)
	}

	// Test IN with expression
	_, err = table.Output([]string{"*"}).Filter("(c2 + 1) in (8, 9, 10)").ToResult()
	if err != nil {
		t.Fatalf("Select with IN expression failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectJSON tests select operations with JSON data type
func TestSelectJSON(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_json" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with JSON column
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "varchar"},
		{Name: "c3", DataType: "json"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data with JSON
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 654321, "c2": `{"2":3232,"434":"4321","3":43432,"4":1.123}`, "c3": `{"2":3232,"434":"4321","3":43432,"4":1.123}`},
	})
	if err != nil {
		t.Fatalf("Failed to insert first row: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": 123456, "c2": `{"1":null,"2":"123","3":12,"4":1.123}`, "c3": `{"1":null,"2":"10","3":12,"4":1.123}`},
	})
	if err != nil {
		t.Fatalf("Failed to insert second row: %v", err)
	}

	// Test select c2
	_, err = table.Output([]string{"c2"}).ToResult()
	if err != nil {
		t.Fatalf("Select c2 failed: %v", err)
	}

	// Test select c3
	_, err = table.Output([]string{"c3"}).ToResult()
	if err != nil {
		t.Fatalf("Select c3 failed: %v", err)
	}

	// Test count(*)
	_, err = table.Output([]string{"count(*)"}).ToResult()
	if err != nil {
		t.Fatalf("Select count(*) failed: %v", err)
	}

	// Test json_extract
	_, err = table.Output([]string{"json_extract(c3,'$.2')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract failed: %v", err)
	}

	// Test json_extract_string
	_, err = table.Output([]string{"json_extract_string(c3,'$.2')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_string failed: %v", err)
	}

	// Test json_extract_int
	_, err = table.Output([]string{"json_extract_int(c3,'$.2')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_int failed: %v", err)
	}

	// Test json_extract_int with expression
	_, err = table.Output([]string{"json_extract_int(c3,'$.2') + 1"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_int with expression failed: %v", err)
	}

	// Test json_extract_double
	_, err = table.Output([]string{"json_extract_double(c3,'$.4')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_double failed: %v", err)
	}

	// Test json_extract_isnull
	_, err = table.Output([]string{"json_extract_isnull(c3,'$.1')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_isnull failed: %v", err)
	}

	// Test json_exists_path
	_, err = table.Output([]string{"json_exists_path(c3,'$.1')"}).ToResult()
	if err != nil {
		t.Fatalf("json_exists_path failed: %v", err)
	}

	// Test Cast to Varchar
	_, err = table.Output([]string{"Cast(c3 AS Varchar)"}).ToResult()
	if err != nil {
		t.Fatalf("Cast to Varchar failed: %v", err)
	}

	// Insert data with boolean
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 111, "c2": "aaa", "c3": `{"1":null,"2":"10","3":12,"4":true}`},
	})
	if err != nil {
		t.Fatalf("Failed to insert boolean row: %v", err)
	}

	// Test json_extract_bool
	_, err = table.Output([]string{"json_extract_bool(c3,'$.4')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_bool failed: %v", err)
	}

	// Insert data with array
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 222, "c2": "bbb", "c3": `[1,null,"a",1.23,true]`},
	})
	if err != nil {
		t.Fatalf("Failed to insert array row: %v", err)
	}

	// Test json_contains
	_, err = table.Output([]string{"json_contains(c3,'null')"}).ToResult()
	if err != nil {
		t.Fatalf("json_contains failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectJSONComprehensive tests comprehensive JSON operations
func TestSelectJSONComprehensive(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_json_comprehensive" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with JSON column
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "varchar"},
		{Name: "c3", DataType: "json"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Test 1: Nested JSON objects
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": "nested", "c3": `{"name":"test","value":42.5,"active":false,"nested":{"key1":"val1","key2":999}}`},
	})
	if err != nil {
		t.Fatalf("Failed to insert nested JSON: %v", err)
	}

	// Test json_extract_string on nested object
	_, err = table.Output([]string{"json_extract_string(c3,'$.name')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_string $.name failed: %v", err)
	}

	// Test json_extract_double
	_, err = table.Output([]string{"json_extract_double(c3,'$.value')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_double $.value failed: %v", err)
	}

	// Test json_extract_bool
	_, err = table.Output([]string{"json_extract_bool(c3,'$.active')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_bool $.active failed: %v", err)
	}

	// Test nested path extraction
	_, err = table.Output([]string{"json_extract_int(c3,'$.nested.key2')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_int $.nested.key2 failed: %v", err)
	}

	// Test 2: JSON Arrays
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 2, "c2": "array", "c3": `["item1","item2","item3"]`},
	})
	if err != nil {
		t.Fatalf("Failed to insert array JSON: %v", err)
	}

	// Test array element extraction with $[0], $[1], $[2]
	_, err = table.Output([]string{"json_extract_string(c3,'$[0]')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_string $[0] failed: %v", err)
	}

	_, err = table.Output([]string{"json_extract_string(c3,'$[1]')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_string $[1] failed: %v", err)
	}

	_, err = table.Output([]string{"json_extract_string(c3,'$[2]')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_string $[2] failed: %v", err)
	}

	// Test out of bounds index
	_, err = table.Output([]string{"json_extract_string(c3,'$[10]')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_string $[10] failed: %v", err)
	}

	// Test json_contains on arrays
	_, err = table.Output([]string{"json_contains(c3,'\"item1\"')"}).ToResult()
	if err != nil {
		t.Fatalf("json_contains item1 failed: %v", err)
	}

	_, err = table.Output([]string{"json_contains(c3,'\"nonexistent\"')"}).ToResult()
	if err != nil {
		t.Fatalf("json_contains nonexistent failed: %v", err)
	}

	// Test 3: Boolean values
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 3, "c2": "bool", "c3": `{"bool_true":true,"bool_false":false,"zero":0,"negative":-100}`},
	})
	if err != nil {
		t.Fatalf("Failed to insert boolean JSON: %v", err)
	}

	_, err = table.Output([]string{"json_extract_bool(c3,'$.bool_true')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_bool $.bool_true failed: %v", err)
	}

	_, err = table.Output([]string{"json_extract_bool(c3,'$.bool_false')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_bool $.bool_false failed: %v", err)
	}

	_, err = table.Output([]string{"json_extract_int(c3,'$.zero')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_int $.zero failed: %v", err)
	}

	_, err = table.Output([]string{"json_extract_int(c3,'$.negative')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_int $.negative failed: %v", err)
	}

	// Test 4: Mixed type arrays
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 4, "c2": "mixed", "c3": `[1,"two",3.0,true,null,{"key":"value"}]`},
	})
	if err != nil {
		t.Fatalf("Failed to insert mixed array JSON: %v", err)
	}

	// Test array element extraction for different types
	_, err = table.Output([]string{"json_extract_int(c3,'$[0]')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_int $[0] failed: %v", err)
	}

	_, err = table.Output([]string{"json_extract_string(c3,'$[1]')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_string $[1] failed: %v", err)
	}

	_, err = table.Output([]string{"json_extract_double(c3,'$[2]')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_double $[2] failed: %v", err)
	}

	_, err = table.Output([]string{"json_extract_bool(c3,'$[3]')"}).ToResult()
	if err != nil {
		t.Fatalf("json_extract_bool $[3] failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectWithOrderBy tests select with order by clauses
func TestSelectWithOrderBy(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_order" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "varchar"},
		{Name: "c3", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 3, "c2": "charlie", "c3": 3.3},
		{"c1": 1, "c2": "alpha", "c3": 1.1},
		{"c1": 2, "c2": "bravo", "c3": 2.2},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test order by c1 ascending
	_, err = table.Output([]string{"*"}).Sort([][2]interface{}{{"c1", infinity.SortTypeAsc}}).ToResult()
	if err != nil {
		t.Fatalf("Order by c1 asc failed: %v", err)
	}

	// Test order by c1 descending
	_, err = table.Output([]string{"*"}).Sort([][2]interface{}{{"c1", infinity.SortTypeDesc}}).ToResult()
	if err != nil {
		t.Fatalf("Order by c1 desc failed: %v", err)
	}

	// Test order by c2
	_, err = table.Output([]string{"*"}).Sort([][2]interface{}{{"c2", infinity.SortTypeAsc}}).ToResult()
	if err != nil {
		t.Fatalf("Order by c2 asc failed: %v", err)
	}

	// Test order by c3
	_, err = table.Output([]string{"*"}).Sort([][2]interface{}{{"c3", infinity.SortTypeDesc}}).ToResult()
	if err != nil {
		t.Fatalf("Order by c3 desc failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectWithLimitOffset tests select with limit and offset
func TestSelectWithLimitOffset(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_limit" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert 10 rows of test data
	for i := 1; i <= 10; i++ {
		_, err = table.Insert([]map[string]interface{}{
			{"c1": i, "c2": string(rune('a' + i - 1))},
		})
		if err != nil {
			t.Fatalf("Failed to insert row %d: %v", i, err)
		}
	}

	// Test limit
	_, err = table.Output([]string{"*"}).Limit(5).ToResult()
	if err != nil {
		t.Fatalf("Select with limit failed: %v", err)
	}

	// Test limit with offset
	_, err = table.Output([]string{"*"}).Offset(3).Limit(4).ToResult()
	if err != nil {
		t.Fatalf("Select with limit and offset failed: %v", err)
	}

	// Test offset only
	_, err = table.Output([]string{"*"}).Offset(7).ToResult()
	if err != nil {
		t.Fatalf("Select with offset failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectWithGroupBy tests select with group by
func TestSelectWithGroupBy(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_group" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "category", DataType: "varchar"},
		{Name: "amount", DataType: "float"},
		{Name: "count", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"category": "A", "amount": 100.0, "count": 1},
		{"category": "A", "amount": 200.0, "count": 2},
		{"category": "B", "amount": 150.0, "count": 3},
		{"category": "B", "amount": 250.0, "count": 4},
		{"category": "C", "amount": 300.0, "count": 5},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test group by with count
	_, err = table.Output([]string{"category", "count(*)"}).GroupBy([]string{"category"}).ToResult()
	if err != nil {
		t.Fatalf("Group by with count failed: %v", err)
	}

	// Test group by with sum
	_, err = table.Output([]string{"category", "sum(amount)"}).GroupBy([]string{"category"}).ToResult()
	if err != nil {
		t.Fatalf("Group by with sum failed: %v", err)
	}

	// Test group by with avg
	_, err = table.Output([]string{"category", "avg(amount)"}).GroupBy([]string{"category"}).ToResult()
	if err != nil {
		t.Fatalf("Group by with avg failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}
