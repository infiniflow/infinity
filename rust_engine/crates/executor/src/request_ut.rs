//! Engine-level ports of the reference's `new_request` tests
//! (`src/unit_test/storage/new_request/{ddl,conflict,index,data}_request_ut.cpp`).
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

/// The rows of a query, as rendered strings.
fn rows(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in e.execute_blocking(sql).unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    panic!("query returned no rows: {sql}");
}

/// `SELECT count(*)` — the visible row count the reference reads through
/// `NewTxnGetVisibleRangeState`.
fn count(e: &Engine, table: &str) -> usize {
    for result in e
        .execute_blocking(&format!("SELECT count(*) FROM {table};"))
        .unwrap()
    {
        if let QueryResult::Rows { rows, .. } = result {
            return rows[0][0].to_slt_string().parse().unwrap();
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

    e.execute_blocking("CREATE DATABASE db1;").unwrap();
    assert!(e.execute_blocking("CREATE DATABASE db1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:54`
/// (`test_drop_db`).
#[test]
fn drop_database_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE DATABASE db1;").unwrap();
    e.execute_blocking("DROP DATABASE db1;").unwrap();
    assert!(e.execute_blocking("DROP DATABASE db1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:77`
/// (`test_create_table`).
#[test]
fn create_table_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    assert!(e
        .execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:93`
/// (`test_drop_table`).
#[test]
fn drop_table_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("DROP TABLE t1;").unwrap();
    assert!(!e.table_exists("t1"));
    assert!(e.execute_blocking("DROP TABLE t1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:116`
/// (`test_create_index`).
#[test]
fn create_index_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("CREATE INDEX idx1 ON t1 (c1);").unwrap();
    assert!(e.execute_blocking("CREATE INDEX idx1 ON t1 (c1);").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:139`
/// (`test_drop_index`).
#[test]
fn drop_index_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("CREATE INDEX idx1 ON t1 (c1);").unwrap();
    e.execute_blocking("DROP INDEX idx1 ON t1;").unwrap();
    assert!(e.execute_blocking("DROP INDEX idx1 ON t1;").is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:169`
/// (`test_add_column`).
#[test]
fn add_column_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("ALTER TABLE t1 ADD COLUMN (c3 INT DEFAULT 0);")
        .unwrap();
    assert!(e
        .execute_blocking("ALTER TABLE t1 ADD COLUMN (c3 INT DEFAULT 0);")
        .is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:192`
/// (`test_drop_column`).
#[test]
fn drop_column_succeeds_once() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("ALTER TABLE t1 DROP COLUMN (c1);")
        .unwrap();
    assert!(e
        .execute_blocking("ALTER TABLE t1 DROP COLUMN (c1);")
        .is_err());
}

/// C++: `src/unit_test/storage/new_request/ddl_request_ut.cpp:215`
/// (`test_use_schema`).
#[test]
fn use_requires_existing_database() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE DATABASE db1;").unwrap();
    e.execute_blocking("USE db1;").unwrap();
    assert!(e.execute_blocking("USE db2;").is_err());
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
            e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
                .unwrap();
            e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute_blocking("CREATE INDEX idx1 ON t1 (c1);").unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute_blocking("INSERT INTO t1 VALUES (3, 'abc'), (4, 'def');")
                .unwrap();
        });
    });

    // The reference reads the visible range through `SegmentMeta`/`BlockMeta`:
    // exactly one segment and block, with all four rows visible.
    assert_eq!(count(&e, "t1"), 4);
    assert_eq!(e.segments("t1"), vec![(0, 4)]);
    assert_eq!(e.segment_rows("t1", 0), 4);
    e.execute_blocking("SELECT * FROM t1;").unwrap();
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
            e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
                .unwrap();
            e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute_blocking("ALTER TABLE t1 ADD COLUMN (c3 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            append_ok = e
                .execute_blocking("INSERT INTO t1 VALUES (3, 'abc'), (4, 'def');")
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
    e.execute_blocking("SELECT * FROM t1;").unwrap();
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
            e.execute_blocking(
                "CREATE TABLE t1 (c1 INTEGER DEFAULT 1, c2 INTEGER DEFAULT 4, \
                 c3 EMBEDDING(FLOAT,3) DEFAULT [1,2,3], \
                 c4 TENSORARRAY(FLOAT,3) DEFAULT [[[1,2,3],[5,7,8]],[[9,9,9]]], \
                 c5 EMBEDDING(INT,3) DEFAULT [1.3, 4.1, 33.7]);",
            )
            .unwrap();
            barrier.wait();
            e.execute_blocking("ALTER TABLE t1 ADD COLUMN (c6 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            import_ok = e
                .execute_blocking(&format!(
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
    e.execute_blocking("SELECT * FROM t1;").unwrap();
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
            e.execute_blocking(
                "CREATE TABLE t1 (c1 INTEGER DEFAULT 1, c2 INTEGER DEFAULT 4, \
                 c3 EMBEDDING(FLOAT,3) DEFAULT [1,2,3], \
                 c4 TENSORARRAY(FLOAT,3) DEFAULT [[[1,2,3],[5,7,8]],[[9,9,9]]], \
                 c5 EMBEDDING(INT,3) DEFAULT [1.3, 4.1, 33.7]);",
            )
            .unwrap();
            e.execute_blocking(&format!(
                "COPY t1 FROM '{csv}' WITH ( DELIMITER ',', FORMAT CSV );"
            ))
            .unwrap();
            barrier.wait();
            e.execute_blocking("ALTER TABLE t1 ADD COLUMN (c6 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute_blocking("COMPACT TABLE t1;").unwrap();
        });
    });

    e.execute_blocking("SELECT * FROM t1;").unwrap();
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
            e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
                .unwrap();
            e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute_blocking("ALTER TABLE t1 ADD COLUMN (c3 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute_blocking("ALTER TABLE t1 ADD COLUMN (c4 VARCHAR DEFAULT 'default');")
                .unwrap();
        });
    });

    e.execute_blocking("SELECT * FROM t1;").unwrap();
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
            e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
                .unwrap();
            e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute_blocking("ALTER TABLE t1 DROP COLUMN (c2);")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            append_ok = e
                .execute_blocking("INSERT INTO t1 VALUES (3, 'abc'), (4, 'def');")
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
    e.execute_blocking("SELECT * FROM t1;").unwrap();
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
            e.execute_blocking(
                "CREATE TABLE t1 (c1 INTEGER DEFAULT 1, c2 INTEGER DEFAULT 4, \
                 c3 EMBEDDING(FLOAT,3) DEFAULT [1,2,3], \
                 c4 TENSORARRAY(FLOAT,3) DEFAULT [[[1,2,3],[5,7,8]],[[9,9,9]]], \
                 c5 EMBEDDING(INT,3) DEFAULT [1.3, 4.1, 33.7]);",
            )
            .unwrap();
            barrier.wait();
            e.execute_blocking("ALTER TABLE t1 DROP COLUMN (c2);")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            import_ok = e
                .execute_blocking(&format!(
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
    e.execute_blocking("SELECT * FROM t1;").unwrap();
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
            e.execute_blocking(
                "CREATE TABLE t1 (c1 INTEGER DEFAULT 1, c2 INTEGER DEFAULT 4, \
                 c3 EMBEDDING(FLOAT,3) DEFAULT [1,2,3], \
                 c4 TENSORARRAY(FLOAT,3) DEFAULT [[[1,2,3],[5,7,8]],[[9,9,9]]], \
                 c5 EMBEDDING(INT,3) DEFAULT [1.3, 4.1, 33.7]);",
            )
            .unwrap();
            e.execute_blocking(&format!(
                "COPY t1 FROM '{csv}' WITH ( DELIMITER ',', FORMAT CSV );"
            ))
            .unwrap();
            barrier.wait();
            e.execute_blocking("ALTER TABLE t1 DROP COLUMN (c2);")
                .unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute_blocking("COMPACT TABLE t1;").unwrap();
        });
    });

    e.execute_blocking("SELECT * FROM t1;").unwrap();
}

/// C++: `src/unit_test/storage/new_request/conflict_request_ut.cpp:693`
/// (`drop_column_create_index`): dropping the indexed column while the index is
/// being built leaves at least one of the two committed, and the table stays
/// readable either way.
#[test]
fn conflict_drop_column_create_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    let barrier = Barrier::new(2);
    let mut drop_ok = false;
    let mut index_ok = false;

    std::thread::scope(|s| {
        s.spawn(|| {
            e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
                .unwrap();
            e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            drop_ok = e
                .execute_blocking("ALTER TABLE t1 DROP COLUMN (c1);")
                .is_ok();
        });
        s.spawn(|| {
            barrier.wait();
            index_ok = e.execute_blocking("CREATE INDEX idx1 ON t1 (c1);").is_ok();
        });
    });

    assert!(
        drop_ok || index_ok,
        "at least one of DROP COLUMN / CREATE INDEX must commit"
    );
    // The reference reads the table back after either outcome; the schema it
    // ends with is the one that committed.
    e.execute_blocking("SELECT * FROM t1;").unwrap();
    if index_ok && !drop_ok {
        assert_eq!(count(&e, "t1"), 2);
    }
}

/// `DROP COLUMN` racing `RENAME TABLE`: the row count must not double.
///
/// The drop re-encodes every row without its column; the rename only moves
/// catalog records. Both leave the append buffer holding the ids the drop
/// rewrote, and when the rename wins the drop's `pending_rows.retain` runs
/// against the entry the rename migrated, so the ids stay listed while the
/// blocks already hold them. `SELECT *` merges the buffer over the blocks and
/// skips duplicate ids; `count(*)` added both sides and reported twice the
/// table.
#[test]
fn conflict_drop_column_rename() {
    for round in 0..30 {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        let barrier = Barrier::new(2);
        let mut drop_ok = false;
        let mut rename_ok = false;
        std::thread::scope(|s| {
            s.spawn(|| {
                e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
                    .unwrap();
                e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                    .unwrap();
                barrier.wait();
                drop_ok = e
                    .execute_blocking("ALTER TABLE t1 DROP COLUMN (c1);")
                    .is_ok();
            });
            s.spawn(|| {
                barrier.wait();
                rename_ok = e.execute_blocking("ALTER TABLE t1 RENAME TO t2;").is_ok();
            });
        });
        let name = if rename_ok { "t2" } else { "t1" };
        let read = e.execute_blocking(&format!("SELECT * FROM {name};"));
        assert!(
            read.is_ok(),
            "round {round}: drop_ok={drop_ok} rename_ok={rename_ok} read({name}) failed: {:?}",
            read.err()
        );
        // A scan and a count read the same table, so they must agree.
        let n = count(&e, name);
        assert!(
            n == 2,
            "round {round}: drop_ok={drop_ok} rename_ok={rename_ok} rows={n}"
        );
    }
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
            e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
                .unwrap();
            e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
                .unwrap();
            barrier.wait();
            e.execute_blocking("DELETE FROM t1 WHERE c1 = 1;").unwrap();
        });
        s.spawn(|| {
            barrier.wait();
            e.execute_blocking("INSERT INTO t1 VALUES (1, 'hij'), (3, 'klm');")
                .unwrap();
        });
    });

    e.execute_blocking("SELECT * FROM t1;").unwrap();
}

/// Asserts that `sql` returns a first row whose first column is `expected`
/// (the reference reads `data_blocks_[0]->column_vectors_[0]->GetValueByIndex(0)`).
fn assert_first(e: &Engine, sql: &str, expected: &str) {
    let got = rows(e, sql);
    assert_eq!(
        got.first().map(|row| row[0].as_str()),
        Some(expected),
        "{sql}"
    );
}

/// C++: `src/unit_test/storage/new_request/index_request_ut.cpp:43`
/// (`index_scan`): a secondary index answers a range scan.
#[test]
fn index_scan_returns_filtered_rows() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
        .unwrap();
    e.execute_blocking("CREATE INDEX idx1 ON t1 (c1);").unwrap();

    assert_eq!(
        rows(&e, "SELECT * FROM t1 WHERE c1 > 1;"),
        vec![vec!["2".to_string(), "def".to_string()]]
    );
}

/// C++: `src/unit_test/storage/new_request/index_request_ut.cpp:88`
/// (`fulltext_index_scan`): a full-text index answers `MATCH TEXT`.
#[test]
fn fulltext_index_scan_returns_matching_row() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
        .unwrap();
    e.execute_blocking("CREATE INDEX idx2 ON t1 (c2) USING FULLTEXT;")
        .unwrap();

    assert_eq!(
        rows(
            &e,
            "SELECT * FROM t1 SEARCH MATCH TEXT ('c2', 'def', 'topn=1');"
        ),
        vec![vec!["2".to_string(), "def".to_string()]]
    );
}

/// C++: `src/unit_test/storage/new_request/index_request_ut.cpp:132`
/// (`vector_index_scan`): `MATCH VECTOR` ranks the same top-1 before any
/// index, through the HNSW index (chosen implicitly and by `USING INDEX`), and
/// through the IVF index.
#[test]
fn vector_index_scan_matches_with_hnsw_and_ivf() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 EMBEDDING(FLOAT,4));")
        .unwrap();
    e.execute_blocking("INSERT INTO t1 VALUES (1, [0.1,0.1,0.1,0.1]), (2, [0.2,0.2,0.2,0.2]);")
        .unwrap();

    let search = "SELECT c1 FROM t1 SEARCH MATCH VECTOR \
                  (c2, [0.3,0.3,0.2,0.2], 'float', 'l2', 1)";
    assert_first(&e, &format!("{search};"), "2");

    e.execute_blocking(
        "CREATE INDEX idx1 ON t1 (c2) USING HNSW \
         WITH (M=16, ef_construction=200, metric=l2);",
    )
    .unwrap();
    assert_first(&e, &format!("{search};"), "2");
    assert_first(&e, &format!("{search} USING INDEX (idx1);"), "2");

    e.execute_blocking("CREATE INDEX idx2 ON t1 (c2) USING IVF WITH (metric=l2);")
        .unwrap();
    assert_first(&e, &format!("{search} USING INDEX (idx2);"), "2");
}

/// C++: `src/unit_test/storage/new_request/index_request_ut.cpp:186`
/// (`sparse_index_scan`): `MATCH SPARSE` ranks the same top-1 before and after
/// the BMP index is built.
#[test]
fn sparse_index_scan_matches_with_bmp() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 SPARSE(FLOAT,100));")
        .unwrap();
    e.execute_blocking(
        "INSERT INTO t1 VALUES (1, [10:1.0, 20:1.0, 30:1.0]), \
         (2, [40:1.0, 50:1.0, 60:1.0]);",
    )
    .unwrap();

    let search = "SELECT c1 FROM t1 SEARCH MATCH SPARSE \
                  (c2, [20:1.0,30:1.0,40:1.0], 'ip', 1);";
    assert_first(&e, search, "1");

    e.execute_blocking(
        "CREATE INDEX idx2 ON t1 (c2) USING BMP \
         WITH (block_size=8, compress_type=compress);",
    )
    .unwrap();
    assert_first(&e, search, "1");
}

/// C++: `src/unit_test/storage/new_request/index_request_ut.cpp:226`
/// (`tensor_index_scan`): `MATCH TENSOR` applies MaxSim over the 4-wide leaves
/// of each row, with and without the EMVB index.
#[test]
fn tensor_index_scan_matches_with_emvb() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    let search = "SELECT c1 FROM t1 SEARCH MATCH TENSOR \
                  (c2, [0.4,0.5,0.6,0.7], 'float', 'maxsim', '');";
    for use_index in [false, true] {
        e.execute_blocking("CREATE TABLE t1(c1 INT, c2 TENSOR(FLOAT,4));")
            .unwrap();
        if use_index {
            e.execute_blocking(
                "CREATE INDEX idx1 ON t1 (c2) USING EMVB \
                 WITH (pq_subspace_num=4, pq_subspace_bits=8);",
            )
            .unwrap();
        }
        // The second row concatenates two 4-wide vectors and wins on MaxSim.
        e.execute_blocking(
            "INSERT INTO t1 VALUES (1, [0.0,1.0,2.0,3.0]), \
             (2, [0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0]);",
        )
        .unwrap();
        assert_first(&e, search, "2");
        e.execute_blocking("DROP TABLE t1;").unwrap();
    }
}

/// C++: `src/unit_test/storage/new_request/index_request_ut.cpp:276`
/// (`test_optimize_index`): an LVQ-encoded HNSW index survives
/// `ALTER INDEX … WITH (lvq_avg)` and still answers the top-1 query.
#[test]
fn optimize_index_preserves_vector_search() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 EMBEDDING(FLOAT,4));")
        .unwrap();
    e.execute_blocking(
        "CREATE INDEX idx1 ON t1 (c2) USING HNSW \
         WITH (M=16, ef_construction=200, metric=l2, encode=lvq);",
    )
    .unwrap();
    e.execute_blocking("INSERT INTO t1 VALUES (1, [0.1,0.1,0.1,0.1]), (2, [0.2,0.2,0.2,0.2]);")
        .unwrap();
    e.execute_blocking("ALTER idx1 ON t1 WITH (lvq_avg);")
        .unwrap();

    assert_first(
        &e,
        "SELECT c1 FROM t1 SEARCH MATCH VECTOR \
         (c2, [0.3,0.3,0.2,0.2], 'float', 'l2', 1);",
        "2",
    );
}

/// C++: `src/unit_test/storage/new_request/data_request_ut.cpp:52`
/// (`test_append`): an append succeeds, while appending to a missing table is
/// rejected.
#[test]
fn append_into_missing_table_fails() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc');")
        .unwrap();
    assert!(e
        .execute_blocking("INSERT INTO t2 VALUES (1, 'abc');")
        .is_err());
}

/// C++: `src/unit_test/storage/new_request/data_request_ut.cpp:72`
/// (`test_select`): `SELECT *` returns the appended row verbatim.
#[test]
fn select_returns_inserted_row() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc');")
        .unwrap();
    assert_eq!(
        rows(&e, "SELECT * FROM t1;"),
        vec![vec!["1".to_string(), "abc".to_string()]]
    );
}

/// C++: `src/unit_test/storage/new_request/data_request_ut.cpp:104`
/// (`test_delete`): `DELETE` removes the matching row and leaves the other.
#[test]
fn delete_removes_matching_row() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE TABLE t1(c1 INT, c2 VARCHAR);")
        .unwrap();
    e.execute_blocking("INSERT INTO t1 VALUES (1, 'abc'), (2, 'def');")
        .unwrap();
    e.execute_blocking("DELETE FROM t1 WHERE c1 = 1;").unwrap();
    assert_eq!(
        rows(&e, "SELECT * FROM t1;"),
        vec![vec!["2".to_string(), "def".to_string()]]
    );
}

/// C++: `main/table_ut.cpp` `InfinityTableTest.test1` — the `Infinity` facade
/// path (create database → get database → create a table with an embedding
/// column → insert → query).
#[test]
fn infinity_facade_create_database_table_insert_select() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();

    e.execute_blocking("CREATE DATABASE db1;").unwrap();
    assert!(e.execute_blocking("CREATE DATABASE db1;").is_err());
    e.execute_blocking("CREATE TABLE db1.tb1 (col1 EMBEDDING(FLOAT,128));")
        .unwrap();
    assert!(e
        .execute_blocking("CREATE TABLE db1.tb1 (col1 INT);")
        .is_err());

    let vector = format!("[{}]", vec!["0.5"; 128].join(","));
    e.execute_blocking(&format!("INSERT INTO db1.tb1 VALUES ({vector});"))
        .unwrap();
    assert_eq!(count(&e, "db1.tb1"), 1);

    let result = rows(&e, "SELECT col1 FROM db1.tb1;");
    assert_eq!(result.len(), 1);
    assert!(result[0][0].contains("0.5"), "{:?}", result[0]);

    e.execute_blocking("DROP TABLE db1.tb1;").unwrap();
    assert_eq!(count(&e, "db1.tb1"), 0);
}
