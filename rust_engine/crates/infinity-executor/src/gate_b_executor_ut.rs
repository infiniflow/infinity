//! Gate-B planner/executor rows ported from the C++ unit suite.
//!
//! - `src/unit_test/planner/explain_basic_ut.cpp`
//! - `src/unit_test/planner/node/logical_create_collection_ut.cpp`
//! - `src/unit_test/planner/node/logical_drop_collection_ut.cpp`
//! - `src/unit_test/executor/fragment/plan_fragment_ut.cpp`
//! - `src/unit_test/executor/pipeline/fragment_ut.cpp`

use crate::{Engine, QueryResult};
use tempfile::tempdir;

fn open() -> (tempfile::TempDir, Engine) {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    (dir, engine)
}

/// C++ `ExplainBasicTest.test1` (`explain_basic_ut.cpp`): every `EXPLAIN`
/// flavour succeeds for selects over a real table and for the DDL/DML/SHOW
/// statements in the reference list.
#[test]
fn explain_basic_test1() {
    let (_dir, engine) = open();
    engine.execute("create database db1").unwrap();
    engine.execute("create collection collection1").unwrap();
    engine
        .execute("create table tb(col1 int, col2 varchar)")
        .unwrap();
    for i in 0..200 {
        engine
            .execute(&format!("insert into tb values({i}, 'abc')"))
            .unwrap();
    }
    engine
        .execute("create table tb_embedding(col1 int, col2 embedding(float, 4))")
        .unwrap();
    engine
        .execute(
            "insert into tb_embedding values(1, [0.1, 0.1, 0.1, 0.1]), \
             (2, [0.2, 0.2, 0.2, 0.2])",
        )
        .unwrap();

    let explain = |engine: &Engine, sql: &str, read_only: bool| {
        let mut variants = vec![
            format!("explain ast {sql}"),
            format!("explain raw {sql}"),
            format!("explain logical {sql}"),
            format!("explain physical {sql}"),
            format!("explain fragment {sql}"),
            format!("explain {sql}"),
        ];
        if read_only {
            variants.push(format!("explain analyze {sql}"));
            variants.push(format!("explain pipeline {sql}"));
        }
        for variant in variants {
            let mut result = engine
                .execute(&variant)
                .unwrap_or_else(|err| panic!("{variant}: {err}"));
            assert_eq!(result.len(), 1, "{variant}");
            assert!(
                matches!(result.remove(0), QueryResult::Rows { .. }),
                "{variant} did not return rows"
            );
        }
    };

    // Selects.
    explain(&engine, "select * from tb where col1 > 100 and 1 = 1", true);
    explain(&engine, "select * from tb order by col1", true);
    explain(&engine, "select * from tb limit 100", true);
    explain(
        &engine,
        "select min(col1), max(col1), avg(col1) from tb where 1 = 1 group by col1",
        true,
    );
    explain(
        &engine,
        "select * from tb where col1 > 100 and 1 = 1 order by col1 limit 100",
        true,
    );
    explain(
        &engine,
        "select * from tb where col1 in (100, 150, 200)",
        true,
    );
    explain(
        &engine,
        "select * from tb where col1 between 100 and 200",
        true,
    );
    explain(
        &engine,
        "select col1 from tb_embedding search match vector (col2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 1)",
        true,
    );

    // Create.
    explain(
        &engine,
        "create index idx on tb(col1) using secondary",
        false,
    );
    explain(
        &engine,
        "create index idx on tb(col2) using fulltext",
        false,
    );
    explain(&engine, "create table tb1(c1 int, c2 varchar)", false);
    explain(&engine, "create database db1", false);
    explain(&engine, "create collection c1", false);

    // Insert / update / drop.
    explain(&engine, "insert into tb values(1000, 'xyz')", false);
    explain(&engine, "update tb set col1 = 999 where col1 > 100", false);
    explain(
        &engine,
        "update tb set col1 = 999 where col1 between 100 and 200",
        false,
    );
    explain(&engine, "drop table tb", false);
    explain(&engine, "drop database db1", false);
    explain(&engine, "drop collection collection1", false);

    // Show.
    for sql in [
        "show databases",
        "show tables",
        "show database default_db",
        "show table tb",
        "show table tb columns",
        "show table tb segments",
        "show table tb segment 0",
        "show table tb segment 0 blocks",
        "show table tb segment 0 block 0",
        "show table tb segment 0 block 0 column 0",
        "show table tb indexes",
        "show checkpoint",
        "show buffer",
        "show tasks",
        "show configs",
        "show config version",
        "show profiles",
        "show queries",
        "show transactions",
        "show transaction history",
        "show session variables",
        "show session variable query_count",
        "show snapshots",
    ] {
        explain(&engine, sql, true);
    }

    explain(&engine, "flush catalog", true);
}

