package loadtest

// Engine-facing operations: create the production table, load it, and run the
// query mix. All of it goes through the Go SDK, which is also what the load
// phases use, so a slow query cannot be blamed on a different client.

import (
	"context"
	"fmt"
	"math/rand"
	"strings"
	"sync"
	"time"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

// Engine wraps one SDK connection plus the schema it operates on.
type Engine struct {
	Conn    *infinity.InfinityConnection
	DB      *infinity.Database
	DBName  string
	Mapping *Mapping
	Dim     int
}

// Connect opens a connection and gets (or creates) the database.
func Connect(host string, port int, dbName string, m *Mapping, dim int) (*Engine, error) {
	conn, err := infinity.Connect(infinity.NetworkAddress{IP: host, Port: port})
	if err != nil {
		return nil, fmt.Errorf("connect %s:%d: %w", host, port, err)
	}
	if _, err := conn.CreateDatabase(dbName, infinity.ConflictTypeIgnore, ""); err != nil {
		// Ignore "already exists"; GetDatabase below is the real check.
		_ = err
	}
	db, err := conn.GetDatabase(dbName)
	if err != nil {
		_, _ = conn.Disconnect()
		return nil, fmt.Errorf("get database %q: %w", dbName, err)
	}
	return &Engine{Conn: conn, DB: db, DBName: dbName, Mapping: m, Dim: dim}, nil
}

// Close disconnects.
func (e *Engine) Close() {
	if e.Conn != nil {
		_, _ = e.Conn.Disconnect()
	}
}

// CreateTable issues CREATE TABLE with the mapping's columns plus the vector
// column, then the indexes.
//
// Indexes are created before any row is inserted, which is production's order
// (RAGFlow creates them in createChunkStoreWithDB) and costs ~4.7x in ingest
// throughput versus loading first. That is the number that predicts the
// migration, so it is the default here; the builder-order variant is a flag.
func (e *Engine) CreateTable(name string, withIndexes bool, sparsegram bool) (*infinity.Table, error) {
	cols, err := e.Mapping.ColumnTypes(e.Dim)
	if err != nil {
		return nil, err
	}
	schema := make(infinity.TableSchema, 0, len(cols))
	for _, c := range cols {
		schema = append(schema, &infinity.ColumnDefinition{Name: c[0], DataType: c[1]})
	}
	table, err := e.DB.CreateTable(name, schema, infinity.ConflictTypeError)
	if err != nil {
		return nil, fmt.Errorf("create table %s: %w", name, err)
	}
	if !withIndexes {
		return table, nil
	}
	if err := e.CreateIndexes(table, sparsegram); err != nil {
		return nil, err
	}
	return table, nil
}

// CreateIndexes adds the full-text, secondary and HNSW indexes for a table.
//
// It is separate from CreateTable so the P1b variant (load, then index) can call
// it after the import.
func (e *Engine) CreateIndexes(table *infinity.Table, sparsegram bool) error {
	for _, ft := range e.Mapping.FullTextIndexes(sparsegram) {
		col, an := ft[0], ft[1]
		name := SafeIndexName("ft_", col, an)
		if _, err := table.CreateIndex(name,
			infinity.NewIndexInfo(col, infinity.IndexTypeFullText, map[string]string{"ANALYZER": an}),
			infinity.ConflictTypeError, ""); err != nil {
			return fmt.Errorf("create fulltext index %s: %w", name, err)
		}
	}
	for _, col := range e.Mapping.SecondaryIndexes() {
		name := SafeIndexName("sec_", col, "")
		name = strings.TrimSuffix(name, "_")
		if _, err := table.CreateIndex(name,
			infinity.NewIndexInfo(col, infinity.IndexTypeSecondary, nil),
			infinity.ConflictTypeError, ""); err != nil {
			return fmt.Errorf("create secondary index %s: %w", name, err)
		}
	}
	vecCol := VectorColumn(e.Dim)
	if _, err := table.CreateIndex("vec_idx",
		infinity.NewIndexInfo(vecCol, infinity.IndexTypeHnsw, map[string]string{
			"M":               "16",
			"ef_construction": "50",
			"metric":          "cosine",
			// lvq needs a float column, which is what ColumnTypes emits.
			"encode": "lvq",
		}),
		infinity.ConflictTypeError, ""); err != nil {
		return fmt.Errorf("create hnsw index: %w", err)
	}
	return nil
}

// IngestBatch embeds a batch of texts and inserts the rows.
//
// Embedding and inserting are separate arguments on purpose: the caller can
// embed several batches ahead while the previous insert is in flight, which is
// what keeps the measured ingest rate from being a measure of the embedder.
func (e *Engine) IngestBatch(table *infinity.Table, rows []Row, vectors []float32) error {
	if len(rows) == 0 {
		return nil
	}
	if len(vectors) != len(rows)*e.Dim {
		return fmt.Errorf("ingest: %d vectors for %d rows at dim %d", len(vectors)/max(1, e.Dim), len(rows), e.Dim)
	}
	vecCol := VectorColumn(e.Dim)
	batch := make([]map[string]interface{}, len(rows))
	for i := range rows {
		m := rows[i].Values
		// The SDK's float32 path packs the slice into the thrift embedding
		// field; a []float64 would be a different element type and the server
		// would reject the width.
		m[vecCol] = vectors[i*e.Dim : (i+1)*e.Dim]
		batch[i] = m
	}
	if _, err := table.Insert(batch); err != nil {
		return fmt.Errorf("insert %d rows: %w", len(batch), err)
	}
	return nil
}

// QuerySpec is one entry of the Q1-Q7 mix.
type QuerySpec struct {
	Kind  string // "dense", "text", "hybrid", "filtered", "point", "like"
	Text  string
	Vec   []float32
	Table string // table to run against; "" means any
	DocID string
	KBID  string
}

// RunQuery executes one query and returns the row count actually read, which is
// what a latency number has to be conditioned on.
func (e *Engine) RunQuery(ctx context.Context, spec QuerySpec) (int, error) {
	table, err := e.DB.GetTable(spec.Table)
	if err != nil {
		return 0, fmt.Errorf("get table %s: %w", spec.Table, err)
	}
	vecCol := VectorColumn(e.Dim)
	topN := 100

	var df *infinity.DataFrame
	switch spec.Kind {
	case "dense":
		// ef is an index parameter, not a MATCH VECTOR argument; the SDK sends
		// it through OptParams, which is why the map is the right place for it.
		t := table.Output([]string{"id", "doc_id"}).
			MatchDense(vecCol, spec.Vec, "float", "cosine", topN, map[string]string{"ef": "200"}).
			Filter("available_int = 1").
			Limit(10)
		df, err = t.ToDataFrame()
	case "text":
		// Field names are the converted ones production uses: docnm,
		// important_keywords and content, weighted.
		t := table.Output([]string{"id", "doc_id"}).
			MatchText("content^1,docnm^10,important_keywords^30", spec.Text, topN,
				map[string]string{"minimum_should_match": "30%"}).
			Limit(10)
		df, err = t.ToDataFrame()
	case "hybrid":
		// weighted_sum with production's weights. match_both does not exist and
		// would silently degrade to RRF.
		t := table.Output([]string{"id", "doc_id"}).
			MatchText("content^1,docnm^10,important_keywords^30", spec.Text, topN,
				map[string]string{"minimum_should_match": "0%"}).
			MatchDense(vecCol, spec.Vec, "float", "cosine", topN, map[string]string{
				"ef":     "200",
				"filter": "available_int = 1",
			}).
			Fusion("weighted_sum", topN, map[string]interface{}{"weights": "0.7,0.3"}).
			Limit(10)
		df, err = t.ToDataFrame()
	case "filtered":
		filter := "available_int = 1"
		if term := sanitizeTerm(spec.Text); term != "" {
			filter = fmt.Sprintf("(available_int = 1) AND filter_fulltext('content', '%s')",
				escapeSQL(term))
		}
		if err := validateFilter(filter); err != nil {
			return 0, err
		}
		t := table.Output([]string{"id", "doc_id"}).
			MatchDense(vecCol, spec.Vec, "float", "cosine", topN, map[string]string{"ef": "200"}).
			Filter(filter).
			Limit(10)
		df, err = t.ToDataFrame()
	case "point":
		// Q5/Q6: driven by a secondary index (kb_id) or a plain filter.
		filter := fmt.Sprintf("doc_id = '%s'", escapeSQL(spec.DocID))
		if spec.DocID == "" {
			filter = fmt.Sprintf("kb_id = '%s'", escapeSQL(spec.KBID))
		}
		if err := validateFilter(filter); err != nil {
			return 0, err
		}
		df, err = table.Output([]string{"id"}).Filter(filter).Limit(10).ToDataFrame()
	case "like":
		term := sanitizeTerm(spec.Text)
		if term == "" {
			return 0, fmt.Errorf("like: no usable term in %q", spec.Text)
		}
		filter := fmt.Sprintf("content LIKE '%%%s%%'", escapeSQL(term))
		if err := validateFilter(filter); err != nil {
			return 0, err
		}
		df, err = table.Output([]string{"id"}).Filter(filter).Limit(10).ToDataFrame()
	default:
		return 0, fmt.Errorf("unknown query kind %q", spec.Kind)
	}
	if err != nil {
		return 0, err
	}
	return df.RowCount, nil
}

// escapeSQL makes a string safe to splice into a filter expression.
//
// Three hazards, all found the hard way:
//
//   - The Go SDK's expression parser is what builds the filter, and it does not
//     understand `''` the way SQL does. It also panics — not errors — on a
//     string literal with an unbalanced quote (`parseConstant` does
//     `expr[1:len(expr)-1]` on a lone `'`, a slice-bounds panic; see
//     go/expression_parser.go:824). A real corpus contains apostrophes ("it's",
//     "1980's"), so a phrase with one aborted the whole run.
//   - `parseBetweenExpression` (go/expression_parser.go:458) scans for
//     " BETWEEN " and " AND " anywhere in the expression. Prose contains
//     "between the ages of 5 and 12", so a sampled phrase would be parsed as a
//     BETWEEN range and then crash. That is why LIKE patterns are restricted to
//     a single term by sanitizeTerm below.
//
// So apostrophes and backslashes are removed rather than escaped. That changes
// a Q7 term slightly ("it's" -> "its"), which is acceptable for a LIKE scan and
// is noted in the report; silently aborting is not.
func escapeSQL(s string) string {
	s = strings.ReplaceAll(s, "'", "")
	s = strings.ReplaceAll(s, "\\", "")
	return s
}

// sanitizeTerm reduces a phrase to one alphanumeric word.
//
// Q7 and the filtered-dense pre-filter need a term, not a sentence: a sentence
// brings both the BETWEEN/AND hazard above and SQL keywords that the SDK parser
// interprets as operators. A single word is also what a regex/LIKE selectivity
// test is about.
func sanitizeTerm(s string) string {
	var b strings.Builder
	for _, r := range s {
		switch {
		case r >= 'a' && r <= 'z', r >= 'A' && r <= 'Z', r >= '0' && r <= '9', r == '-':
			b.WriteRune(r)
		case r == ' ' || r == '\t':
			if b.Len() > 0 {
				return b.String()
			}
		}
	}
	return b.String()
}

// validateFilter reports whether the SDK can parse a filter, converting the
// parser's panics into an error.
//
// It runs in the caller's goroutine on purpose: a panic raised on a load-phase
// goroutine cannot be recovered by the test runner and takes the process down,
// which is exactly what happened before this guard existed.
func validateFilter(filter string) (err error) {
	if filter == "" {
		return nil
	}
	defer func() {
		if r := recover(); r != nil {
			err = fmt.Errorf("SDK filter parser panicked on %q: %v", filter, r)
		}
	}()
	_, err = infinity.ParseFilter(filter)
	return err
}

// BuildQueries freezes the query set from the corpus.
//
// Query vectors come from the encoder, never from a stored row: copying a row's
// vector would put distance 0 at the top of every result and hide LVQ recall
// loss.
//
// Phrases are drawn from windows that look like prose. A window is rejected when
// fewer than half its words start with a letter: the extract interleaves formula
// fragments with text, and a phrase like ":n = 4: 2 + 8 = 7" makes the engine
// refuse the query outright ("Trying to match: ... failed", measured on 6 of 20
// queries before this filter).
func BuildQueries(rng *rand.Rand, c *Corpus, alloc Allocation, emb *Embedder, n int) ([]QuerySpec, error) {
	nonEmpty := alloc.NonEmpty()
	if len(nonEmpty) == 0 {
		return nil, fmt.Errorf("no non-empty table to query")
	}

	type cand struct {
		text  string
		table string
	}
	cands := make([]cand, 0, n)
	for attempts := 0; len(cands) < n && attempts < n*200; attempts++ {
		_, text := c.Sample(rng)
		words := proseWords(text)
		if len(words) < 3 {
			continue
		}
		take := 3 + rng.Intn(min(10, len(words)-2))
		begin := rng.Intn(max(1, len(words)-take))
		phrase := strings.Join(words[begin:begin+take], " ")
		cands = append(cands, cand{text: phrase, table: alloc.Names[nonEmpty[rng.Intn(len(nonEmpty))]]})
	}
	if len(cands) == 0 {
		return nil, fmt.Errorf("could not sample %d prose queries from the corpus", n)
	}

	texts := make([]string, len(cands))
	for i, cd := range cands {
		texts[i] = cd.text
	}
	vecs, err := emb.Encode(texts, fallbackTokens)
	if err != nil {
		return nil, fmt.Errorf("embed queries: %w", err)
	}

	specs := make([]QuerySpec, len(cands))
	for i, cd := range cands {
		specs[i] = QuerySpec{
			Kind:  "hybrid",
			Text:  cd.text,
			Vec:   vecs[i*emb.Dim() : (i+1)*emb.Dim()],
			Table: cd.table,
		}
	}
	return specs, nil
}

// proseWords returns the words of a window if it is mostly prose, else nil.
//
// Every returned word is pure ASCII letters. That is stricter than it looks
// necessary, and it is: a token with punctuation in it (`sk`, `n&nbsp;`) makes
// the engine's MATCH TEXT fail with either "Trying to match: ... failed" or a
// confusing `Column "sk" index "" doesn't exist`. Filtering to letters is the
// only rule that held across every failure observed here.
func proseWords(text string) []string {
	words := strings.Fields(text)
	if len(words) < 3 {
		return nil
	}
	out := make([]string, 0, len(words))
	for _, w := range words {
		if isWord(w) {
			out = append(out, w)
		}
	}
	// Require that most of the source was usable prose, so a window of formula
	// fragments does not produce a handful of stray words.
	if len(out)*2 < len(words) || len(out) < 3 {
		return nil
	}
	return out
}

// isWord reports whether s is a plain alphabetic token of a plausible length.
func isWord(s string) bool {
	if len(s) < 2 || len(s) > 24 {
		return false
	}
	for i := 0; i < len(s); i++ {
		c := s[i]
		if (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') {
			return false
		}
	}
	return true
}

// Timed is one query execution.
type Timed struct {
	Kind    string
	Latency time.Duration
	Rows    int
	Err     error
}

// RunSequential runs specs in order on one connection and times each.
func (e *Engine) RunSequential(ctx context.Context, specs []QuerySpec) []Timed {
	out := make([]Timed, 0, len(specs))
	for _, s := range specs {
		start := time.Now()
		n, err := e.RunQuery(ctx, s)
		out = append(out, Timed{Kind: s.Kind, Latency: time.Since(start), Rows: n, Err: err})
	}
	return out
}

// RunConcurrent distributes specs across N worker connections and times each.
func RunConcurrent(ctx context.Context, host string, port int, dbName string, m *Mapping, dim int, specs []QuerySpec, workers int) []Timed {
	out := make([]Timed, len(specs))
	if len(specs) == 0 {
		return out
	}
	workers = min(max(1, workers), len(specs))
	jobs := make(chan int, len(specs))
	for i := range specs {
		jobs <- i
	}
	close(jobs)

	var wg sync.WaitGroup
	for w := 0; w < workers; w++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			eng, err := Connect(host, port, dbName, m, dim)
			if err != nil {
				for idx := range jobs {
					out[idx] = Timed{Kind: specs[idx].Kind, Err: err}
				}
				return
			}
			defer eng.Close()
			for idx := range jobs {
				start := time.Now()
				n, qerr := eng.RunQuery(ctx, specs[idx])
				out[idx] = Timed{Kind: specs[idx].Kind, Latency: time.Since(start), Rows: n, Err: qerr}
			}
		}()
	}
	wg.Wait()
	return out
}


// allReadLatencies flattens the per-goroutine latency slices.
func allReadLatencies(m *sync.Map) []time.Duration {
	var out []time.Duration
	m.Range(func(_, v interface{}) bool {
		out = append(out, v.([]time.Duration)...)
		return true
	})
	return out
}
