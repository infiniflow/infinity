//! `infinity-txn`: the transaction commit pipeline.
//!
//! Mirrors the engine's order (plan 附录 A.4): **WriteWAL (serial) -> CommitBottom
//! (KV atomic write)**. Recovery replays WAL records with `commit_ts > checkpoint`
//! into the KV store idempotently.

#![forbid(unsafe_code)]

use std::path::PathBuf;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::{Arc, Mutex};

use infinity_common::{Error, Result, TxnTimeStamp};
use infinity_storage::{MetaStore, WalManager, WalRecord};

/// Encode a batch of writes into a WAL payload.
fn encode_writes(writes: &[(Vec<u8>, Vec<u8>)]) -> Vec<u8> {
    let mut out = Vec::new();
    for (k, v) in writes {
        out.extend_from_slice(&(k.len() as u32).to_le_bytes());
        out.extend_from_slice(k);
        out.extend_from_slice(&(v.len() as u32).to_le_bytes());
        out.extend_from_slice(v);
    }
    out
}

fn decode_writes(mut buf: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
    let mut out = Vec::new();
    while !buf.is_empty() {
        if buf.len() < 4 {
            return Err(Error::Wal("truncated write batch".into()));
        }
        let klen = u32::from_le_bytes(buf[0..4].try_into().unwrap()) as usize;
        buf = &buf[4..];
        if buf.len() < klen + 4 {
            return Err(Error::Wal("truncated write key".into()));
        }
        let k = buf[..klen].to_vec();
        buf = &buf[klen..];
        let vlen = u32::from_le_bytes(buf[0..4].try_into().unwrap()) as usize;
        buf = &buf[4..];
        if buf.len() < vlen {
            return Err(Error::Wal("truncated write value".into()));
        }
        let v = buf[..vlen].to_vec();
        buf = &buf[vlen..];
        out.push((k, v));
    }
    Ok(out)
}

/// A pending transaction.
pub struct Txn {
    pub id: u64,
    pub begin_ts: TxnTimeStamp,
    writes: Vec<(Vec<u8>, Vec<u8>)>,
}

impl Txn {
    pub fn put(&mut self, key: impl Into<Vec<u8>>, value: impl Into<Vec<u8>>) {
        self.writes.push((key.into(), value.into()));
    }
    pub fn len(&self) -> usize {
        self.writes.len()
    }
    pub fn is_empty(&self) -> bool {
        self.writes.is_empty()
    }
}

/// Owns the WAL + KV pair and hands out transactions.
pub struct TxnManager {
    store: Arc<dyn MetaStore>,
    wal: Mutex<WalManager>,
    wal_path: PathBuf,
    next_txn_id: AtomicU64,
    next_ts: AtomicU64,
}

impl TxnManager {
    /// Opens the manager, seeding counters from any existing WAL (recovery).
    pub fn open(wal_path: impl Into<PathBuf>, store: Arc<dyn MetaStore>) -> Result<Self> {
        let wal_path = wal_path.into();
        let recs = WalManager::read_all(&wal_path)?;
        let max_ts = recs.iter().map(|r| r.commit_ts).max().unwrap_or(0);
        let max_id = recs.iter().map(|r| r.txn_id).max().unwrap_or(0);
        let wal = WalManager::open(&wal_path)?;
        Ok(Self {
            store,
            wal: Mutex::new(wal),
            wal_path,
            next_txn_id: AtomicU64::new(max_id + 1),
            next_ts: AtomicU64::new(max_ts + 1),
        })
    }

    pub fn begin(&self) -> Txn {
        let id = self.next_txn_id.fetch_add(1, Ordering::SeqCst);
        let begin_ts = self.next_ts.load(Ordering::SeqCst);
        Txn {
            id,
            begin_ts,
            writes: Vec::new(),
        }
    }

