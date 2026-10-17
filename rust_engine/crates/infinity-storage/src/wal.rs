//! Custom write-ahead log, mirroring `src/storage/wal/` (plan 附录 A.4).
//!
//! Frame layout (little-endian):
//!   `[magic: u32][size: u32][checksum: u32][content: size bytes]`
//!   `content = txn_id:u64 | commit_ts:u64 | payload`
//!   `checksum = CRC-32/IEEE(content)`
//!
//! The reader stops at the first torn / corrupt / foreign frame, so a crash
//! during append leaves a valid prefix. Appends are serialized by `&mut self`
//! (single writer), matching the engine's serial `WriteWAL` stage.

use std::fs::{File, OpenOptions};
use std::io::{BufReader, BufWriter, Read, Write};
use std::path::{Path, PathBuf};

use infinity_common::{Error, Result, TxnTimeStamp};

use crate::crc::crc32_ieee;

/// Frame magic (`"WAL1"`).
pub const WAL_MAGIC: u32 = 0x5741_4C31;
const HEADER_LEN: usize = 12; // magic + size + checksum

/// A single redo-log record.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct WalRecord {
    pub txn_id: u64,
    pub commit_ts: TxnTimeStamp,
    pub payload: Vec<u8>,
}

impl WalRecord {
    pub fn new(txn_id: u64, commit_ts: TxnTimeStamp, payload: impl Into<Vec<u8>>) -> Self {
        Self {
            txn_id,
            commit_ts,
            payload: payload.into(),
        }
    }

    fn encode_content(&self) -> Vec<u8> {
        let mut c = Vec::with_capacity(16 + self.payload.len());
        c.extend_from_slice(&self.txn_id.to_le_bytes());
        c.extend_from_slice(&self.commit_ts.to_le_bytes());
        c.extend_from_slice(&self.payload);
        c
    }

    fn encode_frame(&self) -> Vec<u8> {
        let content = self.encode_content();
        let mut f = Vec::with_capacity(HEADER_LEN + content.len());
        f.extend_from_slice(&WAL_MAGIC.to_le_bytes());
        f.extend_from_slice(&(content.len() as u32).to_le_bytes());
        f.extend_from_slice(&crc32_ieee(&content).to_le_bytes());
        f.extend_from_slice(&content);
        f
    }
}

/// Serializes the decoding of a WAL frame stream.
fn decode_records(mut buf: &[u8]) -> Vec<WalRecord> {
    let mut out = Vec::new();
    loop {
        if buf.len() < HEADER_LEN {
            break;
        }
        let magic = u32::from_le_bytes(buf[0..4].try_into().unwrap());
        if magic != WAL_MAGIC {
            break;
        }
        let size = u32::from_le_bytes(buf[4..8].try_into().unwrap()) as usize;
        let checksum = u32::from_le_bytes(buf[8..12].try_into().unwrap());
        if buf.len() < HEADER_LEN + size {
            break; // torn frame
        }
        let content = &buf[HEADER_LEN..HEADER_LEN + size];
        if crc32_ieee(content) != checksum || content.len() < 16 {
            break; // corrupt
        }
        out.push(WalRecord {
            txn_id: u64::from_le_bytes(content[0..8].try_into().unwrap()),
            commit_ts: u64::from_le_bytes(content[8..16].try_into().unwrap()),
            payload: content[16..].to_vec(),
        });
        buf = &buf[HEADER_LEN + size..];
    }
    out
}

