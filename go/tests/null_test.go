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

// NULL semantics: omitted cells, explicit NULL, `IS NULL` / `IS NOT NULL`,
// aggregates over NULLs, the NOT NULL constraint and column defaults.
//
// Source: `python/test_pysdk/test_null.py`. Go has no DataFrame equality, so
// each case asserts on the values the SDK decodes: a NULL cell arrives as
// `nil`, which is the distinction the Python test makes with `pd.NA`.

import (
	"fmt"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// columnOf runs `output` over `table` with `filter` and returns one column.
func columnOf(t *testing.T, table *infinity.Table, output, filter, column string) []interface{} {
	t.Helper()
	q := table.Output([]string{output})
	if filter != "" {
		q = q.Filter(filter)
	}
	raw, err := q.ToResult()
	if err != nil {
		t.Fatalf("%s [%s]: %v", output, filter, err)
	}
	result, ok := raw.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("%s returned %T, want *QueryResult", output, raw)
	}
	values, ok := result.Data[column]
	if !ok {
		t.Fatalf("%s [%s] returned no column %q: %v", output, filter, column, result.Data)
	}
	return values
}

// nullsIn reports how many of `values` are nil.
func nullsIn(values []interface{}) int {
	n := 0
	for _, v := range values {
		if v == nil {
			n++
		}
	}
	return n
}

// scalarText renders one decoded cell for comparison.
func scalarText(v interface{}) string {
	return fmt.Sprint(v)
}

