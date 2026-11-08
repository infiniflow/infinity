//! Port of `src/unit_test/storage/new_catalog/alter_ut.cpp`
//! (TEST_AUDIT rows 687-688: `TestTxnAlter.{add_column0, drop_column0}`).
//!
//! The reference drives `NewTxn::AddColumns` / `DropColumns` and then walks the
//! table's `SegmentMeta` / `BlockMeta` chain. The engine exposes the same state
//! through `SHOW TABLE … COLUMNS`, [`Engine::segments`],
//! [`Engine::write_blocks`] and the block helpers; every expected value
//! (segment id, block ids, per-block row counts, visible range, and every
//! column cell) is preserved.

use crate::{Engine, QueryResult};
use parser::Literal;
use tempfile::tempdir;

const BLOCK_ROW_CNT: usize = 8192;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    panic!("query returned no rows: {sql}");
}

/// `SHOW TABLE tb1 COLUMNS` as `(name, type)` pairs.
fn columns(engine: &Engine, table: &str) -> Vec<(String, String)> {
    rows(engine, &format!("SHOW TABLE {table} COLUMNS;"))
        .into_iter()
        .map(|row| (row[0].clone(), row[1].clone()))
        .collect()
}

/// `txn->Append(*db_name, *table_name, input_block)` with `row_cnt` rows.
fn append_block(engine: &Engine, table: &str, cells: Vec<Literal>, row_cnt: usize) {
    let block: Vec<Vec<Literal>> = (0..row_cnt).map(|_| cells.clone()).collect();
    engine.insert_literals(table, None, block).unwrap();
}

/// Asserts the reference's block chain for a single segment 0 split into two
/// 8192-row blocks and checks every cell of every block through the engine.
fn assert_two_blocks(engine: &Engine, table: &str, expected_cells: &[String]) {
    assert_eq!(engine.segment_ids(table), vec![0]);
    assert_eq!(engine.write_blocks(table).unwrap(), 2 * BLOCK_ROW_CNT);
    assert_eq!(engine.block_ids(table, 0), vec![0, 1]);
    let all = rows(engine, &format!("SELECT * FROM {table};"));
    assert_eq!(all.len(), 2 * BLOCK_ROW_CNT);
    for block_id in [0u16, 1] {
        assert_eq!(
            engine.block_row_count(table, 0, block_id),
            Some(BLOCK_ROW_CNT)
        );
        // `NewCatalog::GetBlockVisibleRange`: no deletes, so `[0, row_cnt)`.
        assert_eq!(
            engine.block_visible_range(table, 0, block_id),
            Some((0, BLOCK_ROW_CNT))
        );
        let start = block_id as usize * BLOCK_ROW_CNT;
        for row in &all[start..start + BLOCK_ROW_CNT] {
            assert_eq!(row.as_slice(), expected_cells, "block {block_id}");
        }
    }
}

/// C++ `alter_ut.cpp:63` (`TestTxnAlter.add_column0`): two appends of 8192 rows
/// form blocks 0 and 1 of segment 0; after `ADD COLUMN col2 VARCHAR DEFAULT ''`
/// every old row reads back the empty default in the new column.
#[test]
fn add_column0() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT);").unwrap();

    for _ in 0..2 {
        append_block(&e, "default_db.tb1", vec![Literal::Int(1)], BLOCK_ROW_CNT);
    }

    e.execute("ALTER TABLE tb1 ADD COLUMN (col2 VARCHAR DEFAULT '');")
        .unwrap();

    let column_defs = columns(&e, "default_db.tb1");
    assert_eq!(
        column_defs,
        vec![
            ("col1".to_string(), "Integer".to_string()),
            ("col2".to_string(), "Varchar".to_string()),
        ]
    );
    // The added column's id is 1 (column ids are stable): the reference scans
    // both blocks of segment 0 through `ColumnMeta(1, block)`.
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 2 * BLOCK_ROW_CNT);

    assert_two_blocks(&e, "default_db.tb1", &["1".to_string(), String::new()]);

    e.execute("DROP TABLE tb1;").unwrap();
}

