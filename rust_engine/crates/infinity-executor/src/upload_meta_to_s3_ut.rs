//! Port of `UploadMetaToS3.DISABLED_MINIO_test1`
//! (`src/unit_test/infinity_cloud/upload_meta_to_s3_ut.cpp`).
//!
//! The reference creates a database/table, imports one row, checkpoints to
//! MinIO, downloads the remote metadata into a fresh catalog directory and
//! verifies the recovered table meta (`segment 0` → `block 0` → one row).
//!
//! The test is `#[ignore]`d exactly like the C++ `DISABLED_` case. It performs
//! a real upload/list/download round trip through [`infinity_storage::s3`];
//! when the environment is absent it prints how to enable it and returns
//! without touching the network.

use std::path::Path;

use crate::Engine;
use infinity_storage::s3::{restore_tree, S3Client, S3Config};
use tempfile::tempdir;

fn minio_config() -> Option<S3Config> {
    let endpoint = std::env::var("INFINITY_MINIO_ENDPOINT").unwrap_or_default();
    let bucket = std::env::var("INFINITY_MINIO_BUCKET").unwrap_or_default();
    if endpoint.is_empty() || bucket.is_empty() {
        eprintln!(
            "skipping minio_upload_meta_to_s3: set INFINITY_MINIO_ENDPOINT and \
             INFINITY_MINIO_BUCKET (optional: INFINITY_MINIO_ACCESS_KEY, \
             INFINITY_MINIO_SECRET_KEY, INFINITY_MINIO_HTTPS=1) and run with --ignored"
        );
        return None;
    }
    Some(S3Config {
        endpoint,
        bucket,
        access_key: std::env::var("INFINITY_MINIO_ACCESS_KEY")
            .unwrap_or_else(|_| "minioadmin".to_string()),
        secret_key: std::env::var("INFINITY_MINIO_SECRET_KEY")
            .unwrap_or_else(|_| "minioadmin".to_string()),
        https: std::env::var("INFINITY_MINIO_HTTPS").ok().as_deref() == Some("1"),
    })
}

fn count_block_dirs(root: &Path, out: &mut usize) {
    let Ok(entries) = std::fs::read_dir(root) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            if path.to_string_lossy().ends_with("seg_0/blk_0") {
                *out += 1;
            }
            count_block_dirs(&path, out);
        }
    }
}

#[test]
#[ignore = "C++ DISABLED_MINIO_test1 (needs MinIO)"]
fn minio_upload_meta_to_s3() {
    let Some(config) = minio_config() else {
        return;
    };
    let client = S3Client::new(config);
    client.ensure_bucket().unwrap();

    let src = tempdir().unwrap();
    let engine = Engine::open(src.path()).unwrap();
    engine.execute("CREATE DATABASE db1;").unwrap();
    engine
        .execute("CREATE TABLE db1.tb1 (col1 INT, col2 VARCHAR);")
        .unwrap();
    engine
        .execute("INSERT INTO db1.tb1 VALUES (1, 'abcdefghijklmnopqrstuvwxyz');")
        .unwrap();

    // Checkpoint with the real object store: `data` is mirrored by `flush`,
    // the catalog and row store by the checkpoint itself.
    engine.set_object_store(std::sync::Arc::new(client.clone()));
    engine.checkpoint_to_object_store().unwrap();

    let remote_files = client.list_objects("").unwrap();
    assert!(
        remote_files.iter().any(|(key, _)| key.starts_with("data/")),
        "no data objects: {remote_files:?}"
    );
    assert!(
        remote_files
            .iter()
            .any(|(key, _)| key.starts_with("catalog/")),
        "no catalog (meta) objects: {remote_files:?}"
    );
    assert!(
        remote_files
            .iter()
            .any(|(key, _)| key.starts_with("persistence/")),
        "no row-store objects: {remote_files:?}"
    );

    // Download the remote metadata and data into a fresh engine directory.
    let dst = tempdir().unwrap();
    let restored_catalog = restore_tree(&client, &dst.path().join("catalog"), "catalog").unwrap();
    let restored_rows =
        restore_tree(&client, &dst.path().join("persistence"), "persistence").unwrap();
    let restored_data = restore_tree(&client, &dst.path().join("data"), "data").unwrap();
    assert!(restored_catalog > 0, "catalog restore was empty");
    assert!(restored_rows > 0, "row-store restore was empty");
    assert!(restored_data > 0, "data restore was empty");

    // The recovered catalog must describe the same table. The reference checks
    // `GetTableMeta` -> segment ids {0} -> block ids {0} -> one row.
    let restored = Engine::open(dst.path()).unwrap();
    assert!(restored.table_exists("db1.tb1"));
    assert_eq!(restored.row_count("db1.tb1").unwrap(), 1);

    let mut block_dirs = 0;
    count_block_dirs(&dst.path().join("data"), &mut block_dirs);
    assert_eq!(block_dirs, 1, "expected exactly seg_0/blk_0");
    assert_eq!(restored.cpp_block_row_count("db1.tb1").unwrap(), 1);

    let mut rows = Vec::new();
    for result in restored.execute("SELECT col1, col2 FROM db1.tb1;").unwrap() {
        if let crate::QueryResult::Rows { rows: out, .. } = result {
            rows = out;
        }
    }
    assert_eq!(
        rows,
        vec![vec![
            "1".to_string(),
            "abcdefghijklmnopqrstuvwxyz".to_string()
        ]]
    );
}
