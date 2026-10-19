//! Structured WAL entries — the counterpart of the reference's `WalEntry`,
//! `WalCmd*`, `WalEntryIterator` and `WalListIterator`
//! (`src/storage/wal/wal_entry.cppm`, `wal_entry_impl.cpp`).
//!
//! The engine's existing [`crate::wal::WalManager`] writes opaque payloads with
//! a private frame. This module adds the reference's *structured* redo log:
//! every entry carries typed commands (create/drop database, create/drop
//! table, import, append, delete, checkpoint, compact, dump index, rename and
//! column alters), which is what replay is defined over.
//!
//! # Wire format
//!
//! An entry is serialized exactly like the reference
//! (`wal_entry_impl.cpp:2193`):
//!
//! ```text
//! [size: i32][checksum: u32][txn_id: i64][commit_ts: u64]
//! [cmd_count: i32][cmd]...
//! [size: i32]                       // copy of the leading size
//! ```
//!
//! `size` counts the whole frame including both size fields; `checksum` is
//! CRC-32/IEEE over those `size` bytes with the checksum field itself zeroed
//! (`wal_entry_impl.cpp:2203-2210`). The trailing copy lets the backward
//! iterator find the previous frame; `ReadAdv` cross-checks it against the
//! leading size (`wal_entry_impl.cpp:2233`).
//!
//! Strings are `[len: i32][bytes]`, integers little-endian. The payloads of the
//! individual commands follow `WalCmd*::WriteAdv`; the `DataBlock` payload
//! stores each column's logical type and typed values.

use std::collections::VecDeque;
use std::fs::OpenOptions;
use std::io::Write;
use std::path::{Path, PathBuf};

use infinity_common::{Error, LogicalType, Result, TxnTimeStamp};

use crate::crc::crc32_ieee;
use crate::data_block::{BlockValue, Column, ColumnVector, DataBlock};

/// `WalEntryHeader` (`wal_entry.cppm:879`) is 24 bytes on the wire.
pub const WAL_ENTRY_HEADER_LEN: usize = 24;

/// C++: `WalCommandType` (`wal_entry.cppm:42`).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[repr(i8)]
pub enum WalCommandType {
    Invalid = 0,
    CreateDatabaseV2 = 8,
    DropDatabaseV2 = 9,
    CreateTableV2 = 10,
    DropTableV2 = 11,
    AlterInfoV2 = 12,
    CreateIndexV2 = 13,
    DropIndexV2 = 14,
    ImportV2 = 23,
    AppendV2 = 24,
    DeleteV2 = 25,
    RenameTableV2 = 43,
    AddColumnsV2 = 44,
    DropColumnsV2 = 45,
    AlterIndexV2 = 46,
    CheckpointV2 = 104,
    CompactV2 = 105,
    DumpIndexV2 = 107,
    Cleanup = 108,
    CreateTableSnapshot = 110,
    CreateDbSnapshot = 111,
    CreateSystemSnapshot = 112,
    RestoreTableSnapshot = 113,
    RestoreDatabaseSnapshot = 114,
    RestoreSystemSnapshot = 115,
}

impl WalCommandType {
    fn from_i8(v: i8) -> Option<Self> {
        Some(match v {
            0 => Self::Invalid,
            8 => Self::CreateDatabaseV2,
            9 => Self::DropDatabaseV2,
            10 => Self::CreateTableV2,
            11 => Self::DropTableV2,
            12 => Self::AlterInfoV2,
            13 => Self::CreateIndexV2,
            14 => Self::DropIndexV2,
            23 => Self::ImportV2,
            24 => Self::AppendV2,
            25 => Self::DeleteV2,
            43 => Self::RenameTableV2,
            44 => Self::AddColumnsV2,
            45 => Self::DropColumnsV2,
            46 => Self::AlterIndexV2,
            104 => Self::CheckpointV2,
            105 => Self::CompactV2,
            107 => Self::DumpIndexV2,
            108 => Self::Cleanup,
            110 => Self::CreateTableSnapshot,
            111 => Self::CreateDbSnapshot,
            112 => Self::CreateSystemSnapshot,
            113 => Self::RestoreTableSnapshot,
            114 => Self::RestoreDatabaseSnapshot,
            115 => Self::RestoreSystemSnapshot,
            _ => return None,
        })
    }
}

/// C++: `RowID` (`src/common/types/row_id.h`): 8 bytes, `segment_id` then
/// `segment_offset` (the Rust engine's [`infinity_common::RowID`] is a packed
/// `u64`; WAL entries keep the reference's pair).
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq, PartialOrd, Ord)]
pub struct WalRowId {
    pub segment_id: u32,
    pub segment_offset: u32,
}

impl WalRowId {
    pub fn new(segment_id: u32, segment_offset: u32) -> Self {
        Self {
            segment_id,
            segment_offset,
        }
    }
}

/// C++: `ConstraintType`.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum ConstraintType {
    PrimaryKey = 1,
    NotNull = 2,
    Unique = 3,
    Invalid = 4,
}

/// C++: `ColumnDef` as carried by `WalCmdCreateTableV2`/`WalCmdAddColumnsV2`.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct WalColumnDef {
    pub column_id: u32,
    pub name: String,
    pub logical_type: LogicalType,
    pub constraints: Vec<ConstraintType>,
}

/// C++: `TableDef` as carried by `WalCmdCreateTableV2`.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct WalTableDef {
    pub db_name: String,
    pub table_name: String,
    pub table_id: String,
    pub comment: String,
    pub columns: Vec<WalColumnDef>,
}

impl WalTableDef {
    pub fn new(
        db_name: impl Into<String>,
        table_name: impl Into<String>,
        table_id: impl Into<String>,
        comment: impl Into<String>,
        columns: Vec<WalColumnDef>,
    ) -> Self {
        Self {
            db_name: db_name.into(),
            table_name: table_name.into(),
            table_id: table_id.into(),
            comment: comment.into(),
            columns,
        }
    }
}

/// C++: `IndexBase` as carried by `WalCmdCreateIndexV2`.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct WalIndexBase {
    pub index_name: String,
    pub comment: String,
    pub index_id: String,
    pub column_names: Vec<String>,
    pub index_type: String,
    pub params: Vec<(String, String)>,
}

impl WalIndexBase {
    pub fn new(
        index_name: impl Into<String>,
        comment: impl Into<String>,
        index_id: impl Into<String>,
        column_names: Vec<String>,
        index_type: impl Into<String>,
        params: Vec<(String, String)>,
    ) -> Self {
        Self {
            index_name: index_name.into(),
            comment: comment.into(),
            index_id: index_id.into(),
            column_names,
            index_type: index_type.into(),
            params,
        }
    }
}

/// C++: `WalBlockInfo` (`wal_entry.cppm:93`).
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct WalBlockInfo {
    pub block_id: u32,
    pub row_count: u16,
    pub row_capacity: u16,
    /// `(buffer_count, last_chunk_offset)` per column outline.
    pub outline_infos: Vec<(u32, u64)>,
}

/// C++: `WalSegmentInfo` (`wal_entry.cppm:114`).
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct WalSegmentInfo {
    pub segment_id: u32,
    pub column_count: u64,
    pub row_count: u64,
    pub actual_row_count: u64,
    pub row_capacity: u64,
    pub block_infos: Vec<WalBlockInfo>,
}

/// C++: `WalChunkIndexInfo` (`wal_entry.cppm:156`).
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct WalChunkIndexInfo {
    pub chunk_id: u32,
    pub base_name: String,
    pub base_row_id: WalRowId,
    pub row_count: u32,
    pub term_count: u32,
    pub index_size: u32,
    pub deprecate_ts: TxnTimeStamp,
}

/// C++: `WalSegmentIndexInfo` (`wal_entry.cppm:181`).
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct WalSegmentIndexInfo {
    pub segment_id: u32,
    pub chunk_infos: Vec<WalChunkIndexInfo>,
}

/// C++: `WalCmd` hierarchy (`wal_entry.cppm:202-877`).
#[derive(Clone, Debug, PartialEq)]
pub enum WalCmd {
    CreateDatabase {
        db_name: String,
        db_id: String,
        db_comment: String,
    },
    DropDatabase {
        db_name: String,
        db_id: String,
        create_ts: TxnTimeStamp,
    },
    CreateTable {
        db_name: String,
        db_id: String,
        table_id: String,
        table_def: WalTableDef,
    },
    DropTable {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        create_ts: TxnTimeStamp,
        table_key: String,
    },
    CreateIndex {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        index_id: String,
        index_base: WalIndexBase,
        segment_index_infos: Vec<WalSegmentIndexInfo>,
        table_key: String,
    },
    DropIndex {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        index_name: String,
        index_id: String,
        create_ts: TxnTimeStamp,
        index_key: String,
    },
    Import {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        segment_info: WalSegmentInfo,
    },
    Append {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        row_ranges: Vec<(WalRowId, u64)>,
        block: DataBlock,
    },
    Delete {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        row_ids: Vec<WalRowId>,
    },
    Checkpoint {
        max_commit_ts: TxnTimeStamp,
    },
    Compact {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        index_names: Vec<String>,
        index_ids: Vec<String>,
        new_segment_infos: Vec<WalSegmentInfo>,
        deprecated_segment_ids: Vec<u32>,
    },
    DumpIndex {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        index_name: String,
        index_id: String,
        segment_id: u32,
        chunk_infos: Vec<WalChunkIndexInfo>,
        deprecate_ids: Vec<u32>,
        table_key: String,
    },
    RenameTable {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        new_table_name: String,
        old_table_key: String,
    },
    AddColumns {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        column_idx_list: Vec<u32>,
        column_defs: Vec<WalColumnDef>,
        table_key: String,
    },
    DropColumns {
        db_name: String,
        db_id: String,
        table_name: String,
        table_id: String,
        column_names: Vec<String>,
        column_ids: Vec<u32>,
        table_key: String,
        column_keys: Vec<String>,
    },
    Cleanup {
        timestamp: TxnTimeStamp,
    },
}

