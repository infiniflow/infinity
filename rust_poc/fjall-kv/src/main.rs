//! PoC B (RUST_REWRITE_PLAN.md 附录 B / §3 D2): does the pure-Rust `fjall` engine
//! cover the four "KV gate" requirements that Infinity's RocksDB usage relies on?
//!
//! Gates:
//!   1. multi-key (cross-keyspace) ATOMIC commit      -> `Database::batch()`
//!   2. snapshot isolation / repeatable reads         -> `Database::snapshot()`
//!   3. atomic counter read-modify-write              -> `SingleWriterTxDatabase` + `update_fetch`
//!   4. backup / restore                              -> ??? (probe)
//!
//! Run:  cargo run -p fjall-kv

use std::sync::Arc;
use std::thread;

use fjall::{Database, KeyspaceCreateOptions, PersistMode, Readable, SingleWriterTxDatabase};
use tempfile::tempdir;

fn kb(b: &[u8]) -> u64 {
    u64::from_be_bytes(b[..8].try_into().unwrap())
}

fn main() {
    let mut failed = 0;
    println!("== PoC B: fjall four KV gates ==");
    failed += gate1_atomic_batch() as i32;
    failed += gate2_snapshot() as i32;
    failed += gate3_counter_rmw() as i32;
    failed += gate4_backup() as i32;
    println!("== PoC B result: {} ==", if failed == 0 { "PASS" } else { "ISSUES" });
    std::process::exit(if failed == 0 { 0 } else { 1 });
}

/// Gate 1: a write batch spanning two keyspaces commits atomically.
fn gate1_atomic_batch() -> bool {
    let dir = tempdir().unwrap();
    let db = Database::builder(dir.path()).open().unwrap();
    let catalog = db.keyspace("catalog", KeyspaceCreateOptions::default).unwrap();
    let manifest = db.keyspace("manifest", KeyspaceCreateOptions::default).unwrap();

    // dropped (uncommitted) batch -> must NOT be applied
    {
        let mut b = db.batch();
        b.insert(&catalog, b"tbl:1", b"v1");
        b.insert(&manifest, b"obj:1", b"v1");
        // no commit(); drop
    }
    let applied_after_drop = catalog.get(b"tbl:1").unwrap().is_some()
        || manifest.get(b"obj:1").unwrap().is_some();

    // committed batch -> both keys present
    let mut b = db.batch();
    b.insert(&catalog, b"tbl:1", b"v1");
    b.insert(&manifest, b"obj:1", b"v1");
    b.commit().unwrap();

    let both = catalog.get(b"tbl:1").unwrap().is_some() && manifest.get(b"obj:1").unwrap().is_some();
    let ok = !applied_after_drop && both;
    println!(
        "  [gate1 atomic batch] {}  (drop-applied={applied_after_drop}, commit-both={both})",
        if ok { "PASS" } else { "FAIL" }
    );
    !ok
}

/// Gate 2: a snapshot provides repeatable reads across concurrent writes.
fn gate2_snapshot() -> bool {
    let dir = tempdir().unwrap();
    let db = Database::builder(dir.path()).open().unwrap();
    let ks = db.keyspace("kv", KeyspaceCreateOptions::default).unwrap();
    ks.insert(b"k", b"old").unwrap();

    let snap = db.snapshot(); // repeatable view
    ks.insert(b"k", b"new").unwrap();

    let from_snap = snap.get(&ks, b"k").unwrap().map(|v| v.to_vec());
    let from_now = ks.get(b"k").unwrap().map(|v| v.to_vec());
    let ok = from_snap.as_deref() == Some(b"old".as_slice())
        && from_now.as_deref() == Some(b"new".as_slice());
    println!(
        "  [gate2 snapshot]     {}  (snapshot={:?}, current={:?})",
        if ok { "PASS" } else { "FAIL" },
        from_snap.as_deref().map(String::from_utf8_lossy),
        from_now.as_deref().map(String::from_utf8_lossy),
    );
    !ok
}

