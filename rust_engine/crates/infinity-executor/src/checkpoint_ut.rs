//! Ports of `src/unit_test/storage/new_catalog/checkpoint*_ut.cpp`.
//!
//! The reference drives `NewTxn` directly; here `Engine::flush()` is the
//! `kNewCheckpoint` transaction (dump the mem indexes, fsync the catalog and
//! truncate the WAL prefix) and dropping/reopening the engine is
//! `RestartTxnMgr`.

use crate::{Engine, QueryResult};
use infinity_parser::Literal;
use std::path::Path;
use tempfile::tempdir;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { rows, .. } = result {
            return rows;
        }
    }
    Vec::new()
}

/// `SHOW DATABASES` names, sorted (`NewTxn::ListDatabase`).
fn db_names(engine: &Engine) -> Vec<String> {
    let mut names: Vec<String> = rows(engine, "SHOW DATABASES;")
        .into_iter()
        .map(|r| r[0].clone())
        .collect();
    names.sort();
    names
}

/// One 8192-row block of constant values, matching `MakeInputBlock`.
fn constant_rows(n: usize, values: &[Literal]) -> Vec<Vec<Literal>> {
    (0..n).map(|_| values.to_vec()).collect()
}

/// Appends one 8192-row block whose columns hold `values`.
fn append_block(engine: &Engine, table: &str, values: &[Literal]) {
    engine
        .insert_literals(table, None, constant_rows(8192, values))
        .unwrap();
}

/// Drops the engine (releasing the store lock) and opens it again, i.e.
/// `RestartTxnMgr`.
fn restart(old: Engine, dir: &Path) -> Engine {
    drop(old);
    Engine::open(dir).unwrap()
}

/// `RestartTxnMgr` in the `NEW_CONFIG_NOWAL_PATH` configuration.
fn restart_without_wal(old: Engine, dir: &Path) -> Engine {
    drop(old);
    Engine::open_without_wal(dir).unwrap()
}

/// C++: `checkpoint1_ut.cpp:100` (`checkpoint_and_create_db`).
///
/// With the WAL disabled, a database created before a checkpoint survives the
/// restart and one created after the checkpoint is gone.
#[test]
fn checkpoint_and_create_db() {
    // Create db, then checkpoint: the catalog is flushed.
    let dir = tempdir().unwrap();
    let e = Engine::open_without_wal(dir.path()).unwrap();
    e.execute("CREATE DATABASE db1;").unwrap();
    e.flush().unwrap();
    let e = restart_without_wal(e, dir.path());
    assert_eq!(
        db_names(&e),
        vec!["db1".to_string(), "default_db".to_string()]
    );
    drop(e);

    // Checkpoint first, then create db: the DDL is not durable without WAL.
    let dir = tempdir().unwrap();
    let e = Engine::open_without_wal(dir.path()).unwrap();
    e.flush().unwrap();
    e.execute("CREATE DATABASE db1;").unwrap();
    let e = restart_without_wal(e, dir.path());
    assert_eq!(db_names(&e), vec!["default_db".to_string()]);
}

