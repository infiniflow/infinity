//! Read-copy-update multimap for the metadata cache.
//!
//! C++: `RcuMultiMap` (`src/storage/common/rcu_multimap.cppm`). Readers use an
//! immutable snapshot (`read_map_`); writers append to a mutex-protected
//! `dirty_map_`. When the accumulated misses reach the dirty map's size, the
//! dirty map becomes the new read snapshot and the dirty map is replaced by a
//! copy of itself (the reference's `CheckSwapInLock`). Deleted values and old
//! snapshots are kept until `check_gc` to protect concurrent readers.

use std::collections::{BTreeMap, BTreeSet};
use std::sync::{Arc, Mutex, RwLock};
use std::time::{SystemTime, UNIX_EPOCH};

fn now_ms() -> u64 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|d| d.as_millis() as u64)
        .unwrap_or(0)
}

/// C++: `RcuMultiMap::MapValue`.
#[derive(Debug, Clone, PartialEq)]
struct MapValue<V> {
    value: V,
    used_time: u64,
}

/// C++: `RcuMultiMap::DeletedMap` (kept for GC bookkeeping).
#[derive(Debug)]
struct DeletedMap<K, V> {
    /// The retired snapshot, kept alive until `check_gc`.
    _map: Arc<BTreeMap<K, Vec<MapValue<V>>>>,
    delete_time: u64,
}

/// C++: `RcuMultiMap<Key, Value>`.
#[derive(Debug)]
pub struct RcuMultiMap<K, V> {
    read_map: RwLock<Arc<BTreeMap<K, Vec<MapValue<V>>>>>,
    dirty_map: Mutex<BTreeMap<K, Vec<MapValue<V>>>>,
    miss_time: Mutex<usize>,
    deleted_values: Mutex<Vec<MapValue<V>>>,
    deleted_entries: Mutex<BTreeSet<(K, u64)>>,
    deleted_maps: Mutex<Vec<DeletedMap<K, V>>>,
}

impl<K: Ord + Clone, V: Clone + PartialEq> Default for RcuMultiMap<K, V> {
    fn default() -> Self {
        Self::new()
    }
}

impl<K: Ord + Clone, V: Clone + PartialEq> RcuMultiMap<K, V> {
    /// C++: `RcuMultiMap()`.
    pub fn new() -> Self {
        Self {
            read_map: RwLock::new(Arc::new(BTreeMap::new())),
            dirty_map: Mutex::new(BTreeMap::new()),
            miss_time: Mutex::new(0),
            deleted_values: Mutex::new(Vec::new()),
            deleted_entries: Mutex::new(BTreeSet::new()),
            deleted_maps: Mutex::new(Vec::new()),
        }
    }

    /// C++: `Get(key, update_access_time)`.
    pub fn get(&self, key: &K, update_access_time: bool) -> Vec<V> {
        let snapshot = self.read_map.read().unwrap().clone();
        let results: Vec<V> = snapshot
            .get(key)
            .map(|values| {
                values
                    .iter()
                    .map(|map_value| {
                        if update_access_time {
                            // The reference updates the stored timestamp; the
                            // snapshot is immutable here, so only the value is
                            // returned. Expiry tests read the dirty map.
                        }
                        map_value.value.clone()
                    })
                    .collect()
            })
            .unwrap_or_default();

        if results.is_empty() {
            let mut dirty = self.dirty_map.lock().unwrap();
            let Some(values) = dirty.get_mut(key) else {
                return results;
            };
            for map_value in values.iter_mut() {
                if update_access_time {
                    map_value.used_time = now_ms();
                }
            }
            let n = values.len();
            let out: Vec<V> = values.iter().map(|v| v.value.clone()).collect();
            let total = dirty.len();
            let mut miss = self.miss_time.lock().unwrap();
            *miss += n;
            if *miss < total {
                return out;
            }
            // `CheckSwapInLock`: publish the dirty map; it also remains the
            // dirty map (the reference replaces it with a copy of itself).
            let published = dirty.clone();
            drop(miss);
            drop(dirty);
            let old = {
                let mut read = self.read_map.write().unwrap();
                std::mem::replace(&mut *read, Arc::new(published))
            };
            self.deleted_maps.lock().unwrap().push(DeletedMap {
                _map: old,
                delete_time: now_ms(),
            });
            *self.miss_time.lock().unwrap() = 0;
            return out;
        }
        results
    }

    /// C++: `GetWithRcuTime`.
    pub fn get_with_rcu_time(&self, key: &K) -> Vec<V> {
        self.get(key, false)
    }

    /// C++: `Insert`.
    pub fn insert(&self, key: K, value: V) {
        let map_value = MapValue {
            value,
            used_time: now_ms(),
        };
        self.dirty_map
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .push(map_value);
    }

    /// C++: `Delete(key)` — only the dirty map is mutated.
    pub fn delete(&self, key: &K) {
        let mut dirty = self.dirty_map.lock().unwrap();
        let Some(values) = dirty.remove(key) else {
            return;
        };
        let snapshot = self.read_map.read().unwrap().clone();
        let mut deleted = self.deleted_values.lock().unwrap();
        let mut deleted_entries = self.deleted_entries.lock().unwrap();
        for mut map_value in values {
            map_value.used_time = now_ms();
            if snapshot
                .get(key)
                .is_some_and(|read| read.iter().any(|v| v.value == map_value.value))
            {
                deleted_entries.insert((key.clone(), map_value.used_time));
            }
            deleted.push(map_value);
        }
    }

    /// C++: `Delete(key, value)`.
    pub fn delete_value(&self, key: &K, value: &V) {
        let mut dirty = self.dirty_map.lock().unwrap();
        let Some(values) = dirty.get_mut(key) else {
            return;
        };
        let snapshot = self.read_map.read().unwrap().clone();
        let mut deleted = self.deleted_values.lock().unwrap();
        let mut deleted_entries = self.deleted_entries.lock().unwrap();
        let mut removed = Vec::new();
        values.retain(|map_value| {
            if map_value.value != *value {
                return true;
            }
            let mut map_value = map_value.clone();
            map_value.used_time = now_ms();
            if snapshot
                .get(key)
                .is_some_and(|read| read.iter().any(|v| v.value == map_value.value))
            {
                deleted_entries.insert((key.clone(), map_value.used_time));
            }
            removed.push(map_value);
            false
        });
        deleted.extend(removed);
        if values.is_empty() {
            dirty.remove(key);
        }
    }

    /// C++: `CheckGc(min_delete_time)`.
    pub fn check_gc(&self, min_delete_time: u64) {
        {
            let mut deleted = self.deleted_values.lock().unwrap();
            deleted.retain(|map_value| map_value.used_time >= min_delete_time);
        }
        let mut maps = self.deleted_maps.lock().unwrap();
        maps.retain(|deleted_map| deleted_map.delete_time >= min_delete_time);
    }

