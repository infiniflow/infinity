//! The publish marker of one sealed full-text chunk: `{base_name}.ok`.
//!
//! A chunk is three files (`.dic`, `.pos`, `.len`) and the reference writes
//! them one after another with no transaction around the set, so a crash can
//! leave `.dic` in place while `.pos`/`.len` are missing or still the previous
//! generation's. Every file is published atomically on its own
//! (`definition::format::write_synced` renames a sibling temp file), but the
//! *set* is not - and `load_sealed` discovers chunks by scanning `*.dic`, so it
//! would pick such a chunk up.
//!
//! This marker closes that window. It is written **last**, and it records the
//! exact byte length of each of the three files plus the two numbers the chunk
//! cannot recompute after deletion (`live_docs`, `total_len`). Its presence
//! means "all three files reached the disk at the sizes below"; a chunk whose
//! `.ok` is missing is treated as a failed publish by the loader once it also
//! fails a structural check, and a chunk whose files do not match the lengths
//! here is refused instead of being decoded into wrong rows.
//!
//! The marker lives beside the chunk and is not part of the on-disk format the
//! reference reads: `.ok` is not one of `.dic`/`.pos`/`.len`, so an engine that
//! does not know it simply ignores the file. A chunk written by the reference
//! (or by this port before the marker existed) has no `.ok`; the loader then
//! falls back to the structural check alone, so old data stays readable.
//!
//! `live_docs`/`total_len` are what compaction needs to score the chunk the way
//! the predicate sees it: after a merge drops the rows that were deleted,
//! `span_docs` still spans the row range (a posting's `docid_t` is
//! `row - base_row_id`, so the `.len` slots cannot move) while `live_docs` is
//! the BM25 `N` and `total_len` its `sum(dl)`.

use std::path::{Path, PathBuf};

use crate::format::FormatError;

/// `FTCM` in little endian, so a truncated or foreign file is not read as a
/// manifest.
const MAGIC: u32 = 0x4d_43_54_46;

/// Layout version. A manifest of another version is ignored rather than
/// misread: the loader falls back to the structural check, which either accepts
/// the chunk or refuses it.
pub const MANIFEST_VERSION: u32 = 1;

/// Bytes of one manifest: magic + version + 3 × u32 (span, live, reserved)
/// + `total_len` + the three file lengths.
pub const MANIFEST_BYTES: usize = 4 + 4 + 12 + 8 + 24;

/// What a predicate still sees of a chunk once its deleted rows are gone:
/// BM25's `N` and its `sum(dl)`.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct LiveStats {
    pub docs: u32,
    pub total_len: u64,
}

impl LiveStats {
    /// The documents of `docs` a predicate can actually return.
    ///
    /// A slot with `len == 0` is a row without text (the reference's
    /// `InsertGap`, or a document whose text produced no term): it has no
    /// posting, so counting it in BM25's `N` would shrink every score against a
    /// document no query can reach. The merge path applies the same rule
    /// (`ColumnIndexMerger::merge_with`), which is what keeps a sealed chunk and
    /// the chunk it merges into describing the corpus the same way.
    pub fn of(docs: &[(u64, u32)]) -> Self {
        let mut live = Self::default();
        for (_, len) in docs {
            if *len > 0 {
                live.docs += 1;
                live.total_len += u64::from(*len);
            }
        }
        live
    }

    /// Compact the statistics, dropping the rows `deleted` names.
    pub fn without(docs: &[(u64, u32)], deleted: &std::collections::HashSet<u64>) -> Self {
        let mut live = Self::default();
        for (row, len) in docs {
            if *len > 0 && !deleted.contains(row) {
                live.docs += 1;
                live.total_len += u64::from(*len);
            }
        }
        live
    }

    /// Recompute the statistics over a chunk's `.len` array.
    pub fn over_slots(
        lens: &[u32],
        deleted: Option<&std::collections::HashSet<u64>>,
        base: u64,
    ) -> Self {
        let mut live = Self::default();
        for (offset, len) in lens.iter().enumerate() {
            if *len == 0 {
                continue;
            }
            if deleted.is_some_and(|deleted| deleted.contains(&(base + offset as u64))) {
                continue;
            }
            live.docs += 1;
            live.total_len += u64::from(*len);
        }
        live
    }
}

/// The publish marker of one chunk.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct ChunkManifest {
    /// `.len` slots, i.e. the row span of the chunk (`row_range().1 - base + 1`).
    pub span_docs: u32,
    /// Documents a predicate still sees: BM25's `N`.
    pub live_docs: u32,
    /// `sum(dl)` over the live documents: BM25's corpus length.
    pub total_len: u64,
    pub dic_len: u64,
    pub pos_len: u64,
    pub len_len: u64,
}

