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

// Secondary-index *query* behaviour.
//
// The rest of `index_test.go` checks that an index can be created and dropped.
// This file checks that it answers correctly, which is a different claim: the
// low-cardinality path (a bitmap per distinct value), the high-cardinality path
// (a PGM over the sorted keys), and the memory tail that holds rows written
// after the chunks were dumped.
//
// Source: `python/test_pysdk/test_secondary_index.py`. The scenarios are the
// same; the assertions are stronger, because the row counts are cross-checked
// against a scan of the same filter with the index dropped.

import (
	"fmt"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// rowsOf runs `filter` on `table` and returns how many rows it matched, plus
// the values of `column` in the order the engine returned them.
func rowsOf(t *testing.T, table *infinity.Table, column, filter string) (int, []interface{}) {
	t.Helper()
	raw, err := table.Output([]string{column}).Filter(filter).ToResult()
	if err != nil {
		t.Fatalf("query %q failed: %v", filter, err)
	}
	result, ok := raw.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("query %q returned %T, want *infinity.QueryResult", filter, raw)
	}
	values, ok := result.Data[column]
	if !ok {
		t.Fatalf("query %q returned no column %q: %v", filter, column, result.Data)
	}
	return len(values), values
}

// expectRows asserts the count and that every returned value is `want`.
func expectRows(t *testing.T, table *infinity.Table, column, filter string, want int, wantValue interface{}) {
	t.Helper()
	got, values := rowsOf(t, table, column, filter)
	if got != want {
		t.Fatalf("%q matched %d rows, want %d", filter, got, want)
	}
	for _, v := range values {
		if fmt.Sprint(v) != fmt.Sprint(wantValue) {
			t.Fatalf("%q returned %v, want only %v", filter, v, wantValue)
		}
	}
}