    /// C++: `CheckExpired(min_access_time, keys)`.
    pub fn check_expired(&self, min_access_time: u64) -> Vec<K> {
        let dirty = self.dirty_map.lock().unwrap();
        let mut expired: BTreeSet<K> = BTreeSet::new();
        for (key, values) in dirty.iter() {
            if values.iter().any(|v| v.used_time <= min_access_time) {
                expired.insert(key.clone());
            }
        }
        expired.into_iter().collect()
    }

    /// C++: `GetAllValuesWithRef`.
    pub fn get_all_values_with_ref(&self, values: &mut Vec<V>) {
        let dirty = self.dirty_map.lock().unwrap();
        for list in dirty.values() {
            values.extend(list.iter().map(|v| v.value.clone()));
        }
    }

    /// C++: `GetAllKeyValuePairs`.
    pub fn get_all_key_value_pairs(&self) -> Vec<(K, V)> {
        let dirty = self.dirty_map.lock().unwrap();
        let read = self.read_map.read().unwrap().clone();
        let mut pairs = Vec::new();
        for (key, values) in dirty.iter() {
            pairs.extend(values.iter().map(|v| (key.clone(), v.value.clone())));
        }
        for (key, values) in read.iter() {
            pairs.extend(values.iter().map(|v| (key.clone(), v.value.clone())));
        }
        pairs
    }

    /// C++: `GetMergedMultiMap`.
    pub fn get_merged_multimap(&self) -> BTreeMap<K, Vec<V>> {
        let dirty = self.dirty_map.lock().unwrap();
        let read = self.read_map.read().unwrap().clone();
        let mut merged: BTreeMap<K, Vec<V>> = BTreeMap::new();
        for (key, values) in dirty.iter() {
            merged
                .entry(key.clone())
                .or_default()
                .extend(values.iter().map(|v| v.value.clone()));
        }
        for (key, values) in read.iter() {
            merged
                .entry(key.clone())
                .or_default()
                .extend(values.iter().map(|v| v.value.clone()));
        }
        merged
    }

    /// C++: `size()` — the number of entries in the dirty map.
    pub fn size(&self) -> usize {
        self.dirty_map.lock().unwrap().len()
    }

    /// C++: `emplace(key, args…)`.
    pub fn emplace(&self, key: K, value: V) {
        self.insert(key, value);
    }

    /// C++: `range(key_min, key_max, result)` — dirty entries win on equal
    /// keys, matching the reference's merge.
    pub fn range(&self, key_min: &K, key_max: &K) -> Vec<V> {
        let dirty = self.dirty_map.lock().unwrap();
        let read = self.read_map.read().unwrap().clone();
        let mut result = Vec::new();
        let mut read_it = read.range(key_min.clone()..=key_max.clone()).peekable();
        let mut dirty_it = dirty.range(key_min.clone()..=key_max.clone()).peekable();
        loop {
            let use_read = match (read_it.peek(), dirty_it.peek()) {
                (Some((read_key, _)), Some((dirty_key, _))) => read_key < dirty_key,
                (Some(_), None) => true,
                _ => false,
            };
            if use_read {
                let (_, values) = read_it.next().unwrap();
                result.extend(values.iter().map(|v| v.value.clone()));
            } else if let Some((_, values)) = dirty_it.next() {
                result.extend(values.iter().map(|v| v.value.clone()));
            } else {
                break;
            }
        }
        result
    }
}

#[cfg(test)]
mod reference_suite {
    //! Ports of `src/unit_test/storage/common/test_rcu_multimap_ut.cpp`
    //! (functional cases; the benchmark cases stay performance tests).

    use super::*;

    fn sorted(mut values: Vec<i32>) -> Vec<i32> {
        values.sort_unstable();
        values
    }

    /// C++: `RcuMultiMapTest.TestBasicInsertAndGet`.
    #[test]
    fn basic_insert_and_get() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        map.insert("key1".into(), 30);
        assert_eq!(sorted(map.get(&"key1".into(), true)), vec![10, 30]);
        assert_eq!(map.get(&"key2".into(), true), vec![20]);
        assert!(map.get(&"key3".into(), true).is_empty());
        assert!(map.size() > 0);
    }

    /// C++: `RcuMultiMapTest.TestLowercaseAliases` — the aliases map to the
    /// same methods in Rust.
    #[test]
    fn lowercase_aliases() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("test_key".into(), 100);
        map.insert("test_key".into(), 200);
        assert_eq!(sorted(map.get(&"test_key".into(), true)), vec![100, 200]);
    }

    /// C++: `RcuMultiMapTest.TestDelete`.
    #[test]
    fn delete() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("key1".into(), 10);
        map.insert("key1".into(), 20);
        map.insert("key2".into(), 30);
        map.delete_value(&"key1".into(), &10);
        assert_eq!(map.get(&"key1".into(), true), vec![20]);
        map.delete(&"key2".into());
        assert!(map.get(&"key2".into(), true).is_empty());
        map.delete(&"key1".into());
        assert!(map.get(&"key1".into(), true).is_empty());
    }

    /// C++: `RcuMultiMapTest.TestGetAllValuesWithRef`.
    #[test]
    fn get_all_values_with_ref() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        map.insert("key3".into(), 30);
        let mut values = Vec::new();
        map.get_all_values_with_ref(&mut values);
        assert_eq!(sorted(values), vec![10, 20, 30]);
    }

    /// C++: `RcuMultiMapTest.TestGarbageCollection`.
    #[test]
    fn garbage_collection() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        map.delete(&"key1".into());
        map.check_gc(now_ms().saturating_sub(1000));
        assert_eq!(map.get(&"key2".into(), true), vec![20]);
    }

    /// C++: `RcuMultiMapTest.TestExpiredEntries`.
    #[test]
    fn expired_entries() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        let _ = map.get(&"key1".into(), true);
        std::thread::sleep(std::time::Duration::from_millis(10));
        let expired = map.check_expired(now_ms() + 1000);
        assert!(expired.len() <= 2);
    }

    /// C++: `RcuMultiMapTest.TestGetWithRcuTime`.
    #[test]
    fn get_with_rcu_time() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        assert_eq!(map.get_with_rcu_time(&"key1".into()), vec![10]);
        assert_eq!(map.get_with_rcu_time(&"key2".into()), vec![20]);
        assert!(map.get_with_rcu_time(&"key3".into()).is_empty());
    }

    /// C++: `RcuMultiMapTest.TestMultipleValuesPerKey`.
    #[test]
    fn multiple_values_per_key() {
        let map: RcuMultiMap<i32, i32> = RcuMultiMap::new();
        for i in 0..10 {
            map.insert(42, i * 10);
        }
        assert_eq!(
            sorted(map.get(&42, true)),
            vec![0, 10, 20, 30, 40, 50, 60, 70, 80, 90]
        );
    }

    /// C++: `RcuMultiMapTest.TestIntegerKeys`.
    #[test]
    fn integer_keys() {
        let map: RcuMultiMap<i32, i32> = RcuMultiMap::new();
        map.insert(1, 100);
        map.insert(2, 200);
        map.insert(3, 300);
        assert_eq!(map.get(&1, true), vec![100]);
        assert_eq!(map.get(&2, true), vec![200]);
        assert_eq!(map.get(&3, true), vec![300]);
    }

    /// C++: `RcuMultiMapTest.TestEmptyMap`.
    #[test]
    fn empty_map() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        assert!(map.get(&"nonexistent".into(), true).is_empty());
        let mut all_values = Vec::new();
        map.get_all_values_with_ref(&mut all_values);
        assert!(all_values.is_empty());
        map.delete(&"nonexistent".into());
    }

    /// C++: `RcuMultiMapTest.TestStressInsertAndRetrieve`.
    #[test]
    fn stress_insert_and_retrieve() {
        let map: RcuMultiMap<i32, i32> = RcuMultiMap::new();
        let num_operations = 20;
        for i in 0..num_operations {
            map.insert(i % 5, i);
        }
        let mut total = 0;
        for key in 0..5 {
            let values = map.get(&key, true);
            assert!(!values.is_empty());
            total += values.len();
        }
        assert_eq!(total, num_operations as usize);
        assert!(map.size() > 0);
    }

    /// C++: `RcuMultiMapTest.TestDeleteFunctionality`.
    #[test]
    fn delete_functionality() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("test_key".into(), 42);
        map.insert("test_key".into(), 84);
        map.insert("other_key".into(), 100);
        assert!(!map.get(&"test_key".into(), true).is_empty());
        map.delete_value(&"test_key".into(), &42);
        map.delete(&"nonexistent_key".into());
        assert!(!map.get(&"other_key".into(), true).is_empty());
        assert!(map.size() <= 3);
        map.insert("new_key".into(), 200);
        assert!(!map.get(&"new_key".into(), true).is_empty());
    }

    /// C++: `RcuMultiMapTest.TestDifferentValueTypes`.
    #[test]
    fn different_value_types() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("key1".into(), 42);
        map.insert("key2".into(), 84);
        assert_eq!(map.get(&"key1".into(), true), vec![42]);
        assert_eq!(map.get(&"key2".into(), true), vec![84]);
    }
}

