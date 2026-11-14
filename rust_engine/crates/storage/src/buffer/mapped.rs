//! Read-only file mappings, shared by every reader in the process.
//!
//! A sealed block's files and an index chunk are immutable once written, so a
//! reader maps them and decodes straight out of the mapping. That is what the
//! reference's `BufferManager` achieves with an owned copy plus an LRU: here the
//! OS page cache holds the pages, the mapping (address space, not memory) is
//! shared by every reader of the same file, and no block is ever read into the
//! heap.
//!
//! The two counters below back `SHOW BUFFERS` / `buffer_pool_usage`: they say how
//! many mappings the process holds and how many bytes they cover.

use std::ops::Deref;
use std::path::Path;
use std::sync::atomic::{AtomicUsize, Ordering};

use memmap2::Mmap;

/// Live mappings in this process.
static MAPPINGS: AtomicUsize = AtomicUsize::new(0);
/// Bytes covered by the live mappings.
static MAPPED_BYTES: AtomicUsize = AtomicUsize::new(0);

/// One read-only mapping of a file.
///
/// It derefs to the mapped bytes, so a decoder reads cells straight out of the
/// page cache. Dropping it unmaps the file and takes it off the counters.
pub struct MappedFile {
    /// `None` is the empty mapping: `Mmap::map` refuses a zero-length file
    /// (`EINVAL`), and an index chunk legitimately has an empty `.len` (a
    /// segment whose documents all carry no text). Readers slice this buffer,
    /// so the empty case has to behave like a zero-length file rather than a
    /// failure.
    map: Option<Mmap>,
    /// The slice of the mapping this reader sees.
    ///
    /// A file written by this engine is mapped whole (`0..len`). A file that
    /// lives inside a composed persistence object is one part of that object,
    /// so the reader maps the object and addresses `part_offset..part_offset +
    /// part_size` ([`Self::open_range`]). Every decoder slices whatever this
    /// buffer holds, so a range behaves exactly like a file of that length.
    range: std::ops::Range<usize>,
}

impl MappedFile {
    /// A zero-length mapping, for a file that exists but holds no bytes.
    pub fn empty() -> Self {
        Self {
            map: None,
            range: 0..0,
        }
    }

    /// Maps `path` read-only, through the installed [`crate::chunk_store`].
    ///
    /// This is the read half of the reference's rule that every data file goes
    /// through the persistence manager (`pm->GetObjCache` +
    /// `VirtualStore::MmapFile(pm->GetObjPath(...))`). With the direct-disk
    /// store this is [`Self::open_path`]; with a persistence profile the bytes
    /// are resolved to the object that holds them when no local copy exists.
    pub fn open(path: impl AsRef<Path>) -> std::io::Result<Self> {
        crate::chunk_store::chunk_store().open(path.as_ref())
    }

    /// Maps a real file, read-only.
    ///
    /// The mapping stays valid even when the file is rewritten while it is
    /// alive: every publisher writes a sibling temp file and renames it over the
    /// target (`definition::format::write_synced`), which swaps the directory
    /// entry and leaves the inode this mapping refers to untouched. Truncating
    /// in place instead would make the mapping fault with SIGBUS.
    #[allow(unsafe_code)]
    pub fn open_path(path: impl AsRef<Path>) -> std::io::Result<Self> {
        let file = std::fs::File::open(path)?;
        // A zero-length file cannot be mapped; it is an empty mapping instead.
        if file.metadata()?.len() == 0 {
            return Ok(Self::empty());
        }
        // SAFETY: the mapping is read-only and the file is immutable while it
        // is mapped (see the function docs). `Mmap` keeps the mapping valid
        // after the descriptor is closed, so the file is dropped at once.
        let map = unsafe { Mmap::map(&file)? };
        Ok(Self::from_mmap(map))
    }

    /// Maps `path` read-only and exposes `offset..offset + len` of it.
    ///
    /// This is how a chunk inside a composed persistence object is read: the
    /// object file holds many parts and the [`crate::persistence::ObjAddr`]
    /// names one of them. A zero `len` is an empty mapping, and a range that
    /// does not fit the file is an error rather than a short read - a truncated
    /// object means the record and the bytes disagree, which a decoder must not
    /// paper over.
    #[allow(unsafe_code)]
    pub fn open_range(path: impl AsRef<Path>, offset: usize, len: usize) -> std::io::Result<Self> {
        if len == 0 {
            return Ok(Self::empty());
        }
        let file = std::fs::File::open(path)?;
        let file_len = file.metadata()?.len() as usize;
        let end = offset
            .checked_add(len)
            .filter(|end| *end <= file_len)
            .ok_or_else(|| {
                std::io::Error::new(
                    std::io::ErrorKind::InvalidData,
                    format!(
                        "range {offset}..{} does not fit a {file_len}-byte file",
                        offset.saturating_add(len)
                    ),
                )
            })?;
        // SAFETY: the mapping is read-only and the object file is immutable
        // while it is mapped (a persisted object is replaced, never truncated
        // in place).
        let map = unsafe { Mmap::map(&file)? };
        MAPPINGS.fetch_add(1, Ordering::Relaxed);
        MAPPED_BYTES.fetch_add(len, Ordering::Relaxed);
        Ok(Self {
            map: Some(map),
            range: offset..end,
        })
    }

    /// Takes an already-created mapping (the index readers map their file and
    /// hand the result here, so every mapping in the process is counted the
    /// same way).
    pub fn from_mmap(map: Mmap) -> Self {
        let len = map.len();
        MAPPINGS.fetch_add(1, Ordering::Relaxed);
        MAPPED_BYTES.fetch_add(len, Ordering::Relaxed);
        Self {
            map: Some(map),
            range: 0..len,
        }
    }

    /// The file this mapping covers, while it is alive.
    pub fn len(&self) -> usize {
        self.range.len()
    }

    pub fn is_empty(&self) -> bool {
        self.range.is_empty()
    }

    fn bytes(&self) -> &[u8] {
        match &self.map {
            Some(map) => &map[self.range.clone()],
            None => &[],
        }
    }
}

impl AsRef<[u8]> for MappedFile {
    fn as_ref(&self) -> &[u8] {
        self.bytes()
    }
}

impl Deref for MappedFile {
    type Target = [u8];

    fn deref(&self) -> &[u8] {
        self.bytes()
    }
}

impl Drop for MappedFile {
    fn drop(&mut self) {
        if self.map.is_some() {
            MAPPINGS.fetch_sub(1, Ordering::Relaxed);
            MAPPED_BYTES.fetch_sub(self.range.len(), Ordering::Relaxed);
        }
    }
}

impl std::fmt::Debug for MappedFile {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("MappedFile")
            .field("len", &self.len())
            .finish()
    }
}

/// How many files this process has mapped.
pub fn mapping_count() -> usize {
    MAPPINGS.load(Ordering::Relaxed)
}

/// How many bytes the live mappings cover.
pub fn mapped_bytes() -> usize {
    MAPPED_BYTES.load(Ordering::Relaxed)
}
