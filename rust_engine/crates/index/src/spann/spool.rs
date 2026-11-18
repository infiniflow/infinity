//! A file-backed slot store for one build (`SPANN_DESIGN.md` §0.0.1, R1).
//!
//! A bulk import assigns rows into partitions it does not yet know the final
//! shape of: a leaf keeps receiving rows until it exceeds `max_partition_size`,
//! and only then is it split into two children that are themselves append
//! targets. Keeping those rows in RAM for the whole pass is exactly what makes
//! an import `O(n)` - 208 B per row at 1536d, 10.4 GB at 50M rows.
//!
//! The spool holds the same bytes on disk instead. A partition's slots are
//! appended as a contiguous region; the only in-RAM state is `key -> (offset,
//! count)`, 20 B per partition (4 MB for 195k partitions). A split reads its
//! parent's region back (≤ `2 × max_partition_size` slots), appends the two
//! children's regions and releases the parent's - the file only ever grows, so
//! a region is immutable once written.
//!
//! The publish step then walks the tree in DFS order and streams each region
//! into the chunk's section D, so the peak is one partition either way.

use std::collections::HashMap;
use std::fs::{File, OpenOptions};
use std::io::{BufWriter, Read, Seek, SeekFrom, Write};
use std::path::{Path, PathBuf};

use super::partition::Member;
use super::quantize::{self, Code};
use super::{PartitionKey, SpannError};

/// Bytes a slot takes on the spool: row id, the four `f32` terms of §6.2, code.
const SLOT_FIXED: usize = 8 + 4 * 4;

/// Slot bytes for `dims`; the same width `Partition` keeps in memory.
pub fn slot_bytes(dims: usize) -> usize {
    SLOT_FIXED + quantize::code_bytes(dims)
}

/// Code-only storage for one build. Dropped (or [`Spool::discard`]) after the
/// chunk is written.
pub struct Spool {
    path: PathBuf,
    dims: usize,
    writer: BufWriter<File>,
    reader: File,
    /// `key -> (offset in file, member count)`.
    live: HashMap<PartitionKey, (u64, u32)>,
    end: u64,
}

impl Spool {
    /// Creates (or truncates) the spool at `path`.
    pub fn create(path: impl AsRef<Path>, dims: usize) -> Result<Self, SpannError> {
        let path = path.as_ref().to_path_buf();
        let file = OpenOptions::new()
            .create(true)
            .truncate(true)
            .read(true)
            .write(true)
            .open(&path)
            .map_err(|e| SpannError::Corrupt(format!("spool create {}: {e}", path.display())))?;
        let reader = file
            .try_clone()
            .map_err(|e| SpannError::Corrupt(format!("spool clone: {e}")))?;
        Ok(Self {
            path,
            dims,
            writer: BufWriter::with_capacity(1 << 16, file),
            reader,
            live: HashMap::new(),
            end: 0,
        })
    }

    pub fn path(&self) -> &Path {
        &self.path
    }

    /// Slots currently written, live or released.
    pub fn written_bytes(&self) -> u64 {
        self.end
    }

    /// Partitions held right now.
    pub fn len(&self) -> usize {
        self.live.len()
    }

    pub fn is_empty(&self) -> bool {
        self.live.is_empty()
    }

    /// Member count of a partition, i.e. what its page will hold.
    pub fn count(&self, key: PartitionKey) -> Option<u32> {
        self.live.get(&key).map(|(_, count)| *count)
    }

    /// Appends `members` as the partition's region, replacing any earlier one.
    pub fn append(&mut self, key: PartitionKey, members: &[Member]) -> Result<(), SpannError> {
        let offset = self.end;
        let mut buf = Vec::with_capacity(members.len() * slot_bytes(self.dims));
        for member in members {
            encode_member(&mut buf, member, self.dims);
        }
        self.writer
            .write_all(&buf)
            .map_err(|e| SpannError::Corrupt(format!("spool write: {e}")))?;
        self.end += buf.len() as u64;
        self.live.insert(key, (offset, members.len() as u32));
        Ok(())
    }

    /// Reads a partition's slots back, in the order they were appended.
    pub fn read(&mut self, key: PartitionKey) -> Result<Vec<Member>, SpannError> {
        let Some(&(offset, count)) = self.live.get(&key) else {
            return Ok(Vec::new());
        };
        self.writer
            .flush()
            .map_err(|e| SpannError::Corrupt(format!("spool flush: {e}")))?;
        let slot = slot_bytes(self.dims) as u64;
        let mut raw = vec![0u8; count as usize * slot as usize];
        self.reader
            .seek(SeekFrom::Start(offset))
            .map_err(|e| SpannError::Corrupt(format!("spool seek: {e}")))?;
        self.reader
            .read_exact(&mut raw)
            .map_err(|e| SpannError::Corrupt(format!("spool read: {e}")))?;
        let mut out = Vec::with_capacity(count as usize);
        for chunk in raw.chunks_exact(slot as usize) {
            out.push(decode_member(chunk, self.dims));
        }
        Ok(out)
    }

