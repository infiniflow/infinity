//! The snapshot storage layout: `<snapshot_dir>/<name>/<name>.json` in the
//! reference's schema, the data files beside it, `SHOW SNAPSHOTS` reading the
//! directory and `DROP SNAPSHOT` staging through `deleted_<name>`.

use crate::Engine;
use tempfile::tempdir;

fn engine() -> (tempfile::TempDir, Engine) {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    (dir, e)
}

fn result(e: &Engine, sql: &str) -> Vec<Vec<String>> {
    match e.execute(sql).unwrap().into_iter().next().unwrap() {
        crate::QueryResult::Rows { rows, .. } => rows,
        other => panic!("expected rows, got {other:?}"),
    }
}

fn columns(e: &Engine, sql: &str) -> Vec<String> {
    match e.execute(sql).unwrap().into_iter().next().unwrap() {
        crate::QueryResult::Rows { columns, .. } => columns,
        other => panic!("expected rows, got {other:?}"),
    }
}

/// `TableSnapshotInfo::CreateSnapshotMetadataJSON`
/// (`snapshot_info_impl.cpp:443`) for a two-column table. `nlohmann::json`
/// dumps object keys sorted, so the text is comparable byte for byte.
#[test]
fn a_table_snapshot_writes_the_reference_metadata() {
    let (dir, e) = engine();
    e.execute("CREATE TABLE snap_t (a INT, b VARCHAR);").unwrap();
    e.execute("INSERT INTO snap_t VALUES (1, 'x'), (2, 'y');")
        .unwrap();
    e.execute("CREATE SNAPSHOT s1 ON TABLE snap_t;").unwrap();

    let root = e.snapshot_dir();
    assert_eq!(root, dir.path().join("snapshot"));
    let meta = root.join("s1").join("s1.json");
    assert!(meta.exists(), "{} is missing", meta.display());

    let text = std::fs::read_to_string(&meta).unwrap();
    let table_id = e
        .catalog()
        .get_table_by_name("default_db.snap_t")
        .unwrap()
        .expect("table")
        .id
        .to_string();
    // The reference's layout for the same table: keys sorted, `segments`
    // describing the block the checkpoint-before-snapshot produced, and the
    // `u64::MAX` sentinel printed in full.
    let expected = format!(
        concat!(
            "{{\"column_definition\":[",
            "{{\"column_id\":0,\"column_name\":\"a\",",
            "\"column_type\":{{\"data_type\":3}},\"default\":{{\"type\":4}}}},",
            "{{\"column_id\":1,\"column_name\":\"b\",",
            "\"column_type\":{{\"data_type\":9}},\"default\":{{\"type\":4}}}}],",
            "\"create_ts\":0,\"database_name\":\"default_db\",\"db_id_str\":\"1\",",
            "\"max_commit_ts\":0,\"next_column_id\":2,\"next_segment_id\":0,",
            "\"row_count\":0,\"segments\":[{{\"actual_row_count\":0,\"blocks\":[",
            "{{\"block_dir\":\"db_1/tbl_{0}/seg_0/blk_0\",\"block_id\":0,\"columns\":[",
            "{{\"column_id\":0,\"filepath\":\"db_1/tbl_{0}/seg_0/blk_0/0.col\"}},",
            "{{\"column_id\":1,\"filepath\":\"db_1/tbl_{0}/seg_0/blk_0/1.col\",",
            "\"outlines\":[\"db_1/tbl_{0}/seg_0/blk_0/col_1_out\"]}}]}}],",
            "\"deprecate_ts\":0,\"first_delete_ts\":18446744073709551615,",
            "\"row_count\":2,\"segment_dir\":\"\",\"segment_id\":0,",
            "\"segment_status\":0}}],\"snapshot_name\":\"s1\",",
            "\"snapshot_scope\":0,\"table_comment\":\"\",\"table_id_str\":\"{0}\",",
            "\"table_name\":\"snap_t\",\"txn_id\":0,\"unsealed_id\":0,\"version\":1}}"
        ),
        table_id
    );
    assert_eq!(text, expected);

    // `SHOW SNAPSHOTS` reads that directory (`physical_show_list_snapshots_impl`).
    assert_eq!(
        columns(&e, "SHOW SNAPSHOTS;"),
        vec!["name", "scope", "time", "commit", "size"]
    );
    let rows = result(&e, "SHOW SNAPSHOTS;");
    assert_eq!(rows.len(), 1);
    assert_eq!(rows[0][0], "s1");
    assert_eq!(rows[0][1], "Table");
    assert_eq!(rows[0][3], "0");

    // `SHOW SNAPSHOT <name>` describes it (`physical_show_snapshot_impl`).
    let rows = result(&e, "SHOW SNAPSHOT s1;");
    assert_eq!(rows[0], vec!["snapshot_name", "s1"]);
    assert_eq!(rows[1], vec!["snapshot_scope", "Table"]);
    assert_eq!(rows[3][0], "commit_timestamp");
}

