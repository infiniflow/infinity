package loadtest

// End-to-end load test against a running Infinity server.
//
// This is the executable form of docs/load_test_plan_ragflow_prod.md: it walks
// P0 (create) -> P1 (ingest) -> P3/P4/P5 (query phases) -> P7 (readers plus
// writers) -> P8 (teardown), and prints a report per phase. Every phase is
// gated by an environment variable so the cheap ones can run in CI and the L
// scenario can be started deliberately:
//
//	LT_SERVER=127.0.0.1:28317 \
//	LT_MAPPING=/path/to/ragflow/conf/infinity_mapping.json \
//	LT_MODEL=/path/to/model2vec-1024 \
//	LT_SCENARIO=S \
//	go test -v -timeout 6h -run TestLoadTest ./...
//
// LT_SCENARIO may be S / M / L / XL, or "S:10x200" for a custom table count and
// rows per table. LT_TABLES / LT_ROWS override either.

import (
	"context"
	"fmt"
	"math/rand"
	"os"
	"runtime"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
	"testing"
	"time"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

// config is everything the run needs, read from the environment.
type config struct {
	Server    string
	Mapping   string
	Model     string
	Scenario  Scenario
	Seed      int64
	Queries   int
	Sparsegram bool
	Skew      bool
	IndexLast bool // P1b: load first, then build indexes
	Batches   int  // concurrent ingest workers
	LimitTables int // 0 = all tables in the scenario
}

func loadConfig(t *testing.T) config {
	t.Helper()
	server := os.Getenv("LT_SERVER")
	if server == "" {
		t.Skip("set LT_SERVER=host:port to run the load test")
	}
	mapping := os.Getenv("LT_MAPPING")
	if mapping == "" {
		t.Fatal("LT_MAPPING must point at RAGFlow's conf/infinity_mapping.json")
	}
	model := os.Getenv("LT_MODEL")
	if model == "" {
		t.Fatal("LT_MODEL must point at the pinned model2vec model directory")
	}

	scName := getenvDefault("LT_SCENARIO", "S")
	sc, err := parseScenario(scName)
	if err != nil {
		t.Fatalf("LT_SCENARIO: %v", err)
	}
	if v := os.Getenv("LT_TABLES"); v != "" {
		n, err := strconv.Atoi(v)
		if err != nil {
			t.Fatalf("LT_TABLES: %v", err)
		}
		sc.Tables = n
		if os.Getenv("LT_ROWS") == "" {
			sc.Rows = n * 1000
		}
	}
	if v := os.Getenv("LT_ROWS"); v != "" {
		n, err := strconv.Atoi(v)
		if err != nil {
			t.Fatalf("LT_ROWS: %v", err)
		}
		sc.Rows = n
	}

	return config{
		Server:   server,
		Mapping:  mapping,
		Model:    model,
		Scenario: sc,
		Seed:     int64(getenvInt("LT_SEED", 20261124)),
		Queries:  getenvInt("LT_QUERIES", 200),
		Sparsegram: os.Getenv("LT_SPARSEGRAM") != "",
		Skew:     os.Getenv("LT_SKEW") != "",
		IndexLast: os.Getenv("LT_INDEX_LAST") != "",
		Batches:  getenvInt("LT_BATCHES", 4),
		// LimitTables trims the plan to the first N tables. It exists so a run
		// fits inside a window where the machine is not reclaimed underneath it;
		// it is not a way to measure a smaller workload (the per-table shape is
		// unchanged, so the numbers stay comparable).
		LimitTables: getenvInt("LT_LIMIT_TABLES", 0),
	}
}

// parseScenario accepts "S", "M", "L", "XL" or "S:10x200".
//
// A trailing scale is also accepted: "L/100" means the L scenario with both the
// table count and the row count divided by 100, so a smoke run keeps the same
// shape (rows per table, rows per document) at 1% of the size. That matters
// because the interesting properties here are per-table and per-row ratios, not
// the totals — scaling only the row count would collapse every table to a
// handful of rows and measure something else.
func parseScenario(s string) (Scenario, error) {
	scale := 1
	if i := strings.LastIndex(s, "/"); i >= 0 {
		v := s[i+1:]
		n, err := strconv.Atoi(v)
		if err != nil || n < 1 {
			return Scenario{}, fmt.Errorf("bad scale in %q: expected <name>/<divisor>", s)
		}
		scale = n
		s = s[:i]
	}

	sc, custom, err := parseScenarioBase(s)
	if err != nil {
		return Scenario{}, err
	}
	if scale > 1 {
		// Custom "NxM" shapes already say exactly what they want; only the named
		// scenarios get scaled, and the original name is kept with the factor so
		// a report cannot be mistaken for a full-size run.
		if custom {
			return Scenario{}, fmt.Errorf("scale is only for named scenarios (S/M/L/XL), not %q", s)
		}
		sc.Tables = max(1, sc.Tables/scale)
		sc.Rows = max(1, sc.Rows/scale)
		sc.Name = fmt.Sprintf("%s/%d", sc.Name, scale)
	}
	return sc, nil
}

func parseScenarioBase(s string) (Scenario, bool, error) {
	if base := strings.SplitN(s, ":", 2); len(base) == 2 {
		var tables, rows int
		if _, err := fmt.Sscanf(base[1], "%dx%d", &tables, &rows); err != nil {
			return Scenario{}, false, fmt.Errorf("expected <name>:<tables>x<rows>, got %q", s)
		}
		return Scenario{Name: base[0], Tables: tables, Rows: tables * rows}, true, nil
	}
	sc, ok := Scenarios[s]
	if !ok {
		return Scenario{}, false, fmt.Errorf("unknown scenario %q (want S/M/L/XL, optionally with a /scale, or <name>:<tables>x<rows>)", s)
	}
	return sc, false, nil
}

func getenvDefault(k, d string) string {
	if v := os.Getenv(k); v != "" {
		return v
	}
	return d
}

func getenvInt(k string, d int) int {
	if v := os.Getenv(k); v != "" {
		n, err := strconv.Atoi(v)
		if err == nil {
			return n
		}
	}
	return d
}

// TestLoadTest is the single entry point. It is a test rather than a main so
// `go test -timeout` bounds a hung run and `-run` selects phases.
func TestLoadTest(t *testing.T) {
	cfg := loadConfig(t)
	run := &Run{Cfg: cfg, T: t}
	run.Execute()
}

// Run holds the state a run accumulates, so the report can be assembled at the
// end even when a phase fails.
type Run struct {
	Cfg config
	T   *testing.T

	mapping *Mapping
	corpus  *Corpus
	emb     *Embedder
	alloc   Allocation
	engine  *Engine

	report []phaseResult
}

// phaseResult is one measured phase.
type phaseResult struct {
	Phase   string
	Seconds float64
	Rows    int
	Detail  string
}

func (r *Run) record(phase string, dur time.Duration, rows int, detail string) {
	r.report = append(r.report, phaseResult{Phase: phase, Seconds: dur.Seconds(), Rows: rows, Detail: detail})
	r.T.Logf("[%s] %.2fs %s", phase, dur.Seconds(), detail)
}

// Execute walks the phases in order. A failure in a setup phase stops the run;
// a failure inside a query phase is recorded and the run continues, because the
// point of the phase is the distribution, not a single query.
func (r *Run) Execute() {
	t := r.T

	// ---- setup -----------------------------------------------------------
	start := time.Now()
	m, err := LoadMapping(r.Cfg.Mapping)
	if err != nil {
		t.Fatalf("load mapping: %v", err)
	}
	r.mapping = m

	arts, sha, err := LoadCorpus(wikiPath(), PinnedWikiSHA256)
	if err != nil {
		t.Fatalf("load corpus: %v", err)
	}
	r.corpus = NewCorpus(arts)

	emb, err := LoadEmbedder(r.Cfg.Model, 1024)
	if err != nil {
		t.Fatalf("load embedder: %v", err)
	}
	defer emb.Close()
	r.emb = emb

	r.alloc = PlanAllocation(r.Cfg.Scenario, r.Cfg.Seed, r.Cfg.Skew, "ragflow_lt")
	if n := r.Cfg.LimitTables; n > 0 && n < len(r.alloc.Rows) {
		r.alloc.Rows = r.alloc.Rows[:n]
		r.alloc.Docs = r.alloc.Docs[:n]
		r.alloc.Names = r.alloc.Names[:n]
		t.Logf("LT_LIMIT_TABLES=%d: plan trimmed to the first %d tables", n, n)
	}
	t.Logf("scenario %s: %s", r.Cfg.Scenario.Name, r.alloc.Summary())
	t.Logf("corpus: %d articles, %d start positions, sha256 %s", len(arts), r.corpus.Starts(), sha[:16])
	t.Logf("schema: %d mapping columns + 1 vector, %d index objects (variant=%s)",
		len(m.Columns), m.IndexObjects(r.Cfg.Sparsegram), variantName(r.Cfg.Sparsegram))

	host, port, err := splitHostPort(r.Cfg.Server)
	if err != nil {
		t.Fatalf("LT_SERVER: %v", err)
	}
	eng, err := Connect(host, port, "lt_load", m, 1024)
	if err != nil {
		t.Fatalf("connect: %v", err)
	}
	defer eng.Close()
	r.engine = eng
	r.record("setup", time.Since(start), 0, "mapping+corpus+embedder+connect")

	// ---- P0: create tables ----------------------------------------------
	r.phaseCreate()

	// ---- P1: ingest ------------------------------------------------------
	r.phaseIngest()

	// ---- P2: index (only in the load-then-index order) -------------------
	if r.Cfg.IndexLast {
		r.phaseIndex()
	}

	// ---- queries --------------------------------------------------------
	queries, err := BuildQueries(rand.New(rand.NewSource(r.Cfg.Seed)), r.corpus, r.alloc, r.emb, r.Cfg.Queries)
	if err != nil {
		t.Fatalf("build queries: %v", err)
	}
	// Exercise each kind against the same query set, so the difference between
	// the numbers is the query shape and nothing else.
	kinds := []string{"dense", "text", "hybrid", "filtered", "like"}
	for _, k := range kinds {
		r.phaseQueries(k, withKind(queries, k))
	}

	// ---- P7: readers plus writers ---------------------------------------
	r.phaseMixed(queries)

	// ---- P8: teardown ---------------------------------------------------
	r.phaseTeardown()

	r.printReport()
}

func variantName(sparsegram bool) string {
	if sparsegram {
		return "+sparsegram"
	}
	return "default"
}

func wikiPath() string {
	if p := os.Getenv("LT_WIKI"); p != "" {
		return p
	}
	return "../../test/data/benchmark/enwiki/enwiki-10w.csv"
}

func splitHostPort(s string) (string, int, error) {
	i := strings.LastIndex(s, ":")
	if i < 0 {
		return "", 0, fmt.Errorf("expected host:port, got %q", s)
	}
	port, err := strconv.Atoi(s[i+1:])
	if err != nil {
		return "", 0, fmt.Errorf("bad port in %q: %w", s, err)
	}
	return s[:i], port, nil
}

// phaseCreate runs P0: CREATE TABLE plus every index, before any row exists.
//
// Existing tables with this scenario's names are dropped first: a rerun has to
// be idempotent, and CREATE TABLE with ConflictTypeError would otherwise fail on
// the second run against the same database (which is how this was found).
func (r *Run) phaseCreate() {
	r.dropExisting()

	start := time.Now()
	for i, name := range r.alloc.Names {
		if _, err := r.engine.CreateTable(name, !r.Cfg.IndexLast, r.Cfg.Sparsegram); err != nil {
			r.T.Fatalf("create table %d/%d (%s): %v", i+1, len(r.alloc.Names), name, err)
		}
	}
	dur := time.Since(start)
	n := len(r.alloc.Names)
	r.record("P0 create", dur, 0, fmt.Sprintf("%d tables x %d columns + %d indexes, %.1f ms/table",
		n, len(r.mapping.Columns)+1, r.mapping.IndexObjects(r.Cfg.Sparsegram), float64(dur.Milliseconds())/float64(n)))
}

// dropExisting removes tables left by an earlier run, plus any P7 writer tables.
func (r *Run) dropExisting() {
	resp, err := r.engine.DB.ListTables()
	if err != nil {
		r.T.Fatalf("list tables: %v", err)
	}
	// ListTables returns an untyped thrift response; read the names out of it.
	names := tableNames(resp)
	dropped := 0
	for _, name := range names {
		if !strings.HasPrefix(name, "ragflow_lt") {
			continue
		}
		if _, err := r.engine.DB.DropTable(name, infinity.ConflictTypeIgnore); err != nil {
			r.T.Logf("drop stale table %s: %v", name, err)
			continue
		}
		dropped++
	}
	if dropped > 0 {
		r.T.Logf("dropped %d tables from an earlier run", dropped)
	}
}

// tableNames pulls TableNames out of the SDK's ListTables response without
// importing the internal thrift package (which Go forbids from outside the SDK).
func tableNames(resp interface{}) []string {
	type named interface{ GetTableNames() []string }
	if n, ok := resp.(named); ok {
		return n.GetTableNames()
	}
	// Fall back to formatting the struct and picking bracket contents, which is
	// what the SDK returns when the typed getter is absent.
	s := fmt.Sprint(resp)
	i := strings.Index(s, "TableNames:[")
	if i < 0 {
		return nil
	}
	rest := s[i+len("TableNames:["):]
	j := strings.Index(rest, "]")
	if j < 0 {
		return nil
	}
	return strings.Fields(rest[:j])
}

// phaseIngest runs P1: build rows, embed them, insert them.
//
// Rows are generated and embedded in batches on several goroutines, each with
// its own SDK connection, but each table is written by exactly one goroutine so
// the per-table row order stays reproducible. The embedder is shared and holds
// its own lock; model2vec-rs parallelises inside a call, so the win here comes
// from overlapping embed with insert, not from concurrent encodes.
func (r *Run) phaseIngest() {
	t := r.T
	totalRows := r.alloc.Total()
	start := time.Now()

	type job struct {
		idx int
	}
	jobs := make(chan job, len(r.alloc.Names))
	for i := range r.alloc.Names {
		jobs <- job{idx: i}
	}
	close(jobs)

	var inserted atomic.Int64
	batchSize := getenvInt("LT_BATCH_SIZE", 1024)
	var wg sync.WaitGroup
	errCh := make(chan error, len(r.alloc.Names))

	// Progress heartbeat. Without it, a run that stalls at table 100 of 600 looks
	// identical to one that is merely slow, which is exactly how a 40-minute
	// stall was first seen.
	done := make(chan struct{})
	go func() {
		tick := time.NewTicker(15 * time.Second)
		defer tick.Stop()
		for {
			select {
			case <-done:
				return
			case <-tick.C:
				n := inserted.Load()
				t.Logf("P1 progress: %d/%d rows, %.0f rows/s so far",
					n, totalRows, float64(n)/time.Since(start).Seconds())
			}
		}
	}()
	defer close(done)

	workers := max(1, r.Cfg.Batches)
	for w := 0; w < workers; w++ {
		wg.Add(1)
		go func(worker int) {
			defer wg.Done()
			// One connection per worker: the engine's per-connection state is
			// not shared, and reusing one connection across goroutines would
			// serialise the inserts and measure the SDK's mutex.
			host, port, _ := splitHostPort(r.Cfg.Server)
			eng, err := Connect(host, port, "lt_load", r.mapping, 1024)
			if err != nil {
				errCh <- fmt.Errorf("worker %d connect: %w", worker, err)
				return
			}
			defer eng.Close()

			for j := range jobs {
				idx := j.idx
				rows := r.alloc.Rows[idx]
				if rows == 0 {
					continue
				}
				tbl, err := eng.DB.GetTable(r.alloc.Names[idx])
				if err != nil {
					errCh <- fmt.Errorf("get table %s: %w", r.alloc.Names[idx], err)
					return
				}
				tblStart := time.Now()
				if err := r.ingestTable(eng, tbl, idx, batchSize, &inserted); err != nil {
					errCh <- err
					return
				}
				t.Logf("P1: table %s (%d rows) took %.1fs", r.alloc.Names[idx], r.alloc.Rows[idx], time.Since(tblStart).Seconds())
			}
		}(w)
	}
	wg.Wait()
	close(errCh)
	for err := range errCh {
		t.Fatalf("ingest: %v", err)
	}

	dur := time.Since(start)
	got := int(inserted.Load())
	if got != totalRows {
		t.Errorf("ingested %d rows, want %d", got, totalRows)
	}
	r.record("P1 ingest", dur, got, fmt.Sprintf("%d rows into %d tables, %.0f rows/s, batch %d",
		got, len(r.alloc.Names), float64(got)/dur.Seconds(), getenvInt("LT_BATCH_SIZE", 1024)))
}

// ingestTable fills one table.
func (r *Run) ingestTable(eng *Engine, tbl *infinity.Table, idx, batchSize int, inserted *atomic.Int64) error {
	rows := r.alloc.Rows[idx]
	docs := r.alloc.Docs[idx]
	kbID := fmt.Sprintf("kb_%05d", idx)
	docIDs := make([]string, docs)
	for i := range docIDs {
		docIDs[i] = fmt.Sprintf("%032x", rand.New(rand.NewSource(int64(idx)<<32|int64(i))).Int63())
	}

	// Per-table RNG so a table's content does not depend on which worker got it.
	rng := rand.New(rand.NewSource(r.Cfg.Seed + int64(idx)*7919))
	rb := &RowBuilder{Mapping: r.mapping, Dim: r.emb.Dim()}

	rowBatch := make([]Row, 0, batchSize)
	texts := make([]string, 0, batchSize)
	flush := func() error {
		if len(rowBatch) == 0 {
			return nil
		}
		vectors, err := r.emb.Encode(texts, fallbackTokens)
		if err != nil {
			return fmt.Errorf("embed table %s: %w", r.alloc.Names[idx], err)
		}
		if err := eng.IngestBatch(tbl, rowBatch, vectors); err != nil {
			return err
		}
		inserted.Add(int64(len(rowBatch)))
		rowBatch = rowBatch[:0]
		texts = texts[:0]
		return nil
	}

	for i := 0; i < rows; i++ {
		art, text := r.corpus.Sample(rng)
		docID := docIDs[i%docs]
		row, err := rb.Build(rng, art, text, docID, kbID, i%chunksPerDoc, 1+(i%chunksPerDoc)/10)
		if err != nil {
			return fmt.Errorf("build row: %w", err)
		}
		rowBatch = append(rowBatch, row)
		texts = append(texts, text)
		if len(rowBatch) == batchSize {
			if err := flush(); err != nil {
				return err
			}
		}
	}
	return flush()
}

// phaseIndex is P2 for the load-then-index order: build every index after the
// rows are in.
//
// It runs single-threaded and with no readers, deliberately. A CREATE INDEX on a
// populated table segfaults the server if a SELECT is running concurrently
// (measured; see phaseMixed), so this phase must not overlap anything.
func (r *Run) phaseIndex() {
	t := r.T
	start := time.Now()
	totalRows := 0
	for i, name := range r.alloc.Names {
		if r.alloc.Rows[i] == 0 {
			continue
		}
		tbl, err := r.engine.DB.GetTable(name)
		if err != nil {
			t.Fatalf("P2 get table %s: %v", name, err)
		}
		if err := r.engine.CreateIndexes(tbl, r.Cfg.Sparsegram); err != nil {
			t.Fatalf("P2 create indexes on %s: %v", name, err)
		}
		totalRows += r.alloc.Rows[i]
		if i%25 == 0 {
			t.Logf("P2: %d/%d tables indexed (%.0fs)", i+1, len(r.alloc.Names), time.Since(start).Seconds())
		}
	}
	dur := time.Since(start)
	r.record("P2 index", dur, totalRows, fmt.Sprintf("%d index objects per table over %d rows, %.1f ms/row",
		r.mapping.IndexObjects(r.Cfg.Sparsegram), totalRows, float64(dur.Milliseconds())/float64(max(1, totalRows))))
}

// withKind copies specs under another query kind, keeping text and vector.
func withKind(specs []QuerySpec, kind string) []QuerySpec {
	out := make([]QuerySpec, len(specs))
	for i, s := range specs {
		s.Kind = kind
		if kind == "point" {
			// Q5/Q6 need an id or kb, not a phrase; the driver fills them from
			// the table's own rows below.
			s.Text = ""
		}
		out[i] = s
	}
	return out
}

// phaseQueries runs one query kind N times sequentially and reports latency.
//
// Sequential on one connection: this measures the engine, not the client. The
// concurrency phases (P4/P5) are what measure the engine under load, and
// concurrency in this phase would confound the two.
func (r *Run) phaseQueries(kind string, specs []QuerySpec) {
	if kind == "point" {
		// Resolve a real id per table so the point lookup is not a miss.
		specs = r.pointSpecs(specs)
	}
	ctx := context.Background()
	start := time.Now()
	timings := r.engine.RunSequential(ctx, specs)
	dur := time.Since(start)

	errs := 0
	rows := 0
	lats := make([]time.Duration, 0, len(timings))
	for _, tm := range timings {
		if tm.Err != nil {
			errs++
			continue
		}
		lats = append(lats, tm.Latency)
		rows += tm.Rows
	}
	if errs > 0 {
		r.T.Errorf("Q(%s): %d/%d queries failed; first error: %v", kind, errs, len(timings), firstErr(timings))
	}
	p50, p99 := percentiles(lats)
	r.record("Q "+kind, dur, rows, fmt.Sprintf("%d queries, p50 %s p99 %s, %d errors",
		len(timings), p50.Round(time.Microsecond), p99.Round(time.Microsecond), errs))
}

// pointSpecs resolves a real doc_id or kb_id for each point query, so Q5/Q6 hit
// rows instead of missing.
func (r *Run) pointSpecs(specs []QuerySpec) []QuerySpec {
	// kb_id is known from the allocation: table i uses kb_%05d.
	out := make([]QuerySpec, 0, len(specs))
	for _, s := range specs {
		if s.Table == "" {
			continue
		}
		var idx int
		if _, err := fmt.Sscanf(s.Table, "ragflow_lt_%05d", &idx); err != nil {
			continue
		}
		s.KBID = fmt.Sprintf("kb_%05d", idx)
		s.DocID = ""
		out = append(out, s)
	}
	return out
}

func firstErr(ts []Timed) error {
	for _, t := range ts {
		if t.Err != nil {
			return t.Err
		}
	}
	return nil
}

func percentiles(lats []time.Duration) (p50, p99 time.Duration) {
	if len(lats) == 0 {
		return 0, 0
	}
	sorted := append([]time.Duration(nil), lats...)
	sortDurations(sorted)
	return sorted[len(sorted)/2], sorted[min(len(sorted)-1, len(sorted)*99/100)]
}

func sortDurations(xs []time.Duration) {
	for i := 1; i < len(xs); i++ {
		for j := i; j > 0 && xs[j] < xs[j-1]; j-- {
			xs[j], xs[j-1] = xs[j-1], xs[j]
		}
	}
}

// phaseMixed is P7: readers and writers at the same time.
//
// The writers create and fill a fresh table, which is what a tenant onboarding a
// dataset does; the readers keep running the hybrid mix. The measured thing is
// whether read latency degrades, so the result reports both halves.
func (r *Run) phaseMixed(queries []QuerySpec) {
	t := r.T
	hybrid := withKind(queries, "hybrid")
	readers := getenvInt("LT_READERS", 8)
	writers := getenvInt("LT_WRITERS", 2)
	dur := time.Duration(getenvInt("LT_MIXED_SECONDS", 20)) * time.Second
	if len(hybrid) == 0 {
		t.Log("P7 skipped: no queries")
		return
	}

	ctx, cancel := context.WithTimeout(context.Background(), dur)
	defer cancel()

	var readLats sync.Map // goroutine -> *[]time.Duration
	var readErrs atomic.Int64
	var firstReadErr atomic.Value // error, so a failure can be attributed
	var aborted atomic.Bool       // set once the server is gone; stops the phase
	var wg sync.WaitGroup

	// cancelOnServerLoss ends the phase when reads keep failing: at that point
	// the remaining seconds measure nothing, and a phase that "completes" with
	// millions of errors hides the real result.
	phaseCtx, stopPhase := context.WithCancel(ctx)
	defer stopPhase()
	go func() {
		tick := time.NewTicker(2 * time.Second)
		defer tick.Stop()
		last := int64(0)
		for {
			select {
			case <-phaseCtx.Done():
				return
			case <-tick.C:
				cur := readErrs.Load()
				if cur > 0 && cur == last {
					// no new errors this window; nothing to judge
					continue
				}
				last = cur
				if cur > 0 && len(allReadLatencies(&readLats)) == 0 {
					aborted.Store(true)
					stopPhase()
					return
				}
			}
		}
	}()

	// Readers
	for i := 0; i < readers; i++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			host, port, _ := splitHostPort(r.Cfg.Server)
			eng, err := Connect(host, port, "lt_load", r.mapping, 1024)
			if err != nil {
				readErrs.Add(1)
				firstReadErr.CompareAndSwap(nil, err)
				return
			}
			defer eng.Close()
			lats := make([]time.Duration, 0, 512)
			j := id
			for phaseCtx.Err() == nil {
				s := hybrid[j%len(hybrid)]
				j++
				st := time.Now()
				if _, err := eng.RunQuery(ctx, s); err != nil {
					// A cancelled context is the phase ending, not a failure;
					// the SDK surfaces it as an error like any other.
					if phaseCtx.Err() == nil {
						readErrs.Add(1)
						firstReadErr.CompareAndSwap(nil, err)
						// Back off on failure. Without this, a server that has
						// gone away turns the reader loop into a busy spin: one
						// run recorded 33,572,219 errors in 10 s and the error
						// count said more about the loop than about the server.
						select {
						case <-phaseCtx.Done():
						case <-time.After(50 * time.Millisecond):
						}
					}
					continue
				}
				lats = append(lats, time.Since(st))
			}
			readLats.Store(id, lats)
		}(i)
	}

	// Writers: new tables, filled while the readers run.
	//
	// CREATE TABLE is serialised across writers: the catalog write
	// (`db|2|next_table_id`) is a single RocksDB key, and two concurrent creates
	// lose the race with "Resource busy" (measured). That is a real server
	// limitation worth noting in the report, not something to paper over with
	// retries inside a timing loop, so the writers take a lock and the creates
	// are counted as their own step.
	var createMu sync.Mutex
	var written atomic.Int64
	for i := 0; i < writers; i++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			host, port, _ := splitHostPort(r.Cfg.Server)
			eng, err := Connect(host, port, "lt_load", r.mapping, 1024)
			if err != nil {
				return
			}
			defer eng.Close()

			name := fmt.Sprintf("ragflow_lt_w%d_%d", id, time.Now().UnixNano()%100000)
			createMu.Lock()
			// P7 writer tables are always created without indexes, regardless of
			// LT_INDEX_LAST. Reason, measured: a CREATE INDEX issued on a table
			// that already has rows, while another connection is running SELECT,
			// segfaults the server ("Can't bind column expression ...
			// column_remapper_impl.cpp:148", then SIGSEGV). Building the index
			// with no rows present never crashed. So the mixed phase cannot
			// exercise the load-then-index order; that order is measured in P1
			// where nothing else is running.
			tbl, err := eng.CreateTable(name, false, r.Cfg.Sparsegram)
			createMu.Unlock()
			if err != nil {
				r.T.Logf("P7 writer %d: create %s: %v", id, name, err)
				return
			}
			defer eng.DB.DropTable(name, infinity.ConflictTypeIgnore)
			rng := rand.New(rand.NewSource(r.Cfg.Seed + int64(id)*104729))
			rb := &RowBuilder{Mapping: r.mapping, Dim: r.emb.Dim()}
			for ctx.Err() == nil && written.Load() < int64(getenvInt("LT_WRITER_ROWS", 400)) {
				rows := make([]Row, 0, 32)
				texts := make([]string, 0, 32)
				for k := 0; k < 32; k++ {
					art, text := r.corpus.Sample(rng)
					row, err := rb.Build(rng, art, text, fmt.Sprintf("%032x", rng.Int63()), "kb_w", k, 1)
					if err != nil {
						return
					}
					rows = append(rows, row)
					texts = append(texts, text)
				}
				vecs, err := r.emb.Encode(texts, fallbackTokens)
				if err != nil {
					return
				}
				if err := eng.IngestBatch(tbl, rows, vecs); err != nil {
					r.T.Logf("P7 writer %d: insert: %v", id, err)
					return
				}
				written.Add(int64(len(rows)))
			}
		}(i)
	}
	wg.Wait()

	var all []time.Duration
	all = allReadLatencies(&readLats)
	p50, p99 := percentiles(all)
	if aborted.Load() {
		r.T.Errorf("P7: server stopped answering; phase aborted after %s", dur)
	}
	detail := fmt.Sprintf("%d readers / %d writers for %s: %d reads, p50 %s p99 %s, %d read errors, %d rows written",
		readers, writers, dur, len(all), p50.Round(time.Microsecond), p99.Round(time.Microsecond),
		readErrs.Load(), written.Load())
	if err, _ := firstReadErr.Load().(error); err != nil {
		detail += fmt.Sprintf("; first error: %v", err)
	}
	r.record("P7 mixed", dur, int(written.Load()), detail)
}

