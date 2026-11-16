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

// Index edge cases the Python suite covers and this one did not: an index over
// an empty column, invalid options, an index created after the rows were
// updated, and an explicit `dump_index`.
//
// Source: `python/test_pysdk/test_index.py`
// (`test_empty_fulltext_index`, `test_create_index_with_invalid_options`,
// `test_create_index_on_update_table`, `test_dump_index`,
// `test_fulltext_match_with_invalid_analyzer`).

import (
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// TestEmptyFulltextIndex: an index over a column that has no rows must build,
// answer nothing, and keep working once rows arrive.
func TestEmptyFulltextIndex(t *testing.T) {
	const tableName = "test_empty_ft"
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
		{Name: "body", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// No rows yet: the build path has nothing to index.
	if _, err := table.CreateIndex("ft", infinity.NewIndexInfo("body", infinity.IndexTypeFullText, nil), infinity.ConflictTypeError, ""); err != nil {
		t.Fatalf("create index on an empty column: %v", err)
	}
	result, err := table.Output([]string{"id"}).Filter("body = 'nothing'").ToResult()
	if err != nil {
		t.Fatalf("query an empty index: %v", err)
	}
	if qr, ok := result.(*infinity.QueryResult); !ok || len(qr.Data["id"]) != 0 {
		t.Fatalf("an empty index answered %v", result)
	}

	// Rows written afterwards have to reach the same index.
	if _, err := table.Insert([]map[string]interface{}{
		{"id": 1, "body": "alpha beta"},
		{"id": 2, "body": "gamma"},
	}); err != nil {
		t.Fatalf("insert: %v", err)
	}
	result, err = table.Output([]string{"id"}).ToResult()
	if err != nil {
		t.Fatalf("query after insert: %v", err)
	}
	if qr, ok := result.(*infinity.QueryResult); !ok || len(qr.Data["id"]) != 2 {
		t.Fatalf("rows written after the index was built are missing: %v", result)
	}
}

// TestCreateIndexWithInvalidOptions: an option the index type does not know is
// an error, not a silently ignored key.
func TestCreateVectorIndexWithUnknownOption(t *testing.T) {
	const tableName = "test_invalid_idx_options"
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
		{Name: "v", DataType: "vector,4,float"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// `not_an_option` is not a HNSW parameter.
	if _, err := table.CreateIndex(
		"bad",
		infinity.NewIndexInfo("v", infinity.IndexTypeHnsw, map[string]string{
			"metric":        "l2",
			"not_an_option": "1",
		}),
		infinity.ConflictTypeError,
		"",
	); err == nil {
		t.Fatal("an unknown HNSW option was accepted")
	}

	// A valid one still works.
	if _, err := table.CreateIndex(
		"good",
		infinity.NewIndexInfo("v", infinity.IndexTypeHnsw, map[string]string{
			"metric":          "l2",
			"M":               "16",
			"ef_construction": "20",
		}),
		infinity.ConflictTypeError,
		"",
	); err != nil {
		t.Fatalf("a valid HNSW option set was rejected: %v", err)
	}
}

// TestCreateIndexOnUpdateTable: an index built after an UPDATE has to see the
// updated rows, not the pre-update copies.
func TestCreateIndexOnUpdateTable(t *testing.T) {
	const tableName = "test_index_on_update"
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
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	rows := make([]map[string]interface{}, 0, 8)
	for i := 0; i < 8; i++ {
		rows = append(rows, map[string]interface{}{"id": i, "grp": 0})
	}
	if _, err := table.Insert(rows); err != nil {
		t.Fatalf("insert: %v", err)
	}
	// Move four of them to group 1 before the index exists.
	if _, err := table.Update("id < 4", map[string]interface{}{"grp": 1}); err != nil {
		t.Fatalf("update: %v", err)
	}
	if _, err := table.CreateIndex("gi", infinity.NewIndexInfo("grp", infinity.IndexTypeSecondary, nil), infinity.ConflictTypeError, ""); err != nil {
		t.Fatalf("create index: %v", err)
	}

	count := func(filter string) int {
		raw, err := table.Output([]string{"id"}).Filter(filter).ToResult()
		if err != nil {
			t.Fatalf("%s: %v", filter, err)
		}
		qr, ok := raw.(*infinity.QueryResult)
		if !ok {
			t.Fatalf("%s returned %T", filter, raw)
		}
		return len(qr.Data["id"])
	}
	if got := count("grp = 0"); got != 4 {
		t.Fatalf("grp = 0 matched %d rows after the update, want 4", got)
	}
	if got := count("grp = 1"); got != 4 {
		t.Fatalf("grp = 1 matched %d rows after the update, want 4", got)
	}
}

// TestFulltextMatchWithInvalidAnalyzer: an analyzer name the index family does
// not know is an error at CREATE INDEX time.
func TestFulltextMatchWithInvalidAnalyzer(t *testing.T) {
	const tableName = "test_invalid_analyzer"
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
		{Name: "body", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	if _, err := table.CreateIndex(
		"ft",
		infinity.NewIndexInfo("body", infinity.IndexTypeFullText, map[string]string{
			"analyzer": "no_such_analyzer",
		}),
		infinity.ConflictTypeError,
		"",
	); err == nil {
		t.Fatal("an unknown analyzer was accepted")
	}
}
