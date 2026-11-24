package loadtest

// Schema and row shaping: the table is RAGFlow's production chunk table, read
// from its own mapping file rather than re-declared here.
//
// Why read the file instead of hard-coding 92 columns: the CSV import path
// (and the table definition) depends on column order, and the mapping has
// already drifted once during this plan's life (`md_with_weight` inserted at
// #56, `content` losing `sparsegram`). A hard-coded list silently mis-maps in
// that situation; reading the file cannot.

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"os"
	"regexp"
	"strings"
)

// Column is one entry of RAGFlow's conf/infinity_mapping.json.
type Column struct {
	Name     string
	Type     string
	Default  interface{}
	Analyzer []string
	Enabled  bool
	HasIdx   bool // index_type present
}

// AnalyserNames normalises the analyzer field, which is a string, a list, or
// absent.
func (c Column) AnalyserNames() []string { return c.Analyzer }

// Mapping is the ordered column list from the RAGFlow mapping file.
type Mapping struct {
	Columns []Column
	byName  map[string]Column
	SHA256  string
}

// IsJSON reports whether the column is stored as a json column. It matters for
// the CSV path: a json column takes JSON *text* in a cell, and Parquet cannot
// carry it at all (see docs/load_test_plan_ragflow_prod.md section 4.3).
func (c Column) IsJSON() bool { return c.Type == "json" }

// LoadMapping parses the mapping file, preserving file order.
func LoadMapping(path string) (*Mapping, error) {
	raw, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	// encoding/json loses object order, and order is the whole point, so the
	// keys are pulled out with a streaming decoder instead.
	dec := json.NewDecoder(strings.NewReader(string(raw)))
	tok, err := dec.Token()
	if err != nil {
		return nil, err
	}
	if d, ok := tok.(json.Delim); !ok || d != '{' {
		return nil, fmt.Errorf("%s: expected a JSON object of column -> spec", path)
	}

	m := &Mapping{byName: make(map[string]Column, 128), SHA256: sha256Hex(raw)}
	for dec.More() {
		keyTok, err := dec.Token()
		if err != nil {
			return nil, err
		}
		name, _ := keyTok.(string)

		var spec struct {
			Type     string          `json:"type"`
			Default  json.RawMessage `json:"default"`
			Analyzer json.RawMessage `json:"analyzer"`
			Enabled  *bool           `json:"enabled"`
			IndexTyp json.RawMessage `json:"index_type"`
		}
		if err := dec.Decode(&spec); err != nil {
			return nil, fmt.Errorf("%s: column %q: %w", path, name, err)
		}

		col := Column{Name: name, Type: spec.Type, Enabled: true}
		if spec.Enabled != nil {
			col.Enabled = *spec.Enabled
		}
		col.HasIdx = len(spec.IndexTyp) > 0 && string(spec.IndexTyp) != "null"

		// default is a string, number or bool depending on the column type.
		if len(spec.Default) > 0 && string(spec.Default) != "null" {
			var v interface{}
			if err := json.Unmarshal(spec.Default, &v); err == nil {
				col.Default = v
			}
		}

		// analyzer is either "rag-fine" or ["rag-coarse","rag-fine"].
		if len(spec.Analyzer) > 0 && string(spec.Analyzer) != "null" {
			var one string
			if err := json.Unmarshal(spec.Analyzer, &one); err == nil {
				col.Analyzer = []string{one}
			} else {
				var many []string
				if err := json.Unmarshal(spec.Analyzer, &many); err != nil {
					return nil, fmt.Errorf("%s: column %q analyzer: %w", path, name, err)
				}
				col.Analyzer = many
			}
		}

		m.Columns = append(m.Columns, col)
		m.byName[name] = col
	}
	if _, err := dec.Token(); err != nil { // closing brace
		return nil, err
	}
	if len(m.Columns) == 0 {
		return nil, fmt.Errorf("%s: no columns", path)
	}
	return m, nil
}

// FullTextIndexes is one (column, analyzer) pair per full-text index object.
func (m *Mapping) FullTextIndexes(sparsegram bool) [][2]string {
	var out [][2]string
	for _, c := range m.Columns {
		names := c.Analyzer
		if sparsegram && c.Name == "content" && !contains(names, "sparsegram-3-12-fold") {
			names = append(append([]string{}, names...), "sparsegram-3-12-fold")
		}
		for _, a := range names {
			out = append(out, [2]string{c.Name, a})
		}
	}
	return out
}

// SecondaryIndexes lists the columns carrying index_type: secondary. Columns
// with both an analyzer and an index_type count as full-text, matching the
// plan's precedence.
func (m *Mapping) SecondaryIndexes() []string {
	var out []string
	for _, c := range m.Columns {
		if len(c.Analyzer) == 0 && c.HasIdx {
			out = append(out, c.Name)
		}
	}
	return out
}

// IndexObjects is the per-table index count: full-text + secondary + HNSW.
func (m *Mapping) IndexObjects(sparsegram bool) int {
	return len(m.FullTextIndexes(sparsegram)) + len(m.SecondaryIndexes()) + 1
}

// VectorColumn is the name production gives the embedding column: q_<dim>_vec.
// It is not in the mapping file; RAGFlow appends it at CREATE TABLE
// (internal/engine/infinity/chunk.go:165).
func VectorColumn(dim int) string { return fmt.Sprintf("q_%d_vec", dim) }

