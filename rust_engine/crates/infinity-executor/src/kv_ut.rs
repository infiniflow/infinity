//! Ports of `src/unit_test/storage/new_catalog/kv_store_ut.cpp` and
//! `kv_utility_ut.cpp` onto the `MetaStore` surface.
//!
//! The reference tests drive `KVStore`/`KVInstance`, i.e. a RocksDB
//! `TransactionDB` with snapshots, merge operators, WAL toggles and
//! backup/restore. The Rust meta store
//! (`infinity-storage/src/metastore.rs`) offers `get` / `put` / `delete` /
//! `atomic_write` / `scan_prefix` / `flush`; the rows whose assertions need one
//! of the missing primitives are reported blocked rather than approximated.
//!
//! Ported here:
//! - `kv_store6` — prefix iteration order plus `Flush`.
//! - `kv_utility` `test1` — catalog key spelling and the timestamp suffix.

use infinity_storage::catalog_format::{catalog_db_key, catalog_table_key, get_timestamp_from_key};
use infinity_storage::{FjallMetaStore, MetaStore};
use tempfile::tempdir;

/// C++: `kv_store_ut.cpp:446` (`kv_store6`).
///
/// Seven writes under a transaction, iterated over the empty prefix; the
/// reference prints the pairs (the loop must see every key in key order) and
/// then commits and flushes.
#[test]
fn kv_store6_prefix_iteration_and_flush() {
    let dir = tempdir().unwrap();
    let store = FjallMetaStore::open(dir.path(), "default").unwrap();

    let pairs = [
        ("key1", "value1"),
        ("key2", "value2"),
        ("key3", "value3"),
        ("key4", "value4"),
        ("key5", "value5"),
        ("foo", "value5"),
        ("loop", "value5"),
    ];
    for (k, v) in pairs {
        store.put(k.as_bytes(), v.as_bytes()).unwrap();
    }

    // The transaction iterates its own writes before the commit.
    let scanned: Vec<(String, String)> = store
        .scan_prefix(b"")
        .unwrap()
        .into_iter()
        .map(|(k, v)| (String::from_utf8(k).unwrap(), String::from_utf8(v).unwrap()))
        .collect();
    assert_eq!(
        scanned,
        vec![
            ("foo".to_string(), "value5".to_string()),
            ("key1".to_string(), "value1".to_string()),
            ("key2".to_string(), "value2".to_string()),
            ("key3".to_string(), "value3".to_string()),
            ("key4".to_string(), "value4".to_string()),
            ("key5".to_string(), "value5".to_string()),
            ("loop".to_string(), "value5".to_string()),
        ]
    );

    store.flush().unwrap();
}

/// C++: `kv_utility_ut.cpp:29` (`KVUtilityTest.test1`).
///
/// `KeyEncode::CatalogDbKey` / `CatalogTableKey` spell the ts as the final
/// field, and `GetTimestampFromKey` reads it back.
#[test]
fn kv_utility_catalog_keys_carry_the_timestamp() {
    let key = catalog_db_key("db1", 123);
    assert_eq!(key, "catalog|db|db1|123");
    assert_eq!(get_timestamp_from_key(&key), 123);

    let key = catalog_table_key("3", "tbl1", 0);
    assert_eq!(key, "catalog|tbl|3|tbl1|0");
    assert_eq!(get_timestamp_from_key(&key), 0);
}
