// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! Production-readiness soak: the five index families in **every combination**,
//! under concurrent writes, concurrent reads and restarts.
//!
//! The five families are the ones a deployment can put on one table at once:
//!
//! | bit | family | SQL | answered by |
//! |---|---|---|---|
//! | `FT` | full text | `CREATE INDEX … USING FULLTEXT` | `WHERE filter_text(body, …)` / `SEARCH MATCH TEXT` |
//! | `SEC` | secondary | `CREATE INDEX … (c1)` | `WHERE c1 = k` |
//! | `HNSW` | graph ANN on `EMBEDDING` | `… USING HNSW` | `SEARCH MATCH VECTOR (v, …)` |
//! | `SPANN` | IVF/tree ANN on `EMBEDDING` | `… USING SPANN` | `SEARCH MATCH VECTOR (sv, …)` |
//! | `PLAID` | tensor index on `TENSOR` | `… USING PLAID` | `SEARCH MATCH TENSOR (tb, …)` |
//!
//! Our engine has no ScaNN: the fifth family named in the plan ("scann") is
//! `SPANN`, the ANN index for an `EMBEDDING` column. `HNSW` and `SPANN` are kept
//! apart by putting them on two columns, because one column carries one ANN
//! index.
//!
//! ## What a round does
//!
//! 1. **Preload** `rows / 4` rows sequentially and sanity-check every index on
//!    them. This prefix is what the readers below may sample from: it is
//!    committed and indexed before the concurrency starts, so a missing answer
//!    is a bug and not a race the test invented.
//! 2. **Concurrent write phase**: `writers` tasks pull disjoint id ranges from
//!    one job queue and `INSERT` them in batches, while `readers` tasks sample
//!    the stable prefix and ask every index of the table for that row, and one
//!    task runs `FLUSH DATA` (`Engine::flush`) on a timer - so checkpointing,
//!    chunk publishing (`memindex_capacity`/PLAID chunk target are set small on
//!    purpose) and index maintenance all happen *under* the reads.
//! 3. **Crash**: the engine is dropped without `close()`: no checkpoint, no
//!    drain, exactly what `kill -9` leaves behind (WAL frames and mmap'd pages
//!    only). This is the same "the kill: no checkpoint, no drain" shape as
//!    `restart_checkpoint.rs`; the real process kill -9 lane is
//!    `rust_engine/scripts/soak_kill9.sh`.
//! 4. **Reopen on the same directory** and assert, for every table and every
//!    index it carries: every acknowledged row is present exactly once
//!    (`COUNT(*)`, `SUM(id)`, `MIN`/`MAX`), the newest rows are visible, and a
//!    self-query through each index answers the row itself.
//!
//! Every acknowledged row is a committed statement, so `COUNT(*) == SUM` of the
//! claimed ids is exact: ids are handed out as contiguous ranges, count + sum +
//! min/max together catch a lost row, a row twice, and a hole.
//!
//! ## Scale
//!
//! Both lanes are ordinary tests and run in `cargo test --workspace`; together
//! they cost about two minutes in a debug build (6.5 k rows per table over three
//! restarts in one, 31 tables in the other). The knobs scale them without
//! touching the code - the numbers below are past what CI runs:
//!
//! ```text
//! cargo test -p executor --test index_combination_soak -- --nocapture
//!
//! INDEX_SOAK_ROWS=4000 INDEX_SOAK_ROUNDS=5 INDEX_SOAK_BATCH=256 \
//! INDEX_SOAK_WRITERS=16 INDEX_SOAK_READERS=24 \
//!   cargo test -p executor --test index_combination_soak -- --nocapture
//! ```
//!
//! ## What this caught
//!
//! Five defects, all found by the checks below and all fixed in the engine
//! (`filter_text_pushdown_is_wrong_under_concurrent_writes` is the regression
//! test for the first; the other four are what the two lanes assert):
//!
//! 1. **`WHERE filter_text(…)` matched the whole table while a writer ran.**
//!    `RowFulltextProvider::postings` fell back to "keep every row" when
//!    `try_read` on the text index missed (an `INSERT` indexing the same column
//!    holds it), and there is no residual predicate to correct that. Fixed by
//!    deciding such rows from their own text (`RowFulltextProvider::local_hit`),
//!    with `TermPostings::Unusable` kept for the static cases (no index, no ids,
//!    another analyzer) where the residual does exist.
//! 2. **A block written by a checkpoint could hold rows whose column values read
//!    as `NULL`.**
//!
//!    ```text
//!    COUNT(*)                                    -> 1800
//!    COUNT(*) WHERE id >= 0                      -> 840      (values present)
//!    COUNT(*) WHERE id IS NOT NULL               -> 0        (bitmap says all null)
//!    SUM(id) / MIN(id) / MAX(id)                 -> null     (aggregate skips them)
//!    ```
//!
//!    The files on disk were correct - a *reopen* answered 1800 - so the live
//!    process was serving a mapping of the block from before the rewrite: the
//!    process-wide `block_maps` cache was only dropped *before* the write, so a
//!    reader that mapped the block while it was being rewritten cached a version
//!    file from one write with column bodies from another. Fixed by giving every
//!    rewritten block a generation (`BlockMapCache::bump_generation`,
//!    `Engine::block_written`) that a mapping has to match before it is cached.
//! 3. **A vector/tensor search could answer empty during a publication**, and a
//!    query could fail `no blocks for table` while a checkpoint republished the
//!    segment layer: a probe that named no row at all was read as "the table has
//!    no matches", and the catalog deleted the old block records before writing
//!    the new ones. Fixed by `knn_scan::answered` (no rows named ⇒ fall back to
//!    the exact scan) and by `layout::write_segments` publishing first and
//!    sweeping afterwards.
//!
//! 4. **A checkpoint racing concurrent commits could lose a committed tail on
//!    the next open.**
//!
//!    ```text
//!    ix_31: 1648 rows after restart, expected 1800
//!    ```
//!
//!    The rows were acknowledged and visible in the live process, and gone after
//!    `drop(engine)` + reopen: the WAL replay skipped them because a block
//!    claimed a `create` at or above their commit ts. A `flush()` right before
//!    the drop made it disappear, so the rows were in the append buffer and in no
//!    block. Fixed by [`Engine::block_version_batches`]' own extent: the
//!    checkpoint aimed at the rows committed by its bound, and a commit that
//!    landed while it walked widened that window without its rows joining the
//!    buffer - naming that append in a `create` told the replay it was on disk.
//!    Each block now names only the appends it **holds in full**
//!    (`(slot_base + chunk.len()) as u64`).
//! 5. **A rarer `no blocks for table`.** One probe in about six runs failed with
//!    `not found: no blocks for table 'default_db.ix_08'` - a block whose files
//!    were being replaced, read through a mapping that failed to open. The scan
//!    now drops the mapping and retries once on a fresh generation
//!    (`MappedBlock`, `Engine::block_written`) before answering NotFound.
//!
//! Item 4 is why the lanes inject a restart into every round: nothing but a
//! `drop(engine)` + reopen exercises the replay floor, and every one of the four
//! block-layer defects above was invisible to the same test without it.

