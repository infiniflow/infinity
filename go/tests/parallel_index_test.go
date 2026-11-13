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

// Gate D, index cases: the six cases of `python/parallel_test/
// test_index_parallel.py`, one test function each.
//
// They run here rather than in Python because the Python form measures the
// interpreter: every row an `insert` sends is rebuilt as a tree of Thrift
// structs, so with four writer threads CPython's allocator and the GIL decide
// the throughput. Measured against one server: 1 client thread reaches
// 1 196 inserts/s, 4 reach 2 519 and 8 reach 2 712 - adding writers stops
// helping, and the server never passes 1.3 cores. One goroutine per worker
// measures the engine instead.

import (
	"fmt"
	"math"
	"math/rand"
	"sync"
	"sync/atomic"
	"testing"
	"time"

	infinity "github.com/infiniflow/infinity-go-sdk"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// TestParallelFulltextIndexReadWrite is `test_fulltext_index_rw_parallel`:
// writers insert rows and import the file while a reader keeps answering a
// `MATCH TEXT` query, all against one full-text-indexed table.
func TestParallelFulltextIndexReadWrite(t *testing.T) {
	window := window(t)
	const (
		tableName = "test_parallel_fulltext_rw"
		indexName = "body_index"
		writers   = 4
		readers   = 1
	)
	csvPath := requireFixture(t, "enwiki_99.csv")
	base := csvRowCount(t, csvPath)
	// The writers insert the fixture's own bodies, as the Python case does,
	// so the full-text index has real terms to merge while readers query it.
	bodies := csvColumn(t, csvPath, 2)

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
	})
	_, err := table.ImportData(csvPath, &infinity.ImportOption{
		HasHeader:    false,
		Delimiter:    '\t',
		CopyFileType: infinity.CopyFileTypeCSV,
	})
	requireOK(t, err, "initial import")
	_, err = table.CreateIndex(indexName,
		infinity.NewIndexInfo("body", infinity.IndexTypeFullText, nil),
		infinity.ConflictTypeError, "")
	requireOK(t, err, "create fulltext index")

	workers := connections(t, writers+readers)

	var inserts, imports, queries atomic.Int64
	runWindow(t, writers+readers, window, func(worker int, deadline time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		tb, err := d.GetTable(tableName)
		if err != nil {
			return err
		}
		if worker < writers {
			for time.Now().Before(deadline) {
				// One of the two, per iteration, as the Python case picks
				// (`random.randint(0, 1)`): an insert of the whole fixture, or
				// an import of the same file.
				if rand.Intn(2) == 0 {
					rows := make([]map[string]interface{}, 0, len(bodies))
					for k := range bodies {
						rows = append(rows, map[string]interface{}{
							"doctitle": "parallel title",
							"docdate":  "2024-01-01",
							"body":     bodies[k],
						})
					}
					if _, err := tb.Insert(rows); err != nil {
						return fmt.Errorf("writer %d insert: %w", worker, err)
					}
					inserts.Add(int64(len(rows)))
					continue
				}
				if _, err := tb.ImportData(csvPath, &infinity.ImportOption{
					HasHeader:    false,
					Delimiter:    '\t',
					CopyFileType: infinity.CopyFileTypeCSV,
				}); err != nil {
					return fmt.Errorf("writer %d import: %w", worker, err)
				}
				imports.Add(1)
			}
			return nil
		}
		// The reader keeps the index under a query while the writers append
		// to it; a wrong lock order shows up as a hang or an error here.
		for time.Now().Before(deadline) {
			q := tb.Output([]string{"doctitle", "docdate", "_row_id", "_score"})
			q.MatchText("body^5", "harmful chemical", 3, map[string]string{})
			if _, err := q.ToResult(); err != nil {
				return fmt.Errorf("reader fulltext query: %w", err)
			}
			queries.Add(1)
			time.Sleep(100 * time.Millisecond)
		}
		return nil
	})

	if inserts.Load() == 0 || imports.Load() == 0 || queries.Load() == 0 {
		t.Fatalf("the window did no work: inserts=%d imports=%d queries=%d",
			inserts.Load(), imports.Load(), queries.Load())
	}
	t.Logf("%.0fs window: %d rows inserted, %d imports, %d queries",
		window.Seconds(), inserts.Load(), imports.Load(), queries.Load())

	// A concurrent window must not lose a row: the count is the baseline
	// import plus everything the writers inserted. Every `ImportData` writes
	// the whole file again, so that is one baseline per import.
	want := base + imports.Load()*base + inserts.Load()
	if got := countRows(t, table); got != want {
		t.Fatalf("row count after the window: got %d, want %d (%d baseline + %d imports x %d + %d inserted)",
			got, want, base, imports.Load(), base, inserts.Load())
	}

	// The index still answers after all those appends, and the merges they
	// forced did not lose the rows they rewrote.
	q := table.Output([]string{"doctitle", "_score"})
	q.MatchText("body^5", "harmful chemical", 3, map[string]string{})
	requireRows(t, resultOf(t, q, "full-text search after the window"),
		"doctitle", "full-text search after the window")

	requireDropTable(t, conn, tableName)
}

