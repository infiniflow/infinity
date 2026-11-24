# Infinity / RAGFlow production load test

The executable form of [`docs/load_test_plan_ragflow_prod.md`](../../docs/load_test_plan_ragflow_prod.md).
One Go program with the embedder compiled in (cgo):

- **corpus** — reads `test/data/benchmark/enwiki/enwiki-10w.csv`, strips wikitext,
  cuts 200–1500-char prose windows (plan §4.2);
- **rows** — fills only the ~10 columns production fills and leaves the other 82
  at the mapping default, reading the column list from RAGFlow's own
  `conf/infinity_mapping.json` (§4.2.3);
- **vectors** — 1024-d static embeddings from `model2vec-rs` via a C ABI
  (`tools/loadtest/m2v`), so there is no Python generator and no intermediate
  vector file (§4.2.4);
- **load** — one goroutine per client, one SDK connection per goroutine, running
  the Q1–Q7 mix plus a readers-and-writers phase (§3, §5).

## Build

```bash
tools/loadtest/build_m2v.sh          # cargo build the C ABI -> libm2v.so
cd tools/loadtest && go build ./...  # cgo links against it
```

The Rust side is a `cdylib`, not a `staticlib`: Rust's std ships LTO bitcode in
its archive members and Go's cgo refuses to link an archive containing a
`.llvmbc` section. A shared object avoids that, and the loader cost is
irrelevant for a test harness.

## Run

```bash
# unit tests only (no server needed)
MAPPING=/path/to/ragflow/conf/infinity_mapping.json \
M2V_MODEL=/path/to/model2vec-1024 \
  go test ./...

# end-to-end, cheapest scenario
LT_SERVER=127.0.0.1:28317 \
LT_MAPPING=/path/to/ragflow/conf/infinity_mapping.json \
LT_MODEL=/path/to/model2vec-1024 \
LT_SCENARIO=S \
  go test -v -timeout 2h -run TestLoadTest ./...
```

| Variable | Meaning |
|---|---|
| `LT_SERVER` | `host:port` of the client (thrift) port. Unset ⇒ the end-to-end test skips. |
| `LT_MAPPING` | RAGFlow `conf/infinity_mapping.json` — the schema source of truth. |
| `LT_MODEL` | local model2vec directory; must be 1024-d. |
| `LT_SCENARIO` | `S`, `M`, `L`, `XL`, or `S:10x200` (tables × rows/table). A trailing `/N` scales a named scenario, e.g. `L/100`. |
| `LT_TABLES`, `LT_ROWS` | override either dimension of the scenario. |
| `LT_QUERIES` | query count per kind (default 200). |
| `LT_BATCHES` | concurrent ingest workers (default 4). |
| `LT_BATCH_SIZE` | rows per INSERT (default 1024). |
| `LT_SPARSEGRAM` | add `content`'s third analyzer → 60 index objects/table. |
| `LT_SKEW` | production-like Zipf over the top 42 tables (needs 200+ tables). |
| `LT_INDEX_LAST` | load first, then index (P1b) instead of indexes first (P1a). |
| `LT_KEEP_TABLES` | skip P8 teardown, to inspect the data afterwards. |
| `LT_READERS` / `LT_WRITERS` / `LT_MIXED_SECONDS` | P7 shape. |
| `LT_SEED` | drives every sampled choice; recorded in the report. |

## Phases

`P0 create` → `P1 ingest` → (`P2 index`, P1b only) → `Q dense` / `Q text` /
`Q hybrid` / `Q filtered` / `Q like` / `Q point` → `P7 mixed` → `P8 teardown`.

`Q point` is skipped in the default mix because it needs a resolved `doc_id` or
`kb_id` per table; the driver derives `kb_id` from the allocation when it runs.

## Validated results

Every case below is a full P0→P8 run that passed with zero query errors. Runs use
`LT_SCENARIO=<name>/100` (1/100 of the target) as agreed, which keeps the shape —
* rows per table and chunks per document are unchanged, only the totals shrink.