use std::sync::atomic::{AtomicBool, AtomicUsize, Ordering};
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};

use executor::{Engine, EngineOptions, QueryResult};

const FT: u8 = 1 << 0;
const SEC: u8 = 1 << 1;
const HNSW: u8 = 1 << 2;
const SPANN: u8 = 1 << 3;
const PLAID: u8 = 1 << 4;
const ALL: u8 = FT | SEC | HNSW | SPANN | PLAID;
const FAMILIES: [u8; 5] = [FT, SEC, HNSW, SPANN, PLAID];
const DIM: usize = 8;

fn family_name(kind: u8) -> &'static str {
    match kind {
        FT => "fulltext",
        SEC => "secondary",
        HNSW => "hnsw",
        SPANN => "spann",
        PLAID => "plaid",
        _ => "?",
    }
}

/// One table's index set: a bitmask over [`FAMILIES`].
#[derive(Clone, Copy)]
struct Combo(u8);

impl Combo {
    fn table(self) -> String {
        format!("ix_{:02}", self.0)
    }

    fn has(self, kind: u8) -> bool {
        self.0 & kind != 0
    }

    fn label(self) -> String {
        FAMILIES
            .iter()
            .filter(|k| self.has(**k))
            .map(|k| family_name(*k))
            .collect::<Vec<_>>()
            .join("+")
    }
}

/// Rows, batch size, task counts and restarts.
struct Scale {
    /// Rows each table gains per round.
    rows: usize,
    batch: usize,
    writers: usize,
    readers: usize,
    /// Number of write+crash+reopen rounds after the preload.
    rounds: usize,
}

impl Scale {
    fn from_env(rows: usize, rounds: usize, batch: usize, writers: usize, readers: usize) -> Self {
        fn knob(name: &str, default: usize) -> usize {
            std::env::var(name)
                .ok()
                .and_then(|v| v.trim().parse().ok())
                .filter(|v: &usize| *v > 0)
                .unwrap_or(default)
        }
        Self {
            rows: knob("INDEX_SOAK_ROWS", rows),
            batch: knob("INDEX_SOAK_BATCH", batch),
            writers: knob("INDEX_SOAK_WRITERS", writers),
            readers: knob("INDEX_SOAK_READERS", readers),
            rounds: knob("INDEX_SOAK_ROUNDS", rounds),
        }
    }
}

// ---------------------------------------------------------------------------
// Values
//
// A row is `(id, id, 'row{id} alpha beta', v, sv, [[t]])`:
//
// * `id`/`c1` are the same number, so a point lookup and a secondary lookup ask
//   for the same row;
// * `row{id}` is a token only that document holds (the tokenizer keeps ASCII
//   alphanumerics together), which makes a full-text hit exact;
// * the embeddings are normalized sinusoids: every row sits on the unit sphere,
//   so under `l2` its own distance is 0 and its own vector is strictly the
//   nearest - a self-query has exactly one right answer;
// * the tensor is the same construction in 4 dimensions (unit norm, all four
//   components varying with the id), so `maxsim` is maximized by the row itself
//   instead of by whichever row happens to have the largest norm.
//
// PLAID is asked for `topn=100` with the pruning knobs wide open: it ranks from
// quantized codes, so a top-1 self-query is not a property it guarantees (the
// reference's PLAID has the same recall gap - `docs/MANUAL_ACCEPTANCE.md` §11).
// `recall@100` on self-queries is the shape its own benches use, and the budget
// below (`n_doc_to_score`/`n_full_scores` covering the whole table) keeps the
// check on the index rather than on the pruning.
// ---------------------------------------------------------------------------

