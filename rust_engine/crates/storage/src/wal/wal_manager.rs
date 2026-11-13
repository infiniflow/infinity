//! Append-only redo log, byte-compatible with the reference
//! (`src/storage/wal/`).
//!
//! The file is a stream of `WalEntry` records and nothing else — no magic, no
//! per-engine frame. An entry is (`wal_entry_impl.cpp:2180-2271`):
//!
//! ```text
//! [size: i32][checksum: u32][txn_id: i64][commit_ts: u64]   // WalEntryHeader, 24 bytes
//! [cmd_count: i32][cmd]...
//! [size: i32]                                               // the leading size, again
//! ```
//!
//! `size` counts the whole entry including both size fields; `checksum` is
//! CRC-32/IEEE over those `size` bytes with the checksum field itself zeroed.
//! The trailing copy exists so the reference's backward iterator can find the
//! previous entry; `ReadAdv` cross-checks it against the leading size.
//!
//! The reader applies the reference's three checks in the same order and with
//! the same outcome (stop replay, "this is not an entry"):
//!
//! 1. `size` must be in `(sizeof(header), bytes handed in]`
//!    (`wal_entry_impl.cpp:2229`),
//! 2. the trailing `i32` must equal `size` (`wal_entry_impl.cpp:2233`),
//! 3. the CRC, recomputed with the checksum field zeroed, must match
//!    (`wal_entry_impl.cpp:2239`).
//!
//! So a crash during append leaves a valid prefix and recovery stops at the
//! first torn / corrupt record. Appends are serialized by `&mut self` (single
//! writer), matching the engine's serial `WriteWAL` stage.

use std::fs::{File, OpenOptions};
use std::io::{BufReader, BufWriter, Read, Write};
use std::path::{Path, PathBuf};

use ::common::{Error, Result, TxnTimeStamp};

use crate::wal_entry::{WalEntry, WAL_ENTRY_HEADER_LEN};

// Port of the reference's `storage/wal` subsystem, one module per source
// unit. `log_file` and `bottom_executor` are done; `wal_manager` and
// `wal_entry` follow.

/// Append-only WAL writer for a single file.
pub struct WalManager {
    path: PathBuf,
    file: File,
    /// `NewFlush` appends a batch of entries and then fsyncs once, so the
    /// commit ts of the newest entry in the file is what that `Sync()` covers.
    appended_ts: TxnTimeStamp,
}

impl WalManager {
    /// Opens (creating if needed) the WAL at `path` for appending.
    pub fn open(path: impl Into<PathBuf>) -> Result<Self> {
        let path = path.into();
        let file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(&path)
            .map_err(|e| Error::io(&path, e))?;
        Ok(Self {
            path,
            file,
            appended_ts: 0,
        })
    }

    pub fn path(&self) -> &Path {
        &self.path
    }

    /// Appends one entry and returns the byte offset it started at.
    pub fn append(&mut self, entry: &WalEntry) -> Result<u64> {
        let mut buf = Vec::with_capacity(entry.size_in_bytes());
        entry.write_adv(&mut buf);
        let offset = self.file.metadata().map(|m| m.len()).unwrap_or(0);
        self.file
            .write_all(&buf)
            .map_err(|e| Error::io(&self.path, e))?;
        self.appended_ts = self.appended_ts.max(entry.commit_ts);
        // The write itself is not synced: `WalManager::NewFlush` owns that
        // decision. Upstream ends every flush batch with
        // `wal_handle_->Sync()` (#3534, replacing the old userspace
        // `ofs_.flush()`), so `TxnManager::commit_at` calls
        // [`WalManager::sync`] right after this append - one fsync per commit.
        Ok(offset)
    }

    /// The commit ts of the newest entry written to the file. A sync covers
    /// exactly this far, never an entry appended behind it.
    pub fn appended_ts(&self) -> TxnTimeStamp {
        self.appended_ts
    }

    /// Makes everything appended so far durable (fsync).
    ///
    /// Called after every commit batch and after the checkpoint frame: the
    /// reference's WAL is the durability boundary for everything the catalog
    /// store does not flush, because the store keeps RocksDB's log off.
    pub fn sync(&mut self) -> Result<()> {
        self.file.sync_all().map_err(|e| Error::io(&self.path, e))
    }

