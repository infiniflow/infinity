// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

use std::path::{Path, PathBuf};

use tokio::fs::{File, OpenOptions};
use tokio::io::{AsyncReadExt, AsyncWriteExt, BufReader, BufWriter};

use ::common::{Error, Result, TxnTimeStamp};

use crate::wal_entry::{WalEntry, WAL_ENTRY_HEADER_LEN};

// Port of the reference's `storage/wal` subsystem, one module per source
// unit. `log_file` and `bottom_executor` are done; `wal_manager` and
// `wal_entry` follow.

/// Append-only WAL writer for a single file.
///
/// One file, one writer: every method takes `&mut self`, and the engine gives
/// it exactly one owner - the flush thread in `txn::TxnManager`, which is the
/// reference's `new_flush_thread_` (`wal_manager.cppm:132`). The reference
/// keeps that thread and `wal_handle_` in one class; here the file is this
/// type and the thread wrapper is `WalFlushThread`, because the crash suite drives
/// `append`/`sync` directly on a thread of its own and must keep doing so.
pub struct WalManager {
    path: PathBuf,
    file: File,
    /// `NewFlush` appends a batch of entries and then fsyncs once, so the
    /// commit ts of the newest entry in the file is what that `Sync()` covers.
    appended_ts: TxnTimeStamp,
    /// Tracked byte length of the active WAL file to avoid an async `fstat`
    /// (`metadata().await`) on every `append`.
    file_size: u64,
}

impl WalManager {
    /// Opens (creating if needed) the WAL at `path` for appending.
    pub async fn open(path: impl Into<PathBuf>) -> Result<Self> {
        let path = path.into();
        let file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(&path)
            .await
            .map_err(|e| Error::io(&path, e))?;
        let file_size = file
            .metadata()
            .await
            .map_err(|e| Error::io(&path, e))?
            .len();
        Ok(Self {
            path,
            file,
            appended_ts: 0,
            file_size,
        })
    }

    pub fn path(&self) -> &Path {
        &self.path
    }

