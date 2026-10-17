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
        access_key: "minioadmin".into(),
        secret_key: "minioadmin".into(),
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
    let src = tempfile::tempdir().unwrap();
    std::fs::create_dir_all(src.path().join("db_1/tbl_0")).unwrap();
    std::fs::write(src.path().join("db_1/tbl_0/blk_0"), b"payload").unwrap();
    std::fs::write(src.path().join("db_1/tbl_0/chunk_0.idx"), b"idx").unwrap();
    let up = sync_tree(&c, src.path(), "tree-test").unwrap();
    assert!(up >= 2, "uploaded {up}");

    let dst = tempfile::tempdir().unwrap();
    let down = restore_tree(&c, dst.path(), "tree-test").unwrap();
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
    assert_eq!(sync_tree(&c, src.path(), "tree-test").unwrap(), 0);
}
