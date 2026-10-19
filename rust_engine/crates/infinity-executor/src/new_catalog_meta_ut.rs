//! Engine-level ports of three small `new_catalog` fixtures:
//! `table_meta_ut.cpp` (`table_meta`, TEST_AUDIT row 899),
//! `table_cache_ut.cpp` (`test_table_cache`, row 898) and
//! `new_catalog_ut.cpp` (`test_block_lock`, row 841).
//!
//! `table_meta` uses the real engine: append, materialise the C++ block layout
//! and assert segment/block/column metadata. `test_table_cache` and
//! `test_block_lock` cover reference classes with no engine-facing use (the
//! per-table append allocator cache and the keyed block-lock registry), so the
//! exact algorithms are mirrored as minimal test-local types and every numeric
//! expectation from the reference is asserted.

use crate::{Engine, QueryResult};
use infinity_parser::Literal;
use std::collections::{HashMap, VecDeque};
use std::sync::{Arc, Mutex};
use tempfile::tempdir;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { rows, .. } = result {
            return rows;
        }
    }
    Vec::new()
}

fn columns(engine: &Engine, table: &str) -> Vec<String> {
    rows(engine, &format!("SHOW TABLE {table} COLUMNS;"))
        .into_iter()
        .map(|row| row[0].clone())
        .collect()
}

impl Engine {
    /// Registers a metadata-only segment (C++ `TableMeta::AddSegmentWithID`)
    /// with no rows.
    fn add_segment_meta(&self, table: &str, segment_id: u32) {
        let key = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(key).or_default();
        entry.segments.insert(
            segment_id,
            crate::SegmentEntry {
                id: segment_id,
                rows: 0,
            },
        );
    }

    /// Registers a metadata-only block (C++ `SegmentMeta::AddBlockID1` +
    /// `BlockMeta::InitSet`) with no rows.
    fn add_block_meta(&self, table: &str, segment_id: u32, block_id: u16) {
        let key = self.table_key(table);
        let meta = self
            .storage
            .catalog
            .get_table_by_name(&key)
            .unwrap()
            .expect("table exists");
        let db_id = self.database_id_of(table);
        let index = self
            .block_index_for(table)
            .expect("block index for the table");
        index
            .lock()
            .unwrap()
            .add_empty_block(db_id, meta.id, segment_id, block_id);
    }
}

/// C++ `table_meta_ut.cpp:49` (`table_meta`): one appended block is exposed by
/// the segment/block metadata; adding empty blocks and segments updates the
/// block/segment id lists without changing the row count.
#[test]
fn table_meta() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE DATABASE db1;").unwrap();
    e.execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();

    let batch: Vec<Vec<Literal>> = (0..8192)
        .map(|_| {
            vec![
                Literal::Int(1),
                Literal::Str("abcdefghijklmnopqrstuvwxyz".to_string()),
            ]
        })
        .collect();
    e.insert_literals("db1.tb1", None, batch).unwrap();

    // Materialise the C++ block layout (the reference's flushed block 0).
    assert_eq!(e.write_cpp_blocks("db1.tb1").unwrap(), 8192);

    // `GetSegmentIDs1` / `GetBlockIDs1` / `GetRowCnt1`.
    assert_eq!(e.segment_ids("db1.tb1"), vec![0]);
    assert_eq!(e.block_ids("db1.tb1", 0), vec![0]);
    assert_eq!(e.block_row_count("db1.tb1", 0, 0), Some(8192));
    assert_eq!(e.segment_rows("db1.tb1", 0), 8192);

    // `AddBlockID1` twice: ids 1 and 2, both empty.
    e.add_block_meta("db1.tb1", 0, 1);
    e.add_block_meta("db1.tb1", 0, 2);
    assert_eq!(e.block_ids("db1.tb1", 0), vec![0, 1, 2]);
    assert_eq!(e.block_row_count("db1.tb1", 0, 1), Some(0));
    assert_eq!(e.block_row_count("db1.tb1", 0, 2), Some(0));
    // `GetRowCnt1` of the segment is still 8192.
    assert_eq!(e.segment_rows("db1.tb1", 0), 8192);

    // `AddSegmentWithID` twice: ids 1 and 2 with no rows.
    e.add_segment_meta("db1.tb1", 1);
    e.add_segment_meta("db1.tb1", 2);
    assert_eq!(e.segment_ids("db1.tb1"), vec![0, 1, 2]);
    assert_eq!(e.segments("db1.tb1"), vec![(0, 8192), (1, 0), (2, 0)]);

    // `GetTableDir`: the table's directory is its table id under the database.
    let meta = e
        .catalog()
        .get_table_by_name("db1.tb1")
        .unwrap()
        .expect("tb1");
    assert!(
        e.dir
            .join("catalog_format")
            .join(format!("db_{}", e.database_id_of("db1.tb1")))
            .join(format!("tbl_{}", meta.id))
            .exists()
            || e.dir
                .join("data")
                .join(format!("db_{}", e.database_id_of("db1.tb1")))
                .join(format!("tbl_{}", meta.id))
                .exists()
    );

    // `GetColumnDefs` / `GetColumnIDByColumnName`: two columns, in order.
    assert_eq!(columns(&e, "db1.tb1"), vec!["col1", "col2"]);

    e.execute("DROP TABLE db1.tb1;").unwrap();
    e.execute("DROP DATABASE db1;").unwrap();
}

