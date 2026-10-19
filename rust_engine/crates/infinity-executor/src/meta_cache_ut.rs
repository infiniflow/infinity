//! Engine-level port of `src/unit_test/storage/new_catalog/meta_cache_ut.cpp`
//! (TEST_AUDIT rows 836-840).
//!
//! The reference drives `MetaCache` (`src/storage/catalog/meta_cache_impl.cpp`)
//! directly, so the port exercises the Rust `infinity_catalog::meta_cache`
//! mirror of that class with the same constructors, capacity, versioned
//! lookups and `Size()` semantics. Every numeric expectation from the
//! reference is asserted, and the request/hit/miss counters are checked as
//! well.

use infinity_catalog::meta_cache::{
    CacheItemArg, MetaCache, MetaCacheType, MetaDbCache, MetaEraseCache, MetaIndexCache,
    MetaTableCache,
};
use std::sync::Arc;

fn db(cache: &mut MetaCache, name: &str, id: u64, commit_ts: u64, key: &str, dropped: bool) {
    cache.put_or_erase(CacheItemArg::Db(Arc::new(MetaDbCache::new(
        name, id, commit_ts, key, dropped,
    ))));
}

fn table(
    cache: &mut MetaCache,
    db_id: u64,
    name: &str,
    table_id: u64,
    commit_ts: u64,
    key: &str,
    dropped: bool,
) {
    cache.put_or_erase(CacheItemArg::Table(Arc::new(MetaTableCache::new(
        db_id, name, table_id, commit_ts, key, dropped,
    ))));
}

#[allow(clippy::too_many_arguments)]
fn index(
    cache: &mut MetaCache,
    db_id: u64,
    table_id: u64,
    name: &str,
    index_id: u64,
    commit_ts: u64,
    key: &str,
    dropped: bool,
) {
    cache.put_or_erase(CacheItemArg::Index(Arc::new(MetaIndexCache::new(
        db_id, table_id, name, index_id, commit_ts, key, dropped,
    ))));
}