/// C++: `checkpoint1_ut.cpp:233` (`checkpoint_and_create_table`).
///
/// Same rule for a table in `default_db`.
#[test]
fn checkpoint_and_create_table() {
    // Create table, then checkpoint.
    let dir = tempdir().unwrap();
    let e = Engine::open_without_wal(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e.flush().unwrap();
    let e = restart_without_wal(e, dir.path());
    assert!(e.table_exists("default_db.tb1"));
    assert_eq!(e.row_count("default_db.tb1").unwrap(), 0);
    drop(e);

    // Checkpoint first, then create table.
    let dir = tempdir().unwrap();
    let e = Engine::open_without_wal(dir.path()).unwrap();
    e.flush().unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    assert!(e.table_exists("default_db.tb1"));
    let e = restart_without_wal(e, dir.path());
    assert!(!e.table_exists("default_db.tb1"));
    assert!(rows(&e, "SHOW TABLES FROM default_db;").is_empty());
}

/// C++: `checkpoint_add_column_ut.cpp:82` (`addcol_checkpoint_insert`).
///
/// A column added before a checkpoint must read back for rows appended after
/// it, and the checkpoint must preserve both blocks.
#[test]
fn addcol_checkpoint_insert() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    e.insert_literals(
        "db1.tb1",
        None,
        constant_rows(10, &[Literal::Int(1), Literal::Str("abcde".into())]),
    )
    .unwrap();

    e.execute("ALTER TABLE db1.tb1 ADD COLUMN (col3 VARCHAR DEFAULT '');")
        .unwrap();
    e.flush().unwrap();

    e.insert_literals(
        "db1.tb1",
        None,
        constant_rows(
            10,
            &[
                Literal::Int(1),
                Literal::Str("abcde".into()),
                Literal::Str("abcde".into()),
            ],
        ),
    )
    .unwrap();

    let e = restart(e, dir.path());

    // C++ walks every block and sums `BlockInfo::row_count_`. The Rust block
    // mirror only covers flushed rows; the row store (which the reopened
    // engine reconstructs) is authoritative for the second block.
    assert_eq!(e.row_count("db1.tb1").unwrap(), 20);
    assert_eq!(rows(&e, "SELECT count(*) FROM db1.tb1;")[0][0], "20");

    // The added column is part of the persisted schema and old rows kept their
    // values.
    assert_eq!(
        rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col3 = 'abcde';")[0][0],
        "10"
    );
}

/// C++: `checkpoint_internal_ut.cpp:85` (`test_checkpoint0`).
///
/// Two 8192-row appends form one segment with two blocks; two checkpoints and a
/// restart must not duplicate, drop or reorder any row.
#[test]
fn checkpoint_internal_test_checkpoint0() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    append_block(
        &e,
        "db1.tb1",
        &[
            Literal::Int(1),
            Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
        ],
    );
    append_block(
        &e,
        "db1.tb1",
        &[
            Literal::Int(1),
            Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
        ],
    );

    // C++ checkpoints twice before the restart.
    e.flush().unwrap();
    e.flush().unwrap();

    let e = restart(e, dir.path());

    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    assert_eq!(e.block_ids("db1.tb1", 0), vec![0, 1]);
    for block in [0u16, 1u16] {
        assert_eq!(e.block_row_count("db1.tb1", 0, block), Some(8192));
    }
    // `check_column(0, 1)` / `check_column(1, "abc...")` for every row.
    assert_eq!(
        rows(
            &e,
            "SELECT count(*) FROM db1.tb1 \
             WHERE col1 = 1 AND col2 = 'abcdefghijklmnopqrstuvwxyz';"
        )[0][0],
        "16384"
    );
}

/// C++: `checkpoint_internal_ut.cpp:214`
/// (`DISABLED_SLOW_test_checkpoint1`).
///
/// A long DDL/replay sequence: the table survives every checkpoint + restart,
/// follows `RENAME`, and an index created afterwards survives another restart.
#[test]
#[ignore = "SLOW: 8192-row imports interleaved with 15 checkpoints/restarts"]
fn checkpoint_internal_test_checkpoint1() {
    let dir = tempdir().unwrap();
    let mut e = Engine::open(dir.path()).unwrap();
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();

    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    let block = [
        Literal::Int(1),
        Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
    ];
    append_block(&e, "db1.tb1", &block);
    e.flush().unwrap();
    e = restart(e, dir.path());

    // `Import` is another 8192-row batch (the reference's physical import).
    append_block(&e, "db1.tb1", &block);
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    assert!(rows(&e, "SHOW DATABASES;").iter().any(|r| r[0] == "db1"));

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    e.execute(
        "ALTER TABLE db1.tb1 ADD COLUMN (\
         col2 VARCHAR DEFAULT 'abcdefghijklmnopqrstuvwxyz', col3 INT DEFAULT 1);",
    )
    .unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    e.flush().unwrap();
    e = restart(e, dir.path());

    e.execute("ALTER TABLE db1.tb1 RENAME TO db1.renametable;")
        .unwrap();
    assert!(!e.table_exists("db1.tb1"));
    assert!(e.table_exists("db1.renametable"));
    e.flush().unwrap();

    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    e.execute("CREATE INDEX index_name ON db1.renametable (col1, col2);")
        .unwrap();
    e.flush().unwrap();
    e = restart(e, dir.path());

    assert!(e.table_exists("db1.renametable"));
    let count: usize = rows(&e, "SELECT count(*) FROM db1.renametable;")[0][0]
        .parse()
        .unwrap();
    assert_eq!(count, 16384);
}