    /// Seals the active log as `wal.log.{seal_ts}` and starts a fresh one.
    /// Rotation is O(1); like `WalManager::SwapWalFile` it closes the old file
    /// and renames it. The directory is synced because the rename and the new
    /// file's name only survive a power loss once the directory entry does.
    pub fn rotate(&mut self, seal_ts: TxnTimeStamp) -> Result<()> {
        // The reference syncs the handle before it renames it away
        // (`WalManager::SwapWalFile`, #3534): a rename only makes the *name*
        // durable, so the contents have to be on disk first. Commit entries are
        // synced one by one, but the checkpoint frame is appended unsynced and
        // `rotate_wal_if_large` can run before `sync_wal`.
        self.file.sync_all().map_err(|e| Error::io(&self.path, e))?;
        let sealed = self.path.with_extension(format!("log.{seal_ts}"));
        std::fs::rename(&self.path, &sealed).map_err(|e| Error::io(&sealed, e))?;
        self.file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(&self.path)
            .map_err(|e| Error::io(&self.path, e))?;
        if let Some(dir) = self.path.parent() {
            let handle = std::fs::File::open(dir).map_err(|e| Error::io(dir, e))?;
            handle.sync_all().map_err(|e| Error::io(dir, e))?;
        }
        Ok(())
    }

    /// Current size of the WAL in bytes.
    pub fn file_size(&self) -> Result<u64> {
        Ok(self
            .file
            .metadata()
            .map_err(|e| Error::io(&self.path, e))?
            .len())
    }

    /// Rewrites the WAL keeping only entries with `commit_ts > ts`, streaming
    /// record by record so a multi-gigabyte log never has to fit in memory.
    /// Returns the number of entries retained. The append handle is reopened on
    /// the new inode.
    pub fn truncate_up_to_streaming(&mut self, ts: TxnTimeStamp) -> Result<usize> {
        let tmp = self.path.with_extension("log.compact");
        let file_len = self.file_size()?;
        let mut consumed = 0u64;
        let mut kept = 0usize;
        {
            let src = File::open(&self.path).map_err(|e| Error::io(&self.path, e))?;
            let mut rd = BufReader::new(src);
            let out = File::create(&tmp).map_err(|e| Error::io(&tmp, e))?;
            let mut wr = BufWriter::new(out);
            loop {
                let mut header = [0u8; WAL_ENTRY_HEADER_LEN];
                match rd.read_exact(&mut header) {
                    Ok(()) => {}
                    Err(e) if e.kind() == std::io::ErrorKind::UnexpectedEof => break,
                    Err(e) => return Err(Error::io(&self.path, e)),
                }
                // `size` indexes the rest of the record, so bound it against
                // the bytes left in the file before allocating or reading.
                let size = i32::from_le_bytes(header[0..4].try_into().unwrap());
                if size <= WAL_ENTRY_HEADER_LEN as i32
                    || size as u64 > file_len.saturating_sub(consumed)
                {
                    break; // torn / corrupt tail
                }
                let mut frame = Vec::with_capacity(size as usize);
                frame.extend_from_slice(&header);
                let mut body = vec![0u8; size as usize - WAL_ENTRY_HEADER_LEN];
                if rd.read_exact(&mut body).is_err() {
                    break; // torn frame at the tail
                }
                frame.extend_from_slice(&body);
                let Some((entry, _)) = WalEntry::read_adv(&frame) else {
                    break; // trailing size or checksum disagrees
                };
                consumed += size as u64;
                if entry.commit_ts > ts {
                    wr.write_all(&frame).map_err(|e| Error::io(&tmp, e))?;
                    kept += 1;
                }
            }
            wr.flush().map_err(|e| Error::io(&tmp, e))?;
            wr.get_ref().sync_all().map_err(|e| Error::io(&tmp, e))?;
        }
        std::fs::rename(&tmp, &self.path).map_err(|e| Error::io(&self.path, e))?;
        *self = WalManager::open(&self.path)?;
        Ok(kept)
    }

    /// Reads every valid entry from a WAL file path, stopping at the first
    /// record the reference's `ReadAdv` would reject.
    pub fn read_all(path: impl AsRef<Path>) -> Result<Vec<WalEntry>> {
        let path = path.as_ref();
        match std::fs::read(path) {
            Ok(buf) => Ok(decode_entries(&buf)),
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => Ok(Vec::new()),
            Err(e) => Err(Error::io(path, e)),
        }
    }

    /// Replays entries with `commit_ts > checkpoint_ts` (recovery helper).
    pub fn replay_since(
        path: impl AsRef<Path>,
        checkpoint_ts: TxnTimeStamp,
        mut f: impl FnMut(&WalEntry) -> Result<()>,
    ) -> Result<usize> {
        let mut n = 0;
        for entry in Self::read_all(path)? {
            if entry.commit_ts > checkpoint_ts {
                f(&entry)?;
                n += 1;
            }
        }
        Ok(n)
    }

