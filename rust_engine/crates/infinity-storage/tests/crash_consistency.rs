//! Cross-substrate crash-consistency regression (plan §6 Gate G / INVARIANT_MATRIX G3).
//!
//! Models Infinity's three persistence substrates on one commit path — block
//! files, the WAL (redo log) and the metadata KV — and injects a crash at each
//! boundary, then verifies recovery keeps **exactly the committed prefix**:
//!
//!   commit protocol (§附录 A.4):  write+fsync block → append+fsync WAL → commit KV
//!   recovery:                    read checkpoint ts from KV, replay WAL ts > cp,
//!                                reconstruct any block file the WAL still knows
//!
//! This is the `rust_engine` regression form of the `rust_poc/crash-consistency`
//! PoC, so the invariant is exercised by `cargo test` rather than only by the PoC.

use std::fs;
use std::path::{Path, PathBuf};

use infinity_storage::{FjallMetaStore, MetaStore, WalManager, WalRecord};

/// Where the simulated process died.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum Stop {
    AfterBlock,
    AfterWal,
    Full,
}

fn block_path(dir: &Path, id: u64) -> PathBuf {
    dir.join("blocks").join(format!("blk_{id}"))
}

fn checkpoint_key() -> &'static [u8] {
    b"checkpoint"
}

fn txn_key(id: u64) -> Vec<u8> {
    format!("txn:{id}").into_bytes()
}

/// Encodes a block id plus its bytes into a WAL payload.
fn payload(block_id: u64, data: &[u8]) -> Vec<u8> {
    let mut p = block_id.to_le_bytes().to_vec();
    p.extend_from_slice(data);
    p
}

fn decode_payload(p: &[u8]) -> (u64, Vec<u8>) {
    let id = u64::from_le_bytes(p[..8].try_into().unwrap());
    (id, p[8..].to_vec())
}

/// Runs the commit protocol for one transaction, stopping at the requested
/// boundary to simulate a crash (later durable writes never happen).
fn commit_until(dir: &Path, kv: &FjallMetaStore, txn_id: u64, data: &[u8], stop: Stop) {
    let block_id = txn_id;
    fs::create_dir_all(dir.join("blocks")).unwrap();
    fs::write(block_path(dir, block_id), data).unwrap();
    if stop == Stop::AfterBlock {
        return;
    }
    let mut wal = WalManager::open(dir.join("wal.log")).unwrap();
    // commit_ts = txn_id + 1 so it is strictly greater than the checkpoint 0.
    wal.append(&WalRecord::new(txn_id, txn_id + 1, payload(block_id, data)))
        .unwrap();
    wal.flush().unwrap();
    if stop == Stop::AfterWal {
        return;
    }
    kv.put(checkpoint_key(), &(txn_id + 1).to_le_bytes())
        .unwrap();
    kv.put(&txn_key(txn_id), &(txn_id + 1).to_le_bytes())
        .unwrap();
    kv.flush().unwrap();
}

/// Recovery: replay the WAL past the KV checkpoint, rebuilding any missing
/// block file, and record the replayed transactions back into the KV.
fn recover(dir: &Path, kv: &FjallMetaStore) -> Vec<u64> {
    let checkpoint = kv
        .get(checkpoint_key())
        .unwrap()
        .map(|v| u64::from_le_bytes(v.try_into().unwrap()))
        .unwrap_or(0);
    let mut replayed = Vec::new();
    WalManager::replay_since(dir.join("wal.log"), checkpoint, |rec| {
        let (block_id, data) = decode_payload(&rec.payload);
        kvs_put_missing_block(dir, block_id, &data);
        kv.put(&txn_key(rec.txn_id), &rec.commit_ts.to_le_bytes())
            .unwrap();
        replayed.push(rec.txn_id);
        Ok(())
    })
    .unwrap();
    replayed
}

fn kvs_put_missing_block(dir: &Path, block_id: u64, data: &[u8]) {
    let p = block_path(dir, block_id);
    if !p.exists() {
        fs::create_dir_all(dir.join("blocks")).unwrap();
        fs::write(p, data).unwrap();
    }
}

/// A transaction is visible once the KV records it (either at commit time or
/// via WAL replay).
fn visible(kv: &FjallMetaStore, txn_id: u64) -> bool {
    kv.get(&txn_key(txn_id)).unwrap().is_some()
}

fn open_kv(dir: &Path) -> FjallMetaStore {
    FjallMetaStore::open(dir.join("kv"), "default").unwrap()
}

#[test]
fn crash_after_block_is_invisible() {
    let dir = tempfile::tempdir().unwrap();
    let t = dir.path();
    commit_until(t, &open_kv(t), 1, b"row", Stop::AfterBlock);
    // The block file exists but nothing references it: the transaction is not
    // committed, and recovery (empty WAL) leaves it that way.
    assert!(block_path(t, 1).exists());
    let kv = open_kv(t);
    assert!(recover(t, &kv).is_empty());
    assert!(!visible(&kv, 1));
}

#[test]
fn crash_after_wal_replays_the_transaction() {
    let dir = tempfile::tempdir().unwrap();
    let t = dir.path();
    commit_until(t, &open_kv(t), 1, b"row", Stop::AfterWal);
    let kv = open_kv(t);
    assert_eq!(recover(t, &kv), vec![1]);
    assert!(visible(&kv, 1));
    assert_eq!(fs::read(block_path(t, 1)).unwrap(), b"row");
}

#[test]
fn full_commit_is_visible_without_replay() {
    let dir = tempfile::tempdir().unwrap();
    let t = dir.path();
    commit_until(t, &open_kv(t), 1, b"row", Stop::Full);
    let kv = open_kv(t);
    // checkpoint already at ts=2, so nothing needs replaying
    assert!(recover(t, &kv).is_empty());
    assert!(visible(&kv, 1));
}

#[test]
fn recovery_rebuilds_a_missing_block_file() {
    let dir = tempfile::tempdir().unwrap();
    let t = dir.path();
    commit_until(t, &open_kv(t), 1, b"row", Stop::AfterWal);
    // the block file is lost before recovery
    fs::remove_file(block_path(t, 1)).unwrap();
    let kv = open_kv(t);
    assert_eq!(recover(t, &kv), vec![1]);
    assert_eq!(fs::read(block_path(t, 1)).unwrap(), b"row");
}

#[test]
fn torn_wal_tail_stops_replay_at_the_durable_prefix() {
    let dir = tempfile::tempdir().unwrap();
    let t = dir.path();
    commit_until(t, &open_kv(t), 1, b"row", Stop::AfterWal);
    // append a half-written frame (crash mid-append)
    let mut f = fs::OpenOptions::new()
        .append(true)
        .open(t.join("wal.log"))
        .unwrap();
    std::io::Write::write_all(&mut f, &[0x57, 0x41, 0x4c, 0x31, 0x00]).unwrap();
    let kv = open_kv(t);
    // only the intact transaction replays; the torn tail is dropped
    assert_eq!(recover(t, &kv), vec![1]);
    assert!(visible(&kv, 1));
}
