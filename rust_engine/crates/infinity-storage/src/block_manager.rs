//! The block store of one table/segment — the counterpart of the reference's
//! `BlockManager` / `BlockEntry` / `BlockMeta`
//! (`src/storage/buffer/block_manager.cppm`,
//! `src/storage/meta/entry/block_entry.cppm`,
//! `src/storage/meta/entry/block_meta.cppm`).
//!
//! A block is the C++ on-disk unit: one directory with one `{column_id}.col`
//! file per column (plus its `_out` outline file) and a `version` file written
//! last. [`BlockManager::create_block`] writes that layout and records the
//! block's row range; [`BlockManager::get_block`] reads it back through a
//! [`BufferManager`] LRU, so a scan of many blocks is bounded by
//! `buffer_manager_size` instead of the table size.

use std::collections::BTreeMap;
use std::path::{Path, PathBuf};
use std::sync::Arc;

use infinity_common::{Error, Result};

use crate::buffer::{block_bytes, BufferManager};
use crate::cpp_format::{
    block_dir, read_block, write_block, BlockColumn, BlockId, BlockVersionFile, RawBlockColumn,
};

/// Metadata of one block: its id and row range.
///
/// C++: `BlockMeta` (`src/storage/meta/entry/block_meta.cppm`).
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct BlockMeta {
    pub id: BlockId,
    /// First row offset of the block inside its segment.
    pub row_offset: usize,
    /// Rows written into the block.
    pub row_count: usize,
}

/// An opened block: its metadata plus the decoded columns.
///
/// C++: `BlockEntry` (`src/storage/meta/entry/block_entry.cppm`). The Rust
/// executor reads columns straight from [`RawBlockColumn`]; this type owns the
/// pairing of metadata and payload.
pub struct BlockEntry {
    pub meta: BlockMeta,
    pub columns: Arc<Vec<RawBlockColumn>>,
}

impl BlockEntry {
    pub fn column(&self, column_id: u64) -> Option<&RawBlockColumn> {
        self.columns.iter().find(|c| c.column_id == column_id)
    }
}

/// The block store of one table or segment.
///
/// C++: `BlockManager` (`src/storage/buffer/block_manager.cppm`).
pub struct BlockManager {
    root: PathBuf,
    metas: BTreeMap<(u32, u16), BlockMeta>,
    cache: BufferManager<Vec<RawBlockColumn>>,
}

impl BlockManager {
    /// `root` is the table's data root (the directory above `seg_*`); blocks
    /// live at `root/seg_{segment}/blk_{block}` as in the reference.
    pub fn new(root: impl Into<PathBuf>, budget: usize) -> Self {
        Self {
            root: root.into(),
            metas: BTreeMap::new(),
            cache: BufferManager::new(budget),
        }
    }

    pub fn set_buffer_budget(&mut self, budget: usize) {
        self.cache.set_budget(budget);
    }

    pub fn root(&self) -> &Path {
        &self.root
    }

    /// C++ `BlockManager::Open`: recovers the block list from the on-disk
    /// `version` files (written last by [`create_block`](Self::create_block)),
    /// deriving each block's row range from the cumulative row counts.
    pub fn open(
        root: impl Into<PathBuf>,
        db_id: u64,
        table_id: u64,
        budget: usize,
    ) -> Result<Self> {
        let mut mgr = Self::new(root, budget);
        let table_root = mgr
            .root
            .join(format!("db_{db_id}"))
            .join(format!("tbl_{table_id}"));
        let Ok(entries) = std::fs::read_dir(&table_root) else {
            return Ok(mgr);
        };
        let mut segments: Vec<u32> = entries
            .flatten()
            .filter_map(|e| {
                e.file_name()
                    .to_str()?
                    .strip_prefix("seg_")?
                    .parse::<u32>()
                    .ok()
            })
            .collect();
        segments.sort_unstable();
        for segment_id in segments {
            let mut row_offset = 0usize;
            for block_id in 0..=u16::MAX {
                let id = BlockId {
                    db_id,
                    table_id,
                    segment_id,
                    block_id,
                };
                let version_path = mgr
                    .root
                    .join(block_dir(db_id, table_id, segment_id, block_id))
                    .join(crate::cpp_format::VERSION_FILE);
                let Ok(bytes) = std::fs::read(&version_path) else {
                    break;
                };
                let Ok(version) = BlockVersionFile::decode(&bytes) else {
                    break;
                };
                let row_count = version
                    .created
                    .last()
                    .map(|c| c.cumulative_row_count as usize)
                    .unwrap_or(0);
                mgr.metas.insert(
                    (segment_id, block_id),
                    BlockMeta {
                        id,
                        row_offset,
                        row_count,
                    },
                );
                row_offset += row_count;
            }
        }
        Ok(mgr)
    }