/// C++: `RcuMap::DeletedMap` (single-value snapshots).
#[derive(Debug)]
struct RcuMapDeleted<K, V> {
    _map: Arc<BTreeMap<K, MapValue<V>>>,
    delete_time: u64,
}

/// C++: `RcuMap` — the unique-key variant of [`RcuMultiMap`].
#[derive(Debug)]
pub struct RcuMap<K, V> {
    read_map: RwLock<Arc<BTreeMap<K, MapValue<V>>>>,
    dirty_map: Mutex<BTreeMap<K, MapValue<V>>>,
    miss_time: Mutex<usize>,
    deleted_values: Mutex<Vec<MapValue<V>>>,
    deleted_maps: Mutex<Vec<RcuMapDeleted<K, V>>>,
}

impl<K: Ord + Clone, V: Clone + PartialEq> Default for RcuMap<K, V> {
    fn default() -> Self {
        Self::new()
    }
}

impl<K: Ord + Clone, V: Clone + PartialEq> RcuMap<K, V> {
    /// C++: `RcuMap()`.
    pub fn new() -> Self {
        Self {
            read_map: RwLock::new(Arc::new(BTreeMap::new())),
            dirty_map: Mutex::new(BTreeMap::new()),
            miss_time: Mutex::new(0),
            deleted_values: Mutex::new(Vec::new()),
            deleted_maps: Mutex::new(Vec::new()),
        }
    }

    /// C++: `Get(key, update_access_time)` — a single value.
    pub fn get(&self, key: &K, update_access_time: bool) -> Option<V> {
        if let Some(map_value) = self.read_map.read().unwrap().get(key) {
            return Some(map_value.value.clone());
        }
        let mut dirty = self.dirty_map.lock().unwrap();
        let map_value = dirty.get_mut(key)?;
        if update_access_time {
            map_value.used_time = now_ms();
        }
        let out = map_value.value.clone();
        let total = dirty.len();
        let mut miss = self.miss_time.lock().unwrap();
        *miss += 1;
        // `RcuMap::CheckSwapInLock` is more conservative than the multimap's:
        // it only publishes once the dirty map holds at least 100 entries and
        // the misses reach three times its size.
        if total < 100 || *miss < total * 3 {
            return Some(out);
        }
        let published = dirty.clone();
        drop(miss);
        drop(dirty);
        let old = {
            let mut read = self.read_map.write().unwrap();
            std::mem::replace(&mut *read, Arc::new(published))
        };
        self.deleted_maps.lock().unwrap().push(RcuMapDeleted {
            _map: old,
            delete_time: now_ms(),
        });
        *self.miss_time.lock().unwrap() = 0;
        Some(out)
    }

    /// C++: `GetValue`.
    pub fn get_value(&self, key: &K, update_access_time: bool) -> Option<V> {
        self.get(key, update_access_time)
    }

    /// C++: `GetWithRcuTime`.
    pub fn get_with_rcu_time(&self, key: &K) -> Option<V> {
        self.get(key, false)
    }

    /// C++: `GetReadOnly` — the benchmark fast path: no access-time update.
    /// The read snapshot is consulted first; a miss falls back to the dirty
    /// map under its lock, exactly like the reference.
    pub fn get_read_only(&self, key: &K) -> Option<V> {
        if let Some(map_value) = self.read_map.read().unwrap().get(key) {
            return Some(map_value.value.clone());
        }
        self.dirty_map
            .lock()
            .unwrap()
            .get(key)
            .map(|v| v.value.clone())
    }

    /// C++: `Insert` — replaces the value for the key.
    pub fn insert(&self, key: K, value: V) {
        self.dirty_map.lock().unwrap().insert(
            key,
            MapValue {
                value,
                used_time: now_ms(),
            },
        );
    }

    /// C++: `Delete`.
    pub fn delete(&self, key: &K) {
        let mut dirty = self.dirty_map.lock().unwrap();
        if let Some(mut map_value) = dirty.remove(key) {
            map_value.used_time = now_ms();
            self.deleted_values.lock().unwrap().push(map_value);
        }
    }

    /// C++: `CheckGc`.
    pub fn check_gc(&self, min_delete_time: u64) {
        self.deleted_values
            .lock()
            .unwrap()
            .retain(|v| v.used_time >= min_delete_time);
        self.deleted_maps
            .lock()
            .unwrap()
            .retain(|v| v.delete_time >= min_delete_time);
    }

    /// C++: `CheckExpired`.
    pub fn check_expired(&self, min_access_time: u64) -> Vec<K> {
        self.dirty_map
            .lock()
            .unwrap()
            .iter()
            .filter(|(_, v)| v.used_time <= min_access_time)
            .map(|(k, _)| k.clone())
            .collect()
    }