/// C++ `meta_cache_ut.cpp:31` (`MetaCacheTest.test_db`).
#[test]
fn test_db() {
    let mut cache = MetaCache::new(2);
    {
        db(&mut cache, "db1", 0, 4, "catalog|db|db1|4", false);
        assert!(cache.get_db("db1", 3).is_none());
        let entry = cache.get_db("db1", 5).expect("db1@4 is visible at ts 5");
        assert_eq!(entry.commit_ts(), 4);
        assert!(!entry.is_dropped());
        assert_eq!(cache.size(), 1);
    }
    {
        db(&mut cache, "db1", 0, 6, "catalog|db|db1|6", true);
        assert!(cache.get_db("db1", 3).is_none());
        let entry = cache.get_db("db1", 5).expect("db1@4");
        assert_eq!(entry.commit_ts(), 4);
        assert!(!entry.is_dropped());
        let entry = cache.get_db("db1", 7).expect("db1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());
        assert_eq!(cache.size(), 2);
    }
    {
        db(&mut cache, "db1", 0, 8, "catalog|db|db1|8", false);
        assert!(cache.get_db("db1", 3).is_none());
        // Capacity 2 evicted db1@4 when db1@8 was put.
        assert!(cache.get_db("db1", 5).is_none());
        let entry = cache.get_db("db1", 7).expect("db1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());
        let entry = cache.get_db("db1", 9).expect("db1@8");
        assert_eq!(entry.commit_ts(), 8);
        assert!(!entry.is_dropped());
        assert_eq!(cache.size(), 2);
    }
    {
        // `cache.GetDb("db1", 7)` touches db1@6.
        let entry = cache.get_db("db1", 7).expect("db1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());

        db(&mut cache, "db2", 1, 10, "catalog|db|db2|10", false);

        let entry = cache.get_db("db1", 9).expect("db1@6 survived the put");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());

        let entry = cache.get_db("db2", 11).expect("db2@10");
        assert_eq!(entry.commit_ts(), 10);
        assert!(!entry.is_dropped());

        assert_eq!(cache.size(), 2);
    }

    // Twelve lookups, eight hits (three of the twelve miss).
    let status = cache.status(MetaCacheType::Db);
    assert_eq!(status.requests, 12);
    assert_eq!(status.hits, 8);
    assert_eq!(cache.misses(MetaCacheType::Db), 4);
}

/// C++ `meta_cache_ut.cpp:104` (`MetaCacheTest.test_table`).
#[test]
fn test_table() {
    let mut cache = MetaCache::new(2);
    {
        table(&mut cache, 0, "tbl1", 0, 4, "catalog|tbl|0|tbl1|4", false);
        assert!(cache.get_table(0, "tbl1", 3).is_none());
        let entry = cache.get_table(0, "tbl1", 5).expect("tbl1@4");
        assert_eq!(entry.commit_ts(), 4);
        assert!(!entry.is_dropped());
        assert_eq!(cache.size(), 1);
    }
    {
        table(&mut cache, 0, "tbl1", 0, 6, "catalog|tbl|0|tbl1|6", true);
        assert!(cache.get_table(0, "tbl1", 3).is_none());
        let entry = cache.get_table(0, "tbl1", 5).expect("tbl1@4");
        assert_eq!(entry.commit_ts(), 4);
        assert!(!entry.is_dropped());
        let entry = cache.get_table(0, "tbl1", 7).expect("tbl1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());
        assert_eq!(cache.size(), 2);
    }
    {
        table(&mut cache, 0, "tbl1", 0, 8, "catalog|tbl|0|tbl1|8", false);
        assert!(cache.get_table(0, "tbl1", 3).is_none());
        assert!(cache.get_table(0, "tbl1", 5).is_none());
        let entry = cache.get_table(0, "tbl1", 7).expect("tbl1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());
        let entry = cache.get_table(0, "tbl1", 9).expect("tbl1@8");
        assert_eq!(entry.commit_ts(), 8);
        assert!(!entry.is_dropped());
        assert_eq!(cache.size(), 2);
    }
    {
        let entry = cache.get_table(0, "tbl1", 7).expect("tbl1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());

        table(&mut cache, 0, "tbl2", 1, 10, "catalog|tbl|0|tbl2|8", false);

        let entry = cache
            .get_table(0, "tbl1", 9)
            .expect("tbl1@6 survived the put");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());

        let entry = cache.get_table(0, "tbl2", 11).expect("tbl2@10");
        assert_eq!(entry.commit_ts(), 10);
        assert!(!entry.is_dropped());

        assert_eq!(cache.size(), 2);
    }

    let status = cache.status(MetaCacheType::Table);
    assert_eq!(status.requests, 12);
    assert_eq!(status.hits, 8);
    assert_eq!(cache.misses(MetaCacheType::Table), 4);
}

/// C++ `meta_cache_ut.cpp:175` (`MetaCacheTest.test_index`).
#[test]
fn test_index() {
    let mut cache = MetaCache::new(2);
    {
        index(
            &mut cache,
            0,
            0,
            "idx1",
            0,
            4,
            "catalog|idx|0|0|idx1|4",
            false,
        );
        assert!(cache.get_index(0, 0, "idx1", 3).is_none());
        assert!(cache.get_index(0, 0, "idx1", 5).is_some());
        assert_eq!(cache.size(), 1);
    }
    {
        index(
            &mut cache,
            0,
            0,
            "idx1",
            0,
            6,
            "catalog|idx|0|0|idx1|6",
            true,
        );
        assert!(cache.get_index(0, 0, "idx1", 3).is_none());
        let entry = cache.get_index(0, 0, "idx1", 5).expect("idx1@4");
        assert_eq!(entry.commit_ts(), 4);
        assert!(!entry.is_dropped());
        let entry = cache.get_index(0, 0, "idx1", 7).expect("idx1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());
        assert_eq!(cache.size(), 2);
    }
    {
        index(
            &mut cache,
            0,
            0,
            "idx1",
            0,
            8,
            "catalog|idx|0|0|idx1|8",
            false,
        );
        assert!(cache.get_index(0, 0, "idx1", 3).is_none());
        assert!(cache.get_index(0, 0, "idx1", 5).is_none());
        let entry = cache.get_index(0, 0, "idx1", 7).expect("idx1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());
        let entry = cache.get_index(0, 0, "idx1", 9).expect("idx1@8");
        assert_eq!(entry.commit_ts(), 8);
        assert!(!entry.is_dropped());
        assert_eq!(cache.size(), 2);
    }
    {
        let entry = cache.get_index(0, 0, "idx1", 7).expect("idx1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());

        index(
            &mut cache,
            0,
            0,
            "idx2",
            1,
            10,
            "catalog|idx|0|0|idx2|10",
            false,
        );

        let entry = cache
            .get_index(0, 0, "idx1", 9)
            .expect("idx1@6 survived the put");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());

        let entry = cache.get_index(0, 0, "idx2", 11).expect("idx2@10");
        assert_eq!(entry.commit_ts(), 10);
        assert!(!entry.is_dropped());

        assert_eq!(cache.size(), 2);
    }

    let status = cache.status(MetaCacheType::Index);
    assert_eq!(status.requests, 12);
    assert_eq!(status.hits, 8);
    assert_eq!(cache.misses(MetaCacheType::Index), 4);
}

/// C++ `meta_cache_ut.cpp:245` (`MetaCacheTest.test_mix`).
#[test]
fn test_mix() {
    let mut cache = MetaCache::new(2);
    {
        db(&mut cache, "db1", 0, 4, "catalog|db|db1|4", false);
        assert!(cache.get_db("db1", 3).is_none());
        let entry = cache.get_db("db1", 5).expect("db1@4");
        assert_eq!(entry.commit_ts(), 4);
        assert!(!entry.is_dropped());
        assert_eq!(cache.size(), 1);
    }
    {
        table(&mut cache, 0, "tbl1", 0, 6, "catalog|tbl|0|tbl1|6", true);
        assert!(cache.get_db("db1", 3).is_none());
        let entry = cache.get_db("db1", 5).expect("db1@4");
        assert_eq!(entry.commit_ts(), 4);

        let entry = cache.get_table(0, "tbl1", 7).expect("tbl1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());
        assert_eq!(cache.size(), 2);
    }
    {
        index(
            &mut cache,
            0,
            0,
            "idx1",
            0,
            8,
            "catalog|idx|0|0|idx1|8",
            false,
        );
        assert!(cache.get_db("db1", 3).is_none());
        // The index put evicted db1@4 (capacity 2).
        assert!(cache.get_db("db1", 5).is_none());

        let entry = cache.get_table(0, "tbl1", 7).expect("tbl1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());

        let entry = cache.get_index(0, 0, "idx1", 9).expect("idx1@8");
        assert_eq!(entry.commit_ts(), 8);
        assert!(!entry.is_dropped());
        assert_eq!(cache.size(), 2);
    }

    // db: 6 lookups, 2 hits; table: 2 lookups, 2 hits; index: 1 lookup, 1 hit.
    assert_eq!(cache.status(MetaCacheType::Db).requests, 6);
    assert_eq!(cache.status(MetaCacheType::Db).hits, 2);
    assert_eq!(cache.misses(MetaCacheType::Db), 4);
    assert_eq!(cache.status(MetaCacheType::Table).requests, 2);
    assert_eq!(cache.status(MetaCacheType::Table).hits, 2);
    assert_eq!(cache.misses(MetaCacheType::Table), 0);
    assert_eq!(cache.status(MetaCacheType::Index).requests, 1);
    assert_eq!(cache.status(MetaCacheType::Index).hits, 1);
    assert_eq!(cache.misses(MetaCacheType::Index), 0);
}

/// C++ `meta_cache_ut.cpp:292` (`MetaCacheTest.test_erase`): erasing removes
/// the lookup entries but leaves the stale LRU nodes, so `Size()` is unchanged.
#[test]
fn test_erase() {
    let mut cache = MetaCache::new(3);
    {
        db(&mut cache, "db1", 0, 4, "catalog|db|db1|4", false);
        let entry = cache.get_db("db1", 5).expect("db1@4");
        assert_eq!(entry.commit_ts(), 4);
        assert!(!entry.is_dropped());
        assert_eq!(cache.size(), 1);
    }
    {
        table(&mut cache, 0, "tbl1", 0, 6, "catalog|tbl|0|tbl1|6", true);
        let entry = cache.get_table(0, "tbl1", 7).expect("tbl1@6");
        assert_eq!(entry.commit_ts(), 6);
        assert!(entry.is_dropped());
        assert_eq!(cache.size(), 2);
    }
    {
        index(
            &mut cache,
            0,
            0,
            "idx1",
            0,
            8,
            "catalog|idx|0|0|idx1|8",
            false,
        );
        let entry = cache.get_index(0, 0, "idx1", 9).expect("idx1@8");
        assert_eq!(entry.commit_ts(), 8);
        assert!(!entry.is_dropped());
        assert_eq!(cache.size(), 3);
    }
    {
        cache.erase_and_commit(
            &[MetaEraseCache::Db {
                db_name: "db1".to_string(),
            }],
            1,
        );
        assert!(cache.get_db("db1", 5).is_none());
        // C++ `Size()` is the LRU length; the stale node is still counted.
        assert_eq!(cache.size(), 3);
    }
    {
        cache.erase_and_commit(
            &[MetaEraseCache::Table {
                db_id: 0,
                table_name: "tbl1".to_string(),
            }],
            1,
        );
        assert!(cache.get_table(0, "tbl1", 5).is_none());
        assert_eq!(cache.size(), 3);
    }
    {
        cache.erase_and_commit(
            &[MetaEraseCache::Index {
                db_id: 0,
                table_id: 0,
                index_name: "idx1".to_string(),
            }],
            1,
        );
        assert!(cache.get_index(0, 0, "idx1", 5).is_none());
        assert_eq!(cache.size(), 3);
    }

    // Six lookups total: one hit per kind before the erase, one miss after.
    for kind in [
        MetaCacheType::Db,
        MetaCacheType::Table,
        MetaCacheType::Index,
    ] {
        let status = cache.status(kind);
        assert_eq!(status.requests, 2);
        assert_eq!(status.hits, 1);
        assert_eq!(cache.misses(kind), 1);
    }
}
