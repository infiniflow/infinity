//! Metadata cache over catalog entries (C++ `MetaCache`,
//! `src/storage/catalog/meta_cache.cppm` / `meta_cache_impl.cpp`).
//!
//! The reference keeps one versioned entry per `(kind, name)` in an LRU list
//! and looks entries up by transaction begin timestamp: a lookup returns the
//! newest version whose `commit_ts` is **strictly smaller** than `begin_ts`.
//! The LRU has a fixed capacity and evicts from the back; `EraseAndCommitKV`
//! removes the map entries for a dropped object but deliberately leaves the
//! stale LRU nodes in place (that is why `Size()` still counts them, which the
//! reference's `test_erase` asserts).
//!
//! The port keeps the same structure and adds the request/hit counters the
//! reference exposes through `GetCacheStatus`.

use std::collections::{BTreeMap, HashMap, VecDeque};
use std::sync::Arc;

/// Kind of a cached metadata object (C++ `MetaCacheType`).
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub enum MetaCacheType {
    Db,
    Table,
    Index,
}

/// A cached database version (C++ `MetaDbCache`).
#[derive(Clone, Debug)]
pub struct MetaDbCache {
    db_name: String,
    db_id: u64,
    commit_ts: u64,
    db_key: String,
    is_dropped: bool,
}

impl MetaDbCache {
    pub fn new(
        db_name: impl Into<String>,
        db_id: u64,
        commit_ts: u64,
        db_key: impl Into<String>,
        is_dropped: bool,
    ) -> Self {
        Self {
            db_name: db_name.into(),
            db_id,
            commit_ts,
            db_key: db_key.into(),
            is_dropped,
        }
    }

    pub fn name(&self) -> &str {
        &self.db_name
    }
    pub fn db_id(&self) -> u64 {
        self.db_id
    }
    pub fn commit_ts(&self) -> u64 {
        self.commit_ts
    }
    pub fn db_key(&self) -> &str {
        &self.db_key
    }
    pub fn is_dropped(&self) -> bool {
        self.is_dropped
    }
}

/// A cached table version (C++ `MetaTableCache`).
#[derive(Clone, Debug)]
pub struct MetaTableCache {
    db_id: u64,
    table_name: String,
    table_id: u64,
    commit_ts: u64,
    table_key: String,
    is_dropped: bool,
}

impl MetaTableCache {
    pub fn new(
        db_id: u64,
        table_name: impl Into<String>,
        table_id: u64,
        commit_ts: u64,
        table_key: impl Into<String>,
        is_dropped: bool,
    ) -> Self {
        Self {
            db_id,
            table_name: table_name.into(),
            table_id,
            commit_ts,
            table_key: table_key.into(),
            is_dropped,
        }
    }

    pub fn name(&self) -> &str {
        &self.table_name
    }
    pub fn db_id(&self) -> u64 {
        self.db_id
    }
    pub fn table_id(&self) -> u64 {
        self.table_id
    }
    pub fn commit_ts(&self) -> u64 {
        self.commit_ts
    }
    pub fn table_key(&self) -> &str {
        &self.table_key
    }
    pub fn is_dropped(&self) -> bool {
        self.is_dropped
    }
}

/// A cached index version (C++ `MetaIndexCache`).
#[derive(Clone, Debug)]
pub struct MetaIndexCache {
    db_id: u64,
    table_id: u64,
    index_name: String,
    index_id: u64,
    commit_ts: u64,
    index_key: String,
    is_dropped: bool,
}

impl MetaIndexCache {
    #[allow(clippy::too_many_arguments)]
    pub fn new(
        db_id: u64,
        table_id: u64,
        index_name: impl Into<String>,
        index_id: u64,
        commit_ts: u64,
        index_key: impl Into<String>,
        is_dropped: bool,
    ) -> Self {
        Self {
            db_id,
            table_id,
            index_name: index_name.into(),
            index_id,
            commit_ts,
            index_key: index_key.into(),
            is_dropped,
        }
    }

