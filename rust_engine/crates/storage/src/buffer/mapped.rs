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
/// Bytes whose resident pages have been dropped by [`MappedFile::release_pages`].
static PAGES_RELEASED: AtomicUsize = AtomicUsize::new(0);

/// Every live mapping, weakly, in a slot table.
///
/// This is what lets a reclaim pass find the pages **without asking the
/// filesystem**: the alternative - walk the data directory and `open` every file
/// to `fadvise` it - costs one syscall per file on the tree, on a path that runs
/// when the process is already at its memory limit.
///
/// A slot, not a bare `Vec` that only prunes during a pass: a long-running
/// server maps and unmaps blocks continuously (the block cache evicts, a scan
/// remaps), and a dead weak entry per mapping would grow the table forever
/// whenever the process stays under its watermark and no pass runs. Each
/// [`MappedFile`] therefore owns one slot and frees it in its `Drop`, and the
/// free list reuses the hole, so the table's length is the peak number of
/// concurrent mappings rather than the number ever created.
struct Registry {
    slots: Vec<Option<std::sync::Weak<Mmap>>>,
    free: Vec<usize>,
}

static REGISTRY: std::sync::Mutex<Registry> = std::sync::Mutex::new(Registry {
    slots: Vec::new(),
    free: Vec::new(),
});

/// Registers `map` and returns its slot. `usize::MAX` when the registry lock is
/// poisoned: a mapping that cannot be reclaimed is still a valid mapping.
fn register(map: &std::sync::Arc<Mmap>) -> usize {
    let Ok(mut registry) = REGISTRY.lock() else {
        return usize::MAX;
    };
    let weak = std::sync::Arc::downgrade(map);
    match registry.free.pop() {
        Some(slot) => {
            registry.slots[slot] = Some(weak);
            slot
        }
        None => {
            registry.slots.push(Some(weak));
            registry.slots.len() - 1
        }
    }
}

fn unregister(slot: usize) {
    if slot == usize::MAX {
        return;
    }
    let Ok(mut registry) = REGISTRY.lock() else {
        return;
    };
    if let Some(entry) = registry.slots.get_mut(slot) {
        if entry.is_some() {
            *entry = None;
            registry.free.push(slot);
        }
    }
}

/// Where the next [`release_mapped_pages`] pass starts.
///
/// A pass stops once it believes it has released `want` bytes, and the figure it
/// counts is the mapping's length - an upper bound, since not every page of a
/// mapping is resident. Starting from the same entry every time would therefore
/// keep advising the same few mappings while the rest of a tree larger than
/// `want` was never touched. Rotating makes successive passes sweep the whole
/// registry; the caller re-measures afterwards, so a pass that over-counted is
/// followed by one that continues where it left off.
static RELEASE_CURSOR: AtomicUsize = AtomicUsize::new(0);

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
    /// Shared with [`REGISTRY`], which needs a weak handle to reach the pages of
    /// a mapping it does not own. `Arc` is one refcount on a mapping that is
    /// already shared by every reader of the file.
    map: Option<std::sync::Arc<Mmap>>,
    /// The slice of the mapping this reader sees.
    ///
    /// A file written by this engine is mapped whole (`0..len`). A file that
    /// lives inside a composed persistence object is one part of that object,
    /// so the reader maps the object and addresses `part_offset..part_offset +
    /// part_size` ([`Self::open_range`]). Every decoder slices whatever this
    /// buffer holds, so a range behaves exactly like a file of that length.
    range: std::ops::Range<usize>,
    /// This mapping's [`Registry`] slot; `usize::MAX` for the empty mapping.
    slot: usize,
}