    /// Appends one entry and returns the byte offset it started at.
    pub async fn append(&mut self, entry: &WalEntry) -> Result<u64> {
        let mut buf = Vec::with_capacity(entry.size_in_bytes());
        entry.write_adv(&mut buf);
        let offset = self.file_size;
        self.file
            .write_all(&buf)
            .await
            .map_err(|e| Error::io(&self.path, e))?;
        self.file_size += buf.len() as u64;
        // Hand the bytes to the kernel before returning. `write_all` on a
        // `tokio::fs::File` completes the write, but the handle's own buffer is
        // only drained on `flush`, and a reader that opens the path straight
        // afterwards (`std::fs::read` in the golden-bytes test, or a restart
        // that reopens the log) could otherwise see a short file. This is not
        // an fsync - durability stays with `sync`, one per commit batch.
        self.file
            .flush()
            .await
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
    pub async fn sync(&mut self) -> Result<()> {
        self.file
            .sync_all()
            .await
            .map_err(|e| Error::io(&self.path, e))
    }

    /// A second handle on the same file, for a caller that wants to fsync off
    /// the thread holding the write lock.
    ///
    /// `sync_all` on a clone is an fsync of the same inode, so the bytes it
    /// covers are exactly the ones the writer has handed to the kernel up to
    /// that point - the ordering guarantee the single flush thread used to
    /// give.
    pub async fn sync_handle(&self) -> Result<File> {
        self.file
            .try_clone()
            .await
            .map_err(|e| Error::io(&self.path, e))
    }

    /// Flushes all dirty data and metadata on the filesystem containing the WAL
    /// (WAL and all open/closed process files) on the blocking pool.
    pub async fn sync_fs(handle: File) -> Result<()> {
        #[cfg(target_os = "linux")]
        {
            let std_file = handle.into_std().await;
            tokio::task::spawn_blocking(move || -> std::io::Result<()> {
                use std::os::fd::AsRawFd;
                extern "C" {
                    fn syncfs(fd: std::os::raw::c_int) -> std::os::raw::c_int;
                }
                #[allow(unsafe_code)]
                // SAFETY: `std_file` is a valid open fd; `syncfs` flushes all
                // dirty data and metadata on its filesystem.
                let rc = unsafe { syncfs(std_file.as_raw_fd()) };
                if rc != 0 {
                    return Err(std::io::Error::last_os_error());
                }
                Ok(())
            })
            .await
            .map_err(|e| Error::Wal(format!("WAL syncfs join: {e}")))?
            .map_err(|e| Error::Wal(format!("WAL syncfs: {e}")))?;
        }
        #[cfg(not(target_os = "linux"))]
        {
            handle
                .sync_all()
                .await
                .map_err(|e| Error::Wal(format!("WAL syncfs fallback: {e}")))?;
        }
        Ok(())
    }

    /// Flushes all dirty data and metadata on the filesystem containing `dir`
    /// on the blocking pool. Used as a data-filesystem barrier before a
    /// checkpoint frame is appended to the WAL.
    pub async fn sync_dir_fs(dir: &Path) -> Result<()> {
        let dir = dir.to_path_buf();
        tokio::task::spawn_blocking(move || -> std::io::Result<()> {
            let file = match std::fs::File::open(&dir) {
                Ok(f) => f,
                Err(e) if e.kind() == std::io::ErrorKind::NotFound => return Ok(()),
                Err(e) => return Err(e),
            };
            #[cfg(target_os = "linux")]
            {
                use std::os::fd::AsRawFd;
                extern "C" {
                    fn syncfs(fd: std::os::raw::c_int) -> std::os::raw::c_int;
                }
                #[allow(unsafe_code)]
                // SAFETY: `file` is a valid open fd; `syncfs` flushes all
                // dirty data and metadata on its filesystem.
                let rc = unsafe { syncfs(file.as_raw_fd()) };
                if rc != 0 {
                    return Err(std::io::Error::last_os_error());
                }
            }
            #[cfg(not(target_os = "linux"))]
            {
                fn sync_tree_recursive(path: &Path) -> std::io::Result<()> {
                    for entry in std::fs::read_dir(path)? {
                        let entry = entry?;
                        let entry_path = entry.path();
                        let file_type = entry.file_type()?;
                        if file_type.is_dir() {
                            sync_tree_recursive(&entry_path)?;
                        } else if file_type.is_file() {
                            std::fs::File::open(&entry_path)?.sync_all()?;
                        }
                    }
                    std::fs::File::open(path)?.sync_all()?;
                    Ok(())
                }
                drop(file);
                sync_tree_recursive(&dir)?;
            }
            Ok(())
        })
        .await
        .map_err(|e| Error::Wal(format!("data syncfs join: {e}")))?
        .map_err(|e| Error::Wal(format!("data syncfs: {e}")))?;
        Ok(())
    }

    /// Seals the active log as `wal.log.{seal_ts}` and starts a fresh one.
    /// Rotation is O(1); like `WalManager::SwapWalFile` it closes the old file
    /// and renames it. The directory is synced because the rename and the new
    /// file's name only survive a power loss once the directory entry does.
    pub async fn rotate(&mut self, seal_ts: TxnTimeStamp) -> Result<()> {
        // The reference syncs the handle before it renames it away
        // (`WalManager::SwapWalFile`, #3534): a rename only makes the *name*
        // durable, so the contents have to be on disk first. Commit entries are
        // synced one by one, but the checkpoint frame is appended unsynced and
        // `rotate_wal_if_large` can run before `sync_wal`.
        self.file
            .sync_all()
            .await
            .map_err(|e| Error::io(&self.path, e))?;
        let sealed = self.path.with_extension(format!("log.{seal_ts}"));
        tokio::fs::rename(&self.path, &sealed)
            .await
            .map_err(|e| Error::io(&sealed, e))?;
        self.file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(&self.path)
            .await
            .map_err(|e| Error::io(&self.path, e))?;
        self.file_size = 0;
        // A directory is synced through a plain `File`: `tokio::fs::File`
        // cannot open one portably, and the fsync is the whole point (the
        // rename is only durable once the directory entry is). It is still a
        // real fsync, so it stays on the blocking pool instead of a worker.
        if let Some(dir) = self.path.parent() {
            let dir = dir.to_path_buf();
            tokio::task::spawn_blocking(move || {
                let handle = std::fs::File::open(&dir).map_err(|e| Error::io(&dir, e))?;
                handle.sync_all().map_err(|e| Error::io(&dir, e))
            })
            .await
            .map_err(|e| Error::Wal(format!("WAL directory sync: {e}")))??;
        }
        Ok(())
    }

    /// Current size of the WAL in bytes.
    pub async fn file_size(&self) -> Result<u64> {
        Ok(self.file_size)
    }

    /// Rewrites the WAL keeping only entries with `commit_ts > ts`, streaming
    /// record by record so a multi-gigabyte log never has to fit in memory.
    /// Returns the number of entries retained. The append handle is reopened on
    /// the new inode.
    pub async fn truncate_up_to_streaming(&mut self, ts: TxnTimeStamp) -> Result<usize> {
        let tmp = self.path.with_extension("log.compact");
        // The size below bounds how far the reader walks, and it comes from the
        // file's metadata. A `write_all` that has not been flushed can still be
        // queued in the file object, so the length read here would be short and
        // the reader would stop at a frame boundary that is not the end of the
        // log - `streaming_truncate_keeps_only_newer_records` saw that as four
        // records retained instead of five, one run in five.
        self.file
            .flush()
            .await
            .map_err(|e| Error::io(&self.path, e))?;
        let file_len = self.file_size().await?;
        let mut consumed = 0u64;
        let mut kept = 0usize;
        {
            let src = File::open(&self.path)
                .await
                .map_err(|e| Error::io(&self.path, e))?;
            let mut rd = BufReader::new(src);
            let out = File::create(&tmp).await.map_err(|e| Error::io(&tmp, e))?;
            let mut wr = BufWriter::new(out);
            loop {
                let mut header = [0u8; WAL_ENTRY_HEADER_LEN];
                match rd.read_exact(&mut header).await {
                    Ok(_) => {}
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
                if rd.read_exact(&mut body).await.is_err() {
                    break; // torn frame at the tail
                }
                frame.extend_from_slice(&body);
                let Some((entry, _)) = WalEntry::read_adv(&frame) else {
                    break; // trailing size or checksum disagrees
                };
                consumed += size as u64;
                if entry.commit_ts > ts {
                    wr.write_all(&frame).await.map_err(|e| Error::io(&tmp, e))?;
                    kept += 1;
                }
            }
            wr.flush().await.map_err(|e| Error::io(&tmp, e))?;
            // `sync_all` through the writer's own handle: `BufWriter` flushes
            // to it on `flush`, so the bytes are on the filesystem before the
            // rename below.
            wr.get_ref()
                .sync_all()
                .await
                .map_err(|e| Error::io(&tmp, e))?;
        }
        tokio::fs::rename(&tmp, &self.path)
            .await
            .map_err(|e| Error::io(&self.path, e))?;
        *self = WalManager::open(&self.path).await?;
        Ok(kept)
    }

    /// Reads every valid entry from a WAL file path, stopping at the first
    /// record the reference's `ReadAdv` would reject.
    pub async fn read_all(path: impl AsRef<Path>) -> Result<Vec<WalEntry>> {
        let path = path.as_ref();
        match tokio::fs::read(path).await {
            Ok(buf) => Ok(decode_entries(&buf)),
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => Ok(Vec::new()),
            Err(e) => Err(Error::io(path, e)),
        }
    }

    /// Replays entries with `commit_ts > checkpoint_ts` (recovery helper).
    pub async fn replay_since(
        path: impl AsRef<Path>,
        checkpoint_ts: TxnTimeStamp,
        mut f: impl FnMut(&WalEntry) -> Result<()>,
    ) -> Result<usize> {
        let mut n = 0;
        for entry in Self::read_all(path).await? {
            if entry.commit_ts > checkpoint_ts {
                f(&entry)?;
                n += 1;
            }
        }
        Ok(n)
    }

    /// Rewrites the WAL keeping only entries with `commit_ts > ts` (used at a
    /// checkpoint). Returns the number of entries retained.
    pub async fn truncate_up_to_ts(path: impl AsRef<Path>, ts: TxnTimeStamp) -> Result<usize> {
        let path = path.as_ref();
        let keep: Vec<WalEntry> = Self::read_all(path)
            .await?
            .into_iter()
            .filter(|r| r.commit_ts > ts)
            .collect();
        let tmp = path.with_extension("log.compact");
        {
            let mut f = File::create(&tmp).await.map_err(|e| Error::io(&tmp, e))?;
            for entry in &keep {
                let mut buf = Vec::with_capacity(entry.size_in_bytes());
                entry.write_adv(&mut buf);
                f.write_all(&buf).await.map_err(|e| Error::io(&tmp, e))?;
            }
            f.sync_all().await.map_err(|e| Error::io(&tmp, e))?;
        }
        tokio::fs::rename(&tmp, path)
            .await
            .map_err(|e| Error::io(path, e))?;
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
    #[tokio::test]
    async fn writer_emits_the_reference_bytes_and_reader_accepts_them() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).await.unwrap();
        w.append(&golden_entry()).await.unwrap();
        drop(w);

        assert_eq!(std::fs::read(&path).unwrap(), GOLDEN_ENTRY);
        assert_eq!(
            WalManager::read_all(&path).await.unwrap(),
            vec![golden_entry()]
        );
    }

    /// Check 3 (`wal_entry_impl.cpp:2239`): a payload byte that does not match
    /// the checksum makes `ReadAdv` reject the entry, so replay stops there.
    #[tokio::test]
    async fn a_corrupt_byte_fails_the_checksum() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut bytes = GOLDEN_ENTRY.to_vec();
        bytes[33] ^= 0xFF; // inside the `db1` string
        std::fs::write(&path, &bytes).unwrap();
        assert!(WalManager::read_all(&path).await.unwrap().is_empty());
    }