/// C++: `checkpoint_internal_ut.cpp:436` (`test_checkpoint2`).
///
/// Drop + re-add columns around a checkpoint; the reference asserts only that
/// every DDL transaction succeeds, so the port additionally reads the state
/// back after the restart.
#[test]
fn checkpoint_internal_test_checkpoint2() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    append_block(
        &e,
        "db1.tb1",
        &[
            Literal::Int(1),
            Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
        ],
    );

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .unwrap();
    e.execute(
        "ALTER TABLE db1.tb1 ADD COLUMN (\
         col2 VARCHAR DEFAULT 'abcdefghijklmnopqrstuvwxyz', col3 INT DEFAULT 1);",
    )
    .unwrap();

    e.flush().unwrap();
    let e = restart(e, dir.path());

    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
    assert_eq!(
        rows(
            &e,
            "SELECT count(*) FROM db1.tb1 \
             WHERE col2 = 'abcdefghijklmnopqrstuvwxyz' AND col3 = 1;"
        )[0][0],
        "8192"
    );
}

/// C++: `checkpoint_internal_ut.cpp:549`
/// (`DISABLED_SLOW_test_checkpoint3`).
///
/// A column referenced by an index cannot be dropped; after the index is
/// dropped the column can go, and both checkpoints and restarts happen in
/// between.
#[test]
#[ignore = "SLOW: reference is a long DDL/checkpoint/restart sequence"]
fn checkpoint_internal_test_checkpoint3() {
    let dir = tempdir().unwrap();
    let mut e = Engine::open(dir.path()).unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    e.execute("CREATE DATABASE db1;").unwrap();
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();

    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR, col3 VARCHAR);")
        .unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    e.execute("CREATE INDEX index_name ON db1.tb1 (col2);")
        .unwrap();
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();

    // `DumpMemIndex` — the checkpoint dumps the mem index.
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    // The index still references col2, so the drop is rejected.
    assert!(e
        .execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .is_err());
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col1);")
        .unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    e.optimize_indexes().unwrap();
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();

    e.execute("DROP INDEX index_name ON db1.tb1;").unwrap();
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();

    e.execute("ALTER TABLE db1.tb1 DROP COLUMN (col2);")
        .unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
    e = restart(e, dir.path());

    e.optimize_indexes().unwrap();
    e.flush().unwrap();
    e = restart(e, dir.path());
    e.flush().unwrap();
}

/// C++: `checkpoint_internal_ut.cpp:696`
/// (`DISABLED_SLOW_test_checkpoint4`).
///
/// One segment holding one block; a checkpoint, a restart and a compaction of
/// that segment must all succeed and keep the rows.
#[test]
#[ignore = "SLOW: reference is a long checkpoint/compact/restart sequence"]
fn checkpoint_internal_test_checkpoint4() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    append_block(
        &e,
        "db1.tb1",
        &[
            Literal::Int(1),
            Literal::Str("abcdefghijklmnopqrstuvwxyz".into()),
        ],
    );

    e.flush().unwrap();
    let e = restart(e, dir.path());
    e.flush().unwrap();

    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);

    e.compact_table("db1.tb1").unwrap();
    e.flush().unwrap();
    let e = restart(e, dir.path());
    e.flush().unwrap();

    assert_eq!(e.row_count("db1.tb1").unwrap(), 8192);
}

/// C++: `checkpoint_internal_ut.cpp:790` (`test_checkpoint5`).
///
/// `DROP DATABASE` followed by a checkpoint and a cleanup removes the database
/// for good.
#[test]
fn checkpoint_internal_test_checkpoint5() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE DATABASE db1;").unwrap();

    e.execute("DROP DATABASE db1;").unwrap();
    e.flush().unwrap();
    // `TransactionType::kCleanup` -> `NewTxn::Cleanup`.
    e.cleanup().unwrap();

    assert_eq!(db_names(&e), vec!["default_db".to_string()]);
    assert!(rows(&e, "SHOW DATABASES;").iter().all(|r| r[0] != "db1"));
}