/// C++ `alter_ut.cpp:193` (`TestTxnAlter.drop_column0`): two appends of 8192
/// rows form blocks 0 and 1 of segment 0; after `DROP COLUMN col2` only col1
/// remains and every old row still reads back `1`.
#[test]
fn drop_column0() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    for _ in 0..2 {
        append_block(
            &e,
            "default_db.tb1",
            vec![
                Literal::Int(1),
                Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
            ],
            BLOCK_ROW_CNT,
        );
    }

    e.execute("ALTER TABLE tb1 DROP COLUMN (col2);").unwrap();

    // `GetColumnDefs`: col2 (id 1) is gone, col1 (id 0) remains.
    assert_eq!(
        columns(&e, "default_db.tb1"),
        vec![("col1".to_string(), "Integer".to_string())]
    );
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 2 * BLOCK_ROW_CNT);
    assert!(e.execute("SELECT col2 FROM tb1;").is_err());

    assert_two_blocks(&e, "default_db.tb1", &["1".to_string()]);

    e.execute("DROP TABLE tb1;").unwrap();
}

/// A row that only the WAL holds, replayed after the catalog already carries
/// the `ADD`/`DROP COLUMN`, still lands in the right cells.
///
/// A DDL's catalog record is written when the statement commits, while rows
/// stay in the append buffer until a checkpoint. A restart in between therefore
/// replays rows written **before** the schema change against a catalog that
/// already has it. In `test_alter_complex` that state made the old `c2` read
/// back out of `c3`: the replay skipped the row's reordering because the column
/// was already gone from the catalog.
///
/// This is the deterministic form of that flake - no checkpoint, no kill, just
/// the two commit boundaries the reference has.
#[test]
fn replay_of_rows_that_predate_a_committed_column_change() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (c1 INT, c2 INT, c3 VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (1, 2, 'before');")
        .unwrap();
    // Both DDLs are in the catalog now; the row above is still only in the WAL.
    e.execute("ALTER TABLE tb1 ADD COLUMN (c4 VARCHAR DEFAULT 'default');")
        .unwrap();
    e.execute("ALTER TABLE tb1 DROP COLUMN (c2);").unwrap();
    // A row written after the changes: the replayed table has to converge on
    // one schema for both rows.
    e.execute("INSERT INTO tb1 VALUES (2, 'after', 'x');")
        .unwrap();

    // `RestartTxnMgr`: drop the engine (no flush, so the rows are replayed from
    // the WAL) and open the same directory again.
    drop(e);
    let e = Engine::open(dir.path()).unwrap();

    // id 0 keeps 1, c2 (id 1) is gone, c3 (id 2) keeps its own value and c4
    // (id 3) holds the default it was added with.
    assert_eq!(
        rows(&e, "SELECT * FROM tb1 ORDER BY c1;"),
        vec![
            vec!["1".to_string(), "before".to_string(), "default".to_string()],
            vec!["2".to_string(), "after".to_string(), "x".to_string()],
        ]
    );
}

/// Minimal form of the same bug: only the `DROP` is committed, so a replayed
/// three-cell row is read against a two-column schema and `c3` takes the old
/// `c2`.
#[test]
fn replay_of_a_row_that_predates_a_committed_drop_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (c1 INT, c2 INT, c3 VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (1, 2, 'keep');").unwrap();
    e.execute("ALTER TABLE tb1 DROP COLUMN (c2);").unwrap();

    drop(e);
    let e = Engine::open(dir.path()).unwrap();

    assert_eq!(
        rows(&e, "SELECT * FROM tb1;"),
        vec![vec!["1".to_string(), "keep".to_string()]]
    );
}

/// The complementary case: `ADD` alone. The replayed frame is short by one
/// cell, which the append path pads with the new column's default, so this
/// shape has always been benign - it is here to keep it that way.
#[test]
fn replay_of_a_row_that_predates_a_committed_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (c1 INT, c2 INT);").unwrap();
    e.execute("INSERT INTO tb1 VALUES (1, 2);").unwrap();
    e.execute("ALTER TABLE tb1 ADD COLUMN (c3 VARCHAR DEFAULT 'added');")
        .unwrap();

    drop(e);
    let e = Engine::open(dir.path()).unwrap();

    assert_eq!(
        rows(&e, "SELECT * FROM tb1;"),
        vec![vec!["1".to_string(), "2".to_string(), "added".to_string()]]
    );
}

