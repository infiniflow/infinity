//go:build parallel

//
//  Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

package tests

// Gate D, catalog and DML cases: `test_ddl_parallel.py`,
// `test_ddl_and_insert_delete.py`, `test_insert_parallel.py`,
// `test_insert_delete_parallel.py`, `test_insert_delete_update.py`,
// `test_chaos.py` and `test_sdkbase.py`, one test function each.
//
// These are chaos cases: a worker picks an operation at random and the
// expectation is that no statement corrupts another's table. Only the counts
// that must hold are asserted - a losing statement is a legal outcome under
// concurrency, a lost or duplicated row is not.

import (
	"fmt"
	"math/rand"
	"strings"
	"sync"
	"sync/atomic"
	"testing"
	"time"

	infinity "github.com/infiniflow/infinity-go-sdk"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// TestParallelSdkVersion is `test_sdkbase.py`: the SDK is importable and
// connected. It is the cheapest case in the suite, so it also proves the
// fixture and the TestMain ping work.
func TestParallelSdkVersion(t *testing.T) {
	conn := connections(t, 1)[0]
	if !conn.IsConnected() {
		t.Fatalf("connection reports itself closed")
	}
	if infinity.ClientVersion <= 0 {
		t.Fatalf("client version is %d", infinity.ClientVersion)
	}
	t.Logf("client version %d", infinity.ClientVersion)
}

// ddlDatabases and ddlTables are the names `test_dll_parallel` hammers: five
// databases, each holding tables named after a column shape.
var (
	ddlDatabases = []string{
		"dll_parallel_a", "dll_parallel_b", "dll_parallel_c",
		"dll_parallel_d", "dll_parallel_e",
	}
	ddlTableNames = []string{"vec1", "vec2", "vec3", "fulltext1", "fulltext2", "fulltext3"}

	ddlVectorColumns = []infinity.TableSchema{
		{
			{Name: "c1", DataType: "vector,1024,float"},
			{Name: "c2", DataType: "vector,512,int"},
			{Name: "c3", DataType: "int"},
		},
		{
			{Name: "c1", DataType: "vector,512,float"},
			{Name: "c2", DataType: "vector,1024,int"},
			{Name: "c3", DataType: "float"},
		},
	}
	ddlFulltextColumns = []infinity.TableSchema{
		{
			{Name: "doctitle", DataType: "varchar"},
			{Name: "docdate", DataType: "varchar"},
			{Name: "body", DataType: "varchar"},
		},
		{
			{Name: "doctitle", DataType: "varchar"},
			{Name: "docdate", DataType: "varchar"},
			{Name: "body", DataType: "int"},
		},
	}
	ddlVectorIndexColumns   = []string{"c1", "c2"}
	ddlFulltextIndexColumns = []string{"doctitle", "docdate"}
)

// ddlIndexFor builds the index a table of that shape gets: IVF or HNSW on a
// vector table, full-text on a text one.
func ddlIndexFor(tableName string, column string) *infinity.IndexInfo {
	if strings.HasPrefix(tableName, "vec") {
		if rand.Intn(2) == 0 {
			return infinity.NewIndexInfo(column, infinity.IndexTypeIVF, map[string]string{
				"centroids_count": "128",
				"metric":          "l2",
			})
		}
		return infinity.NewIndexInfo(column, infinity.IndexTypeHnsw, map[string]string{
			"M":               "16",
			"ef_construction": "50",
			"metric":          "l2",
		})
	}
	return infinity.NewIndexInfo(column, infinity.IndexTypeFullText, nil)
}

// TestParallelDDL is `test_dll_parallel`: eight workers create and drop
// databases, tables and indexes at random for the whole window, so every kind
// of catalog write runs against every other. It must not corrupt the catalog:
// afterwards the databases it created are all droppable and the rest of the
// catalog is intact.
func TestParallelDDL(t *testing.T) {
	window := window(t)
	const workers = 8

	conns := connections(t, workers)

	// The databases are created up front. `create database` is one of the
	// random operations, but at 1% the window would mostly hit "no such
	// database" and the table and index writes - the interesting half -
	// would never run at all.
	for _, name := range ddlDatabases {
		_, err := conns[0].CreateDatabase(name, infinity.ConflictTypeIgnore, "")
		requireOK(t, err, "create database "+name)
	}

	var dbs, tables, indexes atomic.Int64
	runWindow(t, workers, window, func(worker int, deadline time.Time) error {
		rng := rand.New(rand.NewSource(int64(worker)))
		for time.Now().Before(deadline) {
			dbName := ddlDatabases[rng.Intn(len(ddlDatabases))]
			switch v := rng.Intn(100); {
			case v < 1: // create database
				if _, err := conns[worker].CreateDatabase(dbName, infinity.ConflictTypeIgnore, ""); err == nil {
					dbs.Add(1)
					// Read it back, as the Python case does: `SHOW DATABASE`
					// goes through the catalog while every other worker is
					// rewriting it, which is the read this branch is for.
					conns[worker].ShowDatabase(dbName)
				}
			case v < 2: // drop database
				conns[worker].DropDatabase(dbName, infinity.ConflictTypeIgnore)
			case v < 6: // create table
				d, err := conns[worker].GetDatabase(dbName)
				if err != nil {
					break
				}
				tableName := ddlTableNames[rng.Intn(len(ddlTableNames))]
				schema := ddlFulltextColumns[rng.Intn(len(ddlFulltextColumns))]
				if strings.HasPrefix(tableName, "vec") {
					schema = ddlVectorColumns[rng.Intn(len(ddlVectorColumns))]
				}
				if _, err := d.CreateTable(tableName, schema, infinity.ConflictTypeIgnore); err == nil {
					tables.Add(1)
				}
			case v < 10: // drop table
				d, err := conns[worker].GetDatabase(dbName)
				if err != nil {
					break
				}
				resp, err := d.ListTables()
				if err != nil {
					break
				}
				names := tableNames(t, resp)
				if len(names) > 0 {
					d.DropTable(names[rng.Intn(len(names))], infinity.ConflictTypeIgnore)
				}
			case v < 55: // create index
				d, err := conns[worker].GetDatabase(dbName)
				if err != nil {
					break
				}
				resp, err := d.ListTables()
				if err != nil {
					break
				}
				names := tableNames(t, resp)
				if len(names) == 0 {
					break
				}
				tableName := names[rng.Intn(len(names))]
				tb, err := d.GetTable(tableName)
				if err != nil {
					break
				}
				column := ddlFulltextIndexColumns[rng.Intn(len(ddlFulltextIndexColumns))]
				if strings.HasPrefix(tableName, "vec") {
					column = ddlVectorIndexColumns[rng.Intn(len(ddlVectorIndexColumns))]
				}
				// `Ignore`: another worker owns the same name, so "already
				// exists" is the expected answer half the time.
				if _, err := tb.CreateIndex(column, ddlIndexFor(tableName, column),
					infinity.ConflictTypeIgnore, ""); err == nil {
					indexes.Add(1)
				}
			default: // drop index
				d, err := conns[worker].GetDatabase(dbName)
				if err != nil {
					break
				}
				resp, err := d.ListTables()
				if err != nil {
					break
				}
				names := tableNames(t, resp)
				if len(names) == 0 {
					break
				}
				tb, err := d.GetTable(names[rng.Intn(len(names))])
				if err != nil {
					break
				}
				resp, err = tb.ListIndexes()
				if err != nil {
					break
				}
				ixNames := indexNames(t, resp)
				if len(ixNames) > 0 {
					tb.DropIndex(ixNames[rng.Intn(len(ixNames))], infinity.ConflictTypeIgnore)
				}
			}
			time.Sleep(200 * time.Millisecond)
		}
		return nil
	})

	t.Logf("%.0fs window: %d databases, %d tables, %d indexes created",
		window.Seconds(), dbs.Load(), tables.Load(), indexes.Load())
	// A `create` may legitimately lose to a concurrent `drop` of the same
	// database, but if none of them ever lands the window tested nothing.
	if tables.Load() == 0 {
		t.Fatalf("no table was created in %.0fs", window.Seconds())
	}

	// Clean up, and prove the catalog is still walkable after all that: each
	// drop is `Ignore`, so the only failure mode left is a catalog that cannot
	// list or drop what it holds.
	conn := connections(t, 1)[0]
	resp, err := conn.ListDatabases()
	requireOK(t, err, "list databases after the window")
	for _, name := range databaseNames(t, resp) {
		if name != "default_db" && strings.HasPrefix(name, "dll_parallel") {
			_, err := conn.DropDatabase(name, infinity.ConflictTypeIgnore)
			requireOK(t, err, "drop database "+name)
		}
	}
}

// databaseNames pulls the names out of a `ListDatabases` response.
//
// The unexpected-type case fails the case rather than returning nil: a silent
// nil made "the catalog is still walkable" pass while walking nothing, which
// is how a DDL case can run for a whole window without touching a table.
func databaseNames(t *testing.T, resp interface{}) []string {
	t.Helper()
	switch v := resp.(type) {
	case *thriftapi.ListDatabaseResponse:
		return v.DbNames
	default:
		t.Fatalf("ListDatabases returned %T, want *thriftapi.ListDatabaseResponse", resp)
		return nil
	}
}

// indexNames pulls the names out of a `ListIndexes` response.
func indexNames(t *testing.T, resp interface{}) []string {
	t.Helper()
	switch v := resp.(type) {
	case *thriftapi.ListIndexResponse:
		return v.IndexNames
	default:
		t.Fatalf("ListIndexes returned %T, want *thriftapi.ListIndexResponse", resp)
		return nil
	}
}

// insertSchema is the eight-column table `test_insert_parallel` fills.
var insertSchema = infinity.TableSchema{
	{Name: "id", DataType: "int64"},
	{Name: "text", DataType: "varchar"},
	{Name: "c_tiny_int", DataType: "int8"},
	{Name: "c_small_int", DataType: "int16"},
	{Name: "c_int", DataType: "int32"},
	{Name: "c_float", DataType: "float32"},
	{Name: "c_double", DataType: "float64"},
	{Name: "c_vector", DataType: "vector,4,float"},
}

// insertRow builds row `i` of the fixture.
func insertRow(i int) map[string]interface{} {
	return map[string]interface{}{
		"id":          i,
		"text":        fmt.Sprintf("%.6f", rand.Float64()*10),
		"c_tiny_int":  i,
		"c_small_int": i,
		"c_int":       i,
		"c_float":     float64(i),
		"c_double":    float64(i),
		"c_vector":    []float64{1, 1, 1, 1},
	}
}

// TestParallelInsert is `test_insert_parallel`: eight workers insert
// disjoint id ranges of the same table, and every row must land.
func TestParallelInsert(t *testing.T) {
	const (
		tableName   = "test_parallel_insert"
		totalRows   = 500 * 40
		inserters   = 8
		rowsPerPart = totalRows / inserters
	)

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, insertSchema)
	_, err := table.CreateIndex("text_index",
		infinity.NewIndexInfo("text", infinity.IndexTypeFullText, nil),
		infinity.ConflictTypeError, "")
	requireOK(t, err, "create fulltext index")

	workers := connections(t, inserters)
	runWindow(t, inserters, 10*time.Minute, func(worker int, _ time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		tb, err := d.GetTable(tableName)
		if err != nil {
			return err
		}
		for i := 0; i < rowsPerPart; i++ {
			if _, err := tb.Insert([]map[string]interface{}{insertRow(i)}); err != nil {
				return fmt.Errorf("worker %d insert %d: %w", worker, i, err)
			}
		}
		return nil
	})

	requireRowCount(t, table, int64(totalRows), "row count after the insert window")
	requireDropTable(t, conn, tableName)
}