    /// Check 2 (`wal_entry_impl.cpp:2233`): the trailing `i32` must repeat the
    /// leading size.
    #[tokio::test]
    async fn a_mismatched_trailing_size_is_rejected() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut bytes = GOLDEN_ENTRY.to_vec();
        let wrong = 74i32;
        bytes[71..75].copy_from_slice(&wrong.to_le_bytes());
        std::fs::write(&path, &bytes).unwrap();
        assert!(WalManager::read_all(&path).await.unwrap().is_empty());
    }

    /// Check 1 (`wal_entry_impl.cpp:2229`): the leading size may not exceed the
    /// bytes handed to `ReadAdv` — a torn tail is not an entry.
    #[tokio::test]
    async fn a_truncated_entry_is_rejected() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        std::fs::write(&path, &GOLDEN_ENTRY[..71]).unwrap();
        assert!(WalManager::read_all(&path).await.unwrap().is_empty());
    }

    #[tokio::test]
    async fn rotation_seals_the_active_log() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).await.unwrap();
        w.append(&entry(1, 1)).await.unwrap();
        w.rotate(1).await.unwrap();
        w.append(&entry(2, 2)).await.unwrap();
        assert!(dir.path().join("wal.log.1").exists());
        assert_eq!(WalManager::read_all(&path).await.unwrap().len(), 1);
        assert_eq!(
            WalManager::read_all(dir.path().join("wal.log.1"))
                .await
                .unwrap()
                .len(),
            1
        );
    }

    #[tokio::test]
    async fn streaming_truncate_keeps_only_newer_records() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).await.unwrap();
        for ts in 1..=10u64 {
            w.append(&entry(ts as i64, ts)).await.unwrap();
        }
        assert_eq!(w.truncate_up_to_streaming(6).await.unwrap(), 4);
        let recs = WalManager::read_all(&path).await.unwrap();
        assert_eq!(
            recs.iter().map(|r| r.commit_ts).collect::<Vec<_>>(),
            vec![7, 8, 9, 10]
        );
        // the append handle points at the rewritten file
        w.append(&entry(11, 11)).await.unwrap();
        assert_eq!(WalManager::read_all(&path).await.unwrap().len(), 5);
    }

    #[tokio::test]
    async fn append_and_read_roundtrip() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).await.unwrap();
        w.append(&WalEntry::new(1, 10, vec![])).await.unwrap();
        w.append(&WalEntry::new(
            2,
            20,
            vec![WalCmd::Checkpoint { max_commit_ts: 9 }],
        ))
        .await
        .unwrap();

        let recs = WalManager::read_all(&path).await.unwrap();
        assert_eq!(recs.len(), 2);
        assert_eq!(recs[0], WalEntry::new(1, 10, vec![]));
        assert_eq!(
            recs[1],
            WalEntry::new(2, 20, vec![WalCmd::Checkpoint { max_commit_ts: 9 }])
        );
    }

    #[tokio::test]
    async fn torn_tail_is_ignored() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        {
            let mut w = WalManager::open(&path).await.unwrap();
            w.append(&entry(1, 10)).await.unwrap();
        }
        // simulate a crash mid-append: a header claiming more bytes than exist
        {
            let mut f = OpenOptions::new().append(true).open(&path).await.unwrap();
            f.write_all(&4096i32.to_le_bytes()).await.unwrap();
            f.write_all(&0u32.to_le_bytes()).await.unwrap();
            f.write_all(&0u64.to_le_bytes()).await.unwrap();
            f.write_all(&0u64.to_le_bytes()).await.unwrap();
            f.sync_all().await.unwrap();
        }
        let recs = WalManager::read_all(&path).await.unwrap();
        assert_eq!(recs.len(), 1);
        assert_eq!(recs[0].txn_id, 1);
    }

    #[tokio::test]
    async fn replay_since_filters_by_commit_ts() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).await.unwrap();
        for i in 1..=5u64 {
            w.append(&entry(i as i64, i)).await.unwrap();
        }
        drop(w);
        let mut seen = vec![];
        let n = WalManager::replay_since(&path, 3, |r| {
            seen.push(r.txn_id);
            Ok(())
        })
        .await
        .unwrap();
        assert_eq!(n, 2);
        assert_eq!(seen, vec![4, 5]);
    }

    #[tokio::test]
    async fn truncate_keeps_tail() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).await.unwrap();
        for i in 1..=5u64 {
            w.append(&entry(i as i64, i)).await.unwrap();
        }
        drop(w);
        let kept = WalManager::truncate_up_to_ts(&path, 3).await.unwrap();
        assert_eq!(kept, 2);
        let recs = WalManager::read_all(&path).await.unwrap();
        assert_eq!(
            recs.iter().map(|r| r.txn_id).collect::<Vec<_>>(),
            vec![4, 5]
        );
    }
}
