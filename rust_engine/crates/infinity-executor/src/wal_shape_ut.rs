//! The shape of the WAL frames the bulk paths write.
//!
//! The reference logs a compaction as one `CompactV2` command carrying the
//! merged segment and the deprecated segment ids (`CompactTxnStore::ToWalEntry`,
//! `base_txn_store_impl.cpp:489`) and an import as `ImportV2` with the new
//! segment's info (`ImportTxnStore::ToWalEntry`, `:378`). Neither carries any
//! row: the rows live in the blocks the transaction wrote. The byte fixtures
//! below were taken from the reference's own `wal.log` for the same statements.

use std::path::{Path, PathBuf};

use crate::Engine;
use infinity_storage::wal_entry::{WalCmd, WalEntry, WalEntryIterator, WalRowId};
use tempfile::tempdir;

fn write_csv(dir: &Path, name: &str, rows: usize) -> PathBuf {
    let path = dir.join(name);
    let mut text = String::new();
    for i in 0..rows {
        text.push_str(&format!("{i},abcdefghijklmnopqrstuvwxyz\n"));
    }
    std::fs::write(&path, text).unwrap();
    path
}

/// Every WAL entry the directory holds, in log order (a checkpoint rotates the
/// file, so all of them are read).
fn wal_entries(dir: &Path) -> Vec<WalEntry> {
    let wal_dir = dir.join("wal");
    let mut files: Vec<PathBuf> = std::fs::read_dir(&wal_dir)
        .unwrap()
        .flatten()
        .map(|e| e.path())
        .filter(|p| {
            p.file_name()
                .map(|n| n.to_string_lossy().starts_with("wal.log"))
                .unwrap_or(false)
        })
        .collect();
    // `wal.log.<seal_ts>` names the file by the ts it was sealed at.
    files.sort_by_key(|p| {
        p.file_name()
            .and_then(|n| n.to_str())
            .and_then(|n| n.rsplit('.').next())
            .and_then(|n| n.parse::<u64>().ok())
            .unwrap_or(u64::MAX)
    });
    let mut out = Vec::new();
    for file in files {
        let Ok(mut it) = WalEntryIterator::make(&file, false) else {
            continue;
        };
        while it.has_next() {
            let Some(entry) = it.next() else { break };
            out.push(entry);
        }
    }
    out
}

/// The frames that carry a command matching `predicate`.
fn frames_with(dir: &Path, predicate: impl Fn(&WalCmd) -> bool) -> Vec<Vec<WalCmd>> {
    wal_entries(dir)
        .into_iter()
        .filter(|e| e.cmds.iter().any(&predicate))
        .map(|e| e.cmds)
        .collect()
}

/// The command as it goes on the wire: an entry is `[24-byte header][cmds]
/// [i32 trailing size]`, so the command is everything between them.
fn command_bytes(cmd: &WalCmd) -> String {
    let entry = WalEntry::new(0, 0, vec![cmd.clone()]);
    let mut buf = Vec::new();
    entry.write_adv(&mut buf);
    // `[size i32][crc u32][txn_id i64][commit_ts u64][cmd_count i32]` is the
    // 28-byte header; the trailing size closes the frame.
    buf[28..buf.len() - 4]
        .iter()
        .map(|b| format!("{b:02x}"))
        .collect()
}