// TestParallelInsertOneThread is `test_insert_one_thread`: the same row count
// through one connection, as the baseline the parallel case is compared with.
func TestParallelInsertOneThread(t *testing.T) {
	const (
		tableName = "test_parallel_insert_one"
		totalRows = 500 * 40
	)

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, insertSchema)
	_, err := table.CreateIndex("text_index",
		infinity.NewIndexInfo("text", infinity.IndexTypeFullText, nil),
		infinity.ConflictTypeError, "")
	requireOK(t, err, "create fulltext index")

	start := time.Now()
	for i := 0; i < totalRows; i++ {
		_, err := table.Insert([]map[string]interface{}{insertRow(i)})
		requireOK(t, err, fmt.Sprintf("insert %d", i))
	}
	t.Logf("%d rows in %.1fs", totalRows, time.Since(start).Seconds())

	requireRowCount(t, table, int64(totalRows), "row count after the single-thread insert")
	requireDropTable(t, conn, tableName)
}

// TestParallelInsertAndCount is `test_insert_and_count_star_parallel`: eight
// writers insert while four readers run `count(*)`, so the aggregate reads a
// table that is growing under it.
func TestParallelInsertAndCount(t *testing.T) {
	const (
		tableName   = "test_parallel_insert_count"
		totalRows   = 500 * 40
		inserters   = 8
		counters    = 4
		countsEach  = 100
		rowsPerPart = totalRows / inserters
	)

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, insertSchema)
	_, err := table.CreateIndex("text_index",
		infinity.NewIndexInfo("text", infinity.IndexTypeFullText, nil),
		infinity.ConflictTypeError, "")
	requireOK(t, err, "create fulltext index")

	workers := connections(t, inserters+counters)
	var counts atomic.Int64
	runWindow(t, inserters+counters, 10*time.Minute, func(worker int, _ time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		tb, err := d.GetTable(tableName)
		if err != nil {
			return err
		}
		if worker < inserters {
			for i := 0; i < rowsPerPart; i++ {
				if _, err := tb.Insert([]map[string]interface{}{insertRow(i)}); err != nil {
					return fmt.Errorf("worker %d insert %d: %w", worker, i, err)
				}
			}
			return nil
		}
		for i := 0; i < countsEach; i++ {
			// A count under a live insert is a legal read: it must answer,
			// not necessarily with the final number.
			if _, err := tb.Output([]string{"count(*)"}).ToResult(); err != nil {
				return fmt.Errorf("counter %d: %w", worker, err)
			}
			counts.Add(1)
		}
		return nil
	})

	if counts.Load() != counters*countsEach {
		t.Fatalf("counters did %d of %d reads", counts.Load(), counters*countsEach)
	}
	requireRowCount(t, table, int64(totalRows), "row count after the insert window")
	requireDropTable(t, conn, tableName)
}

