/// The full-text index file-family suffixes (`index_defines.cppm:55-58`).
///
/// A chunk's files are `{index_dir}/{base_name}{suffix}`, where `index_dir` is
/// [`crate::format::segment_index_dir`] and the suffixes are these.
pub const FULLTEXT_DICT_SUFFIX: &str = ".dic";

pub const FULLTEXT_POSTING_SUFFIX: &str = ".pos";

pub const FULLTEXT_SPILL_SUFFIX: &str = ".spill";

pub const FULLTEXT_LENGTH_SUFFIX: &str = ".len";

/// The persistent members of a full-text chunk (`.spill` is transient, removed
/// after the chunk is built).
pub const FULLTEXT_CHUNK_FILES: [&str; 3] = [
    FULLTEXT_DICT_SUFFIX,
    FULLTEXT_POSTING_SUFFIX,
    FULLTEXT_LENGTH_SUFFIX,
];

/// `{index_dir}/{base_name}{suffix}` — the full-text file path shape
/// (`disk_segment_reader_impl.cpp:51-57`).
pub fn fulltext_chunk_file(index_dir: &str, base_name: &str, suffix: &str) -> String {
    format!("{index_dir}/{base_name}{suffix}")
}

/// `{index_root}/seg_{segment}` — the directory one `ROW_ID` segment's chunks
/// live in.
///
/// `index_root` is the index's own directory (`.../idx_{index_id}`), so this is
/// the reference's `SegmentIndexMeta::GetSegmentIndexDir`
/// (`src/storage/catalog/meta/segment_index_meta_impl.cpp:288`): every index
/// kind keeps one subdirectory per segment, and the catalog's `idx_chunk`
/// records name the same place.
pub fn segment_chunk_dir(index_root: &str, segment: u64) -> String {
    format!("{index_root}/seg_{segment}")
}
