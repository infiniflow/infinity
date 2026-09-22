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