#[test]
fn a_compaction_logs_one_compact_command() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
    // Four imported segments: `NewCompactionAlg` needs four in one layer.
    for (i, rows) in [100usize, 60, 40, 30].iter().enumerate() {
        let csv = write_csv(dir.path(), &format!("s{i}.csv"), *rows);
        e.execute(&format!(
            "COPY t FROM '{}' WITH ( DELIMITER ',', FORMAT CSV );",
            csv.display()
        ))
        .unwrap();
    }
    assert_eq!(e.segment_ids("default_db.t"), vec![0, 1, 2, 3]);
    e.compact_all_segments("default_db.t").unwrap();

    let frames = frames_with(dir.path(), |c| matches!(c, WalCmd::Compact { .. }));
    assert_eq!(frames.len(), 1, "one compaction, one frame");
    let cmds = &frames[0];
    assert_eq!(cmds.len(), 1, "the frame carries only the compact command");
    let WalCmd::Compact {
        db_name,
        table_name,
        index_names,
        index_ids,
        new_segment_infos,
        deprecated_segment_ids,
        ..
    } = &cmds[0]
    else {
        panic!("expected CompactV2, got {:?}", cmds[0].command_type());
    };
    assert_eq!(db_name, "default_db");
    assert_eq!(table_name, "t");
    assert!(index_names.is_empty() && index_ids.is_empty());
    assert_eq!(deprecated_segment_ids, &vec![0, 1, 2, 3]);
    assert_eq!(new_segment_infos.len(), 1);
    let merged = &new_segment_infos[0];
    assert_eq!(merged.segment_id, 4);
    assert_eq!(merged.column_count, 2);
    assert_eq!(merged.row_capacity, 8_388_608);
    assert_eq!(merged.block_infos.len(), 1);
    assert_eq!(merged.block_infos[0].block_id, 0);
    assert_eq!(merged.block_infos[0].row_capacity, 8192);
    // `WalSegmentInfo(SegmentMeta, ts)` writes one `(1, 0)` outline per column.
    assert_eq!(merged.block_infos[0].outline_infos, vec![(1, 0), (1, 0)]);
    // `CompactTxnStore::ToWalEntry` copies the segment infos *before* the row
    // counts are filled in (`new_txn_data_impl.cpp:758` vs `:766`), so the
    // reference's command carries 0 there; replay ignores the field.
    assert_eq!(merged.row_count, 0);
    assert_eq!(merged.block_infos[0].row_count, 0);

    assert_eq!(
        command_bytes(&cmds[0]),
        "690a00000064656661756c745f646201000000310100000074010000003000000000000000000100000004000000020000000000000000000000000000000000000000000000000080000000000001000000000000000020020000000100000000000000000000000100000000000000000000000400000000000000010000000200000003000000",
        "the compact command must stay byte-identical to the reference's"
    );

    // No row command rides along: the rows are in the blocks.
    let rows_in_wal = wal_entries(dir.path())
        .iter()
        .flat_map(|e| e.cmds.iter())
        .filter(|c| matches!(c, WalCmd::Append { .. } | WalCmd::Delete { .. }))
        .count();
    assert_eq!(rows_in_wal, 0, "no AppendV2/DeleteV2 frames");
    assert_eq!(e.row_count("default_db.t").unwrap(), 230);
}

#[test]
fn an_import_logs_import_commands() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
    let csv = write_csv(dir.path(), "only.csv", 100);
    e.execute(&format!(
        "COPY t FROM '{}' WITH ( DELIMITER ',', FORMAT CSV );",
        csv.display()
    ))
    .unwrap();

    let frames = frames_with(dir.path(), |c| matches!(c, WalCmd::Import { .. }));
    assert_eq!(frames.len(), 1, "a 100-row import is one transaction");
    let cmds = &frames[0];
    assert_eq!(cmds.len(), 1);
    let WalCmd::Import {
        db_name,
        table_name,
        segment_info,
        ..
    } = &cmds[0]
    else {
        panic!("expected ImportV2");
    };
    assert_eq!(db_name, "default_db");
    assert_eq!(table_name, "t");
    assert_eq!(segment_info.segment_id, 0);
    assert_eq!(segment_info.column_count, 2);
    assert_eq!(segment_info.row_count, 100);
    assert_eq!(segment_info.row_capacity, 8_388_608);
    assert_eq!(segment_info.block_infos.len(), 1);
    assert_eq!(segment_info.block_infos[0].row_count, 100);
    assert_eq!(segment_info.block_infos[0].row_capacity, 8192);
    // Unlike the compact, the import's counts are filled in before
    // `ImportTxnStore::ToWalEntry` copies them (`new_txn_data_impl.cpp:340`).
    assert_eq!(
        command_bytes(&cmds[0]),
        "170a00000064656661756c745f64620100000031010000007401000000300000000002000000000000006400000000000000000000000000000000008000000000000100000000006400002002000000010000000000000000000000010000000000000000000000",
        "the import command must stay byte-identical to the reference's"
    );
    assert_eq!(e.row_count("default_db.t").unwrap(), 100);
}