/// The `test_alter_complex` sequence: `c2` is dropped and added again under a
/// new id, which the timeline has to undo in the right order.
#[test]
fn replay_of_drop_then_readd_a_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (c1 INT, c2 INT, c3 VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (1, 2, 'test');").unwrap();
    e.execute("ALTER TABLE tb1 ADD COLUMN (c4 VARCHAR DEFAULT 'tttt');")
        .unwrap();
    e.execute("ALTER TABLE tb1 DROP COLUMN (c2);").unwrap();
    e.execute("ALTER TABLE tb1 ADD COLUMN (c2 FLOAT DEFAULT 0.0);")
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (2, 'test2', 'x', 1.5);")
        .unwrap();

    drop(e);
    let e = Engine::open(dir.path()).unwrap();

    assert_eq!(
        rows(&e, "SELECT c1, c3, c4 FROM tb1 ORDER BY c1;"),
        vec![
            vec!["1".to_string(), "test".to_string(), "tttt".to_string()],
            vec!["2".to_string(), "test2".to_string(), "x".to_string()],
        ]
    );
    // The re-added column keeps its own values: the float default for the row
    // that predates it, and the inserted one.
    assert_eq!(
        rows(&e, "SELECT count(*) FROM tb1 WHERE c2 = 0.0;"),
        vec![vec!["1".to_string()]]
    );
    assert_eq!(
        rows(&e, "SELECT count(*) FROM tb1 WHERE c2 = 1.5;"),
        vec![vec!["1".to_string()]]
    );
}

/// The same sequence with the checkpoint on the other side of the DDLs: the
/// rows are in a block and the catalog is ahead of it, which is the state the
/// restart suite hits when the periodic checkpoint fires.
#[test]
fn replay_of_drop_then_readd_a_column_around_a_checkpoint() {
    for checkpoint_after_ddl in [true, false] {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE tb1 (c1 INT, c2 INT, c3 VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO tb1 VALUES (1, 2, 'test');").unwrap();
        if !checkpoint_after_ddl {
            e.flush().unwrap();
        }
        e.execute("ALTER TABLE tb1 ADD COLUMN (c4 VARCHAR DEFAULT 'tttt');")
            .unwrap();
        e.execute("ALTER TABLE tb1 DROP COLUMN (c2);").unwrap();
        e.execute("ALTER TABLE tb1 ADD COLUMN (c2 FLOAT DEFAULT 0.0);")
            .unwrap();
        e.execute("INSERT INTO tb1 VALUES (2, 'test2', 'x', 1.5);")
            .unwrap();
        if checkpoint_after_ddl {
            e.flush().unwrap();
        }

        drop(e);
        let e = Engine::open(dir.path()).unwrap();

        assert_eq!(
            rows(&e, "SELECT c1, c3, c4 FROM tb1 ORDER BY c1;"),
            vec![
                vec!["1".to_string(), "test".to_string(), "tttt".to_string()],
                vec!["2".to_string(), "test2".to_string(), "x".to_string()],
            ],
            "checkpoint after ddl: {checkpoint_after_ddl}"
        );
        assert_eq!(
            rows(&e, "SELECT count(*) FROM tb1 WHERE c2 = 0.0;"),
            vec![vec!["1".to_string()]],
            "checkpoint after ddl: {checkpoint_after_ddl}"
        );
    }
}

/// Exactly `test_alter_complex[False-False]`: a full-text index, a column
/// dropped and added again, then one more column after the second insert.
#[test]
fn replay_of_the_test_alter_complex_sequence() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (c1 INT, c2 INT, c3 VARCHAR);")
        .unwrap();
    e.execute("CREATE INDEX idx1 ON tb1 (c3) USING FULLTEXT;")
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (1, 2, 'test');").unwrap();
    e.execute("ALTER TABLE tb1 ADD COLUMN (c4 VARCHAR DEFAULT 'tttt');")
        .unwrap();
    e.execute("ALTER TABLE tb1 DROP COLUMN (c2);").unwrap();
    e.execute("ALTER TABLE tb1 ADD COLUMN (c2 FLOAT DEFAULT 0.0);")
        .unwrap();
    e.execute("INSERT INTO tb1 VALUES (2, 'test2', 'tttt2', 1.0);")
        .unwrap();
    e.execute("ALTER TABLE tb1 ADD COLUMN (c5 INT DEFAULT 0);")
        .unwrap();

    drop(e);
    let e = Engine::open(dir.path()).unwrap();

    assert_eq!(
        rows(&e, "SELECT c1, c3, c4 FROM tb1 ORDER BY c1;"),
        vec![
            vec!["1".to_string(), "test".to_string(), "tttt".to_string()],
            vec!["2".to_string(), "test2".to_string(), "tttt2".to_string()],
        ]
    );
    assert_eq!(
        rows(&e, "SELECT count(*) FROM tb1 WHERE c2 = 1.0;"),
        vec![vec!["1".to_string()]]
    );
    assert_eq!(
        rows(&e, "SELECT count(*) FROM tb1 WHERE c5 = 0;"),
        vec![vec!["2".to_string()]]
    );
}