/// C++ `LogicalCreateCollectionTest.test1`: create a table, index and rows,
/// then `CREATE COLLECTION` acknowledges with `OK` (the physical operator is a
/// writable-mode no-op).
#[test]
fn logical_create_collection_test1() {
    let (_dir, engine) = open();
    engine
        .execute("create table tb(col1 int, col2 varchar)")
        .unwrap();
    engine.execute("create index idx on tb(col1)").unwrap();
    for i in 0..200 {
        engine
            .execute(&format!("insert into tb values({i}, 'abc')"))
            .unwrap();
    }
    let result = engine.execute("create collection tb").unwrap();
    assert_eq!(result.len(), 1);
    let QueryResult::Rows { columns, rows } = &result[0] else {
        panic!("expected an OK row, got {:?}", result[0]);
    };
    assert_eq!(columns, &vec!["OK".to_string()]);
    assert!(rows.is_empty());
}

/// C++ `LogicalDropCollectionTest.test1`: `CREATE COLLECTION` followed by
/// `DROP COLLECTION` both acknowledge with `OK`.
#[test]
fn logical_drop_collection_test1() {
    let (_dir, engine) = open();
    let created = engine.execute("create collection tb").unwrap();
    assert!(matches!(created[0], QueryResult::Rows { .. }));
    let dropped = engine.execute("drop collection tb").unwrap();
    assert!(matches!(dropped[0], QueryResult::Rows { .. }));
}

/// C++ `PlanFragmentTest.test1` (`plan_fragment_ut.cpp`): a filtered select
/// yields a fragment plan. The reference only logs the text; the port asserts
/// the fragment plan exists and describes the table.
#[test]
fn plan_fragment_test1() {
    let (_dir, engine) = open();
    engine
        .execute("create table tb(col1 int, col2 varchar)")
        .unwrap();
    for i in 0..200 {
        engine
            .execute(&format!("insert into tb values({i}, 'abc')"))
            .unwrap();
    }
    let result = engine
        .execute("explain fragment select * from tb where col1 < 100")
        .unwrap();
    let QueryResult::Rows { rows, .. } = &result[0] else {
        panic!("expected fragment rows");
    };
    assert!(!rows.is_empty(), "fragment plan is empty");
    let text = rows
        .iter()
        .map(|row| row.join(" "))
        .collect::<Vec<_>>()
        .join("\n");
    assert!(
        text.to_ascii_lowercase().contains("tb"),
        "fragment plan does not mention the table: {text}"
    );
}

/// C++ `FragmentTest.test_build_fragment` (`fragment_ut.cpp`): DDL and SHOW
/// over a fresh engine; `SHOW TABLES` has eight columns and `SHOW TABLE t2`
/// has two.
#[test]
fn fragment_test_build_fragment() {
    let (_dir, engine) = open();
    for sql in [
        "create table t1(a bigint)",
        "create database db1",
        "create table db1.t1(a bigint)",
        "create table t2(a bigint)",
        "create table t3(c1 embedding(bit,10))",
    ] {
        let result = engine.execute(sql).unwrap_or_else(|e| panic!("{sql}: {e}"));
        assert_eq!(result.len(), 1, "{sql}");
    }
    // Drop database and table.
    assert!(matches!(
        engine.execute("drop database db1").unwrap()[0],
        QueryResult::DroppedDatabase { .. }
    ));
    assert!(matches!(
        engine.execute("drop table t1").unwrap()[0],
        QueryResult::DroppedTable { existed: true, .. }
    ));

    // Show.
    let show_tables = engine.execute("show tables").unwrap();
    let QueryResult::Rows { columns, .. } = &show_tables[0] else {
        panic!("expected SHOW TABLES rows");
    };
    assert_eq!(columns.len(), 8);

    let show_table = engine.execute("show table t2").unwrap();
    let QueryResult::Rows { columns, rows } = &show_table[0] else {
        panic!("expected SHOW TABLE rows");
    };
    assert_eq!(columns.len(), 2);
    assert_eq!(rows.len(), 6);
    assert_eq!(
        rows[0],
        vec!["database_name".to_string(), "default_db".to_string()]
    );
    assert_eq!(rows[1], vec!["table_name".to_string(), "t2".to_string()]);
    assert_eq!(rows[4], vec!["column_count".to_string(), "1".to_string()]);

    assert!(matches!(
        engine.execute("drop table t3").unwrap()[0],
        QueryResult::DroppedTable { existed: true, .. }
    ));
    assert!(matches!(
        engine.execute("drop table t2").unwrap()[0],
        QueryResult::DroppedTable { existed: true, .. }
    ));
}