/// Gate 3: N threads increment a shared counter; final value must equal N
/// (i.e. the engine offers a *safe* atomic read-modify-write).
fn gate3_counter_rmw() -> bool {
    let dir = tempdir().unwrap();
    let db = SingleWriterTxDatabase::builder(dir.path()).open().unwrap();
    let ks = db.keyspace("counters", KeyspaceCreateOptions::default).unwrap();
    ks.insert(b"next_id", 0u64.to_be_bytes()).unwrap();

    const N: u64 = 200;
    const T: u64 = 8;
    let per = N / T;
    let db = Arc::new(db);
    let ks = Arc::new(ks);

    let mut handles = vec![];
    for _ in 0..T {
        let db = db.clone();
        let ks = ks.clone();
        handles.push(thread::spawn(move || {
            for _ in 0..per {
                let mut tx = db.write_tx();
                tx.update_fetch(&ks, b"next_id", |old| {
                    let cur = old.map(|b| kb(b.as_ref())).unwrap_or(0);
                    Some((cur + 1).to_be_bytes().to_vec().into())
                })
                .unwrap();
                tx.commit().unwrap();
            }
        }));
    }
    for h in handles {
        h.join().unwrap();
    }

    let got = ks.get(b"next_id").unwrap().map(|v| kb(v.as_ref())).unwrap_or(0);
    let ok = got == N;
    println!(
        "  [gate3 counter RMW]  {}  (final={got}, expected={N})",
        if ok { "PASS" } else { "FAIL" }
    );
    !ok
}

/// Gate 4: is there a built-in backup/restore? (docs say no) If absent, does a
/// directory-copy after `persist(SyncAll)` round-trip correctly?
fn gate4_backup() -> bool {
    let dir = tempdir().unwrap();
    let db = Database::builder(dir.path()).open().unwrap();
    let ks = db.keyspace("kv", KeyspaceCreateOptions::default).unwrap();
    ks.insert(b"a", b"1").unwrap();
    ks.insert(b"b", b"2").unwrap();
    db.persist(PersistMode::SyncAll).unwrap();

    // "backup" = copy the data directory (no engine API for this)
    let backup = tempdir().unwrap();
    drop(db); // release files

    let mut copied = 0;
    for entry in std::fs::read_dir(dir.path()).unwrap() {
        let entry = entry.unwrap();
        let dst = backup.path().join(entry.file_name());
        if entry.file_type().unwrap().is_dir() {
            copy_dir(&entry.path(), &dst);
        } else {
            std::fs::copy(entry.path(), &dst).unwrap();
        }
        copied += 1;
    }

    let restored = Database::builder(backup.path()).open().unwrap();
    let rks = restored.keyspace("kv", KeyspaceCreateOptions::default).unwrap();
    let a = rks.get(b"a").unwrap().map(|v| v.to_vec());
    let b = rks.get(b"b").unwrap().map(|v| v.to_vec());
    let ok = a.as_deref() == Some(b"1".as_slice()) && b.as_deref() == Some(b"2".as_slice());
    println!(
        "  [gate4 backup]       {}  (no engine backup API; dir-copy workaround copied {copied} entries, restored ok={ok})",
        if ok { "PASS(with workaround)" } else { "FAIL" }
    );
    // This gate is reported as a *gap*, not a hard failure: the workaround works,
    // but there is no first-class backup/restore -> must be built by Infinity.
    !ok
}

fn copy_dir(src: &std::path::Path, dst: &std::path::Path) {
    std::fs::create_dir_all(dst).unwrap();
    for entry in std::fs::read_dir(src).unwrap() {
        let entry = entry.unwrap();
        let to = dst.join(entry.file_name());
        if entry.file_type().unwrap().is_dir() {
            copy_dir(&entry.path(), &to);
        } else {
            std::fs::copy(entry.path(), &to).unwrap();
        }
    }
}