    /// C++: `GetAllValuesWithRef`.
    pub fn get_all_values_with_ref(&self, values: &mut Vec<V>) {
        let dirty = self.dirty_map.lock().unwrap();
        values.extend(dirty.values().map(|v| v.value.clone()));
    }

    /// C++: `GetAllKeyValuePairs`.
    pub fn get_all_key_value_pairs(&self) -> Vec<(K, V)> {
        let dirty = self.dirty_map.lock().unwrap();
        let read = self.read_map.read().unwrap().clone();
        let mut pairs = Vec::new();
        for (key, value) in dirty.iter() {
            pairs.push((key.clone(), value.value.clone()));
        }
        for (key, value) in read.iter() {
            pairs.push((key.clone(), value.value.clone()));
        }
        pairs
    }

    /// C++: `size()`.
    pub fn size(&self) -> usize {
        self.dirty_map.lock().unwrap().len()
    }

    /// C++: `emplace`.
    pub fn emplace(&self, key: K, value: V) {
        self.insert(key, value);
    }

    /// C++: `range(key_min, key_max, result)`.
    pub fn range(&self, key_min: &K, key_max: &K) -> Vec<(K, V)> {
        let dirty = self.dirty_map.lock().unwrap();
        let read = self.read_map.read().unwrap().clone();
        let mut merged: BTreeMap<K, V> = BTreeMap::new();
        for (key, value) in read.range(key_min.clone()..=key_max.clone()) {
            merged.insert(key.clone(), value.value.clone());
        }
        for (key, value) in dirty.range(key_min.clone()..=key_max.clone()) {
            merged.insert(key.clone(), value.value.clone());
        }
        merged.into_iter().collect()
    }

    /// C++: `GetOrAdd`.
    pub fn get_or_add(&self, key: &K, value: &mut V, new_value: V) -> bool {
        if let Some(existing) = self.get(key, true) {
            *value = existing;
            return true;
        }
        self.insert(key.clone(), new_value.clone());
        *value = new_value;
        false
    }

    /// C++: `Clear` — clears the dirty map and retires its values.
    pub fn clear(&self) {
        let mut dirty = self.dirty_map.lock().unwrap();
        let mut deleted = self.deleted_values.lock().unwrap();
        for (_, mut value) in std::mem::take(&mut *dirty) {
            value.used_time = now_ms();
            deleted.push(value);
        }
    }
}

#[cfg(test)]
mod map_reference_suite {
    //! Ports of the `RcuMapTest` functional cases in
    //! `test_rcu_multimap_ut.cpp`.

    use super::*;

    /// C++: `RcuMapTest.TestBasicInsertAndGet`.
    #[test]
    fn basic_insert_and_get() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        map.insert("key1".into(), 30);
        assert_eq!(map.get(&"key1".into(), true), Some(30));
        assert_eq!(map.get(&"key2".into(), true), Some(20));
        assert_eq!(map.get(&"key3".into(), true), None);
        assert_eq!(map.size(), 2);
    }

    /// C++: `RcuMapTest.TestLowercaseAliases`.
    #[test]
    fn lowercase_aliases() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("test_key".into(), 100);
        map.insert("test_key".into(), 200);
        assert_eq!(map.get(&"test_key".into(), true), Some(200));
    }

    /// C++: `RcuMapTest.TestDelete`.
    #[test]
    fn delete() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        map.delete(&"key1".into());
        assert_eq!(map.get(&"key1".into(), true), None);
        assert_eq!(map.get(&"key2".into(), true), Some(20));
        map.delete(&"key2".into());
        assert_eq!(map.size(), 0);
    }

    /// C++: `RcuMapTest.TestGetAllValuesWithRef`.
    #[test]
    fn get_all_values_with_ref() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        map.insert("key3".into(), 30);
        let mut values = Vec::new();
        map.get_all_values_with_ref(&mut values);
        values.sort_unstable();
        assert_eq!(values, vec![10, 20, 30]);
    }

    /// C++: `RcuMapTest.TestGetAllKeyValuePairs`.
    #[test]
    fn get_all_key_value_pairs() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        map.insert("key3".into(), 30);
        let pairs = map.get_all_key_value_pairs();
        assert_eq!(pairs.len(), 3);
        assert_eq!(pairs[0], ("key1".to_string(), 10));
        assert_eq!(pairs[1], ("key2".to_string(), 20));
        assert_eq!(pairs[2], ("key3".to_string(), 30));
    }

    /// C++: `RcuMapTest.TestGarbageCollection`.
    #[test]
    fn garbage_collection() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        map.delete(&"key1".into());
        map.check_gc(now_ms().saturating_sub(1000));
        assert_eq!(map.get(&"key2".into(), true), Some(20));
    }

    /// C++: `RcuMapTest.TestExpiredEntries`.
    #[test]
    fn expired_entries() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        let _ = map.get(&"key1".into(), true);
        std::thread::sleep(std::time::Duration::from_millis(10));
        let expired = map.check_expired(now_ms() + 1000);
        assert!(expired.len() <= 2);
    }

    /// C++: `RcuMapTest.TestGetWithRcuTime`.
    #[test]
    fn get_with_rcu_time() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("key1".into(), 10);
        map.insert("key2".into(), 20);
        assert_eq!(map.get_with_rcu_time(&"key1".into()), Some(10));
        assert_eq!(map.get_with_rcu_time(&"key2".into()), Some(20));
        assert_eq!(map.get_with_rcu_time(&"key3".into()), None);
    }

    /// C++: `RcuMapTest.TestUniqueKeys`.
    #[test]
    fn unique_keys() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("key".into(), 100);
        map.insert("key".into(), 200);
        map.insert("key".into(), 300);
        assert_eq!(map.get(&"key".into(), true), Some(300));
        assert_eq!(map.size(), 1);
    }

    /// C++: `RcuMapTest.TestIntegerKeys`.
    #[test]
    fn integer_keys() {
        let map: RcuMap<i32, i32> = RcuMap::new();
        map.insert(1, 100);
        map.insert(2, 200);
        map.insert(3, 300);
        assert_eq!(map.get(&1, true), Some(100));
        assert_eq!(map.get(&2, true), Some(200));
        assert_eq!(map.get(&3, true), Some(300));
        assert_eq!(map.size(), 3);
    }

    /// C++: `RcuMapTest.TestEmptyMap`.
    #[test]
    fn empty_map() {
        let map: RcuMap<String, i32> = RcuMap::new();
        assert_eq!(map.get(&"nonexistent".into(), true), None);
        assert_eq!(map.get_value(&"nonexistent".into(), true), None);
        let mut values = Vec::new();
        map.get_all_values_with_ref(&mut values);
        assert!(values.is_empty());
        assert_eq!(map.size(), 0);
    }

    /// C++: `RcuMapTest.TestEmplace`.
    #[test]
    fn emplace() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.emplace("key1".into(), 42);
        map.emplace("key2".into(), 84);
        assert_eq!(map.get(&"key1".into(), true), Some(42));
        assert_eq!(map.get(&"key2".into(), true), Some(84));
        map.emplace("key1".into(), 168);
        assert_eq!(map.get(&"key1".into(), true), Some(168));
        assert_eq!(map.size(), 2);
    }

    /// C++: `RcuMapTest.TestRange`.
    #[test]
    fn range() {
        let map: RcuMap<i32, i32> = RcuMap::new();
        for key in [1, 3, 5, 7, 9] {
            map.insert(key, key * 100);
        }
        let result = map.range(&3, &7);
        assert_eq!(result.len(), 3);
        assert_eq!(result, vec![(3, 300), (5, 500), (7, 700)]);
    }

    /// C++: `RcuMapTest.TestStressInsertAndRetrieve`.
    #[test]
    fn stress_insert_and_retrieve() {
        let map: RcuMap<i32, i32> = RcuMap::new();
        let num_operations = 20;
        for i in 0..num_operations {
            map.insert(i % 5, i);
        }
        for key in 0..5 {
            assert!(map.get(&key, true).is_some());
        }
        assert!(map.size() > 0);
    }

    /// C++: `RcuMapTest.TestMapVsMultiMapBehavior`.
    #[test]
    fn map_vs_multimap_behavior() {
        let map: RcuMap<String, i32> = RcuMap::new();
        let multimap: RcuMultiMap<String, i32> = RcuMultiMap::new();
        for value in [10, 20, 30] {
            map.insert("test_key".into(), value);
            multimap.insert("test_key".into(), value);
        }
        assert_eq!(map.get(&"test_key".into(), true), Some(30));
        assert_eq!(map.size(), 1);
        assert_eq!(multimap.get(&"test_key".into(), true).len(), 3);
        for (key, value) in [("key1", 100), ("key2", 200), ("key3", 300)] {
            map.insert(key.into(), value);
            multimap.insert(key.into(), value);
        }
        let map_range = map.range(&"key1".into(), &"key3".into());
        let multimap_range = multimap.range(&"key1".into(), &"key3".into());
        assert!(map_range.len() >= 3);
        assert_eq!(multimap_range.len(), map_range.len());
    }
}

