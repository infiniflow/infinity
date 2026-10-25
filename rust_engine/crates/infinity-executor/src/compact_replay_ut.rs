//! Ports of `src/unit_test/storage/new_catalog/compact_ut.cpp`,
//! `compact_internal_ut.cpp` and `replay_compact_ut.cpp`
//! (`TestTxnCompact`, `TestTxnCompactInternal`, `TestTxnReplayCompact`).
//!
//! `NewTxn::Compact` becomes [`Engine::compact_all_segments`] (the primitive;
//! `COMPACT` goes through `Engine::compact_table`, which first asks whether any
//! segment pair is worth merging). `RestartTxnMgr`
//! becomes [`crate::new_catalog_ut::reopen`]. The reference's segment ids
//! (`{2}` after a merge) are an allocator artefact: the Rust engine renumbers
//! live rows into segment 0, so the ports assert the same row/block totals and
//! boundary values on the compacted table and note the id mapping.

use crate::new_catalog_ut as h;
use crate::Engine;
use infinity_parser::Literal;
use tempfile::tempdir;

/// Names of the `seg_*` directories under `root`, at any depth.
fn walkdir(root: &std::path::Path) -> Vec<String> {
    let mut out = Vec::new();
    let Ok(entries) = std::fs::read_dir(root) else {
        return out;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            let name = entry.file_name().to_string_lossy().to_string();
            out.push(name);
            out.extend(walkdir(&path));
        }
    }
    out
}

fn create_table(e: &Engine) {
    h::ok(e, "CREATE DATABASE db1;");
    h::ok(e, "CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);");
}

/// `MakeInputBlock(v, text, 8192)`: one homogeneous 8192-row block.
fn const_block(value: i64, text: &str) -> Vec<Vec<Literal>> {
    (0..8192)
        .map(|_| vec![Literal::Int(value), Literal::Str(text.to_string())])
        .collect()
}

/// `MakeInputBlock` alternating `(1, "abc")` / `(2, "abcdefghijklmnopqrstuvwxyz")`.
fn alternating_block() -> Vec<Vec<Literal>> {
    (0..8192)
        .map(|i| {
            if i % 2 == 0 {
                vec![Literal::Int(1), Literal::Str("abc".to_string())]
            } else {
                vec![
                    Literal::Int(2),
                    Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
                ]
            }
        })
        .collect()
}

/// One `NewTxn::Import` call: two blocks (16384 rows).
fn import_two_blocks(e: &Engine) -> usize {
    h::append(e, "db1.tb1", const_block(1, "abc"))
        + h::append(e, "db1.tb1", const_block(2, "abcdefghijklmnopqrstuvwxyz"))
}

/// Two imports: four 8192-row blocks.
fn import_four_blocks(e: &Engine) {
    for _ in 0..2 {
        assert_eq!(import_two_blocks(e), 16384);
    }
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
}