/// A `COPY` that spans several parse batches is still **one** transaction and
/// therefore one frame, as in the reference (`NewTxn::Import`).
#[test]
fn a_long_import_is_still_one_frame() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
    let csv = write_csv(dir.path(), "big.csv", 20_000);
    e.execute(&format!(
        "COPY t FROM '{}' WITH ( DELIMITER ',', FORMAT CSV );",
        csv.display()
    ))
    .unwrap();

    let frames = frames_with(dir.path(), |c| matches!(c, WalCmd::Import { .. }));
    assert_eq!(frames.len(), 1, "one import, one transaction");
    let cmds = &frames[0];
    assert_eq!(cmds.len(), 1);
    let WalCmd::Import { segment_info, .. } = &cmds[0] else {
        panic!("expected ImportV2");
    };
    assert_eq!(segment_info.segment_id, 0);
    assert_eq!(segment_info.row_count, 20_000);
    // 20000 rows over `DEFAULT_BLOCK_CAPACITY` = 8192: 8192 + 8192 + 3616.
    assert_eq!(
        segment_info
            .block_infos
            .iter()
            .map(|b| b.row_count)
            .collect::<Vec<_>>(),
        vec![8192, 8192, 3616]
    );
    assert_eq!(e.row_count("default_db.t").unwrap(), 20_000);
}

/// The dump that runs inside an import puts its `DumpIndexV2` in the **same**
/// frame (`ImportTxnStore::ToWalEntry`, `base_txn_store_impl.cpp:387`), one per
/// index, after the `ImportV2`.
#[test]
fn an_indexed_import_logs_the_dump_in_the_same_frame() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
    e.execute("CREATE INDEX idx ON t(id);").unwrap();
    e.execute("CREATE INDEX ft ON t(name) USING fulltext;")
        .unwrap();
    let csv = write_csv(dir.path(), "idx.csv", 100);
    e.execute(&format!(
        "COPY t FROM '{}' WITH ( DELIMITER ',', FORMAT CSV );",
        csv.display()
    ))
    .unwrap();

    let frames = frames_with(dir.path(), |c| matches!(c, WalCmd::Import { .. }));
    assert_eq!(frames.len(), 1);
    let cmds = &frames[0];
    assert_eq!(
        cmds.iter().map(|c| c.command_type()).collect::<Vec<_>>(),
        vec![
            infinity_storage::wal_entry::WalCommandType::ImportV2,
            infinity_storage::wal_entry::WalCommandType::DumpIndexV2,
            infinity_storage::wal_entry::WalCommandType::DumpIndexV2,
        ]
    );
    let key = e.table_catalog_key("default_db.t").unwrap();
    let dumps: Vec<_> = cmds
        .iter()
        .filter_map(|c| match c {
            WalCmd::DumpIndex {
                index_name,
                segment_id,
                chunk_infos,
                deprecate_ids,
                table_key,
                ..
            } => Some((
                index_name.clone(),
                *segment_id,
                chunk_infos.clone(),
                deprecate_ids.clone(),
                table_key.clone(),
            )),
            _ => None,
        })
        .collect();
    assert_eq!(dumps.len(), 2);
    // `idx` (secondary): the reference leaves the chunk's `base_name` empty.
    assert_eq!(dumps[0].0, "idx");
    assert_eq!(dumps[0].1, 0);
    assert_eq!(dumps[0].4, key);
    let chunk = &dumps[0].2[0];
    assert_eq!(chunk.chunk_id, 0);
    assert_eq!(chunk.base_name, "");
    assert_eq!(chunk.row_count, 100);
    assert_eq!(chunk.term_count, 0);
    assert_eq!(chunk.index_size, 0);
    assert_eq!(chunk.deprecate_ts, u64::MAX);
    assert!(dumps[0].3.is_empty());
    // `ft` (full text): the chunk names its `ft_<base>` file and counts the
    // `(term, row)` postings it holds.
    assert_eq!(dumps[1].0, "ft");
    let chunk = &dumps[1].2[0];
    assert_eq!(chunk.base_name, "ft_0000000000000000");
    assert_eq!(chunk.row_count, 100);
    assert_eq!(chunk.term_count, 100);
    assert_eq!(e.row_count("default_db.t").unwrap(), 100);
}