impl WalCmd {
    pub fn command_type(&self) -> WalCommandType {
        match self {
            WalCmd::CreateDatabase { .. } => WalCommandType::CreateDatabaseV2,
            WalCmd::DropDatabase { .. } => WalCommandType::DropDatabaseV2,
            WalCmd::CreateTable { .. } => WalCommandType::CreateTableV2,
            WalCmd::DropTable { .. } => WalCommandType::DropTableV2,
            WalCmd::CreateIndex { .. } => WalCommandType::CreateIndexV2,
            WalCmd::DropIndex { .. } => WalCommandType::DropIndexV2,
            WalCmd::Import { .. } => WalCommandType::ImportV2,
            WalCmd::Append { .. } => WalCommandType::AppendV2,
            WalCmd::Delete { .. } => WalCommandType::DeleteV2,
            WalCmd::Checkpoint { .. } => WalCommandType::CheckpointV2,
            WalCmd::Compact { .. } => WalCommandType::CompactV2,
            WalCmd::DumpIndex { .. } => WalCommandType::DumpIndexV2,
            WalCmd::RenameTable { .. } => WalCommandType::RenameTableV2,
            WalCmd::AddColumns { .. } => WalCommandType::AddColumnsV2,
            WalCmd::DropColumns { .. } => WalCommandType::DropColumnsV2,
            WalCmd::Cleanup { .. } => WalCommandType::Cleanup,
        }
    }
}

/// C++: `WalEntry` (`wal_entry.cppm:888`). `size`/`checksum` are produced on
/// write (see [`WalEntry::write_adv`]); the in-memory entry compares like the
/// reference, over `txn_id`, `commit_ts` and the command list
/// (`wal_entry_impl.cpp:2153`).
#[derive(Clone, Debug, Default, PartialEq)]
pub struct WalEntry {
    pub txn_id: i64,
    pub commit_ts: TxnTimeStamp,
    pub cmds: Vec<WalCmd>,
}

impl WalEntry {
    pub fn new(txn_id: i64, commit_ts: TxnTimeStamp, cmds: Vec<WalCmd>) -> Self {
        Self {
            txn_id,
            commit_ts,
            cmds,
        }
    }

    /// C++: `WalEntry::GetSizeInBytes` (`wal_entry_impl.cpp:2169`).
    pub fn size_in_bytes(&self) -> usize {
        let mut counter = CountSink(0);
        self.encode_entry(&mut counter);
        counter.0
    }

    /// C++: `WalEntry::IsCheckPoint` (`wal_entry_impl.cpp:2273`): returns the
    /// largest `max_commit_ts` among the checkpoint commands.
    pub fn checkpoint_ts(&self) -> Option<TxnTimeStamp> {
        self.cmds
            .iter()
            .filter_map(|cmd| match cmd {
                WalCmd::Checkpoint { max_commit_ts } => Some(*max_commit_ts),
                _ => None,
            })
            .max()
    }

    pub fn is_checkpoint(&self) -> bool {
        self.checkpoint_ts().is_some()
    }

    fn encode_entry<S: Sink>(&self, sink: &mut S) {
        put_i32(sink, 0); // size placeholder
        put_u32(sink, 0); // checksum placeholder
        put_i64(sink, self.txn_id);
        put_u64(sink, self.commit_ts);
        put_i32(sink, self.cmds.len() as i32);
        for cmd in &self.cmds {
            encode_cmd(sink, cmd);
        }
        put_i32(sink, 0); // trailing size placeholder
    }

    /// C++: `WalEntry::WriteAdv` (`wal_entry_impl.cpp:2193`). Appends exactly
    /// [`WalEntry::size_in_bytes`] bytes to `out`.
    pub fn write_adv(&self, out: &mut Vec<u8>) {
        let start = out.len();
        self.encode_entry(out);
        let size = (out.len() - start) as i32;
        out[start..start + 4].copy_from_slice(&size.to_le_bytes());
        out[start + 4..start + 8].copy_from_slice(&0u32.to_le_bytes());
        // The trailing size copy was written as zero; store the real value in
        // it before computing the checksum (the reference includes the pad).
        let end = out.len();
        out[end - 4..].copy_from_slice(&size.to_le_bytes());
        let checksum = crc32_ieee(&out[start..end]);
        out[start + 4..start + 8].copy_from_slice(&checksum.to_le_bytes());
    }

    /// C++: `WalEntry::ReadAdv` (`wal_entry_impl.cpp:2213`). Returns the entry
    /// and the number of bytes it occupies.
    pub fn read_adv(buf: &[u8]) -> Option<(Self, usize)> {
        if buf.len() < WAL_ENTRY_HEADER_LEN {
            return None;
        }
        let size = i32::from_le_bytes(buf[0..4].try_into().ok()?) as i64;
        if size <= WAL_ENTRY_HEADER_LEN as i64 || size > buf.len() as i64 {
            return None;
        }
        let size = size as usize;
        let stored_checksum = u32::from_le_bytes(buf[4..8].try_into().ok()?);
        let trailing = i32::from_le_bytes(buf[size - 4..size].try_into().ok()?);
        if trailing as usize != size {
            return None;
        }
        let mut hasher = crc32fast::Hasher::new();
        hasher.update(&buf[..4]);
        hasher.update(&[0u8; 4]);
        hasher.update(&buf[8..size]);
        if hasher.finalize() != stored_checksum {
            return None;
        }

        // The header carries `txn_id` and `commit_ts`; the command count is the
        // first field behind it (`wal_entry_impl.cpp:2251`).
        let txn_id = i64::from_le_bytes(buf[8..16].try_into().ok()?);
        let commit_ts = u64::from_le_bytes(buf[16..24].try_into().ok()?);
        let mut cursor = Cursor::new(&buf[WAL_ENTRY_HEADER_LEN..size - 4]);
        let count = cursor.i32()?;
        if count < 0 {
            return None;
        }
        let mut cmds = Vec::with_capacity(count as usize);
        for _ in 0..count {
            cmds.push(decode_cmd(&mut cursor)?);
        }
        Some((
            Self {
                txn_id,
                commit_ts,
                cmds,
            },
            size,
        ))
    }
}

/// C++: `WalEntryIterator` (`wal_entry.cppm:927`). Forward iteration starts at
/// offset 0; backward iteration starts at end-of-file and steps over the
/// trailing size copies.
#[derive(Debug)]
pub struct WalEntryIterator {
    file_name: PathBuf,
    is_backward: bool,
    off: usize,
    buf: Vec<u8>,
}

impl WalEntryIterator {
    /// C++: `WalEntryIterator::Make` (`wal_entry_impl.cpp:2433`).
    pub fn make(wal_path: impl AsRef<Path>, is_backward: bool) -> Result<Self> {
        let file_name = wal_path.as_ref().to_path_buf();
        let buf = std::fs::read(&file_name).map_err(|e| Error::io(&file_name, e))?;
        let off = if is_backward { buf.len() } else { 0 };
        Ok(Self {
            file_name,
            is_backward,
            off,
            buf,
        })
    }

    pub fn file_name(&self) -> &Path {
        &self.file_name
    }

    /// C++: `WalEntryIterator::HasNext` (`wal_entry.cppm:938`).
    pub fn has_next(&self) -> bool {
        if self.is_backward {
            self.off > 0
        } else {
            self.off < self.buf.len()
        }
    }

    /// C++: `WalEntryIterator::GetOffset` (`wal_entry.cppm:942`).
    pub fn get_offset(&self) -> usize {
        self.off
    }

    /// C++: `WalEntryIterator::Next` (`wal_entry_impl.cpp:2447`). Returns
    /// `None` without advancing when the next frame is torn or corrupt, so
    /// [`Self::get_all_entries`] can stop there.
    #[allow(clippy::should_implement_trait)] // the C++ name is `Next`
    pub fn next(&mut self) -> Option<WalEntry> {
        if self.is_backward {
            if self.off < 4 {
                return None;
            }
            let entry_size =
                i32::from_le_bytes(self.buf[self.off - 4..self.off].try_into().ok()?) as i64;
            if entry_size <= 0 || entry_size as usize > self.off {
                return None;
            }
            let entry_size = entry_size as usize;
            let (entry, _) = WalEntry::read_adv(&self.buf[self.off - entry_size..])?;
            self.off -= entry_size;
            Some(entry)
        } else {
            if self.off + 4 > self.buf.len() {
                return None;
            }
            let entry_size =
                i32::from_le_bytes(self.buf[self.off..self.off + 4].try_into().ok()?) as i64;
            if entry_size <= 0 || self.off + entry_size as usize > self.buf.len() {
                return None;
            }
            let entry_size = entry_size as usize;
            let (entry, _) = WalEntry::read_adv(&self.buf[self.off..self.off + entry_size])?;
            self.off += entry_size;
            Some(entry)
        }
    }

    /// C++: `WalEntryIterator::GetAllEntries` (`wal_entry_impl.cpp:2500`).
    /// Backward iteration returns the entries in file order.
    pub fn get_all_entries(&mut self) -> Vec<WalEntry> {
        let mut entries = Vec::new();
        while self.has_next() {
            let Some(entry) = self.next() else {
                break;
            };
            entries.push(entry);
        }
        if self.is_backward {
            entries.reverse();
        }
        entries
    }

    /// C++: `WalEntryIterator::IsGood` (`wal_entry_impl.cpp:2518`): true when
    /// the whole file was consumed.
    pub fn is_good(&self) -> bool {
        if self.is_backward {
            self.off == 0
        } else {
            self.off == self.buf.len()
        }
    }
}

/// C++: `WalListIterator` (`wal_entry.cppm:957`). Walks the WAL files from the
/// newest to the oldest, after [`Self::new`] has purged every file whose tail
/// is damaged past the latest reachable checkpoint.
#[derive(Debug)]
pub struct WalListIterator {
    wal_list: VecDeque<PathBuf>,
    iter: Option<WalEntryIterator>,
}

impl WalListIterator {
    /// C++: `WalListIterator::WalListIterator` (`wal_entry_impl.cpp:2520`).
    pub fn new(wal_list: &[PathBuf]) -> Result<Self> {
        let mut iterator = Self {
            wal_list: wal_list.iter().cloned().collect(),
            iter: None,
        };
        iterator.purge_bad_entries_after_latest_checkpoint();
        if let Some(front) = iterator.wal_list.front() {
            iterator.iter = Some(WalEntryIterator::make(front, true)?);
        }
        Ok(iterator)
    }

