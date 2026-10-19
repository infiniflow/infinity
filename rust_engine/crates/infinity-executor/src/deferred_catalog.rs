//! Checkpoint-only catalog durability (`NEW_CONFIG_NOWAL_PATH`).
//!
//! The reference's no-WAL configuration still writes catalog records to
//! RocksDB, but a restart only loads what the last checkpoint published. The
//! port reproduces the observable rule — a DDL committed after the last
//! checkpoint is lost — with an undo journal in front of the catalog store:
//!
//! * every catalog mutation records the key's before-image in
//!   `<dir>/catalog.pending`,
//! * reads and writes go straight through, so the session sees its own DDL,
//! * `flush()` (the checkpoint) fsyncs the underlying store and deletes the
//!   journal, making the writes durable,
//! * opening again without a checkpoint replays the journal backwards and
//!   removes the pending keys/restores the overwritten values.
//!
//! This mirrors `NewTxnManager` replaying only WAL records up to
//! `LastCheckpointTS()`: catalog changes after the checkpoint never reach the
//! restored state.

use std::fs::File;
use std::io::Write;
use std::path::{Path, PathBuf};
use std::sync::{Arc, Mutex};

use infinity_common::{Error, Result};
use infinity_storage::MetaStore;

const MAGIC: &[u8; 8] = b"INFDCAT1";

/// Name of the undo journal next to the catalog directory.
pub(crate) const JOURNAL_FILE: &str = "catalog.pending";

fn journal_path(catalog_dir: &Path) -> PathBuf {
    catalog_dir.join(JOURNAL_FILE)
}

/// A `MetaStore` that can undo every write since the last checkpoint.
pub(crate) struct DeferredCatalogStore {
    inner: Arc<dyn MetaStore>,
    path: PathBuf,
    journal: Mutex<Option<File>>,
}

impl DeferredCatalogStore {
    pub(crate) fn new(inner: Arc<dyn MetaStore>, catalog_dir: &Path) -> Self {
        Self {
            inner,
            path: journal_path(catalog_dir),
            journal: Mutex::new(None),
        }
    }

    /// Records `key`'s current value before a mutation overwrites or removes
    /// it. Called with `journal` locked.
    fn record(file: &mut File, key: &[u8], old: Option<&[u8]>) -> Result<()> {
        let mut buf = Vec::with_capacity(9 + key.len() + old.map_or(0, |v| v.len()));
        buf.extend_from_slice(&(key.len() as u32).to_le_bytes());
        buf.extend_from_slice(key);
        match old {
            Some(v) => {
                buf.push(1);
                buf.extend_from_slice(&(v.len() as u32).to_le_bytes());
                buf.extend_from_slice(v);
            }
            None => buf.push(0),
        }
        file.write_all(&buf)
            .map_err(|e| Error::Storage(format!("catalog journal: {e}")))?;
        file.flush()
            .map_err(|e| Error::Storage(format!("catalog journal: {e}")))
    }

    fn log_before(&self, keys: &[Vec<u8>]) -> Result<()> {
        let mut guard = self.journal.lock().unwrap();
        if guard.is_none() {
            let mut file = File::options()
                .create(true)
                .append(true)
                .open(&self.path)
                .map_err(|e| Error::io(&self.path, e))?;
            let len = file
                .metadata()
                .map_err(|e| Error::io(&self.path, e))?
                .len();
            if len == 0 {
                file.write_all(MAGIC)
                    .map_err(|e| Error::io(&self.path, e))?;
            }
            *guard = Some(file);
        }
        let file = guard.as_mut().expect("journal opened above");
        for key in keys {
            let old = self.inner.get(key)?;
            Self::record(file, key, old.as_deref())?;
        }
        Ok(())
    }
}

impl MetaStore for DeferredCatalogStore {
    fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>> {
        self.inner.get(key)
    }

    fn put(&self, key: &[u8], value: &[u8]) -> Result<()> {
        self.log_before(&[key.to_vec()])?;
        self.inner.put(key, value)
    }

    fn delete(&self, key: &[u8]) -> Result<()> {
        self.log_before(&[key.to_vec()])?;
        self.inner.delete(key)
    }

    fn atomic_write(&self, items: &[(Vec<u8>, Vec<u8>)]) -> Result<()> {
        let keys: Vec<Vec<u8>> = items.iter().map(|(k, _)| k.clone()).collect();
        self.log_before(&keys)?;
        self.inner.atomic_write(items)
    }

    fn scan_prefix(&self, prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        self.inner.scan_prefix(prefix)
    }

    fn count_prefix(&self, prefix: &[u8]) -> Result<usize> {
        self.inner.count_prefix(prefix)
    }

    /// The checkpoint: make everything durable, then forget the undo records.
    fn flush(&self) -> Result<()> {
        let mut guard = self.journal.lock().unwrap();
        self.inner.flush()?;
        *guard = None;
        match std::fs::remove_file(&self.path) {
            Ok(()) => Ok(()),
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => Ok(()),
            Err(e) => Err(Error::io(&self.path, e)),
        }
    }
}

/// Applies a journal left behind by a process that exited without a
/// checkpoint, in reverse order (last write undone first), then removes it.
pub(crate) fn rollback_pending(inner: &dyn MetaStore, catalog_dir: &Path) -> Result<()> {
    let path = journal_path(catalog_dir);
    let bytes = match std::fs::read(&path) {
        Ok(b) => b,
        Err(e) if e.kind() == std::io::ErrorKind::NotFound => return Ok(()),
        Err(e) => return Err(Error::io(&path, e)),
    };
    if bytes.len() < MAGIC.len() || &bytes[..MAGIC.len()] != MAGIC {
        // A torn header means no mutation was recorded.
        let _ = std::fs::remove_file(&path);
        return Ok(());
    }
    let mut pos = MAGIC.len();
    let mut records: Vec<(Vec<u8>, Option<Vec<u8>>)> = Vec::new();
    while pos + 4 <= bytes.len() {
        let klen = u32::from_le_bytes(bytes[pos..pos + 4].try_into().unwrap()) as usize;
        pos += 4;
        if pos + klen + 1 > bytes.len() {
            break;
        }
        let key = bytes[pos..pos + klen].to_vec();
        pos += klen;
        let present = bytes[pos];
        pos += 1;
        let old = if present == 1 {
            if pos + 4 > bytes.len() {
                break;
            }
            let vlen = u32::from_le_bytes(bytes[pos..pos + 4].try_into().unwrap()) as usize;
            pos += 4;
            if pos + vlen > bytes.len() {
                break;
            }
            let value = bytes[pos..pos + vlen].to_vec();
            pos += vlen;
            Some(value)
        } else {
            None
        };
        records.push((key, old));
    }
    for (key, old) in records.into_iter().rev() {
        match old {
            Some(value) => inner.put(&key, &value)?,
            None => inner.delete(&key)?,
        }
    }
    inner.flush()?;
    std::fs::remove_file(&path).map_err(|e| Error::io(&path, e))?;
    Ok(())
}
