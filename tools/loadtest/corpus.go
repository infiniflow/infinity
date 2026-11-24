package loadtest

// Corpus preparation: turn the enwiki extract into load-test rows.
//
// The rules here are the plan's section 4.2, and each one exists because the
// obvious alternative was measured and was wrong:
//
//   - The extract is raw wikitext, not prose (12.1% templates, 14.5% <ref>,
//     17.3% [[links]]), so it must be cleaned or the inverted index is mostly
//     citation markup and `minimum_should_match` denominators mean nothing.
//   - Windows are cut at word boundaries and stripped of leftover section
//     headings (`== Pain ==`), otherwise `content` starts mid-word and the
//     analyzer indexes `==` as tokens.
//   - Start positions are drawn uniformly over each article's whole cleaned
//     body (~70.7M starts for this corpus). Enumerating them is unnecessary and
//     coarse strides make the corpus more repetitive than production.

import (
	"bufio"
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"hash/fnv"
	"io"
	"math/rand"
	"os"
	"regexp"
	"sort"
	"strings"
	"time"
	"unicode/utf8"
)

// PinnedWikiSHA256 is the corpus this plan was written against. Pass an empty
// expected sum to skip the check.
const PinnedWikiSHA256 = "72a981c39339c9ec6bfcc5853bb9bebe1db7a9b3c62a1d250532a58d5b2249d7"

const (
	minWindow      = 200
	maxWindow      = 1500
	minCleanChars  = 40 // rows below this carry nothing retrievable
	chunksPerDoc   = 20 // production: 8.34M chunks / 405,617 docs = 20.6
	fallbackTokens = 512
)

var (
	reRefSelf  = regexp.MustCompile(`<ref[^>]*/>`)
	reRefPair  = regexp.MustCompile(`(?s)<ref[^>]*>.*?</ref>`)
	reMathPair = regexp.MustCompile(`(?si)<math[^>]*>.*?</math>`)
	reMathSelf = regexp.MustCompile(`(?i)<math[^>]*/>`)
	reTable    = regexp.MustCompile(`(?s)\{\|.*?\|\}`)
	reTemplate = regexp.MustCompile(`\{\{[^{}]*\}\}`)
	reWikiLink = regexp.MustCompile(`\[\[([^\[\]]*)\]\]`)
	reEmphasis = regexp.MustCompile(`'{2,}`)
	reExtLink  = regexp.MustCompile(`\[https?://\S+\s+([^\]]*)\]`)
	reTag      = regexp.MustCompile(`<[^>]+>`)
	reHeading  = regexp.MustCompile(`={2,}[^=]{1,80}={2,}`)
	// reTableLine drops a wikitable row: these start with `|` and carry
	// parameter noise (`|year=1994 |title=...`) rather than prose.
	reTableLine = regexp.MustCompile(`(?m)^[ \t]*\|[^\n]*$`)
	// reEntity drops HTML entities. `&nbsp;` is the common one, and left in place
	// it becomes a term the engine refuses to match: a query built from such a
	// window fails with "Trying to match: ... :n&nbsp;=&nbsp;4 ... failed"
	// (measured), because the tokenizer sees `&nbsp;` as a word fragment.
	reEntity = regexp.MustCompile(`&#?\w+;`)
	// reSymbols drops runs of MathJax/ASCII math punctuation. Prose in this
	// corpus is interleaved with formula fragments; they are not words and the
	// full-text engine rejects a query made of them.
	reSymbols = regexp.MustCompile("[=+^~|<>\\[\\]{}()\\\\]+")
	// reInterwiki drops the multilingual title list at the end of an article
	// ("te:అడిలైడ్ th:แอดิเลด tr:Adelaide zh:阿德莱德 ..."). These are not prose,
	// and they are the tail of every article in this extract, so a sampled window
	// near the end landed in them (measured: a 190-char window of nothing but
	// interlanguage links).
	reInterwiki = regexp.MustCompile(`(?:\b[a-z]{2,3}(?:-[a-z]{2,10})?:[^\s]+)(?:\s+[a-z]{2,3}(?:-[a-z]{2,10})?:[^\s]+)+`)
	reSpaces    = regexp.MustCompile(`\s+`)
	reUnbalanced = strings.NewReplacer(
		"[[", " ", "]]", " ",
		"{{", " ", "}}", " ",
		"{|", " ", "|}", " ",
		"{", " ", "}", " ",
	)
)

// Article is one Wikipedia article, as the extract groups it.
type Article struct {
	Title  string
	TS     string   // "30-APR-2012 03:25:17.000"
	Paras  []string // cleaned paragraphs
	Text   string   // cleaned body, paragraphs joined
	Starts int      // valid window start positions
}