impl ChunkManifest {
    /// `{index_dir}/{base_name}.ok`.
    pub fn path(index_dir: &str, base_name: &str) -> PathBuf {
        Path::new(index_dir).join(format!("{base_name}.ok"))
    }

    /// Writes the marker atomically (temp file + fsync + rename).
    ///
    /// It has to be the **last** file of the set: a chunk is only complete once
    /// every file it names is on the disk at the size it records.
    pub fn write(&self, index_dir: &str, base_name: &str) -> Result<(), FormatError> {
        let mut bytes = Vec::with_capacity(MANIFEST_BYTES);
        bytes.extend_from_slice(&MAGIC.to_le_bytes());
        bytes.extend_from_slice(&MANIFEST_VERSION.to_le_bytes());
        bytes.extend_from_slice(&self.span_docs.to_le_bytes());
        bytes.extend_from_slice(&self.live_docs.to_le_bytes());
        bytes.extend_from_slice(&0u32.to_le_bytes()); // reserved
        bytes.extend_from_slice(&self.total_len.to_le_bytes());
        bytes.extend_from_slice(&self.dic_len.to_le_bytes());
        bytes.extend_from_slice(&self.pos_len.to_le_bytes());
        bytes.extend_from_slice(&self.len_len.to_le_bytes());
        crate::chunk_store::chunk_store()
            .write_dedicated(&Self::path(index_dir, base_name), &bytes)
            .map_err(|error| FormatError::Io(error.to_string()))
    }

    /// Reads the marker: `Ok(None)` when it is absent - a chunk the reference
    /// wrote (or an older build of this port) has none - and an error for
    /// anything else.
    ///
    /// Only `NotFound` selects the legacy path. A read that failed for another
    /// reason, or a marker that is truncated, of another version or not a
    /// manifest at all, must not look like "no marker": the caller would then
    /// fall back to `matches_files`' structural check and rebuild a chunk whose
    /// published lengths it could not actually compare.
    pub fn read(index_dir: &str, base_name: &str) -> Result<Option<Self>, FormatError> {
        let bytes = match crate::chunk_store::chunk_store().read(&Self::path(index_dir, base_name))
        {
            Ok(bytes) => bytes,
            Err(error) if error.kind() == std::io::ErrorKind::NotFound => return Ok(None),
            Err(error) => return Err(FormatError::Io(error.to_string())),
        };
        if bytes.len() != MANIFEST_BYTES {
            return Err(FormatError::Invalid(format!(
                "chunk manifest {base_name}.ok is {} bytes, not {MANIFEST_BYTES}",
                bytes.len()
            )));
        }
        let u32_at = |at: usize| -> u32 {
            u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap_or([0; 4]))
        };
        let u64_at = |at: usize| -> u64 {
            u64::from_le_bytes(bytes[at..at + 8].try_into().unwrap_or([0; 8]))
        };
        if u32_at(0) != MAGIC {
            return Err(FormatError::Invalid(format!(
                "chunk manifest {base_name}.ok does not start with its magic"
            )));
        }
        if u32_at(4) != MANIFEST_VERSION {
            return Err(FormatError::Invalid(format!(
                "chunk manifest {base_name}.ok is version {}, not {MANIFEST_VERSION}",
                u32_at(4)
            )));
        }
        Ok(Some(Self {
            span_docs: u32_at(8),
            live_docs: u32_at(12),
            total_len: u64_at(20),
            dic_len: u64_at(28),
            pos_len: u64_at(36),
            len_len: u64_at(44),
        }))
    }

    /// Whether the three files on the disk are exactly the ones this marker
    /// describes. A length mismatch is a chunk whose file set was published in
    /// two generations (a seal retried, or a rebuild that reused the base name),
    /// which decodes into wrong rows instead of failing.
    pub fn matches_files(&self, dic: u64, pos: u64, len: u64) -> bool {
        self.dic_len == dic && self.pos_len == pos && self.len_len == len
    }
}

/// Removes a chunk's three files and its marker. Used for a chunk that failed
/// validation: keeping it would make every open retry it, and loading a stale
/// generation beside its successor would count the rows twice.
pub fn remove_chunk(index_dir: &str, base_name: &str) {
    let store = crate::chunk_store::chunk_store();
    for suffix in [".dic", ".pos", ".len", ".ok"] {
        // Best effort by design: this drops a chunk nothing references any
        // more, and a retry costs nothing. The flow that decides whether a
        // **live** chunk may be forgotten is `reclaim_deprecated_chunks`, which
        // does check the store's answer.
        let _ = store.remove(&Path::new(index_dir).join(format!("{base_name}{suffix}")));
    }
}
