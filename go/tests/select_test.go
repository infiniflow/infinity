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

// TestSelectDateTime tests select operations with date, time, datetime, and timestamp types
func TestSelectDateTime(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_datetime" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with date, time, datetime, timestamp columns
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "date"},
		{Name: "c2", DataType: "time"},
		{Name: "c3", DataType: "datetime"},
		{Name: "c4", DataType: "timestamp"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": "2024-09-23", "c2": "20:45:11", "c3": "2024-09-23 20:45:11", "c4": "2024-09-23 20:45:11"},
	})
	if err != nil {
		t.Fatalf("Failed to insert first row: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": "2022-05-26", "c2": "21:44:33", "c3": "2022-05-26 21:44:33", "c4": "2022-05-26 21:44:33"},
	})
	if err != nil {
		t.Fatalf("Failed to insert second row: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": "2021-03-04", "c2": "20:58:59", "c3": "2021-03-04 20:58:59", "c4": "2021-03-04 20:58:59"},
	})
	if err != nil {
		t.Fatalf("Failed to insert third row: %v", err)
	}

	// Test select *
	_, err = table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Select * failed: %v", err)
	}

	// Test select c1, c2 with filter c1 = DATE '2024-09-23'
	_, err = table.Output([]string{"c1", "c2"}).Filter("c1 = '2024-09-23'").ToResult()
	if err != nil {
		t.Fatalf("Select with date filter failed: %v", err)
	}

	// Test select * with filter c2 = TIME '21:44:33'
	_, err = table.Output([]string{"*"}).Filter("c2 = '21:44:33'").ToResult()
	if err != nil {
		t.Fatalf("Select with time filter failed: %v", err)
	}

	// Test select * with filter c3 = DATETIME '2021-03-04 20:58:59'
	_, err = table.Output([]string{"*"}).Filter("c3 = '2021-03-04 20:58:59'").ToResult()
	if err != nil {
		t.Fatalf("Select with datetime filter failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectVarchar tests select operations with varchar columns and string comparisons
func TestSelectVarchar(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_varchar" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	// Create table with varchar columns (c1 primary key, c2 not null)
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "varchar", Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey, infinity.ConstraintNotNull}},
		{Name: "c2", DataType: "varchar", Constraints: []infinity.ColumnConstraint{infinity.ConstraintNotNull}},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data (a through m)
	_, err = table.Insert([]map[string]interface{}{
		{"c1": "a", "c2": "a"},
		{"c1": "b", "c2": "b"},
		{"c1": "c", "c2": "c"},
		{"c1": "d", "c2": "d"},
		{"c1": "e", "c2": "e"},
		{"c1": "f", "c2": "f"},
		{"c1": "g", "c2": "g"},
		{"c1": "h", "c2": "h"},
		{"c1": "i", "c2": "i"},
		{"c1": "j", "c2": "j"},
		{"c1": "k", "c2": "k"},
		{"c1": "l", "c2": "l"},
		{"c1": "m", "c2": "m"},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test select *
	_, err = table.Output([]string{"*"}).ToResult()
	if err != nil {
		t.Fatalf("Select * failed: %v", err)
	}

	// Test select c1, c2 with filter c1 = 'a'
	_, err = table.Output([]string{"c1", "c2"}).Filter("c1 = 'a'").ToResult()
	if err != nil {
		t.Fatalf("Select with c1 = 'a' failed: %v", err)
	}

	// Test select c1 with filter c1 > 'a' and c2 < 'c' (should return 'b')
	_, err = table.Output([]string{"c1"}).Filter("c1 > 'a' and c2 < 'c'").ToResult()
	if err != nil {
		t.Fatalf("Select with c1 > 'a' and c2 < 'c' failed: %v", err)
	}

	// Test select c2 with filter ('a' < c1 or 'm' <= c1) and (c1 = 'a')
	_, err = table.Output([]string{"c2"}).Filter("('a' < c1 or 'm' <= c1) and (c1 = 'a')").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 1 failed: %v", err)
	}

	// Test select c2 with filter ('a' < c1 and c1 <= 'b') or (c1 >= 'c' and 'd' > c1)
	_, err = table.Output([]string{"c2"}).Filter("('a' < c1 and c1 <= 'b') or (c1 >= 'c' and 'd' > c1)").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 2 failed: %v", err)
	}

	// Test select c2 with complex nested filter
	_, err = table.Output([]string{"c2"}).Filter("((c1 >= 'a' and 'd' >= c1) or (c1 >= 'e' and 'j' > c1)) and ((c1 > 'e' and c1 <= 'f') or (c1 > 'a' and c1 < 'c'))").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 3 failed: %v", err)
	}

	// Test select c2 with filter ('a' < c1 or 'm' <= c1) and (c2 = 'a')
	_, err = table.Output([]string{"c2"}).Filter("('a' < c1 or 'm' <= c1) and (c2 = 'a')").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 4 failed: %v", err)
	}

	// Test select c2 with filter ('a' < c1 and c2 <= 'b') or (c1 >= 'c' and 'd' > c2)
	_, err = table.Output([]string{"c2"}).Filter("('a' < c1 and c2 <= 'b') or (c1 >= 'c' and 'd' > c2)").ToResult()
	if err != nil {
		t.Fatalf("Select with complex filter 5 failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectBig tests selecting from a table with large amount of data
func TestSelectBig(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_big" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "varchar", Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey, infinity.ConstraintNotNull}},
		{Name: "c2", DataType: "varchar", Constraints: []infinity.ColumnConstraint{infinity.ConstraintNotNull}},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert 4000 rows (1000 iterations * 4 rows)
	for i := 0; i < 1000; i++ {
		_, err = table.Insert([]map[string]interface{}{
			{"c1": "a", "c2": "a"},
			{"c1": "b", "c2": "b"},
			{"c1": "c", "c2": "c"},
			{"c1": "d", "c2": "d"},
		})
		if err != nil {
			t.Fatalf("Failed to insert data at iteration %d: %v", i, err)
		}
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectSameOutput tests selecting same column multiple times
func TestSelectSameOutput(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_same_output" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 2},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test selecting same column multiple times
	_, err = table.Output([]string{"c1", "c2"}).ToResult()
	if err != nil {
		t.Fatalf("Select c1, c2 failed: %v", err)
	}

	_, err = table.Output([]string{"c1", "c1"}).ToResult()
	if err != nil {
		t.Fatalf("Select c1, c1 failed: %v", err)
	}

	_, err = table.Output([]string{"c1", "c2", "c1"}).ToResult()
	if err != nil {
		t.Fatalf("Select c1, c2, c1 failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestEmptyTable tests selecting from an empty table
func TestEmptyTable(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_empty_table" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Test selecting from empty table
	_, err = table.Output([]string{"c1", "c2"}).ToResult()
	if err != nil {
		t.Fatalf("Select from empty table failed: %v", err)
	}

	_, err = table.Output([]string{"c1", "c1"}).ToResult()
	if err != nil {
		t.Fatalf("Select same column from empty table failed: %v", err)
	}

	_, err = table.Output([]string{"c1", "c2", "c1"}).ToResult()
	if err != nil {
		t.Fatalf("Select multiple columns from empty table failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectVarcharLength tests varchar length function
func TestSelectVarcharLength(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_varchar_length" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "varchar", Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey, infinity.ConstraintNotNull}},
		{Name: "c2", DataType: "varchar", Constraints: []infinity.ColumnConstraint{infinity.ConstraintNotNull}},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": "a", "c2": "a"},
		{"c1": "b", "c2": "b"},
		{"c1": "c", "c2": "c"},
		{"c1": "d", "c2": "d"},
		{"c1": "abc", "c2": "abc"},
		{"c1": "bbc", "c2": "bbc"},
		{"c1": "cbc", "c2": "cbc"},
		{"c1": "dbc", "c2": "dbc"},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test char_length = 1
	_, err = table.Output([]string{"*"}).Filter("char_length(c1) = 1").ToResult()
	if err != nil {
		t.Fatalf("Select with char_length = 1 failed: %v", err)
	}

	// Test char_length = 3
	_, err = table.Output([]string{"*"}).Filter("char_length(c1) = 3").ToResult()
	if err != nil {
		t.Fatalf("Select with char_length = 3 failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectUpperLower tests upper and lower functions
func TestSelectUpperLower(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_upper_lower" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "varchar", Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey, infinity.ConstraintNotNull}},
		{Name: "c2", DataType: "varchar", Constraints: []infinity.ColumnConstraint{infinity.ConstraintNotNull}},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": "a", "c2": "A"},
		{"c1": "b", "c2": "B"},
		{"c1": "c", "c2": "C"},
		{"c1": "d", "c2": "D"},
		{"c1": "abc", "c2": "ABC"},
		{"c1": "bbc", "c2": "bbc"},
		{"c1": "cbc", "c2": "cbc"},
		{"c1": "dbc", "c2": "dbc"},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test upper(c1) = c2
	_, err = table.Output([]string{"*"}).Filter("upper(c1) = c2").ToResult()
	if err != nil {
		t.Fatalf("Select with upper(c1) = c2 failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectSqrt tests sqrt function
func TestSelectSqrt(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_sqrt" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "integer"},
		{Name: "c2", DataType: "double"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 2.0},
		{"c1": 4, "c2": 5.0},
		{"c1": 9, "c2": 10.0},
		{"c1": 16, "c2": 17.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test sqrt in output
	_, err = table.Output([]string{"*", "sqrt(c1)", "sqrt(c2)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with sqrt in output failed: %v", err)
	}

	// Test sqrt in filter
	_, err = table.Output([]string{"*"}).Filter("sqrt(c1) = 2").ToResult()
	if err != nil {
		t.Fatalf("Select with sqrt in filter failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectRound tests round, ceil, floor functions
func TestSelectRound(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_round" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "integer"},
		{Name: "c2", DataType: "double"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 2.41},
		{"c1": 4, "c2": -2.49},
		{"c1": 9, "c2": 2.55},
		{"c1": 16, "c2": -2.55},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test round
	_, err = table.Output([]string{"c1", "round(c2)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with round failed: %v", err)
	}

	// Test round with precision
	_, err = table.Output([]string{"c1", "round(c2, 1)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with round(c2, 1) failed: %v", err)
	}

	// Test ceil
	_, err = table.Output([]string{"c1", "ceil(c2)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with ceil failed: %v", err)
	}

	// Test floor
	_, err = table.Output([]string{"c1", "floor(c2)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with floor failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectDateFunctions tests year, month, day functions
func TestSelectDateFunctions(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_date_funcs" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "date"},
		{Name: "c2", DataType: "datetime"},
		{Name: "c3", DataType: "timestamp"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": "2024-09-23", "c2": "2022-05-26 21:44:33", "c3": "2024-09-23 20:45:11"},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test year function
	_, err = table.Output([]string{"year(c1)", "year(c2)", "year(c3)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with year failed: %v", err)
	}

	// Test month function
	_, err = table.Output([]string{"month(c1)", "month(c2)", "month(c3)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with month failed: %v", err)
	}

	// Test day function
	_, err = table.Output([]string{"day(c1)", "day(c2)", "day(c3)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with day failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestSelectTimeFunctions tests hour, minute, second functions
func TestSelectTimeFunctions(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_select_time_funcs" + generateSuffix(t)
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "time"},
		{Name: "c2", DataType: "datetime"},
		{Name: "c3", DataType: "timestamp"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	_, err = table.Insert([]map[string]interface{}{
		{"c1": "0:0:0", "c2": "2022-05-26 21:44:33", "c3": "2024-09-23 20:45:11"},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test hour function
	_, err = table.Output([]string{"hour(c1)", "hour(c2)", "hour(c3)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with hour failed: %v", err)
	}

	// Test minute function
	_, err = table.Output([]string{"minute(c1)", "minute(c2)", "minute(c3)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with minute failed: %v", err)
	}

	// Test second function
	_, err = table.Output([]string{"second(c1)", "second(c2)", "second(c3)"}).ToResult()
	if err != nil {
		t.Fatalf("Select with second failed: %v", err)
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}