    pub fn name(&self) -> &str {
        &self.index_name
    }
    pub fn db_id(&self) -> u64 {
        self.db_id
    }
    pub fn table_id(&self) -> u64 {
        self.table_id
    }
    pub fn index_id(&self) -> u64 {
        self.index_id
    }
    pub fn commit_ts(&self) -> u64 {
        self.commit_ts
    }
    pub fn index_key(&self) -> &str {
        &self.index_key
    }
    pub fn is_dropped(&self) -> bool {
        self.is_dropped
    }
}

/// An object dropped by a committed transaction (C++ `MetaErase*Cache`).
#[derive(Clone, Debug)]
pub enum MetaEraseCache {
    Db {
        db_name: String,
    },
    Table {
        db_id: u64,
        table_name: String,
    },
    Index {
        db_id: u64,
        table_id: u64,
        index_name: String,
    },
}

/// One version stored in the cache.
#[derive(Clone)]
enum CacheItem {
    Db(Arc<MetaDbCache>),
    Table(Arc<MetaTableCache>),
    Index(Arc<MetaIndexCache>),
}

impl CacheItem {
    fn kind(&self) -> MetaCacheType {
        match self {
            CacheItem::Db(_) => MetaCacheType::Db,
            CacheItem::Table(_) => MetaCacheType::Table,
            CacheItem::Index(_) => MetaCacheType::Index,
        }
    }

    /// Identity inside its kind's map.
    fn name(&self) -> String {
        match self {
            CacheItem::Db(c) => db_name_key(c.name()),
            CacheItem::Table(c) => table_name_key(c.db_id(), c.name()),
            CacheItem::Index(c) => index_name_key(c.db_id(), c.table_id(), c.name()),
        }
    }

    fn commit_ts(&self) -> u64 {
        match self {
            CacheItem::Db(c) => c.commit_ts(),
            CacheItem::Table(c) => c.commit_ts(),
            CacheItem::Index(c) => c.commit_ts(),
        }
    }
}

fn db_name_key(name: &str) -> String {
    format!("db|{name}")
}
fn table_name_key(db_id: u64, name: &str) -> String {
    format!("tbl|{db_id}|{name}")
}
fn index_name_key(db_id: u64, table_id: u64, name: &str) -> String {
    format!("idx|{db_id}|{table_id}|{name}")
}

/// Requests and hits of one cache kind (C++ `CacheStatus`).
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct CacheStatus {
    pub size: usize,
    pub requests: u64,
    pub hits: u64,
}

/// A fixed-capacity versioned metadata cache (C++ `MetaCache`).
#[derive(Default)]
pub struct MetaCache {
    capacity: usize,
    /// LRU list, most recently used first. Stale nodes survive an erase.
    lru: VecDeque<CacheItem>,
    dbs: HashMap<String, BTreeMap<u64, Arc<MetaDbCache>>>,
    tables: HashMap<String, BTreeMap<u64, Arc<MetaTableCache>>>,
    indexes: HashMap<String, BTreeMap<u64, Arc<MetaIndexCache>>>,
    db_requests: u64,
    db_hits: u64,
    table_requests: u64,
    table_hits: u64,
    index_requests: u64,
    index_hits: u64,
}

impl MetaCache {
    pub fn new(capacity: usize) -> Self {
        Self {
            capacity,
            ..Default::default()
        }
    }

    /// C++ `MetaCache::PutOrErase` for a single item.
    pub fn put_or_erase(&mut self, item: CacheItemArg) {
        match item {
            CacheItemArg::Db(c) => self.put_db(c),
            CacheItemArg::Table(c) => self.put_table(c),
            CacheItemArg::Index(c) => self.put_index(c),
        }
    }