impl MappedFile {
    /// A zero-length mapping, for a file that exists but holds no bytes.
    pub fn empty() -> Self {
        Self {
            map: None,
            range: 0..0,
            slot: usize::MAX,
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
        let map = std::sync::Arc::new(map);
        MAPPINGS.fetch_add(1, Ordering::Relaxed);
        MAPPED_BYTES.fetch_add(len, Ordering::Relaxed);
        let slot = register(&map);
        Ok(Self {
            map: Some(map),
            range: offset..end,
            slot,
        })
    }

    /// Takes an already-created mapping (the index readers map their file and
    /// hand the result here, so every mapping in the process is counted the
    /// same way).
    pub fn from_mmap(map: Mmap) -> Self {
        let len = map.len();
        MAPPINGS.fetch_add(1, Ordering::Relaxed);
        MAPPED_BYTES.fetch_add(len, Ordering::Relaxed);
        let map = std::sync::Arc::new(map);
        let slot = register(&map);
        Self {
            map: Some(map),
            range: 0..len,
            slot,
        }
    }

    /// Asks the kernel to drop the resident pages of this mapping.
    ///
    /// A mapped index chunk's slot pages are clean file-backed pages: dropping
    /// them costs a later re-read from the object/file and nothing else, which
    /// is the only way back to a small RSS without closing the index. Returns
    /// the bytes whose pages were dropped (0 for the empty mapping).
    ///
    /// Correct while readers hold the mapping: `MADV_DONTNEED` on a read-only
    /// file mapping only discards cache; the next access faults the page back
    /// in from the file, which is immutable once published.
    #[allow(unsafe_code)]
    pub fn release_pages(&self) -> usize {
        let Some(map) = &self.map else {
            return 0;
        };
        #[cfg(unix)]
        {
            // SAFETY: every mapping this process makes is read-only over an
            // immutable file (a publisher renames a new file over the old one,
            // never truncating or writing in place), so `MADV_DONTNEED` drops
            // clean pages and the next access faults the same bytes back in.
            // No borrow of the mapped slice is created here, and a concurrent
            // reader outside this call sees unchanged contents - the mapping is
            // never written through.
            if unsafe { map.unchecked_advise(memmap2::UncheckedAdvice::DontNeed) }.is_err() {
                return 0;
            }
        }
        let len = self.range.len();
        PAGES_RELEASED.fetch_add(len, Ordering::Relaxed);
        len
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
            // Free the slot before the `Arc` goes: a reclaim pass holding the
            // registry lock cannot miss this mapping halfway through its drop.
            unregister(self.slot);
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

/// How many mapped bytes have had their resident pages dropped.
pub fn pages_released_bytes() -> usize {
    PAGES_RELEASED.load(Ordering::Relaxed)
}

/// Drops the resident pages of live mappings until `want` bytes have been
/// released (all of them for [`usize::MAX`]), returning what was released.
///
/// The pass walks [`REGISTRY`] - an in-memory vector - instead of the data
/// directory, so it costs one `madvise` per mapping and no `open`/`stat` per
/// file. Every index reader in the process goes through [`MappedFile`], so one
/// call covers the block files, the SPANN chunks, the text segments and the
/// secondary structures alike.
///
/// Correct while readers hold a mapping: `MADV_DONTNEED` on a read-only mapping
/// of an immutable file only drops clean pages, and the next access faults them
/// back in.
#[allow(unsafe_code)]
pub fn release_mapped_pages(want: usize) -> usize {
    let Ok(mut registry) = REGISTRY.lock() else {
        return 0;
    };
    // Prune the owners that are gone. `MappedFile::drop` frees its slot itself,
    // so a dead entry only survives a poisoned lock on that path; collecting it
    // here keeps the table from keeping the hole forever. A pass is bounded by
    // the target, not by the number of mappings: the caller is over its limit
    // and does not need the whole address space re-read afterwards.
    for slot in 0..registry.slots.len() {
        let dead = registry.slots[slot]
            .as_ref()
            .is_some_and(|weak| weak.strong_count() == 0);
        if dead {
            registry.slots[slot] = None;
            registry.free.push(slot);
        }
    }
    let len = registry.slots.len();
    if len == 0 {
        return 0;
    }
    // Start where the last pass stopped (see [`RELEASE_CURSOR`]) and wrap, so a
    // tree larger than one pass's target is covered over successive passes
    // instead of the same head being advised every second.
    let start = RELEASE_CURSOR.fetch_add(1, Ordering::Relaxed) % len;
    let mut released = 0usize;
    for step in 0..len {
        if released >= want {
            break;
        }
        let Some(map) = registry.slots[(start + step) % len]
            .as_ref()
            .and_then(std::sync::Weak::upgrade)
        else {
            continue;
        };
        #[cfg(unix)]
        {
            // SAFETY: the mapping is read-only over an immutable file (a
            // publisher renames a new file over the old one, never truncating or
            // writing in place); no borrow of the bytes is created here, and a
            // concurrent reader sees unchanged contents - only the cached pages
            // go away.
            if unsafe { map.unchecked_advise(memmap2::UncheckedAdvice::DontNeed) }.is_err() {
                continue;
            }
        }
        released += map.len();
        PAGES_RELEASED.fetch_add(map.len(), Ordering::Relaxed);
    }
    released
}