/// The same sequence with a checkpoint after each step in turn: the periodic
/// checkpoint (`checkpoint_interval = 1s`) lands wherever it likes, so the
/// rows and the catalog can be on either side of it.
#[test]
fn replay_of_the_test_alter_complex_sequence_around_a_checkpoint() {
    for checkpoint_after in 0..=5 {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE tb1 (c1 INT, c2 INT, c3 VARCHAR);")
            .unwrap();
        e.execute("CREATE INDEX idx1 ON tb1 (c3) USING FULLTEXT;")
            .unwrap();
        e.execute("INSERT INTO tb1 VALUES (1, 2, 'test');").unwrap();
        if checkpoint_after == 1 {
            e.flush().unwrap();
        }
        e.execute("ALTER TABLE tb1 ADD COLUMN (c4 VARCHAR DEFAULT 'tttt');")
            .unwrap();
        if checkpoint_after == 2 {
            e.flush().unwrap();
        }
        e.execute("ALTER TABLE tb1 DROP COLUMN (c2);").unwrap();
        if checkpoint_after == 3 {
            e.flush().unwrap();
        }
        e.execute("ALTER TABLE tb1 ADD COLUMN (c2 FLOAT DEFAULT 0.0);")
            .unwrap();
        if checkpoint_after == 4 {
            e.flush().unwrap();
        }
        e.execute("INSERT INTO tb1 VALUES (2, 'test2', 'tttt2', 1.0);")
            .unwrap();
        if checkpoint_after == 5 {
            e.flush().unwrap();
        }
        e.execute("ALTER TABLE tb1 ADD COLUMN (c5 INT DEFAULT 0);")
            .unwrap();

        drop(e);
        let e = Engine::open(dir.path()).unwrap();

        assert_eq!(
            rows(&e, "SELECT c1, c3, c4 FROM tb1 ORDER BY c1;"),
            vec![
                vec!["1".to_string(), "test".to_string(), "tttt".to_string()],
                vec!["2".to_string(), "test2".to_string(), "tttt2".to_string()],
            ],
            "checkpoint after step {checkpoint_after}"
        );
        assert_eq!(
            rows(&e, "SELECT count(*) FROM tb1 WHERE c2 = 1.0;"),
            vec![vec!["1".to_string()]],
            "checkpoint after step {checkpoint_after}"
        );
    }
}

/// A checkpoint running through a column change and the commit that follows
/// must not leave the block half written.
///
/// `PeriodicTriggerThread` runs `checkpoint_wal()` + `flush()` on its own
/// thread; the column change writes one file per block (`AddColumnsData`)
/// while the checkpoint rewrites the whole block. Two writers on one block
/// directory produced a `version` that did not match the column files, and the
/// next commit failed its own "the chunk tiles the committed window" check.
#[test]
fn a_checkpoint_through_a_column_change_keeps_the_block_readable() {
    for _ in 0..50 {
        let dir = tempdir().unwrap();
        let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
        e.execute("CREATE TABLE tb1 (c1 INT, c2 INT, c3 VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO tb1 VALUES (1, 2, 'test');").unwrap();
        let stop = std::sync::Arc::new(std::sync::atomic::AtomicBool::new(false));
        let flusher = {
            let e = std::sync::Arc::clone(&e);
            let stop = std::sync::Arc::clone(&stop);
            std::thread::spawn(move || {
                while !stop.load(std::sync::atomic::Ordering::Relaxed) {
                    // What `PeriodicTriggerThread::Run` does each tick.
                    e.checkpoint_wal();
                    let _ = e.flush();
                }
            })
        };
        e.execute("ALTER TABLE tb1 ADD COLUMN (c4 VARCHAR DEFAULT 'tttt');")
            .unwrap();
        e.execute("ALTER TABLE tb1 DROP COLUMN (c2);").unwrap();
        e.execute("ALTER TABLE tb1 ADD COLUMN (c2 FLOAT DEFAULT 0.0);")
            .unwrap();
        e.execute("INSERT INTO tb1 VALUES (2, 'test2', 'tttt2', 1.0);")
            .unwrap();
        stop.store(true, std::sync::atomic::Ordering::Relaxed);
        flusher.join().unwrap();
        e.flush().unwrap();

        // The block is the only copy after a restart.
        drop(e);
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            rows(&e, "SELECT c1, c3, c4 FROM tb1 ORDER BY c1;"),
            vec![
                vec!["1".to_string(), "test".to_string(), "tttt".to_string()],
                vec!["2".to_string(), "test2".to_string(), "tttt2".to_string()],
            ]
        );
    }
}
/// Two column changes racing on one table: whoever loses the conflict must
/// leave nothing behind (no catalog record, no buffer cell, no column file),
/// and the directory must reopen to exactly the table the winner produced.
#[test]
fn concurrent_column_changes_leave_no_side_effect_behind() {
    for _ in 0..20 {
        let dir = tempdir().unwrap();
        let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
        e.execute("CREATE TABLE tb1 (c1 INT, c2 VARCHAR);").unwrap();
        e.execute("INSERT INTO tb1 VALUES (1, 'a');").unwrap();
        let mut handles = Vec::new();
        for k in 0..2 {
            let e = std::sync::Arc::clone(&e);
            handles.push(std::thread::spawn(move || {
                e.execute(&format!(
                    "ALTER TABLE tb1 ADD COLUMN (c{k} INT DEFAULT {k});"
                ))
                .is_ok()
            }));
        }
        let added = handles
            .into_iter()
            .map(|handle| handle.join().unwrap())
            .filter(|ok| *ok)
            .count();
        let schema = columns(&e, "default_db.tb1");
        assert_eq!(schema.len(), 2 + added);
        // Every cell of the surviving schema reads back, and a reopen (WAL
        // replay) produces the same table.
        let before = rows(&e, "SELECT * FROM tb1;");
        assert_eq!(before.len(), 1);
        drop(e);
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(columns(&e, "default_db.tb1"), schema);
        assert_eq!(rows(&e, "SELECT * FROM tb1;"), before);
    }
}