fn assert_compacted(e: &Engine, segment: u32) {
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(h::count(e, "db1.tb1"), 32768);
    assert_eq!(e.segment_ids("db1.tb1"), vec![segment]);
    e.flush().unwrap();
    assert_eq!(e.block_ids("db1.tb1", segment), vec![0, 1, 2, 3]);
    for block in 0..4u16 {
        assert_eq!(e.block_row_count("db1.tb1", segment, block), Some(8192));
        assert_eq!(
            e.block_visible_range("db1.tb1", segment, block),
            Some((0, 8192))
        );
    }
    // Count through the unindexed column: the reference reads the merged
    // segment's row count, not a duplicate-key index lookup.
    assert_eq!(
        h::rows(e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abc';"),
        vec![vec!["16384"]]
    );
    assert_eq!(
        h::rows(
            e,
            "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"
        ),
        vec![vec!["16384"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:384`
/// (`TestTxnCompact.compact_with_index_commit`): a committed merge keeps every
/// row and the secondary index answers the same queries.
#[test]
fn compact_with_index_commit() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    import_four_blocks(&e);

    e.compact_all_segments("db1.tb1").unwrap();
    assert_compacted(&e, 1);
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:428`
/// (`TestTxnCompact.compact_with_index_rollback`): a rolled-back merge leaves
/// the pre-merge rows and index intact.
#[test]
fn compact_with_index_rollback() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    import_four_blocks(&e);

    // `RollBackTxn` discards the merge: no compact_table call.
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abc';"),
        vec![vec!["16384"]]
    );
    assert_eq!(h::segment_rows(&e, "db1.tb1"), 32768);
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:467`
/// (`TestTxnCompact.compact_and_drop_db`): a compacted database drops cleanly.
#[test]
fn compact_then_drop_database() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);
    e.compact_all_segments("db1.tb1").unwrap();

    h::ok(&e, "DROP DATABASE db1;");
    assert!(e.execute("SELECT * FROM db1.tb1;").is_err());
    assert!(e.execute("INSERT INTO db1.tb1 VALUES (1, 'x');").is_err());
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:602`
/// (`TestTxnCompact.compact_and_drop_table`).
#[test]
fn compact_then_drop_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);
    e.compact_all_segments("db1.tb1").unwrap();

    h::ok(&e, "DROP TABLE db1.tb1;");
    assert!(!e.table_exists("db1.tb1"));
    assert!(e.execute("SELECT * FROM db1.tb1;").is_err());
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:749`
/// (`TestTxnCompact.compact_and_add_columns`): the compacted rows read the new
/// column's default.
#[test]
fn compact_then_add_columns() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);
    e.compact_all_segments("db1.tb1").unwrap();

    h::ok(&e, "ALTER TABLE db1.tb1 ADD COLUMN (col3 INT DEFAULT 3);");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col3 = 3;"),
        vec![vec!["32768"]]
    );
    // The reference checks each of the four blocks after the column add; the
    // default is visible for every compacted block.
    for col1 in [1, 2] {
        assert_eq!(
            h::rows(
                &e,
                &format!("SELECT count(*) FROM db1.tb1 WHERE col1 = {col1} AND col3 = 3;")
            ),
            vec![vec!["16384"]]
        );
    }
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:1024`
/// (`TestTxnCompact.compact_and_drop_columns`): the surviving column keeps the
/// compacted values.
#[test]
fn compact_then_drop_columns() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);
    e.compact_all_segments("db1.tb1").unwrap();

    h::ok(&e, "ALTER TABLE db1.tb1 DROP COLUMN (col2);");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col1 = 2;"),
        vec![vec!["16384"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:1229`
/// (`TestTxnCompact.compact_and_rename_table`).
#[test]
fn compact_then_rename_table() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);
    e.compact_all_segments("db1.tb1").unwrap();

    h::ok(&e, "ALTER TABLE db1.tb1 RENAME TO db1.tb2;");
    assert_eq!(e.row_count("db1.tb2").unwrap(), 32768);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb2 WHERE col1 = 1;"),
        vec![vec!["16384"]]
    );
}

/// `NewCompactionAlg` (DBT): a compaction needs four segments in one layer.
///
/// `DBTCompactionAlg::GetCompactableSegments` returns nothing until a layer
/// holds `DBT_COMPACTION_M` (4) segments, so `COMPACT` on a two- or
/// three-segment table opens no transaction - the reference's own
/// `test_a_compaction_with_one_segment_is_a_no_op` covers the one-segment case
/// end to end. Four 8192-row segments all land in layer 0 (`row_cnt / 8192 ==
/// 1`), so the fourth import makes them compactable.
#[test]
fn compaction_needs_four_segments_in_one_layer() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    for _ in 0..3 {
        e.import_literals("db1.tb1", None, const_block(1, "abc"))
            .unwrap();
    }
    assert_eq!(e.segment_ids("db1.tb1"), vec![0, 1, 2]);
    e.compact_table("db1.tb1").unwrap();
    assert_eq!(
        e.segment_ids("db1.tb1"),
        vec![0, 1, 2],
        "three segments are not a compaction candidate"
    );

    e.import_literals(
        "db1.tb1",
        None,
        const_block(2, "abcdefghijklmnopqrstuvwxyz"),
    )
    .unwrap();
    assert_eq!(e.segment_ids("db1.tb1"), vec![0, 1, 2, 3]);
    // The mirror is what a compaction leaves behind on disk, so write it first.
    e.flush().unwrap();
    e.compact_table("db1.tb1").unwrap();
    assert_eq!(e.segment_ids("db1.tb1"), vec![4]);
    assert_eq!(h::count(&e, "db1.tb1"), 4 * 8192);
    // The source segments stay on disk, deprecated: a reader at a later ts
    // ignores them, and `Cleanup` is what reclaims them.
    e.flush().unwrap();
    let mut dirs: Vec<String> = Vec::new();
    for entry in walkdir(&dir.path().join("data")) {
        if entry.starts_with("seg_") {
            dirs.push(entry);
        }
    }
    dirs.sort();
    assert_eq!(
        dirs,
        vec![
            "seg_0".to_string(),
            "seg_1".to_string(),
            "seg_2".to_string(),
            "seg_3".to_string(),
            "seg_4".to_string()
        ]
    );
}

/// `DBTCompactionAlg` merges the **first four** segments of the lowest layer
/// that holds `DBT_COMPACTION_M` (4) of them, not every live segment: with six
/// segments the remaining two stay where they are, and the merged segment takes
/// the id after *every* segment (6), not after the four that were merged.
#[test]
fn a_compaction_of_more_than_four_segments_keeps_the_rest() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    for value in 0..6 {
        e.import_literals("db1.tb1", None, const_block(value, "abc"))
            .unwrap();
    }
    assert_eq!(e.segment_ids("db1.tb1"), vec![0, 1, 2, 3, 4, 5]);
    // The mirror is what a compaction leaves behind on disk, so write it first.
    e.flush().unwrap();

    e.compact_table("db1.tb1").unwrap();
    assert_eq!(
        e.segment_ids("db1.tb1"),
        vec![4, 5, 6],
        "the four candidates merge into 6, the other two keep their ids"
    );
    assert_eq!(h::count(&e, "db1.tb1"), 6 * 8192);
    // Every imported value survives exactly once, in whichever segment holds
    // it now: the merged four moved, the untouched two did not.
    for value in 0..6 {
        assert_eq!(
            h::rows(
                &e,
                &format!("SELECT count(*) FROM db1.tb1 WHERE col1 = {value};")
            ),
            vec![vec!["8192".to_string()]],
            "value {value}"
        );
    }
    // The layout the reference leaves for the same six imports (measured: the
    // merged segment holds the four source blocks, the two it did not pick keep
    // their single block each, and the four deprecated segments keep their
    // files until `Cleanup`).
    e.flush().unwrap();
    let table_dir = {
        let mut found = None;
        for db in std::fs::read_dir(dir.path().join("data"))
            .unwrap()
            .flatten()
        {
            for tbl in std::fs::read_dir(db.path()).unwrap().flatten() {
                found = Some(tbl.path());
            }
        }
        found.expect("the table directory")
    };
    let blocks = |segment: u32| -> usize {
        let dir = table_dir.join(format!("seg_{segment}"));
        let mut count = 0;
        if let Ok(entries) = std::fs::read_dir(&dir) {
            for entry in entries.flatten() {
                if entry.file_name().to_string_lossy().starts_with("blk_") {
                    count += 1;
                }
            }
        }
        count
    };
    assert_eq!(blocks(0), 1, "a deprecated segment keeps its block");
    assert_eq!(blocks(3), 1);
    assert_eq!(blocks(4), 1, "an untouched segment keeps its block");
    assert_eq!(blocks(5), 1);
    assert_eq!(blocks(6), 4, "the merged segment holds the four sources");

    // Three segments left, so a second COMPACT picks nothing.
    e.compact_table("db1.tb1").unwrap();
    assert_eq!(e.segment_ids("db1.tb1"), vec![4, 5, 6]);
}

/// The segment an `INSERT` is still appending to is **unsealed**, and
/// `CompactionProcessor` removes it from the candidate list before asking
/// `NewCompactionAlg` (`compaction_process_impl.cpp:176`), so three imports plus
/// one insert are three candidates, not four.
#[test]
fn an_unsealed_segment_is_not_a_compaction_candidate() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    for _ in 0..3 {
        e.import_literals("db1.tb1", None, const_block(1, "abc"))
            .unwrap();
    }
    // An `INSERT` after a `COPY` opens a new segment and leaves it unsealed.
    e.insert_literals("db1.tb1", None, const_block(2, "abc"))
        .unwrap();
    assert_eq!(e.segment_ids("db1.tb1"), vec![0, 1, 2, 3]);
    e.compact_table("db1.tb1").unwrap();
    assert_eq!(
        e.segment_ids("db1.tb1"),
        vec![0, 1, 2, 3],
        "the open segment does not count towards the four"
    );
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:1448`
/// (`TestTxnCompact.compact_and_compact`): a second merge is a no-op.
#[test]
fn compact_and_compact() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);

    e.compact_all_segments("db1.tb1").unwrap();
    assert_compacted(&e, 1);
    e.compact_all_segments("db1.tb1").unwrap();
    assert_compacted(&e, 2);
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:1570`
/// (`TestTxnCompact.compact_and_create_index`): an index created after the
/// merge covers the compacted rows.
#[test]
fn compact_then_create_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);
    e.compact_all_segments("db1.tb1").unwrap();

    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    assert_eq!(e.segment_ids("db1.tb1"), vec![1]);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abc';"),
        vec![vec!["16384"]]
    );
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/compact_ut.cpp:1784`
/// (`TestTxnCompact.compact_and_drop_index`).
#[test]
fn compact_then_drop_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    import_four_blocks(&e);
    e.compact_all_segments("db1.tb1").unwrap();

    h::ok(&e, "DROP INDEX index1 ON db1.tb1;");
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abc';"),
        vec![vec!["16384"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/compact_internal_ut.cpp:81`