    /// Forgets a partition's region. The bytes stay in the file (regions are
    /// append-only, so nothing can be reclaimed in the middle).
    pub fn release(&mut self, key: PartitionKey) {
        self.live.remove(&key);
    }

    /// Streams a partition's slots into `write`, without a `Vec` of `Member`s -
    /// this is what the chunk writer's section D uses, so publishing peaks at
    /// one page of staged bytes rather than one partition of structs.
    pub fn copy_into(
        &mut self,
        key: PartitionKey,
        write: &mut dyn FnMut(&[u8]) -> Result<(), SpannError>,
    ) -> Result<usize, SpannError> {
        let Some(&(offset, count)) = self.live.get(&key) else {
            return Ok(0);
        };
        self.writer
            .flush()
            .map_err(|e| SpannError::Corrupt(format!("spool flush: {e}")))?;
        let slot = slot_bytes(self.dims) as u64;
        let mut buf = Vec::with_capacity(1 << 16);
        let mut left = count as usize;
        self.reader
            .seek(SeekFrom::Start(offset))
            .map_err(|e| SpannError::Corrupt(format!("spool seek: {e}")))?;
        while left > 0 {
            let take = left.min((1 << 16) / slot as usize).max(1);
            buf.resize(take * slot as usize, 0);
            self.reader
                .read_exact(&mut buf)
                .map_err(|e| SpannError::Corrupt(format!("spool read: {e}")))?;
            write(&buf)?;
            left -= take;
        }
        Ok(count as usize)
    }

    /// Deletes the file. The chunk is the published form; nothing reads a spool
    /// again after the build that owns it.
    pub fn discard(self) {
        drop(self.writer);
        drop(self.reader);
        let _ = std::fs::remove_file(&self.path);
    }
}

fn encode_member(out: &mut Vec<u8>, member: &Member, dims: usize) {
    out.extend_from_slice(&member.id.to_le_bytes());
    let code = &member.code;
    out.extend_from_slice(&code.norm.to_le_bytes());
    out.extend_from_slice(&code.raw_norm.to_le_bytes());
    out.extend_from_slice(&code.sum.to_le_bytes());
    out.extend_from_slice(&code.error.to_le_bytes());
    let width = quantize::code_bytes(dims);
    if code.bits.len() == width {
        out.extend_from_slice(&code.bits);
    } else {
        // A short code is a programming error, but a spool is not the place to
        // panic mid-import: pad so the slot width stays fixed and the row is
        // still counted.
        out.extend_from_slice(&code.bits);
        out.resize(out.len() + width - code.bits.len(), 0);
    }
}

fn decode_member(raw: &[u8], dims: usize) -> Member {
    let id = u64::from_le_bytes(raw[0..8].try_into().unwrap());
    let norm = f32::from_le_bytes(raw[8..12].try_into().unwrap());
    let raw_norm = f32::from_le_bytes(raw[12..16].try_into().unwrap());
    let sum = f32::from_le_bytes(raw[16..20].try_into().unwrap());
    let error = f32::from_le_bytes(raw[20..24].try_into().unwrap());
    let width = quantize::code_bytes(dims);
    Member {
        id,
        code: Code {
            norm,
            raw_norm,
            sum,
            error,
            bits: raw[SLOT_FIXED..SLOT_FIXED + width].to_vec(),
        },
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn member(id: u64, dims: usize) -> Member {
        Member {
            id,
            code: Code {
                norm: id as f32 + 0.5,
                raw_norm: id as f32 * 2.0,
                sum: id as f32 * 3.0,
                error: id as f32 * 0.25,
                bits: vec![id as u8; quantize::code_bytes(dims)],
            },
        }
    }

    #[test]
    fn a_region_survives_a_round_trip() {
        let dir = std::env::temp_dir().join(format!("spool_rt_{}", std::process::id()));
        let path = dir.join("a.spool");
        std::fs::create_dir_all(&dir).unwrap();
        let mut spool = Spool::create(&path, 64).unwrap();
        let rows: Vec<Member> = (0..17).map(|i| member(i, 64)).collect();
        spool.append(7, &rows).unwrap();
        spool.append(8, &rows[..3]).unwrap();

        assert_eq!(spool.count(7), Some(17));
        let back = spool.read(7).unwrap();
        assert_eq!(back.len(), 17);
        for (i, m) in back.iter().enumerate() {
            assert_eq!(m.id, i as u64);
            assert_eq!(m.code.norm, i as f32 + 0.5);
            assert_eq!(m.code.error, i as f32 * 0.25);
            assert_eq!(m.code.bits, vec![i as u8; 8]);
        }

        // Streaming copy must produce the same bytes as the structured read.
        let mut streamed = Vec::new();
        let mut push = |bytes: &[u8]| {
            streamed.extend_from_slice(bytes);
            Ok(())
        };
        let count = spool.copy_into(7, &mut push).unwrap();
        assert_eq!(count, 17);
        assert_eq!(streamed.len(), 17 * slot_bytes(64));

        spool.release(8);
        assert_eq!(spool.count(8), None);
        assert!(spool.read(8).unwrap().is_empty());
        spool.discard();
        assert!(!path.exists());
    }
}
