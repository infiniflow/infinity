//! PoC C (RUST_REWRITE_PLAN.md 附录 B / §3 D2): cross-substrate crash consistency.
//!
//! Models Infinity's three persistence substrates on one commit path and injects
//! a crash at each boundary, then verifies recovery:
//!   * KV (catalog)      -> fjall
//!   * WAL (redo log)    -> framed append-only file (magic + len + CRC32 + payload)
//!   * block files       -> plain files under `blocks/`
//!
//! Commit protocol mirrors the plan's invariant (§附录 A.4):
//!   1. write+fsync block file        (durable)
//!   2. append+fsync WAL record       (durable)      -- serial single-writer
//!   3. commit KV catalog + persist   (durable)
//! Recovery: read checkpoint ts from KV; replay WAL entries with ts > cp
//! (idempotent, and reconstruct a missing block file from the WAL payload).
//!
//! Run:  cargo run -p crash-consistency

use std::fs::{File, OpenOptions};
use std::io::{Read, Write};
use std::path::{Path, PathBuf};

use fjall::{Database, Keyspace, KeyspaceCreateOptions, PersistMode};
use tempfile::tempdir;

const WAL_MAGIC: u32 = 0x5741_4C31; // "WAL1"

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum Stop {
    AfterBlock,
    AfterWal,
    Full,
}

#[derive(Clone, Debug)]
struct Entry {
    txn_id: u64,
    commit_ts: u64,
    data: Vec<u8>,
}

fn blocks_dir(dir: &Path) -> PathBuf {
    dir.join("blocks")
}
fn block_path(dir: &Path, id: u64) -> PathBuf {
    blocks_dir(dir).join(format!("blk_{id}"))
}
fn wal_path(dir: &Path) -> PathBuf {
    dir.join("wal.log")
}

fn open_kv(dir: &Path) -> (Database, Keyspace) {
    let db = Database::builder(dir.join("kv")).open().unwrap();
    let ks = db.keyspace("catalog", KeyspaceCreateOptions::default).unwrap();
    (db, ks)
}

fn write_block(dir: &Path, id: u64, data: &[u8]) {
    std::fs::create_dir_all(blocks_dir(dir)).unwrap();
    let mut f = File::create(block_path(dir, id)).unwrap();
    f.write_all(data).unwrap();
    f.sync_all().unwrap();
}

fn wal_append(dir: &Path, e: &Entry) {
    let mut payload = Vec::new();
    payload.extend_from_slice(&e.txn_id.to_le_bytes());
    payload.extend_from_slice(&e.commit_ts.to_le_bytes());
    payload.extend_from_slice(&e.data);
    let crc = crc32fast::hash(&payload);
    let mut rec = Vec::new();
    rec.extend_from_slice(&WAL_MAGIC.to_le_bytes());
    rec.extend_from_slice(&(payload.len() as u32).to_le_bytes());
    rec.extend_from_slice(&crc.to_le_bytes());
    rec.extend_from_slice(&payload);
    let mut f = OpenOptions::new().create(true).append(true).open(wal_path(dir)).unwrap();
    f.write_all(&rec).unwrap();
    f.sync_all().unwrap();
}

/// Robust reader: stops at the first torn/foreign record (simulates a crash
/// during append).
fn wal_read(dir: &Path) -> Vec<Entry> {
    let mut out = Vec::new();
    let mut buf = Vec::new();
    if File::open(wal_path(dir)).and_then(|mut f| f.read_to_end(&mut buf)).is_err() {
        return out;
    }
    let mut i = 0usize;
    while i + 12 <= buf.len() {
        let magic = u32::from_le_bytes(buf[i..i + 4].try_into().unwrap());
        if magic != WAL_MAGIC {
            break;
        }
        let plen = u32::from_le_bytes(buf[i + 4..i + 8].try_into().unwrap()) as usize;
        let crc = u32::from_le_bytes(buf[i + 8..i + 12].try_into().unwrap());
        if i + 12 + plen > buf.len() {
            break; // torn record
        }
        let payload = &buf[i + 12..i + 12 + plen];
        if crc32fast::hash(payload) != crc {
            break; // corrupt
        }
        let txn_id = u64::from_le_bytes(payload[0..8].try_into().unwrap());
        let commit_ts = u64::from_le_bytes(payload[8..16].try_into().unwrap());
        out.push(Entry { txn_id, commit_ts, data: payload[16..].to_vec() });
        i += 12 + plen;
    }
    out
}

fn commit(dir: &Path, txn_id: u64, commit_ts: u64, data: &[u8], stop: Stop) {
    write_block(dir, txn_id, data); // step 1
    if stop == Stop::AfterBlock {
        return; // crash before WAL
    }
    wal_append(dir, &Entry { txn_id, commit_ts, data: data.to_vec() }); // step 2
    if stop == Stop::AfterWal {
        return; // crash before KV commit
    }
    let (db, ks) = open_kv(dir);
    let mut b = db.batch();
    b.insert(&ks, format!("blk:{txn_id}").as_str(), (data.len() as u64).to_le_bytes());
    b.commit().unwrap();
    db.persist(PersistMode::SyncAll).unwrap(); // step 3
}

fn checkpoint(dir: &Path, ts: u64) {
    let (db, ks) = open_kv(dir);
    let mut b = db.batch();
    b.insert(&ks, b"__cp", ts.to_le_bytes());
    b.commit().unwrap();
    db.persist(PersistMode::SyncAll).unwrap();
    drop(db);
    // truncate WAL: drop entries with commit_ts <= ts
    let keep: Vec<Entry> = wal_read(dir).into_iter().filter(|e| e.commit_ts > ts).collect();
    let _ = std::fs::remove_file(wal_path(dir));
    for e in keep {
        wal_append(dir, &e);
    }
}

