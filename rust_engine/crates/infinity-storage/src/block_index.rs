//! The block index of one table — the counterpart of the reference's
//! `BlockIndex` / `BlockMeta` (`src/storage/common/block_index.cppm`,
//! `src/storage/catalog/meta/block_meta.cppm`).
//!
//! A block is the C++ on-disk unit: one directory with one `{column_id}.col`
//! file per column (plus its `_out` outline file) and a `version` file written
//! last. [`BlockIndex::create_block`] writes that layout and records the
//! block's row range; [`BlockIndex::get_block`] reads it back through a
//! [`BufferManager`] LRU, so a scan of many blocks is bounded by
//! `buffer_manager_size` instead of the table size. [`BlockIndex::open`]
//! rebuilds the block map from the `version` files (`BlockMeta::GetRowCnt1`).

use std::collections::BTreeMap;
use std::path::{Path, PathBuf};
use std::sync::Arc;

use infinity_common::{Error, Result};

use crate::buffer::{block_bytes, BufferManager};
use crate::cpp_format::{
    block_dir, read_block, write_block, BlockColumn, BlockId, BlockVersionFile, RawBlockColumn,
    VERSION_FILE,
};

/// Metadata of one block: its id and row range.
///
/// C++: `BlockMeta` (`src/storage/catalog/meta/block_meta.cppm`). The C++
/// class also owns the version buffer and the rough filter; this port reads
/// those from the block directory on demand.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct BlockMeta {
    pub id: BlockId,
    /// First row offset of the block inside its segment.
    pub row_offset: usize,
    /// Rows written into the block (`GetRowCnt1`).
    pub row_count: usize,
}

impl BlockMeta {
    /// C++ `BlockMeta::GetBlockDir()`.
    pub fn block_dir(&self, root: &Path) -> PathBuf {
        root.join(block_dir(
            self.id.db_id,
            self.id.table_id,
            self.id.segment_id,
            self.id.block_id,
        ))
    }
}

/// The block map of one table.
///
/// C++: `BlockIndex` (`src/storage/common/block_index.cppm`). `blocks` is the
/// `new_segment_block_index_` snapshot; the LRU is the read path the reference
/// implements with `BlockColumnInfo` + `BufferManager`.
pub struct BlockIndex {
    root: PathBuf,
    blocks: BTreeMap<(u32, u16), BlockMeta>,
}

impl BlockIndex {
    /// `root` is the data root (the directory above `db_{id}`); blocks live at
    /// `root/db_x/tbl_y/seg_z/blk_n` as in the reference.
    pub fn new(root: impl Into<PathBuf>) -> Self {
        Self {
            root: root.into(),
            blocks: BTreeMap::new(),
        }
    }