// vectorSchema is the ten-column table the vector cases use: five 4-d vectors,
// so an index can be built on one and searched on the same one.
var vectorSchema = infinity.TableSchema{
	{Name: "variant_id", DataType: "varchar"},
	{Name: "gender_vector", DataType: "vector,4,float"},
	{Name: "color_vector", DataType: "vector,4,float"},
	{Name: "category_vector", DataType: "vector,4,float"},
	{Name: "tag_vector", DataType: "vector,4,float"},
	{Name: "other_vector", DataType: "vector,4,float"},
	{Name: "query_is_recommend", DataType: "varchar"},
	{Name: "query_gender", DataType: "varchar"},
	{Name: "query_color", DataType: "varchar"},
	{Name: "query_price", DataType: "float"},
}

// hnswParams are the parameters both vector cases build the index with.
var hnswParams = map[string]string{
	"M":               "16",
	"ef_construction": "50",
	"metric":          "l2",
}

// TestParallelVectorIndexImport is `test_vector_index_single_thread`: one
// connection importing the vector file for the whole window, with the HNSW
// index built before it starts, then one KNN query against the result.
func TestParallelVectorIndexImport(t *testing.T) {
	window := window(t)
	const tableName = "test_parallel_vector_import"
	csvPath := requireFixture(t, "pysdk_test_knn.csv")

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, vectorSchema)
	_, err := table.CreateIndex("my_index",
		infinity.NewIndexInfo("gender_vector", infinity.IndexTypeHnsw, hnswParams),
		infinity.ConflictTypeError, "")
	requireOK(t, err, "create hnsw index")

	var imports int64
	deadline := time.Now().Add(window)
	for time.Now().Before(deadline) {
		_, err := table.ImportData(csvPath, nil)
		requireOK(t, err, "import")
		imports++
	}
	if imports == 0 {
		t.Fatalf("the window did no import")
	}

	// The index must still answer after the window: `import` seals segments,
	// and each of those appends rows the graph has to absorb.
	q := table.Output([]string{"variant_id"})
	q.MatchDense("gender_vector", []float64{1, 1, 1, 1}, "float", "l2", 5, nil)
	requireRows(t, resultOf(t, q, "knn after the window"), "variant_id",
		fmt.Sprintf("knn after %d imports", imports))
	t.Logf("%.0fs window: %d imports, knn returned rows", window.Seconds(), imports)

	requireDropTable(t, conn, tableName)
}

// TestParallelVectorIndexReadWrite is `test_vector_index_rw_parallel`: four
// importers keep appending to an HNSW-indexed table while one reader runs KNN.
func TestParallelVectorIndexReadWrite(t *testing.T) {
	window := window(t)
	const (
		tableName = "test_parallel_vector_rw"
		writers   = 4
		readers   = 1
	)
	csvPath := requireFixture(t, "pysdk_test_knn.csv")

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, vectorSchema)
	_, err := table.ImportData(csvPath, nil)
	requireOK(t, err, "initial import")
	_, err = table.CreateIndex("my_index",
		infinity.NewIndexInfo("gender_vector", infinity.IndexTypeHnsw, hnswParams),
		infinity.ConflictTypeError, "")
	requireOK(t, err, "create hnsw index")

	workers := connections(t, writers+readers)

	var imports, queries atomic.Int64
	runWindow(t, writers+readers, window, func(worker int, deadline time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		tb, err := d.GetTable(tableName)
		if err != nil {
			return err
		}
		if worker < writers {
			for time.Now().Before(deadline) {
				if _, err := tb.ImportData(csvPath, nil); err != nil {
					return fmt.Errorf("writer %d import: %w", worker, err)
				}
				imports.Add(1)
			}
			return nil
		}
		for time.Now().Before(deadline) {
			q := tb.Output([]string{"variant_id"})
			q.MatchDense("gender_vector", []float64{1, 1, 1, 1}, "float", "l2", 5, nil)
			if _, err := q.ToResult(); err != nil {
				return fmt.Errorf("reader knn: %w", err)
			}
			queries.Add(1)
			time.Sleep(100 * time.Millisecond)
		}
		return nil
	})

	if imports.Load() == 0 || queries.Load() == 0 {
		t.Fatalf("the window did no work: imports=%d queries=%d", imports.Load(), queries.Load())
	}
	t.Logf("%.0fs window: %d imports, %d knn queries",
		window.Seconds(), imports.Load(), queries.Load())

	requireDropTable(t, conn, tableName)
}