/// A column change touches **one file per block**, not the block.
///
/// NewTxn::AddColumnsData writes the new column's {id}.col for every block
/// that already exists and leaves every other column file, and the block's
/// version, byte for byte alone; DropColumnsData writes a marker per block and
/// only Cleanup removes the file. The port used to re-encode the whole block,
/// which rewrote every column file and the version (measured against the
/// reference: the version file differed after a plain ADD COLUMN).
#[test]
fn a_column_change_touches_only_its_own_files() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 INT, col3 VARCHAR);")
        .unwrap();
    append_block(
        &e,
        "default_db.tb1",
        vec![
            Literal::Int(1),
            Literal::Int(2),
            Literal::Str("x".to_string()),
        ],
        4,
    );
    e.flush().unwrap();

    let block = dir.path().join("data/db_1/tbl_0/seg_0/blk_0");
    let read = |name: &str| std::fs::read(block.join(name)).unwrap();
    let before: Vec<(String, Vec<u8>)> = ["0.col", "1.col", "2.col", "col_2_out", "version"]
        .iter()
        .map(|n| (n.to_string(), read(n)))
        .collect();

    e.execute("ALTER TABLE tb1 ADD COLUMN (col4 INT DEFAULT 7);")
        .unwrap();
    e.flush().unwrap();
    // Only the new column's file is new; every other file is unchanged.
    for (name, bytes) in &before {
        assert_eq!(&read(name), bytes, "{name} must not change on ADD COLUMN");
    }
    assert_eq!(rows(&e, "SELECT col4 FROM tb1;")[0], vec!["7".to_string()]);

    let mut names: Vec<String> = std::fs::read_dir(&block)
        .unwrap()
        .flatten()
        .map(|e| e.file_name().to_string_lossy().to_string())
        .collect();
    names.sort();
    assert_eq!(
        names,
        vec!["0.col", "1.col", "2.col", "3.col", "col_2_out", "version"]
    );

    // DROP leaves the file in place until Cleanup, and the marker keeps the
    // column out of the schema in the meantime.
    e.execute("ALTER TABLE tb1 DROP COLUMN (col2);").unwrap();
    assert!(block.join("1.col").exists());
    assert_eq!(
        columns(&e, "default_db.tb1"),
        vec![
            ("col1".to_string(), "Integer".to_string()),
            ("col3".to_string(), "Varchar".to_string()),
            ("col4".to_string(), "Integer".to_string()),
        ]
    );
    // The file survives a checkpoint and a restart.
    e.flush().unwrap();
    drop(e);
    let e = Engine::open(dir.path()).unwrap();
    assert!(block.join("1.col").exists());
    assert_eq!(rows(&e, "SELECT col4 FROM tb1;")[0], vec!["7".to_string()]);

    e.cleanup().unwrap();
    assert!(!block.join("1.col").exists());
}