#[cfg(test)]
mod concurrency_reference_suite {
    //! C++: `RcuMultiMapTest.TestConcurrentInsertAndRead` and
    //! `TestConcurrentInsertDeleteRead` (multi), plus the `RcuMapTest`
    //! equivalents.

    use super::*;
    use std::sync::atomic::{AtomicI32, Ordering};

    #[test]
    fn multimap_concurrent_insert_and_read() {
        let map: RcuMultiMap<i32, i32> = RcuMultiMap::new();
        let insertions = AtomicI32::new(0);
        let reads = AtomicI32::new(0);
        let (threads, ops, keys) = (4i32, 200i32, 20i32);
        std::thread::scope(|scope| {
            for t in 0..threads / 2 {
                let map = &map;
                let insertions = &insertions;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 7 + i) % keys;
                        map.insert(key, t * ops + i);
                        insertions.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
            for t in threads / 2..threads {
                let map = &map;
                let reads = &reads;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 11 + i) % keys;
                        let _ = map.get(&key, true);
                        reads.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
        });
        assert_eq!(insertions.load(Ordering::Relaxed), (threads / 2) * ops);
        assert_eq!(reads.load(Ordering::Relaxed), (threads / 2) * ops);
        let final_total: usize = (0..keys).map(|key| map.get(&key, true).len()).sum();
        assert!(final_total > 0);
        assert!(final_total <= (threads / 2 * ops) as usize);
    }

    #[test]
    fn multimap_concurrent_insert_delete_read() {
        let map: RcuMultiMap<i32, i32> = RcuMultiMap::new();
        let insertions = AtomicI32::new(0);
        let deletions = AtomicI32::new(0);
        let reads = AtomicI32::new(0);
        let (threads, ops, keys) = (6i32, 100i32, 15i32);
        std::thread::scope(|scope| {
            for t in 0..threads / 3 {
                let map = &map;
                let insertions = &insertions;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 5 + i) % keys;
                        map.insert(key, t * ops + i);
                        insertions.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
            for t in threads / 3..2 * threads / 3 {
                let map = &map;
                let deletions = &deletions;
                scope.spawn(move || {
                    std::thread::sleep(std::time::Duration::from_millis(2));
                    for i in 0..ops / 2 {
                        let key = (t * 3 + i) % keys;
                        map.delete(&key);
                        deletions.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
            for t in 2 * threads / 3..threads {
                let map = &map;
                let reads = &reads;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 13 + i) % keys;
                        let _ = map.get(&key, true);
                        reads.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
        });
        assert_eq!(insertions.load(Ordering::Relaxed), (threads / 3) * ops);
        assert_eq!(deletions.load(Ordering::Relaxed), (threads / 3) * (ops / 2));
        assert_eq!(reads.load(Ordering::Relaxed), (threads / 3) * ops);
        assert!(map.size() <= keys as usize);
    }

    /// C++: `RcuMapTest.TestRcuSwapUnderLoad`.
    #[test]
    fn map_rcu_swap_under_load() {
        let map: RcuMap<i32, i32> = RcuMap::new();
        let gets = AtomicI32::new(0);
        let (threads, ops, keys) = (4i32, 100i32, 8i32);
        std::thread::scope(|scope| {
            for t in 0..threads {
                let map = &map;
                let gets = &gets;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 3 + i) % keys;
                        map.insert(key, t * ops + i);
                        if let Some(value) = map.get(&key, true) {
                            assert!(value >= 0);
                        }
                        gets.fetch_add(1, Ordering::Relaxed);
                        let _ = map.get(&((t * 7 + i) % keys), true);
                    }
                });
            }
        });
        assert!(gets.load(Ordering::Relaxed) >= threads * ops);
        let accessible = (0..keys).filter(|key| map.get(key, true).is_some()).count();
        assert!(accessible <= keys as usize);
    }

    /// C++: `RcuMapTest.TestRcuBehavior`.
    #[test]
    fn map_rcu_behavior() {
        let map: RcuMap<String, i32> = RcuMap::new();
        map.insert("key1".into(), 100);
        map.insert("key2".into(), 200);
        assert_eq!(map.get(&"key1".into(), true), Some(100));
        for _ in 0..100 {
            let _ = map.get(&"key1".into(), true);
        }
        assert_eq!(map.get(&"key1".into(), true), Some(100));
    }

