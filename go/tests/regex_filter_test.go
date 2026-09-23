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

import (
	"fmt"
	"reflect"
	"sort"
	"testing"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

// TestRegexFilterShape checks the filter expression the SDK builds.
func TestRegexFilterShape(t *testing.T) {
	cases := []struct {
		column   string
		pattern  string
		expected string
	}{
		{"doc", "colou?r of the (sky|sea)", "regex(doc, 'colou?r of the (sky|sea)')"},
		{"doc", `fn\s+main`, `regex(doc, 'fn\s+main')`},
		{"doc", `\d{2,4}[A-Z]`, `regex(doc, '\d{2,4}[A-Z]')`},
	}
	for _, c := range cases {
		if got := infinity.RegexFilter(c.column, c.pattern); got != c.expected {
			t.Errorf("RegexFilter(%q, %q) = %q, want %q", c.column, c.pattern, got, c.expected)
		}
	}
}

// TestSparsegramAnalyzerNames checks the analyzer name built for the index.
func TestSparsegramAnalyzerNames(t *testing.T) {
	name, err := infinity.SparsegramAnalyzer(3, 12, false)
	if err != nil || name != "sparsegram-3-12" {
		t.Errorf("SparsegramAnalyzer(3, 12, false) = %q, %v", name, err)
	}
	name, err = infinity.SparsegramAnalyzer(1, 8, true)
	if err != nil || name != "sparsegram-1-8-fold" {
		t.Errorf("SparsegramAnalyzer(1, 8, true) = %q, %v", name, err)
	}
	for _, c := range [][2]int{{9, 3}, {0, 12}} {
		if _, err := infinity.SparsegramAnalyzer(c[0], c[1], false); err == nil {
			t.Errorf("SparsegramAnalyzer(%d, %d) should fail", c[0], c[1])
		}
	}
}

// TestNewSparsegramIndexInfo checks the full-text index it configures.
func TestNewSparsegramIndexInfo(t *testing.T) {
	info, err := infinity.NewSparsegramIndexInfo("doc", 3, 12, true)
	if err != nil {
		t.Fatalf("NewSparsegramIndexInfo failed: %v", err)
	}
	if info.TargetName != "doc" || info.IndexType != infinity.IndexTypeFullText {
		t.Errorf("unexpected index info: %+v", info)
	}
	if info.Params["analyzer"] != "sparsegram-3-12-fold" {
		t.Errorf("unexpected analyzer: %q", info.Params["analyzer"])
	}
}

// TestQuoteStringLiteralEscapesQuote pins the only escaping a filter needs:
// a backslash stays as it is, because a regular expression is built from them.
func TestQuoteStringLiteralEscapesQuote(t *testing.T) {
	if got := infinity.QuoteStringLiteral("it's"); got != "'it''s'" {
		t.Errorf("QuoteStringLiteral = %q, want %q", got, "'it''s'")
	}
	if got := infinity.QuoteStringLiteral(`a\b`); got != `'a\b'` {
		t.Errorf("QuoteStringLiteral = %q, want %q", got, `'a\b'`)
	}
}

// TestRegexFilterSparsegramIndex runs the filter against a live server, on a
// column that carries both a regular full-text index and a sparse gram one.
func TestRegexFilterSparsegramIndex(t *testing.T) {
	tableName := "test_regex_filter_sparsegram"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}
	if _, err = db.DropTable(tableName, infinity.ConflictTypeIgnore); err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	table, err := db.CreateTable(tableName, infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "doc", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// A full-text index only sees the rows that exist when it is built.
	if _, err = table.Insert([]map[string]interface{}{
		{"id": 1, "doc": "harmful chemical reaction"},
		{"id": 2, "doc": "chemical processes are harmful"},
		{"id": 3, "doc": "nothing here"},
		{"id": 4, "doc": "fn main() { return 0; }"},
	}); err != nil {
		t.Fatalf("Failed to insert rows: %v", err)
	}

	std := infinity.NewIndexInfo("doc", infinity.IndexTypeFullText, map[string]string{"analyzer": "standard"})
	if _, err = table.CreateIndex("regex_ft_std", std, infinity.ConflictTypeError, ""); err != nil {
		t.Fatalf("Failed to create the regular index: %v", err)
	}
	sg, err := infinity.NewSparsegramIndexInfo("doc", 3, 12, true)
	if err != nil {
		t.Fatalf("Failed to build the sparse gram index info: %v", err)
	}
	if _, err = table.CreateIndex("regex_ft_sg", sg, infinity.ConflictTypeError, ""); err != nil {
		t.Fatalf("Failed to create the sparse gram index: %v", err)
	}

	ids := func(filter string) []string {
		t.Helper()
		res, err := table.Output([]string{"id"}).Filter(filter).ToResult()
		if err != nil {
			t.Fatalf("query %q failed: %v", filter, err)
		}
		result, ok := res.(*infinity.QueryResult)
		if !ok {
			t.Fatalf("query %q returned %T", filter, res)
		}
		out := make([]string, 0, len(result.Data["id"]))
		for _, v := range result.Data["id"] {
			out = append(out, fmt.Sprint(v))
		}
		sort.Strings(out)
		return out
	}

	cases := []struct {
		filter string
		want   []string
	}{
		// `regex()` is answered by the column's sparse gram index.
		{infinity.RegexFilter("doc", `chemical (reaction|processes)`), []string{"1", "2"}},
		// A backslash escape reaches the server unchanged.
		{infinity.RegexFilter("doc", `fn\s+main`), []string{"4"}},
		// `(?i)` needs the folded index, which this one is built with.
		{infinity.RegexFilter("doc", `(?i)CHEMICAL`), []string{"1", "2"}},
		// An unqualified fulltext filter goes to the regular analyzer: row 2 has
		// the two words apart, which the grams of `harmful chemical` would miss.
		{"filter_fulltext('doc', 'harmful chemical', 'operator=and')", []string{"1", "2"}},
		// Naming the gram index changes the answer, which is what shows the two
		// indexes really are separate.
		{"filter_fulltext('doc@regex_ft_sg', 'harmful chemical', 'operator=and')", []string{"1"}},
	}
	for _, c := range cases {
		if got := ids(c.filter); !reflect.DeepEqual(got, c.want) {
			t.Errorf("filter %q = %v, want %v", c.filter, got, c.want)
		}
	}

	// The retrieval pipeline takes the regex as its own filter.
	res, err := table.Output([]string{"id"}).
		MatchText("doc", "harmful chemical", 5, nil).
		Filter(infinity.RegexFilter("doc", "reaction")).
		ToResult()
	if err != nil {
		t.Fatalf("retrieval with a regex filter failed: %v", err)
	}
	result, ok := res.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("retrieval returned %T", res)
	}
	if len(result.Data["id"]) != 1 || fmt.Sprint(result.Data["id"][0]) != "1" {
		t.Errorf("retrieval with a regex filter = %v, want [1]", result.Data["id"])
	}

	if _, err := db.DropTable(tableName, infinity.ConflictTypeError); err != nil {
		t.Errorf("Failed to drop table: %v", err)
	}
}
