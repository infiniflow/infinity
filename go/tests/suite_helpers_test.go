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

// Shared plumbing for the Gate D cases. The assertion helpers follow the
// ragflow API tests: a `require…(t, …)` that calls `t.Helper()`, names the
// thing it checked, and fails the case immediately. A concurrency case has to
// stop at the first broken invariant - carrying on writes more rows into a
// table whose count is already wrong, and the report becomes unreadable.

import (
	"errors"
	"fmt"
	"math"
	"os"
	"strings"
	"sync"
	"testing"
	"time"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

// window is the per-case running time. A non-positive window disables the
// cases, which is what a `-short` local run wants.
func window(t *testing.T) time.Duration {
	t.Helper()
	if testing.Short() {
		t.Skip("concurrency case")
	}
	if TestConfig.Window <= 0 {
		t.Skip("INFINITY_PARALLEL_SECONDS<=0 disables the concurrency cases")
	}
	return TestConfig.Window
}

// runWindow starts `workers` goroutines and stops them after `window`. Every
// error is collected: a worker that fails must fail the case, because a
// dropped write is exactly what these cases exist to catch.
func runWindow(t *testing.T, workers int, window time.Duration, body func(worker int, deadline time.Time) error) {
	t.Helper()
	var (
		wg       sync.WaitGroup
		mu       sync.Mutex
		firstErr error
	)
	deadline := time.Now().Add(window)
	for i := 0; i < workers; i++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			if err := body(id, deadline); err != nil {
				mu.Lock()
				if firstErr == nil {
					firstErr = err
				}
				mu.Unlock()
			}
		}(i)
	}
	wg.Wait()
	if firstErr != nil {
		t.Fatalf("concurrent window: %v", firstErr)
	}
}

// requireOK fails the case when a statement returned an error.
func requireOK(t *testing.T, err error, what string) {
	t.Helper()
	if err != nil {
		t.Fatalf("%s: %v", what, err)
	}
}

// requireErrorCode fails unless `err` carries `code`.
func requireErrorCode(t *testing.T, err error, code infinity.ErrorCode, what string) {
	t.Helper()
	if err == nil {
		t.Fatalf("%s: expected error code %d, got success", what, code)
	}
	var e *infinity.InfinityException
	if !errors.As(err, &e) {
		t.Fatalf("%s: expected error code %d, got %T: %v", what, code, err, err)
	}
	if e.ErrorCode != int(code) {
		t.Fatalf("%s: expected error code %d, got %d: %v", what, code, e.ErrorCode, err)
	}
}

// isTransactionConflict reports whether `err` is the engine's conflict code
// (`ErrorCodeTXNConflict`). A statement that keeps conflicting past its retry
// limit reports it, and under concurrent DDL that is a correct outcome.
func isTransactionConflict(err error) bool {
	if err == nil {
		return false
	}
	// `NewInfinityException` returns a pointer, so the assertion has to go
	// through `errors.As` rather than a type switch on the value.
	var e *infinity.InfinityException
	if errors.As(err, &e) {
		return e.ErrorCode == int(infinity.ErrorCodeTXNConflict)
	}
	return false
}

// resultOf runs a built query and returns its columns.
func resultOf(t *testing.T, tb *infinity.Table, what string) *infinity.QueryResult {
	t.Helper()
	res, err := tb.ToResult()
	requireOK(t, err, what)
	qr, ok := res.(*infinity.QueryResult)
	if !ok {
		t.Fatalf("%s: result is %T, want *infinity.QueryResult", what, res)
	}
	return qr
}

// requireRows fails unless `qr` returned at least one row for `column`.
func requireRows(t *testing.T, qr *infinity.QueryResult, column, what string) []interface{} {
	t.Helper()
	rows, ok := qr.Data[column]
	if !ok {
		t.Fatalf("%s: result has no column %q: %v", what, column, keysOf(qr.Data))
	}
	if len(rows) == 0 {
		t.Fatalf("%s: column %q is empty", what, column)
	}
	return rows
}

func keysOf(m map[string][]interface{}) []string {
	out := make([]string, 0, len(m))
	for k := range m {
		out = append(out, k)
	}
	return out
}

// countRows runs `count(*)` and returns the single value.
func countRows(t *testing.T, table *infinity.Table) int64 {
	t.Helper()
	qr := resultOf(t, table.Output([]string{"count(*)"}), "count(*)")
	rows := requireRows(t, qr, "count(star)", "count(*)")
	v, ok := toInt64(rows[0])
	if !ok {
		t.Fatalf("count(*) returned %T", rows[0])
	}
	return v
}

