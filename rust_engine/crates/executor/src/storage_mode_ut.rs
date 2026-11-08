//! Ports of `Storage::SetStorageMode` (`src/storage/storage_impl.cpp:805-877`)
//! and the write gate every operator applies (`physical_insert_impl.cpp:117-128`).
//!
//! **Registered deviation** (`CLASS_MAP.md`, 有意的设计变更): in the reference a
//! storage-mode transition is a lifecycle operation. `InitToAdmin` builds the
//! WAL manager, the persistence layer and the read-only `KVStore`;
//! `AdminToReaderBottom` and `AdminToWriter` build the buffer manager, the
//! catalog, the transaction manager, the memory-index tracer and the four
//! background processors (starting the WAL replay, `AttachCatalog(checkpoint_ts)`
//! and `RecoverMemIndex` on the way); `WriterTo*` / `UnInitFrom*` stop and drop
//! them again. This port builds all of those once in `Engine::open` and tears
//! them down with the `Engine`, so a transition only changes the mode and the
//! statements it admits. What is preserved is what a client observes: the set of
//! legal transitions, and the error (`Unrecoverable` when uninitialized,
//! `InvalidNodeRole` 8007 otherwise) that a write gets outside `kWritable`.

use crate::{Engine, QueryResult, StorageMode};
use parser::Literal;
use tempfile::tempdir;

fn rows(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
    for result in engine.execute(sql).unwrap() {
        if let QueryResult::Rows { .. } = result {
            return result.rows_text();
        }
    }
    Vec::new()
}

fn create_table(engine: &Engine) {
    engine
        .execute("CREATE TABLE st (id INT, name VARCHAR);")
        .unwrap();
}

/// `ErrorCode::kInvalidNodeRole` (`src/common/status.cppm:213`).
const INVALID_NODE_ROLE: i64 = 8007;

/// The reference's standalone/leader startup reaches `kWritable` before it
/// serves a request (`InfinityContext::ChangeServerRole` → `AdminToWriter`), and
/// every write below works.
#[test]
fn open_is_writable_and_accepts_writes() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    assert_eq!(engine.storage_mode(), StorageMode::Writable);

    create_table(&engine);
    engine.execute("INSERT INTO st VALUES (1, 'a');").unwrap();
    assert_eq!(
        rows(&engine, "SELECT id FROM st;"),
        vec![vec!["1".to_string()]]
    );
}

/// A follower/learner replica: `kWritable` writes are rejected with 8007 while
/// reads keep working. The failure is the reference's
/// `Status::InvalidNodeRole("Attempt to write on non-writable node")`, and it
/// carries the reference's node-role code rather than a generic parameter error.
#[test]
fn readable_rejects_writes_but_allows_reads() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    create_table(&engine);
    engine.execute("INSERT INTO st VALUES (1, 'a');").unwrap();

    engine.set_storage_mode(StorageMode::Readable).unwrap();
    assert_eq!(engine.storage_mode(), StorageMode::Readable);

    // SELECT is unaffected.
    assert_eq!(
        rows(&engine, "SELECT id FROM st;"),
        vec![vec!["1".to_string()]]
    );

    for sql in [
        "INSERT INTO st VALUES (2, 'b');",
        "UPDATE st SET id = 2;",
        "DELETE FROM st;",
        "DROP TABLE st;",
    ] {
        let error = engine.execute(sql).unwrap_err();
        assert_eq!(error.status_code(), INVALID_NODE_ROLE, "{sql}");
        assert_eq!(error.to_string(), "Attempt to write on non-writable node");
    }

    // The Thrift write RPCs call the engine methods directly, so they are
    // gated there too.
    let error = engine
        .insert_literals("st", None, vec![vec![Literal::Int(3)]])
        .unwrap_err();
    assert_eq!(error.status_code(), INVALID_NODE_ROLE);
    // A checkpoint needs `kWritable` (`physical_flush_impl.cpp:42-52`) ...
    assert_eq!(engine.flush().unwrap_err().status_code(), INVALID_NODE_ROLE);
    // ... while cleanup runs on a read-only replica too
    // (`background_process_impl.cpp:109`).
    engine.cleanup().unwrap();

    // Nothing was published.
    assert_eq!(
        rows(&engine, "SELECT count(*) FROM st;"),
        vec![vec!["1".to_string()]]
    );
}

/// `ReaderToWriter` (`storage_impl.cpp:600-712`): writes come back.
#[test]
fn returning_to_writable_restores_writes() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    create_table(&engine);

    engine.set_storage_mode(StorageMode::Readable).unwrap();
    assert!(engine.execute("INSERT INTO st VALUES (1, 'a');").is_err());

    engine.set_storage_mode(StorageMode::Writable).unwrap();
    assert_eq!(engine.storage_mode(), StorageMode::Writable);
    engine.execute("INSERT INTO st VALUES (1, 'a');").unwrap();
    assert_eq!(
        rows(&engine, "SELECT id FROM st;"),
        vec![vec!["1".to_string()]]
    );
}

/// `kUnInitialized` only moves to `kAdmin` (`storage_impl.cpp:813-818`); the
/// reference calls `UnrecoverableError` on anything else, so the port returns
/// the same message as an error instead of aborting.
#[test]
fn uninitialized_to_writable_is_rejected() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();

    engine.set_storage_mode(StorageMode::UnInitialized).unwrap();
    let error = engine.set_storage_mode(StorageMode::Writable).unwrap_err();
    assert_eq!(
        error.to_string(),
        "Attempt to set storage mode from UnInitialized to Writable"
    );
    // The illegal transition does not change the mode.
    assert_eq!(engine.storage_mode(), StorageMode::UnInitialized);

    // A mutating statement on an uninitialized storage is the reference's
    // fatal case, not 8007.
    let error = engine.execute("CREATE TABLE st (id INT);").unwrap_err();
    assert_eq!(error.to_string(), "Uninitialized storage mode");

    // UnInit → Admin is the one legal step, and same-mode is a no-op
    // (`Set unchanged mode`).
    engine.set_storage_mode(StorageMode::Admin).unwrap();
    engine.set_storage_mode(StorageMode::Admin).unwrap();
    assert_eq!(engine.storage_mode(), StorageMode::Admin);
}

/// `kAdmin` (the start of `ChangeServerRole`) reads the catalog but accepts no
/// user write.
#[test]
fn admin_allows_reads_but_not_writes() {
    let dir = tempdir().unwrap();
    let engine = Engine::open(dir.path()).unwrap();
    create_table(&engine);

    engine.set_storage_mode(StorageMode::Admin).unwrap();

    // The catalog is readable (`InitToAdmin` opens the `KVStore` read-only).
    assert_eq!(
        rows(&engine, "SELECT count(*) FROM st;"),
        vec![vec!["0".to_string()]]
    );
    let error = engine
        .execute("INSERT INTO st VALUES (1, 'a');")
        .unwrap_err();
    assert_eq!(error.status_code(), INVALID_NODE_ROLE);
    // Admin is below `kReadable`, so even the replica-level cleanup is refused.
    assert_eq!(
        engine.cleanup().unwrap_err().status_code(),
        INVALID_NODE_ROLE
    );
}