// CleanText strips wikitext markup, keeping the prose.
//
// It must be given a whole article, not one extract row: the extract splits
// articles at arbitrary points, so a `{{citation` opened in one row closes in a
// later one. Cleaning per row left 14% of rows with unbalanced markup (measured
// on this corpus); cleaning per article drops that to 0.4%.
//
// Templates are removed repeatedly because they nest (`{{clade|1={{clade|...`)
// and one pass leaves the inner braces behind, which then show up as tokens.
func CleanText(raw string) string {
	t := reRefSelf.ReplaceAllString(raw, "")
	t = reRefPair.ReplaceAllString(t, "")
	// <math> bodies are LaTeX; left in place, `\frac`, `\left` and stray braces
	// become the most frequent terms in the index.
	t = reMathPair.ReplaceAllString(t, " ")
	t = reMathSelf.ReplaceAllString(t, " ")
	t = reTable.ReplaceAllString(t, " ")
	for i := 0; i < 12; i++ {
		next := reTemplate.ReplaceAllString(t, "")
		if next == t {
			break
		}
		t = next
	}
	t = reWikiLink.ReplaceAllStringFunc(t, func(m string) string {
		inner := m[2 : len(m)-2]
		if i := strings.LastIndex(inner, "|"); i >= 0 {
			return inner[i+1:]
		}
		return inner
	})
	t = reEmphasis.ReplaceAllString(t, "")
	t = reExtLink.ReplaceAllStringFunc(t, func(m string) string {
		rest := strings.TrimPrefix(strings.TrimPrefix(m, "[http://"), "[https://")
		if i := strings.IndexByte(rest, ' '); i >= 0 {
			return rest[i+1:]
		}
		return ""
	})
	t = reTag.ReplaceAllString(t, "")
	t = stripHeadings(t)
	t = reTableLine.ReplaceAllString(t, " ")
	t = reEntity.ReplaceAllString(t, " ")
	t = reInterwiki.ReplaceAllString(t, " ")
	t = reSymbols.ReplaceAllString(t, " ")
	// Whatever braces remain belong to a construct whose opener was cut off by
	// the extract. Drop the punctuation, keep the words.
	t = reUnbalanced.Replace(t)
	t = reSpaces.ReplaceAllString(t, " ")
	return strings.TrimSpace(t)
}

// stripHeadings removes `== Section ==` markers but keeps the section title as
// plain words, so a window that begins at a heading does not index "==".
func stripHeadings(s string) string {
	return reHeading.ReplaceAllStringFunc(s, func(m string) string {
		return " " + strings.TrimSpace(strings.Trim(m, "= ")) + " "
	})
}

// LoadCorpus reads the 3-column TSV extract and returns the articles plus the
// file's SHA-256. Rows whose cleaned text is shorter than minCleanChars are
// dropped: they are page furniture, and a chunk nobody could retrieve is not a
// useful row.
func LoadCorpus(path, expectSHA string) ([]*Article, string, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, "", err
	}
	defer f.Close()

	h := sha256.New()
	if _, err := io.Copy(h, f); err != nil {
		return nil, "", err
	}
	sum := hex.EncodeToString(h.Sum(nil))
	if expectSHA != "" && sum != expectSHA {
		return nil, sum, fmt.Errorf(
			"corpus sha256 mismatch: %s != %s (the plan pins the file by hash; "+
				"regenerate the manifest deliberately if the corpus is meant to change)",
			sum, expectSHA)
	}

	if _, err := f.Seek(0, io.SeekStart); err != nil {
		return nil, sum, err
	}

	var articles []*Article
	byTitle := make(map[string]*Article, 4096)
	raw := make(map[string]*strings.Builder, 4096)
	sc := bufio.NewScanner(f)
	sc.Buffer(make([]byte, 0, 1<<20), 1<<26) // rows are ~1 KB; be generous
	for sc.Scan() {
		parts := strings.SplitN(sc.Text(), "\t", 3)
		if len(parts) != 3 {
			continue
		}
		title, ts, body := parts[0], parts[1], parts[2]
		art, ok := byTitle[title]
		if !ok {
			art = &Article{Title: title, TS: ts}
			byTitle[title] = art
			articles = append(articles, art)
			raw[title] = &strings.Builder{}
		}
		// Join with a newline so that table rows (`|a || b`) still start a line
		// and the table-line rule can see them.
		raw[title].WriteString(body)
		raw[title].WriteByte('\n')
	}
	if err := sc.Err(); err != nil {
		return nil, sum, err
	}

	kept := articles[:0]
	total := 0
	for _, a := range articles {
		// Clean the whole article, not each row: see CleanText's comment.
		a.Text = CleanText(raw[a.Title].String())
		a.Starts = len(a.Text) - minWindow + 1
		if a.Starts < 1 {
			a.Starts = 0
			continue
		}
		a.Paras = splitParagraphs(a.Text)
		total += a.Starts
		kept = append(kept, a)
	}
	if total == 0 {
		return nil, sum, fmt.Errorf("no usable windows; is %s the expected 3-column TSV?", path)
	}
	return kept, sum, nil
}

// splitParagraphs turns the cleaned body back into sentence-ish units for the
// short fields (important_keywords takes 0-3 of them).
func splitParagraphs(text string) []string {
	parts := strings.Split(text, ". ")
	out := make([]string, 0, len(parts))
	for _, p := range parts {
		if p = strings.TrimSpace(p); p != "" {
			out = append(out, p)
		}
	}
	if len(out) == 0 {
		return []string{text}
	}
	return out
}