// TestNullOmittedCells covers a row that names only some of its columns: the
// rest are NULL, not an error.
func TestNullOmittedCells(t *testing.T) {
	const tableName = "test_null_omit"
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("get database: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeIgnore); err != nil {
		t.Fatalf("drop table: %v", err)
	}
	table, err := db.CreateTable(tableName, infinity.TableSchema{
		{Name: "a", DataType: "int"},
		{Name: "b", DataType: "varchar"},
		{Name: "c", DataType: "int"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	// The first row names only `c`: `a` and `b` must come back NULL.
	if _, err := table.Insert([]map[string]interface{}{{"c": 1}}); err != nil {
		t.Fatalf("insert: %v", err)
	}
	if _, err := table.Insert([]map[string]interface{}{{"a": 100, "c": 2}}); err != nil {
		t.Fatalf("insert: %v", err)
	}

	all := columnOf(t, table, "a", "", "a")
	if len(all) != 2 || nullsIn(all) != 1 {
		t.Fatalf("a: %v, want one NULL and one 100", all)
	}
	if got := columnOf(t, table, "a", "a IS NULL", "a"); len(got) != 1 {
		t.Fatalf("a IS NULL matched %d rows, want 1", len(got))
	}
	// Every `b` was omitted, so `b IS NOT NULL` matches nothing.
	if got := columnOf(t, table, "b", "b IS NOT NULL", "b"); len(got) != 0 {
		t.Fatalf("b IS NOT NULL matched %d rows, want 0", len(got))
	}

	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestNullExplicitValues inserts NULLs as explicit map omissions mixed with
// values, and checks the whole column keeps them in place.
func TestNullExplicitValues(t *testing.T) {
	const tableName = "test_null_val"
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("get database: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeIgnore); err != nil {
		t.Fatalf("drop table: %v", err)
	}
	table, err := db.CreateTable(tableName, infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "name", DataType: "varchar"},
		{Name: "score", DataType: "float"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	rows := []map[string]interface{}{
		{"id": 1, "name": "Alice", "score": 90.5},
		{"id": 2},
		{"id": 3, "name": "Bob"},
		{"id": 4, "score": 60.0},
	}
	if _, err := table.Insert(rows); err != nil {
		t.Fatalf("insert: %v", err)
	}

	names := columnOf(t, table, "name", "", "name")
	if len(names) != 4 || nullsIn(names) != 2 {
		t.Fatalf("name: %v, want 2 NULLs among 4", names)
	}
	scores := columnOf(t, table, "score", "", "score")
	if len(scores) != 4 || nullsIn(scores) != 2 {
		t.Fatalf("score: %v, want 2 NULLs among 4", scores)
	}
	// The non-NULL values have to survive next to the NULLs.
	found := map[string]bool{}
	for _, v := range names {
		if v != nil {
			found[scalarText(v)] = true
		}
	}
	if !found["Alice"] || !found["Bob"] {
		t.Fatalf("name values lost: %v", names)
	}

	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestIsNullIsNotNull checks both predicates on the same table, so a predicate
// that silently matched everything (or nothing) would show up.
func TestIsNullIsNotNull(t *testing.T) {
	const tableName = "test_null_pred"
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("get database: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeIgnore); err != nil {
		t.Fatalf("drop table: %v", err)
	}
	table, err := db.CreateTable(tableName, infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "name", DataType: "varchar"},
		{Name: "score", DataType: "int"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	rows := []map[string]interface{}{
		{"id": 1},
		{"id": 2, "name": "Alice"},
		{"id": 3, "name": "Bob", "score": 100},
	}
	if _, err := table.Insert(rows); err != nil {
		t.Fatalf("insert: %v", err)
	}

	cases := []struct {
		filter string
		want   int
	}{
		{"name IS NULL", 1},
		{"name IS NOT NULL", 2},
		{"score IS NULL", 2},
		{"score IS NOT NULL", 1},
	}
	for _, c := range cases {
		got := columnOf(t, table, "id", c.filter, "id")
		if len(got) != c.want {
			t.Fatalf("%q matched %d rows, want %d: %v", c.filter, len(got), c.want, got)
		}
	}

	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestNullCountAggregate checks that `count(*)` counts rows while the result of
// the filtered set is what the Python case asserts with the same filters.
func TestNullCountAggregate(t *testing.T) {
	const tableName = "test_null_count"
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("get database: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeIgnore); err != nil {
		t.Fatalf("drop table: %v", err)
	}
	table, err := db.CreateTable(tableName, infinity.TableSchema{
		{Name: "a", DataType: "int"},
		{Name: "b", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	if _, err := table.Insert([]map[string]interface{}{
		{"a": 1},
		{"a": 2, "b": "x"},
		{"a": 3},
	}); err != nil {
		t.Fatalf("insert: %v", err)
	}

	// The server labels the aggregate `count(star)`; use that as the key.
	star := columnOf(t, table, "count(*)", "b IS NULL", "count(star)")
	if len(star) != 1 || star[0] == nil || scalarText(star[0]) != "2" {
		t.Fatalf("count(*) over b IS NULL = %v, want 2", star)
	}
	// The whole table, unfiltered, is three rows.
	all := columnOf(t, table, "count(*)", "", "count(star)")
	if len(all) != 1 || all[0] == nil || scalarText(all[0]) != "3" {
		t.Fatalf("count(*) = %v, want 3", all)
	}

	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestNotNullConstraintRejectsNull is the constraint half: a column declared
// NOT NULL must refuse an omitted cell rather than storing NULL.
func TestNotNullConstraintRejectsNull(t *testing.T) {
	const tableName = "test_null_constraint"
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("get database: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeIgnore); err != nil {
		t.Fatalf("drop table: %v", err)
	}
	table, err := db.CreateTable(tableName, infinity.TableSchema{
		{Name: "id", DataType: "int", Constraints: []infinity.ColumnConstraint{infinity.ConstraintNotNull}},
		{Name: "name", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	// Naming the NOT NULL column works.
	if _, err := table.Insert([]map[string]interface{}{{"id": 1}}); err != nil {
		t.Fatalf("insert with id: %v", err)
	}
	// Omitting it must fail.
	if _, err := table.Insert([]map[string]interface{}{{"name": "no id"}}); err == nil {
		t.Fatal("an omitted NOT NULL column was accepted")
	}

	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestNullDefault checks that a column default fills an omitted cell, so it is
// a value rather than NULL.
func TestNullDefault(t *testing.T) {
	const tableName = "test_null_default"
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("get database: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeIgnore); err != nil {
		t.Fatalf("drop table: %v", err)
	}
	table, err := db.CreateTable(tableName, infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "grp", DataType: "int", Default: 7},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	if _, err := table.Insert([]map[string]interface{}{{"id": 1}}); err != nil {
		t.Fatalf("insert: %v", err)
	}
	got := columnOf(t, table, "grp", "", "grp")
	if len(got) != 1 || got[0] == nil || scalarText(got[0]) != "7" {
		t.Fatalf("grp = %v, want the default 7", got)
	}

	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}