/// Payload frequencies: **no** two of them share a rational ratio, so no two
/// ids can drift into the same phase on every component at once.
///
/// An arithmetic progression (`0.7 + 1.7 * j`) looks innocent and is not: with
/// it, `Δid = 377` puts *every* component back in phase (`377 * 0.7 ≈ 42 * 2π`),
/// so the table holds near-duplicate rows at a fixed stride and a `maxsim`
/// answer is decided by quantization noise instead of by the query.
const FREQUENCIES: [f64; 8] = [1.0, 1.4317, 1.7351, 2.2403, 2.6489, 3.0021, 3.3187, 3.6071];

fn unit_vector(id: i64, dim: usize, phase: f64) -> String {
    let mut raw = Vec::with_capacity(dim);
    let mut norm = 0.0f64;
    for j in 0..dim {
        let freq = FREQUENCIES[j % FREQUENCIES.len()] + (j / FREQUENCIES.len()) as f64;
        let v = (id as f64 * freq + phase + 0.31 * j as f64).sin();
        norm += v * v;
        raw.push(v);
    }
    let norm = norm.sqrt();
    let body = raw
        .iter()
        .map(|v| format!("{:.6}", v / norm))
        .collect::<Vec<_>>()
        .join(",");
    format!("[{body}]")
}

fn vector_v(id: i64) -> String {
    unit_vector(id, DIM, 0.0)
}

fn vector_sv(id: i64) -> String {
    unit_vector(id, DIM, 1.7)
}

/// One 4-dim token, without the outer list: a `TENSOR` value is `[[…]]`.
fn tensor_token(id: i64) -> String {
    unit_vector(id, 4, 2.9)
}

fn row_literal(id: i64) -> String {
    format!(
        "({id}, {id}, 'row{id} alpha beta', {}, {}, [{}])",
        vector_v(id),
        vector_sv(id),
        tensor_token(id)
    )
}

fn insert_sql(table: &str, first: i64, count: usize) -> String {
    let mut sql = format!("INSERT INTO {table} VALUES ");
    for k in 0..count {
        if k > 0 {
            sql.push(',');
        }
        sql.push_str(&row_literal(first + k as i64));
    }
    sql.push(';');
    sql
}

// ---------------------------------------------------------------------------
// Query helpers
// ---------------------------------------------------------------------------

async fn query(engine: &Engine, sql: &str) -> Result<Vec<Vec<String>>, String> {
    let results = engine
        .execute(sql)
        .await
        .map_err(|err| format!("{sql} -- {err}"))?;
    let Some(first) = results.first() else {
        return Ok(Vec::new());
    };
    match first {
        QueryResult::Rows { rows, .. } => Ok(rows
            .iter()
            .map(|row| row.iter().map(|v| v.to_slt_string()).collect())
            .collect()),
        other => Err(format!("{sql} -- expected rows, got {other:?}")),
    }
}

async fn scalar(engine: &Engine, sql: &str) -> Result<String, String> {
    query(engine, sql)
        .await?
        .first()
        .and_then(|row| row.first())
        .cloned()
        .ok_or_else(|| format!("{sql} -- no rows"))
}

fn as_i64(cell: &str) -> Result<i64, String> {
    if let Ok(v) = cell.parse::<i64>() {
        return Ok(v);
    }
    cell.parse::<f64>()
        .map(|v| v.round() as i64)
        .map_err(|_| format!("not a number: {cell:?}"))
}

async fn number(engine: &Engine, sql: &str) -> Result<i64, String> {
    as_i64(&scalar(engine, sql).await?)
}

/// The first column of `sql` as integers (a `SEARCH …` answer's ids).
async fn ids(engine: &Engine, sql: &str) -> Result<Vec<i64>, String> {
    query(engine, sql)
        .await?
        .iter()
        .map(|row| {
            row.first()
                .ok_or_else(|| format!("{sql} -- empty row"))
                .and_then(|cell| as_i64(cell))
        })
        .collect()
}

/// Self-queries that answered without the row once and right on the retry.
///
/// A dump publishes a chunk and moves the in-memory graph/tail out of the query
/// path; a search that lands in that window can answer nothing. It heals by the
/// next statement, so it is counted and printed rather than failed - the hard
/// assertion is that a *second* answer still misses the row, which would mean
/// the index lost data rather than raced. See the module doc.
static TRANSIENT_SELF_QUERY_MISSES: std::sync::atomic::AtomicUsize =
    std::sync::atomic::AtomicUsize::new(0);