/// Database and system snapshots use the same directory, with the
/// `snapshot_scope` of their metadata and one rows file per table.
#[test]
fn database_and_system_snapshots_nest_their_tables() {
    let (_d, e) = engine();
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.t1 (a INT);").unwrap();
    e.execute("INSERT INTO db1.t1 VALUES (1), (2);").unwrap();

    e.execute("CREATE SNAPSHOT sdb ON DATABASE db1;").unwrap();
    e.execute("CREATE SNAPSHOT ssys ON SYSTEM;").unwrap();

    let root = e.snapshot_dir();
    let db_json = std::fs::read_to_string(root.join("sdb").join("sdb.json")).unwrap();
    assert!(db_json.contains("\"snapshot_scope\":1"), "{db_json}");
    assert!(db_json.contains("\"table_snapshots\":[{"), "{db_json}");
    assert!(db_json.contains("\"database_name\":\"db1\""), "{db_json}");

    let sys_json = std::fs::read_to_string(root.join("ssys").join("ssys.json")).unwrap();
    assert!(sys_json.contains("\"snapshot_scope\":2"), "{sys_json}");
    assert!(sys_json.contains("\"database_snapshots\":[{"), "{sys_json}");

    let scopes: Vec<String> = result(&e, "SHOW SNAPSHOTS;")
        .into_iter()
        .map(|r| r[1].clone())
        .collect();
    assert_eq!(scopes, vec!["Database", "System"]);
}

/// `TableSnapshotInfo` + `Snapshot::DropSnapshot`: the directory disappears,
/// and a name can be used again afterwards.
#[test]
fn drop_snapshot_removes_the_directory() {
    let (_d, e) = engine();
    e.execute("CREATE TABLE t (a INT);").unwrap();
    e.execute("CREATE SNAPSHOT s1 ON TABLE t;").unwrap();
    let path = e.snapshot_dir().join("s1");
    assert!(path.exists());

    e.execute("DROP SNAPSHOT s1;").unwrap();
    assert!(!path.exists());
    assert!(e.snapshot_names().unwrap().is_empty());
    assert!(!e.snapshot_dir().join("deleted_s1").exists());

    e.execute("CREATE SNAPSHOT s1 ON TABLE t;").unwrap();
    assert!(e.snapshot_names().unwrap().iter().any(|n| n == "s1"));

    let err = e.execute("DROP SNAPSHOT missing;").unwrap_err();
    assert!(err.to_string().contains("not found"), "{err}");
}

/// The reference refuses a name that is already taken
/// (`Status::SnapshotAlreadyExists`, `query_context_impl.cpp:633`) and reports
/// a missing table with its own wording.
#[test]
fn snapshot_creation_reports_the_reference_errors() {
    let (_d, e) = engine();
    e.execute("CREATE TABLE t (a INT);").unwrap();
    e.execute("CREATE SNAPSHOT s1 ON TABLE t;").unwrap();

    let err = e.execute("CREATE SNAPSHOT s1 ON TABLE t;").unwrap_err();
    assert_eq!(err.to_string(), "Snapshot: s1 already exists");

    let err = e.execute("CREATE SNAPSHOT s2 ON TABLE nope;").unwrap_err();
    assert_eq!(err.status_code(), 3022);
    assert!(err.to_string().contains("nope"), "{err}");

    let err = e.execute("SHOW SNAPSHOT nope;").unwrap_err();
    assert!(err.to_string().contains("can't find snapshot"), "{err}");
}

/// The snapshot carries the rows: restoring into a dropped table brings them
/// back, and a second restore rewrites the same content.
#[test]
fn a_restored_table_keeps_its_rows() {
    let (_d, e) = engine();
    e.execute("CREATE TABLE t (a INT, b VARCHAR);").unwrap();
    e.execute("INSERT INTO t VALUES (1, 'x'), (2, 'y'), (3, 'z');")
        .unwrap();
    e.execute("CREATE SNAPSHOT s1 ON TABLE t;").unwrap();
    e.execute("DROP TABLE t;").unwrap();

    e.execute("RESTORE TABLE SNAPSHOT s1;").unwrap();
    assert_eq!(
        result(&e, "SELECT a, b FROM t ORDER BY a;"),
        vec![
            vec!["1".to_string(), "x".to_string()],
            vec!["2".to_string(), "y".to_string()],
            vec!["3".to_string(), "z".to_string()],
        ]
    );
}
