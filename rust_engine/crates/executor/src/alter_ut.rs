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