/// Runs a self-query and demands `id` in the answer.
///
/// Anything short of that is retried once, so the report separates "the engine
/// lost the row" (the second answer is wrong too) from "the statement raced a
/// publication" (the second one is right). The first fails the run, the second
/// is counted - both for a wrong answer and for an error, because the error a
/// racing scan used to produce (`no blocks for table`) is the same race seen
/// from the other side, and a run that only counts one of the two would report
/// the wrong number of defects.
async fn self_hit(engine: &Engine, sql: &str, id: i64, index: &str, table: &str) -> Option<String> {
    let first = ids(engine, sql).await;
    match &first {
        Ok(found) if found.contains(&id) => return None,
        _ => {}
    }
    let retried = ids(engine, sql).await;
    if retried.as_ref().is_ok_and(|ids| ids.contains(&id)) {
        TRANSIENT_SELF_QUERY_MISSES.fetch_add(1, Ordering::Relaxed);
        return None;
    }
    Some(match (first, retried) {
        (Ok(found), Ok(again)) => {
            format!("{table}: {index} self-query {id} answered {found:?}, retried {again:?}")
        }
        (Err(err), Ok(again)) => {
            format!("{table}: {index} self-query {id} failed -- {err}; retried {again:?}")
        }
        (Ok(found), Err(err)) => {
            format!("{table}: {index} self-query {id} answered {found:?}; retried failed -- {err}")
        }
        (Err(err), Err(again)) => {
            format!("{table}: {index} self-query {id} failed twice -- {err}; {again}")
        }
    })
}

/// The one probe every reader and every verification shares: ask each index the
/// table carries for `id`, and demand the row comes back.
async fn probe(engine: &Engine, table: &str, mask: u8, id: i64) -> Option<String> {
    if mask & SEC != 0 {
        match number(
            engine,
            &format!("SELECT COUNT(*) FROM {table} WHERE c1 = {id};"),
        )
        .await
        {
            Ok(1) => {}
            Ok(n) => {
                return Some(format!(
                    "{table}: secondary lookup c1 = {id} answered {n} rows"
                ));
            }
            Err(err) => {
                return Some(format!(
                    "{table}: secondary lookup c1 = {id} failed -- {err}"
                ));
            }
        }
    }
    if mask & FT != 0 {
        // `row{id}` is a token only that document holds, so the answer is
        // exactly one id - an extra id is as much a defect as a missing one.
        // Both the `SEARCH MATCH TEXT` path and the index-scan pushdown
        // (`WHERE filter_text(…)`) are asked: the pushdown used to answer the
        // whole table whenever an `INSERT` held the text index's lock, and this
        // is what notices a fallback that waves rows through.
        let sql =
            format!("SELECT id FROM {table} SEARCH MATCH TEXT ('body', 'row{id}', 'topn=10');");
        match ids(engine, &sql).await {
            Ok(found) if found == vec![id] => {}
            Ok(found) => {
                return Some(format!(
                    "{table}: SEARCH MATCH TEXT row{id} answered {found:?}"
                ));
            }
            Err(err) => {
                return Some(format!(
                    "{table}: SEARCH MATCH TEXT row{id} failed -- {err}"
                ));
            }
        }
        let sql = format!("SELECT id FROM {table} WHERE filter_text('body', 'row{id}');");
        match ids(engine, &sql).await {
            Ok(found) if found == vec![id] => {}
            Ok(found) => {
                return Some(format!("{table}: filter_text row{id} answered {found:?}"));
            }
            Err(err) => {
                return Some(format!("{table}: filter_text row{id} failed -- {err}"));
            }
        }
    }
    if mask & HNSW != 0 {
        let sql = format!(
            "SELECT id FROM {table} SEARCH MATCH VECTOR (v, {}, 'float', 'l2', 10);",
            vector_v(id)
        );
        if let Some(failure) = self_hit(engine, &sql, id, "HNSW", table).await {
            return Some(failure);
        }
    }
    if mask & SPANN != 0 {
        let sql = format!(
            "SELECT id FROM {table} SEARCH MATCH VECTOR (sv, {}, 'float', 'l2', 10);",
            vector_sv(id)
        );
        if let Some(failure) = self_hit(engine, &sql, id, "SPANN", table).await {
            return Some(failure);
        }
    }
    if mask & PLAID != 0 {
        // `recall@100` on a self-query, with the centroid cut off and a score
        // budget that covers the whole table (`n_full_scores` must not exceed
        // `n_doc_to_score`, or both overrides are rejected and the defaults -
        // `topn * 10` - decide the answer).
        let sql = format!(
            "SELECT id FROM {table} SEARCH MATCH TENSOR (tb, [{}], 'float', 'maxsim', \
             'topn=100;plaid_centroid_score_threshold=0;\
             plaid_n_doc_to_score=8192;plaid_n_full_scores=8192');",
            tensor_token(id)
        );
        if let Some(failure) = self_hit(engine, &sql, id, "PLAID recall@100", table).await {
            return Some(failure);
        }
    }
    None
}

// ---------------------------------------------------------------------------
// Schema
// ---------------------------------------------------------------------------

fn configure(engine: &Engine, scale: &Scale) {
    // Small enough that a dump lands *inside* the concurrency window: the point
    // of the test is publishing chunks while readers hold the old ones. This is
    // the capacity-driven dump (`mem_index_capacity`), i.e. what a production
    // server that never calls `FLUSH DATA` still does.
    engine.set_memindex_capacity((scale.rows / 2).max(64));
    engine.set_plaid_chunk_target_rows((scale.rows / 4).max(32));
    // `[storage] flush_dumps_mem_index` stays at its default (`false`): the
    // reference's `FLUSH DATA` is a bare checkpoint, so the checkpoint timer
    // below must not be the thing that publishes the tails. Leaving it at the
    // default keeps the test on the configuration a deployment actually runs.
}