    /// C++: `WalListIterator::PurgeBadEntriesAfterLatestCheckpoint`
    /// (`wal_entry_impl.cpp:2532`).
    fn purge_bad_entries_after_latest_checkpoint(&mut self) {
        let mut found_checkpoint = false;
        let mut file_num = 0usize;
        let mut cur = 0usize;

        while !found_checkpoint && cur < self.wal_list.len() {
            let path = self.wal_list[cur].clone();
            let mut bad_offset: Option<usize> = None;

            let mut iter = match WalEntryIterator::make(&path, false) {
                Ok(iter) => iter,
                Err(_) => {
                    // A file that cannot be read holds nothing.
                    let _ = crate::virtual_store::VirtualStore::delete_file(&path);
                    self.wal_list.drain(..=cur);
                    cur = 0;
                    file_num = 0;
                    continue;
                }
            };
            while iter.has_next() {
                match iter.next() {
                    Some(entry) => {
                        if entry.is_checkpoint() {
                            found_checkpoint = true;
                        }
                    }
                    None => {
                        bad_offset = Some(iter.get_offset());
                        break;
                    }
                }
            }

            let Some(bad_offset) = bad_offset else {
                cur += 1;
                file_num += 1;
                continue;
            };

            // Damage in front of a checkpoint is covered by that checkpoint:
            // peek backward, which walks the intact tail first.
            let mut ignore_bad_entry = false;
            if let Ok(mut backward_iter) = WalEntryIterator::make(&path, true) {
                while backward_iter.has_next() {
                    match backward_iter.next() {
                        Some(entry) => {
                            if entry.is_checkpoint() {
                                ignore_bad_entry = backward_iter.get_offset() > bad_offset;
                                break;
                            }
                        }
                        None => break,
                    }
                }
            }
            if ignore_bad_entry {
                found_checkpoint = true;
                cur += 1;
                file_num += 1;
                continue;
            }

            if bad_offset == 0 {
                let _ = crate::virtual_store::VirtualStore::delete_file(&path);
                let remove = (file_num + 1).min(self.wal_list.len());
                self.wal_list.drain(..remove);
                cur = 0;
                file_num = 0;
            } else {
                let _ = crate::virtual_store::VirtualStore::truncate(&path, bad_offset as u64);
                let remove = file_num.min(self.wal_list.len());
                self.wal_list.drain(..remove);
                cur = 1;
                file_num = 1;
            }
        }
        self.iter = None;
    }

    /// C++: `WalListIterator::HasNext` (`wal_entry_impl.cpp:2615`).
    pub fn has_next(&mut self) -> bool {
        let Some(iter) = self.iter.as_ref() else {
            return false;
        };
        if iter.has_next() {
            return true;
        }
        loop {
            if self.wal_list.len() <= 1 {
                return false;
            }
            self.wal_list.pop_front();
            let front = self.wal_list.front().expect("non-empty").clone();
            match WalEntryIterator::make(front, true) {
                Ok(next_iter) => {
                    let has = next_iter.has_next();
                    self.iter = Some(next_iter);
                    if has {
                        return true;
                    }
                }
                Err(_) => {
                    self.iter = None;
                    return false;
                }
            }
        }
    }

    /// C++: `WalListIterator::Next` (`wal_entry_impl.cpp:2634`).
    #[allow(clippy::should_implement_trait)] // the C++ name is `Next`
    pub fn next(&mut self) -> Option<WalEntry> {
        self.iter.as_mut().and_then(WalEntryIterator::next)
    }

    #[cfg(test)]
    fn files(&self) -> &VecDeque<PathBuf> {
        &self.wal_list
    }
}

// ---------------------------------------------------------------------------
// Serialization primitives
// ---------------------------------------------------------------------------

trait Sink {
    fn put(&mut self, bytes: &[u8]);
}

impl Sink for Vec<u8> {
    fn put(&mut self, bytes: &[u8]) {
        self.extend_from_slice(bytes);
    }
}

struct CountSink(usize);

impl Sink for CountSink {
    fn put(&mut self, bytes: &[u8]) {
        self.0 += bytes.len();
    }
}

fn put_i8<S: Sink>(sink: &mut S, v: i8) {
    sink.put(&v.to_le_bytes());
}
fn put_u8<S: Sink>(sink: &mut S, v: u8) {
    sink.put(&v.to_le_bytes());
}
fn put_u16<S: Sink>(sink: &mut S, v: u16) {
    sink.put(&v.to_le_bytes());
}
fn put_u32<S: Sink>(sink: &mut S, v: u32) {
    sink.put(&v.to_le_bytes());
}
fn put_i32<S: Sink>(sink: &mut S, v: i32) {
    sink.put(&v.to_le_bytes());
}
fn put_u64<S: Sink>(sink: &mut S, v: u64) {
    sink.put(&v.to_le_bytes());
}
fn put_i64<S: Sink>(sink: &mut S, v: i64) {
    sink.put(&v.to_le_bytes());
}
fn put_f32<S: Sink>(sink: &mut S, v: f32) {
    sink.put(&v.to_le_bytes());
}
fn put_f64<S: Sink>(sink: &mut S, v: f64) {
    sink.put(&v.to_le_bytes());
}
fn put_str<S: Sink>(sink: &mut S, v: &str) {
    put_i32(sink, v.len() as i32);
    sink.put(v.as_bytes());
}
fn put_len<S: Sink>(sink: &mut S, len: usize) {
    put_i32(sink, len as i32);
}

struct Cursor<'a> {
    buf: &'a [u8],
    pos: usize,
}

impl<'a> Cursor<'a> {
    fn new(buf: &'a [u8]) -> Self {
        Self { buf, pos: 0 }
    }

    fn take(&mut self, n: usize) -> Option<&'a [u8]> {
        let end = self.pos.checked_add(n)?;
        let slice = self.buf.get(self.pos..end)?;
        self.pos = end;
        Some(slice)
    }

    fn u8(&mut self) -> Option<u8> {
        Some(self.take(1)?[0])
    }
    fn i8(&mut self) -> Option<i8> {
        Some(self.u8()? as i8)
    }
    fn u16(&mut self) -> Option<u16> {
        Some(u16::from_le_bytes(self.take(2)?.try_into().ok()?))
    }
    fn u32(&mut self) -> Option<u32> {
        Some(u32::from_le_bytes(self.take(4)?.try_into().ok()?))
    }
    fn i32(&mut self) -> Option<i32> {
        Some(i32::from_le_bytes(self.take(4)?.try_into().ok()?))
    }
    fn u64(&mut self) -> Option<u64> {
        Some(u64::from_le_bytes(self.take(8)?.try_into().ok()?))
    }
    fn i64(&mut self) -> Option<i64> {
        Some(i64::from_le_bytes(self.take(8)?.try_into().ok()?))
    }
    fn f32(&mut self) -> Option<f32> {
        Some(f32::from_le_bytes(self.take(4)?.try_into().ok()?))
    }
    fn f64(&mut self) -> Option<f64> {
        Some(f64::from_le_bytes(self.take(8)?.try_into().ok()?))
    }
    fn string(&mut self) -> Option<String> {
        let len = self.i32()?;
        if len < 0 {
            return None;
        }
        let bytes = self.take(len as usize)?;
        String::from_utf8(bytes.to_vec()).ok()
    }
    fn len(&mut self) -> Option<usize> {
        let len = self.i32()?;
        if len < 0 {
            return None;
        }
        Some(len as usize)
    }
}

fn logical_type_code(t: LogicalType) -> (u8, u32) {
    match t {
        LogicalType::Invalid => (0, 0),
        LogicalType::Boolean => (1, 0),
        LogicalType::TinyInt => (2, 0),
        LogicalType::SmallInt => (3, 0),
        LogicalType::Integer => (4, 0),
        LogicalType::BigInt => (5, 0),
        LogicalType::HugeInt => (6, 0),
        LogicalType::Float => (7, 0),
        LogicalType::Double => (8, 0),
        LogicalType::Decimal => (9, 0),
        LogicalType::Varchar => (10, 0),
        LogicalType::Date => (11, 0),
        LogicalType::Time => (12, 0),
        LogicalType::DateTime => (13, 0),
        LogicalType::Timestamp => (14, 0),
        LogicalType::Interval => (15, 0),
        LogicalType::Uuid => (16, 0),
        LogicalType::Array => (17, 0),
        LogicalType::Mixed => (18, 0),
        LogicalType::Embedding(dim) => (19, dim),
        LogicalType::Null => (20, 0),
        LogicalType::Tuple => (21, 0),
        LogicalType::RowID => (22, 0),
        LogicalType::Missing => (23, 0),
    }
}

fn logical_type_from(code: u8, dim: u32) -> Option<LogicalType> {
    Some(match code {
        0 => LogicalType::Invalid,
        1 => LogicalType::Boolean,
        2 => LogicalType::TinyInt,
        3 => LogicalType::SmallInt,
        4 => LogicalType::Integer,
        5 => LogicalType::BigInt,
        6 => LogicalType::HugeInt,
        7 => LogicalType::Float,
        8 => LogicalType::Double,
        9 => LogicalType::Decimal,
        10 => LogicalType::Varchar,
        11 => LogicalType::Date,
        12 => LogicalType::Time,
        13 => LogicalType::DateTime,
        14 => LogicalType::Timestamp,
        15 => LogicalType::Interval,
        16 => LogicalType::Uuid,
        17 => LogicalType::Array,
        18 => LogicalType::Mixed,
        19 => LogicalType::Embedding(dim),
        20 => LogicalType::Null,
        21 => LogicalType::Tuple,
        22 => LogicalType::RowID,
        23 => LogicalType::Missing,
        _ => return None,
    })
}