// Corpus is the article set with a cumulative index for O(log n) sampling.
type Corpus struct {
	Articles []*Article
	cum      []int
}

// NewCorpus precomputes the cumulative start counts.
func NewCorpus(articles []*Article) *Corpus {
	c := &Corpus{Articles: articles, cum: make([]int, len(articles))}
	total := 0
	for i, a := range articles {
		total += a.Starts
		c.cum[i] = total
	}
	return c
}

// Starts is the number of distinct window start positions across the corpus.
func (c *Corpus) Starts() int {
	if len(c.cum) == 0 {
		return 0
	}
	return c.cum[len(c.cum)-1]
}

// Sample draws a uniform (article, start) and returns the window text.
//
// Uniform over starts, not over articles: long articles contribute more rows,
// which is what production looks like and what makes per-table HNSW density
// realistic. Drawing per article instead would flatten it.
func (c *Corpus) Sample(rng *rand.Rand) (art *Article, text string) {
	// Retry rather than return a short window: cutWindow returns "" when the
	// drawn span contains no usable prose, and callers must never see a row
	// below minWindow (the row shape and the query set both assume from it).
	for attempt := 0; attempt < 32; attempt++ {
		pick := rng.Intn(c.Starts())
		i := sort.SearchInts(c.cum, pick+1)
		base := 0
		if i > 0 {
			base = c.cum[i-1]
		}
		art = c.Articles[i]
		start := pick - base
		length := len(art.Text) - start
		if length > maxWindow {
			length = maxWindow
		}
		if w := cutWindow(art.Text, start, length); w != "" {
			return art, w
		}
	}
	// Last resort: the longest window in the corpus, which is always usable.
	best := c.Articles[len(c.Articles)-1]
	return best, cutWindow(best.Text, 0, min(maxWindow, len(best.Text)))
}

// cutWindow takes [start,start+length) and pulls the end back to a word
// boundary, so a window never ends mid-word.
//
// The end is only moved back while the result still satisfies minWindow: a
// single very long word near the end would otherwise shrink a 1,500-char window
// below 200 chars, and the callers (and the tests) rely on the floor. Measured
// before this guard: 190-char windows appeared on this corpus.
func cutWindow(text string, start, length int) string {
	end := start + length
	if end > len(text) {
		end = len(text)
	}
	if end < len(text) {
		if sp := strings.LastIndexByte(text[start:end], ' '); sp > 0 && start+sp >= minWindow {
			end = start + sp
		}
	}
	// Do not start mid-word: step forward to the next space when the byte before
	// the start is not a separator.
	for start < end && start > 0 && !isBoundary(text[start-1]) {
		start++
	}
	// A start advanced past the end (a run of non-space bytes) or a text whose
	// multibyte characters got cut leaves nothing usable; callers treat "" as a
	// discarded sample rather than a short row.
	if end-start < minWindow {
		return ""
	}
	w := strings.TrimSpace(text[start:end])
	if len(w) < minWindow || !utf8.ValidString(w) {
		return ""
	}
	return w
}

func isBoundary(b byte) bool {
	return b == ' ' || b == '\t' || b == '\n' || b == '\r'
}

// hexArray renders `%08x` values joined by `_`, as production's position
// columns do (internal/utility/convert.go:138-150). They are varchar despite the
// `_int` suffix.
func hexArray(vals ...uint32) string {
	parts := make([]string, len(vals))
	for i, v := range vals {
		parts[i] = fmt.Sprintf("%08x", v)
	}
	return strings.Join(parts, "_")
}

// chunkID reproduces production's ChunkID: %016x of xxhash64(text+docID)
// (internal/common/format.go:191). FNV-1a is used instead of xxhash64 to avoid a
// dependency; the load test needs stable, well-distributed ids, not RAGFlow's
// exact values, and nothing in the query mix recomputes an id from content.
func chunkID(docID, text string) string {
	h := fnv.New64a()
	_, _ = h.Write([]byte(text))
	_, _ = h.Write([]byte(docID))
	return fmt.Sprintf("%016x", h.Sum64())
}

// tsToFields converts "30-APR-2012 03:25:17.000" into the two time columns
// production writes: a formatted varchar and float seconds with a fractional
// part (float64(UnixNano)/1e9). The fractional part is deliberate — it is what
// distinguishes a generated row from a hand-made one.
func tsToFields(ts string) (string, float64, error) {
	t, err := time.Parse("02-Jan-2006 15:04:05.000", ts)
	if err != nil {
		return "", 0, err
	}
	t = t.UTC()
	return t.Format("2006-01-02 15:04:05"), float64(t.UnixNano()) / 1e9, nil
}

// sha256Hex is shared by the corpus and mapping loaders.
func sha256Hex(b []byte) string {
	h := sha256.Sum256(b)
	return hex.EncodeToString(h[:])
}