/// (`TestTxnCompactInternal.test_compact`): deleting every second offset of
/// the first block and merging the two source segments leaves four blocks with
/// the reference's surviving values.
#[test]
fn internal_compact_skips_deleted_offsets() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    for _ in 0..2 {
        assert_eq!(h::append(&e, "db1.tb1", alternating_block()), 8192);
        assert_eq!(h::append(&e, "db1.tb1", alternating_block()), 8192);
    }
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);

    // `txn->Delete(RowID(0, {1,3,5,…8191}))`: the odd offsets of block 0.
    let ids: Vec<(u32, u32)> = (1..8192).step_by(2).map(|i| (0, i as u32)).collect();
    let mut t = e.begin_txn();
    assert_eq!(t.delete_row_ids("db1.tb1", &ids).unwrap(), 4096);
    t.commit().unwrap();

    e.compact_all_segments("db1.tb1").unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768 - 4096);
    assert_eq!(e.segment_ids("db1.tb1"), vec![1]);
    e.flush().unwrap();
    assert_eq!(e.block_ids("db1.tb1", 1), vec![0, 1, 2, 3]);
    // The reference keeps the block's stored row count (8192) and hides the
    // tombstones from the visible range; the engine reports the same stored
    // count, and `row_count` below accounts for the deleted offsets.
    for block in 0..3u16 {
        assert_eq!(e.block_row_count("db1.tb1", 1, block), Some(8192));
    }
    assert_eq!(e.block_row_count("db1.tb1", 1, 3), Some(4096));
    // Odd offsets in the first block were `col1 = 2`; the other three blocks
    // still alternate 4096/4096.
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abc';"),
        vec![vec!["16384"]]
    );
    assert_eq!(
        h::rows(
            &e,
            "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"
        ),
        vec![vec!["12288"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/compact_internal_ut.cpp:265`
/// (`TestTxnCompactInternal.test_compact_with_index`): the merge rebuilds the
/// secondary index over the surviving rows.
#[test]
fn internal_compact_with_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    for _ in 0..2 {
        h::append(&e, "db1.tb1", alternating_block());
        h::append(&e, "db1.tb1", alternating_block());
    }
    let ids: Vec<(u32, u32)> = (1..8192).step_by(2).map(|i| (0, i as u32)).collect();
    let mut t = e.begin_txn();
    assert_eq!(t.delete_row_ids("db1.tb1", &ids).unwrap(), 4096);
    t.commit().unwrap();

    e.compact_all_segments("db1.tb1").unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 28672);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abc';"),
        vec![vec!["16384"]]
    );
    assert_eq!(
        h::rows(
            &e,
            "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abcdefghijklmnopqrstuvwxyz';"
        ),
        vec![vec!["12288"]]
    );
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}

/// C++ `src/unit_test/storage/new_catalog/replay_compact_ut.cpp:283`
/// (`TestTxnReplayCompact.test_compact_commit`): a committed merge survives a
/// restart.
#[test]
fn replay_compact_commit() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);
    e.compact_all_segments("db1.tb1").unwrap();
    let e = h::reopen(e, dir.path());
    assert_compacted(&e, 1);

    h::ok(&e, "DROP TABLE db1.tb1;");
    assert!(!e.table_exists("db1.tb1"));
}

/// C++ `src/unit_test/storage/new_catalog/replay_compact_ut.cpp:325`
/// (`TestTxnReplayCompact.test_compact_rollback`): the failed merge (the
/// concurrent DROP COLUMN commits first) leaves the two source segments and
/// all rows after a restart.
#[test]
fn replay_compact_rollback() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);

    // The winning request drops col1; the merge is rolled back.
    h::ok(&e, "ALTER TABLE db1.tb1 DROP COLUMN (col1);");
    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(h::segment_rows(&e, "db1.tb1"), 32768);
    e.flush().unwrap();
    let blocks: usize = e
        .segment_ids("db1.tb1")
        .into_iter()
        .map(|s| e.block_ids("db1.tb1", s).len())
        .sum();
    assert_eq!(blocks, 4);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abc';"),
        vec![vec!["16384"]]
    );
}

/// C++ `src/unit_test/storage/new_catalog/replay_compact_ut.cpp:374`
/// (`TestTxnReplayCompact.test_replay_compact_flush_gap`): a checkpoint between
/// the merge and the restart does not lose rows.
#[test]
fn replay_compact_flush_gap() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    import_four_blocks(&e);
    e.checkpoint_wal();
    e.compact_all_segments("db1.tb1").unwrap();
    e.checkpoint_wal();

    let e = h::reopen(e, dir.path());
    assert_compacted(&e, 1);
}

/// C++ `src/unit_test/storage/new_catalog/replay_compact_ut.cpp:429`
/// (`TestTxnReplayCompact.test_compact_interrupt`): an uncommitted merge leaves
/// the source segments after a restart.
#[test]
fn replay_compact_interrupt() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    import_four_blocks(&e);
    // No compact_table: the merge transaction is interrupted.

    // Reopen without a flush to exercise WAL replay of the committed imports.
    drop(e);
    let e = Engine::open(dir.path()).unwrap();
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(h::segment_rows(&e, "db1.tb1"), 32768);
    h::ok(&e, "DROP TABLE db1.tb1;");
}

/// C++ `src/unit_test/storage/new_catalog/replay_compact_ut.cpp:468`
/// (`TestTxnReplayCompact.DISABLED_SLOW_test_compact_with_index`).
#[test]
#[ignore = "C++ DISABLED_SLOW_test_compact_with_index"]
fn disabled_slow_test_compact_with_index() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    h::ok(&e, "CREATE INDEX index2 ON db1.tb1 (col2) USING fulltext;");
    import_four_blocks(&e);
    e.compact_all_segments("db1.tb1").unwrap();

    let e = h::reopen(e, dir.path());
    assert_compacted(&e, 1);
    assert_eq!(h::count(&e, "db1.tb1"), 32768);
    assert_eq!(
        h::rows(&e, "SELECT count(*) FROM db1.tb1 WHERE col2 = 'abc';"),
        vec![vec!["16384"]]
    );
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index2;");
}

/// C++ `src/unit_test/storage/new_catalog/replay_compact_ut.cpp:522`
/// (`TestTxnReplayCompact.test_compact1_rollback`): dropping `col2` wins and
/// the merge is discarded; after a restart the index still serves `col1` and
/// the surviving column's rows are intact.
#[test]
fn replay_compact1_rollback() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    create_table(&e);
    h::ok(&e, "CREATE INDEX index1 ON db1.tb1 (col1);");
    import_four_blocks(&e);

    h::ok(&e, "ALTER TABLE db1.tb1 DROP COLUMN (col2);");
    let e = h::reopen(e, dir.path());
    assert_eq!(e.row_count("db1.tb1").unwrap(), 32768);
    assert_eq!(h::count(&e, "db1.tb1"), 32768);
    assert_eq!(h::rows(&e, "SHOW TABLE db1.tb1 COLUMNS;").len(), 1);
    // The merge is discarded; `col1`'s values are unchanged.
    assert_eq!(
        h::rows(&e, "SELECT col1 FROM db1.tb1 ORDER BY col1 LIMIT 1;"),
        vec![vec!["1".to_string()]]
    );
    assert_eq!(
        h::rows(&e, "SELECT col1 FROM db1.tb1 ORDER BY col1 DESC LIMIT 1;"),
        vec![vec!["2".to_string()]]
    );
    h::ok(&e, "SHOW TABLE db1.tb1 INDEX index1;");
}