    /// C++ `BlockManager::CreateBlock` + `BlockEntry::CommitAppend`: writes the
    /// block's column files and records its metadata. `row_offset` is the
    /// block's first offset inside the segment.
    pub fn create_block(
        &mut self,
        id: BlockId,
        row_offset: usize,
        capacity: usize,
        columns: &[BlockColumn],
        row_count: usize,
        ts: infinity_common::TxnTimeStamp,
    ) -> Result<BlockMeta> {
        let version = BlockVersionFile::new(&[(ts, row_count as u32)], capacity);
        let dir = self
            .root
            .join(block_dir(id.db_id, id.table_id, id.segment_id, id.block_id));
        write_block(&self.root, id, columns, &version).map_err(|e| Error::io(dir, e))?;
        // A rewritten block must not be served from the previous payload.
        self.cache.invalidate(&id);
        let meta = BlockMeta {
            id,
            row_offset,
            row_count,
        };
        self.metas
            .insert((id.segment_id, id.block_id), meta.clone());
        Ok(meta)
    }

    /// C++ `BlockManager::GetBlock`: decodes a block, serving repeats from the
    /// LRU cache. The returned columns are immutable once read.
    pub fn get_block(&mut self, id: &BlockId) -> Result<BlockEntry> {
        let meta = self
            .metas
            .get(&(id.segment_id, id.block_id))
            .cloned()
            .ok_or_else(|| Error::NotFound(format!("block {id:?}")))?;
        if let Some(columns) = self.cache.get(id) {
            return Ok(BlockEntry { meta, columns });
        }
        let dir = self
            .root
            .join(block_dir(id.db_id, id.table_id, id.segment_id, id.block_id));
        let (_, columns) = read_block(&self.root, *id).map_err(|e| Error::io(dir, e))?;
        let bytes = block_bytes(&columns);
        let columns = Arc::new(columns);
        self.cache.put(*id, columns.clone(), bytes);
        Ok(BlockEntry { meta, columns })
    }

    /// Drops a block's cached payload and metadata (block delete).
    pub fn remove_block(&mut self, id: &BlockId) {
        self.cache.invalidate(id);
        self.metas.remove(&(id.segment_id, id.block_id));
    }

    pub fn block_count(&self) -> usize {
        self.metas.len()
    }

    pub fn row_count(&self) -> usize {
        self.metas.values().map(|m| m.row_count).sum()
    }

    pub fn metas(&self) -> impl Iterator<Item = &BlockMeta> {
        self.metas.values()
    }

