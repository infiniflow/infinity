//! Ports of `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp`
//! (`TestTxnDumpMemIndex`).
//!
//! The reference drives `kDumpMemIndex` transactions concurrently with DDL and
//! reads `SegmentIndexMeta` / `ChunkIndexMeta`; `Engine::flush()` is the dump
//! (it seals every memory index into the C++-format chunks) and
//! `secondary_tail` / `secondary_chunks` read the same state the reference
//! inspects.
//!
//! Rows whose assertions need a `DumpMemIndex` transaction that *conflicts*
//! with a DDL transaction are reported blocked: `flush()` is synchronous and
//! engine-wide, so it can neither be started and committed later nor fail
//! because a concurrent `DROP`/`RENAME` won.
//!
//! Blocked rows (no test):
//! - 775 `dump_and_drop_db` (9 `EXPECT_FALSE`: dump commits after the db drop
//!   must fail),
//! - 776 `dump_and_drop_table` (5), 779 `dump_and_rename_table` (per-table dump
//!   of the old name must fail after `RENAME`), 781 `dump_and_drop_index` (6),
//! - 782 `dump_and_import` (the reference's `Import` allocates a *new*
//!   segment, so `GetSegmentIDs1() == {0,1}`; every Rust write batch stays in
//!   segment 0 until the 8M-row segment capacity),
//! - 786 `DISABLED_SLOW_test_dump_index_and_optimize_index` (6 conflict
//!   assertions).

use crate::{Engine, QueryResult};
use infinity_parser::Literal;
use tempfile::tempdir;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { rows, .. } = result {
            return rows;
        }
    }
    Vec::new()
}

impl Engine {
    /// Memory tail of a secondary index: `(begin row id, row count)`
    /// (C++ `MemIndex::GetSecondaryIndex()->GetBeginRowID()/GetRowCount()`).
    fn dumped_secondary_tail(&self, table: &str, column: &str) -> Option<(u64, usize)> {
        let key = self.table_key(table);
        let tables = self.tables.lock().unwrap();
        let index = tables.get(&key)?.secondary.indexes.get(column)?;
        Some((index.begin_row_id()?, index.row_count()))
    }

    /// Sealed secondary chunks: `(base row id, row count)` (C++
    /// `ChunkIndexMeta::GetChunkInfo`).
    fn dumped_secondary_chunks(&self, table: &str, column: &str) -> Vec<(u64, usize)> {
        let key = self.table_key(table);
        let tables = self.tables.lock().unwrap();
        tables
            .get(&key)
            .and_then(|e| e.secondary.disk.get(column))
            .map(|chunks| {
                chunks
                    .iter()
                    .map(|(base, reader)| (*base, reader.len()))
                    .collect()
            })
            .unwrap_or_default()
    }
}

/// `TableDef::Make(... {col1 INT, col3 EMBEDDING(FLOAT,4), col4 SPARSE})` plus
/// `IndexSecondary` on `col1` (`index1`).
fn fixture(engine: &Engine) {
    engine.execute("CREATE DATABASE db1;").unwrap();
    engine
        .execute(
            "CREATE TABLE db1.tb1 (col1 INT, col3 EMBEDDING(FLOAT,4), \
             col4 SPARSE(FLOAT,10));",
        )
        .unwrap();
    engine
        .execute("CREATE INDEX index1 ON db1.tb1 (col1);")
        .unwrap();
}

/// One 8192-row `input_block`: `col1` alternates 1/2, `col3` is a constant
/// 4-dim vector and `col4` a constant sparse vector.
fn block() -> Vec<Vec<Literal>> {
    (0..8192)
        .map(|i| {
            vec![
                Literal::Int(if i % 2 == 0 { 1 } else { 2 }),
                Literal::Vector(vec![1.0, 2.0, 3.0, 4.0]),
                Literal::Array(vec!["1:1.0".to_string(), "2:2.0".to_string()]),
            ]
        })
        .collect()
}