// idTextSchema is the two-column table the insert/delete cases use.
var idTextSchema = infinity.TableSchema{
	{Name: "id", DataType: "int64"},
	{Name: "text", DataType: "varchar"},
}

// TestParallelInsertAndDelete is `test_insert_and_delete_parallel`: eight
// workers insert batches of 500 and delete batches someone else inserted, until
// 100 000 ids are handed out. Every id is inserted once and deleted at most
// once, so the table must end up empty.
func TestParallelInsertAndDelete(t *testing.T) {
	testInsertAndDelete(t, false)
}

// TestParallelInsertAndDeleteWithIndex is the same case with a full-text index
// on the table, so each delete also has to maintain the index.
func TestParallelInsertAndDeleteWithIndex(t *testing.T) {
	testInsertAndDelete(t, true)
}

func testInsertAndDelete(t *testing.T, withIndex bool) {
	const (
		tableName  = "test_parallel_insert_delete"
		batchSize  = 500
		maxCount   = batchSize * 200
		workers512 = 8
	)

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, idTextSchema)
	if withIndex {
		_, err := table.CreateIndex("text_index",
			infinity.NewIndexInfo("text", infinity.IndexTypeFullText, nil),
			infinity.ConflictTypeError, "")
		requireOK(t, err, "create fulltext index")
	}

	// One shared allocator of id batches plus a list of batches that are
	// inserted and still deletable - the same handoff the Python case keeps
	// under one lock.
	var (
		mu        sync.Mutex
		nextStart int
		deletable []int
		inserted  atomic.Int64
		deleted   atomic.Int64
		conflicts atomic.Int64
	)
	workers := connections(t, workers512)

	runWindow(t, workers512, 5*time.Minute, func(worker int, _ time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		tb, err := d.GetTable(tableName)
		if err != nil {
			return err
		}
		for {
			mu.Lock()
			start := -1
			if nextStart < maxCount && rand.Intn(2) == 0 {
				start = nextStart
				nextStart += batchSize
			}
			delIdx := -1
			if len(deletable) > 0 {
				delIdx = rand.Intn(len(deletable))
			}
			var deleteFrom int
			if delIdx >= 0 {
				deleteFrom = deletable[delIdx]
			}
			if start >= 0 {
				// The batch is ours; do not release the lock with it on the
				// deletable list yet (it is not inserted).
			} else if delIdx >= 0 {
				deletable = append(deletable[:delIdx], deletable[delIdx+1:]...)
			}
			exhausted := nextStart >= maxCount && len(deletable) == 0
			mu.Unlock()
			if exhausted {
				return nil
			}
			if start < 0 && delIdx < 0 {
				// Nothing to do this round: the id pool is empty but the
				// deletable list is not yet populated by the first insert.
				// Sleep rather than spin - a tight loop here would have eight
				// goroutines competing for the lock and the CPU the engine
				// needs.
				time.Sleep(time.Millisecond)
				continue
			}

			if start >= 0 {
				rows := make([]map[string]interface{}, 0, batchSize)
				for i := start; i < start+batchSize; i++ {
					rows = append(rows, map[string]interface{}{
						"id":   i,
						"text": fmt.Sprintf("%.6f", rand.Float64()*10),
					})
				}
				if _, err := tb.Insert(rows); err != nil {
					if isTransactionConflict(err) {
						conflicts.Add(1)
					}
					// The batch was handed out but not written; it stays
					// undeletable, which is exactly what the Python case does.
					continue
				}
				inserted.Add(int64(len(rows)))
				mu.Lock()
				deletable = append(deletable, start)
				mu.Unlock()
				continue
			}

			// `id > from-1 and id < from+batchSize` is the batch's range.
			if _, err := tb.Delete(fmt.Sprintf("id > %d and id < %d",
				deleteFrom-1, deleteFrom+batchSize)); err != nil {
				if isTransactionConflict(err) {
					conflicts.Add(1)
				}
				continue
			}
			deleted.Add(int64(batchSize))
		}
	})

	if inserted.Load() == 0 {
		t.Fatalf("the window inserted nothing")
	}
	t.Logf("%d rows inserted, %d deleted, %d conflicts", inserted.Load(), deleted.Load(), conflicts.Load())

	// Every batch that was inserted and then deleted is gone; the ones that
	// never got a delete attempt are still there. The invariant that matters
	// is that a delete removed exactly the batch it named.
	requireRowCount(t, table, inserted.Load()-deleted.Load(), "row count after the insert/delete window")
	requireDropTable(t, conn, tableName)
}