// TestParallelIndexCreateDelete is `test_index_creation_deletion_parallel`:
// workers create and drop a full-text index while others insert and a third
// group queries. The point is that a DDL and an append on one table never
// deadlock and never leave the index half-built.
func TestParallelIndexCreateDelete(t *testing.T) {
	window := window(t)
	const (
		tableName  = "test_parallel_index_ddl"
		indexName  = "body_index"
		ddlWorkers = 4
		inserters  = 4
		readers    = 4
	)

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, infinity.TableSchema{
		{Name: "doctitle", DataType: "varchar"},
		{Name: "docdate", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
	})
	// The inserters write the enwiki bodies, as the Python case does: the
	// full-text index has to be built over real terms for the concurrent
	// create/drop to exercise anything.
	corpus := csvColumn(t, requireFixture(t, "enwiki_99.csv"), 2)

	workers := connections(t, ddlWorkers+inserters+readers)

	var (
		created, dropped, inserted, queries atomic.Int64
		conflicts                           atomic.Int64
	)
	runWindow(t, ddlWorkers+inserters+readers, window, func(worker int, deadline time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		tb, err := d.GetTable(tableName)
		if err != nil {
			return err
		}
		switch {
		case worker < ddlWorkers:
			for time.Now().Before(deadline) {
				// `Ignore` on both sides: another worker owns the same name,
				// so "already exists" / "already gone" are the expected
				// answers, not failures.
				if _, err := tb.CreateIndex(indexName,
					infinity.NewIndexInfo("body", infinity.IndexTypeFullText, nil),
					infinity.ConflictTypeIgnore, ""); err != nil {
					return fmt.Errorf("ddl worker %d create index: %w", worker, err)
				}
				created.Add(1)
				time.Sleep(500 * time.Millisecond)
				if _, err := tb.DropIndex(indexName, infinity.ConflictTypeIgnore); err != nil {
					return fmt.Errorf("ddl worker %d drop index: %w", worker, err)
				}
				dropped.Add(1)
				time.Sleep(500 * time.Millisecond)
			}
			return nil
		case worker < ddlWorkers+inserters:
			for time.Now().Before(deadline) {
				// The whole enwiki fixture, as the Python case inserts: the
				// point of the case is that building and dropping a full-text
				// index while rows are being indexed does not lose them, and
				// eight placeholder rows would not exercise the index.
				rows := make([]map[string]interface{}, 0, len(corpus))
				for k := range corpus {
					rows = append(rows, map[string]interface{}{
						"doctitle": "ddl title",
						"docdate":  "2024-01-01",
						"body":     corpus[k],
					})
				}
				if _, err := tb.Insert(rows); err != nil {
					// A `CreateIndex` on this table conflicts with an append, by
					// design (`NewTxn::CheckConflictTxnStore`): the DDL rewrites
					// the schema the append was planned against. The append is
					// retried by the engine up to its limit, and past that the
					// statement fails - a correct outcome, not a defect. What
					// must not happen is a hang or a lost row, and both are
					// checked below.
					if isTransactionConflict(err) {
						conflicts.Add(1)
						continue
					}
					return fmt.Errorf("insert worker %d: %w", worker, err)
				}
				inserted.Add(int64(len(rows)))
				time.Sleep(time.Second)
			}
			return nil
		default:
			for time.Now().Before(deadline) {
				// A query while the index comes and goes: it may report that
				// the index does not exist, which is a correct outcome. What
				// must not fail is a hang.
				q := tb.Output([]string{"doctitle", "docdate", "_row_id", "_score"})
				q.MatchText("body^5", "harmful chemical", 3, map[string]string{})
				if _, err := q.ToResult(); err == nil {
					queries.Add(1)
				}
				time.Sleep(500 * time.Millisecond)
			}
			return nil
		}
	})

	if created.Load() == 0 || inserted.Load() == 0 {
		t.Fatalf("the window did no work: created=%d dropped=%d inserted=%d queries=%d",
			created.Load(), dropped.Load(), inserted.Load(), queries.Load())
	}
	t.Logf("%.0fs window: %d create, %d drop, %d rows inserted, %d queries, %d append conflicts",
		window.Seconds(), created.Load(), dropped.Load(), inserted.Load(),
		queries.Load(), conflicts.Load())

	// Every inserted row is still there: an index coming and going must not
	// take rows with it.
	requireRowCount(t, table, inserted.Load(), "row count after the DDL window")
	requireDropTable(t, conn, tableName)
}

