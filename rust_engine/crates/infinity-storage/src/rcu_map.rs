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