/// A compaction appends the merged segment's dump to its own frame, with an
/// empty `table_key` (`CompactTxnStore` never sets it).
#[test]
fn an_indexed_compaction_logs_the_dump_in_the_same_frame() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
    e.execute("CREATE INDEX idxc ON t(id);").unwrap();
    for (i, rows) in [100usize, 60, 40, 30].iter().enumerate() {
        let csv = write_csv(dir.path(), &format!("c{i}.csv"), *rows);
        e.execute(&format!(
            "COPY t FROM '{}' WITH ( DELIMITER ',', FORMAT CSV );",
            csv.display()
        ))
        .unwrap();
    }
    e.compact_all_segments("default_db.t").unwrap();

    let frames = frames_with(dir.path(), |c| matches!(c, WalCmd::Compact { .. }));
    assert_eq!(frames.len(), 1);
    let cmds = &frames[0];
    assert_eq!(cmds.len(), 2);
    let WalCmd::DumpIndex {
        index_name,
        segment_id,
        chunk_infos,
        table_key,
        ..
    } = &cmds[1]
    else {
        panic!("expected a DumpIndexV2 after the CompactV2");
    };
    assert_eq!(index_name, "idxc");
    assert_eq!(*segment_id, 4);
    assert_eq!(table_key, "");
    assert_eq!(chunk_infos.len(), 1);
    assert_eq!(chunk_infos[0].base_row_id, WalRowId::new(4, 0));
    assert_eq!(chunk_infos[0].row_count, 230);
    assert_eq!(chunk_infos[0].deprecate_ts, u64::MAX);
}

/// `CREATE TABLE … WITH (bloom_filter_columns = '…')` puts the flag into the
/// `CreateTableV2` command's column defs (`TableDef::WriteAdv` ->
/// `ColumnDef::WriteAdv`), which is the only place the reference records it:
/// `ColumnDef::ToJson` omits it, so the catalog's `tbl|col|…` record does not
/// carry it and a reader of that key space sees every flag as false.
#[test]
fn a_create_table_logs_the_bloom_flag() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE bf (b BOOLEAN, i INT) WITH (bloom_filter_columns = 'b');")
        .unwrap();
    // The WAL writer buffers a frame until a checkpoint flushes it.
    e.flush().unwrap();
    for entry in wal_entries(dir.path()) {
        eprintln!(
            "ENTRY {} ts={} cmds={:?}",
            entry.txn_id,
            entry.commit_ts,
            entry
                .cmds
                .iter()
                .map(|c| format!("{:?}", c.command_type()))
                .collect::<Vec<_>>()
        );
    }
    let table_def = wal_entries(dir.path())
        .iter()
        .flat_map(|entry| entry.cmds.iter())
        .find_map(|cmd| match cmd {
            WalCmd::CreateTable { table_def, .. } if table_def.table_name == "bf" => {
                Some(table_def.clone())
            }
            _ => None,
        })
        .expect("the CREATE TABLE frame");
    assert_eq!(
        table_def
            .columns
            .iter()
            .map(|c| c.build_bloom_filter)
            .collect::<Vec<_>>(),
        vec![true, false]
    );
}

/// A DDL frame names the target **the statement created**, not whatever else
/// the batch holds. The batch carries the whole C++ layout (the DDL mirrors it
/// inside `Catalog::capture`), so "the last table/index record" stops being the
/// new one as soon as the catalog has more than one - which is how a second
/// `CREATE INDEX` used to be logged as a copy of the first.
#[test]
fn a_ddl_frame_names_the_target_the_statement_created() {
    use infinity_storage::wal::index_def::IndexDef;
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t1 (a INT, b VARCHAR);").unwrap();
    e.execute("CREATE TABLE t2 (id INT);").unwrap();
    e.execute("CREATE INDEX i1 ON t1 (a);").unwrap();
    e.execute("CREATE INDEX i2 ON t2 (id);").unwrap();

    let mut tables: Vec<(String, Vec<String>)> = Vec::new();
    let mut indexes: Vec<(String, String)> = Vec::new();
    for entry in wal_entries(dir.path()) {
        for cmd in &entry.cmds {
            match cmd {
                WalCmd::CreateTable { table_def, .. } => tables.push((
                    table_def.table_name.clone(),
                    table_def
                        .columns
                        .iter()
                        .map(|c| c.name.clone())
                        .collect::<Vec<_>>(),
                )),
                WalCmd::CreateIndex {
                    table_name,
                    index_base,
                    ..
                } => {
                    let IndexDef::Secondary { base, .. } = index_base else {
                        panic!("expected a secondary index, got {index_base:?}");
                    };
                    indexes.push((table_name.clone(), base.index_name.clone()));
                }
                _ => {}
            }
        }
    }
    assert_eq!(
        tables,
        vec![
            ("t1".to_string(), vec!["a".to_string(), "b".to_string()]),
            ("t2".to_string(), vec!["id".to_string()]),
        ],
        "each table frame carries its own columns"
    );
    assert_eq!(
        indexes,
        vec![
            ("t1".to_string(), "i1".to_string()),
            ("t2".to_string(), "i2".to_string()),
        ],
        "each index frame carries its own name"
    );
}