    /// Commits: append to WAL (serial), then apply atomically to KV. Returns the
    /// commit timestamp.
    pub fn commit(&self, txn: Txn) -> Result<TxnTimeStamp> {
        if txn.is_empty() {
            return Err(Error::Txn("empty transaction".into()));
        }
        let commit_ts = self.next_ts.fetch_add(1, Ordering::SeqCst);
        let payload = encode_writes(&txn.writes);
        {
            let mut wal = self.wal.lock().unwrap();
            wal.append(&WalRecord::new(txn.id, commit_ts, payload))?;
        }
        self.store.atomic_write(&txn.writes)?;
        Ok(commit_ts)
    }

    /// Replays the WAL into the KV store for all records after `checkpoint_ts`.
    /// Idempotent: re-applying is safe.
    pub fn recover(&self, checkpoint_ts: TxnTimeStamp) -> Result<usize> {
        let mut applied = 0;
        for rec in WalManager::read_all(&self.wal_path)? {
            if rec.commit_ts > checkpoint_ts {
                let writes = decode_writes(&rec.payload)?;
                if !writes.is_empty() {
                    self.store.atomic_write(&writes)?;
                }
                applied += 1;
            }
        }
        Ok(applied)
    }

    pub fn flush(&self) -> Result<()> {
        self.wal.lock().unwrap().flush()?;
        self.store.flush()
    }

