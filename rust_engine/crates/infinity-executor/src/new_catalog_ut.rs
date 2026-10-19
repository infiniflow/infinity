//! Shared helpers for the `src/unit_test/storage/new_catalog/` ports.
//!
//! The reference fixtures drive `NewTxn` directly and read back `SegmentMeta` /
//! `BlockMeta` / `TableIndexMeta`. The Rust engine exposes the same state
//! through [`Engine::execute`] plus the metadata helpers below. Every ported
//! test keeps the reference's visible-data assertions; whenever the Rust
//! surface has no exact counterpart (for example the reference's chunk-id
//! bookkeeping for a secondary index) the test documents the mapping instead
//! of dropping the corresponding row/value checks.

#![allow(dead_code)]

use crate::{Engine, QueryResult};
use infinity_parser::Literal;
use std::path::Path;

/// Rows of the first `Rows` result of `sql`.
pub(crate) fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { rows, .. } = result {
            return rows;
        }
    }
    panic!("query returned no rows: {sql}");
}

/// `SELECT count(*)` for a possibly-qualified table.
pub(crate) fn count(engine: &Engine, table: &str) -> usize {
    let table = qualified(table);
    rows(engine, &format!("SELECT count(*) FROM {table};"))[0][0]
        .parse()
        .unwrap()
}

/// Qualifies an unqualified name with `default_db`.
pub(crate) fn qualified(table: &str) -> String {
    if table.contains('.') {
        table.to_string()
    } else {
        format!("default_db.{table}")
    }
}

/// Sum of the rows the engine attributes to the table's segments
/// (`TableMeta::GetSegmentIDs1` + `SegmentMeta::GetRowCnt1`).
pub(crate) fn segment_rows(engine: &Engine, table: &str) -> usize {
    engine
        .segments(table)
        .iter()
        .map(|(_, rows)| *rows as usize)
        .sum()
}

/// Appends literal rows in one write batch (`NewTxn::Append`).
pub(crate) fn append(engine: &Engine, table: &str, rows: Vec<Vec<Literal>>) -> usize {
    match engine
        .insert_literals(&qualified(table), None, rows)
        .unwrap()
    {
        QueryResult::Inserted { count } => count,
        other => panic!("expected an insert result, got {other:?}"),
    }
}

/// `MakeInputBlock(v1, v2, row_cnt)` for the fixture's
/// `(col1 INT, col2 VARCHAR)` tables.
pub(crate) fn repeated_rows(value: i64, text: &str, row_cnt: usize) -> Vec<Vec<Literal>> {
    (0..row_cnt)
        .map(|_| vec![Literal::Int(value), Literal::Str(text.to_string())])
        .collect()
}

/// Appends `row_cnt` rows carrying the same two values.
pub(crate) fn append_repeated(
    engine: &Engine,
    table: &str,
    value: i64,
    text: &str,
    row_cnt: usize,
) -> usize {
    append(engine, table, repeated_rows(value, text, row_cnt))
}

/// Flushes the WAL/KV and reopens the engine at `dir` — the reference's
/// `RestartTxnMgr` (`replay_test.cppm`).
pub(crate) fn reopen(engine: Engine, dir: &Path) -> Engine {
    engine.flush().unwrap();
    drop(engine);
    Engine::open(dir).unwrap()
}

/// `Status::ok()` for a statement the reference expects to commit.
pub(crate) fn ok(engine: &Engine, sql: &str) {
    engine
        .execute(sql)
        .unwrap_or_else(|e| panic!("expected `{sql}` to succeed: {e}"));
}

/// Result of executing `sql`, for assertions on `QueryResult::{Deleted,
/// Updated, Inserted}`.
pub(crate) fn result(engine: &Engine, sql: &str) -> QueryResult {
    engine.execute(sql).unwrap().remove(0)
}