    /// Rewrites the WAL keeping only entries with `commit_ts > ts` (used at a
    /// checkpoint). Returns the number of entries retained.
    pub fn truncate_up_to_ts(path: impl AsRef<Path>, ts: TxnTimeStamp) -> Result<usize> {
        let path = path.as_ref();
        let keep: Vec<WalEntry> = Self::read_all(path)?
            .into_iter()
            .filter(|r| r.commit_ts > ts)
            .collect();
        let tmp = path.with_extension("log.compact");
        {
            let mut f = File::create(&tmp).map_err(|e| Error::io(&tmp, e))?;
            for entry in &keep {
                let mut buf = Vec::with_capacity(entry.size_in_bytes());
                entry.write_adv(&mut buf);
                f.write_all(&buf).map_err(|e| Error::io(&tmp, e))?;
            }
            f.sync_all().map_err(|e| Error::io(&tmp, e))?;
        }
        std::fs::rename(&tmp, path).map_err(|e| Error::io(path, e))?;
        Ok(keep.len())
    }
}

/// Walks a byte buffer as a stream of `WalEntry` records.
///
/// Each iteration hands [`WalEntry::read_adv`] the rest of the buffer, which is
/// exactly the `max_bytes` the reference passes to `WalEntry::ReadAdv` from
/// `WalEntryIterator::Next`. A rejected record ends the walk.
pub(crate) fn decode_entries(mut buf: &[u8]) -> Vec<WalEntry> {
    let mut out = Vec::new();
    while let Some((entry, size)) = WalEntry::read_adv(buf) {
        out.push(entry);
        buf = &buf[size..];
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::wal_entry::{WalCmd, WalRowId};
    use tempfile::tempdir;

    fn entry(txn_id: i64, commit_ts: u64) -> WalEntry {
        WalEntry::new(txn_id, commit_ts, vec![])
    }

    /// Bytes of one entry with `txn_id = 42`, `commit_ts = 7` and two commands
    /// (`DELETE_V2` of `db1`/`t1` row `(1, 3)`, then `CHECKPOINT_V2(9)`),
    /// transcribed by hand from `wal_entry_impl.cpp:2180-2211`:
    ///
    /// ```text
    /// 4b 00 00 00              size = 75 (whole entry)
    /// 23 d1 aa 7a              CRC32IEEE with this field zeroed
    /// 2a 00 00 00 00 00 00 00  txn_id = 42
    /// 07 00 00 00 00 00 00 00  commit_ts = 7
    /// 02 00 00 00              cmd_count = 2
    /// 19                       WalCommandType::DELETE_V2
    /// 03 00 00 00 "db1"        db_name
    /// 00 00 00 00              db_id
    /// 02 00 00 00 "t1"         table_name
    /// 00 00 00 00              table_id
    /// 01 00 00 00              one row id
    /// 03 00 00 00  01 00 00 00  RowID segment_offset = 3, segment_id = 1
    ///                          (`row_id.h:36-47`: the union's `u64` value is
    ///                          written little-endian, so offset comes first)
    /// 68                       WalCommandType::CHECKPOINT_V2
    /// 09 00 00 00 00 00 00 00  max_commit_ts = 9 (i64)
    /// 4b 00 00 00              size = 75 again
    /// ```
    ///
    /// The checksum is the literal value `zlib.crc32` (== CRC-32/IEEE) yields
    /// for these bytes with the field zeroed, so this also pins the CRC
    /// algorithm.
    const GOLDEN_ENTRY: [u8; 75] = [
        0x4b, 0x00, 0x00, 0x00, 0x23, 0xd1, 0xaa, 0x7a, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x19, 0x03,
        0x00, 0x00, 0x00, 0x64, 0x62, 0x31, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x74,
        0x31, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x68, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x00, 0x00, 0x00,
    ];

    fn golden_entry() -> WalEntry {
        WalEntry::new(
            42,
            7,
            vec![
                WalCmd::Delete {
                    db_name: "db1".to_string(),
                    db_id: String::new(),
                    table_name: "t1".to_string(),
                    table_id: String::new(),
                    row_ids: vec![WalRowId::new(1, 3)],
                },
                WalCmd::Checkpoint { max_commit_ts: 9 },
            ],
        )
    }

    /// The writer must produce the reference's bytes, not merely something the
    /// Rust reader understands, and the reader must accept those bytes.
    #[test]
    fn writer_emits_the_reference_bytes_and_reader_accepts_them() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        w.append(&golden_entry()).unwrap();
        drop(w);

        assert_eq!(std::fs::read(&path).unwrap(), GOLDEN_ENTRY);
        assert_eq!(WalManager::read_all(&path).unwrap(), vec![golden_entry()]);
    }

    /// Check 3 (`wal_entry_impl.cpp:2239`): a payload byte that does not match
    /// the checksum makes `ReadAdv` reject the entry, so replay stops there.
    #[test]
    fn a_corrupt_byte_fails_the_checksum() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut bytes = GOLDEN_ENTRY.to_vec();
        bytes[33] ^= 0xFF; // inside the `db1` string
        std::fs::write(&path, &bytes).unwrap();
        assert!(WalManager::read_all(&path).unwrap().is_empty());
    }

    /// Check 2 (`wal_entry_impl.cpp:2233`): the trailing `i32` must repeat the
    /// leading size.
    #[test]
    fn a_mismatched_trailing_size_is_rejected() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut bytes = GOLDEN_ENTRY.to_vec();
        let wrong = 74i32;
        bytes[71..75].copy_from_slice(&wrong.to_le_bytes());
        std::fs::write(&path, &bytes).unwrap();
        assert!(WalManager::read_all(&path).unwrap().is_empty());
    }

    /// Check 1 (`wal_entry_impl.cpp:2229`): the leading size may not exceed the
    /// bytes handed to `ReadAdv` — a torn tail is not an entry.
    #[test]
    fn a_truncated_entry_is_rejected() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        std::fs::write(&path, &GOLDEN_ENTRY[..71]).unwrap();
        assert!(WalManager::read_all(&path).unwrap().is_empty());
    }

    #[test]
    fn rotation_seals_the_active_log() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        w.append(&entry(1, 1)).unwrap();
        w.rotate(1).unwrap();
        w.append(&entry(2, 2)).unwrap();
        assert!(dir.path().join("wal.log.1").exists());
        assert_eq!(WalManager::read_all(&path).unwrap().len(), 1);
        assert_eq!(
            WalManager::read_all(dir.path().join("wal.log.1"))
                .unwrap()
                .len(),
            1
        );
    }

    #[test]
    fn streaming_truncate_keeps_only_newer_records() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        for ts in 1..=10u64 {
            w.append(&entry(ts as i64, ts)).unwrap();
        }
        assert_eq!(w.truncate_up_to_streaming(6).unwrap(), 4);
        let recs = WalManager::read_all(&path).unwrap();
        assert_eq!(
            recs.iter().map(|r| r.commit_ts).collect::<Vec<_>>(),
            vec![7, 8, 9, 10]
        );
        // the append handle points at the rewritten file
        w.append(&entry(11, 11)).unwrap();
        assert_eq!(WalManager::read_all(&path).unwrap().len(), 5);
    }

    #[test]
    fn append_and_read_roundtrip() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        w.append(&WalEntry::new(1, 10, vec![])).unwrap();
        w.append(&WalEntry::new(
            2,
            20,
            vec![WalCmd::Checkpoint { max_commit_ts: 9 }],
        ))
        .unwrap();

        let recs = WalManager::read_all(&path).unwrap();
        assert_eq!(recs.len(), 2);
        assert_eq!(recs[0], WalEntry::new(1, 10, vec![]));
        assert_eq!(
            recs[1],
            WalEntry::new(2, 20, vec![WalCmd::Checkpoint { max_commit_ts: 9 }])
        );
    }

    #[test]
    fn torn_tail_is_ignored() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        {
            let mut w = WalManager::open(&path).unwrap();
            w.append(&entry(1, 10)).unwrap();
        }
        // simulate a crash mid-append: a header claiming more bytes than exist
        {
            let mut f = OpenOptions::new().append(true).open(&path).unwrap();
            f.write_all(&4096i32.to_le_bytes()).unwrap();
            f.write_all(&0u32.to_le_bytes()).unwrap();
            f.write_all(&0u64.to_le_bytes()).unwrap();
            f.write_all(&0u64.to_le_bytes()).unwrap();
            f.sync_all().unwrap();
        }
        let recs = WalManager::read_all(&path).unwrap();
        assert_eq!(recs.len(), 1);
        assert_eq!(recs[0].txn_id, 1);
    }

    #[test]
    fn replay_since_filters_by_commit_ts() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        for i in 1..=5u64 {
            w.append(&entry(i as i64, i)).unwrap();
        }
        drop(w);
        let mut seen = vec![];
        let n = WalManager::replay_since(&path, 3, |r| {
            seen.push(r.txn_id);
            Ok(())
        })
        .unwrap();
        assert_eq!(n, 2);
        assert_eq!(seen, vec![4, 5]);
    }

    #[test]
    fn truncate_keeps_tail() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        for i in 1..=5u64 {
            w.append(&entry(i as i64, i)).unwrap();
        }
        drop(w);
        let kept = WalManager::truncate_up_to_ts(&path, 3).unwrap();
        assert_eq!(kept, 2);
        let recs = WalManager::read_all(&path).unwrap();
        assert_eq!(
            recs.iter().map(|r| r.txn_id).collect::<Vec<_>>(),
            vec![4, 5]
        );
    }
}