    /// Bytes currently cached, for the unified memory budget.
    pub fn cached_bytes(&self) -> usize {
        self.cache.used_bytes()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::cpp_format::ColumnBody;

    fn id(segment: u32, block: u16) -> BlockId {
        BlockId {
            db_id: 1,
            table_id: 2,
            segment_id: segment,
            block_id: block,
        }
    }

    fn column(column_id: u64, values: &[i32]) -> BlockColumn {
        let data: Vec<u8> = values.iter().flat_map(|v| v.to_le_bytes()).collect();
        let body = ColumnBody::build(values.len(), 4, &data, &vec![true; values.len()]).unwrap();
        BlockColumn {
            column_id,
            body,
            outline: Vec::new(),
        }
    }

    /// C++ `BlockManager::CreateBlock` / `GetBlock`: every block reads back
    /// with its own rows and columns.
    #[test]
    fn creates_and_reads_blocks() {
        let dir = tempfile::tempdir().unwrap();
        let mut mgr = BlockManager::new(dir.path(), 1 << 20);
        let id0 = id(0, 0);
        let id1 = id(0, 1);
        mgr.create_block(id0, 0, 4, &[column(0, &[1, 2, 3])], 3, 10)
            .unwrap();
        mgr.create_block(id1, 3, 4, &[column(0, &[4, 5])], 2, 20)
            .unwrap();
        assert_eq!(mgr.block_count(), 2);
        assert_eq!(mgr.row_count(), 5);

        let b0 = mgr.get_block(&id0).unwrap();
        assert_eq!(b0.meta.row_offset, 0);
        assert_eq!(b0.meta.row_count, 3);
        let body = &b0.column(0).unwrap().body;
        assert!(body.len() >= 12, "data region + null bitmap");
        assert_eq!(
            i32::from_le_bytes(body[8..12].try_into().unwrap()),
            3,
            "third value of the first block"
        );

        let b1 = mgr.get_block(&id1).unwrap();
        assert_eq!(b1.meta.row_offset, 3);
        assert_eq!(
            i32::from_le_bytes(b1.column(0).unwrap().body[0..4].try_into().unwrap()),
            4
        );
    }

    /// The decoded payload is cached and evicted by the LRU budget, but a
    /// reload is always correct.
    #[test]
    fn reads_go_through_the_bounded_cache() {
        let dir = tempfile::tempdir().unwrap();
        let mut mgr = BlockManager::new(dir.path(), 16);
        for b in 0..4u16 {
            mgr.create_block(id(0, b), b as usize, 4, &[column(0, &[b as i32; 4])], 1, 1)
                .unwrap();
        }
        for b in 0..4u16 {
            let entry = mgr.get_block(&id(0, b)).unwrap();
            assert_eq!(
                i32::from_le_bytes(entry.column(0).unwrap().body[0..4].try_into().unwrap()),
                b as i32
            );
        }
        assert!(mgr.cached_bytes() <= 16);
    }

    /// C++ `BlockManager::Open`: the `version` files are enough to rebuild the
    /// block list and every block's row range.
    #[test]
    fn open_recovers_block_ranges_from_version_files() {
        let dir = tempfile::tempdir().unwrap();
        let mut mgr = BlockManager::new(dir.path(), 1 << 20);
        mgr.create_block(id(0, 0), 0, 4, &[column(0, &[1, 2, 3])], 3, 10)
            .unwrap();
        mgr.create_block(id(0, 1), 3, 4, &[column(0, &[4, 5])], 2, 20)
            .unwrap();
        drop(mgr);

        let mut reopened = BlockManager::open(dir.path(), 1, 2, 1 << 20).unwrap();
        assert_eq!(reopened.block_count(), 2);
        assert_eq!(reopened.row_count(), 5);
        let offsets: Vec<usize> = reopened.metas().map(|m| m.row_offset).collect();
        assert_eq!(offsets, vec![0, 3]);
        assert_eq!(reopened.get_block(&id(0, 1)).unwrap().meta.row_count, 2);
    }

    #[test]
    fn removing_a_block_drops_it_from_the_manager() {
        let dir = tempfile::tempdir().unwrap();
        let mut mgr = BlockManager::new(dir.path(), 1 << 20);
        mgr.create_block(id(0, 0), 0, 4, &[column(0, &[1])], 1, 1)
            .unwrap();
        mgr.remove_block(&id(0, 0));
        assert_eq!(mgr.block_count(), 0);
        assert_eq!(mgr.row_count(), 0);
        assert!(mgr.get_block(&id(0, 0)).is_err());
    }
}