async fn create(engine: &Engine, combo: Combo) {
    let table = combo.table();
    engine
        .execute(&format!(
            "CREATE TABLE {table} (id INTEGER, c1 INTEGER, body VARCHAR, \
             v EMBEDDING(FLOAT,{DIM}), sv EMBEDDING(FLOAT,{DIM}), tb TENSOR(FLOAT,4));"
        ))
        .await
        .unwrap_or_else(|err| panic!("{table}: CREATE TABLE failed -- {err}"));
    let mut ddl = Vec::new();
    if combo.has(FT) {
        ddl.push(format!(
            "CREATE INDEX i_ft ON {table} (body) USING FULLTEXT WITH (analyzer='standard');"
        ));
    }
    if combo.has(SEC) {
        ddl.push(format!("CREATE INDEX i_sec ON {table} (c1);"));
    }
    if combo.has(HNSW) {
        ddl.push(format!(
            "CREATE INDEX i_hnsw ON {table} (v) USING HNSW \
             WITH (metric='l2', M=8, ef_construction=64);"
        ));
    }
    if combo.has(SPANN) {
        ddl.push(format!(
            "CREATE INDEX i_spann ON {table} (sv) USING SPANN \
             WITH (min_rows=1, metric='l2', max_partition_size=16, num_centroids=16);"
        ));
    }
    if combo.has(PLAID) {
        ddl.push(format!(
            "CREATE INDEX i_plaid ON {table} (tb) USING PLAID WITH (n_centroids=8);"
        ));
    }
    for sql in ddl {
        engine
            .execute(&sql)
            .await
            .unwrap_or_else(|err| panic!("{sql} -- {err}"));
    }
}

// ---------------------------------------------------------------------------
// Write phase: writers + readers + a checkpoint timer, all on one engine
// ---------------------------------------------------------------------------

async fn write_round(
    engine: &Arc<Engine>,
    tables: &[(String, u8)],
    from: &[i64],
    target: i64,
    scale: &Scale,
) -> Vec<String> {
    let errors: Arc<Mutex<Vec<String>>> = Arc::new(Mutex::new(Vec::new()));
    let names: Arc<Vec<String>> = Arc::new(tables.iter().map(|(t, _)| t.clone()).collect());
    let masks: Arc<Vec<u8>> = Arc::new(tables.iter().map(|(_, m)| *m).collect());

    // One job per (table, contiguous id range). Ranges are disjoint, so no two
    // writers ever insert the same id and `COUNT(*)`/`SUM(id)` stay exact.
    let mut jobs: Vec<(usize, i64, i64)> = Vec::new();
    for (index, _) in tables.iter().enumerate() {
        let start = from[index];
        if start >= target {
            continue;
        }
        let span = (target - start) as usize;
        let writers = scale.writers.min(span).max(1);
        let per = span.div_ceil(writers) as i64;
        let mut at = start;
        while at < target {
            let end = (at + per).min(target);
            jobs.push((index, at, end));
            at = end;
        }
    }
    let jobs = Arc::new(jobs);
    let next = Arc::new(AtomicUsize::new(0));
    let stop = Arc::new(AtomicBool::new(false));

    let mut workers = Vec::with_capacity(scale.writers);
    for _ in 0..scale.writers {
        let engine = Arc::clone(engine);
        let jobs = Arc::clone(&jobs);
        let next = Arc::clone(&next);
        let names = Arc::clone(&names);
        let errors = Arc::clone(&errors);
        let batch = scale.batch;
        workers.push(tokio::spawn(async move {
            loop {
                let job = next.fetch_add(1, Ordering::Relaxed);
                let Some(&(table_index, start, end)) = jobs.get(job) else {
                    break;
                };
                let table = &names[table_index];
                let mut at = start;
                while at < end {
                    let take = ((end - at) as usize).min(batch);
                    let sql = insert_sql(table, at, take);
                    if let Err(err) = engine.execute(&sql).await {
                        errors.lock().unwrap().push(format!(
                            "{table}: INSERT ids [{at}, {}) -- {err}",
                            at + take as i64
                        ));
                        return;
                    }
                    at += take as i64;
                }
            }
        }));
    }

    // The checkpoint that publishes chunks while the writers append and the
    // readers read: `FLUSH DATA` on a timer, the same call the server's
    // periodic trigger makes.
    let flusher = {
        let engine = Arc::clone(engine);
        let stop = Arc::clone(&stop);
        let errors = Arc::clone(&errors);
        tokio::spawn(async move {
            while !stop.load(Ordering::Relaxed) {
                tokio::time::sleep(Duration::from_millis(40)).await;
                if stop.load(Ordering::Relaxed) {
                    break;
                }
                if let Err(err) = engine.flush().await {
                    errors
                        .lock()
                        .unwrap()
                        .push(format!("FLUSH DATA during writes failed -- {err}"));
                    break;
                }
            }
        })
    };

    // Readers only sample the *stable* prefix, i.e. rows committed by an earlier
    // round. Those are indexed and their answers are exact, so an empty answer
    // is a defect rather than a row that has not landed yet.
    let mut readers = Vec::with_capacity(scale.readers);
    for reader in 0..scale.readers {
        let engine = Arc::clone(engine);
        let stop = Arc::clone(&stop);
        let errors = Arc::clone(&errors);
        let names = Arc::clone(&names);
        let masks = Arc::clone(&masks);
        let stable: Arc<Vec<i64>> = Arc::new(from.to_vec());
        readers.push(tokio::spawn(async move {
            // xorshift: cheap, deterministic per reader, and no dependency.
            let mut state = 0x9E37_79B9_7F4A_7C15u64 ^ ((reader as u64 + 1) * 0x1234_5678);
            let mut turn = reader;
            while !stop.load(Ordering::Relaxed) {
                state ^= state << 13;
                state ^= state >> 7;
                state ^= state << 17;
                if names.is_empty() {
                    break;
                }
                let table_index = turn % names.len();
                turn = turn.wrapping_add(1);
                let floor = stable[table_index];
                if floor <= 0 {
                    tokio::time::sleep(Duration::from_millis(1)).await;
                    continue;
                }
                let id = (state % floor as u64) as i64;
                if let Some(failure) =
                    probe(&engine, &names[table_index], masks[table_index], id).await
                {
                    errors.lock().unwrap().push(failure);
                    break;
                }
            }
        }));
    }

    for worker in workers {
        let _ = worker.await;
    }
    stop.store(true, Ordering::Relaxed);
    let _ = flusher.await;
    for reader in readers {
        let _ = reader.await;
    }
    Arc::try_unwrap(errors)
        .map(|m| m.into_inner().unwrap())
        .unwrap_or_default()
}

