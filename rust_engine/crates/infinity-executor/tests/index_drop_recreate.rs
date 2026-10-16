//! `DROP INDEX` → recreate invariants (plan §6 Gate G / INVARIANT_MATRIX §3).
//!
//! Recreating an index after `DROP INDEX` must leave lookups correct (the
//! rebuilt index answers the same rows as before the drop).

use infinity_executor::{Engine, QueryResult};

fn ids(engine: &Engine, sql: &str) -> Vec<i64> {
    let rs = engine.execute(sql).expect("select");
    match &rs[0] {
        QueryResult::Rows { rows, .. } => rows.iter().map(|r| r[0].parse().unwrap()).collect(),
        other => panic!("expected rows, got {other:?}"),
    }
}

#[test]
fn lookups_stay_correct_across_drop_and_recreate() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (id INTEGER, name VARCHAR);")
        .unwrap();
    e.execute("CREATE INDEX idx ON t (name);").unwrap();
    e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'a');")
        .unwrap();

    let q = "SELECT id FROM t WHERE name = 'a' ORDER BY id;";
    assert_eq!(ids(&e, q), vec![1, 3], "before drop");

    e.execute("DROP INDEX idx;").unwrap();
    assert_eq!(ids(&e, q), vec![1, 3], "after drop (falls back to a scan)");

    // recreate: the rebuilt index must answer the same rows
    e.execute("CREATE INDEX idx ON t (name);").unwrap();
    assert_eq!(ids(&e, q), vec![1, 3], "after recreate");

    // and new rows are visible through the rebuilt index
    e.execute("INSERT INTO t VALUES (4, 'a');").unwrap();
    assert_eq!(
        ids(&e, q),
        vec![1, 3, 4],
        "after insert into the rebuilt index"
    );
}
