//! `infinity-catalog-convert` — move a data directory's stores between the two
//! substrates (fjall and RocksDB).
//!
//! The C++ catalog key space is logical: the reference reads it out of a
//! RocksDB, the port writes the same keys into fjall. The two directories then
//! differ only in the storage engine underneath the keys, so they can be
//! converted into each other instead of the engine carrying both substrates.
//!
//! ```text
//! infinity-catalog-convert --dir <data-dir> --from fjall --to rocksdb
//! ```
//!
//! converts `<data-dir>/catalog` and `<data-dir>/persistence` in place (the
//! target is built beside the source and swapped in, so a failure leaves the
//! original untouched). Use it to hand a directory to the reference
//! (fjall → rocksdb) or to make a reference-written directory readable by a
//! fjall-only engine (rocksdb → fjall).

use std::path::{Path, PathBuf};
use std::process::ExitCode;

use infinity_common::{Error, Result};
use infinity_storage::catalog_convert::{copy_all, copy_cpp_catalog};
use infinity_storage::metastore::MetaStore;
use infinity_storage::{FjallMetaStore, RocksMetaStore};

/// Which substrate a store lives on.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum Backend {
    Fjall,
    RocksDb,
}

impl Backend {
    fn parse(text: &str) -> Result<Self> {
        match text.to_ascii_lowercase().as_str() {
            "fjall" => Ok(Backend::Fjall),
            "rocksdb" | "rocks" => Ok(Backend::RocksDb),
            other => Err(Error::Invalid(format!(
                "unknown backend '{other}' (expected fjall or rocksdb)"
            ))),
        }
    }

    fn name(self) -> &'static str {
        match self {
            Backend::Fjall => "fjall",
            Backend::RocksDb => "rocksdb",
        }
    }

    /// Opens the store at `path`; the fjall store lives in the keyspace
    /// `keyspace`, the RocksDB store in the database directory itself.
    fn open(self, path: &Path, keyspace: &str) -> Result<Box<dyn MetaStore>> {
        match self {
            Backend::Fjall => Ok(Box::new(FjallMetaStore::open(path, keyspace)?)),
            Backend::RocksDb => Ok(Box::new(RocksMetaStore::open(path)?)),
        }
    }
}

/// The two stores of a data directory, with the keyspace each one uses in
/// fjall and whether it holds the reference's key space.
const STORES: [(&str, &str, bool); 2] = [
    // (directory, fjall keyspace, holds the C++ key space)
    ("catalog", "catalog", true),
    ("persistence", "default", false),
];

fn usage() -> ExitCode {
    eprintln!(
        "usage: infinity-catalog-convert --dir <data-dir> --from <fjall|rocksdb> --to <fjall|rocksdb> [--dry-run]"
    );
    ExitCode::from(2)
}

fn main() -> ExitCode {
    let mut dir: Option<PathBuf> = None;
    let mut from: Option<Backend> = None;
    let mut to: Option<Backend> = None;
    let mut dry_run = false;
    let mut args = std::env::args().skip(1);
    while let Some(arg) = args.next() {
        match arg.as_str() {
            "--dir" => dir = args.next().map(PathBuf::from),
            "--from" => from = args.next().and_then(|v| Backend::parse(&v).ok()),
            "--to" => to = args.next().and_then(|v| Backend::parse(&v).ok()),
            "--dry-run" => dry_run = true,
            "--help" | "-h" => return usage(),
            other => {
                eprintln!("unknown argument '{other}'");
                return usage();
            }
        }
    }
    let (Some(dir), Some(from), Some(to)) = (dir, from, to) else {
        return usage();
    };
    match convert(&dir, from, to, dry_run) {
        Ok(()) => ExitCode::SUCCESS,
        Err(e) => {
            eprintln!("convert failed: {e}");
            ExitCode::FAILURE
        }
    }
}

fn convert(dir: &Path, from: Backend, to: Backend, dry_run: bool) -> Result<()> {
    if from == to {
        eprintln!("nothing to do: the source and the target are the same backend");
        return Ok(());
    }
    for (name, keyspace, is_catalog) in STORES {
        let path = dir.join(name);
        if !path.exists() {
            eprintln!("{name}: no store at {}, skipped", path.display());
            continue;
        }
        let staging = path.with_extension("converted");
        if staging.exists() {
            std::fs::remove_dir_all(&staging).map_err(|e| Error::io(&staging, e))?;
        }
        let report = {
            // The source is opened, read and **closed** before the directories
            // are swapped: a store that is still open when its directory is
            // renamed flushes into the *new* directory, which would leave a
            // RocksDB's CURRENT and MANIFEST next to a fjall store and make the
            // detection pick the wrong substrate.
            let source = from.open(&path, keyspace)?;
            if dry_run {
                let report = if is_catalog {
                    copy_cpp_catalog(source.as_ref(), &NullStore)?
                } else {
                    copy_all(source.as_ref(), &NullStore)?
                };
                eprintln!(
                    "{name}: {} keys ({} bytes) would move {} -> {}",
                    report.keys,
                    report.bytes,
                    from.name(),
                    to.name()
                );
                continue;
            }
            let target = to.open(&staging, keyspace)?;
            let report = if is_catalog {
                copy_cpp_catalog(source.as_ref(), target.as_ref())?
            } else {
                copy_all(source.as_ref(), target.as_ref())?
            };
            target.flush()?;
            drop(target);
            drop(source);
            report
        };
        let backup = path.with_extension("converted.old");
        if backup.exists() {
            std::fs::remove_dir_all(&backup).map_err(|e| Error::io(&backup, e))?;
        }
        std::fs::rename(&path, &backup).map_err(|e| Error::io(&path, e))?;
        std::fs::rename(&staging, &path).map_err(|e| Error::io(&path, e))?;
        if backup.exists() {
            std::fs::remove_dir_all(&backup).map_err(|e| Error::io(&backup, e))?;
        }
        eprintln!(
            "{name}: {} keys ({} bytes) moved {} -> {}",
            report.keys,
            report.bytes,
            from.name(),
            to.name()
        );
    }
    Ok(())
}

/// A store that drops everything written to it, for counting a conversion
/// before it is carried out.
struct NullStore;

impl MetaStore for NullStore {
    fn get(&self, _key: &[u8]) -> Result<Option<Vec<u8>>> {
        Ok(None)
    }
    fn put(&self, _key: &[u8], _value: &[u8]) -> Result<()> {
        Ok(())
    }
    fn delete(&self, _key: &[u8]) -> Result<()> {
        Ok(())
    }
    fn atomic_batch(&self, _items: &[(Vec<u8>, Option<Vec<u8>>)]) -> Result<()> {
        Ok(())
    }
    fn merge(&self, _key: &[u8], _delta: u64) -> Result<u64> {
        Ok(0)
    }
    fn count_prefix(&self, _prefix: &[u8]) -> Result<usize> {
        Ok(0)
    }
    fn scan_prefix(&self, _prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        Ok(Vec::new())
    }
    fn flush(&self) -> Result<()> {
        Ok(())
    }
}