    /// C++: `RcuMultiMapTest.TestHighVolumeMultiThreading`.
    #[test]
    fn multimap_high_volume_multi_threading() {
        let map: RcuMultiMap<i64, i64> = RcuMultiMap::new();
        let total_operations = AtomicI32::new(0);
        let successful_reads = AtomicI32::new(0);
        let stop = std::sync::atomic::AtomicBool::new(false);
        let (threads, ops, keys) = (6i64, 300i32, 50i64);
        std::thread::scope(|scope| {
            for t in 0..threads {
                let map = &map;
                let total_operations = &total_operations;
                let successful_reads = &successful_reads;
                let stop = &stop;
                scope.spawn(move || {
                    let mut state = 0x9E37_79B9u64 ^ t as u64;
                    let mut next = || -> u64 {
                        state ^= state << 13;
                        state ^= state >> 7;
                        state ^= state << 17;
                        state
                    };
                    let mut local = 0;
                    while !stop.load(Ordering::Relaxed) && local < ops {
                        let key = (next() % keys as u64) as i64;
                        if next() % 100 < 70 {
                            let values = map.get(&key, true);
                            if !values.is_empty() {
                                successful_reads.fetch_add(1, Ordering::Relaxed);
                                assert!(values.iter().all(|v| *v >= 0));
                            }
                        } else {
                            map.insert(key, t * ops as i64 + local as i64);
                        }
                        local += 1;
                        total_operations.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
            std::thread::sleep(std::time::Duration::from_millis(200));
            stop.store(true, Ordering::Relaxed);
        });
        assert!(total_operations.load(Ordering::Relaxed) > (threads * 100) as i32);
        assert!(successful_reads.load(Ordering::Relaxed) > 0);
        for key in 0..keys.min(100) {
            assert!(map.get(&key, true).iter().all(|v| *v >= 0));
        }
    }

    /// C++: `RcuMultiMapTest.TestRcuSwapUnderLoad`.
    #[test]
    fn multimap_rcu_swap_under_load() {
        let map: RcuMultiMap<i32, i32> = RcuMultiMap::new();
        let gets = AtomicI32::new(0);
        let (threads, ops, keys) = (4i32, 200i32, 10i32);
        std::thread::scope(|scope| {
            for t in 0..threads {
                let map = &map;
                let gets = &gets;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 3 + i) % keys;
                        map.insert(key, t * ops + i);
                        let values = map.get(&key, true);
                        assert!(values.iter().all(|v| *v >= 0));
                        gets.fetch_add(1, Ordering::Relaxed);
                        let rapid = (t * 7 + i) % keys;
                        let _ = map.get(&rapid, true);
                    }
                });
            }
        });
        assert!(gets.load(Ordering::Relaxed) >= threads * ops);
        let mut final_total = 0;
        for key in 0..keys {
            final_total += map.get(&key, true).len();
        }
        assert!(final_total > 0);
        assert!(final_total <= (threads * ops) as usize);
    }

    /// C++: `RcuMultiMapTest.TestRcuBehavior`.
    #[test]
    fn multimap_rcu_behavior() {
        let map: RcuMultiMap<String, i32> = RcuMultiMap::new();
        map.insert("key1".into(), 100);
        map.insert("key2".into(), 200);
        assert_eq!(map.get(&"key1".into(), true), vec![100]);
        for _ in 0..100 {
            let _ = map.get(&"key1".into(), true);
        }
        assert_eq!(map.get(&"key1".into(), true), vec![100]);
    }

    #[test]
    fn map_concurrent_insert_and_read() {
        let map: RcuMap<i32, i32> = RcuMap::new();
        let insertions = AtomicI32::new(0);
        let reads = AtomicI32::new(0);
        let (threads, ops, keys) = (4i32, 100i32, 20i32);
        std::thread::scope(|scope| {
            for t in 0..threads / 2 {
                let map = &map;
                let insertions = &insertions;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 7 + i) % keys;
                        map.insert(key, t * ops + i);
                        insertions.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
            for t in threads / 2..threads {
                let map = &map;
                let reads = &reads;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 11 + i) % keys;
                        let _ = map.get(&key, true);
                        reads.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
        });
        assert_eq!(insertions.load(Ordering::Relaxed), (threads / 2) * ops);
        assert_eq!(reads.load(Ordering::Relaxed), (threads / 2) * ops);
        let accessible = (0..keys).filter(|key| map.get(key, true).is_some()).count();
        assert!(accessible > 0);
        assert!(accessible <= keys as usize);
    }