fn put_block_value<S: Sink>(sink: &mut S, value: &BlockValue) {
    match value {
        BlockValue::Null => put_u8(sink, 0),
        BlockValue::Bool(v) => {
            put_u8(sink, 1);
            put_u8(sink, u8::from(*v));
        }
        BlockValue::TinyInt(v) => {
            put_u8(sink, 2);
            put_i8(sink, *v);
        }
        BlockValue::SmallInt(v) => {
            put_u8(sink, 3);
            sink.put(&v.to_le_bytes());
        }
        BlockValue::Int32(v) => {
            put_u8(sink, 4);
            put_i32(sink, *v);
        }
        BlockValue::Int(v) => {
            put_u8(sink, 5);
            put_i64(sink, *v);
        }
        BlockValue::HugeInt(v) => {
            put_u8(sink, 6);
            sink.put(&v.to_le_bytes());
        }
        BlockValue::Float(v) => {
            put_u8(sink, 7);
            put_f32(sink, *v);
        }
        BlockValue::Double(v) => {
            put_u8(sink, 8);
            put_f64(sink, *v);
        }
        BlockValue::Decimal(v) => {
            put_u8(sink, 9);
            sink.put(&v.value.to_le_bytes());
            put_u32(sink, v.scale);
        }
        BlockValue::Str(v) => {
            put_u8(sink, 10);
            put_str(sink, v);
        }
        BlockValue::Date(v) => {
            put_u8(sink, 11);
            put_i32(sink, v.value);
        }
        BlockValue::Time(v) => {
            put_u8(sink, 12);
            put_i32(sink, v.value);
        }
        BlockValue::DateTime(v) => {
            put_u8(sink, 13);
            put_i32(sink, v.date);
            put_i32(sink, v.time);
        }
        BlockValue::Timestamp(v) => {
            put_u8(sink, 14);
            put_i32(sink, v.date);
            put_i32(sink, v.time);
        }
        BlockValue::Interval(v) => {
            put_u8(sink, 15);
            put_i32(sink, v.value);
        }
        BlockValue::Uuid(v) => {
            put_u8(sink, 16);
            sink.put(v);
        }
        BlockValue::Embedding(v) => {
            put_u8(sink, 17);
            put_len(sink, v.len());
            for x in v {
                put_f32(sink, *x);
            }
        }
        BlockValue::Array(v) => {
            put_u8(sink, 18);
            put_len(sink, v.len());
            for x in v {
                put_i64(sink, *x);
            }
        }
        BlockValue::Mixed(v) => {
            put_u8(sink, 19);
            put_str(sink, &v.text());
        }
    }
}

fn read_block_value(cursor: &mut Cursor<'_>) -> Option<BlockValue> {
    Some(match cursor.u8()? {
        0 => BlockValue::Null,
        1 => BlockValue::Bool(cursor.u8()? != 0),
        2 => BlockValue::TinyInt(cursor.i8()?),
        3 => BlockValue::SmallInt(cursor.u16()? as i16),
        4 => BlockValue::Int32(cursor.i32()?),
        5 => BlockValue::Int(cursor.i64()?),
        6 => BlockValue::HugeInt(i128::from_le_bytes(cursor.take(16)?.try_into().ok()?)),
        7 => BlockValue::Float(cursor.f32()?),
        8 => BlockValue::Double(cursor.f64()?),
        9 => BlockValue::Decimal(infinity_common::DecimalT::new(
            i128::from_le_bytes(cursor.take(16)?.try_into().ok()?),
            cursor.u32()?,
        )),
        10 => BlockValue::Str(cursor.string()?),
        11 => BlockValue::Date(infinity_common::DateT::new(cursor.i32()?)),
        12 => BlockValue::Time(infinity_common::TimeT::new(cursor.i32()?)),
        13 => BlockValue::DateTime(infinity_common::DateTimeT::new(
            cursor.i32()?,
            cursor.i32()?,
        )),
        14 => BlockValue::Timestamp(infinity_common::TimestampT::new(
            cursor.i32()?,
            cursor.i32()?,
        )),
        15 => BlockValue::Interval(infinity_common::IntervalT::new(cursor.i32()?)),
        16 => BlockValue::Uuid(cursor.take(16)?.try_into().ok()?),
        17 => {
            let n = cursor.len()?;
            let mut v = Vec::with_capacity(n);
            for _ in 0..n {
                v.push(cursor.f32()?);
            }
            BlockValue::Embedding(v)
        }
        18 => {
            let n = cursor.len()?;
            let mut v = Vec::with_capacity(n);
            for _ in 0..n {
                v.push(cursor.i64()?);
            }
            BlockValue::Array(v)
        }
        19 => BlockValue::Mixed(infinity_common::MixedT::new(cursor.string()?)),
        _ => return None,
    })
}

fn empty_column(logical_type: LogicalType) -> Option<Column> {
    Some(match logical_type {
        LogicalType::Boolean => Column::Bool(ColumnVector::new()),
        LogicalType::TinyInt => Column::TinyInt(ColumnVector::new()),
        LogicalType::SmallInt => Column::SmallInt(ColumnVector::new()),
        LogicalType::Integer => Column::Int32(ColumnVector::new()),
        LogicalType::BigInt => Column::Int(ColumnVector::new()),
        LogicalType::HugeInt => Column::HugeInt(ColumnVector::new()),
        LogicalType::Float => Column::Float32(ColumnVector::new()),
        LogicalType::Double => Column::Float(ColumnVector::new()),
        LogicalType::Decimal => Column::Decimal(ColumnVector::new()),
        LogicalType::Varchar => Column::Str(ColumnVector::new()),
        LogicalType::Date => Column::Date(ColumnVector::new()),
        LogicalType::Time => Column::Time(ColumnVector::new()),
        LogicalType::DateTime => Column::DateTime(ColumnVector::new()),
        LogicalType::Timestamp => Column::Timestamp(ColumnVector::new()),
        LogicalType::Interval => Column::Interval(ColumnVector::new()),
        LogicalType::Uuid => Column::Uuid(ColumnVector::new()),
        LogicalType::Array => Column::Array(ColumnVector::new()),
        LogicalType::Mixed => Column::Mixed(ColumnVector::new()),
        LogicalType::Embedding(_) => Column::Embedding(ColumnVector::new()),
        _ => return None,
    })
}

fn put_data_block<S: Sink>(sink: &mut S, block: &DataBlock) {
    put_len(sink, block.column_count());
    put_len(sink, block.row_count());
    for (idx, column) in block.columns().iter().enumerate() {
        let (code, dim) = logical_type_code(column.logical_type());
        put_u8(sink, code);
        if code == 19 {
            put_u32(sink, dim);
        }
        for row in 0..block.row_count() {
            match block.get_value(idx, row) {
                Some(value) => {
                    put_u8(sink, 1);
                    put_block_value(sink, &value);
                }
                None => put_u8(sink, 0),
            }
        }
    }
}

fn read_data_block(cursor: &mut Cursor<'_>) -> Option<DataBlock> {
    let column_count = cursor.len()?;
    let row_count = cursor.len()?;
    let mut columns = Vec::with_capacity(column_count);
    let mut values: Vec<Vec<BlockValue>> = Vec::with_capacity(column_count);
    for _ in 0..column_count {
        let code = cursor.u8()?;
        let dim = if code == 19 { cursor.u32()? } else { 0 };
        let logical_type = logical_type_from(code, dim)?;
        columns.push(empty_column(logical_type)?);
        let mut column_values = Vec::with_capacity(row_count);
        for _ in 0..row_count {
            column_values.push(if cursor.u8()? == 0 {
                BlockValue::Null
            } else {
                read_block_value(cursor)?
            });
        }
        values.push(column_values);
    }
    let mut block = DataBlock::new(columns, row_count);
    for (column, column_values) in values.into_iter().enumerate() {
        for value in column_values {
            block.append_value(column, value).ok()?;
        }
    }
    block.finalize();
    Some(block)
}

fn encode_segment_info<S: Sink>(sink: &mut S, info: &WalSegmentInfo) {
    put_u32(sink, info.segment_id);
    put_u64(sink, info.column_count);
    put_u64(sink, info.row_count);
    put_u64(sink, info.actual_row_count);
    put_u64(sink, info.row_capacity);
    put_len(sink, info.block_infos.len());
    for block in &info.block_infos {
        put_u32(sink, block.block_id);
        put_u16(sink, block.row_count);
        put_u16(sink, block.row_capacity);
        put_len(sink, block.outline_infos.len());
        for (count, offset) in &block.outline_infos {
            put_u32(sink, *count);
            put_u64(sink, *offset);
        }
    }
}

fn decode_segment_info(cursor: &mut Cursor<'_>) -> Option<WalSegmentInfo> {
    let segment_id = cursor.u32()?;
    let column_count = cursor.u64()?;
    let row_count = cursor.u64()?;
    let actual_row_count = cursor.u64()?;
    let row_capacity = cursor.u64()?;
    let count = cursor.len()?;
    let mut block_infos = Vec::with_capacity(count);
    for _ in 0..count {
        let block_id = cursor.u32()?;
        let row_count = cursor.u16()?;
        let row_capacity = cursor.u16()?;
        let outline_count = cursor.len()?;
        let mut outline_infos = Vec::with_capacity(outline_count);
        for _ in 0..outline_count {
            outline_infos.push((cursor.u32()?, cursor.u64()?));
        }
        block_infos.push(WalBlockInfo {
            block_id,
            row_count,
            row_capacity,
            outline_infos,
        });
    }
    Some(WalSegmentInfo {
        segment_id,
        column_count,
        row_count,
        actual_row_count,
        row_capacity,
        block_infos,
    })
}

fn encode_chunk_info<S: Sink>(sink: &mut S, info: &WalChunkIndexInfo) {
    put_u32(sink, info.chunk_id);
    put_str(sink, &info.base_name);
    put_u32(sink, info.base_row_id.segment_id);
    put_u32(sink, info.base_row_id.segment_offset);
    put_u32(sink, info.row_count);
    put_u32(sink, info.term_count);
    put_u32(sink, info.index_size);
    put_u64(sink, info.deprecate_ts);
}

fn decode_chunk_info(cursor: &mut Cursor<'_>) -> Option<WalChunkIndexInfo> {
    Some(WalChunkIndexInfo {
        chunk_id: cursor.u32()?,
        base_name: cursor.string()?,
        base_row_id: WalRowId::new(cursor.u32()?, cursor.u32()?),
        row_count: cursor.u32()?,
        term_count: cursor.u32()?,
        index_size: cursor.u32()?,
        deprecate_ts: cursor.u64()?,
    })
}

