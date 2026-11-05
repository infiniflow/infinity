//! The block index of one table — the counterpart of the reference's
//! `BlockIndex` / `BlockMeta` (`src/storage/common/block_index.cppm`,
//! `src/storage/catalog/meta/block_meta.cppm`).
//!
//! A block is the C++ on-disk unit: one directory with one `{column_id}.col`
//! file per column (plus its `_out` outline file) and a `version` file written
//! last. [`BlockIndex::create_block`] writes that layout and records the
//! block's row range; [`BlockIndex::get_block`] maps it back (see
//! [`MappedBlock`]), so a scan reads the page cache instead of a heap copy.
//! [`BlockIndex::open`] rebuilds the block map from the `version` files

use std::collections::BTreeMap;
use std::path::{Path, PathBuf};
use std::sync::Arc;

use ::common::{Error, Result};

use crate::cpp_format::{
    block_dir, write_block, BlockColumn, BlockId, BlockVersionFile, VERSION_FILE,
};
use crate::mapped_block::MappedBlock;

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
/// `new_segment_block_index_` snapshot; a read maps the block's files instead
/// of taking a copy through a buffer manager.
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
        ts: ::common::TxnTimeStamp,
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

    /// Maps a block's files (C++ `BlockMeta::GetBlockColumnInfo`).
    ///
    /// The caller shares the returned mapping: the same `Arc` serves every
    /// reader of the block, and its pages are the page cache's, not the
    /// process heap.
    pub fn get_block(&self, id: &BlockId) -> Result<(BlockMeta, Arc<MappedBlock>)> {
        let meta = self
            .blocks
            .get(&(id.segment_id, id.block_id))
            .cloned()
            .ok_or_else(|| Error::NotFound(format!("block {id:?}")))?;
        let dir = meta.block_dir(&self.root);
        let block = MappedBlock::open(&self.root, *id).map_err(|e| Error::io(dir, e))?;
        Ok((meta, Arc::new(block)))
    }

    /// Drops a block's metadata (block delete).
    pub fn remove_block(&mut self, id: &BlockId) {
        self.blocks.remove(&(id.segment_id, id.block_id));
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::cpp_format::{BlockColumns, ColumnBody};

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

        let (meta, block) = index.get_block(&id(0, 0)).unwrap();
        assert_eq!(meta.row_offset, 0);
        assert_eq!(meta.row_count, 3);
        let (body, _) = block.column(0).unwrap();
        assert!(body.len() >= 12, "data region + null bitmap");
        assert_eq!(
            i32::from_le_bytes(body[8..12].try_into().unwrap()),
            3,
            "third value of the first block"
        );

        let (meta, block) = index.get_block(&id(0, 1)).unwrap();
        assert_eq!(meta.row_offset, 3);
        let (body, _) = block.column(0).unwrap();
        assert_eq!(i32::from_le_bytes(body[0..4].try_into().unwrap()), 4);
    }

    /// Each block maps only its own files, and dropping the mapping takes it
    /// off the process-wide counter.
    #[test]
    fn every_block_maps_only_its_own_files() {
        let dir = tempfile::tempdir().unwrap();
        let before = crate::mapped::mapping_count();
        let mut index = BlockIndex::new(dir.path());
        for b in 0..4u16 {
            index
                .create_block(id(0, b), b as usize, 4, &[column(0, &[b as i32; 4])], 1, 1)
                .unwrap();
        }
        {
            let mut mapped = Vec::new();
            for b in 0..4u16 {
                let (_, block) = index.get_block(&id(0, b)).unwrap();
                let (body, _) = block.column(0).unwrap();
                assert_eq!(i32::from_le_bytes(body[0..4].try_into().unwrap()), b as i32);
                mapped.push(block);
            }
            assert_eq!(
                crate::mapped::mapping_count(),
                before + 4,
                "one mapping per block, shared while it is held"
            );
        }
        assert_eq!(crate::mapped::mapping_count(), before);
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
        assert_eq!(reopened.block_count(), 2);
        assert_eq!(reopened.row_count(), 5);
        let offsets: Vec<usize> = reopened.metas().map(|m| m.row_offset).collect();
        assert_eq!(offsets, vec![0, 3]);
        assert_eq!(reopened.get_block(&id(0, 1)).unwrap().0.row_count, 2);
    }

    #[test]
    fn removing_a_block_drops_it_from_the_index() {
        let dir = tempfile::tempdir().unwrap();
        let mut index = BlockIndex::new(dir.path());
        index
            .create_block(id(0, 0), 0, 4, &[column(0, &[1])], 1, 1)
            .unwrap();
        index.remove_block(&id(0, 0));
        assert_eq!(index.block_count(), 0);
        assert_eq!(index.row_count(), 0);
        assert!(index.get_block(&id(0, 0)).is_err());
    }
}