    pub fn wal_path(&self) -> &PathBuf {
        &self.wal_path
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_storage::FjallMetaStore;
    use tempfile::tempdir;

    fn manager(dir: &std::path::Path) -> TxnManager {
        let store = Arc::new(FjallMetaStore::open(dir.join("kv"), "default").unwrap());
        TxnManager::open(dir.join("wal.log"), store).unwrap()
    }

    #[test]
    fn commit_applies_to_kv() {
        let dir = tempdir().unwrap();
        {
            let store: Arc<dyn MetaStore> =
                Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
            let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
            let mut t = m.begin();
            t.put(b"a".to_vec(), b"1".to_vec());
            t.put(b"b".to_vec(), b"2".to_vec());
            let ts = m.commit(t).unwrap();
            assert!(ts >= 1);
            // commit is visible in the KV store immediately
            assert_eq!(store.get(b"a").unwrap(), Some(b"1".to_vec()));
            assert_eq!(store.get(b"b").unwrap(), Some(b"2".to_vec()));
        }
        // reopen after the handle is released
        let m2 = manager(dir.path());
        assert_eq!(m2.recover(0).unwrap(), 1); // idempotent re-apply
    }

    #[test]
    fn recovery_replays_wal_after_reopen() {
        let dir = tempdir().unwrap();
        {
            let m = manager(dir.path());
            for i in 0..3u64 {
                let mut t = m.begin();
                t.put(format!("k{i}").into_bytes(), format!("v{i}").into_bytes());
                m.commit(t).unwrap();
            }
        }
        // fresh manager + fresh store: KV empty, WAL has 3 -> recover
        let store = Arc::new(FjallMetaStore::open(dir.path().join("kv2"), "default").unwrap());
        let m = TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap();
        assert_eq!(m.recover(0).unwrap(), 3);
        assert_eq!(store.get(b"k0").unwrap(), Some(b"v0".to_vec()));
        assert_eq!(store.get(b"k2").unwrap(), Some(b"v2".to_vec()));
        // idempotent
        assert_eq!(m.recover(0).unwrap(), 3);
    }

    #[test]
    fn concurrent_commits_are_serialized_and_durable() {
        use std::collections::BTreeSet;
        let dir = tempdir().unwrap();
        const THREADS: u64 = 4;
        const PER: u64 = 25;
        {
            let store: Arc<dyn MetaStore> =
                Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
            let mgr =
                Arc::new(TxnManager::open(dir.path().join("wal.log"), store.clone()).unwrap());
            let handles: Vec<_> = (0..THREADS)
                .map(|t| {
                    let mgr = mgr.clone();
                    std::thread::spawn(move || {
                        for j in 0..PER {
                            let mut tx = mgr.begin();
                            tx.put(
                                format!("k{t}_{j}").into_bytes(),
                                format!("v{t}_{j}").into_bytes(),
                            );
                            mgr.commit(tx).unwrap();
                        }
                    })
                })
                .collect();
            for h in handles {
                h.join().unwrap();
            }
            mgr.flush().unwrap();
        }
        // all rows are durable in KV
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
        assert_eq!(
            store.scan_prefix(b"k").unwrap().len(),
            (THREADS * PER) as usize
        );

        // WAL has one record per commit, with unique commit timestamps
        let recs = WalManager::read_all(dir.path().join("wal.log")).unwrap();
        assert_eq!(recs.len(), (THREADS * PER) as usize);
        let ts: BTreeSet<u64> = recs.iter().map(|r| r.commit_ts).collect();
        assert_eq!(
            ts.len(),
            (THREADS * PER) as usize,
            "commit timestamps must be unique"
        );
    }

    #[test]
    fn truncated_wal_recovers_exactly_the_valid_prefix() {
        let dir = tempdir().unwrap();
        let wal = dir.path().join("wal.log");
        {
            let store: Arc<dyn MetaStore> =
                Arc::new(FjallMetaStore::open(dir.path().join("kv"), "default").unwrap());
            let m = TxnManager::open(&wal, store).unwrap();
            for i in 0..10u64 {
                let mut t = m.begin();
                t.put(format!("k{i}").into_bytes(), format!("v{i}").into_bytes());
                m.commit(t).unwrap();
            }
        }
        let full = std::fs::read(&wal).unwrap();
        // simulate a crash that truncates the WAL at many different offsets
        for cut in (0..full.len()).step_by(5) {
            let d2 = tempdir().unwrap();
            let wal2 = d2.path().join("wal.log");
            std::fs::write(&wal2, &full[..cut]).unwrap();
            let store: Arc<dyn MetaStore> =
                Arc::new(FjallMetaStore::open(d2.path().join("kv"), "default").unwrap());
            let m = TxnManager::open(&wal2, store.clone()).unwrap();
            m.recover(0).unwrap();
            let expected = WalManager::read_all(&wal2).unwrap().len();
            let got = store.scan_prefix(b"k").unwrap().len();
            assert_eq!(
                got, expected,
                "cut={cut}: recovered {got} rows, expected {expected}"
            );
        }
    }

    #[test]
    fn empty_txn_rejected() {
        let dir = tempdir().unwrap();
        let m = manager(dir.path());
        assert!(m.commit(m.begin()).is_err());
    }
}

/// Loom concurrency-model test (run with `RUSTFLAGS="--cfg loom"`):
///   RUSTFLAGS="--cfg loom" cargo test -p infinity-txn --lib loom
///
/// Models the commit-timestamp allocation state machine: concurrent `fetch_add`
/// must hand out unique, gap-free timestamps regardless of interleaving.
#[cfg(loom)]
mod loom_model {
    use loom::sync::atomic::{AtomicU64, Ordering};
    use loom::sync::Arc;

    #[test]
    fn commit_ts_allocation_is_unique() {
        loom::model(|| {
            let counter = Arc::new(AtomicU64::new(0));
            let handles: Vec<_> = (0..2)
                .map(|_| {
                    let c = counter.clone();
                    loom::thread::spawn(move || {
                        let mut out = Vec::new();
                        for _ in 0..2 {
                            out.push(c.fetch_add(1, Ordering::SeqCst));
                        }
                        out
                    })
                })
                .collect();
            let mut all = Vec::new();
            for h in handles {
                all.extend(h.join().unwrap());
            }
            all.sort_unstable();
            assert_eq!(all, vec![0, 1, 2, 3], "commit timestamps must be unique");
        });
    }
}