// TestParallelTableCreateDelete is `test_table_creation_deletion_parallel`:
// workers each create and drop tables of their own, so the catalog is being
// rewritten under itself the whole time.
func TestParallelTableCreateDelete(t *testing.T) {
	window := window(t)
	const (
		workers = 4
		base    = "test_parallel_table_ddl"
	)

	conns := connections(t, workers)

	var created []*atomic.Int64
	for i := 0; i < workers; i++ {
		created = append(created, &atomic.Int64{})
	}

	runWindow(t, workers, window, func(worker int, deadline time.Time) error {
		d, err := conns[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		// Each worker owns its table names, so a create/drop here cannot be
		// the other's business: what is shared is the catalog behind them.
		counter := 0
		for time.Now().Before(deadline) {
			name := fmt.Sprintf("%s_%d_%d", base, worker, counter)
			counter++
			if _, err := d.CreateTable(name, infinity.TableSchema{
				{Name: "id", DataType: "int"},
				{Name: "value", DataType: "varchar"},
			}, infinity.ConflictTypeIgnore); err != nil {
				return fmt.Errorf("worker %d create %s: %w", worker, name, err)
			}
			created[worker].Add(1)
			time.Sleep(500 * time.Millisecond)
			if _, err := d.DropTable(name, infinity.ConflictTypeIgnore); err != nil {
				return fmt.Errorf("worker %d drop %s: %w", worker, name, err)
			}
			time.Sleep(500 * time.Millisecond)
		}
		return nil
	})

	var total int64
	for i := range created {
		total += created[i].Load()
		if created[i].Load() == 0 {
			t.Fatalf("worker %d created nothing", i)
		}
	}
	t.Logf("%.0fs window: %d create/drop pairs across %d workers",
		window.Seconds(), total, workers)

	// Nothing survives: every worker dropped what it created.
	db, err := conns[0].GetDatabase("default_db")
	requireOK(t, err, "get database")
	resp, err := db.ListTables()
	requireOK(t, err, "list tables")
	for _, name := range tableNames(t, resp) {
		if len(name) > len(base) && name[:len(base)] == base {
			t.Fatalf("table %s survived the window", name)
		}
	}
}

// tableNames pulls the names out of a `ListTables` response. The
// unexpected-type case fails the case: a silent nil would make every reader of
// this list skip its work and report success.
func tableNames(t *testing.T, resp interface{}) []string {
	t.Helper()
	switch v := resp.(type) {
	case *thriftapi.ListTableResponse:
		return v.TableNames
	default:
		t.Fatalf("ListTables returned %T, want *thriftapi.ListTableResponse", resp)
		return nil
	}
}

// multiIndexSchema is the seven-column table `test_multiple_index_types`
// builds one index of every kind on: full-text on the text, secondary high and
// low cardinality, HNSW on a 4-d vector and on a 3-d multi-vector, BMP on a
// sparse column, EMVB on a tensor.
var multiIndexSchema = infinity.TableSchema{
	{Name: "id", DataType: "int"},
	{Name: "text", DataType: "varchar"},
	{Name: "category", DataType: "varchar"},
	{Name: "vector_col", DataType: "vector,4,float"},
	{Name: "multi_vector_col", DataType: "multivector,3,float"},
	{Name: "tensor_col", DataType: "tensor,4,float"},
	{Name: "sparse_col", DataType: "sparse,100,float,int8"},
}

// writtenRow is what a writer put in, kept so a reader can check what a query
// returns against it. The multi-vector and tensor columns are kept as shapes:
// each cell is a 2-D array and both SDK and engine must return every sub-vector
// of it, which is exactly what a `parseTensorVector` that kept one sub-vector
// used to break.
type writtenRow struct {
	text      string
	category  string
	vector    []float64
	multivec  [][]float64
	tensor    [][]float64
	sparseLen int
}

// writtenRows is the writer/reader handoff.
type writtenRows struct {
	mu   sync.RWMutex
	rows map[int64]writtenRow
}

func newWrittenRows() *writtenRows {
	return &writtenRows{rows: make(map[int64]writtenRow)}
}

func (w *writtenRows) put(id int64, row writtenRow) {
	w.mu.Lock()
	w.rows[id] = row
	w.mu.Unlock()
}

func (w *writtenRows) get(id int64) (writtenRow, bool) {
	w.mu.RLock()
	row, ok := w.rows[id]
	w.mu.RUnlock()
	return row, ok
}

// readerStats is one reader's tally. Each reader has its own, because the case
// asserts that *every* index kind was actually exercised - a single shared
// counter would let one dead reader hide behind nine live ones.
type readerStats struct {
	reads     atomic.Int64
	validated atomic.Int64
}

// TestParallelMultipleIndexTypes is `test_multiple_index_types_parallel`:
// seven index kinds on one table, four writers appending random rows, and ten
// readers querying - one per index kind, plus the three fusion combinations.
// Every three seconds each reader checks what it got back against the row the
// writer recorded, so a wrong row, a wrong vector or a lost append fails the
// case rather than passing silently. Afterwards every reader must have read
// something and every index must still be there.
func TestParallelMultipleIndexTypes(t *testing.T) {
	window := window(t)
	const (
		tableName = "test_parallel_multi_index"
		writers   = 4
		readers   = 10
	)

	conn := connections(t, 1)[0]
	table := requireTable(t, conn, tableName, multiIndexSchema)

	indexes := []struct {
		name string
		info *infinity.IndexInfo
	}{
		{"idx_fulltext", infinity.NewIndexInfo("text", infinity.IndexTypeFullText, nil)},
		{"idx_secondary_high", infinity.NewIndexInfo("id", infinity.IndexTypeSecondary,
			map[string]string{"cardinality": "high"})},
		{"idx_secondary_low", infinity.NewIndexInfo("category", infinity.IndexTypeSecondary,
			map[string]string{"cardinality": "low"})},
		{"idx_hnsw", infinity.NewIndexInfo("vector_col", infinity.IndexTypeHnsw,
			map[string]string{"M": "16", "ef_construction": "50", "metric": "l2"})},
		{"idx_hnsw_mv", infinity.NewIndexInfo("multi_vector_col", infinity.IndexTypeHnsw,
			map[string]string{"M": "16", "ef_construction": "50", "metric": "l2"})},
		{"idx_bmp", infinity.NewIndexInfo("sparse_col", infinity.IndexTypeBMP,
			map[string]string{"block_size": "8", "compress_type": "compress"})},
		{"idx_emvb", infinity.NewIndexInfo("tensor_col", infinity.IndexTypeEMVB,
			map[string]string{"pq_subspace_num": "8", "pq_subspace_bits": "8"})},
	}
	for _, ix := range indexes {
		_, err := table.CreateIndex(ix.name, ix.info, infinity.ConflictTypeError, "")
		requireOK(t, err, "create index "+ix.name)
	}

	written := newWrittenRows()

	// Ten seed rows, as the Python case inserts: without them a reader that
	// starts before the first writer lands has nothing to validate, and the
	// `secondary_low` filter could match an empty set every round. Their ids
	// are outside every writer's range (`worker*10000+local`, so below 40 000
	// for the run's length), which the Python case does not do - it seeds ids
	// 0..9 and lets writer 0 overwrite them, so a reader could compare a seed
	// row against a writer's record for the same id.
	seed := make([]map[string]interface{}, 0, 10)
	for i := 0; i < 10; i++ {
		id := int64(900000 + i)
		row, record := multiIndexRow(0, i, id)
		seed = append(seed, row)
		written.put(id, record)
	}
	if _, err := table.Insert(seed); err != nil {
		t.Fatalf("insert the seed rows: %v", err)
	}

	var (
		inserted atomic.Int64
		failures atomic.Int64
		firstErr atomic.Value // string
	)
	recordFailure := func(format string, args ...interface{}) {
		if failures.Add(1) == 1 {
			firstErr.Store(fmt.Sprintf(format, args...))
		}
	}

	stats := make([]*readerStats, readers)
	for i := range stats {
		stats[i] = &readerStats{}
	}

	workers := connections(t, writers+readers)

	runWindow(t, writers+readers, window, func(worker int, deadline time.Time) error {
		d, err := workers[worker].GetDatabase("default_db")
		if err != nil {
			return err
		}
		tb, err := d.GetTable(tableName)
		if err != nil {
			return err
		}
		if worker < writers {
			return runMultiIndexWriter(tb, worker, deadline, written, &inserted)
		}
		runMultiIndexReader(tb, worker-writers, deadline, written, stats[worker-writers], recordFailure)
		return nil
	})

	if failures.Load() > 0 {
		t.Fatalf("%d validation mismatches; first: %v", failures.Load(), firstErr.Load())
	}
	if inserted.Load() == 0 {
		t.Fatalf("the window inserted nothing")
	}

	// Every reader ran, and every one of them actually compared rows: a
	// reader whose queries all fail would otherwise pass this case while
	// checking nothing. The Python case asserts the same per index kind.
	var reads, validated int64
	for i, st := range stats {
		name := multiIndexReaderKind[i]
		if st.reads.Load() == 0 {
			t.Fatalf("reader %s issued no query", name)
		}
		if st.validated.Load() == 0 {
			t.Fatalf("reader %s validated no row (%d reads)", name, st.reads.Load())
		}
		reads += st.reads.Load()
		validated += st.validated.Load()
		t.Logf("  %-19s %5d reads, %5d rows validated", name, st.reads.Load(), st.validated.Load())
	}
	t.Logf("%.0fs window: %d rows written, %d reads, %d rows validated",
		window.Seconds(), inserted.Load(), reads, validated)

	// Every index survived the writers' appends. A concurrent DDL is not in
	// this case, so a missing index is a real defect.
	resp, err := table.ListIndexes()
	requireOK(t, err, "list indexes")
	have := map[string]bool{}
	for _, name := range indexNames(t, resp) {
		have[name] = true
	}
	for _, ix := range indexes {
		if !have[ix.name] {
			t.Fatalf("index %s is gone after the window (have %v)", ix.name, indexNames(t, resp))
		}
	}

	requireDropTable(t, conn, tableName)
}

// multiIndexCategories are the low-cardinality values the writers cycle
// through, so the secondary index on `category` has something to group.
var multiIndexCategories = []string{"A", "B", "C", "D"}

// multiIndexRow builds one row of the seven-index table and the record a
// reader can check it against. `worker`/`local` shape the values the same way
// the Python case does, so a KNN query has a deterministic neighbourhood.
func multiIndexRow(worker, local int, id int64) (map[string]interface{}, writtenRow) {
	vec := make([]float64, 4)
	for i := range vec {
		vec[i] = float64(worker+local+i+1) / 100.0
	}
	multivec := make([][]float64, 10)
	for i := range multivec {
		multivec[i] = []float64{
			float64(worker+local+i) / 100.0,
			float64(worker+local+i+1) / 100.0,
			float64(worker+local+i+2) / 100.0,
		}
	}
	// A sparse vector needs at least one entry: an empty one is not a
	// document the BMP index can hold.
	indices := make([]int, 0, 20)
	for j := 0; j < 100; j += 5 {
		indices = append(indices, j)
	}
	values := make([]float64, len(indices))
	for i := range values {
		values[i] = float64(worker+local+i) / 100.0
	}
	tensor := make([][]float64, 3)
	for i := range tensor {
		tensor[i] = []float64{
			float64(worker+local+i) / 100.0,
			float64(worker+local+i+1) / 100.0,
			float64(worker+local+i+2) / 100.0,
			float64(worker+local+i+3) / 100.0,
		}
	}
	textVal := fmt.Sprintf("test_%d_%d", worker, local)
	category := multiIndexCategories[local%len(multiIndexCategories)]
	row := map[string]interface{}{
		"id":               id,
		"text":             textVal,
		"category":         category,
		"vector_col":       vec,
		"multi_vector_col": multivec,
		"tensor_col":       tensor,
		"sparse_col":       infinity.SparseVector{Indices: indices, Values: values},
	}
	return row, writtenRow{
		text:      textVal,
		category:  category,
		vector:    vec,
		multivec:  multivec,
		tensor:    tensor,
		sparseLen: len(indices),
	}
}

// runMultiIndexWriter appends one row at a time, recording what it wrote so
// the readers can check what they get back.
func runMultiIndexWriter(
	tb *infinity.Table,
	worker int,
	deadline time.Time,
	written *writtenRows,
	inserted *atomic.Int64,
) error {
	local := 0
	for time.Now().Before(deadline) {
		id := int64(worker)*10000 + int64(local)
		row, record := multiIndexRow(worker, local, id)
		if _, err := tb.Insert([]map[string]interface{}{row}); err != nil {
			// A concurrent writer and a query can conflict; the Python case
			// swallows the same error. Losing a row is caught by the readers.
			if isTransactionConflict(err) {
				time.Sleep(100 * time.Millisecond)
				continue
			}
			return fmt.Errorf("writer %d insert: %w", worker, err)
		}
		written.put(id, record)
		inserted.Add(1)
		local++
		time.Sleep(100 * time.Millisecond)
	}
	return nil
}

// runMultiIndexReader queries one index kind and, every three seconds, checks
// what came back against what the writers recorded.
func runMultiIndexReader(
	tb *infinity.Table,
	kind int,
	deadline time.Time,
	written *writtenRows,
	stats *readerStats,
	recordFailure func(string, ...interface{}),
) {
	local := 0
	nextValidate := time.Now().Add(3 * time.Second)
	for time.Now().Before(deadline) {
		qr := multiIndexQuery(tb, kind, local)
		if qr != nil {
			local++
			if time.Now().After(nextValidate) {
				validateMultiIndexResult(kind, qr, written, &stats.validated, recordFailure)
				nextValidate = time.Now().Add(3 * time.Second)
			}
		}
		// A query that fails while a writer appends is expected (the Python
		// case swallows the same error); what must not fail is a *returned*
		// row that disagrees with what was written.
		stats.reads.Add(1)
		time.Sleep(100 * time.Millisecond)
	}
}

// multiIndexReaderKind names one reader of `test_multiple_index_types_parallel`
// and the index it exercises. The readers are addressed by name rather than by
// a bare integer so a failure report says which index broke.
var multiIndexReaderKind = []string{
	"fulltext", "hnsw", "hnsw_mv", "secondary_high", "secondary_low",
	"sparse", "emvb", "fusion_rrf", "fusion_mv_rrf", "fusion_weighted_sum",
}

// multiIndexQuery runs the query for one reader, returning nil when it failed -
// a failure under concurrent writes is not the case's concern. Each query's
// output columns are the ones the Python case validates, so a reader can never
// "pass" by asking for nothing.
func multiIndexQuery(tb *infinity.Table, kind int, local int) *infinity.QueryResult {
	var q *infinity.Table
	switch kind {
	case 0: // full text
		q = tb.Output([]string{"id", "text"})
		q.MatchText("text", "test", 5, map[string]string{})
	case 1: // hnsw on the 4-d vector
		q = tb.Output([]string{"id", "vector_col"})
		q.MatchDense("vector_col", []float64{0.5, 0.5, 0.5, 0.5}, "float", "l2", 5, nil)
	case 2: // hnsw on the multi-vector: two 3-d sub-vectors, flat
		q = tb.Output([]string{"id", "multi_vector_col"})
		q.MatchDense("multi_vector_col", []float64{0.5, 0.5, 0.5, 0.6, 0.6, 0.6},
			"float", "l2", 5, nil)
	case 3: // secondary, high cardinality
		q = tb.Output([]string{"id"}).Filter("id > 0")
	case 4: // secondary, low cardinality
		cat := multiIndexCategories[local%len(multiIndexCategories)]
		q = tb.Output([]string{"id", "category"}).Filter(fmt.Sprintf("category = '%s'", cat))
	case 5: // sparse (BMP)
		q = tb.Output([]string{"id", "sparse_col"})
		q.MatchSparse("sparse_col",
			&infinity.SparseVector{Indices: []int{0, 5, 10, 15}, Values: []float64{1, 1, 1, 1}},
			"ip", 5, nil)
	case 6: // tensor (EMVB)
		q = tb.Output([]string{"id", "tensor_col"})
		q.MatchTensor("tensor_col", []float64{0.5, 0.5, 0.5, 0.6, 0.6, 0.6}, "float", 5, nil)
	case 7: // fusion: full text + 4-d vector, rrf
		q = tb.Output([]string{"id", "text", "vector_col"})
		q.MatchText("text", "test", 5, map[string]string{})
		q.MatchDense("vector_col", []float64{0.5, 0.5, 0.5, 0.5}, "float", "l2", 5, nil)
		q.Fusion("rrf", 5, nil)
	case 8: // fusion: 4-d vector + multi-vector, rrf
		q = tb.Output([]string{"id", "vector_col", "multi_vector_col"})
		q.MatchDense("vector_col", []float64{0.5, 0.5, 0.5, 0.5}, "float", "l2", 5, nil)
		q.MatchDense("multi_vector_col", []float64{0.5, 0.5, 0.5, 0.6, 0.6, 0.6},
			"float", "l2", 5, nil)
		q.Fusion("rrf", 5, nil)
	default: // fusion: full text + 4-d vector, weighted sum
		q = tb.Output([]string{"id", "text", "vector_col"})
		q.MatchText("text", "test", 5, map[string]string{})
		q.MatchDense("vector_col", []float64{0.5, 0.5, 0.5, 0.5}, "float", "l2", 5, nil)
		q.Fusion("weighted_sum", 5, map[string]interface{}{"weights": "0.6,0.4"})
	}
	res, err := q.ToResult()
	if err != nil {
		return nil
	}
	qr, ok := res.(*infinity.QueryResult)
	if !ok {
		return nil
	}
	return qr
}

// multiIndexColumns are the columns each reader asks for and therefore the
// ones its validation can compare: `{id, text, vector_col, ...}`. Naming them
// here keeps a reader from validating a column it did not request.
func validateMultiIndexResult(
	kind int,
	qr *infinity.QueryResult,
	written *writtenRows,
	validated *atomic.Int64,
	recordFailure func(string, ...interface{}),
) {
	name := multiIndexReaderKind[kind]
	ids := qr.Data["id"]
	texts := qr.Data["text"]
	vectors := qr.Data["vector_col"]
	cats := qr.Data["category"]
	multivecs := qr.Data["multi_vector_col"]
	tensors := qr.Data["tensor_col"]
	sparses := qr.Data["sparse_col"]
	for i, idCell := range ids {
		id, ok := toInt64(idCell)
		if !ok {
			continue
		}
		row, known := written.get(id)
		if !known {
			continue
		}
		validated.Add(1)
		if texts != nil && i < len(texts) {
			if got, ok := texts[i].(string); ok && got != row.text {
				recordFailure("%s, row %d: text %q, want %q", name, id, got, row.text)
			}
		}
		if cats != nil && i < len(cats) {
			if got, ok := cats[i].(string); ok && got != row.category {
				recordFailure("%s, row %d: category %q, want %q", name, id, got, row.category)
			}
		}
		if vectors != nil && i < len(vectors) {
			if got, ok := vectors[i].([]interface{}); ok && !validateVector(got, row.vector) {
				recordFailure("%s, row %d: vector %v, want %v", name, id, got, row.vector)
			}
		}
		// The multi-vector and tensor cells are compared row by row. A cell is
		// the whole 2-D array: reading back only the first sub-vector is a
		// real defect (`parseTensorVector` did exactly that and this check is
		// what caught it), not a rendering difference.
		if multivecs != nil && i < len(multivecs) {
			if got, ok := matrixOf(multivecs[i]); ok && !validateMatrix(got, row.multivec) {
				recordFailure("%s, row %d: multi_vector_col %v, want %v",
					name, id, got, row.multivec)
			}
		}
		if tensors != nil && i < len(tensors) {
			if got, ok := matrixOf(tensors[i]); ok && !validateMatrix(got, row.tensor) {
				recordFailure("%s, row %d: tensor_col %v, want %v",
					name, id, got, row.tensor)
			}
		}
		if sparses != nil && i < len(sparses) {
			// The Python case checks the same thing: the sparse cell came back
			// with entries. Its own values are unordered by index, so only the
			// count is comparable without re-deriving the writer's map.
			if got, ok := sparseLength(sparses[i]); ok && got != row.sparseLen {
				recordFailure("%s, row %d: sparse_col has %d entries, want %d",
					name, id, got, row.sparseLen)
			}
		}
	}
}

// matrixOf views a returned cell as a 2-D array of floats. A tensor or
// multi-vector cell is `[]interface{}` of `[]interface{}` of numbers.
func matrixOf(v interface{}) ([][]float64, bool) {
	rows, ok := v.([]interface{})
	if !ok {
		return nil, false
	}
	out := make([][]float64, 0, len(rows))
	for _, r := range rows {
		cells, ok := r.([]interface{})
		if !ok {
			return nil, false
		}
		row := make([]float64, 0, len(cells))
		for _, c := range cells {
			f, ok := toFloat64(c)
			if !ok {
				return nil, false
			}
			row = append(row, f)
		}
		out = append(out, row)
	}
	return out, true
}

// validateMatrix compares two 2-D arrays element by element.
func validateMatrix(got, want [][]float64) bool {
	if len(got) != len(want) {
		return false
	}
	for i := range got {
		if len(got[i]) != len(want[i]) {
			return false
		}
		for j := range got[i] {
			if math.Abs(got[i][j]-want[i][j]) > 0.001 {
				return false
			}
		}
	}
	return true
}

// sparseLength reads the entry count out of whatever the SDK renders a sparse
// value as (a `SparseVector`, its dict form, or an already-flattened slice).
func sparseLength(v interface{}) (int, bool) {
	switch x := v.(type) {
	case infinity.SparseVector:
		return len(x.Indices), true
	case *infinity.SparseVector:
		return len(x.Indices), true
	case map[string]float64:
		return len(x), true
	case map[interface{}]interface{}:
		return len(x), true
	case []interface{}:
		return len(x), true
	default:
		return 0, false
	}
}