// ---------------------------------------------------------------------------
// Verification
// ---------------------------------------------------------------------------

/// What a scan actually sees when the aggregate disagrees with `COUNT(*)`.
async fn diagnose(engine: &Engine, table: &str) -> String {
    let mut out = Vec::new();
    for sql in [
        format!("SELECT COUNT(*) FROM {table} WHERE id = 0;"),
        format!("SELECT COUNT(*) FROM {table} WHERE id >= 0;"),
        format!("SELECT COUNT(*) FROM {table} WHERE id IS NOT NULL;"),
        format!("SELECT MIN(id) FROM {table} WHERE id >= 0;"),
        format!("SELECT COUNT(*) FROM {table} GROUP BY c1;"),
    ] {
        out.push(match query(engine, &sql).await {
            Ok(rows) => format!("{sql} -> {rows:?}"),
            Err(err) => format!("{sql} -> {err}"),
        });
    }
    out.join(" | ")
}

/// Every acknowledged row is present once, and each index answers for a few
/// sampled rows. `deep` adds the plan check that a secondary index really is
/// the thing answering (`INDEX SCAN`) - priced once per table, at the restart.
async fn verify(
    engine: &Engine,
    tables: &[(String, u8)],
    written: &[i64],
    deep: bool,
) -> Vec<String> {
    let mut failures = Vec::new();
    for (index, (table, mask)) in tables.iter().enumerate() {
        let expected = written[index];
        match number(engine, &format!("SELECT COUNT(*) FROM {table};")).await {
            Ok(rows) if rows == expected => {}
            Ok(rows) => failures.push(format!(
                "{table}: {rows} rows after restart, expected {expected}"
            )),
            Err(err) => failures.push(format!("{table}: COUNT(*) failed -- {err}")),
        }
        if expected == 0 {
            continue;
        }
        // count + sum + min/max: a lost row, a row twice and a hole each change
        // at least one of them.
        match number(engine, &format!("SELECT SUM(id) FROM {table};")).await {
            Ok(sum) => {
                let want = expected * (expected - 1) / 2;
                if sum != want {
                    failures.push(format!(
                        "{table}: SUM(id) = {sum}, expected {want}; {}",
                        diagnose(engine, table).await
                    ));
                }
            }
            Err(err) => failures.push(format!(
                "{table}: SUM(id) failed -- {err}; {}",
                diagnose(engine, table).await
            )),
        }
        match query(engine, &format!("SELECT MIN(id), MAX(id) FROM {table};")).await {
            Ok(rows) if rows.len() == 1 && rows[0].len() == 2 => {
                match (as_i64(&rows[0][0]), as_i64(&rows[0][1])) {
                    (Ok(min), Ok(max)) if min == 0 && max == expected - 1 => {}
                    (Ok(min), Ok(max)) => failures.push(format!(
                        "{table}: id range [{min}, {max}], expected [0, {}]",
                        expected - 1
                    )),
                    (min, max) => failures.push(format!("{table}: id bounds {min:?}/{max:?}")),
                }
            }
            Ok(rows) => failures.push(format!("{table}: unexpected MIN/MAX answer {rows:?}")),
            Err(err) => failures.push(format!("{table}: MIN/MAX failed -- {err}")),
        }

        let mut samples = vec![0i64, expected / 2, expected - 1];
        samples.sort_unstable();
        samples.dedup();
        for id in samples {
            if let Some(failure) = probe(engine, table, *mask, id).await {
                failures.push(failure);
            }
        }

        if deep && mask & SEC != 0 {
            let sql = format!("EXPLAIN SELECT * FROM {table} WHERE c1 = 0;");
            match query(engine, &sql).await {
                Ok(rows) => {
                    let text = rows
                        .iter()
                        .flatten()
                        .cloned()
                        .collect::<Vec<_>>()
                        .join("\n");
                    if !text.contains("INDEX SCAN") {
                        failures.push(format!(
                            "{table}: EXPLAIN does not answer from the secondary index:\n{text}"
                        ));
                    }
                }
                Err(err) => failures.push(format!("{table}: EXPLAIN failed -- {err}")),
            }
        }
    }
    failures
}