fn encode_column_def<S: Sink>(sink: &mut S, def: &WalColumnDef) {
    put_u32(sink, def.column_id);
    put_str(sink, &def.name);
    let (code, dim) = logical_type_code(def.logical_type);
    put_u8(sink, code);
    if code == 19 {
        put_u32(sink, dim);
    }
    put_len(sink, def.constraints.len());
    for constraint in &def.constraints {
        put_u8(sink, *constraint as u8);
    }
}

fn decode_column_def(cursor: &mut Cursor<'_>) -> Option<WalColumnDef> {
    let column_id = cursor.u32()?;
    let name = cursor.string()?;
    let code = cursor.u8()?;
    let dim = if code == 19 { cursor.u32()? } else { 0 };
    let logical_type = logical_type_from(code, dim)?;
    let count = cursor.len()?;
    let mut constraints = Vec::with_capacity(count);
    for _ in 0..count {
        constraints.push(match cursor.u8()? {
            1 => ConstraintType::PrimaryKey,
            2 => ConstraintType::NotNull,
            3 => ConstraintType::Unique,
            _ => ConstraintType::Invalid,
        });
    }
    Some(WalColumnDef {
        column_id,
        name,
        logical_type,
        constraints,
    })
}

fn encode_table_def<S: Sink>(sink: &mut S, def: &WalTableDef) {
    put_str(sink, &def.db_name);
    put_str(sink, &def.table_name);
    put_str(sink, &def.table_id);
    put_str(sink, &def.comment);
    put_len(sink, def.columns.len());
    for column in &def.columns {
        encode_column_def(sink, column);
    }
}

fn decode_table_def(cursor: &mut Cursor<'_>) -> Option<WalTableDef> {
    let db_name = cursor.string()?;
    let table_name = cursor.string()?;
    let table_id = cursor.string()?;
    let comment = cursor.string()?;
    let count = cursor.len()?;
    let mut columns = Vec::with_capacity(count);
    for _ in 0..count {
        columns.push(decode_column_def(cursor)?);
    }
    Some(WalTableDef {
        db_name,
        table_name,
        table_id,
        comment,
        columns,
    })
}

fn encode_index_base<S: Sink>(sink: &mut S, base: &WalIndexBase) {
    put_str(sink, &base.index_name);
    put_str(sink, &base.comment);
    put_str(sink, &base.index_id);
    put_len(sink, base.column_names.len());
    for name in &base.column_names {
        put_str(sink, name);
    }
    put_str(sink, &base.index_type);
    put_len(sink, base.params.len());
    for (key, value) in &base.params {
        put_str(sink, key);
        put_str(sink, value);
    }
}

fn decode_index_base(cursor: &mut Cursor<'_>) -> Option<WalIndexBase> {
    let index_name = cursor.string()?;
    let comment = cursor.string()?;
    let index_id = cursor.string()?;
    let column_count = cursor.len()?;
    let mut column_names = Vec::with_capacity(column_count);
    for _ in 0..column_count {
        column_names.push(cursor.string()?);
    }
    let index_type = cursor.string()?;
    let param_count = cursor.len()?;
    let mut params = Vec::with_capacity(param_count);
    for _ in 0..param_count {
        params.push((cursor.string()?, cursor.string()?));
    }
    Some(WalIndexBase {
        index_name,
        comment,
        index_id,
        column_names,
        index_type,
        params,
    })
}

fn encode_cmd<S: Sink>(sink: &mut S, cmd: &WalCmd) {
    put_i8(sink, cmd.command_type() as i8);
    match cmd {
        WalCmd::CreateDatabase {
            db_name,
            db_id,
            db_comment,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, db_comment);
        }
        WalCmd::DropDatabase {
            db_name,
            db_id,
            create_ts,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_u64(sink, *create_ts);
        }
        WalCmd::CreateTable {
            db_name,
            db_id,
            table_id,
            table_def,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_id);
            encode_table_def(sink, table_def);
        }
        WalCmd::DropTable {
            db_name,
            db_id,
            table_name,
            table_id,
            create_ts,
            table_key,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_u64(sink, *create_ts);
            put_str(sink, table_key);
        }
        WalCmd::CreateIndex {
            db_name,
            db_id,
            table_name,
            table_id,
            index_id,
            index_base,
            segment_index_infos,
            table_key,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_str(sink, index_id);
            encode_index_base(sink, index_base);
            put_len(sink, segment_index_infos.len());
            for info in segment_index_infos {
                put_u32(sink, info.segment_id);
                put_len(sink, info.chunk_infos.len());
                for chunk in &info.chunk_infos {
                    encode_chunk_info(sink, chunk);
                }
            }
            put_str(sink, table_key);
        }
        WalCmd::DropIndex {
            db_name,
            db_id,
            table_name,
            table_id,
            index_name,
            index_id,
            create_ts,
            index_key,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_str(sink, index_name);
            put_str(sink, index_id);
            put_u64(sink, *create_ts);
            put_str(sink, index_key);
        }
        WalCmd::Import {
            db_name,
            db_id,
            table_name,
            table_id,
            segment_info,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            encode_segment_info(sink, segment_info);
        }
        WalCmd::Append {
            db_name,
            db_id,
            table_name,
            table_id,
            row_ranges,
            block,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_len(sink, row_ranges.len());
            for (row_id, count) in row_ranges {
                put_u32(sink, row_id.segment_id);
                put_u32(sink, row_id.segment_offset);
                put_u64(sink, *count);
            }
            put_data_block(sink, block);
        }
        WalCmd::Delete {
            db_name,
            db_id,
            table_name,
            table_id,
            row_ids,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_len(sink, row_ids.len());
            for row_id in row_ids {
                put_u32(sink, row_id.segment_id);
                put_u32(sink, row_id.segment_offset);
            }
        }
        WalCmd::Checkpoint { max_commit_ts } => put_u64(sink, *max_commit_ts),
        WalCmd::Compact {
            db_name,
            db_id,
            table_name,
            table_id,
            index_names,
            index_ids,
            new_segment_infos,
            deprecated_segment_ids,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_len(sink, index_names.len());
            for name in index_names {
                put_str(sink, name);
            }
            put_len(sink, index_ids.len());
            for id in index_ids {
                put_str(sink, id);
            }
            put_len(sink, new_segment_infos.len());
            for info in new_segment_infos {
                encode_segment_info(sink, info);
            }
            put_len(sink, deprecated_segment_ids.len());
            for id in deprecated_segment_ids {
                put_u32(sink, *id);
            }
        }
        WalCmd::DumpIndex {
            db_name,
            db_id,
            table_name,
            table_id,
            index_name,
            index_id,
            segment_id,
            chunk_infos,
            deprecate_ids,
            table_key,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_str(sink, index_name);
            put_str(sink, index_id);
            put_u32(sink, *segment_id);
            put_len(sink, chunk_infos.len());
            for chunk in chunk_infos {
                encode_chunk_info(sink, chunk);
            }
            put_len(sink, deprecate_ids.len());
            for id in deprecate_ids {
                put_u32(sink, *id);
            }
            put_str(sink, table_key);
        }
        WalCmd::RenameTable {
            db_name,
            db_id,
            table_name,
            table_id,
            new_table_name,
            old_table_key,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_str(sink, new_table_name);
            put_str(sink, old_table_key);
        }
        WalCmd::AddColumns {
            db_name,
            db_id,
            table_name,
            table_id,
            column_idx_list,
            column_defs,
            table_key,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_len(sink, column_idx_list.len());
            for idx in column_idx_list {
                put_u32(sink, *idx);
            }
            put_len(sink, column_defs.len());
            for def in column_defs {
                encode_column_def(sink, def);
            }
            put_str(sink, table_key);
        }
        WalCmd::DropColumns {
            db_name,
            db_id,
            table_name,
            table_id,
            column_names,
            column_ids,
            table_key,
            column_keys,
        } => {
            put_str(sink, db_name);
            put_str(sink, db_id);
            put_str(sink, table_name);
            put_str(sink, table_id);
            put_len(sink, column_names.len());
            for name in column_names {
                put_str(sink, name);
            }
            put_len(sink, column_ids.len());
            for id in column_ids {
                put_u32(sink, *id);
            }
            put_str(sink, table_key);
            put_len(sink, column_keys.len());
            for key in column_keys {
                put_str(sink, key);
            }
        }
        WalCmd::Cleanup { timestamp } => put_u64(sink, *timestamp),
    }
}