    /// C++ `BlockIndex::NewInit`/`BlockMeta::LoadSet`: recovers the block list
    /// from the on-disk `version` files, deriving each block's row range from
    /// the cumulative row counts.
    pub fn open(root: impl Into<PathBuf>, db_id: u64, table_id: u64) -> Result<Self> {
        let mut index = Self::new(root);
        let table_root = index
            .root
            .join(format!("db_{db_id}"))
            .join(format!("tbl_{table_id}"));
        let Ok(entries) = std::fs::read_dir(&table_root) else {
            return Ok(index);
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
                let version_path = index
                    .root
                    .join(block_dir(db_id, table_id, segment_id, block_id))
                    .join(VERSION_FILE);
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
                index.blocks.insert(
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
        Ok(index)
    }

    pub fn root(&self) -> &Path {
        &self.root
    }

    /// C++ `BlockIndex::BlockCount`.
    pub fn block_count(&self) -> usize {
        self.blocks.len()
    }

    /// C++ `BlockIndex::SegmentCount`.
    pub fn segment_count(&self) -> usize {
        self.blocks
            .keys()
            .map(|(segment, _)| *segment)
            .collect::<std::collections::BTreeSet<_>>()
            .len()
    }

    pub fn row_count(&self) -> usize {
        self.blocks.values().map(|m| m.row_count).sum()
    }

    pub fn is_empty(&self) -> bool {
        self.blocks.is_empty()
    }

    /// The next free block id of `seg_0`.
    pub fn next_block_id(&self) -> u16 {
        self.blocks
            .keys()
            .next_back()
            .map(|(_, block)| block.saturating_add(1))
            .unwrap_or(0)
    }

    pub fn metas(&self) -> impl Iterator<Item = &BlockMeta> {
        self.blocks.values()
    }

    /// C++ `BlockIndex::GetBlockMeta`.
    pub fn get_block_meta(&self, segment_id: u32, block_id: u16) -> Option<&BlockMeta> {
        self.blocks.get(&(segment_id, block_id))
    }

    /// Registers a block with no rows, like `SegmentMeta::AddBlockID1` followed
    /// by `BlockMeta::InitSet` in the reference (used when the allocator opens a
    /// block before its first row is written).
    pub fn add_empty_block(&mut self, db_id: u64, table_id: u64, segment_id: u32, block_id: u16) {
        let row_offset = self
            .blocks
            .iter()
            .filter(|((segment, _), _)| *segment == segment_id)
            .map(|(_, meta)| meta.row_offset + meta.row_count)
            .max()
            .unwrap_or(0);
        self.blocks.insert(
            (segment_id, block_id),
            BlockMeta {
                id: BlockId {
                    db_id,
                    table_id,
                    segment_id,
                    block_id,
                },
                row_offset,
                row_count: 0,
            },
        );
    }

    /// C++ `BlockIndex::GetSegmentOffset`: the segment's first row offset.
    pub fn segment_offset(&self, segment_id: u32) -> Option<usize> {
        self.blocks
            .range((segment_id, 0)..=(segment_id, u16::MAX))
            .next()
            .map(|(_, meta)| meta.row_offset)
    }

    /// C++ `BlockIndex::GetBlockOffset`: the block's first row offset.
    pub fn block_offset(&self, segment_id: u32, block_id: u16) -> Option<usize> {
        self.get_block_meta(segment_id, block_id)
            .map(|meta| meta.row_offset)
    }

    /// C++ `BlockMeta::GetBlockColumnInfo` through the buffer manager: writes a
    /// block's column files and records its metadata.
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
        let meta = BlockMeta {
            id,
            row_offset,
            row_count,
        };
        self.blocks
            .insert((id.segment_id, id.block_id), meta.clone());
        Ok(meta)
    }

    /// Reads a block's columns through the engine-wide buffer manager, serving
    /// repeats from its LRU cache (C++ `BlockMeta::GetBlockColumnInfo` +
    /// `BufferManager::Get`).
    pub fn get_block(
        &self,
        cache: &mut BufferManager<Vec<RawBlockColumn>>,
        id: &BlockId,
    ) -> Result<(BlockMeta, Arc<Vec<RawBlockColumn>>)> {
        let meta = self
            .blocks
            .get(&(id.segment_id, id.block_id))
            .cloned()
            .ok_or_else(|| Error::NotFound(format!("block {id:?}")))?;
        if let Some(columns) = cache.get(id) {
            return Ok((meta, columns));
        }
        let dir = meta.block_dir(&self.root);
        let (_, columns) = read_block(&self.root, *id).map_err(|e| Error::io(dir, e))?;
        let bytes = block_bytes(&columns);
        let columns = Arc::new(columns);
        cache.put(*id, columns.clone(), bytes);
        Ok((meta, columns))
    }

    /// Drops a block's cached payload (the caller owns the buffer manager) and
    /// its metadata (block delete).
    pub fn remove_block(&mut self, cache: &mut BufferManager<Vec<RawBlockColumn>>, id: &BlockId) {
        cache.invalidate(id);
        self.blocks.remove(&(id.segment_id, id.block_id));
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

    /// C++ `BlockIndex::BlockCount`/`GetBlockMeta`: every block reads back with
    /// its own rows and columns.
    #[test]
    fn creates_and_reads_blocks() {
        let dir = tempfile::tempdir().unwrap();
        let mut cache = BufferManager::new(1 << 20);
        let mut index = BlockIndex::new(dir.path());
        index
            .create_block(id(0, 0), 0, 4, &[column(0, &[1, 2, 3])], 3, 10)
            .unwrap();
        index
            .create_block(id(0, 1), 3, 4, &[column(0, &[4, 5])], 2, 20)
            .unwrap();
        assert_eq!(index.block_count(), 2);
        assert_eq!(index.segment_count(), 1);
        assert_eq!(index.row_count(), 5);
        assert_eq!(index.block_offset(0, 1), Some(3));
        assert_eq!(index.segment_offset(0), Some(0));

        let (meta, columns) = index.get_block(&mut cache, &id(0, 0)).unwrap();
        assert_eq!(meta.row_offset, 0);
        assert_eq!(meta.row_count, 3);
        let body = &columns.iter().find(|c| c.column_id == 0).unwrap().body;
        assert!(body.len() >= 12, "data region + null bitmap");
        assert_eq!(
            i32::from_le_bytes(body[8..12].try_into().unwrap()),
            3,
            "third value of the first block"
        );

        let (meta, columns) = index.get_block(&mut cache, &id(0, 1)).unwrap();
        assert_eq!(meta.row_offset, 3);
        assert_eq!(
            i32::from_le_bytes(
                columns.iter().find(|c| c.column_id == 0).unwrap().body[0..4]
                    .try_into()
                    .unwrap()
            ),
            4
        );
    }

    /// The decoded payload is cached and evicted by the LRU budget, but a
    /// reload is always correct.
    #[test]
    fn reads_go_through_the_bounded_cache() {
        let dir = tempfile::tempdir().unwrap();
        let mut cache = BufferManager::new(16);
        let mut index = BlockIndex::new(dir.path());
        for b in 0..4u16 {
            index
                .create_block(id(0, b), b as usize, 4, &[column(0, &[b as i32; 4])], 1, 1)
                .unwrap();
        }
        for b in 0..4u16 {
            let (_, columns) = index.get_block(&mut cache, &id(0, b)).unwrap();
            assert_eq!(
                i32::from_le_bytes(
                    columns.iter().find(|c| c.column_id == 0).unwrap().body[0..4]
                        .try_into()
                        .unwrap()
                ),
                b as i32
            );
        }
        assert!(cache.used_bytes() <= 16);
    }

    /// C++ `BlockIndex::NewInit`: the `version` files are enough to rebuild the
    /// block list and every block's row range.
    #[test]
    fn open_recovers_block_ranges_from_version_files() {
        let dir = tempfile::tempdir().unwrap();
        let mut index = BlockIndex::new(dir.path());
        index
            .create_block(id(0, 0), 0, 4, &[column(0, &[1, 2, 3])], 3, 10)
            .unwrap();
        index
            .create_block(id(0, 1), 3, 4, &[column(0, &[4, 5])], 2, 20)
            .unwrap();
        drop(index);

        let reopened = BlockIndex::open(dir.path(), 1, 2).unwrap();
        let mut cache = BufferManager::new(1 << 20);
        assert_eq!(reopened.block_count(), 2);
        assert_eq!(reopened.row_count(), 5);
        let offsets: Vec<usize> = reopened.metas().map(|m| m.row_offset).collect();
        assert_eq!(offsets, vec![0, 3]);
        assert_eq!(
            reopened
                .get_block(&mut cache, &id(0, 1))
                .unwrap()
                .0
                .row_count,
            2
        );
    }

    #[test]
    fn removing_a_block_drops_it_from_the_index() {
        let dir = tempfile::tempdir().unwrap();
        let mut cache = BufferManager::new(1 << 20);
        let mut index = BlockIndex::new(dir.path());
        index
            .create_block(id(0, 0), 0, 4, &[column(0, &[1])], 1, 1)
            .unwrap();
        index.remove_block(&mut cache, &id(0, 0));
        assert_eq!(index.block_count(), 0);
        assert_eq!(index.row_count(), 0);
        assert!(index.get_block(&mut cache, &id(0, 0)).is_err());
    }
}
