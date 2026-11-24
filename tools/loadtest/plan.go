package loadtest

// Scenario planning: how many tables, how many rows each, and how the row budget
// is skewed. Section 2.1 of the plan.

import (
	"fmt"
	"math"
	"math/rand"
	"sort"
)

// Production totals the L scenario reproduces exactly.
const (
	ProdChunks = 8_340_839
	ProdTables = 5_884
	ProdDocs   = 405_617
	prodEmpty  = 83
	prodHot    = 42 // production has 42 indices above 10k docs
	hotShare   = 0.25
	zipfExp    = 0.9
)

// Scenario is a table-count and row-count target.
type Scenario struct {
	Name   string
	Tables int
	Rows   int
}

// Scenarios are the four table counts the plan measures.
var Scenarios = map[string]Scenario{
	"S": {Name: "S", Tables: 50, Rows: 50 * 1000},
	"M": {Name: "M", Tables: 600, Rows: 600 * 973},
	"L": {Name: "L", Tables: ProdTables, Rows: ProdChunks},
	"XL": {Name: "XL", Tables: 12_000, Rows: 12_000 * 400},
}

// Allocation is the per-table plan.
type Allocation struct {
	Rows  []int // rows per table
	Docs  []int // documents per table, so chunks/doc matches production
	Names []string
}

// Total returns the row sum.
func (a Allocation) Total() int {
	n := 0
	for _, r := range a.Rows {
		n += r
	}
	return n
}

// PlanAllocation distributes rows over tables.
//
// Uniform by default. With skew, it reproduces production's shape — a Zipf over
// the 42 hot tables taking 25% of rows, some empty tables, the rest near the
// mean — which is the plan's "long tail variant". The skew needs enough tables
// for its three groups to be non-degenerate, so it only engages at 200+.
func PlanAllocation(sc Scenario, seed int64, skew bool, namePrefix string) Allocation {
	tables, rows := sc.Tables, sc.Rows
	counts := make([]int, tables)
	for i := range counts {
		counts[i] = rows / tables
	}
	// Compute the remainder once: `rows - sum(counts)` re-evaluated in the loop
	// shrinks as the loop runs (the first version lost 1,605 rows that way).
	remainder := rows - sum(counts)
	for i := 0; i < remainder; i++ {
		counts[i]++
	}

	if skew && tables >= 200 {
		counts = zipfAllocation(tables, rows, seed)
	}

	docs := make([]int, tables)
	names := make([]string, tables)
	for i, r := range counts {
		docs[i] = atLeast1(int(math.Round(float64(r) / chunksPerDoc)))
		names[i] = fmt.Sprintf("%s_%05d", namePrefix, i)
	}
	return Allocation{Rows: counts, Docs: docs, Names: names}
}

func zipfAllocation(tables, rows int, seed int64) []int {
	rng := rand.New(rand.NewSource(seed ^ 0x5EED))
	empty := min(prodEmpty, tables/20)
	hot := min(prodHot, tables/10)
	hotRows := int(float64(rows) * hotShare)
	rest := rows - hotRows

	weights := make([]float64, hot)
	wsum := 0.0
	for i := range weights {
		weights[i] = 1.0 / math.Pow(float64(i+1), zipfExp)
		wsum += weights[i]
	}
	counts := make([]int, tables)
	for i := range weights {
		counts[i] = max(1, int(float64(hotRows)*weights[i]/wsum))
	}
	// Put the rounding drift on the largest table so the total is exact.
	counts[0] += hotRows - sum(counts[:hot])

	mid := tables - hot - empty
	if mid > 0 {
		per := rest / mid
		for i := hot; i < hot+mid; i++ {
			counts[i] = per
		}
		counts[hot] += rest - per*mid
	} else {
		counts[0] += rest
	}

	// Shuffle which table gets which count, so "hot" is not tables 0..41.
	order := rng.Perm(tables)
	shuffled := make([]int, tables)
	for src, dst := range order {
		shuffled[dst] = counts[src]
	}
	return shuffled
}

// NonEmpty returns the indices of tables that can answer a query. Querying an
// empty table would measure the empty path.
func (a Allocation) NonEmpty() []int {
	var out []int
	for i, r := range a.Rows {
		if r > 0 {
			out = append(out, i)
		}
	}
	return out
}

// Summary is a one-line description for the run report.
func (a Allocation) Summary() string {
	rows := append([]int(nil), a.Rows...)
	sort.Ints(rows)
	n := len(rows)
	if n == 0 {
		return "no tables"
	}
	empty := 0
	for _, r := range rows {
		if r == 0 {
			empty++
		}
	}
	return fmt.Sprintf(
		"%d tables, %d rows (min %d / p50 %d / p99 %d / max %d), %d empty",
		n, a.Total(), rows[0], rows[n/2], rows[min(n-1, n*99/100)], rows[n-1], empty)
}

func sum(xs []int) int {
	n := 0
	for _, x := range xs {
		n += x
	}
	return n
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func atLeast1(n int) int {
	if n < 1 {
		return 1
	}
	return n
}
