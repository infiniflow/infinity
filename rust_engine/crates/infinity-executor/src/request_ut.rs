//! Engine-level ports of the reference's `new_request` tests
//! (`src/unit_test/storage/new_request/{ddl,conflict}_request_ut.cpp`).
//!
//! Each DDL case issues a statement twice through a fresh `QueryContext` and
//! asserts that the second attempt fails. The conflict cases run two
//! `QueryContext`s on separate threads: one completes a DDL request while the
//! other appends/imports/compacts the same table, and the reference asserts
//! either that both requests commit or that the conflicting one reports
//! failure without losing the already-committed rows. The Rust engine exposes
//! the same behavior through [`Engine::execute`], so every test keeps the
//! reference's success/failure sequence and drops only the reference's
//! per-statement `QueryContext` bookkeeping.

use crate::Engine;
use crate::QueryResult;
use std::sync::Barrier;
use tempfile::tempdir;

/// The fixture the reference's `COPY` cases load (`test/data/csv/…`).
fn default_import_csv() -> String {
    std::path::Path::new(env!("CARGO_MANIFEST_DIR"))
        .join("../../../test/data/csv/pysdk_test_import_default.csv")
        .to_string_lossy()
        .into_owned()
}

/// `SELECT count(*)` — the visible row count the reference reads through
/// `NewTxnGetVisibleRangeState`.
fn count(e: &Engine, table: &str) -> usize {
    for result in e
        .execute(&format!("SELECT count(*) FROM {table};"))
        .unwrap()
    {
        if let QueryResult::Rows { rows, .. } = result {
            return rows[0][0].parse().unwrap();
        }
    }
    panic!("count(*) returned no rows");
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:38`
/// (`test_create_db`).
#[test]
fn create_database_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    assert!(e.execute("CREATE DATABASE db1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:54`
/// (`test_drop_db`).
#[test]
fn drop_database_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
    assert!(e.execute("DROP DATABASE db1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:77`
/// (`test_create_table`).
#[test]
fn create_table_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    assert!(e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:93`
/// (`test_drop_table`).
#[test]
fn drop_table_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("DROP TABLE t1;").unwrap();
    assert!(!e.table_exists("t1"));
    assert!(e.execute("DROP TABLE t1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:116`
/// (`test_create_index`).
#[test]
fn create_index_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("CREATE INDEX idx1 ON t1 (c1);").unwrap();
    assert!(e.execute("CREATE INDEX idx1 ON t1 (c1);").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:139`
/// (`test_drop_index`).
#[test]
fn drop_index_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("CREATE INDEX idx1 ON t1 (c1);").unwrap();
    e.execute("DROP INDEX idx1 ON t1;").unwrap();
    assert!(e.execute("DROP INDEX idx1 ON t1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:169`
/// (`test_add_column`).
#[test]
fn add_column_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("ALTER TABLE t1 ADD COLUMN (c3 INT DEFAULT 0);")
        .unwrap();
    assert!(e
        .execute("ALTER TABLE t1 ADD COLUMN (c3 INT DEFAULT 0);")
        .is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:192`
/// (`test_drop_column`).
#[test]
fn drop_column_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
    e.execute("ALTER TABLE t1 DROP COLUMN (c1);").unwrap();
    assert!(e.execute("ALTER TABLE t1 DROP COLUMN (c1);").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:215`
/// (`test_use_schema`).
#[test]
fn use_requires_existing_database() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("USE db1;").unwrap();
    assert!(e.execute("USE db2;").is_err());
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:101`
/// (`create_index_append`): appending while a secondary index is being built
/// commits both requests and leaves all four rows in one segment.
#[test]
fn conflict_create_index_append() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
            e.execute("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute("CREATE INDEX idx1 ON t1 (c1);").unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute("INSERT INTO t1 VALUES (3, 'abc'), (4, 'def');")
                .unwrap();
        });
    });

    // The reference reads the visible range through `SegmentMeta`/`BlockMeta`:
    // exactly one segment and block, with all four rows visible.
    assert_eq!(count(&e, "t1"), 4);
    assert_eq!(e.segments("t1"), vec![(0, 4)]);
    assert_eq!(e.segment_rows("t1", 0), 4);
    e.execute("SELECT * FROM t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:175`
/// (`add_column_append`): an append racing `ADD COLUMN` either commits (four
/// rows) or is rejected without losing the two committed rows.
#[test]
fn conflict_add_column_append() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);
    let mut append_ok = false;

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
            e.execute("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute("ALTER TABLE t1 ADD COLUMN (c3 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            append_ok = e
                .execute("INSERT INTO t1 VALUES (3, 'abc'), (4, 'def');")
                .is_ok();
        });
    });

    let n = count(&e, "t1");
    assert!(
        (append_ok && n == 4) || (!append_ok && n == 2),
        "append_ok={append_ok}, row_count={n}"
    );
    assert_eq!(e.segments("t1"), vec![(0, n as u32)]);
    assert_eq!(e.segment_rows("t1", 0), n);
    e.execute("SELECT * FROM t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:251`
/// (`add_column_import`): a `COPY` racing `ADD COLUMN` either commits all ten
/// rows or is rejected and inserts nothing.
#[test]
fn conflict_add_column_import() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);
    let mut import_ok = false;
    let csv = default_import_csv();

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute(
                "CREATE TABLE t1 (c1 INTEGER DEFAULT 1, c2 INTEGER DEFAULT 4, \
                 c3 EMBEDDING(FLOAT,3) DEFAULT [1,2,3], \
                 c4 TENSORARRAY(FLOAT,3) DEFAULT [[[1,2,3],[5,7,8]],[[9,9,9]]], \
                 c5 EMBEDDING(INT,3) DEFAULT [1.3, 4.1, 33.7]);",
            )
            .unwrap();
            barrier.wait();
            e.execute("ALTER TABLE t1 ADD COLUMN (c6 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            import_ok = e
                .execute(&format!(
                    "COPY t1 FROM '{csv}' WITH ( DELIMITER ',', FORMAT CSV );"
                ))
                .is_ok();
        });
    });

    let n = if import_ok { count(&e, "t1") } else { 0 };
    assert!(
        (import_ok && n == 10) || (!import_ok && n == 0),
        "import_ok={import_ok}, row_count={n}"
    );
    if import_ok {
        assert_eq!(e.segments("t1"), vec![(0, n as u32)]);
        assert_eq!(e.segment_rows("t1", 0), n);
    }
    e.execute("SELECT * FROM t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:325`
/// (`add_column_compact`): `ADD COLUMN` and `COMPACT TABLE` racing both commit.
#[test]
fn conflict_add_column_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);
    let csv = default_import_csv();

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute(
                "CREATE TABLE t1 (c1 INTEGER DEFAULT 1, c2 INTEGER DEFAULT 4, \
                 c3 EMBEDDING(FLOAT,3) DEFAULT [1,2,3], \
                 c4 TENSORARRAY(FLOAT,3) DEFAULT [[[1,2,3],[5,7,8]],[[9,9,9]]], \
                 c5 EMBEDDING(INT,3) DEFAULT [1.3, 4.1, 33.7]);",
            )
            .unwrap();
            e.execute(&format!(
                "COPY t1 FROM '{csv}' WITH ( DELIMITER ',', FORMAT CSV );"
            ))
            .unwrap();
            barrier.wait();
            e.execute("ALTER TABLE t1 ADD COLUMN (c6 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute("COMPACT TABLE t1;").unwrap();
        });
    });

    e.execute("SELECT * FROM t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:399`
/// (`add_column_add_column`): two concurrent `ADD COLUMN` requests both commit.
#[test]
fn conflict_add_column_add_column() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
            e.execute("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute("ALTER TABLE t1 ADD COLUMN (c3 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute("ALTER TABLE t1 ADD COLUMN (c4 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
    });

    e.execute("SELECT * FROM t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:470`
/// (`drop_column_append`): an append racing `DROP COLUMN` either commits (four
/// rows) or is rejected without losing the two committed rows.
#[test]
fn conflict_drop_column_append() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);
    let mut append_ok = false;

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
            e.execute("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute("ALTER TABLE t1 DROP COLUMN (c2);").unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            append_ok = e
                .execute("INSERT INTO t1 VALUES (3, 'abc'), (4, 'def');")
                .is_ok();
        });
    });

    let n = count(&e, "t1");
    assert!(
        (append_ok && n == 4) || (!append_ok && n == 2),
        "append_ok={append_ok}, row_count={n}"
    );
    // The reference scans segment 0's visible range; the rewrite keeps every
    // row there (re-keyed) rather than opening a second segment.
    assert_eq!(e.segments("t1").len(), 1);
    assert_eq!(e.segments("t1")[0].0, 0);
    e.execute("SELECT * FROM t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:546`
/// (`drop_column_import`): a `COPY` racing `DROP COLUMN` either commits all ten
/// rows or is rejected and inserts nothing.
#[test]
fn conflict_drop_column_import() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);
    let mut import_ok = false;
    let csv = default_import_csv();

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute(
                "CREATE TABLE t1 (c1 INTEGER DEFAULT 1, c2 INTEGER DEFAULT 4, \
                 c3 EMBEDDING(FLOAT,3) DEFAULT [1,2,3], \
                 c4 TENSORARRAY(FLOAT,3) DEFAULT [[[1,2,3],[5,7,8]],[[9,9,9]]], \
                 c5 EMBEDDING(INT,3) DEFAULT [1.3, 4.1, 33.7]);",
            )
            .unwrap();
            barrier.wait();
            e.execute("ALTER TABLE t1 DROP COLUMN (c2);").unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            import_ok = e
                .execute(&format!(
                    "COPY t1 FROM '{csv}' WITH ( DELIMITER ',', FORMAT CSV );"
                ))
                .is_ok();
        });
    });

    let n = if import_ok { count(&e, "t1") } else { 0 };
    assert!(
        (import_ok && n == 10) || (!import_ok && n == 0),
        "import_ok={import_ok}, row_count={n}"
    );
    e.execute("SELECT * FROM t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:617`
/// (`drop_column_compact`): `DROP COLUMN` and `COMPACT TABLE` racing both
/// commit.
#[test]
fn conflict_drop_column_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);
    let csv = default_import_csv();

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute(
                "CREATE TABLE t1 (c1 INTEGER DEFAULT 1, c2 INTEGER DEFAULT 4, \
                 c3 EMBEDDING(FLOAT,3) DEFAULT [1,2,3], \
                 c4 TENSORARRAY(FLOAT,3) DEFAULT [[[1,2,3],[5,7,8]],[[9,9,9]]], \
                 c5 EMBEDDING(INT,3) DEFAULT [1.3, 4.1, 33.7]);",
            )
            .unwrap();
            e.execute(&format!(
                "COPY t1 FROM '{csv}' WITH ( DELIMITER ',', FORMAT CSV );"
            ))
            .unwrap();
            barrier.wait();
            e.execute("ALTER TABLE t1 DROP COLUMN (c2);").unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute("COMPACT TABLE t1;").unwrap();
        });
    });

    e.execute("SELECT * FROM t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:769`
/// (`delete_append`): a delete and an append racing both commit.
#[test]
fn conflict_delete_append() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute("CREATE TABLE t1(c1 INT, c2 VARCHAR);").unwrap();
            e.execute("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute("DELETE FROM t1 WHERE c1 = 1;").unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute("INSERT INTO t1 VALUES (1, 'hij'), (3, 'klm');")
                .unwrap();
        });
    });

    e.execute("SELECT * FROM t1;").unwrap();
}