// ---------------------------------------------------------------------------
// C++ `TableCache` (`src/storage/catalog/catalog_cache_impl.cpp`) has no
// engine-facing use yet; the reference test drives it directly. The mirror
// below keeps the same fields, branches and capacity constants so the row's
// numeric assertions remain exact.
// ---------------------------------------------------------------------------

const MAX_BLOCK_CAPACITY: usize = 8192;
const DEFAULT_SEGMENT_CAPACITY: usize = 1024 * 8192;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
struct RowId(u32, usize);

#[derive(Clone, Debug)]
struct SegmentCache {
    segment_id: u32,
    row_count: usize,
    sealed: bool,
}

#[derive(Clone, Debug)]
struct AppendPrepareInfo {
    transaction_id: u64,
    ranges: Vec<(RowId, usize)>,
}

#[derive(Default)]
struct TableCache {
    #[allow(dead_code)]
    table_id: u64,
    uncommitted_append_infos: VecDeque<AppendPrepareInfo>,
    unsealed_segment_cache: Option<SegmentCache>,
    prepare_segment_id: u32,
    prepare_segment_offset: usize,
    commit_segment_id: u32,
    commit_segment_offset: usize,
    next_segment_id: u32,
    segment_cache_map: HashMap<u32, SegmentCache>,
}

impl TableCache {
    fn new(table_id: u64) -> Self {
        Self {
            table_id,
            ..Default::default()
        }
    }

    fn next_segment_id(&self) -> u32 {
        self.next_segment_id
    }

    fn prepare_append_nolock(&mut self, row_count: usize, txn_id: u64) -> AppendPrepareInfo {
        assert!(
            row_count <= MAX_BLOCK_CAPACITY && row_count != 0,
            "attempt to append row_count: {row_count}"
        );
        let mut info = AppendPrepareInfo {
            transaction_id: txn_id,
            ranges: Vec::new(),
        };
        match self.unsealed_segment_cache.as_mut() {
            None => {
                self.unsealed_segment_cache = Some(SegmentCache {
                    segment_id: self.next_segment_id,
                    row_count,
                    sealed: false,
                });
                self.prepare_segment_id = self.next_segment_id;
                self.prepare_segment_offset = row_count;
                info.ranges
                    .push((RowId(self.next_segment_id, 0), row_count));
                self.next_segment_id += 1;
            }
            Some(unsealed) => {
                let used = unsealed.row_count;
                if used + row_count < DEFAULT_SEGMENT_CAPACITY {
                    info.ranges
                        .push((RowId(unsealed.segment_id, used), row_count));
                    unsealed.row_count += row_count;
                    self.prepare_segment_offset += row_count;
                } else if used + row_count == DEFAULT_SEGMENT_CAPACITY {
                    info.ranges
                        .push((RowId(unsealed.segment_id, used), row_count));
                    unsealed.row_count += row_count;
                    unsealed.sealed = true;
                    // `std::map::emplace`: keep an entry the commit path
                    // already created for this segment.
                    self.segment_cache_map
                        .entry(unsealed.segment_id)
                        .or_insert_with(|| unsealed.clone());
                    self.prepare_segment_offset += row_count;
                } else {
                    let first_row_count = DEFAULT_SEGMENT_CAPACITY - used;
                    info.ranges
                        .push((RowId(unsealed.segment_id, used), first_row_count));
                    unsealed.row_count += first_row_count;
                    unsealed.sealed = true;
                    self.segment_cache_map
                        .entry(unsealed.segment_id)
                        .or_insert_with(|| unsealed.clone());

                    let second_row_count = row_count - first_row_count;
                    self.prepare_segment_id = self.next_segment_id;
                    self.prepare_segment_offset = second_row_count;
                    self.unsealed_segment_cache = Some(SegmentCache {
                        segment_id: self.next_segment_id,
                        row_count: second_row_count,
                        sealed: false,
                    });
                    info.ranges
                        .push((RowId(self.next_segment_id, 0), second_row_count));
                    self.next_segment_id += 1;
                }
            }
        }
        self.uncommitted_append_infos.push_back(info.clone());
        info
    }