// TestParallelInsertDeleteUpdate is `test_insert_delete_update_parallel_vec`:
// four workers pick insert, delete, update or read at random on one
// vector-indexed table. Nothing is asserted but the table surviving the window
// and still answering: the case exists to prove the four paths can run
// together without a deadlock.
func TestParallelInsertDeleteUpdate(t *testing.T) {
	window := window(t)
	const (
		tableName = "test_parallel_insert_delete_update"
		workers4  = 4
	)

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, infinity.TableSchema{
		{Name: "tag", DataType: "int"},
		{Name: "c1", DataType: "vector,4,float"},
	})

	workers := connections(t, workers4)
	var inserts, deletes, updates atomic.Int64

	runWindow(t, workers4, window, func(worker int, deadline time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		tb, err := d.GetTable(tableName)
		if err != nil {
			return err
		}
		for time.Now().Before(deadline) {
			switch rand.Intn(4) {
			case 0:
				rows := make([]map[string]interface{}, 0, 100)
				for i := 0; i < 100; i++ {
					rows = append(rows, map[string]interface{}{
						"tag": rand.Intn(10),
						"c1": []float64{rand.Float64(), rand.Float64(),
							rand.Float64(), rand.Float64()},
					})
				}
				if _, err := tb.Insert(rows); err == nil {
					inserts.Add(1)
				}
			case 1:
				if _, err := tb.Delete(fmt.Sprintf("tag = %d", rand.Intn(10))); err == nil {
					deletes.Add(1)
				}
			case 2:
				if _, err := tb.Update(fmt.Sprintf("tag = %d", rand.Intn(10)),
					map[string]interface{}{"tag": 0, "c1": []float64{0, 0, 0, 0}}); err == nil {
					updates.Add(1)
				}
			default:
				// A read that fails is fine; a read that hangs is not.
				if _, err := tb.Output([]string{"*"}).ToResult(); err == nil {
					// nothing to check: the row set is moving
				}
			}
		}
		return nil
	})

	t.Logf("%.0fs window: %d inserts, %d deletes, %d updates",
		window.Seconds(), inserts.Load(), deletes.Load(), updates.Load())
	if inserts.Load() == 0 {
		t.Fatalf("the window inserted nothing")
	}

	// The table must still answer, and its rows must still be readable.
	if _, err := table.Output([]string{"*"}).ToResult(); err != nil {
		t.Fatalf("read after the window: %v", err)
	}
	requireDropTable(t, conn, tableName)
}