fn decode_cmd(cursor: &mut Cursor<'_>) -> Option<WalCmd> {
    let command_type = WalCommandType::from_i8(cursor.i8()?)?;
    Some(match command_type {
        WalCommandType::CreateDatabaseV2 => WalCmd::CreateDatabase {
            db_name: cursor.string()?,
            db_id: cursor.string()?,
            db_comment: cursor.string()?,
        },
        WalCommandType::DropDatabaseV2 => WalCmd::DropDatabase {
            db_name: cursor.string()?,
            db_id: cursor.string()?,
            create_ts: cursor.u64()?,
        },
        WalCommandType::CreateTableV2 => WalCmd::CreateTable {
            db_name: cursor.string()?,
            db_id: cursor.string()?,
            table_id: cursor.string()?,
            table_def: decode_table_def(cursor)?,
        },
        WalCommandType::DropTableV2 => WalCmd::DropTable {
            db_name: cursor.string()?,
            db_id: cursor.string()?,
            table_name: cursor.string()?,
            table_id: cursor.string()?,
            create_ts: cursor.u64()?,
            table_key: cursor.string()?,
        },
        WalCommandType::CreateIndexV2 => {
            let db_name = cursor.string()?;
            let db_id = cursor.string()?;
            let table_name = cursor.string()?;
            let table_id = cursor.string()?;
            let index_id = cursor.string()?;
            let index_base = decode_index_base(cursor)?;
            let segment_count = cursor.len()?;
            let mut segment_index_infos = Vec::with_capacity(segment_count);
            for _ in 0..segment_count {
                let segment_id = cursor.u32()?;
                let chunk_count = cursor.len()?;
                let mut chunk_infos = Vec::with_capacity(chunk_count);
                for _ in 0..chunk_count {
                    chunk_infos.push(decode_chunk_info(cursor)?);
                }
                segment_index_infos.push(WalSegmentIndexInfo {
                    segment_id,
                    chunk_infos,
                });
            }
            WalCmd::CreateIndex {
                db_name,
                db_id,
                table_name,
                table_id,
                index_id,
                index_base,
                segment_index_infos,
                table_key: cursor.string()?,
            }
        }
        WalCommandType::DropIndexV2 => WalCmd::DropIndex {
            db_name: cursor.string()?,
            db_id: cursor.string()?,
            table_name: cursor.string()?,
            table_id: cursor.string()?,
            index_name: cursor.string()?,
            index_id: cursor.string()?,
            create_ts: cursor.u64()?,
            index_key: cursor.string()?,
        },
        WalCommandType::ImportV2 => WalCmd::Import {
            db_name: cursor.string()?,
            db_id: cursor.string()?,
            table_name: cursor.string()?,
            table_id: cursor.string()?,
            segment_info: decode_segment_info(cursor)?,
        },
        WalCommandType::AppendV2 => {
            let db_name = cursor.string()?;
            let db_id = cursor.string()?;
            let table_name = cursor.string()?;
            let table_id = cursor.string()?;
            let range_count = cursor.len()?;
            let mut row_ranges = Vec::with_capacity(range_count);
            for _ in 0..range_count {
                row_ranges.push((WalRowId::new(cursor.u32()?, cursor.u32()?), cursor.u64()?));
            }
            WalCmd::Append {
                db_name,
                db_id,
                table_name,
                table_id,
                row_ranges,
                block: read_data_block(cursor)?,
            }
        }
        WalCommandType::DeleteV2 => {
            let db_name = cursor.string()?;
            let db_id = cursor.string()?;
            let table_name = cursor.string()?;
            let table_id = cursor.string()?;
            let count = cursor.len()?;
            let mut row_ids = Vec::with_capacity(count);
            for _ in 0..count {
                row_ids.push(WalRowId::new(cursor.u32()?, cursor.u32()?));
            }
            WalCmd::Delete {
                db_name,
                db_id,
                table_name,
                table_id,
                row_ids,
            }
        }
        WalCommandType::CheckpointV2 => WalCmd::Checkpoint {
            max_commit_ts: cursor.u64()?,
        },
        WalCommandType::CompactV2 => {
            let db_name = cursor.string()?;
            let db_id = cursor.string()?;
            let table_name = cursor.string()?;
            let table_id = cursor.string()?;
            let name_count = cursor.len()?;
            let mut index_names = Vec::with_capacity(name_count);
            for _ in 0..name_count {
                index_names.push(cursor.string()?);
            }
            let id_count = cursor.len()?;
            let mut index_ids = Vec::with_capacity(id_count);
            for _ in 0..id_count {
                index_ids.push(cursor.string()?);
            }
            let segment_count = cursor.len()?;
            let mut new_segment_infos = Vec::with_capacity(segment_count);
            for _ in 0..segment_count {
                new_segment_infos.push(decode_segment_info(cursor)?);
            }
            let deprecated_count = cursor.len()?;
            let mut deprecated_segment_ids = Vec::with_capacity(deprecated_count);
            for _ in 0..deprecated_count {
                deprecated_segment_ids.push(cursor.u32()?);
            }
            WalCmd::Compact {
                db_name,
                db_id,
                table_name,
                table_id,
                index_names,
                index_ids,
                new_segment_infos,
                deprecated_segment_ids,
            }
        }
        WalCommandType::DumpIndexV2 => {
            let db_name = cursor.string()?;
            let db_id = cursor.string()?;
            let table_name = cursor.string()?;
            let table_id = cursor.string()?;
            let index_name = cursor.string()?;
            let index_id = cursor.string()?;
            let segment_id = cursor.u32()?;
            let chunk_count = cursor.len()?;
            let mut chunk_infos = Vec::with_capacity(chunk_count);
            for _ in 0..chunk_count {
                chunk_infos.push(decode_chunk_info(cursor)?);
            }
            let deprecate_count = cursor.len()?;
            let mut deprecate_ids = Vec::with_capacity(deprecate_count);
            for _ in 0..deprecate_count {
                deprecate_ids.push(cursor.u32()?);
            }
            WalCmd::DumpIndex {
                db_name,
                db_id,
                table_name,
                table_id,
                index_name,
                index_id,
                segment_id,
                chunk_infos,
                deprecate_ids,
                table_key: cursor.string()?,
            }
        }
        WalCommandType::RenameTableV2 => WalCmd::RenameTable {
            db_name: cursor.string()?,
            db_id: cursor.string()?,
            table_name: cursor.string()?,
            table_id: cursor.string()?,
            new_table_name: cursor.string()?,
            old_table_key: cursor.string()?,
        },
        WalCommandType::AddColumnsV2 => {
            let db_name = cursor.string()?;
            let db_id = cursor.string()?;
            let table_name = cursor.string()?;
            let table_id = cursor.string()?;
            let idx_count = cursor.len()?;
            let mut column_idx_list = Vec::with_capacity(idx_count);
            for _ in 0..idx_count {
                column_idx_list.push(cursor.u32()?);
            }
            let def_count = cursor.len()?;
            let mut column_defs = Vec::with_capacity(def_count);
            for _ in 0..def_count {
                column_defs.push(decode_column_def(cursor)?);
            }
            WalCmd::AddColumns {
                db_name,
                db_id,
                table_name,
                table_id,
                column_idx_list,
                column_defs,
                table_key: cursor.string()?,
            }
        }
        WalCommandType::DropColumnsV2 => {
            let db_name = cursor.string()?;
            let db_id = cursor.string()?;
            let table_name = cursor.string()?;
            let table_id = cursor.string()?;
            let name_count = cursor.len()?;
            let mut column_names = Vec::with_capacity(name_count);
            for _ in 0..name_count {
                column_names.push(cursor.string()?);
            }
            let id_count = cursor.len()?;
            let mut column_ids = Vec::with_capacity(id_count);
            for _ in 0..id_count {
                column_ids.push(cursor.u32()?);
            }
            let table_key = cursor.string()?;
            let key_count = cursor.len()?;
            let mut column_keys = Vec::with_capacity(key_count);
            for _ in 0..key_count {
                column_keys.push(cursor.string()?);
            }
            WalCmd::DropColumns {
                db_name,
                db_id,
                table_name,
                table_id,
                column_names,
                column_ids,
                table_key,
                column_keys,
            }
        }
        WalCommandType::Cleanup => WalCmd::Cleanup {
            timestamp: cursor.u64()?,
        },
        _ => return None,
    })
}

