//! A text column keeps its cell through the stored form.
//!
//! A stored row is text, and reading it back used to re-infer each cell's type.
//! `str::parse::<f64>` accepts `inf`, `infinity` and `nan` in **any** case
//! (`from_str` compares them with `eq_ignore_ascii_case`), so a stored `iNF`
//! came back as `inf` - the flake `test/sql/dql/sort_top/big_top_varchar.slt`
//! caught on a random fixture - while `007` came back as `7` and `1.50` as
//! `1.5`. A `JSON` cell had the same problem, and a `VARCHAR` cell that spells
//! `null` must still decode as NULL, because the stored form has no other way to
//! say it.

use crate::{Engine, QueryResult};
use tempfile::tempdir;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute_blocking(sql).unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}

fn texts(rows: Vec<Vec<String>>) -> Vec<String> {
    rows.into_iter().map(|r| r.join(" ")).collect()
}

#[test]
fn varchar_cell_is_not_retyped_on_the_way_back() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    rows(&engine, "CREATE TABLE t (c1 VARCHAR, c2 INT);");
    for (i, cell) in ["iNF", "007", "1.50", "infinity", "nan", "true", "null"]
        .iter()
        .enumerate()
    {
        rows(
            &engine,
            &format!("INSERT INTO t VALUES ('{cell}', {});", i + 1),
        );
    }

    // `null` is the spelling the stored form writes for a NULL cell, so that one
    // cell reads back as NULL (rendered `null`); the other six keep their bytes.
    assert_eq!(
        texts(rows(&engine, "SELECT c1 FROM t ORDER BY c2;")),
        vec!["iNF", "007", "1.50", "infinity", "nan", "true", "null"]
    );
    assert_eq!(
        texts(rows(&engine, "SELECT count(*) FROM t WHERE c1 IS NULL;")),
        vec!["1"]
    );
    assert_eq!(
        texts(rows(&engine, "SELECT c2 FROM t WHERE c1 = 'iNF';")),
        vec!["1"]
    );
}

/// The read has to survive a block round trip, not just the in-memory append
/// buffer: `flush` seals the blocks and the second read comes off the files.
#[test]
fn varchar_cell_survives_a_flush() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    rows(&engine, "CREATE TABLE t (c1 VARCHAR, c2 INT);");
    rows(&engine, "INSERT INTO t VALUES ('iNF', 1);");
    rows(&engine, "INSERT INTO t VALUES ('inf', 2);");

    crate::block_on(engine.flush()).unwrap();

    assert_eq!(
        texts(rows(&engine, "SELECT c1, c2 FROM t ORDER BY c2;")),
        vec!["iNF 1", "inf 2"]
    );
}