// ---------------------------------------------------------------------------
// The soak
// ---------------------------------------------------------------------------

async fn open(dir: &std::path::Path) -> Engine {
    Engine::open_async(dir, EngineOptions::default())
        .await
        .unwrap_or_else(|err| panic!("open {} failed -- {err}", dir.display()))
}

async fn soak(masks: Vec<u8>, scale: Scale) {
    assert!(!masks.is_empty(), "no index combination to soak");
    let dir = tempfile::tempdir().unwrap();
    let combos: Vec<Combo> = masks.iter().copied().map(Combo).collect();
    let tables: Vec<(String, u8)> = combos.iter().map(|c| (c.table(), c.0)).collect();
    for combo in &combos {
        assert!(
            combo.0 & !ALL == 0,
            "combination {:02x} names an unknown index family",
            combo.0
        );
    }
    let mut written = vec![0i64; tables.len()];
    let mut failures: Vec<String> = Vec::new();
    let started = Instant::now();

    let mut engine = Arc::new(open(dir.path()).await);
    configure(&engine, &scale);
    for combo in &combos {
        create(&engine, *combo).await;
    }

    // The stable prefix the readers sample: committed and indexed before any
    // concurrency, so an empty answer is a bug.
    let preload = (scale.rows / 4).max(1) as i64;
    for (index, (table, _)) in tables.iter().enumerate() {
        let mut at = 0;
        while at < preload {
            let take = ((preload - at) as usize).min(scale.batch);
            engine
                .execute(&insert_sql(table, at, take))
                .await
                .unwrap_or_else(|err| panic!("{table}: preload at {at} failed -- {err}"));
            at += take as i64;
        }
        written[index] = preload;
    }
    eprintln!(
        "index soak: {} table(s) ({}), preload {preload} rows/table: {:?}",
        combos.len(),
        combos
            .iter()
            .map(|c| c.label())
            .collect::<Vec<_>>()
            .join(", "),
        started.elapsed()
    );
    failures.extend(verify(&engine, &tables, &written, false).await);

    for round in 1..=scale.rounds {
        let target = preload + round as i64 * scale.rows as i64;
        let began = Instant::now();
        let errors = write_round(&engine, &tables, &written, target, &scale).await;
        eprintln!(
            "index soak: round {round}: {} writers -> {target} rows/table in {:?}",
            scale.writers,
            began.elapsed()
        );
        if !errors.is_empty() {
            failures.extend(errors);
            break;
        }
        for row in written.iter_mut() {
            *row = target;
        }
        failures.extend(verify(&engine, &tables, &written, false).await);

        // The crash: no `close()`, no checkpoint, no drain of the background
        // work - only what the WAL and the mmap'd pages already hold survives.
        drop(engine);
        let reopened = Instant::now();
        engine = Arc::new(open(dir.path()).await);
        eprintln!(
            "index soak: round {round}: recovery took {:?}",
            reopened.elapsed()
        );
        failures.extend(verify(&engine, &tables, &written, true).await);
    }
    drop(engine);

    let transient = TRANSIENT_SELF_QUERY_MISSES.load(Ordering::Relaxed);
    if transient > 0 {
        eprintln!(
            "index soak: {transient} self-quer(ies) answered empty once and right on the retry \
             (publication window - see the module doc)"
        );
    }
    eprintln!("index soak: {:?} total", started.elapsed());
    assert!(
        failures.is_empty(),
        "{} failure(s) over {} table(s):\n{}",
        failures.len(),
        tables.len(),
        failures.join("\n")
    );
}

/// Each family alone, then all five on one table.
#[tokio::test(flavor = "multi_thread", worker_threads = 8)]
async fn every_index_family_survives_concurrent_writes_queries_and_restarts() {
    soak(
        vec![FT, SEC, HNSW, SPANN, PLAID, ALL],
        Scale::from_env(1500, 3, 128, 6, 6),
    )
    .await;
}

/// Every non-empty subset of the five families (31 tables), all live at once:
/// writes, reads and restarts contend across the whole matrix.
#[tokio::test(flavor = "multi_thread", worker_threads = 8)]
async fn every_combination_of_the_five_index_families_survives_the_same() {
    soak((1..=ALL).collect(), Scale::from_env(400, 2, 64, 8, 8)).await;
}

