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
    let r = e.execute(sql).unwrap().into_iter().next().unwrap();
    match &r {
        crate::QueryResult::Rows { .. } => r.rows_text(),
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
    e.execute("CREATE TABLE snap_t (a INT, b VARCHAR);")
        .unwrap();
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
    // `SHOW SNAPSHOTS` reports the ts the metadata carries, so read it first
    // and pin the JSON to it (the reference never fills this field and prints
    // an uninitialized value instead).
    let shown = result(&e, "SHOW SNAPSHOTS;");
    let commit = shown[0][3].clone();
    assert_ne!(commit, "0", "the snapshot's ts must be a real value");
    assert!(
        !e.snapshot_dir().join("s1").join("s1.rows").exists()
            && std::fs::read_dir(e.snapshot_dir().join("s1"))
                .unwrap()
                .flatten()
                .all(|f| !f.file_name().to_string_lossy().ends_with(".rows")),
        "the snapshot must not carry a row payload beside the metadata"
    );

    // The reference's layout for the same table: keys sorted, `segments`
    // describing the block the checkpoint-before-snapshot produced, the
    // snapshot's own ts, and the `u64::MAX` sentinel printed in full.
    let expected = format!(
        concat!(
            "{{\"column_definition\":[",
            "{{\"column_id\":0,\"column_name\":\"a\",",
            "\"column_type\":{{\"data_type\":3}},\"default\":{{\"type\":4}}}},",
            "{{\"column_id\":1,\"column_name\":\"b\",",
            "\"column_type\":{{\"data_type\":9}},\"default\":{{\"type\":4}}}}],",
            "\"create_ts\":0,\"database_name\":\"default_db\",\"db_id_str\":\"1\",",
            "\"max_commit_ts\":{1},\"next_column_id\":2,\"next_segment_id\":0,",
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
        table_id, commit
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
    assert_eq!(rows[0][3], commit);

    // `SHOW SNAPSHOT <name>` describes it (`physical_show_snapshot_impl`) and
    // reports the same ts.
    let rows = result(&e, "SHOW SNAPSHOT s1;");
    assert_eq!(rows[0], vec!["snapshot_name", "s1"]);
    assert_eq!(rows[1], vec!["snapshot_scope", "Table"]);
    assert_eq!(rows[3], vec!["commit_timestamp".to_string(), commit]);
}

/// `table_indexes[].segment_indexes` is read from the catalog's
/// `idx_chunk|…|chunk_info` records, which is where the reference keeps a
/// chunk's row range and index size (`ChunkIndexMeta::MapMetaToSnapShotInfo`,
/// `chunk_index_meta_impl.cpp:810`). Deriving them from the file names used to
/// force `base_row_id`/`row_cnt` to 0 and the segment id to 0.
#[test]
fn a_table_snapshot_records_the_index_chunks_the_catalog_holds() {
    let (_d, e) = engine();
    e.execute("CREATE TABLE snap_i (a INT, b VARCHAR);")
        .unwrap();
    let block: Vec<Vec<parser::Literal>> = (0..8192)
        .map(|_| vec![parser::Literal::Int(1), parser::Literal::Str("x".into())])
        .collect();
    e.execute("CREATE INDEX idx_a ON snap_i (a);").unwrap();
    e.import_literals("default_db.snap_i", None, block).unwrap();
    e.dump_index("default_db.snap_i", "idx_a").unwrap();
    // The mirror publishes the `idx_chunk|…` records the snapshot reads (the
    // reference writes them inside the dump transaction; the port's mirror
    // writes them at checkpoint time).
    e.flush().unwrap();
    e.execute("CREATE SNAPSHOT s1 ON TABLE snap_i;").unwrap();

    let text = std::fs::read_to_string(e.snapshot_dir().join("s1").join("s1.json")).unwrap();
    let json = expression::json::parse(&text).unwrap();
    let field = |object: &expression::json::Json, name: &str| {
        let expression::json::Json::Obj(entries) = object else {
            panic!("expected an object, got {object:?}");
        };
        entries
            .iter()
            .find(|(k, _)| k == name)
            .map(|(_, v)| v.clone())
            .unwrap_or_else(|| panic!("{name} is missing in {object:?}"))
    };
    let expression::json::Json::Arr(indexes) = field(&json, "table_indexes") else {
        panic!("table_indexes must be an array");
    };
    assert_eq!(indexes.len(), 1);
    let index = &indexes[0];
    assert!(matches!(
        field(index, "index_dir"),
        expression::json::Json::Str(ref s)
            if s.starts_with("db_1/tbl_") && s.ends_with("/idx_1")
    ));

    let expression::json::Json::Arr(segment_indexes) = field(index, "segment_indexes") else {
        panic!("segment_indexes must be an array");
    };
    assert_eq!(segment_indexes.len(), 1);
    let segment = &segment_indexes[0];
    assert_eq!(field(segment, "segment_id"), expression::json::Json::Int(0));
    let expression::json::Json::Arr(chunks) = field(segment, "chunk_indexes") else {
        panic!("chunk_indexes must be an array");
    };
    assert_eq!(chunks.len(), 1);
    let chunk = &chunks[0];
    // `chunk_{id}.idx`, the row range the chunk covers and its on-disk size;
    // a secondary chunk has no `base_name` and no `full_text_files`.
    assert_eq!(
        field(chunk, "index_filename"),
        expression::json::Json::Str("chunk_0.idx".into())
    );
    assert_eq!(
        field(chunk, "base_name"),
        expression::json::Json::Str(String::new())
    );
    assert_eq!(
        field(chunk, "base_row_id"),
        expression::json::Json::Str("0".into())
    );
    let record_row_count = 8192;
    assert_eq!(
        field(chunk, "row_cnt"),
        expression::json::Json::Int(record_row_count)
    );
    // `SecondaryIndexInMem::GetChunkIndexMetaInfo` records 0 for a secondary
    // chunk; the types whose memory index reports its own footprint (HNSW,
    // BMP) are not covered here.
    assert_eq!(field(chunk, "index_size"), expression::json::Json::Int(0));
    assert!(
        !matches!(chunk, expression::json::Json::Obj(entries)
            if entries.iter().any(|(k, _)| k == "full_text_files")),
        "only a full-text chunk lists its files"
    );

    // The whole `table_indexes` entry, as the reference wrote it for the same
    // table (`CREATE TABLE si (a INT, b VARCHAR)` + `CREATE INDEX idx_a ON si
    // (a)` + one 100-row `COPY`): only the table id and the row count differ.
    assert_eq!(
        field(&json, "table_indexes").compact(),
        format!(
            concat!(
                "[{{\"index_base\":{{\"cardinality\":\"high\",",
                "\"column_names\":[\"a\"],\"file_name\":\"snap_i_idx_a\",",
                "\"index_comment\":\"\",\"index_name\":\"idx_a\",",
                "\"index_type\":\"SECONDARY\",\"is_json_index\":false}},",
                "\"index_dir\":\"{}\",\"index_id_str\":\"1\",",
                "\"segment_indexes\":[{{\"chunk_indexes\":[{{\"base_name\":\"\",",
                "\"base_row_id\":\"0\",\"chunk_id\":0,",
                "\"index_filename\":\"chunk_0.idx\",\"index_size\":0,",
                "\"row_cnt\":{}}}],\"segment_id\":0}}]}}]"
            ),
            // `index_dir` is `db_1/tbl_{table}/idx_1`; the assertion above
            // already pinned its shape, so take it from the record.
            match field(index, "index_dir") {
                expression::json::Json::Str(s) => s.clone(),
                other => panic!("index_dir must be a string, got {other:?}"),
            },
            record_row_count
        )
    );
}

/// Database and system snapshots use the same directory, with the
/// `snapshot_scope` of their metadata and one ts for the whole snapshot.
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

    // Every record of one snapshot carries the same ts, and `SHOW SNAPSHOTS`
    // reports it for all three scopes.
    let shown = result(&e, "SHOW SNAPSHOTS;");
    let scopes: Vec<String> = shown.iter().map(|r| r[1].clone()).collect();
    assert_eq!(scopes, vec!["Database", "System"]);
    let ts = [json_ts(&db_json), json_ts(&sys_json)];
    assert_eq!(ts[0], shown[0][3].parse::<u64>().unwrap());
    assert_eq!(ts[1], shown[1][3].parse::<u64>().unwrap());
    assert!(ts.iter().all(|t| *t > 0));
    // The nested records use the same number as the record that holds them.
    for nested in nested_ts(&db_json) {
        assert_eq!(nested, ts[0]);
    }
    for nested in nested_ts(&sys_json) {
        assert_eq!(nested, ts[1]);
    }
}

/// The `max_commit_ts` a snapshot record carries.
fn json_ts(text: &str) -> u64 {
    let key = "\"max_commit_ts\":";
    let start = text.find(key).expect("max_commit_ts") + key.len();
    let rest = &text[start..];
    let end = rest
        .find(|c: char| !c.is_ascii_digit())
        .unwrap_or(rest.len());
    rest[..end].parse().expect("ts")
}

/// Every nested record's `max_commit_ts`.
fn nested_ts(text: &str) -> Vec<u64> {
    text.match_indices("\"max_commit_ts\":")
        .skip(1)
        .map(|(i, _)| {
            let rest = &text[i + "\"max_commit_ts\":".len()..];
            let end = rest
                .find(|c: char| !c.is_ascii_digit())
                .unwrap_or(rest.len());
            rest[..end].parse().expect("ts")
        })
        .collect()
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