fn append_block(engine: &Engine) {
    engine.insert_literals("db1.tb1", None, block()).unwrap();
}

/// `check_index0`: the dump emptied the memory tail and sealed one chunk.
fn assert_dumped(engine: &Engine) {
    assert_eq!(engine.dumped_secondary_tail("db1.tb1", "col1"), None);
    assert_eq!(
        engine.dumped_secondary_chunks("db1.tb1", "col1"),
        vec![(0, 8192)]
    );
}

/// `check_index`: the second block is the memory tail, starting at row 8192.
fn assert_second_block_in_tail(engine: &Engine) {
    assert_eq!(
        engine.dumped_secondary_tail("db1.tb1", "col1"),
        Some((8192, 8192))
    );
}

/// C++: `dump_mem_index_ut.cpp:1151` (`dump_and_add_column`).
///
/// `ADD COLUMN` interleaved with the dump: in both orders the dumped chunk and
/// the memory tail keep their row ids, and the new column appears.
#[test]
fn dump_and_add_column() {
    // dump then add column
    {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        fixture(&e);
        append_block(&e);
        e.flush().unwrap();
        assert_dumped(&e);
        append_block(&e);
        assert_second_block_in_tail(&e);

        e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col5 VARCHAR DEFAULT 'default');")
            .unwrap();
        assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
        assert!(e.execute("SELECT col5 FROM db1.tb1 LIMIT 1;").is_ok());
        e.execute("DROP DATABASE db1;").unwrap();
    }

    // add column then dump
    {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        fixture(&e);
        append_block(&e);
        e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col5 VARCHAR DEFAULT 'default');")
            .unwrap();
        append_block(&e);
        e.flush().unwrap();
        // Both blocks were in the memory tail, so one chunk covers them.
        assert_eq!(
            e.dumped_secondary_chunks("db1.tb1", "col1"),
            vec![(0, 16384)]
        );
        assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
        e.execute("DROP DATABASE db1;").unwrap();
    }
}

/// C++: `dump_mem_index_ut.cpp:1799` (`dump_and_drop_column`).
///
/// The embedding column is dropped (the index is on `col1`), then another block
/// is appended without it; the dumped chunk and the new tail cover the rows.
#[test]
fn dump_and_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    fixture(&e);
    append_block(&e);
    e.flush().unwrap();
    assert_dumped(&e);
    append_block(&e);
    assert_second_block_in_tail(&e);

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col3);")
        .unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    assert_eq!(
        rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col1 IN (1, 2);")[0][0],
        "16384"
    );
    assert!(e.execute("SELECT col3 FROM db1.tb1 LIMIT 1;").is_err());

    // `append_a_block_after_drop_column`: two columns, no embedding.
    let block: Vec<Vec<Literal>> = (0..8192)
        .map(|i| {
            vec![
                Literal::Int(if i % 2 == 0 { 1 } else { 2 }),
                Literal::Array(vec!["1:1.0".to_string()]),
            ]
        })
        .collect();
    e.insert_literals("db1.tb1", None, block).unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 24576);
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++: `dump_mem_index_ut.cpp:2984` (`dump_and_create_index`).
///
/// `CREATE INDEX` with `ConflictType::kError` on the existing name fails; the
/// dump's chunk and the following memory tail are untouched.
#[test]
fn dump_and_create_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    fixture(&e);
    append_block(&e);
    e.flush().unwrap();
    assert_dumped(&e);

    // Duplicate create without IF NOT EXISTS is `ConflictType::kError`.
    assert!(e.execute("CREATE INDEX index1 ON db1.tb1 (col1);").is_err());

    append_block(&e);
    assert_second_block_in_tail(&e);
    assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++: `dump_mem_index_ut.cpp:4566` (`dump_and_append`).