    fn put_db(&mut self, cache: Arc<MetaDbCache>) {
        let name = db_name_key(cache.name());
        let exists = self
            .dbs
            .get(&name)
            .is_some_and(|versions| versions.contains_key(&cache.commit_ts()));
        if exists {
            self.touch(MetaCacheType::Db, &name, cache.commit_ts());
            self.dbs
                .entry(name)
                .or_default()
                .insert(cache.commit_ts(), cache);
            return;
        }
        self.lru.push_front(CacheItem::Db(cache.clone()));
        self.dbs
            .entry(name)
            .or_default()
            .insert(cache.commit_ts(), cache);
        self.trim();
    }

    fn put_table(&mut self, cache: Arc<MetaTableCache>) {
        let name = table_name_key(cache.db_id(), cache.name());
        let exists = self
            .tables
            .get(&name)
            .is_some_and(|versions| versions.contains_key(&cache.commit_ts()));
        if exists {
            self.touch(MetaCacheType::Table, &name, cache.commit_ts());
            self.tables
                .entry(name)
                .or_default()
                .insert(cache.commit_ts(), cache);
            return;
        }
        self.lru.push_front(CacheItem::Table(cache.clone()));
        self.tables
            .entry(name)
            .or_default()
            .insert(cache.commit_ts(), cache);
        self.trim();
    }

    fn put_index(&mut self, cache: Arc<MetaIndexCache>) {
        let name = index_name_key(cache.db_id(), cache.table_id(), cache.name());
        let exists = self
            .indexes
            .get(&name)
            .is_some_and(|versions| versions.contains_key(&cache.commit_ts()));
        if exists {
            self.touch(MetaCacheType::Index, &name, cache.commit_ts());
            self.indexes
                .entry(name)
                .or_default()
                .insert(cache.commit_ts(), cache);
            return;
        }
        self.lru.push_front(CacheItem::Index(cache.clone()));
        self.indexes
            .entry(name)
            .or_default()
            .insert(cache.commit_ts(), cache);
        self.trim();
    }

    /// Moves the LRU node `(kind, name, commit_ts)` to the front.
    fn touch(&mut self, kind: MetaCacheType, name: &str, commit_ts: u64) {
        if let Some(pos) = self.lru.iter().position(|item| {
            item.kind() == kind && item.name() == name && item.commit_ts() == commit_ts
        }) {
            if let Some(item) = self.lru.remove(pos) {
                self.lru.push_front(item);
            }
        }
    }

    /// C++ `TrimCacheNolock`: evict the least recently used nodes until the
    /// LRU fits the capacity.
    fn trim(&mut self) {
        while self.lru.len() > self.capacity {
            let Some(item) = self.lru.pop_back() else {
                break;
            };
            match item {
                CacheItem::Db(c) => {
                    if let Some(versions) = self.dbs.get_mut(&db_name_key(c.name())) {
                        versions.remove(&c.commit_ts());
                    }
                }
                CacheItem::Table(c) => {
                    if let Some(versions) =
                        self.tables.get_mut(&table_name_key(c.db_id(), c.name()))
                    {
                        versions.remove(&c.commit_ts());
                    }
                }
                CacheItem::Index(c) => {
                    if let Some(versions) =
                        self.indexes
                            .get_mut(&index_name_key(c.db_id(), c.table_id(), c.name()))
                    {
                        versions.remove(&c.commit_ts());
                    }
                }
            }
        }
    }

    /// C++ `MetaCache::GetDb`: the newest version committed strictly before
    /// `begin_ts`.
    pub fn get_db(&mut self, db_name: &str, begin_ts: u64) -> Option<Arc<MetaDbCache>> {
        self.db_requests += 1;
        let name = db_name_key(db_name);
        let found = self
            .dbs
            .get(&name)
            .and_then(|versions| versions.range(..begin_ts).next_back())
            .map(|(_, c)| c.clone());
        if let Some(c) = &found {
            self.db_hits += 1;
            self.touch(MetaCacheType::Db, &name, c.commit_ts());
        }
        found
    }