// phaseTeardown is P8: drop every table this run created.
func (r *Run) phaseTeardown() {
	if os.Getenv("LT_KEEP_TABLES") != "" {
		r.T.Log("P8 skipped: LT_KEEP_TABLES is set")
		return
	}
	start := time.Now()
	dropped := 0
	for _, name := range r.alloc.Names {
		if _, err := r.engine.DB.DropTable(name, infinity.ConflictTypeIgnore); err != nil {
			r.T.Logf("drop %s: %v", name, err)
			continue
		}
		dropped++
	}
	r.record("P8 teardown", time.Since(start), 0, fmt.Sprintf("dropped %d tables", dropped))
}

// printReport writes the phase table and the environment, so two runs can be
// compared without guessing what differed.
func (r *Run) printReport() {
	var b strings.Builder
	b.WriteString("\n=== load test report ===\n")
	fmt.Fprintf(&b, "scenario     %s\n", r.Cfg.Scenario.Name)
	fmt.Fprintf(&b, "variant      %s (%d index objects/table)\n",
		variantName(r.Cfg.Sparsegram), r.mapping.IndexObjects(r.Cfg.Sparsegram))
	fmt.Fprintf(&b, "import order %s\n", map[bool]string{true: "load then index (P1b)", false: "indexes first (P1a)"}[r.Cfg.IndexLast])
	fmt.Fprintf(&b, "seed         %d\n", r.Cfg.Seed)
	fmt.Fprintf(&b, "tables/rows  %d / %d\n", len(r.alloc.Names), r.alloc.Total())
	fmt.Fprintf(&b, "go           %s, GOMAXPROCS=%d\n", runtime.Version(), runtime.GOMAXPROCS(0))
	b.WriteString("\nphase            seconds        rows  detail\n")
	var total float64
	for _, p := range r.report {
		fmt.Fprintf(&b, "%-16s %8.2f %11d  %s\n", p.Phase, p.Seconds, p.Rows, p.Detail)
		total += p.Seconds
	}
	fmt.Fprintf(&b, "%-16s %8.2f\n", "TOTAL", total)

	// Ingest cost attribution. The probes behind these numbers are in
	// tools/loadtest/README.md; they are repeated here because a reader looking
	// at a low rows/s figure needs to know where it came from, and the answer is
	// not "the vector index".
	b.WriteString("\ningest cost attribution (measured, 93-column table):\n")
	for _, line := range []string{
		"  no indexes                       ~3,300 rows/s",
		"  + HNSW (lvq) only                ~2,600 rows/s",
		"  + 11 secondary indexes only      ~3,400 rows/s",
		"  + full-text on whitespace/rank   ~3,300 rows/s",
		"  + ONE rag-coarse|rag-fine index     ~264 rows/s",
		"  + all 47 full-text (production)     ~230 rows/s",
		"",
		"  The first rag-* index costs ~12x and every further rag-* index is free:",
		"  the cost is per row in the RAG analyzer, not per index object. HNSW and",
		"  the secondary indexes are noise by comparison. So a plan that budgets",
		"  ingest from vector-index cost, or from index count, will be wrong by an",
		"  order of magnitude -- the 8.34M-row L scenario is ~10 hours of ingest at",
		"  this rate, not the ~1 hour the plan's 1,936 rows/s estimate implied.",
	} {
		b.WriteString(line + "\n")
	}
	r.T.Log(b.String())
}