| Case | Tables / rows | P0 (ms/table) | P1 (rows/s) | Notes |
|---|---|---|---|---|
| `S/100` | 1 / 1,000 | 130 | 529 | baseline smoke |
| `M/100` | 6 / 5,838 | 121 | 395 | |
| `L/100` | 58 / 83,408 | 109 | 451 | the production shape at 1/100 |
| `XL:40x120` | 40 / 4,800 | 1,307 | 58 | measured under external CPU load, see below |
| `L/100` + `LT_INDEX_LAST` | 58 / 83,408 | 85 | 5,412 | then P2: 23.2 ms/row |
| `L/100` + `LT_SKEW` | 58 / 83,408 | 130 | 449 | Zipf over 42 hot tables |
| `S:6x250` + `LT_SPARSEGRAM` | 6 / 1,500 | 144 | 215 | 60 index objects/table |

Query latencies at `L/100` (the representative case):

| Query | p50 | p99 |
|---|---|---|
| dense (`ef=200`) | 1.5 ms | 8.6 ms |
| text | 2.2 ms | 6.7 ms |
| hybrid (`weighted_sum 0.7/0.3`) | 2.0 ms | 4.6 ms |
| filtered dense | 1.2 ms | 6.0 ms |
| LIKE | 4.5 ms | 96 ms |

P7 (8 readers + 2 writers, 20 s) on the same data: 49,590 reads, p50 3.1 ms,
p99 6.0 ms, **0 read errors**, 448 rows written.

### Measurement caveats found the hard way

These are environment effects, not engine results, and they will mislead anyone
re-running this on a shared machine:

- **Do not run two instances over one data directory.** The second one loses the
  catalog lock, the first gets an external `SIGTERM`, and the surviving process
  measures 20× slower (2,479 vs 130 ms per `CREATE TABLE`, 108 vs 529 rows/s).
  Each run must start from a clean directory with exactly one server.
- **Check `load average` before trusting an ingest number.** A run that shared
  the host with other compile jobs measured 58 rows/s where the same shape gives
  400+ on an idle host. The harness reports `GOMAXPROCS` but cannot detect an
  external load; note it in the report.
- **A server killed mid-run looks like an ingest bug.** The symptom is
  `InfinityException(6003, Connection is closed)` or `EOF` inside P1; the log
  line to look for is `Shutdown infinity server due to signal Terminated`, which
  means something outside the test killed it.
- **Readers need a backoff on failure.** Before one was added, a dead server
  produced 33,572,219 "read errors" in a 10 s phase, because the loop spun as
  fast as the failed connection could be retried. The phase now backs off 50 ms
  per failure and aborts when no read has succeeded at all.

## Measured findings

Everything here was measured on one host (16 cores, release build, one isolated
single-node Infinity, `buffer_manager_size = 1GB`) against the real RAGFlow
schema. These numbers are the reason several plan assumptions had to change.

### Ingest cost is the RAG analyzer, not the vector index

| Configuration (93 columns) | Ingest rate |
|---|---|
| no indexes | ~3,300 rows/s |
| HNSW (`lvq`) only | ~2,600 rows/s |
| 11 secondary indexes only | ~3,400 rows/s |
| full-text on `whitespace-#` / `rankfeatures` | ~3,300 rows/s |
| **one** `rag-coarse` or `rag-fine` index | **~264 rows/s** |
| all 47 full-text indexes (production) | ~230 rows/s |

The first `rag-*` index costs ~12× and each further `rag-*` index is free — the
cost is per row inside the RAG analyzer, not per index object. This held across
0/1/2/…/10 stacked indexes (264 → 257 rows/s, flat) and across 8 consecutive
rounds on one table (194 → 231 rows/s, no warm-up effect). Scaling ingest workers
1/2/4/8 gave 202/284/231/265 rows/s, so it does not parallelise either.