// requireRowCount fails unless the table holds exactly `want` rows.
func requireRowCount(t *testing.T, table *infinity.Table, want int64, what string) {
	t.Helper()
	if got := countRows(t, table); got != want {
		t.Fatalf("%s: table holds %d rows, want %d", what, got, want)
	}
}

// csvRowCount counts the data rows in a headerless fixture.
func csvRowCount(t *testing.T, path string) int64 {
	t.Helper()
	raw, err := os.ReadFile(path)
	if err != nil {
		t.Fatalf("read %s: %v", path, err)
	}
	body := raw
	if len(body) > 0 && body[len(body)-1] == '\n' {
		body = body[:len(body)-1]
	}
	if len(body) == 0 {
		return 0
	}
	n := int64(1)
	for _, b := range body {
		if b == '\n' {
			n++
		}
	}
	return n
}

// csvColumn reads one tab-separated column of a headerless fixture, so a case
// can insert the real enwiki text rather than a placeholder: the full-text
// index has to hold many distinct terms for the search to mean anything.
func csvColumn(t *testing.T, path string, index int) []string {
	t.Helper()
	raw, err := os.ReadFile(path)
	if err != nil {
		t.Fatalf("read %s: %v", path, err)
	}
	var out []string
	for _, line := range strings.Split(strings.TrimRight(string(raw), "\n"), "\n") {
		fields := strings.SplitN(line, "\t", index+2)
		if len(fields) > index {
			out = append(out, fields[index])
		}
	}
	if len(out) == 0 {
		t.Fatalf("%s has no column %d", path, index)
	}
	return out
}

// requireFixture skips the case when a fixture is not installed.
func requireFixture(t *testing.T, name string) string {
	t.Helper()
	path := TestConfig.fixture(name)
	if _, err := os.Stat(path); err != nil {
		t.Skipf("test data %s is not installed: %v", path, err)
	}
	return path
}

// requireTable drops any leftover table, then creates it.
func requireTable(t *testing.T, conn *infinity.InfinityConnection, name string, schema infinity.TableSchema) *infinity.Table {
	t.Helper()
	db, err := conn.GetDatabase("default_db")
	requireOK(t, err, "get database")
	_, err = db.DropTable(name, infinity.ConflictTypeIgnore)
	requireOK(t, err, "drop table "+name)
	table, err := db.CreateTable(name, schema, infinity.ConflictTypeError)
	requireOK(t, err, "create table "+name)
	return table
}

// requireDropTable drops a table and fails if the engine refuses.
func requireDropTable(t *testing.T, conn *infinity.InfinityConnection, name string) {
	t.Helper()
	db, err := conn.GetDatabase("default_db")
	requireOK(t, err, "get database")
	_, err = db.DropTable(name, infinity.ConflictTypeError)
	requireOK(t, err, "drop table "+name)
}

// connection opens one session per worker: the Thrift client is not safe for
// concurrent use, which is also why the Python suite hands out one per thread.
func connections(t *testing.T, n int) []*infinity.InfinityConnection {
	t.Helper()
	out := make([]*infinity.InfinityConnection, n)
	for i := range out {
		conn, err := infinity.Connect(TestConfig.URI)
		requireOK(t, err, fmt.Sprintf("connect worker %d", i))
		out[i] = conn
		t.Cleanup(func() { out[i].Disconnect() })
	}
	return out
}

// toInt64 widens whatever integer a cell came back as.
func toInt64(v interface{}) (int64, bool) {
	switch x := v.(type) {
	case int64:
		return x, true
	case int32:
		return int64(x), true
	case int:
		return int64(x), true
	case float64:
		return int64(x), true
	default:
		return 0, false
	}
}

// toFloat64 widens whatever numeric a vector cell came back as.
func toFloat64(v interface{}) (float64, bool) {
	switch x := v.(type) {
	case float64:
		return x, true
	case float32:
		return float64(x), true
	case int64:
		return float64(x), true
	case int32:
		return float64(x), true
	default:
		return 0, false
	}
}

// validateVector compares a returned vector with the one the writer recorded.
func validateVector(got []interface{}, want []float64) bool {
	if len(got) != len(want) {
		return false
	}
	for i, cell := range got {
		v, ok := toFloat64(cell)
		if !ok || math.Abs(v-want[i]) > 0.001 {
			return false
		}
	}
	return true
}
