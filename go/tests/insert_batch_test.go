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

// Insert throughput: a batch has to reach the server as *one* statement.
//
// The server merges consecutive rows that name the same column list into a
// single INSERT. Go randomises map iteration, so building each row's column
// list with `range` gave a different order per row and the server could not
// merge anything: 20 000 rows arrived as 4 313 statements instead of 20, and
// the workload took 1.73 s instead of 0.68 s.
//
// The case is a regression guard, not a benchmark: it asserts that inserting a
// large batch costs one round trip's worth of statements by comparing against
// the per-row path (which necessarily sends one statement per row).

import (
	"fmt"
	"testing"
	"time"

	"github.com/infiniflow/infinity-go-sdk"
)

// TestInsertBatchIsOneStatement checks that a batched insert is meaningfully
// faster than the same rows sent one at a time.
//
// The ratio is what makes this robust: absolute timings move with the machine,
// but a client that fails to merge a batch always pays the per-row price and
// the two timings converge.
func TestInsertBatchIsOneStatement(t *testing.T) {
	const (
		tableName = "test_insert_batch_cost"
		rows      = 4000
	)
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("get database: %v", err)
	}
	if _, err := db.DropTable(tableName, infinity.ConflictTypeIgnore); err != nil {
		t.Fatalf("drop table: %v", err)
	}
	// Four columns: with Go's randomised map order, a client that does not sort
	// its column names produces a different order on most rows, so the server
	// cannot merge them. Two columns collide too often to be a reliable probe.
	table, err := db.CreateTable(tableName, infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "name", DataType: "varchar"},
		{Name: "grp", DataType: "int"},
		{Name: "tag", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// One statement for the whole batch.
	batch := make([]map[string]interface{}, 0, rows)
	for i := 0; i < rows; i++ {
		batch = append(batch, map[string]interface{}{
			"id":   i,
			"name": fmt.Sprintf("row-%d", i),
			"grp":  i % 7,
			"tag":  fmt.Sprintf("t-%d", i%13),
		})
	}
	started := time.Now()
	if _, err := table.Insert(batch); err != nil {
		t.Fatalf("batched insert: %v", err)
	}
	batched := time.Since(started)

	// One statement per row: the shape a client that fails to merge pays.
	started = time.Now()
	for i := 0; i < rows; i++ {
		if _, err := table.Insert([]map[string]interface{}{{
			"id":   rows + i,
			"name": fmt.Sprintf("single-%d", i),
			"grp":  i % 7,
			"tag":  fmt.Sprintf("t-%d", i%13),
		}}); err != nil {
			t.Fatalf("single insert %d: %v", i, err)
		}
	}
	perRow := time.Since(started)

	t.Logf("batched %d rows: %v; per-row: %v (%.1fx)", rows, batched, perRow,
		float64(perRow)/float64(batched))

	// A merged batch is one round trip; the per-row path is `rows` of them.
	// Measured on this machine (4000 rows, four columns): sorted column names
	// give 18x, unsorted 5x (the server still merges a run of rows that happen
	// to share an order). Ten sits between the two with room for a slower box.
	if float64(perRow)/float64(batched) < 10 {
		t.Fatalf(
			"a batched insert was only %.1fx cheaper than %d single-row inserts: "+
				"the batch was not merged into one statement",
			float64(perRow)/float64(batched), rows,
		)
	}

	// Both halves are in the table.
	raw, err := table.Output([]string{"id"}).ToResult()
	if err != nil {
		t.Fatalf("select: %v", err)
	}
	qr, ok := raw.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("select returned %T", raw)
	}
	if got := len(qr.Data["id"]); got != rows*2 {
		t.Fatalf("%d rows in the table, want %d", got, rows*2)
	}
}

// TestUpdateCoversEveryNamedColumn checks the update path's column list.
//
// `Table.Update` sorts its column names before building the expression array
// (Go's map order is random). A sort that dropped or duplicated an entry would
// leave one column un-updated, which is what this asserts against.
func TestUpdateCoversEveryNamedColumn(t *testing.T) {
	const tableName = "test_update_all_columns"
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
		{Name: "a", DataType: "int"},
		{Name: "b", DataType: "varchar"},
		{Name: "c", DataType: "float"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("create table: %v", err)
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	if _, err := table.Insert([]map[string]interface{}{
		{"id": 1, "a": 0, "b": "before", "c": 0.0},
	}); err != nil {
		t.Fatalf("insert: %v", err)
	}
	// Four columns: the loop that builds the expression array walks a map, so
	// every one of them has to survive the sort.
	if _, err := table.Update("id = 1", map[string]interface{}{
		"a": 10,
		"b": "after",
		"c": 2.5,
	}); err != nil {
		t.Fatalf("update: %v", err)
	}

	row := func(column string) interface{} {
		raw, err := table.Output([]string{column}).Filter("id = 1").ToResult()
		if err != nil {
			t.Fatalf("select %s: %v", column, err)
		}
		qr, ok := raw.(*infinity.QueryResult)
		if !ok {
			t.Fatalf("select %s returned %T", column, raw)
		}
		values := qr.Data[column]
		if len(values) != 1 {
			t.Fatalf("select %s returned %d rows", column, len(values))
		}
		return values[0]
	}
	if got := fmt.Sprint(row("a")); got != "10" {
		t.Fatalf("a = %s, want 10: the update missed a column", got)
	}
	if got := fmt.Sprint(row("b")); got != "after" {
		t.Fatalf("b = %s, want after: the update missed a column", got)
	}
	if got := fmt.Sprint(row("c")); got != "2.5" {
		t.Fatalf("c = %s, want 2.5: the update missed a column", got)
	}
}