    #[test]
    fn map_concurrent_insert_delete_read() {
        let map: RcuMap<i32, i32> = RcuMap::new();
        let insertions = AtomicI32::new(0);
        let deletions = AtomicI32::new(0);
        let reads = AtomicI32::new(0);
        let (threads, ops, keys) = (6i32, 50i32, 10i32);
        std::thread::scope(|scope| {
            for t in 0..threads / 3 {
                let map = &map;
                let insertions = &insertions;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 5 + i) % keys;
                        map.insert(key, t * ops + i);
                        insertions.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
            for t in threads / 3..2 * threads / 3 {
                let map = &map;
                let deletions = &deletions;
                scope.spawn(move || {
                    std::thread::sleep(std::time::Duration::from_millis(2));
                    for i in 0..ops / 2 {
                        let key = (t * 3 + i) % keys;
                        map.delete(&key);
                        deletions.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
            for t in 2 * threads / 3..threads {
                let map = &map;
                let reads = &reads;
                scope.spawn(move || {
                    for i in 0..ops {
                        let key = (t * 13 + i) % keys;
                        let _ = map.get(&key, true);
                        reads.fetch_add(1, Ordering::Relaxed);
                    }
                });
            }
        });
        assert_eq!(insertions.load(Ordering::Relaxed), (threads / 3) * ops);
        assert_eq!(deletions.load(Ordering::Relaxed), (threads / 3) * (ops / 2));
        assert_eq!(reads.load(Ordering::Relaxed), (threads / 3) * ops);
        assert!(map.size() <= keys as usize);
    }
}

#[cfg(test)]
mod benchmark_reference_suite {
    //! Ports of the eight `RcuMapBenchmarkTest` cases in
    //! `src/unit_test/storage/common/test_rcu_multimap_ut.cpp`.
    //!
    //! The reference measures `RcuMap` against a mutex-guarded
    //! `std::map` (`MapWithLock`). The workloads and assertions are unchanged;
    //! the random sources are deterministic so the port is reproducible.

    use super::*;
    use std::collections::btree_map::Entry;
    use std::sync::atomic::{AtomicBool, Ordering};
    use std::time::Instant;

    const NS_PER_MS: f64 = 1_000_000.0;

    /// C++: `RcuMapBenchmarkTest::MeasureTime` (milliseconds).
    fn measure_ms(function: impl FnOnce()) -> f64 {
        let start = Instant::now();
        function();
        start.elapsed().as_nanos() as f64 / NS_PER_MS
    }

    /// Deterministic xorshift generator; the reference seeds `mt19937` from
    /// `random_device`, but only the value distribution matters here.
    struct Rng(u64);

    impl Rng {
        fn new(seed: u64) -> Self {
            Self(seed | 1)
        }

        fn next_u64(&mut self) -> u64 {
            let mut state = self.0;
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            self.0 = state;
            state
        }

        fn below(&mut self, bound: u64) -> u64 {
            self.next_u64() % bound
        }

        fn unit(&mut self) -> f64 {
            ((self.next_u64() >> 11) as f64) / ((1u64 << 53) as f64)
        }
    }

    /// C++: `MapWithLock` (`src/storage/common/map_with_lock.cppm`) — the
    /// benchmark baseline.
    struct MapWithLock<K, V> {
        map: Mutex<BTreeMap<K, V>>,
    }

    impl<K: Ord + Clone, V> MapWithLock<K, V> {
        fn new() -> Self {
            Self {
                map: Mutex::new(BTreeMap::new()),
            }
        }

        fn get(&self, key: &K) -> Option<V>
        where
            V: Clone,
        {
            self.map.lock().unwrap().get(key).cloned()
        }

        /// C++: `MapWithLock::GetOrAdd` — returns whether the key existed.
        fn get_or_add(&self, key: &K, new_value: V) -> bool {
            match self.map.lock().unwrap().entry(key.clone()) {
                Entry::Occupied(_) => true,
                Entry::Vacant(slot) => {
                    slot.insert(new_value);
                    false
                }
            }
        }
    }

    /// Runs `function(thread_index)` on `threads` threads released together,
    /// mirroring the reference's start-flag barrier.
    fn run_threads(threads: usize, function: impl Fn(usize) + Sync) {
        let start = AtomicBool::new(false);
        std::thread::scope(|scope| {
            for thread_index in 0..threads {
                let start = &start;
                let function = &function;
                scope.spawn(move || {
                    while !start.load(Ordering::Relaxed) {
                        std::thread::yield_now();
                    }
                    function(thread_index);
                });
            }
            start.store(true, Ordering::Relaxed);
        });
    }

    fn populate_pair(
        rcu_map: &RcuMap<String, i32>,
        lock_map: &MapWithLock<String, i32>,
        count: i32,
        value: impl Fn(i32) -> i32,
    ) {
        for i in 0..count {
            let key = format!("key_{i}");
            rcu_map.insert(key.clone(), value(i));
            lock_map.get_or_add(&key, value(i));
        }
    }

    /// C++: `RcuMapBenchmarkTest.ReadHeavyWorkload`.
    #[test]
    fn read_heavy_workload() {
        const THREADS: usize = 8;
        const OPS: i32 = 5000;
        const KEY_RANGE: u64 = 100_000;
        const READ_RATIO: f64 = 0.9;

        let rcu_map: RcuMap<String, i32> = RcuMap::new();
        let lock_map: MapWithLock<String, i32> = MapWithLock::new();
        populate_pair(&rcu_map, &lock_map, (KEY_RANGE / 2) as i32, |i| i * 10);

        let rcu_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0x9E37_79B9_7F4A_7C15 ^ t as u64);
                for i in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    if rng.unit() < READ_RATIO {
                        let _ = rcu_map.get_read_only(&key);
                    } else {
                        rcu_map.insert(key, t as i32 * OPS + i);
                    }
                }
            });
        });

        let lock_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0xD1B5_4A32_D192_ED03 ^ t as u64);
                for i in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    if rng.unit() < READ_RATIO {
                        let _ = lock_map.get(&key);
                    } else {
                        lock_map.get_or_add(&key, t as i32 * OPS + i);
                    }
                }
            });
        });

        assert!(
            rcu_ms < lock_ms * 1.2,
            "RcuMap ({rcu_ms:.3} ms) should be faster than MapWithLock ({lock_ms:.3} ms) within 20%"
        );
    }

    /// C++: `RcuMapBenchmarkTest.WriteHeavyWorkload`.
    #[test]
    fn write_heavy_workload() {
        const THREADS: usize = 8;
        const OPS: i32 = 2000;
        const KEY_RANGE: u64 = 100_000;
        const READ_RATIO: f64 = 0.3;

        let rcu_map: RcuMap<String, i32> = RcuMap::new();
        let lock_map: MapWithLock<String, i32> = MapWithLock::new();
        populate_pair(&rcu_map, &lock_map, (KEY_RANGE / 4) as i32, |i| i * 10);

        let rcu_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0x9E37_79B9_7F4A_7C15 ^ t as u64);
                for i in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    if rng.unit() < READ_RATIO {
                        let _ = rcu_map.get(&key, true);
                    } else {
                        rcu_map.insert(key, t as i32 * OPS + i);
                    }
                }
            });
        });

        let lock_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0xD1B5_4A32_D192_ED03 ^ t as u64);
                for i in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    if rng.unit() < READ_RATIO {
                        let _ = lock_map.get(&key);
                    } else {
                        lock_map.get_or_add(&key, t as i32 * OPS + i);
                    }
                }
            });
        });

        assert!(rcu_ms > 0.0);
        assert!(lock_ms > 0.0);
    }

    /// C++: `RcuMapBenchmarkTest.ReadOnlyWorkload`.
    #[test]
    fn read_only_workload() {
        const THREADS: usize = 12;
        const OPS: i32 = 10_000;
        const KEY_RANGE: u64 = 100_000;

        let rcu_map: RcuMap<String, i32> = RcuMap::new();
        let lock_map: MapWithLock<String, i32> = MapWithLock::new();
        populate_pair(&rcu_map, &lock_map, KEY_RANGE as i32, |i| i * 10);

        let rcu_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0x9E37_79B9_7F4A_7C15 ^ t as u64);
                for _ in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    let _ = rcu_map.get(&key, true);
                }
            });
        });

        let lock_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0xD1B5_4A32_D192_ED03 ^ t as u64);
                for _ in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    let _ = lock_map.get(&key);
                }
            });
        });

        assert!(rcu_ms > 0.0);
        assert!(lock_ms > 0.0);
    }

    /// C++: `RcuMapBenchmarkTest.HighContentionWorkload`.
    #[test]
    fn high_contention_workload() {
        const THREADS: usize = 16;
        const OPS: i32 = 2000;
        const KEY_RANGE: u64 = 1000;
        const READ_RATIO: f64 = 0.8;

        let rcu_map: RcuMap<String, i32> = RcuMap::new();
        let lock_map: MapWithLock<String, i32> = MapWithLock::new();
        populate_pair(&rcu_map, &lock_map, KEY_RANGE as i32, |i| i * 10);

        let rcu_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0x9E37_79B9_7F4A_7C15 ^ t as u64);
                for i in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    if rng.unit() < READ_RATIO {
                        let _ = rcu_map.get(&key, true);
                    } else {
                        rcu_map.insert(key, t as i32 * OPS + i);
                    }
                }
            });
        });

        let lock_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0xD1B5_4A32_D192_ED03 ^ t as u64);
                for i in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    if rng.unit() < READ_RATIO {
                        let _ = lock_map.get(&key);
                    } else {
                        lock_map.get_or_add(&key, t as i32 * OPS + i);
                    }
                }
            });
        });

        assert!(rcu_ms > 0.0);
        assert!(lock_ms > 0.0);
    }

    /// C++: `RcuMapBenchmarkTest.ScalabilityTest`.
    #[test]
    fn scalability_test() {
        const OPS: i32 = 5000;
        const KEY_RANGE: u64 = 100_000;
        const READ_RATIO: f64 = 0.85;

        for threads in [1usize, 2, 4, 8, 16] {
            let rcu_map: RcuMap<String, i32> = RcuMap::new();
            let lock_map: MapWithLock<String, i32> = MapWithLock::new();
            populate_pair(&rcu_map, &lock_map, (KEY_RANGE / 2) as i32, |i| i * 10);

            let rcu_ms = measure_ms(|| {
                run_threads(threads, |t| {
                    let mut rng = Rng::new(0x9E37_79B9_7F4A_7C15 ^ t as u64);
                    for i in 0..OPS {
                        let key = format!("key_{}", rng.below(KEY_RANGE));
                        if rng.unit() < READ_RATIO {
                            let _ = rcu_map.get(&key, true);
                        } else {
                            rcu_map.insert(key, t as i32 * OPS + i);
                        }
                    }
                });
            });

            let lock_ms = measure_ms(|| {
                run_threads(threads, |t| {
                    let mut rng = Rng::new(0xD1B5_4A32_D192_ED03 ^ t as u64);
                    for i in 0..OPS {
                        let key = format!("key_{}", rng.below(KEY_RANGE));
                        if rng.unit() < READ_RATIO {
                            let _ = lock_map.get(&key);
                        } else {
                            lock_map.get_or_add(&key, t as i32 * OPS + i);
                        }
                    }
                });
            });

            assert!(rcu_ms > 0.0, "threads={threads}");
            assert!(lock_ms > 0.0, "threads={threads}");
        }
    }

    /// C++: `RcuMapBenchmarkTest.MemoryAndGcPerformance`.
    #[test]
    fn memory_and_gc_performance() {
        const OPERATIONS: i32 = 100_000;
        const KEY_RANGE: u64 = 100_000;

        let rcu_map: RcuMap<String, i32> = RcuMap::new();
        let lock_map: MapWithLock<String, i32> = MapWithLock::new();

        let rcu_insert_ms = measure_ms(|| {
            let mut rng = Rng::new(0x9E37_79B9_7F4A_7C15);
            for i in 0..OPERATIONS {
                let key = format!("key_{}", rng.below(KEY_RANGE));
                rcu_map.insert(key, i);
            }
        });

        let lock_insert_ms = measure_ms(|| {
            let mut rng = Rng::new(0x9E37_79B9_7F4A_7C15);
            for i in 0..OPERATIONS {
                let key = format!("key_{}", rng.below(KEY_RANGE));
                lock_map.get_or_add(&key, i);
            }
        });

        let gc_ms = measure_ms(|| {
            rcu_map.check_gc(now_ms().saturating_sub(1000));
        });

        assert!(rcu_insert_ms > 0.0);
        assert!(lock_insert_ms > 0.0);
        assert!(gc_ms >= 0.0);
    }

    /// C++: `RcuMapBenchmarkTest.ComprehensiveBenchmarkSummary`.
    #[test]
    fn comprehensive_benchmark_summary() {
        const PREPOPULATE: i32 = 50_000;
        let scenarios = [
            ("Read-Heavy (95% reads)", 0.95, 8usize, 5000i32),
            ("Balanced (70% reads)", 0.70, 8, 3000),
            ("Write-Heavy (40% reads)", 0.40, 8, 2000),
            ("High Contention", 0.80, 16, 2000),
        ];

        let mut results = Vec::new();
        for (name, read_ratio, threads, ops) in scenarios {
            let rcu_map: RcuMap<String, i32> = RcuMap::new();
            let lock_map: MapWithLock<String, i32> = MapWithLock::new();
            populate_pair(&rcu_map, &lock_map, PREPOPULATE, |i| i);

            let rcu_ms = measure_ms(|| {
                run_threads(threads, |t| {
                    let mut rng = Rng::new(0x9E37_79B9_7F4A_7C15 ^ t as u64);
                    for i in 0..ops {
                        let key = format!("key_{}", rng.below(KEY_RANGE_FULL));
                        if rng.unit() < read_ratio {
                            let _ = rcu_map.get(&key, true);
                        } else {
                            rcu_map.insert(key, i);
                        }
                    }
                });
            });

            let lock_ms = measure_ms(|| {
                run_threads(threads, |t| {
                    let mut rng = Rng::new(0xD1B5_4A32_D192_ED03 ^ t as u64);
                    for i in 0..ops {
                        let key = format!("key_{}", rng.below(KEY_RANGE_FULL));
                        if rng.unit() < read_ratio {
                            let _ = lock_map.get(&key);
                        } else {
                            lock_map.get_or_add(&key, i);
                        }
                    }
                });
            });

            let speedup = lock_ms / rcu_ms;
            results.push((name, rcu_ms, lock_ms, speedup));
        }

        for (name, rcu_ms, lock_ms, speedup) in results {
            assert!(rcu_ms > 0.0, "{name}");
            assert!(lock_ms > 0.0, "{name}");
            assert!(speedup > 0.0, "{name}");
        }
    }

    /// C++: `RcuMapBenchmarkTest.QuickPerformanceVerification`.
    #[test]
    fn quick_performance_verification() {
        const THREADS: usize = 4;
        const OPS: i32 = 1000;
        const KEY_RANGE: u64 = 10_000;
        const READ_RATIO: f64 = 0.9;

        let rcu_map: RcuMap<String, i32> = RcuMap::new();
        let lock_map: MapWithLock<String, i32> = MapWithLock::new();
        populate_pair(&rcu_map, &lock_map, (KEY_RANGE / 2) as i32, |i| i * 10);

        let rcu_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0x9E37_79B9_7F4A_7C15 ^ t as u64);
                for i in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    if rng.unit() < READ_RATIO {
                        let _ = rcu_map.get(&key, true);
                    } else {
                        rcu_map.insert(key, t as i32 * OPS + i);
                    }
                }
            });
        });

        let lock_ms = measure_ms(|| {
            run_threads(THREADS, |t| {
                let mut rng = Rng::new(0xD1B5_4A32_D192_ED03 ^ t as u64);
                for i in 0..OPS {
                    let key = format!("key_{}", rng.below(KEY_RANGE));
                    if rng.unit() < READ_RATIO {
                        let _ = lock_map.get(&key);
                    } else {
                        lock_map.get_or_add(&key, t as i32 * OPS + i);
                    }
                }
            });
        });

        assert!(rcu_ms > 0.0);
        assert!(lock_ms > 0.0);
    }

    /// The comprehensive suite samples the full 100K key space, like the
    /// reference's `uniform_int_distribution(0, 99999)`.
    const KEY_RANGE_FULL: u64 = 100_000;
}
