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
    map: Mmap,
}

impl MappedFile {
    /// Maps `path` read-only.
    ///
    /// The mapping stays valid even when the file is rewritten while it is
    /// alive: every publisher writes a sibling temp file and renames it over the
    /// target (`definition::format::write_synced`), which swaps the directory
    /// entry and leaves the inode this mapping refers to untouched. Truncating
    /// in place instead would make the mapping fault with SIGBUS.
    #[allow(unsafe_code)]
    pub fn open(path: impl AsRef<Path>) -> std::io::Result<Self> {
        let file = std::fs::File::open(path)?;
        // SAFETY: the mapping is read-only and the file is immutable while it
        // is mapped (see the function docs). `Mmap` keeps the mapping valid
        // after the descriptor is closed, so the file is dropped at once.
        let map = unsafe { Mmap::map(&file)? };
        Ok(Self::from_mmap(map))
    }

    /// Takes an already-created mapping (the index readers map their file and
    /// hand the result here, so every mapping in the process is counted the
    /// same way).
    pub fn from_mmap(map: Mmap) -> Self {
        MAPPINGS.fetch_add(1, Ordering::Relaxed);
        MAPPED_BYTES.fetch_add(map.len(), Ordering::Relaxed);
        Self { map }
    }

    pub fn len(&self) -> usize {
        self.map.len()
    }

    pub fn is_empty(&self) -> bool {
        self.map.is_empty()
    }
}

impl AsRef<[u8]> for MappedFile {
    fn as_ref(&self) -> &[u8] {
        &self.map
    }
}

impl Deref for MappedFile {
    type Target = [u8];

    fn deref(&self) -> &[u8] {
        &self.map
    }
}

impl Drop for MappedFile {
    fn drop(&mut self) {
        MAPPINGS.fetch_sub(1, Ordering::Relaxed);
        MAPPED_BYTES.fetch_sub(self.map.len(), Ordering::Relaxed);
    }
}

impl std::fmt::Debug for MappedFile {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("MappedFile")
            .field("len", &self.map.len())
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
