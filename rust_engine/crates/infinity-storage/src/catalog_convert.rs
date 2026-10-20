//! Moving a catalog store between the two substrates (fjall and RocksDB).
//!
//! The C++ catalog key space is *logical*: the reference reads it out of a
//! RocksDB, and the port writes the same keys into whichever store it uses.
//! A directory the port wrote therefore differs from one the reference wrote
//! only in the storage engine underneath the keys - the data tree, the block
//! mirror and the WAL checkpoint are already byte-compatible - so the two can
//! be converted into each other instead of the engine having to carry both
//! substrates.
//!
//! The copy goes through [`MetaStore`] and not through the files: the fjall
//! store compresses large values (LZ4) on the way in and decompresses them on
//! the way out, so a file-level copy would carry compressed bytes into
//! RocksDB, where the reference would read garbage.
//!
//! Two key spaces live in the catalog store:
//!
//! * the **C++ key space** (`catalog|`, `tbl|`, `col|`, `idx|`, `seg|`,
//!   `blk|`, `blk_col|`, `db|`) - the reference's, and
//! * the port's **native key space** (`tbl:`, `col:`, `idx:`, `db:`, `row:`,
//!   `__counter:…`) - separated by the `:` instead of the `|`.
//!
//! A conversion towards the reference copies the C++ key space; a conversion
//! towards the port copies it as well, because the port imports the C++ layout
//! into its native catalog when it opens a directory the reference wrote
//! (`Catalog::import_cpp_layout`).

use infinity_common::{Error, Result};

use crate::metastore::MetaStore;

/// The key prefixes the reference reads (`KeyEncode::*`,
/// `src/storage/common/kv_code_impl.cpp`).
pub const CPP_CATALOG_PREFIXES: [&str; 8] = [
    "catalog|", "db|", "tbl|", "col|", "idx|", "seg|", "blk|", "blk_col|",
];

/// Whether a key belongs to the C++ catalog key space.
pub fn is_cpp_catalog_key(key: &[u8]) -> bool {
    CPP_CATALOG_PREFIXES
        .iter()
        .any(|prefix| key.starts_with(prefix.as_bytes()))
}

/// What a conversion moved.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub struct CatalogCopy {
    /// Keys written to the target.
    pub keys: usize,
    /// Bytes of value data written.
    pub bytes: usize,
    /// Keys of the source that were not part of the copied key space.
    pub skipped: usize,
}

/// Copies the C++ catalog key space from `src` into `dst`.
///
/// The values are copied as the stores see them: a fjall source hands out the
/// decompressed bytes, a RocksDB source the raw ones, and the target encodes
/// them its own way. The result is the same logical content either way.
pub fn copy_cpp_catalog(src: &dyn MetaStore, dst: &dyn MetaStore) -> Result<CatalogCopy> {
    let mut out = CatalogCopy::default();
    let mut batch: Vec<(Vec<u8>, Option<Vec<u8>>)> = Vec::new();
    for prefix in CPP_CATALOG_PREFIXES {
        for (key, value) in src.scan_prefix(prefix.as_bytes())? {
            if !is_cpp_catalog_key(&key) {
                out.skipped += 1;
                continue;
            }
            out.keys += 1;
            out.bytes += value.len();
            batch.push((key, Some(value)));
        }
    }
    if !batch.is_empty() {
        dst.atomic_batch(&batch)?;
    }
    Ok(out)
}

/// Copies every key of `src` into `dst`.
///
/// Used for the row store, whose keys are the port's own (`row:…`) and have no
/// C++ counterpart: the reference reads the block mirror instead, so the row
/// store only has to be openable by the port that will use it.
pub fn copy_all(src: &dyn MetaStore, dst: &dyn MetaStore) -> Result<CatalogCopy> {
    let mut out = CatalogCopy::default();
    let mut batch: Vec<(Vec<u8>, Option<Vec<u8>>)> = Vec::new();
    for (key, value) in src.scan_prefix(b"")? {
        out.keys += 1;
        out.bytes += value.len();
        batch.push((key, Some(value)));
    }
    if !batch.is_empty() {
        dst.atomic_batch(&batch)?;
    }
    Ok(out)
}