// ColumnTypes is the ordered `name type` list the table needs, plus the vector
// column. The engine's DDL takes the mapping's own type strings verbatim:
// `varchar`, `integer`, `float`, `json`.
func (m *Mapping) ColumnTypes(dim int) ([][2]string, error) {
	out := make([][2]string, 0, len(m.Columns)+1)
	for _, c := range m.Columns {
		switch c.Type {
		case "varchar", "integer", "float", "json":
			out = append(out, [2]string{c.Name, c.Type})
		default:
			return nil, fmt.Errorf("column %q has unsupported mapping type %q", c.Name, c.Type)
		}
	}
	// The vector column uses the SDK's `vector,<dim>,<elem>` spelling, not the
	// SQL `EMBEDDING(FLOAT, <dim>)` one: the Go SDK passes the DataType string
	// straight through and the server's type parser rejects the latter with
	// "unsupported data type: embedding(float)". This is the same spelling
	// RAGFlow uses (internal/engine/infinity/chunk.go:166).
	out = append(out, [2]string{VectorColumn(dim), fmt.Sprintf("vector,%d,float", dim)})
	return out, nil
}

// Row is one chunk, ready to insert. Values are Go-native types so the SDK's
// constant parser sees int64/float64/string rather than strings it would have to
// re-parse.
type Row struct {
	Values map[string]interface{}
	Text   string // the content, kept for batching the embedder
}

// RowBuilder turns a sampled window into a production-shaped row.
type RowBuilder struct {
	Mapping *Mapping
	Dim     int
}

// Build fills the ~10 columns production actually populates and leaves the other
// ~82 at the mapping default.
//
// That sparsity is the point: a table where all 92 columns hold data charges an
// index-maintenance cost per row that production never pays, so filling
// everything would make the ingest numbers pessimistic in a way that does not
// map to the migration.
func (rb *RowBuilder) Build(rng *rand.Rand, art *Article, text, docID, kbID string, order, page int) (Row, error) {
	createTime, createTS, err := tsToFields(art.TS)
	if err != nil {
		return Row{}, err
	}

	important := importantKeywords(art)

	vals := make(map[string]interface{}, len(rb.Mapping.Columns))
	for _, c := range rb.Mapping.Columns {
		// Start from the mapping default so the row is always complete; the
		// specific columns below overwrite it.
		switch c.Type {
		case "integer":
			vals[c.Name] = toInt64(c.Default)
		case "float":
			vals[c.Name] = toFloat64(c.Default)
		case "json":
			// Defaults for json columns are JSON text ("[]" / "{}"); the engine
			// parses them, so a Go slice would confuse the constant parser.
			vals[c.Name] = jsonDefault(c)
		default:
			vals[c.Name] = toStringDefault(c.Default)
		}
	}

	set := func(name string, v interface{}) {
		if _, ok := rb.Mapping.byName[name]; ok {
			vals[name] = v
		}
	}

	set("id", chunkID(docID, text))
	set("doc_id", docID)
	set("kb_id", kbID)
	set("content", text)
	set("docnm", art.Title)
	set("title_kwd", art.Title)
	set("important_keywords", important)
	set("create_time", createTime)
	set("create_timestamp_flt", createTS)
	set("chunk_order_int", int64(order))
	set("available_int", int64(1))
	// position columns are hex strings, not arrays (section 4.2.3)
	set("page_num_int", hexArray(uint32(page), uint32(page+1)))
	set("top_int", hexArray(uint32(order*40), uint32(order*40+40)))
	set("position_int", hexArray(uint32(page), uint32(order*40), 100, 40))
	set("token_num", int64(len(strings.Fields(text))))
	set("weight_int", int64(1))
	set("weight_flt", 1.0)
	set("rank_int", int64(0))
	set("rank_flt", 0.0)
	set("pagerank_fea", int64(rng.Intn(101)))
	set("page_version_int", int64(1))
	set("mom_id", "")

	return Row{Values: vals, Text: text}, nil
}

// importantKeywords is 0-3 short paragraphs joined by `###`, which is how
// production joins keywords (not commas).
func importantKeywords(art *Article) string {
	var picked []string
	for _, p := range art.Paras {
		if len(strings.Fields(p)) <= 6 {
			picked = append(picked, p)
			if len(picked) == 3 {
				break
			}
		}
	}
	if len(picked) == 0 {
		return art.Title
	}
	return strings.Join(picked, "###")
}

func jsonDefault(c Column) string {
	if s, ok := c.Default.(string); ok && s != "" {
		return s
	}
	if c.Name == "source_chunk_hashes" {
		return "{}"
	}
	return "[]"
}

func toStringDefault(v interface{}) string {
	switch t := v.(type) {
	case string:
		return t
	case nil:
		return ""
	default:
		return fmt.Sprint(t)
	}
}

func toInt64(v interface{}) int64 {
	switch t := v.(type) {
	case float64:
		return int64(t)
	case int64:
		return t
	case json.Number:
		if n, err := t.Int64(); err == nil {
			return n
		}
	}
	return 0
}

func toFloat64(v interface{}) float64 {
	switch t := v.(type) {
	case float64:
		return t
	case json.Number:
		if f, err := t.Float64(); err == nil {
			return f
		}
	}
	return 0
}

func contains(hay []string, needle string) bool {
	for _, s := range hay {
		if s == needle {
			return true
		}
	}
	return false
}

var reNonAlnum = regexp.MustCompile(`[^A-Za-z0-9]`)

// SafeIndexName renders `ft_<column>_<analyzer>` the way RAGFlow does
// (internal/engine/infinity/chunk.go:207-212), so the names match production.
func SafeIndexName(prefix, column, analyzer string) string {
	return prefix + reNonAlnum.ReplaceAllString(column, "_") + "_" + reNonAlnum.ReplaceAllString(analyzer, "_")
}