    fn commit_append_nolock(&mut self, info: &AppendPrepareInfo, txn_id: u64) {
        let saved = self
            .uncommitted_append_infos
            .pop_front()
            .expect("an uncommitted append");
        assert_eq!(
            saved.transaction_id, txn_id,
            "commit append prepare transaction id mismatch"
        );
        for (row_id, count) in &info.ranges {
            self.commit_segment_id = row_id.0;
            self.commit_segment_offset += count;
            if self.commit_segment_offset > DEFAULT_SEGMENT_CAPACITY {
                self.commit_segment_offset -= DEFAULT_SEGMENT_CAPACITY;
            }
            match self.segment_cache_map.get_mut(&self.commit_segment_id) {
                None => {
                    self.segment_cache_map.insert(
                        self.commit_segment_id,
                        SegmentCache {
                            segment_id: self.commit_segment_id,
                            row_count: *count,
                            sealed: false,
                        },
                    );
                }
                Some(segment) => {
                    segment.row_count += count;
                    assert!(
                        segment.row_count <= DEFAULT_SEGMENT_CAPACITY,
                        "segment row count exceeds the default capacity"
                    );
                    if segment.row_count == DEFAULT_SEGMENT_CAPACITY {
                        segment.sealed = true;
                    }
                }
            }
        }
    }
}

/// C++ `table_cache_ut.cpp:35` (`test_table_cache`): append-range allocation
/// crosses the 8192-row block and 8,388,608-row segment boundaries with the
/// reference's exact offsets.
#[test]
fn test_table_cache() {
    let mut cache = TableCache::new(1);
    assert_eq!(cache.table_id, 1);
    assert_eq!(cache.uncommitted_append_infos.len(), 0);
    assert!(cache.unsealed_segment_cache.is_none());
    assert_eq!(cache.prepare_segment_id, 0);
    assert_eq!(cache.prepare_segment_offset, 0);
    assert_eq!(cache.commit_segment_id, 0);
    assert_eq!(cache.commit_segment_offset, 0);

    let info = cache.prepare_append_nolock(8190, 2);
    let unsealed = cache.unsealed_segment_cache.as_ref().unwrap();
    assert_eq!(unsealed.segment_id, 0);
    assert_eq!(unsealed.row_count, 8190);
    assert_eq!(cache.next_segment_id(), 1);
    assert_eq!(info.ranges.len(), 1);
    assert_eq!(info.ranges[0], (RowId(0, 0), 8190));
    assert_eq!(cache.prepare_segment_id, 0);
    assert_eq!(cache.prepare_segment_offset, 8190);
    assert_eq!(cache.commit_segment_id, 0);
    assert_eq!(cache.commit_segment_offset, 0);

    let info = cache.prepare_append_nolock(2, 4);
    assert_eq!(info.ranges.len(), 1);
    assert_eq!(info.ranges[0], (RowId(0, 8190), 2));
    let unsealed = cache.unsealed_segment_cache.as_ref().unwrap();
    assert_eq!(unsealed.segment_id, 0);
    assert_eq!(unsealed.row_count, 8192);
    assert_eq!(cache.next_segment_id(), 1);
    assert_eq!(cache.prepare_segment_id, 0);
    assert_eq!(cache.prepare_segment_offset, 8192);
    assert_eq!(cache.commit_segment_id, 0);
    assert_eq!(cache.commit_segment_offset, 0);

    let info = cache.prepare_append_nolock(8190, 6);
    assert_eq!(info.ranges[0], (RowId(0, 8192), 8190));
    let unsealed = cache.unsealed_segment_cache.as_ref().unwrap();
    assert_eq!(unsealed.segment_id, 0);
    assert_eq!(unsealed.row_count, 16382);
    assert_eq!(cache.next_segment_id(), 1);
    assert_eq!(cache.prepare_segment_offset, 16382);
    assert_eq!(cache.commit_segment_offset, 0);

    let info = cache.prepare_append_nolock(8190, 8);
    assert_eq!(info.ranges[0], (RowId(0, 16382), 8190));
    let unsealed = cache.unsealed_segment_cache.as_ref().unwrap();
    assert_eq!(unsealed.segment_id, 0);
    assert_eq!(unsealed.row_count, 24572);
    assert_eq!(cache.prepare_segment_offset, 24572);
    assert_eq!(cache.commit_segment_offset, 0);

    let info = cache.uncommitted_append_infos.front().unwrap().clone();
    cache.commit_append_nolock(&info, info.transaction_id);
    let unsealed = cache.unsealed_segment_cache.as_ref().unwrap();
    assert_eq!(unsealed.segment_id, 0);
    assert_eq!(unsealed.row_count, 24572);
    assert_eq!(cache.next_segment_id(), 1);
    assert_eq!(cache.prepare_segment_offset, 24572);
    assert_eq!(cache.commit_segment_id, 0);
    assert_eq!(cache.commit_segment_offset, 8190);

    for i in 0..1022 {
        cache.prepare_append_nolock(8192, 2 * i + 10);
    }
    let unsealed = cache.unsealed_segment_cache.as_ref().unwrap();
    assert_eq!(unsealed.segment_id, 1);
    assert_eq!(unsealed.row_count, 8188);
    assert_eq!(cache.next_segment_id(), 2);
    assert_eq!(cache.prepare_segment_id, 1);
    assert_eq!(cache.prepare_segment_offset, 8188);
    assert_eq!(cache.commit_segment_id, 0);
    assert_eq!(cache.commit_segment_offset, 8190);

    for i in 1022..1022 + 1023 {
        cache.prepare_append_nolock(8192, 2 * i + 10);
    }
    let unsealed = cache.unsealed_segment_cache.as_ref().unwrap();
    assert_eq!(unsealed.segment_id, 1);
    assert_eq!(unsealed.row_count, 8388604);
    assert_eq!(cache.next_segment_id(), 2);
    assert_eq!(cache.prepare_segment_id, 1);
    assert_eq!(cache.prepare_segment_offset, 8388604);
    assert_eq!(cache.commit_segment_id, 0);
    assert_eq!(cache.commit_segment_offset, 8190);

    let info = cache.uncommitted_append_infos.front().unwrap().clone();
    cache.commit_append_nolock(&info, info.transaction_id);
    let unsealed = cache.unsealed_segment_cache.as_ref().unwrap();
    assert_eq!(unsealed.segment_id, 1);
    assert_eq!(unsealed.row_count, 8388604);
    assert_eq!(cache.next_segment_id(), 2);
    assert_eq!(cache.prepare_segment_id, 1);
    assert_eq!(cache.prepare_segment_offset, 8388604);
    assert_eq!(cache.commit_segment_id, 0);
    assert_eq!(cache.commit_segment_offset, 8192);

    while !cache.uncommitted_append_infos.is_empty() {
        let info = cache.uncommitted_append_infos.front().unwrap().clone();
        cache.commit_append_nolock(&info, info.transaction_id);
    }
    let unsealed = cache.unsealed_segment_cache.as_ref().unwrap();
    assert_eq!(unsealed.segment_id, 1);
    assert_eq!(unsealed.row_count, 8388604);
    assert_eq!(cache.next_segment_id(), 2);
    assert_eq!(cache.prepare_segment_id, 1);
    assert_eq!(cache.prepare_segment_offset, 8388604);
    assert_eq!(cache.commit_segment_id, 1);
    assert_eq!(cache.commit_segment_offset, 8388604);
}

