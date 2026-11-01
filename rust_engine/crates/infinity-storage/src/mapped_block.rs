//! One sealed block, read through `mmap`.
//!
//! The reference keeps a block's payload in a `BufferObject`: it is read into
//! the buffer manager's budget, spilled or switched to a mapping when it is
//! evicted, and handed out through handles. None of that is needed here - the
//! column files of a sealed block never change, so a reader maps them and
//! decodes the cells it wants out of the page cache. What is shared in the
//! process is the mapping itself ([`MappedBlock`] behind an `Arc`), which costs
//! address space rather than memory.

use std::path::{Path, PathBuf};

use crate::cpp_format::{
    block_dir, column_file, column_outline_file, data_file_body, BlockColumns, BlockId,
    BlockVersionFile, VERSION_FILE,
};
use crate::mapped::MappedFile;

/// A sealed block: its `version` file and the mapping of every column file.
#[derive(Debug)]
pub struct MappedBlock {
    id: BlockId,
    version: BlockVersionFile,
    /// `(column id, `{id}.col`, `col_{id}_out` when the column has an outline)`,
    /// in column-id order.
    columns: Vec<(u64, MappedFile, Option<MappedFile>)>,
    bytes: usize,
}

impl MappedBlock {
    /// Maps the block directory `root/db_*/tbl_*/seg_*/blk_*`.
    ///
    /// The `version` file is read first: it is written **last** by the writer,
    /// so a directory whose `version` decodes is a complete block.
    pub fn open(root: &Path, id: BlockId) -> std::io::Result<Self> {
        let dir = root.join(block_dir(id.db_id, id.table_id, id.segment_id, id.block_id));
        let version = BlockVersionFile::decode(&std::fs::read(dir.join(VERSION_FILE))?)
            .map_err(|e| std::io::Error::new(std::io::ErrorKind::InvalidData, e))?;
        let mut columns: Vec<(u64, MappedFile, Option<MappedFile>)> = Vec::new();
        let mut bytes = 0usize;
        for entry in std::fs::read_dir(&dir)? {
            let entry = entry?;
            let name = entry.file_name();
            let Some(name) = name.to_str() else { continue };
            let Some(stem) = name.strip_suffix(".col") else {
                continue;
            };
            let Ok(column_id) = stem.parse::<u64>() else {
                continue;
            };
            let data = MappedFile::open(dir.join(column_file(column_id)))?;
            bytes += data.len();
            let outline_path = dir.join(column_outline_file(column_id));
            let outline = if outline_path.exists() {
                let outline = MappedFile::open(outline_path)?;
                bytes += outline.len();
                Some(outline)
            } else {
                None
            };
            columns.push((column_id, data, outline));
        }
        columns.sort_by_key(|(column_id, _, _)| *column_id);
        Ok(Self {
            id,
            version,
            columns,
            bytes,
        })
    }

    pub fn id(&self) -> BlockId {
        self.id
    }

    pub fn version(&self) -> &BlockVersionFile {
        &self.version
    }

    /// Bytes the block's files cover (what the mapping charges to the page
    /// cache, not what the process allocates).
    pub fn bytes(&self) -> usize {
        self.bytes
    }

    /// The block's column payloads as slices of the mapping.
    pub fn columns(&self) -> MappedColumns<'_> {
        MappedColumns { block: self }
    }

    /// The directory the block was mapped from.
    pub fn dir(&self, root: &Path) -> PathBuf {
        root.join(block_dir(
            self.id.db_id,
            self.id.table_id,
            self.id.segment_id,
            self.id.block_id,
        ))
    }
}
/// A mapped block answers column lookups directly, so a decoder can take it
/// without the [`MappedColumns`] wrapper.
impl BlockColumns for MappedBlock {
    fn column(&self, column_id: u64) -> Option<(&[u8], &[u8])> {
        let (_, data, outline) = self
            .columns
            .iter()
            .find(|(id, _, _)| *id == column_id)?;
        // A `.col` file is `magic + size + body + checksum`; the body is the
        // `[data_region][null_bitmap]` region the decoders split.
        let body = data_file_body(data).ok()?;
        Some((body, outline.as_deref().unwrap_or(&[])))
    }
}

/// [`MappedBlock::columns`]: the block seen as column payloads.
pub struct MappedColumns<'a> {
    block: &'a MappedBlock,
}

impl BlockColumns for MappedColumns<'_> {
    fn column(&self, column_id: u64) -> Option<(&[u8], &[u8])> {
        self.block.column(column_id)
    }
}

// ---------------------------------------------------------------------------
// `SHOW BUFFERS` / `SHOW OBJECT` row shape
// ---------------------------------------------------------------------------

/// C++ `BufferObjectInfo` (`buffer/buffer_obj.cppm:46`) — one row of
/// `SHOW BUFFERS`.
///
/// The port has no owned buffer objects any more: a sealed block is mapped and
/// shared, so a row describes one **mapping** (its block directory, the bytes
/// it covers and the `Mmap` state the reference switches to when a payload
/// outlives its handles).
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct BufferObjectInfo {
    pub object_path: PathBuf,
    pub buffered_status: BufferStatus,
    pub buffered_type: BufferType,
    pub file_type: FileWorkerType,
    pub object_size: usize,
}

/// C++ `BufferStatus` (`buffer_obj.cppm`).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum BufferStatus {
    New,
    Loaded,
    Unloaded,
    Freed,
    Clean,
}

impl BufferStatus {
    /// C++ `BufferStatusToString` (`buffer_obj.cppm:52`).
    pub fn as_str(self) -> &'static str {
        match self {
            BufferStatus::Loaded => "Loaded",
            BufferStatus::Unloaded => "Unloaded",
            BufferStatus::Freed => "Freed",
            BufferStatus::New => "New",
            BufferStatus::Clean => "Clean",
        }
    }
}

/// C++ `BufferType`: whether a payload outlives its last handle.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum BufferType {
    Ephemeral,
    Persistent,
    Temp,
    ToMmap,
    /// `kMmap`: reads come from the mapped file, not from a buffer budget.
    Mmap,
}

impl BufferType {
    /// C++ `BufferTypeToString` (`buffer_obj.cppm:67`).
    pub fn as_str(self) -> &'static str {
        match self {
            BufferType::Persistent => "Persistent",
            BufferType::Ephemeral => "Ephemeral",
            BufferType::Temp => "Temporary",
            BufferType::ToMmap => "ToMmap",
            BufferType::Mmap => "Mmap",
        }
    }
}

/// C++ `FileWorkerType`, as reported by `BufferObjectInfo`.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum FileWorkerType {
    Invalid,
    /// `DataFileWorker`: a fixed-size payload.
    DataFile,
    /// `VarFileWorker`: a payload that grows on append.
    VarFile,
}

impl FileWorkerType {
    /// C++ `FileWorkerTypeToString` (`buffer_obj.cppm`).
    pub fn as_str(self) -> &'static str {
        match self {
            FileWorkerType::Invalid => "Invalid",
            FileWorkerType::DataFile => "data",
            FileWorkerType::VarFile => "var",
        }
    }
}