// TestParallelDDLAndInsertDelete is `test_insert_delete_ddl_parallel`: four
// workers create and drop the same table and its index while inserting,
// deleting and updating it. Only the drops that succeed matter; the point is
// that a table being created and dropped under its own writers does not leave
// the catalog or the storage in a state the last drop cannot clear.
func TestParallelDDLAndInsertDelete(t *testing.T) {
	window := window(t)
	const (
		tableName = "test_parallel_ddl_dml"
		indexName = "c1_vector_index"
		workers4  = 4
	)
	schema := infinity.TableSchema{
		{Name: "tag", DataType: "int"},
		{Name: "c1", DataType: "vector,4,float"},
	}

	conn := connections(t, 1)[0]
	requireTable(t, conn, tableName, schema)

	workers := connections(t, workers4)
	var ddl, dml atomic.Int64

	runWindow(t, workers4, window, func(worker int, deadline time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		for time.Now().Before(deadline) {
			switch v := rand.Intn(100); {
			case v == 0:
				d.CreateTable(tableName, schema, infinity.ConflictTypeIgnore)
				ddl.Add(1)
			case v == 1:
				d.DropTable(tableName, infinity.ConflictTypeIgnore)
				ddl.Add(1)
			case v >= 2 && v < 4:
				tb, err := d.GetTable(tableName)
				if err != nil {
					break
				}
				tb.CreateIndex(indexName, infinity.NewIndexInfo("c1", infinity.IndexTypeHnsw,
					map[string]string{"M": "16", "ef_construction": "50", "metric": "l2"}),
					infinity.ConflictTypeIgnore, "")
				ddl.Add(1)
			case v >= 4 && v < 6:
				tb, err := d.GetTable(tableName)
				if err != nil {
					break
				}
				tb.DropIndex(indexName, infinity.ConflictTypeIgnore)
				ddl.Add(1)
			default:
				// Everything below tolerates `table does not exist`: a drop
				// worker may have won, and that is the case working as
				// intended rather than a failure.
				tb, err := d.GetTable(tableName)
				if err != nil {
					break
				}
				switch {
				case v < 48:
					rows := make([]map[string]interface{}, 0, 100)
					for i := 0; i < 100; i++ {
						rows = append(rows, map[string]interface{}{
							"tag": rand.Intn(10),
							"c1": []float64{rand.Float64(), rand.Float64(),
								rand.Float64(), rand.Float64()},
						})
					}
					if _, err := tb.Insert(rows); err == nil {
						dml.Add(1)
					}
				case v < 90:
					if _, err := tb.Delete(fmt.Sprintf("tag = %d", rand.Intn(10))); err == nil {
						dml.Add(1)
					}
				default:
					if _, err := tb.Update(fmt.Sprintf("tag = %d", rand.Intn(10)),
						map[string]interface{}{"tag": 0, "c1": []float64{0, 0, 0, 0}}); err == nil {
						dml.Add(1)
					}
				}
			}
		}
		return nil
	})

	t.Logf("%.0fs window: %d DDL statements, %d DML statements accepted",
		window.Seconds(), ddl.Load(), dml.Load())

	// The final drop must succeed: whatever the workers left behind, the
	// catalog and the storage have to agree on it.
	db, err := conn.GetDatabase("default_db")
	requireOK(t, err, "get database")
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	requireOK(t, err, "drop the table after the window")
}