/// Appends `entries` to the WAL file at `path` (test/replay helper mirroring
/// `MockWalFile`).
pub fn append_entries(path: impl AsRef<Path>, entries: &[WalEntry]) -> Result<()> {
    let path = path.as_ref();
    let mut file = OpenOptions::new()
        .create(true)
        .append(true)
        .open(path)
        .map_err(|e| Error::io(path, e))?;
    for entry in entries {
        let expected = entry.size_in_bytes();
        let mut buf = Vec::with_capacity(expected);
        entry.write_adv(&mut buf);
        debug_assert_eq!(buf.len(), expected);
        file.write_all(&buf).map_err(|e| Error::io(path, e))?;
    }
    file.sync_all().map_err(|e| Error::io(path, e))
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    const DEFAULT_VECTOR_SIZE: usize = 8192;

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:48` (`MockTableDesc2`).
    fn mock_table_desc2() -> WalTableDef {
        WalTableDef::new(
            "default_db",
            "tbl1",
            "",
            "",
            vec![
                WalColumnDef {
                    column_id: 0,
                    name: "tiny_int_col".to_string(),
                    logical_type: LogicalType::TinyInt,
                    constraints: vec![ConstraintType::Unique, ConstraintType::NotNull],
                },
                WalColumnDef {
                    column_id: 1,
                    name: "big_int_col".to_string(),
                    logical_type: LogicalType::BigInt,
                    constraints: vec![ConstraintType::PrimaryKey],
                },
                WalColumnDef {
                    column_id: 2,
                    name: "double_col".to_string(),
                    logical_type: LogicalType::Double,
                    constraints: vec![ConstraintType::NotNull],
                },
            ],
        )
    }

    /// C++: `wal_entry_ut.cpp:83` (`MakeSegmentInfo`). Note the reference
    /// builds a local `block_infos_` vector and never assigns it, so the
    /// returned `block_infos_` stays empty; mirror that.
    fn make_segment_info(
        row_count: usize,
        _commit_ts: TxnTimeStamp,
        column_count: usize,
    ) -> WalSegmentInfo {
        WalSegmentInfo {
            segment_id: 0,
            column_count: column_count as u64,
            row_count: row_count as u64,
            actual_row_count: row_count as u64,
            row_capacity: (row_count * 2) as u64,
            block_infos: Vec::new(),
        }
    }

    fn mock_data_block(row_count: usize) -> DataBlock {
        let mut block = DataBlock::new(
            vec![
                Column::Bool(ColumnVector::new()),
                Column::TinyInt(ColumnVector::new()),
            ],
            row_count,
        );
        for i in 0..row_count {
            block.append_value(0, BlockValue::Bool(i % 2 == 0)).unwrap();
            block.append_value(1, BlockValue::TinyInt(i as i8)).unwrap();
        }
        block.finalize();
        block
    }

    fn data_entry(commit_ts: TxnTimeStamp) -> WalEntry {
        let row_count = DEFAULT_VECTOR_SIZE;
        WalEntry::new(
            0,
            commit_ts,
            vec![
                WalCmd::CreateDatabase {
                    db_name: "default2".to_string(),
                    db_id: "1".to_string(),
                    db_comment: "default2_comment".to_string(),
                },
                WalCmd::CreateTable {
                    db_name: "default_db".to_string(),
                    db_id: "0".to_string(),
                    table_id: "1".to_string(),
                    table_def: mock_table_desc2(),
                },
                WalCmd::Import {
                    db_name: "default_db".to_string(),
                    db_id: "0".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "1".to_string(),
                    segment_info: make_segment_info(row_count, commit_ts, 2),
                },
                WalCmd::Append {
                    db_name: "db1".to_string(),
                    db_id: "2".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "1".to_string(),
                    row_ranges: vec![(WalRowId::new(0, 0), 8192)],
                    block: mock_data_block(row_count),
                },
            ],
        )
    }

    /// C++: `wal_entry_ut.cpp:103` (`MockWalFile`).
    fn mock_wal_file(path: &Path) {
        for commit_ts in 0..3u64 {
            append_entries(path, &[data_entry(commit_ts)]).unwrap();
        }
        append_entries(
            path,
            &[WalEntry::new(
                0,
                5,
                vec![WalCmd::Compact {
                    db_name: "db1".to_string(),
                    db_id: "2".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "1".to_string(),
                    index_names: Vec::new(),
                    index_ids: Vec::new(),
                    new_segment_infos: (0..3).map(|_| make_segment_info(1, 0, 2)).collect(),
                    deprecated_segment_ids: vec![0, 1, 2],
                }],
            )],
        )
        .unwrap();
        append_entries(
            path,
            &[WalEntry::new(
                0,
                3,
                vec![WalCmd::Checkpoint { max_commit_ts: 123 }],
            )],
        )
        .unwrap();
        append_entries(
            path,
            &[WalEntry::new(
                0,
                4,
                vec![WalCmd::DropTable {
                    db_name: "db1".to_string(),
                    db_id: "2".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "1".to_string(),
                    create_ts: 1,
                    table_key: "table_key".to_string(),
                }],
            )],
        )
        .unwrap();
    }

    fn wal_entry_offsets(path: &Path) -> Vec<usize> {
        let mut offsets = Vec::new();
        let mut iter = WalEntryIterator::make(path, false).unwrap();
        while iter.has_next() {
            offsets.push(iter.get_offset());
            if iter.next().is_none() {
                break;
            }
        }
        offsets
    }

    fn corrupt_entry(path: &Path, entry_offset: usize) {
        let offset = entry_offset + WAL_ENTRY_HEADER_LEN;
        let mut buf = std::fs::read(path).unwrap();
        buf[offset] ^= 0xFF;
        std::fs::write(path, buf).unwrap();
    }

    fn mock_wal_file_without_checkpoint(path: &Path) -> Vec<usize> {
        mock_wal_file(path);
        let offsets = wal_entry_offsets(path);
        assert!(
            offsets.len() >= 5,
            "mock WAL produced {} entries",
            offsets.len()
        );
        let file = OpenOptions::new().write(true).open(path).unwrap();
        file.set_len(offsets[4] as u64).unwrap();
        offsets[..4].to_vec()
    }

    fn mock_wal_pair(newer: &Path, older: &Path) -> Vec<usize> {
        mock_wal_file(older);
        mock_wal_file_without_checkpoint(newer)
    }

    fn replay_to_first_checkpoint(iterator: &mut WalListIterator) -> TxnTimeStamp {
        let mut max_commit_ts = 0;
        while iterator.has_next() {
            let Some(entry) = iterator.next() else {
                break;
            };
            if let Some(ts) = entry.checkpoint_ts() {
                max_commit_ts = ts;
                break;
            }
        }
        max_commit_ts
    }

    fn drain_wal_list(iterator: &mut WalListIterator) -> Vec<TxnTimeStamp> {
        let mut commit_tss = Vec::new();
        while iterator.has_next() {
            let Some(entry) = iterator.next() else {
                break;
            };
            commit_tss.push(entry.commit_ts);
        }
        commit_tss
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:212` (`ReadWriteV2`).
    #[test]
    fn read_write_v2_roundtrips_every_command() {
        let entry = WalEntry::new(
            0,
            0,
            vec![
                WalCmd::CreateDatabase {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    db_comment: "default2_comment".to_string(),
                },
                WalCmd::DropDatabase {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    create_ts: 1,
                },
                WalCmd::CreateTable {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_id: "2".to_string(),
                    table_def: mock_table_desc2(),
                },
                WalCmd::DropTable {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    create_ts: 1,
                    table_key: "table_key".to_string(),
                },
                WalCmd::Import {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    segment_info: make_segment_info(100, 8, 2),
                },
                WalCmd::CreateIndex {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    index_id: "3".to_string(),
                    index_base: WalIndexBase::new(
                        "idx1",
                        "test comment",
                        "idx1_tbl1",
                        vec!["col1".to_string(), "col2".to_string()],
                        "IVF",
                        vec![("metric".to_string(), "ip".to_string())],
                    ),
                    segment_index_infos: Vec::new(),
                    table_key: "table_key".to_string(),
                },
                WalCmd::DropIndex {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    index_name: "idx1".to_string(),
                    index_id: "3".to_string(),
                    create_ts: 1,
                    index_key: "index_key".to_string(),
                },
                WalCmd::Append {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    row_ranges: vec![(WalRowId::new(0, 0), DEFAULT_VECTOR_SIZE as u64)],
                    block: mock_data_block(DEFAULT_VECTOR_SIZE),
                },
                WalCmd::Delete {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    row_ids: vec![WalRowId::new(1, 3)],
                },
                WalCmd::Checkpoint { max_commit_ts: 123 },
                WalCmd::Compact {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    index_names: Vec::new(),
                    index_ids: Vec::new(),
                    new_segment_infos: (0..3).map(|_| make_segment_info(1, 0, 2)).collect(),
                    deprecated_segment_ids: vec![0, 1, 2],
                },
                WalCmd::DumpIndex {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    index_name: "idx1".to_string(),
                    index_id: "3".to_string(),
                    segment_id: 0,
                    chunk_infos: vec![WalChunkIndexInfo {
                        chunk_id: 2,
                        base_name: "base_name".to_string(),
                        base_row_id: WalRowId::new(0, 0),
                        row_count: 4,
                        term_count: 0,
                        index_size: 0,
                        deprecate_ts: 0,
                    }],
                    deprecate_ids: vec![0, 1],
                    table_key: "table_key".to_string(),
                },
                WalCmd::RenameTable {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    new_table_name: "tbl2".to_string(),
                    old_table_key: "old_table_key".to_string(),
                },
                WalCmd::AddColumns {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    column_idx_list: vec![3, 4],
                    column_defs: vec![
                        WalColumnDef {
                            column_id: 3,
                            name: "boolean_col".to_string(),
                            logical_type: LogicalType::Boolean,
                            constraints: Vec::new(),
                        },
                        WalColumnDef {
                            column_id: 4,
                            name: "embedding_col".to_string(),
                            logical_type: LogicalType::Embedding(16),
                            constraints: Vec::new(),
                        },
                    ],
                    table_key: "table_key".to_string(),
                },
                WalCmd::DropColumns {
                    db_name: "db1".to_string(),
                    db_id: "1".to_string(),
                    table_name: "tbl1".to_string(),
                    table_id: "2".to_string(),
                    column_names: vec!["boolean_col".to_string(), "embedding_col".to_string()],
                    column_ids: vec![3, 4],
                    table_key: "table_key".to_string(),
                    column_keys: vec!["column_key1".to_string(), "column_key2".to_string()],
                },
            ],
        );

        let expected = entry.size_in_bytes();
        let mut buf = Vec::with_capacity(expected);
        entry.write_adv(&mut buf);
        assert_eq!(buf.len(), expected);

        let (decoded, consumed) = WalEntry::read_adv(&buf).expect("decode entry");
        assert_eq!(decoded, entry);
        assert_eq!(consumed, expected);
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:339`
    /// (`WalEntryIterator`).
    #[test]
    fn wal_entry_iterator_replay_stops_at_checkpoint() {
        let dir = tempdir().unwrap();
        let wal_file = dir.path().join("wal.log");
        mock_wal_file(&wal_file);

        let mut iterator = WalEntryIterator::make(&wal_file, true).unwrap();
        assert_eq!(iterator.get_all_entries().len(), 6);

        let mut replay_entries = Vec::new();
        let mut max_commit_ts = 0;
        let mut iterator = WalEntryIterator::make(&wal_file, true).unwrap();
        while iterator.has_next() {
            let Some(entry) = iterator.next() else {
                break;
            };
            if let Some(ts) = entry.checkpoint_ts() {
                max_commit_ts = ts;
                break;
            }
            replay_entries.push(entry);
        }
        while iterator.has_next() {
            let Some(entry) = iterator.next() else {
                break;
            };
            if entry.commit_ts > max_commit_ts {
                replay_entries.push(entry);
            }
        }

        assert_eq!(max_commit_ts, 123);
        assert_eq!(replay_entries.len(), 1);
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:409`
    /// (`WalListIterator`).
    #[test]
    fn wal_list_iterator_replay_stops_at_checkpoint() {
        let dir = tempdir().unwrap();
        let wal_file1 = dir.path().join("wal.log");
        let wal_file2 = dir.path().join("wal2.log");
        mock_wal_file(&wal_file1);
        mock_wal_file(&wal_file2);

        let mut iterator = WalListIterator::new(&[wal_file1.clone(), wal_file2.clone()]).unwrap();
        assert_eq!(drain_wal_list(&mut iterator).len(), 12);

        let mut iterator = WalListIterator::new(&[wal_file1, wal_file2]).unwrap();
        let mut replay_entries = Vec::new();
        let mut max_commit_ts = 0;
        while iterator.has_next() {
            let Some(entry) = iterator.next() else {
                break;
            };
            if let Some(ts) = entry.checkpoint_ts() {
                max_commit_ts = ts;
                break;
            }
            replay_entries.push(entry);
        }
        while iterator.has_next() {
            let Some(entry) = iterator.next() else {
                break;
            };
            if entry.commit_ts > max_commit_ts {
                replay_entries.push(entry);
            }
        }
        assert_eq!(max_commit_ts, 123);
        assert_eq!(replay_entries.len(), 1);
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:564`
    /// (`DamagedEntryBeforeCheckpointIsIgnored`).
    #[test]
    fn damaged_entry_before_checkpoint_is_ignored() {
        let dir = tempdir().unwrap();
        let wal_file = dir.path().join("wal.log");
        mock_wal_file(&wal_file);
        let offsets = wal_entry_offsets(&wal_file);
        assert!(!offsets.is_empty());
        let size_before = std::fs::metadata(&wal_file).unwrap().len();
        corrupt_entry(&wal_file, offsets[0]);

        let mut iterator = WalListIterator::new(std::slice::from_ref(&wal_file)).unwrap();
        assert_eq!(replay_to_first_checkpoint(&mut iterator), 123);
        assert_eq!(std::fs::metadata(&wal_file).unwrap().len(), size_before);
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:582`
    /// (`DamagedEntryTruncatesWalFile`).
    #[test]
    fn damaged_entry_truncates_wal_file() {
        let dir = tempdir().unwrap();
        let wal_file1 = dir.path().join("wal.log");
        let wal_file2 = dir.path().join("wal2.log");
        let offsets = mock_wal_pair(&wal_file1, &wal_file2);
        corrupt_entry(&wal_file1, offsets[2]);

        let mut iterator = WalListIterator::new(&[wal_file1.clone(), wal_file2]).unwrap();
        assert_eq!(replay_to_first_checkpoint(&mut iterator), 123);
        assert_eq!(
            std::fs::metadata(&wal_file1).unwrap().len(),
            offsets[2] as u64
        );
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:597`
    /// (`DamagedFirstEntryDropsWalFile`).
    #[test]
    fn damaged_first_entry_drops_wal_file() {
        let dir = tempdir().unwrap();
        let wal_file1 = dir.path().join("wal.log");
        let wal_file2 = dir.path().join("wal2.log");
        let offsets = mock_wal_pair(&wal_file1, &wal_file2);
        corrupt_entry(&wal_file1, offsets[0]);

        let mut iterator = WalListIterator::new(&[wal_file1.clone(), wal_file2.clone()]).unwrap();
        assert_eq!(replay_to_first_checkpoint(&mut iterator), 123);
        assert!(!wal_file1.exists());
        assert!(wal_file2.exists());
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:614`
    /// (`DamagedTailAfterCheckpointIsTruncated`).
    #[test]
    fn damaged_tail_after_checkpoint_is_truncated() {
        let dir = tempdir().unwrap();
        let wal_file = dir.path().join("wal.log");
        mock_wal_file(&wal_file);
        let offsets = wal_entry_offsets(&wal_file);
        assert_eq!(offsets.len(), 6);
        corrupt_entry(&wal_file, offsets[5]);

        let mut iterator = WalListIterator::new(std::slice::from_ref(&wal_file)).unwrap();
        let drained = drain_wal_list(&mut iterator);
        assert_eq!(
            std::fs::metadata(&wal_file).unwrap().len(),
            offsets[5] as u64
        );
        assert_eq!(drained.len(), 5);
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:634`
    /// (`DamagedOlderFileDropsNewerOnes`).
    #[test]
    fn damaged_older_file_drops_newer_ones() {
        let dir = tempdir().unwrap();
        let wal_file1 = dir.path().join("wal.log");
        let wal_file2 = dir.path().join("wal2.log");
        let wal_file3 = dir.path().join("wal3.log");
        let offsets = mock_wal_pair(&wal_file1, &wal_file3);
        mock_wal_file_without_checkpoint(&wal_file2);
        assert_eq!(offsets.len(), 4);

        corrupt_entry(&wal_file1, offsets[2]);
        corrupt_entry(&wal_file2, offsets[0]);

        let mut iterator =
            WalListIterator::new(&[wal_file1.clone(), wal_file2.clone(), wal_file3]).unwrap();
        let drained = drain_wal_list(&mut iterator);
        assert_eq!(
            std::fs::metadata(&wal_file1).unwrap().len(),
            offsets[2] as u64
        );
        assert!(!wal_file2.exists());
        assert_eq!(drained.len(), 6);
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:657`
    /// (`NoCheckpointStillPurgesDamage`).
    #[test]
    fn no_checkpoint_still_purges_damage() {
        let dir = tempdir().unwrap();
        let wal_file1 = dir.path().join("wal.log");
        let wal_file2 = dir.path().join("wal2.log");
        let offsets = mock_wal_file_without_checkpoint(&wal_file1);
        mock_wal_file_without_checkpoint(&wal_file2);
        assert_eq!(offsets.len(), 4);
        corrupt_entry(&wal_file1, offsets[1]);

        let mut iterator = WalListIterator::new(&[wal_file1.clone(), wal_file2]).unwrap();
        let drained = drain_wal_list(&mut iterator);
        assert_eq!(
            std::fs::metadata(&wal_file1).unwrap().len(),
            offsets[1] as u64
        );
        assert_eq!(drained.len(), 5);
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:676`
    /// (`TornTailStopsIteration`).
    #[test]
    fn torn_tail_stops_iteration() {
        let dir = tempdir().unwrap();
        let wal_file = dir.path().join("wal.log");
        mock_wal_file(&wal_file);
        let offsets = wal_entry_offsets(&wal_file);
        assert!(offsets.len() >= 6);
        let file = OpenOptions::new().write(true).open(&wal_file).unwrap();
        file.set_len(offsets[5] as u64 + 5).unwrap();

        let mut iter = WalEntryIterator::make(&wal_file, false).unwrap();
        let entries = iter.get_all_entries();
        assert_eq!(entries.len(), 5);
        assert!(!iter.is_good());
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:713`
    /// (`CorruptedTrailingSizeIsRecoveredByForwardScan`).
    #[test]
    fn corrupted_trailing_size_is_recovered_by_forward_scan() {
        let dir = tempdir().unwrap();
        let wal_file = dir.path().join("wal.log");
        mock_wal_file(&wal_file);
        let offsets = wal_entry_offsets(&wal_file);
        assert!(offsets.len() >= 6);

        let mut buf = std::fs::read(&wal_file).unwrap();
        let leading =
            i32::from_le_bytes(buf[offsets[5]..offsets[5] + 4].try_into().unwrap()) as usize;
        let trail_pos = offsets[5] + leading - 4;
        buf[trail_pos..trail_pos + 4].copy_from_slice(&1024i32.to_le_bytes());
        std::fs::write(&wal_file, buf).unwrap();

        let mut iterator = WalListIterator::new(std::slice::from_ref(&wal_file)).unwrap();
        let drained = drain_wal_list(&mut iterator);
        assert_eq!(
            std::fs::metadata(&wal_file).unwrap().len(),
            offsets[5] as u64
        );
        assert_eq!(drained.len(), 5);
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:735`
    /// (`EmptyWalFileIsNotAnError`).
    #[test]
    fn empty_wal_file_is_not_an_error() {
        let dir = tempdir().unwrap();
        let wal_file = dir.path().join("wal.log");
        std::fs::write(&wal_file, []).unwrap();

        let mut iter = WalEntryIterator::make(&wal_file, false).unwrap();
        assert!(iter.get_all_entries().is_empty());
        assert!(iter.is_good());
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:749`
    /// (`FileShorterThanEntrySize`).
    #[test]
    fn file_shorter_than_entry_size_is_empty() {
        let dir = tempdir().unwrap();
        let wal_file = dir.path().join("wal.log");
        mock_wal_file(&wal_file);
        let file = OpenOptions::new().write(true).open(&wal_file).unwrap();
        file.set_len(2).unwrap();

        let mut forward = WalEntryIterator::make(&wal_file, false).unwrap();
        assert!(forward.get_all_entries().is_empty());
        assert!(!forward.is_good());

        let mut backward = WalEntryIterator::make(&wal_file, true).unwrap();
        assert!(backward.get_all_entries().is_empty());
        assert!(!backward.is_good());
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:768`
    /// (`AllFilesPurgedLeavesEmptyList`).
    #[test]
    fn all_files_purged_leaves_empty_list() {
        let dir = tempdir().unwrap();
        let wal_file1 = dir.path().join("wal.log");
        let wal_file2 = dir.path().join("wal2.log");
        let offsets1 = mock_wal_file_without_checkpoint(&wal_file1);
        let offsets2 = mock_wal_file_without_checkpoint(&wal_file2);
        assert_eq!(offsets1.len(), 4);
        assert_eq!(offsets2.len(), 4);

        corrupt_entry(&wal_file1, offsets1[0]);
        corrupt_entry(&wal_file2, offsets2[0]);

        let mut iterator = WalListIterator::new(&[wal_file1.clone(), wal_file2.clone()]).unwrap();
        assert!(!wal_file1.exists());
        assert!(!wal_file2.exists());
        assert!(!iterator.has_next());
        assert!(iterator.next().is_none());
    }

    /// C++: `src/unit_test/storage/wal/wal_entry_ut.cpp:793`
    /// (`DamageAccumulatesToCheckpointFile`).
    #[test]
    fn damage_accumulates_to_checkpoint_file() {
        let dir = tempdir().unwrap();
        let wal_file1 = dir.path().join("wal.log");
        let wal_file2 = dir.path().join("wal2.log");
        let wal_file3 = dir.path().join("wal3.log");
        let wal_file4 = dir.path().join("wal4.log");
        let offsets1 = mock_wal_file_without_checkpoint(&wal_file1);
        mock_wal_file_without_checkpoint(&wal_file2);
        let offsets3 = mock_wal_file_without_checkpoint(&wal_file3);
        mock_wal_file(&wal_file4);
        assert_eq!(offsets1.len(), 4);
        assert_eq!(offsets3.len(), 4);

        corrupt_entry(&wal_file1, offsets1[2]);
        corrupt_entry(&wal_file3, offsets3[0]);

        let mut iterator = WalListIterator::new(&[
            wal_file1.clone(),
            wal_file2.clone(),
            wal_file3.clone(),
            wal_file4.clone(),
        ])
        .unwrap();
        assert_eq!(
            std::fs::metadata(&wal_file1).unwrap().len(),
            offsets1[2] as u64
        );
        assert!(wal_file2.exists());
        assert!(!wal_file3.exists());
        assert!(wal_file4.exists());
        assert_eq!(replay_to_first_checkpoint(&mut iterator), 123);
    }

    /// The purge drops only files it has to; the retained list starts at the
    /// checkpoint file. C++: `wal_entry_impl.cpp:2532`.
    #[test]
    fn purge_keeps_the_checkpoint_file_at_the_front() {
        let dir = tempdir().unwrap();
        let wal_file1 = dir.path().join("wal.log");
        let wal_file2 = dir.path().join("wal2.log");
        let offsets = mock_wal_pair(&wal_file1, &wal_file2);
        corrupt_entry(&wal_file1, offsets[0]);

        let iterator = WalListIterator::new(&[wal_file1.clone(), wal_file2.clone()]).unwrap();
        assert_eq!(iterator.files().len(), 1);
        assert_eq!(iterator.files()[0], wal_file2);
    }
}