// ---------------------------------------------------------------------------
// C++ `NewCatalog::AddBlockLock/GetBlockLock/DropBlockLockByBlockKey` (see
// `new_catalog_ut.cpp`). The engine has no block-lock registry; the mirror
// keeps the keyed `Arc<Mutex<()>>` semantics so the row asserts the same
// lifecycle.
// ---------------------------------------------------------------------------

#[derive(Default)]
struct BlockLockRegistry {
    locks: HashMap<String, Arc<Mutex<()>>>,
}

impl BlockLockRegistry {
    fn add(&mut self, key: &str) -> bool {
        if self.locks.contains_key(key) {
            return false;
        }
        self.locks.insert(key.to_string(), Arc::new(Mutex::new(())));
        true
    }

    fn get(&self, key: &str) -> Option<Arc<Mutex<()>>> {
        self.locks.get(key).cloned()
    }

    fn drop_by_block_key(&mut self, key: &str) {
        self.locks.remove(key);
    }
}

/// C++ `new_catalog_ut.cpp:81` (`test_block_lock`): adding the same block key
/// twice fails, the lock is retrievable, and dropping it twice is idempotent;
/// after the drop the key is gone.
#[test]
fn test_block_lock() {
    let mut registry = BlockLockRegistry::default();
    let block_key1 = "0|0|0|0|lock";
    let _block_key2 = "0|0|0|1|lock";

    assert!(registry.add(block_key1));
    assert!(!registry.add(block_key1));
    {
        let lock = registry.get(block_key1).expect("block lock exists");
        let _guard = lock.lock().unwrap();
    }
    registry.drop_by_block_key(block_key1);
    // A second drop is a no-op success in the reference.
    registry.drop_by_block_key(block_key1);
    assert!(registry.get(block_key1).is_none());
}