/// Replaces the store at `path` with a copy of the C++ key space of `src`.
///
/// The target is built beside the source and swapped in, so a failure leaves
/// the original store untouched.
pub fn convert_store(
    src: &dyn MetaStore,
    path: &std::path::Path,
    make_target: impl FnOnce(&std::path::Path) -> Result<Box<dyn MetaStore>>,
) -> Result<CatalogCopy> {
    let staging = path.with_extension("converted");
    if staging.exists() {
        std::fs::remove_dir_all(&staging).map_err(|e| Error::io(&staging, e))?;
    }
    let report = {
        let target = make_target(&staging)?;
        let report = copy_cpp_catalog(src, target.as_ref())?;
        target.flush()?;
        report
    };
    let backup = path.with_extension("converted.old");
    if backup.exists() {
        std::fs::remove_dir_all(&backup).map_err(|e| Error::io(&backup, e))?;
    }
    if path.exists() {
        std::fs::rename(path, &backup).map_err(|e| Error::io(path, e))?;
    }
    std::fs::rename(&staging, path).map_err(|e| Error::io(path, e))?;
    if backup.exists() {
        std::fs::remove_dir_all(&backup).map_err(|e| Error::io(&backup, e))?;
    }
    Ok(report)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::metastore::FjallMetaStore;
    use std::sync::Arc;

    fn store(dir: &std::path::Path) -> Arc<dyn MetaStore> {
        Arc::new(FjallMetaStore::open(dir, "catalog").unwrap())
    }

    #[test]
    fn the_cpp_key_space_is_the_pipe_separated_one() {
        assert!(is_cpp_catalog_key(b"catalog|db|default_db|1"));
        assert!(is_cpp_catalog_key(b"tbl|1|0|next_segment_id"));
        assert!(is_cpp_catalog_key(b"seg|1|0|0|first_delete_ts"));
        // The port's own keys use a colon and must not travel to the reference.
        assert!(!is_cpp_catalog_key(b"tbl:0000000000000001"));
        assert!(!is_cpp_catalog_key(b"row:default_db.t:0"));
        assert!(!is_cpp_catalog_key(b"__counter:table"));
    }

    #[test]
    fn a_conversion_moves_the_cpp_keys_with_their_values() {
        let dir = tempfile::tempdir().unwrap();
        let src = store(&dir.path().join("src"));
        let dst = store(&dir.path().join("dst"));
        src.put(b"catalog|db|default_db|1", b"1").unwrap();
        src.put(b"tbl|1|0|next_segment_id", b"1").unwrap();
        src.put(b"tbl:0000000000000001", b"native").unwrap();
        let report = copy_cpp_catalog(src.as_ref(), dst.as_ref()).unwrap();
        assert_eq!(report.keys, 2);
        assert_eq!(dst.get(b"catalog|db|default_db|1").unwrap().unwrap(), b"1");
        assert_eq!(dst.get(b"tbl|1|0|next_segment_id").unwrap().unwrap(), b"1");
        assert_eq!(
            dst.get(b"tbl:0000000000000001").unwrap(),
            None,
            "the port's own keys do not belong to the reference's key space"
        );
    }

    #[test]
    fn a_conversion_keeps_large_values_intact() {
        // The fjall store compresses values of 512 bytes and more; a
        // conversion must hand the target the original bytes.
        let dir = tempfile::tempdir().unwrap();
        let src = store(&dir.path().join("src"));
        let dst = store(&dir.path().join("dst"));
        let big = vec![b'x'; 4096];
        src.put(b"catalog|tbl|1|t|1", &big).unwrap();
        copy_cpp_catalog(src.as_ref(), dst.as_ref()).unwrap();
        assert_eq!(dst.get(b"catalog|tbl|1|t|1").unwrap().unwrap(), big);
    }
}
