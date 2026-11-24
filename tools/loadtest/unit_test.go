package loadtest

// Unit tests for the parts that can be checked without a server: corpus
// cleaning, window sampling, schema reading, the row shape, and the embedder.
// The end-to-end load test lives in load_test.go and needs a running Infinity.

import (
	"math/rand"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

func TestCleanTextStripsMarkup(t *testing.T) {
	in := `{{Redirect|Anarchist}} '''Anarchism''' is a [[political philosophy|philosophy]]` +
		`<ref name="x">{{cite journal|title=Y}}</ref> that holds the [[state (polity)|state]] to be unnecessary.` +
		` == Pain == More text.<ref name="z"/>`
	got := CleanText(in)

	for _, bad := range []string{"{{", "}}", "[[", "]]", "<ref", "'''", "==", "cite journal"} {
		if strings.Contains(got, bad) {
			t.Errorf("CleanText left %q in %q", bad, got)
		}
	}
	for _, want := range []string{"Anarchism", "philosophy", "state", "Pain", "More text"} {
		if !strings.Contains(got, want) {
			t.Errorf("CleanText dropped %q from %q", want, got)
		}
	}
}

func TestCleanTextKeepsLinkLabel(t *testing.T) {
	// [[target|label]] must become "label", not "target": production indexes
	// display text, and keeping targets would add thousands of namespace-ish
	// tokens that no query contains.
	got := CleanText("see [[state (polity)|the state]] here")
	if !strings.Contains(got, "the state") || strings.Contains(got, "polity") {
		t.Errorf("wikilink label not kept: %q", got)
	}
}

func TestCutWindowBoundaries(t *testing.T) {
	text := strings.Repeat("alpha beta gamma delta ", 200) // 4800 chars
	art := &Article{Title: "t", TS: "30-APR-2012 03:25:17.000", Text: text, Starts: len(text) - minWindow + 1}
	c := NewCorpus([]*Article{art})
	rng := rand.New(rand.NewSource(1))

	for i := 0; i < 200; i++ {
		_, w := c.Sample(rng)
		if len(w) < minWindow {
			t.Fatalf("window shorter than %d: %d chars", minWindow, len(w))
		}
		if len(w) > maxWindow+1 {
			t.Fatalf("window longer than %d: %d chars", maxWindow, len(w))
		}
		// A window must not start or end mid-word. The corpus is all words with
		// single spaces, so a non-boundary edge means the cut was wrong.
		if strings.HasPrefix(w, " ") || strings.HasSuffix(w, " ") {
			t.Fatalf("window has a leading/trailing space: %q", w[:20])
		}
		first := strings.Fields(w)[0]
		if first != "alpha" && first != "beta" && first != "gamma" && first != "delta" {
			t.Fatalf("window starts mid-word: %q", first)
		}
	}
}

func TestCorpusSampleIsUniformOverStarts(t *testing.T) {
	// A long and a short article: sampling must favour the long one in
	// proportion to its start positions, otherwise per-table text (and therefore
	// HNSW density) is not production-like.
	long := &Article{Title: "long", TS: "30-APR-2012 03:25:17.000",
		Text: strings.Repeat("word ", 2000), Starts: len(strings.Repeat("word ", 2000)) - minWindow + 1}
	short := &Article{Title: "short", TS: "30-APR-2012 03:25:17.000",
		Text: strings.Repeat("term ", 100), Starts: len(strings.Repeat("term ", 100)) - minWindow + 1}
	c := NewCorpus([]*Article{long, short})

	rng := rand.New(rand.NewSource(7))
	longHits := 0
	const n = 2000
	for i := 0; i < n; i++ {
		art, _ := c.Sample(rng)
		if art.Title == "long" {
			longHits++
		}
	}
	// long has ~9801 starts, short ~301: expect ~97% long.
	got := float64(longHits) / n
	if got < 0.93 || got > 0.99 {
		t.Errorf("sampling not proportional to starts: long share %.3f", got)
	}
}

func TestChunkIDStableAndUnique(t *testing.T) {
	a := chunkID("doc1", "some text")
	b := chunkID("doc1", "some text")
	if a != b {
		t.Errorf("chunkID is not deterministic: %s != %s", a, b)
	}
	if len(a) != 16 {
		t.Errorf("chunkID must be 16 hex chars, got %d (%q)", len(a), a)
	}
	if a == chunkID("doc2", "some text") {
		t.Errorf("chunkID ignores the document id")
	}
	if a == chunkID("doc1", "other text") {
		t.Errorf("chunkID ignores the text")
	}
}

func TestHexArrayFormat(t *testing.T) {
	// Production writes %08x joined by _, and the column is varchar. A raw JSON
	// array would be the wrong type and the import rejects it.
	got := hexArray(1, 40)
	if got != "00000001_00000028" {
		t.Errorf("hexArray = %q", got)
	}
}

func TestTSFields(t *testing.T) {
	text, epoch, err := tsToFields("30-APR-2012 03:25:17.000")
	if err != nil {
		t.Fatal(err)
	}
	if text != "2012-04-30 03:25:17" {
		t.Errorf("create_time = %q", text)
	}
	if epoch < 1.3e9 || epoch > 1.4e9 {
		t.Errorf("create_timestamp_flt = %v, want ~1.33e9", epoch)
	}
}

func TestLoadMapping(t *testing.T) {
	path := os.Getenv("MAPPING")
	if path == "" {
		t.Skip("set MAPPING to RAGFlow's conf/infinity_mapping.json to run this")
	}
	m, err := LoadMapping(path)
	if err != nil {
		t.Fatal(err)
	}
	if len(m.Columns) < 90 {
		t.Fatalf("expected the full production column set, got %d", len(m.Columns))
	}
	// The order must be the file's, because CSV import is positional.
	if m.Columns[0].Name != "id" {
		t.Errorf("first column = %q, want id", m.Columns[0].Name)
	}
	if m.Columns[len(m.Columns)-1].Name != "aliases_flat_kwd" {
		t.Errorf("last column = %q", m.Columns[len(m.Columns)-1].Name)
	}

	ft := m.FullTextIndexes(false)
	if len(ft) != 47 {
		t.Errorf("default variant: %d full-text indexes, want 47", len(ft))
	}
	if n := len(m.FullTextIndexes(true)); n != 48 {
		t.Errorf("+sparsegram variant: %d full-text indexes, want 48", n)
	}
	if n := m.IndexObjects(false); n != 59 {
		t.Errorf("default variant: %d index objects, want 59", n)
	}
	if n := m.IndexObjects(true); n != 60 {
		t.Errorf("+sparsegram variant: %d index objects, want 60", n)
	}

	// content must carry exactly the two rag analyzers by default.
	var content *Column
	for i := range m.Columns {
		if m.Columns[i].Name == "content" {
			content = &m.Columns[i]
		}
	}
	if content == nil {
		t.Fatal("no content column")
	}
	if strings.Join(content.Analyzer, ",") != "rag-coarse,rag-fine" {
		t.Errorf("content analyzers = %v", content.Analyzer)
	}
	if m == nil {
		t.Fatal("nil mapping")
	}
}

func TestBuildRowShape(t *testing.T) {
	path := os.Getenv("MAPPING")
	if path == "" {
		t.Skip("set MAPPING to RAGFlow's conf/infinity_mapping.json to run this")
	}
	m, err := LoadMapping(path)
	if err != nil {
		t.Fatal(err)
	}
	rb := &RowBuilder{Mapping: m, Dim: 1024}
	art := &Article{
		Title: "Anarchism",
		TS:    "30-APR-2012 03:25:17.000",
		Paras: []string{"Anarchism is a political philosophy.", "History", "Origins"},
		Text:  "Anarchism is a political philosophy. History Origins",
	}

	rng := rand.New(rand.NewSource(3))
	row, err := rb.Build(rng, art, art.Text, "doc123", "kb1", 0, 1)
	if err != nil {
		t.Fatal(err)
	}

	// Every mapping column must be present: INSERT sends a sparse map, and a
	// missing column would take the server default rather than the row's value.
	for _, c := range m.Columns {
		if _, ok := row.Values[c.Name]; !ok {
			t.Errorf("row missing column %q", c.Name)
		}
	}

	if got := row.Values["docnm"]; got != "Anarchism" {
		t.Errorf("docnm = %v", got)
	}
	if got, ok := row.Values["create_timestamp_flt"].(float64); !ok || got < 1.3e9 {
		t.Errorf("create_timestamp_flt = %v (%T)", row.Values["create_timestamp_flt"], row.Values["create_timestamp_flt"])
	}
	// position columns are hex strings per production
	if got, ok := row.Values["page_num_int"].(string); !ok || !strings.Contains(got, "_") {
		t.Errorf("page_num_int = %v (%T)", row.Values["page_num_int"], row.Values["page_num_int"])
	}
	// json columns are JSON text, not Go slices
	for _, name := range []string{"source_chunk_ids", "source_chunk_hashes"} {
		s, ok := row.Values[name].(string)
		if !ok {
			t.Errorf("%s = %T, want string of JSON text", name, row.Values[name])
			continue
		}
		if !strings.HasPrefix(s, "[") && !strings.HasPrefix(s, "{") {
			t.Errorf("%s = %q, not JSON", name, s)
		}
	}
	// numeric columns must be Go numbers, not strings, or the SDK packs them as
	// varchar and the insert fails on type.
	if _, ok := row.Values["available_int"].(int64); !ok {
		t.Errorf("available_int = %T, want int64", row.Values["available_int"])
	}
	if _, ok := row.Values["weight_flt"].(float64); !ok {
		t.Errorf("weight_flt = %T, want float64", row.Values["weight_flt"])
	}
	// and the ~82 untouched columns must still be at their declared defaults
	if got := row.Values["removed_kwd"]; got != "" {
		t.Errorf("removed_kwd = %v, want the mapping default", got)
	}
}

func TestPlanAllocationTotals(t *testing.T) {
	for _, name := range []string{"S", "M", "XL"} {
		sc := Scenarios[name]
		a := PlanAllocation(sc, 1, false, "t")
		if a.Total() != sc.Rows {
			t.Errorf("%s: total %d, want %d", name, a.Total(), sc.Rows)
		}
		if len(a.Rows) != sc.Tables {
			t.Errorf("%s: %d tables, want %d", name, len(a.Rows), sc.Tables)
		}
	}
	// L must hit the production chunk count exactly.
	l := PlanAllocation(Scenarios["L"], 1, false, "t")
	if l.Total() != ProdChunks {
		t.Errorf("L: total %d, want %d", l.Total(), ProdChunks)
	}
}

func TestZipfAllocationShape(t *testing.T) {
	sc := Scenarios["L"]
	a := PlanAllocation(sc, 42, true, "t")
	if a.Total() != sc.Rows {
		t.Fatalf("skewed total %d, want %d", a.Total(), sc.Rows)
	}
	rows := append([]int(nil), a.Rows...)
	sortIntsDesc(rows)
	if rows[0] < 300_000 {
		t.Errorf("largest table %d rows, want ~400k", rows[0])
	}
	empty := 0
	for _, r := range a.Rows {
		if r == 0 {
			empty++
		}
	}
	if empty == 0 {
		t.Errorf("skew should leave some tables empty, got 0")
	}
	if len(a.NonEmpty()) == 0 || len(a.NonEmpty()) == len(a.Rows) {
		t.Errorf("NonEmpty = %d of %d", len(a.NonEmpty()), len(a.Rows))
	}
}

func sortIntsDesc(xs []int) {
	for i := 1; i < len(xs); i++ {
		for j := i; j > 0 && xs[j] > xs[j-1]; j-- {
			xs[j], xs[j-1] = xs[j-1], xs[j]
		}
	}
}

func TestLoadCorpusRealFile(t *testing.T) {
	path := filepath.Join("..", "..", "test", "data", "benchmark", "enwiki", "enwiki-10w.csv")
	if _, err := os.Stat(path); err != nil {
		t.Skipf("corpus not present at %s", path)
	}
	arts, sha, err := LoadCorpus(path, PinnedWikiSHA256)
	if err != nil {
		t.Fatalf("LoadCorpus: %v", err)
	}
	if sha != PinnedWikiSHA256 {
		t.Errorf("sha = %s", sha)
	}
	if len(arts) < 3000 {
		t.Errorf("%d articles, want ~3643", len(arts))
	}
	c := NewCorpus(arts)
	if got := c.Starts(); got < 60_000_000 {
		t.Errorf("%d start positions, want ~70.7M", got)
	}

	// Every sampled window must be clean prose: no markup, sane length.
	rng := rand.New(rand.NewSource(11))
	for i := 0; i < 500; i++ {
		_, w := c.Sample(rng)
		if len(w) < minWindow || len(w) > maxWindow+1 {
			t.Fatalf("window length %d out of range", len(w))
		}
		for _, bad := range []string{"{{", "[[", "<ref", "=="} {
			if strings.Contains(w, bad) {
				t.Fatalf("window still contains %q: %.80s", bad, w)
			}
		}
	}
}

func TestEmbedderLoadsAndNormalises(t *testing.T) {
	model := os.Getenv("M2V_MODEL")
	if model == "" {
		t.Skip("set M2V_MODEL to the pinned model directory (e.g. /tmp/m2vmodel)")
	}
	emb, err := LoadEmbedder(model, 1024)
	if err != nil {
		t.Fatalf("LoadEmbedder: %v", err)
	}
	defer emb.Close()

	if emb.Dim() != 1024 {
		t.Fatalf("dim = %d", emb.Dim())
	}
	texts := []string{
		"Anarchism is a political philosophy that rejects the state.",
		"The cat sat on the mat.",
		"Anarchism is a political philosophy that rejects the state.",
	}
	vecs, err := emb.Encode(texts, 512)
	if err != nil {
		t.Fatalf("Encode: %v", err)
	}
	if len(vecs) != 3*1024 {
		t.Fatalf("got %d floats, want %d", len(vecs), 3*1024)
	}

	// Identical text must give identical vectors: the corpus relies on it, and a
	// non-deterministic embedder would make repeat runs incomparable.
	if !sameFloats(vecs[0:1024], vecs[2048:3072]) {
		t.Error("identical text produced different vectors")
	}
	// The model is L2-normalised (config "normalize": true), which Q1/Q3 assume
	// for cosine distance.
	if n := norm(vecs[0:1024]); n < 0.999 || n > 1.001 {
		t.Errorf("first vector has norm %.5f, want 1.0", n)
	}
	// Unrelated sentences must not be identical.
	if sameFloats(vecs[0:1024], vecs[1024:2048]) {
		t.Error("unrelated texts produced identical vectors")
	}
}

func TestEmbedderRejectsWrongDim(t *testing.T) {
	model := os.Getenv("M2V_MODEL")
	if model == "" {
		t.Skip("set M2V_MODEL to the pinned model directory")
	}
	// A 1024-d model asked for 768 must fail loudly: the table column would be
	// q_768_vec and the import would reject the file.
	if _, err := LoadEmbedder(model, 768); err == nil {
		t.Fatal("LoadEmbedder accepted a dimension mismatch")
	}
}

func TestEmbedderConcurrentUse(t *testing.T) {
	model := os.Getenv("M2V_MODEL")
	if model == "" {
		t.Skip("set M2V_MODEL to the pinned model directory")
	}
	emb, err := LoadEmbedder(model, 1024)
	if err != nil {
		t.Fatal(err)
	}
	defer emb.Close()

	// The handle must be usable from several goroutines; a data race here would
	// be a cgo lifetime bug that only shows under load.
	done := make(chan error, 8)
	for g := 0; g < 8; g++ {
		go func(g int) {
			vecs, err := emb.Encode([]string{"text a", "text b", "text c"}, 512)
			if err != nil {
				done <- err
				return
			}
			if len(vecs) != 3*1024 {
				done <- errShort(len(vecs))
				return
			}
			done <- nil
		}(g)
	}
	for i := 0; i < 8; i++ {
		if err := <-done; err != nil {
			t.Fatalf("concurrent encode: %v", err)
		}
	}
}

type errShort int

func (e errShort) Error() string { return "short vector result" }

func sameFloats(a, b []float32) bool {
	if len(a) != len(b) {
		return false
	}
	for i := range a {
		if a[i] != b[i] {
			return false
		}
	}
	return true
}

func norm(v []float32) float64 {
	s := 0.0
	for _, x := range v {
		s += float64(x) * float64(x)
	}
	return sqrt(s)
}

func sqrt(x float64) float64 {
	// Newton's method keeps the test dependency-free.
	if x <= 0 {
		return 0
	}
	z := x
	for i := 0; i < 32; i++ {
		z -= (z*z - x) / (2 * z)
	}
	return z
}

func TestParseScenarioScale(t *testing.T) {
	cases := []struct {
		in     string
		tables int
		rows   int
		name   string
		wantErr bool
	}{
		{in: "S", tables: 50, rows: 50 * 1000, name: "S"},
		{in: "L", tables: ProdTables, rows: ProdChunks, name: "L"},
		{in: "L/100", tables: ProdTables / 100, rows: ProdChunks / 100, name: "L/100"},
		{in: "M/10", tables: 60, rows: 600 * 973 / 10, name: "M/10"},
		{in: "S:10x200", tables: 10, rows: 2000, name: "S"},
		{in: "S:10x200/2", wantErr: true},
		{in: "L/0", wantErr: true},
		{in: "nope", wantErr: true},
	}
	for _, c := range cases {
		sc, err := parseScenario(c.in)
		if c.wantErr {
			if err == nil {
				t.Errorf("%q: expected an error, got %+v", c.in, sc)
			}
			continue
		}
		if err != nil {
			t.Errorf("%q: %v", c.in, err)
			continue
		}
		if sc.Tables != c.tables || sc.Rows != c.rows || sc.Name != c.name {
			t.Errorf("%q -> %+v, want tables=%d rows=%d name=%q", c.in, sc, c.tables, c.rows, c.name)
		}
	}
}

// TestScaledPlanKeepsShape guards the reason scale divides both dimensions: a
// 1/100 run must keep ~1,417 rows per table and ~20 chunks per document, or it
// measures a different workload.
func TestScaledPlanKeepsShape(t *testing.T) {
	full := PlanAllocation(Scenarios["L"], 1, false, "t")
	scaled, err := parseScenario("L/100")
	if err != nil {
		t.Fatal(err)
	}
	a := PlanAllocation(scaled, 1, false, "t")
	if len(a.Rows) != 58 {
		t.Errorf("tables = %d, want 58", len(a.Rows))
	}
	fullPerTable := float64(full.Total()) / float64(len(full.Rows))
	scaledPerTable := float64(a.Total()) / float64(len(a.Rows))
	if diff := scaledPerTable - fullPerTable; diff > 30 || diff < -30 {
		t.Errorf("rows/table changed with scale: %.0f -> %.0f", fullPerTable, scaledPerTable)
	}
}