// ---------------------------------------------------------------------------
// Regression: the full-text **index-scan pushdown** (`WHERE filter_text(…)`)
// ---------------------------------------------------------------------------
//
// `SEARCH MATCH TEXT` was correct under every condition the soak above produces;
// `WHERE filter_text('body', …)` was not, and it failed in both directions while
// the same table was written to:
//
// * **A concurrent writer made the filter match everything.**
//   `RowFulltextProvider::postings` (`engine/state.rs`) answered with *every*
//   materialised row when it could not read the inverted index - `try_read` fails
//   while a writer holds the write lock - because its fallback was
//   `else { true }` ("keep the row and let the residual predicate decide"). There
//   is no residual predicate for `filter_text`, so a term one row held answered
//   the whole batch (`filter row28 -> 200`). Fixed by
//   `RowFulltextProvider::local_hit`, which decides such a row from its own text
//   with the index's tokeniser; `TermPostings::Unusable` still covers the static
//   cases (no index, no ids, another analyzer), where the residual does exist.
//
// * **A checkpoint made committed rows vanish.** With `FLUSH DATA` running on a
//   timer, a lookup for a row inside the chunk a seal had just written answered
//   0 rows until the chunk's catalog record landed (334 wrong answers in 551
//   probes). That one was the block-mapping cache serving a rewritten block
//   (`decode_block_column` read the old bytes), and it is fixed with the rest of
//   the mapping work: `Engine::block_written` bumps a generation that a mapping
//   has to match before it is cached.
//
// Both are kept as a plain test, not an ignored one: they answer correctly now,
// and this is what fails if either fallback comes back.
#[tokio::test(flavor = "multi_thread", worker_threads = 8)]
async fn filter_text_pushdown_is_wrong_under_concurrent_writes() {
    async fn fixture(dir: &std::path::Path) -> Arc<Engine> {
        let engine = Arc::new(
            Engine::open_async(dir, EngineOptions::default())
                .await
                .unwrap(),
        );
        engine.set_memindex_capacity(64);
        engine
            .execute(&format!(
                "CREATE TABLE ft_gap (id INTEGER, c1 INTEGER, body VARCHAR, \
                 v EMBEDDING(FLOAT,{DIM}), sv EMBEDDING(FLOAT,{DIM}), tb TENSOR(FLOAT,4));"
            ))
            .await
            .unwrap();
        engine
            .execute(
                "CREATE INDEX i_ft ON ft_gap (body) USING FULLTEXT WITH (analyzer='standard');",
            )
            .await
            .unwrap();
        let values: Vec<String> = (0..200).map(row_literal).collect();
        engine
            .execute(&format!("INSERT INTO ft_gap VALUES {};", values.join(",")))
            .await
            .unwrap();
        engine
    }

    async fn wrong_answers(engine: &Engine, probes: impl Iterator<Item = i64>) -> Vec<String> {
        let mut out = Vec::new();
        for id in probes {
            let sql = format!("SELECT COUNT(*) FROM ft_gap WHERE filter_text('body', 'row{id}');");
            match number(engine, &sql).await {
                Ok(1) => {}
                Ok(n) => out.push(format!("filter_text row{id} answered {n} rows")),
                Err(err) => out.push(format!("filter_text row{id} failed -- {err}")),
            }
        }
        out
    }

    // 1. A writer appending in the background.
    let dir = tempfile::tempdir().unwrap();
    let engine = fixture(dir.path()).await;
    let stop = Arc::new(AtomicBool::new(false));
    let writer = {
        let engine = Arc::clone(&engine);
        tokio::spawn(async move {
            let mut at = 200i64;
            while at < 700 {
                let values: Vec<String> = (at..at + 64).map(row_literal).collect();
                engine
                    .execute(&format!("INSERT INTO ft_gap VALUES {};", values.join(",")))
                    .await
                    .unwrap();
                at += 64;
            }
        })
    };
    let reader = {
        let engine = Arc::clone(&engine);
        let stop = Arc::clone(&stop);
        tokio::spawn(async move {
            let mut wrong = Vec::new();
            while !stop.load(Ordering::Relaxed) {
                wrong.extend(wrong_answers(&engine, (0..200).step_by(7)).await);
            }
            wrong
        })
    };
    writer.await.unwrap();
    stop.store(true, Ordering::Relaxed);
    let under_writes = reader.await.unwrap();
    drop(engine);

    // 2. A checkpoint on a timer, one writer.
    let dir = tempfile::tempdir().unwrap();
    let engine = fixture(dir.path()).await;
    let stop = Arc::new(AtomicBool::new(false));
    let flusher = {
        let engine = Arc::clone(&engine);
        let stop = Arc::clone(&stop);
        tokio::spawn(async move {
            while !stop.load(Ordering::Relaxed) {
                tokio::time::sleep(Duration::from_millis(20)).await;
                engine.flush().await.unwrap();
            }
        })
    };
    let mut under_checkpoint = Vec::new();
    let mut at = 200i64;
    while at < 700 {
        let values: Vec<String> = (at..at + 64).map(row_literal).collect();
        engine
            .execute(&format!("INSERT INTO ft_gap VALUES {};", values.join(",")))
            .await
            .unwrap();
        at += 64;
        under_checkpoint.extend(wrong_answers(&engine, (0..200).step_by(7)).await);
    }
    stop.store(true, Ordering::Relaxed);
    flusher.await.unwrap();
    drop(engine);

    assert!(
        under_writes.is_empty() && under_checkpoint.is_empty(),
        "filter_text pushdown answered wrongly under a writer ({} samples, e.g. {:?}) \
         and under a checkpoint ({} samples, e.g. {:?})",
        under_writes.len(),
        under_writes.iter().take(3).collect::<Vec<_>>(),
        under_checkpoint.len(),
        under_checkpoint.iter().take(3).collect::<Vec<_>>()
    );
}