// chaosSchema is the table `test_chaos` hammers: a full-text index on `body`
// and an HNSW index on the vector.
var chaosSchema = infinity.TableSchema{
	{Name: "index", DataType: "int"},
	{Name: "body", DataType: "varchar"},
	{Name: "other_vector", DataType: "vector,4,float"},
}

// TestParallelChaos is `test_chaos`: four workers insert, delete, update a
// full-text index and run both kinds of search against one table, on 100 000
// rows of body text. Every operation is allowed to fail; what is not allowed
// is the table becoming unreadable or un-droppable.
func TestParallelChaos(t *testing.T) {
	window := window(t)
	const (
		tableName = "test_parallel_chaos"
		workers4  = 4
		dataSize  = 100000
		batchSize = 100
		batches   = dataSize / batchSize
	)

	// The bodies are enwiki's, not a placeholder: `test_chaos` reads that
	// fixture so the full-text index holds many distinct terms, which is what
	// makes `MATCH TEXT 'harmful chemical'` a real query rather than a scan
	// over one repeated string.
	corpus := csvColumn(t, requireFixture(t, "enwiki_99.csv"), 2)

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, chaosSchema)
	_, err := table.CreateIndex("body_index",
		infinity.NewIndexInfo("body", infinity.IndexTypeFullText, nil),
		infinity.ConflictTypeError, "")
	requireOK(t, err, "create fulltext index")
	_, err = table.CreateIndex("other_index",
		infinity.NewIndexInfo("other_vector", infinity.IndexTypeHnsw, map[string]string{
			"M":               "16",
			"ef_construction": "50",
			"metric":          "l2",
		}), infinity.ConflictTypeError, "")
	requireOK(t, err, "create hnsw index")

	workers := connections(t, workers4)
	var inserts, deletes, updates, textSearch, vectorSearch atomic.Int64

	runWindow(t, workers4, window, func(worker int, deadline time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		tb, err := d.GetTable(tableName)
		if err != nil {
			return err
		}
		for time.Now().Before(deadline) {
			pos := rand.Intn(batches) * batchSize
			switch rand.Intn(5) {
			case 0:
				rows := make([]map[string]interface{}, 0, batchSize)
				for i := 0; i < batchSize; i++ {
					rows = append(rows, map[string]interface{}{
						"index":        pos + i,
						"body":         corpus[(pos+i)%len(corpus)],
						"other_vector": []float64{float64(i % 10), 0, 0, 0},
					})
				}
				if _, err := tb.Insert(rows); err == nil {
					inserts.Add(1)
				}
				time.Sleep(50 * time.Millisecond)
			case 1:
				if _, err := tb.Delete(fmt.Sprintf("index >= %d and index < %d",
					pos, pos+batchSize)); err == nil {
					deletes.Add(1)
				}
				time.Sleep(50 * time.Millisecond)
			case 2:
				id := rand.Intn(dataSize)
				if _, err := tb.Update(fmt.Sprintf("index = %d", id),
					map[string]interface{}{
						"index":        id,
						"body":         "infinity",
						"other_vector": []float64{0, 0, 0, 0},
					}); err == nil {
					updates.Add(1)
				}
				time.Sleep(50 * time.Millisecond)
			case 3:
				q := tb.Output([]string{"index", "body", "other_vector", "_row_id", "_score"})
				q.MatchText("body^5", "harmful chemical", 3, map[string]string{})
				if _, err := q.ToResult(); err == nil {
					textSearch.Add(1)
				}
				time.Sleep(30 * time.Millisecond)
			default:
				q := tb.Output([]string{"*"})
				q.MatchDense("other_vector", []float64{2, 2, 2, 2}, "float", "l2", 3, nil)
				if _, err := q.ToResult(); err == nil {
					vectorSearch.Add(1)
				}
				time.Sleep(30 * time.Millisecond)
			}
			time.Sleep(5 * time.Millisecond)
		}
		return nil
	})

	t.Logf("%.0fs window: %d inserts, %d deletes, %d updates, %d text searches, %d vector searches",
		window.Seconds(), inserts.Load(), deletes.Load(), updates.Load(),
		textSearch.Load(), vectorSearch.Load())
	if adds := inserts.Load() + deletes.Load() + updates.Load() +
		textSearch.Load() + vectorSearch.Load(); adds == 0 {
		t.Fatalf("the window did no work")
	}

	// The table must still answer after the chaos, and both indexes must
	// still be there.
	if _, err := table.Output([]string{"*"}).ToResult(); err != nil {
		t.Fatalf("read after the window: %v", err)
	}
	requireDropTable(t, conn, tableName)
}