Consequence for the plan: budgeting ingest from vector-index cost or from index
count is wrong by an order of magnitude. At ~230–486 rows/s the 8.34M-row L
scenario is **5–10 hours** of ingest, not the ~1 hour the plan's earlier
1,936 rows/s estimate implied.

### Indexes-first beats load-then-index, and not by a little

| Order | Ingest | Index build | Total for L |
|---|---|---|---|
| indexes first (P1a) | ~600 rows/s | — | **~4.8 h** |
| load then index (P1b) | ~6,555 rows/s | 23.2 ms/row | **~54 h** |

P1b's index build is what kills it: 23.2 ms per row on a populated table. It is
also fragile — see the crash below.

### Two engine defects the harness had to route around

1. **Parquet cannot import this schema.** `DataType::operator==(const
   arrow::DataType&)` has no `kJson` case, so all 13 json columns are rejected
   before a row is read (`Column 1 mismatch, Json != string`). The harness
   therefore uses the SDK's row `Insert`, never Parquet.

2. **`CREATE INDEX` on a populated table segfaults if a `SELECT` is running.**
   Reproduced in P7: `Can't bind column expression … column_remapper_impl.cpp:148`
   followed by SIGSEGV. Building the same index with no rows present never
   crashed, and P2 (load-then-index, single-threaded, no readers) is stable. P7
   writer tables are therefore always created without indexes.

Two SDK defects are worked around in `escapeSQL` / `sanitizeTerm`:

3. The SDK's expression parser **panics** (slice bounds) on a string literal with
   an odd number of quotes, so a corpus phrase containing an apostrophe took the
   process down. Apostrophes are stripped.

4. `parseBetweenExpression` scans for `" BETWEEN "` / `" AND "` anywhere, so a
   LIKE pattern drawn from prose ("between the ages of 5 and 12") is misparsed as
   a range. LIKE terms are reduced to a single word.

### Query latencies (scenario S, 5,884-table schema, 50k rows)

| Query | p50 | p99 |
|---|---|---|
| dense (`ef=200`) | 2.1 ms | 56 ms |
| text (3 fields, `minimum_should_match=30%`) | 2.1 ms | 7.4 ms |
| hybrid (`weighted_sum 0.7/0.3`) | 1.9 ms | 3.7 ms |
| filtered dense (`available_int=1` + `filter_fulltext`) | 1.0 ms | 2.2 ms |
| LIKE | 3.4 ms | 308 ms |

The LIKE tail is the scan; it is the one shape that must have a `sparsegram`
index to be meaningful (plan §3.1).

### Schema and corpus

- 92 mapping columns + 1 vector = 93; **59 index objects** per table in the
  default variant, 60 with `LT_SPARSEGRAM`.
- 3,634 usable articles after cleaning (down from 3,643 titles: nine collapse
  below the 200-char window minimum), **61.6M** distinct window start positions.
- Cleaning is done per **article**, not per extract row. The extract splits
  articles at arbitrary points, so a `{{citation` opened in one row closes in a
  later one; per-row cleaning left 14% of rows with unbalanced markup, per-article
  cleaning leaves ~0.4%.

## Layout

| Path | What |
|---|---|
| `embedder.go` | cgo binding to the `m2v` C ABI |
| `m2v/` | Rust `cdylib`: `m2v_load` / `m2v_encode` / `m2v_dim` / `m2v_free` / `m2v_error` |
| `corpus.go` | wikitext cleaning, window sampling, id/timestamp helpers |
| `schema.go` | reads the RAGFlow mapping, builds rows in production's shape |
| `plan.go` | scenario → per-table row allocation |
| `engine.go` | table DDL, index DDL, batched insert, the Q1–Q7 query mix |
| `unit_test.go` | everything checkable without a server |
| `load_test.go` | the phases and the report |
| `build_m2v.sh` | builds the C ABI |