/// Append-only WAL writer for a single file.
pub struct WalManager {
    path: PathBuf,
    file: File,
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
        Ok(Self { path, file })
    }

    pub fn path(&self) -> &Path {
        &self.path
    }

    /// Appends one record and fsyncs. Returns the byte offset the record started at.
    pub fn append(&mut self, rec: &WalRecord) -> Result<u64> {
        let frame = rec.encode_frame();
        let offset = self.file.metadata().map(|m| m.len()).unwrap_or(0);
        self.file
            .write_all(&frame)
            .map_err(|e| Error::io(&self.path, e))?;
        self.file.sync_all().map_err(|e| Error::io(&self.path, e))?;
        Ok(offset)
    }

    pub fn flush(&mut self) -> Result<()> {
        self.file.sync_all().map_err(|e| Error::io(&self.path, e))
    }

    /// Empties the file (used at a checkpoint once every record is applied to
    /// the KV store). The append handle stays usable: with `O_APPEND` the next
    /// write lands at the new end of file.
    pub fn truncate_all(&mut self) -> Result<()> {
        self.file.set_len(0).map_err(|e| Error::io(&self.path, e))?;
        self.file.sync_all().map_err(|e| Error::io(&self.path, e))
    }

    /// Seals the active log as `wal.log.{seal_ts}` and starts a fresh one.
    /// Rotation is O(1); the sealed segment can be deleted once every record it
    /// holds is applied to the KV store.
    pub fn rotate(&mut self, seal_ts: TxnTimeStamp) -> Result<()> {
        self.file.sync_all().map_err(|e| Error::io(&self.path, e))?;
        let sealed = self.path.with_extension(format!("log.{seal_ts}"));
        std::fs::rename(&self.path, &sealed).map_err(|e| Error::io(&sealed, e))?;
        self.file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(&self.path)
            .map_err(|e| Error::io(&self.path, e))?;
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

    /// Rewrites the WAL keeping only frames with `commit_ts > ts`, streaming
    /// frame by frame so a multi-gigabyte log never has to fit in memory.
    /// Returns the number of records retained. The append handle is reopened on
    /// the new inode.
    pub fn truncate_up_to_streaming(&mut self, ts: TxnTimeStamp) -> Result<usize> {
        let tmp = self.path.with_extension("log.compact");
        let mut kept = 0usize;
        {
            let src = File::open(&self.path).map_err(|e| Error::io(&self.path, e))?;
            let mut rd = BufReader::new(src);
            let out = File::create(&tmp).map_err(|e| Error::io(&tmp, e))?;
            let mut wr = BufWriter::new(out);
            loop {
                let mut header = [0u8; HEADER_LEN];
                match rd.read_exact(&mut header) {
                    Ok(()) => {}
                    Err(e) if e.kind() == std::io::ErrorKind::UnexpectedEof => break,
                    Err(e) => return Err(Error::io(&self.path, e)),
                }
                let magic = u32::from_le_bytes(header[0..4].try_into().unwrap());
                let size = u32::from_le_bytes(header[4..8].try_into().unwrap()) as usize;
                let checksum = u32::from_le_bytes(header[8..12].try_into().unwrap());
                if magic != WAL_MAGIC {
                    break;
                }
                let mut content = vec![0u8; size];
                if rd.read_exact(&mut content).is_err() || crc32_ieee(&content) != checksum {
                    break; // torn frame at the tail
                }
                if content.len() < 16 {
                    break;
                }
                let commit_ts = u64::from_le_bytes(content[8..16].try_into().unwrap());
                if commit_ts > ts {
                    wr.write_all(&header)
                        .and_then(|_| wr.write_all(&content))
                        .map_err(|e| Error::io(&tmp, e))?;
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

    /// Reads all valid records from a WAL file path.
    pub fn read_all(path: impl AsRef<Path>) -> Result<Vec<WalRecord>> {
        let path = path.as_ref();
        match std::fs::read(path) {
            Ok(buf) => Ok(decode_records(&buf)),
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => Ok(Vec::new()),
            Err(e) => Err(Error::io(path, e)),
        }
    }

    /// Replays records with `commit_ts > checkpoint_ts` (recovery helper).
    pub fn replay_since(
        path: impl AsRef<Path>,
        checkpoint_ts: TxnTimeStamp,
        mut f: impl FnMut(&WalRecord) -> Result<()>,
    ) -> Result<usize> {
        let mut n = 0;
        for rec in Self::read_all(path)? {
            if rec.commit_ts > checkpoint_ts {
                f(&rec)?;
                n += 1;
            }
        }
        Ok(n)
    }

    /// Rewrites the WAL keeping only records with `commit_ts > ts` (used at a
    /// checkpoint). Returns the number of records retained.
    pub fn truncate_up_to_ts(path: impl AsRef<Path>, ts: TxnTimeStamp) -> Result<usize> {
        let path = path.as_ref();
        let keep: Vec<WalRecord> = Self::read_all(path)?
            .into_iter()
            .filter(|r| r.commit_ts > ts)
            .collect();
        let tmp = path.with_extension("log.compact");
        {
            let mut f = File::create(&tmp).map_err(|e| Error::io(&tmp, e))?;
            for r in &keep {
                f.write_all(&r.encode_frame())
                    .map_err(|e| Error::io(&tmp, e))?;
            }
            f.sync_all().map_err(|e| Error::io(&tmp, e))?;
        }
        std::fs::rename(&tmp, path).map_err(|e| Error::io(path, e))?;
        Ok(keep.len())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[test]
    fn rotation_seals_the_active_log() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        w.append(&WalRecord::new(1, 1, vec![1])).unwrap();
        w.rotate(1).unwrap();
        w.append(&WalRecord::new(2, 2, vec![2])).unwrap();
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
            w.append(&WalRecord::new(ts, ts, vec![ts as u8])).unwrap();
        }
        assert_eq!(w.truncate_up_to_streaming(6).unwrap(), 4);
        let recs = WalManager::read_all(&path).unwrap();
        assert_eq!(
            recs.iter().map(|r| r.commit_ts).collect::<Vec<_>>(),
            vec![7, 8, 9, 10]
        );
        // the append handle points at the rewritten file
        w.append(&WalRecord::new(11, 11, vec![11])).unwrap();
        assert_eq!(WalManager::read_all(&path).unwrap().len(), 5);
    }

    #[test]
    fn append_and_read_roundtrip() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        w.append(&WalRecord::new(1, 10, b"alpha")).unwrap();
        w.append(&WalRecord::new(2, 20, b"beta")).unwrap();

        let recs = WalManager::read_all(&path).unwrap();
        assert_eq!(recs.len(), 2);
        assert_eq!(recs[0], WalRecord::new(1, 10, b"alpha"));
        assert_eq!(recs[1], WalRecord::new(2, 20, b"beta"));
    }

    #[test]
    fn torn_tail_is_ignored() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        {
            let mut w = WalManager::open(&path).unwrap();
            w.append(&WalRecord::new(1, 10, b"alpha")).unwrap();
        }
        // simulate a crash mid-append: a header claiming more bytes than exist
        {
            let mut f = OpenOptions::new().append(true).open(&path).unwrap();
            f.write_all(&WAL_MAGIC.to_le_bytes()).unwrap();
            f.write_all(&4096u32.to_le_bytes()).unwrap();
            f.write_all(&0u32.to_le_bytes()).unwrap();
            f.sync_all().unwrap();
        }
        let recs = WalManager::read_all(&path).unwrap();
        assert_eq!(recs.len(), 1);
        assert_eq!(recs[0].txn_id, 1);
    }

    #[test]
    fn corrupt_checksum_stops_reading() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        w.append(&WalRecord::new(1, 10, b"alpha")).unwrap();
        drop(w);
        // flip a payload byte
        let mut buf = std::fs::read(&path).unwrap();
        let last = buf.len() - 1;
        buf[last] ^= 0xFF;
        std::fs::write(&path, buf).unwrap();
        assert!(WalManager::read_all(&path).unwrap().is_empty());
    }

    #[test]
    fn replay_since_filters_by_commit_ts() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("wal.log");
        let mut w = WalManager::open(&path).unwrap();
        for i in 1..=5u64 {
            w.append(&WalRecord::new(i, i, format!("p{i}"))).unwrap();
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
            w.append(&WalRecord::new(i, i, format!("p{i}"))).unwrap();
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