///
/// A dump between two appends seals only the first block; a dump after both
/// appends seals one 16384-row chunk.
#[test]
fn dump_and_append() {
    {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        fixture(&e);
        append_block(&e);
        e.flush().unwrap();
        assert_dumped(&e);
        append_block(&e);
        assert_second_block_in_tail(&e);
        assert_eq!(e.row_count("db1.tb1").unwrap(), 16384);
    }
    {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        fixture(&e);
        append_block(&e);
        append_block(&e);
        e.flush().unwrap();
        assert_eq!(
            e.dumped_secondary_chunks("db1.tb1", "col1"),
            vec![(0, 16384)]
        );
        assert_eq!(e.dumped_secondary_tail("db1.tb1", "col1"), None);
    }
}

/// C++: `dump_mem_index_ut.cpp:5172` (`dump_and_delete`).
///
/// Every odd row is deleted after the dump; the chunk keeps its 8192 rows (the
/// reference does not rewrite chunks on delete) and only the even rows remain
/// visible.
#[test]
fn dump_and_delete() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    fixture(&e);
    append_block(&e);
    e.flush().unwrap();
    assert_dumped(&e);

    // `RowID(0, row_id)` for every odd row id.
    let row_ids: Vec<(u32, u32)> = (1..8192u32).step_by(2).map(|i| (0, i)).collect();
    let mut txn = e.begin_txn();
    assert_eq!(txn.delete_row_ids("db1.tb1", &row_ids).unwrap(), 4096);
    txn.commit().unwrap();

    // `check_index0`: the dump's memory tail stays empty.
    assert_eq!(e.dumped_secondary_tail("db1.tb1", "col1"), None);
    assert_eq!(
        e.dumped_secondary_chunks("db1.tb1", "col1"),
        vec![(0, 8192)]
    );

    // `check_data`: the surviving rows are 0,2,4,... and every one has col1=1.
    assert_eq!(e.row_count("db1.tb1").unwrap(), 4096);
    let col1: Vec<Vec<String>> = rows(&e, "SELECT col1 FROM db1.tb1;");
    assert_eq!(col1.len(), 4096);
    assert!(col1.iter().all(|r| r[0] == "1"));
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++: `dump_mem_index_ut.cpp:5767` (`DISABLED_SLOW_dump_and_dump`).
///
/// Repeated dump/append rounds: each dump seals the tail and the next append
/// starts at the following row id.
#[test]
#[ignore = "SLOW: the reference repeats the dump/append round many times"]
fn dump_and_dump() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    fixture(&e);

    append_block(&e);
    e.flush().unwrap();
    assert_dumped(&e);

    append_block(&e);
    e.flush().unwrap();
    assert_eq!(
        e.dumped_secondary_chunks("db1.tb1", "col1"),
        vec![(0, 8192), (0, 8192)]
    );
    assert_eq!(e.dumped_secondary_tail("db1.tb1", "col1"), None);

    append_block(&e);
    // The third block starts where the second one ended.
    assert_eq!(
        e.dumped_secondary_tail("db1.tb1", "col1"),
        Some((16384, 8192))
    );
    assert_eq!(e.row_count("db1.tb1").unwrap(), 24576);
    e.execute("DROP DATABASE db1;").unwrap();
}

/// C++: `dump_mem_index_ut.cpp:6622` (`test_dump_index_and_compact`).
///
/// A dump followed by a segment compaction keeps every row; the merged segment
/// is the only one left.
#[test]
fn test_dump_index_and_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    fixture(&e);
    append_block(&e);
    e.flush().unwrap();
    assert_dumped(&e);

    e.compact_table("db1.tb1").unwrap();

    assert_eq!(e.segment_count("db1.tb1"), 1);
    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    let col1: Vec<Vec<String>> = rows(&e, "SELECT col1 FROM db1.tb1;");
    assert_eq!(col1.iter().filter(|r| r[0] == "1").count(), 4096);
    assert_eq!(col1.iter().filter(|r| r[0] == "2").count(), 4096);
    e.execute("DROP DATABASE db1;").unwrap();
}