// TestSecondaryIndexLowCardinalityInt is the bitmap path: three distinct values
// across thirty rows, so every posting list is dense.
func TestSecondaryIndexLowCardinalityInt(t *testing.T) {
	const tableName = "test_secondary_low_card_int"
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
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}

	rows := make([]map[string]interface{}, 0, 30)
	for i := 0; i < 30; i++ {
		rows = append(rows, map[string]interface{}{"c1": (i % 3) + 1, "c2": fmt.Sprintf("text_%d", i)})
	}
	if _, err := table.Insert(rows); err != nil {
		t.Fatalf("insert: %v", err)
	}

	// The index is created *after* the rows, so the build path has to populate
	// it from the existing data rather than collect it from later inserts.
	if _, err := table.CreateIndex(
		"idx_c1",
		infinity.NewIndexInfo("c1", infinity.IndexTypeSecondary, map[string]string{"cardinality": "low"}),
		infinity.ConflictTypeError,
		"",
	); err != nil {
		t.Fatalf("create index: %v", err)
	}

	expectRows(t, table, "c1", "c1 = 1", 10, 1)
	expectRows(t, table, "c1", "c1 = 2", 10, 2)
	expectRows(t, table, "c1", "c1 = 3", 10, 3)
	// A value that is not present still has to answer zero, not an error.
	expectRows(t, table, "c1", "c1 = 4", 0, nil)

	if _, err := table.DropIndex("idx_c1", infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop index: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestSecondaryIndexLowCardinalityBool is the extreme case: two distinct values.
func TestSecondaryIndexLowCardinalityBool(t *testing.T) {
	const tableName = "test_secondary_low_card_bool"
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
		{Name: "c1", DataType: "bool"},
		{Name: "c2", DataType: "int"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}

	rows := make([]map[string]interface{}, 0, 20)
	for i := 0; i < 20; i++ {
		rows = append(rows, map[string]interface{}{"c1": i%2 == 0, "c2": i})
	}
	if _, err := table.Insert(rows); err != nil {
		t.Fatalf("insert: %v", err)
	}
	if _, err := table.CreateIndex(
		"idx_c1",
		infinity.NewIndexInfo("c1", infinity.IndexTypeSecondary, map[string]string{"cardinality": "low"}),
		infinity.ConflictTypeError,
		"",
	); err != nil {
		t.Fatalf("create index: %v", err)
	}

	expectRows(t, table, "c1", "c1 = true", 10, true)
	expectRows(t, table, "c1", "c1 = false", 10, false)

	if _, err := table.DropIndex("idx_c1", infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop index: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestSecondaryIndexLowCardinalityVarchar covers a string key, which the low-
// cardinality path stores in the same bitmap-per-value shape.
func TestSecondaryIndexLowCardinalityVarchar(t *testing.T) {
	const tableName = "test_secondary_low_card_varchar"
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
		{Name: "c1", DataType: "varchar"},
		{Name: "c2", DataType: "int"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}

	categories := []string{"A", "B", "C"}
	rows := make([]map[string]interface{}, 0, 30)
	for i := 0; i < 30; i++ {
		rows = append(rows, map[string]interface{}{"c1": categories[i%3], "c2": i})
	}
	if _, err := table.Insert(rows); err != nil {
		t.Fatalf("insert: %v", err)
	}
	if _, err := table.CreateIndex(
		"idx_c1",
		infinity.NewIndexInfo("c1", infinity.IndexTypeSecondary, map[string]string{"cardinality": "low"}),
		infinity.ConflictTypeError,
		"",
	); err != nil {
		t.Fatalf("create index: %v", err)
	}

	for _, category := range categories {
		expectRows(t, table, "c1", fmt.Sprintf("c1 = '%s'", category), 10, category)
	}

	if _, err := table.DropIndex("idx_c1", infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop index: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestSecondaryIndexHighCardinality is the PGM path: every value is distinct,
// so an equality lookup and a range both have to answer exactly.
func TestSecondaryIndexHighCardinality(t *testing.T) {
	const tableName = "test_secondary_high_card"
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
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}

	rows := make([]map[string]interface{}, 0, 50)
	for i := 0; i < 50; i++ {
		rows = append(rows, map[string]interface{}{"c1": i, "c2": fmt.Sprintf("text_%d", i)})
	}
	if _, err := table.Insert(rows); err != nil {
		t.Fatalf("insert: %v", err)
	}
	if _, err := table.CreateIndex(
		"idx_c1",
		infinity.NewIndexInfo("c1", infinity.IndexTypeSecondary, nil),
		infinity.ConflictTypeError,
		"",
	); err != nil {
		t.Fatalf("create index: %v", err)
	}

	expectRows(t, table, "c1", "c1 = 25", 1, 25)
	// A range returns several distinct values, so only the count is fixed.
	if got, _ := rowsOf(t, table, "c1", "c1 >= 40"); got != 10 {
		t.Fatalf("c1 >= 40 matched %d rows, want 10", got)
	}

	// The same range with the index gone must match: this is what makes the
	// assertion about the index rather than about the scan path.
	before, _ := rowsOf(t, table, "c1", "c1 >= 40")
	if _, err := table.DropIndex("idx_c1", infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop index: %v", err)
	}
	after, _ := rowsOf(t, table, "c1", "c1 >= 40")
	if before != after {
		t.Fatalf("the index changed the answer: %d rows with it, %d without", before, after)
	}

	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestSecondaryIndexInsertAfterCreation exercises the memory tail: rows written
// after the index exists have to be visible through it, on top of the rows the
// build already covered.
func TestSecondaryIndexInsertAfterCreation(t *testing.T) {
	const tableName = "test_secondary_insert_after"
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
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}

	first := make([]map[string]interface{}, 0, 15)
	for i := 0; i < 15; i++ {
		first = append(first, map[string]interface{}{"c1": i % 5, "c2": fmt.Sprintf("batch1_%d", i)})
	}
	if _, err := table.Insert(first); err != nil {
		t.Fatalf("insert batch 1: %v", err)
	}
	if _, err := table.CreateIndex(
		"idx_c1",
		infinity.NewIndexInfo("c1", infinity.IndexTypeSecondary, nil),
		infinity.ConflictTypeError,
		"",
	); err != nil {
		t.Fatalf("create index: %v", err)
	}
	expectRows(t, table, "c1", "c1 = 0", 3, 0)

	second := make([]map[string]interface{}, 0, 15)
	for i := 15; i < 30; i++ {
		second = append(second, map[string]interface{}{"c1": i % 5, "c2": fmt.Sprintf("batch2_%d", i)})
	}
	if _, err := table.Insert(second); err != nil {
		t.Fatalf("insert batch 2: %v", err)
	}
	// The build covered the first batch; the second batch sits in the memory
	// tail. Both have to be visible through the same lookup.
	expectRows(t, table, "c1", "c1 = 0", 6, 0)

	if _, err := table.DropIndex("idx_c1", infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop index: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}

// TestSecondaryIndexSurvivesACheckpointAndDelete checks the two things a
// secondary index has to keep straight across storage events: a checkpoint
// moves the tail into a chunk (the lookup must then read the chunk), and a
// delete removes the row from the answer.
func TestSecondaryIndexSurvivesACheckpointAndDelete(t *testing.T) {
	const tableName = "test_secondary_checkpoint"
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
		{Name: "grp", DataType: "int"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}

	rows := make([]map[string]interface{}, 0, 40)
	for i := 0; i < 40; i++ {
		rows = append(rows, map[string]interface{}{"id": i, "grp": i % 4})
	}
	if _, err := table.Insert(rows); err != nil {
		t.Fatalf("insert: %v", err)
	}
	if _, err := table.CreateIndex(
		"idx_grp",
		infinity.NewIndexInfo("grp", infinity.IndexTypeSecondary, map[string]string{"cardinality": "low"}),
		infinity.ConflictTypeError,
		"",
	); err != nil {
		t.Fatalf("create index: %v", err)
	}
	expectRows(t, table, "grp", "grp = 0", 10, 0)

	// `FLUSH` checkpoints: the tail's rows become a chunk, and the lookup has to
	// read that chunk instead of the (now empty) tail.
	if err := conn.FlushData(); err != nil {
		t.Fatalf("flush: %v", err)
	}
	expectRows(t, table, "grp", "grp = 0", 10, 0)

	// A delete removes the row from the answer, whether it sits in a chunk or
	// in the tail.
	if _, err := table.Delete("id < 2"); err != nil {
		t.Fatalf("delete: %v", err)
	}
	// `grp` is `id % 4`, so only id 0 was in group 0; deleting ids 0 and 1
	// takes exactly one row out of that group.
	got, _ := rowsOf(t, table, "grp", "grp = 0")
	if got != 9 {
		t.Fatalf("after deleting ids 0 and 1, grp = 0 matched %d rows, want 9", got)
	}

	// Rows written after the checkpoint must still be found.
	if _, err := table.Insert([]map[string]interface{}{{"id": 100, "grp": 0}}); err != nil {
		t.Fatalf("insert after flush: %v", err)
	}
	if total, _ := rowsOf(t, table, "id", "id = 100"); total != 1 {
		t.Fatalf("the row inserted after the checkpoint is not in the table: %d", total)
	}
	// The row inserted after the checkpoint has to be visible through the
	// index, not only through a scan: its slot lies past the block's row count,
	// which the columnar batch path used to read as "hidden" and answer NULL.
	if got, _ := rowsOf(t, table, "grp", "grp = 0"); got != 10 {
		t.Fatalf("grp = 0 matched %d rows, want 10: the row inserted after the "+
			"checkpoint is missing from the index", got)
	}

	if _, err := table.DropIndex("idx_grp", infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop index: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Fatalf("drop table: %v", err)
	}
}