    /// C++ `MetaCache::GetTable`.
    pub fn get_table(
        &mut self,
        db_id: u64,
        table_name: &str,
        begin_ts: u64,
    ) -> Option<Arc<MetaTableCache>> {
        self.table_requests += 1;
        let name = table_name_key(db_id, table_name);
        let found = self
            .tables
            .get(&name)
            .and_then(|versions| versions.range(..begin_ts).next_back())
            .map(|(_, c)| c.clone());
        if let Some(c) = &found {
            self.table_hits += 1;
            self.touch(MetaCacheType::Table, &name, c.commit_ts());
        }
        found
    }

    /// C++ `MetaCache::GetIndex`.
    pub fn get_index(
        &mut self,
        db_id: u64,
        table_id: u64,
        index_name: &str,
        begin_ts: u64,
    ) -> Option<Arc<MetaIndexCache>> {
        self.index_requests += 1;
        let name = index_name_key(db_id, table_id, index_name);
        let found = self
            .indexes
            .get(&name)
            .and_then(|versions| versions.range(..begin_ts).next_back())
            .map(|(_, c)| c.clone());
        if let Some(c) = &found {
            self.index_hits += 1;
            self.touch(MetaCacheType::Index, &name, c.commit_ts());
        }
        found
    }

    /// C++ `MetaCache::EraseAndCommitKV`: drop the map entries but leave the
    /// stale LRU nodes (so `size()` still counts them, as the reference does).
    pub fn erase_and_commit(&mut self, items: &[MetaEraseCache], commit_ts: u64) {
        for item in items {
            match item {
                MetaEraseCache::Db { db_name } => {
                    self.dbs.remove(&db_name_key(db_name));
                }
                MetaEraseCache::Table { db_id, table_name } => {
                    self.tables.remove(&table_name_key(*db_id, table_name));
                }
                MetaEraseCache::Index {
                    db_id,
                    table_id,
                    index_name,
                } => {
                    self.indexes
                        .remove(&index_name_key(*db_id, *table_id, index_name));
                }
            }
        }
        let _ = commit_ts;
    }

    /// C++ `MetaCache::Size` — the LRU length, stale nodes included.
    pub fn size(&self) -> usize {
        self.lru.len()
    }

    /// C++ `MetaCache::GetCacheStatus`.
    pub fn status(&self, kind: MetaCacheType) -> CacheStatus {
        match kind {
            MetaCacheType::Db => CacheStatus {
                size: self.dbs.len(),
                requests: self.db_requests,
                hits: self.db_hits,
            },
            MetaCacheType::Table => CacheStatus {
                size: self.tables.len(),
                requests: self.table_requests,
                hits: self.table_hits,
            },
            MetaCacheType::Index => CacheStatus {
                size: self.indexes.len(),
                requests: self.index_requests,
                hits: self.index_hits,
            },
        }
    }

    /// Misses observed for `kind` (`requests - hits`).
    pub fn misses(&self, kind: MetaCacheType) -> u64 {
        let s = self.status(kind);
        s.requests.saturating_sub(s.hits)
    }
}

/// The item accepted by [`MetaCache::put_or_erase`].
#[derive(Clone)]
pub enum CacheItemArg {
    Db(Arc<MetaDbCache>),
    Table(Arc<MetaTableCache>),
    Index(Arc<MetaIndexCache>),
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn lookup_is_strictly_before_the_begin_timestamp() {
        let mut cache = MetaCache::new(2);
        cache.put_or_erase(CacheItemArg::Db(Arc::new(MetaDbCache::new(
            "db1", 0, 4, "k4", false,
        ))));
        assert!(cache.get_db("db1", 4).is_none());
        assert_eq!(cache.get_db("db1", 5).unwrap().commit_ts(), 4);
        assert_eq!(cache.status(MetaCacheType::Db).requests, 2);
        assert_eq!(cache.misses(MetaCacheType::Db), 1);
    }
}
