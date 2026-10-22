//! End-to-end checks for the S3 client against a local MinIO. Run with
//! `INFINITY_MINIO_TEST=1 cargo test --test s3_minio`; without the variable the
//! tests are no-ops so the default suite stays hermetic.

use infinity_storage::s3::{restore_tree, sync_tree, S3Client, S3Config};

fn client(bucket: &str) -> Option<S3Client> {
    if std::env::var("INFINITY_MINIO_TEST").ok().as_deref() != Some("1") {
        return None;
    }
    Some(S3Client::new(S3Config {
        endpoint: std::env::var("INFINITY_MINIO_ENDPOINT")
            .unwrap_or_else(|_| "127.0.0.1:9005".to_string()),
        bucket: bucket.to_string(),
        access_key: std::env::var("INFINITY_MINIO_ACCESS_KEY")
            .unwrap_or_else(|_| "minioadmin".to_string()),
        secret_key: std::env::var("INFINITY_MINIO_SECRET_KEY")
            .unwrap_or_else(|_| "minioadmin".to_string()),
        https: std::env::var("INFINITY_MINIO_HTTPS").ok().as_deref() == Some("1"),
    }))
}

#[test]
fn put_get_list_delete_roundtrip() {
    let Some(c) = client("it-bucket") else {
        return;
    };
    c.ensure_bucket().unwrap();
    c.put_object("dir/hello.bin", b"hello minio").unwrap();
    assert_eq!(
        c.get_object("dir/hello.bin").unwrap().unwrap(),
        b"hello minio"
    );
    let list = c.list_objects("dir/").unwrap();
    assert!(
        list.iter().any(|(k, s)| k == "dir/hello.bin" && *s == 11),
        "listing: {list:?}"
    );
    c.delete_object("dir/hello.bin").unwrap();
    assert!(c.get_object("dir/hello.bin").unwrap().is_none());
}

#[test]
fn tree_sync_and_restore() {
    let Some(c) = client("it-tree") else {
        return;
    };
    c.ensure_bucket().unwrap();
    // A fresh prefix per run: a bucket left over from an earlier run would
    // legitimately skip the uploads this test counts.
    let prefix = format!("tree-test-{}", std::process::id());
    let src = tempfile::tempdir().unwrap();
    std::fs::create_dir_all(src.path().join("db_1/tbl_0")).unwrap();
    std::fs::write(src.path().join("db_1/tbl_0/blk_0"), b"payload").unwrap();
    std::fs::write(src.path().join("db_1/tbl_0/chunk_0.idx"), b"idx").unwrap();
    let up = sync_tree(&c, src.path(), &prefix).unwrap();
    assert!(up >= 2, "uploaded {up}");

    let dst = tempfile::tempdir().unwrap();
    let down = restore_tree(&c, dst.path(), &prefix).unwrap();
    assert!(down >= 2, "restored {down}");
    assert_eq!(
        std::fs::read(dst.path().join("db_1/tbl_0/blk_0")).unwrap(),
        b"payload"
    );
    assert_eq!(
        std::fs::read(dst.path().join("db_1/tbl_0/chunk_0.idx")).unwrap(),
        b"idx"
    );

    // A second sync with the same sizes uploads nothing.
    assert_eq!(sync_tree(&c, src.path(), &prefix).unwrap(), 0);
}

/// A file rewritten to the *same length* still has to be uploaded: RocksDB's
/// `CURRENT` is always sixteen bytes, so a size-only comparison leaves the
/// bucket pointing at a manifest that the sync has already deleted.
#[test]
fn same_size_edit_is_uploaded() {
    let Some(c) = client("it-tree") else {
        return;
    };
    c.ensure_bucket().unwrap();
    let prefix = format!("same-size-{}", std::process::id());
    let src = tempfile::tempdir().unwrap();
    std::fs::write(src.path().join("CURRENT"), b"MANIFEST-000005\n").unwrap();
    assert_eq!(sync_tree(&c, src.path(), &prefix).unwrap(), 1);

    std::fs::write(src.path().join("CURRENT"), b"MANIFEST-000014\n").unwrap();
    assert_eq!(
        sync_tree(&c, src.path(), &prefix).unwrap(),
        1,
        "a same-sized rewrite must not be skipped"
    );

    let dst = tempfile::tempdir().unwrap();
    restore_tree(&c, dst.path(), &prefix).unwrap();
    assert_eq!(
        std::fs::read(dst.path().join("CURRENT")).unwrap(),
        b"MANIFEST-000014\n"
    );
}