fn read_cp(dir: &Path) -> u64 {
    let (_db, ks) = open_kv(dir);
    let v = ks.get(b"__cp").unwrap();
    v.map(|b| u64::from_le_bytes(b.as_ref()[..8].try_into().unwrap())).unwrap_or(0)
}

/// Returns the set of committed txn ids after recovery (also (re)writes missing
/// block files from the WAL payload).
fn recover(dir: &Path) -> Vec<u64> {
    let cp = read_cp(dir);
    let entries = wal_read(dir);
    let (db, ks) = open_kv(dir);
    let mut b = db.batch();
    let mut committed = vec![];
    for e in &entries {
        if e.commit_ts <= cp {
            continue;
        }
        if !block_path(dir, e.txn_id).exists() {
            write_block(dir, e.txn_id, &e.data); // rebuild block from WAL
        }
        b.insert(&ks, format!("blk:{}", e.txn_id).as_str(), (e.data.len() as u64).to_le_bytes());
        committed.push(e.txn_id);
    }
    b.commit().unwrap();
    db.persist(PersistMode::SyncAll).unwrap();
    // also include catalog entries already durable (e.g. below a checkpoint)
    let mut all: Vec<u64> = ks
        .iter()
        .filter_map(|kv| {
            let k = kv.key().ok()?;
            std::str::from_utf8(k.as_ref()).ok().and_then(|s| s.strip_prefix("blk:")).and_then(|n| n.parse().ok())
        })
        .collect();
    all.extend(committed);
    all.sort_unstable();
    all.dedup();
    all
}

fn main() {
    let mut failed = 0;
    println!("== PoC C: cross-substrate crash consistency ==");
    failed += scenario_after_block() as i32;
    failed += scenario_after_wal() as i32;
    failed += scenario_full() as i32;
    failed += scenario_checkpoint_and_tail() as i32;
    failed += scenario_torn_wal() as i32;
    println!("== PoC C result: {} ==", if failed == 0 { "PASS" } else { "ISSUES" });
    std::process::exit(if failed == 0 { 0 } else { 1 });
}

fn report(name: &str, ok: bool, detail: &str) -> bool {
    println!("  [{name}] {}  {}", if ok { "PASS" } else { "FAIL" }, detail);
    !ok
}

fn scenario_after_block() -> bool {
    let dir = tempdir().unwrap();
    commit(dir.path(), 1, 1, b"payload-1", Stop::AfterBlock);
    let r1 = recover(dir.path());
    let r2 = recover(dir.path()); // idempotent
    let catalog_has = file_catalog_has(dir.path(), 1);
    let ok = r1.is_empty() && r2.is_empty() && !catalog_has;
    report(
        "crash after block (before WAL)",
        ok,
        &format!("committed={r1:?} (expect []), catalog_has_txn1={catalog_has} (orphan block allowed)"),
    )
}

fn scenario_after_wal() -> bool {
    let dir = tempdir().unwrap();
    commit(dir.path(), 1, 1, b"payload-1", Stop::AfterWal);
    let r1 = recover(dir.path());
    let r2 = recover(dir.path());
    let ok = r1 == vec![1] && r2 == vec![1];
    report("crash after WAL (replay)", ok, &format!("recovered={r1:?}, replay2={r2:?} (expect [1])"))
}

fn scenario_full() -> bool {
    let dir = tempdir().unwrap();
    commit(dir.path(), 1, 1, b"payload-1", Stop::Full);
    let r = recover(dir.path());
    let ok = r == vec![1] && block_path(dir.path(), 1).exists();
    report("full commit", ok, &format!("recovered={r:?}, block exists={}", block_path(dir.path(), 1).exists()))
}

fn scenario_checkpoint_and_tail() -> bool {
    let dir = tempdir().unwrap();
    for i in 1..=3u64 {
        commit(dir.path(), i, i, format!("p{i}").as_bytes(), Stop::Full);
    }
    checkpoint(dir.path(), 3); // publish checkpoint; WAL truncated to > 3
    for i in 4..=5u64 {
        commit(dir.path(), i, i, format!("p{i}").as_bytes(), Stop::Full);
    }
    let r = recover(dir.path());
    let ok = r == vec![1, 2, 3, 4, 5];
    let wal_left = wal_read(dir.path()).len();
    report(
        "checkpoint + WAL tail",
        ok,
        &format!("recovered={r:?} (expect 1..=5), wal entries after cp={wal_left}"),
    )
}

fn scenario_torn_wal() -> bool {
    let dir = tempdir().unwrap();
    commit(dir.path(), 1, 1, b"p1", Stop::Full);
    // append a partial/torn record (simulate crash mid-append)
    {
        let mut f = OpenOptions::new().append(true).open(wal_path(dir.path())).unwrap();
        f.write_all(&WAL_MAGIC.to_le_bytes()).unwrap();
        f.write_all(&999u32.to_le_bytes()).unwrap(); // claims 999 bytes, provides none
        f.sync_all().unwrap();
    }
    let entries = wal_read(dir.path());
    let r = recover(dir.path());
    let ok = entries.len() == 1 && r == vec![1];
    report("torn WAL tail ignored", ok, &format!("valid entries read={}, recovered={r:?}", entries.len()))
}

fn file_catalog_has(dir: &Path, id: u64) -> bool {
    let (db, ks) = open_kv(dir);
    let has = ks.get(format!("blk:{id}").as_bytes()).unwrap().is_some();
    drop(db);
    has
}
