//! Concurrent skip-list container used by the realtime inverted index.
//!
//! Port of `src/storage/invertedindex/common/skiplist.cppm`: a level-linked
//! list with randomized levels (`MAX_LEVEL = 20`, promotion probability 1/3),
//! `Insert` (which replaces an existing key's value) and exact `Search`.
//! The C++ implementation is lock-free; this port keeps the same observable
//! behavior under concurrent readers/writers with one `RwLock` over the arena.

use std::cmp::Ordering;
use std::sync::{Arc, RwLock};

/// `MAX_LEVEL` (`skiplist.cppm:26`).
const MAX_LEVEL: usize = 20;

/// Key ordering, the analogue of the C++ comparator functor.
pub trait Comparator<K> {
    fn compare(&self, lhs: &K, rhs: &K) -> Ordering;
}

/// `NumComparator`: `int` ordering (`skiplist_ut_nimpl.cpp:48-58`).
#[derive(Clone, Copy, Debug, Default)]
pub struct NumComparator;

impl Comparator<u32> for NumComparator {
    fn compare(&self, lhs: &u32, rhs: &u32) -> Ordering {
        (*lhs as i32).cmp(&(*rhs as i32))
    }
}

/// `KeyComparator`: `strcmp` ordering (`skiplist.cppm:49-54`).
#[derive(Clone, Copy, Debug, Default)]
pub struct KeyComparator;

impl Comparator<String> for KeyComparator {
    fn compare(&self, lhs: &String, rhs: &String) -> Ordering {
        lhs.as_bytes().cmp(rhs.as_bytes())
    }
}

struct Node<K, V> {
    key: K,
    value: V,
    next: Vec<Option<usize>>,
}

struct Inner<K, V> {
    nodes: Vec<Node<K, V>>,
    /// Level links out of the head node.
    head: Vec<Option<usize>>,
    height: usize,
    rng: u64,
}

/// A concurrent skip list (`SkipList<KeyType, ValueType, Comparator>`).
pub struct SkipList<K, V, C> {
    inner: Arc<RwLock<Inner<K, V>>>,
    cmp: C,
}

impl<K, V, C> Clone for SkipList<K, V, C>
where
    C: Clone,
{
    fn clone(&self) -> Self {
        Self {
            inner: Arc::clone(&self.inner),
            cmp: self.cmp.clone(),
        }
    }
}

impl<K, V, C> SkipList<K, V, C>
where
    K: Clone + Ord,
    V: Clone,
    C: Comparator<K>,
{
    /// `SkipList::SkipList`: an empty list with one head sentinel.
    pub fn new(cmp: C) -> Self {
        Self {
            inner: Arc::new(RwLock::new(Inner {
                nodes: Vec::new(),
                head: vec![None; MAX_LEVEL],
                height: 1,
                rng: 0x9e37_79b9_7f4a_7c15,
            })),
            cmp,
        }
    }

    fn random_level(inner: &mut Inner<K, V>) -> usize {
        // `RandomLevel`: advance while `dis(gen) < MAX_NUM / 3`.
        inner.rng = inner
            .rng
            .wrapping_mul(6364136223846793005)
            .wrapping_add(1442695040888963407);
        let mut level = 1;
        while !(inner.rng >> 33).is_multiple_of(3) && level < MAX_LEVEL {
            level += 1;
        }
        level
    }

    fn key_at(inner: &Inner<K, V>, index: usize) -> &K {
        &inner.nodes[index].key
    }

    /// `SkipList::Insert`: insert or replace, returns `true` like the C++.
    pub fn insert(&self, key: K, value: V) -> bool {
        let mut inner = self.inner.write().unwrap();
        let old_height = inner.height;
        let mut preds = [usize::MAX; MAX_LEVEL];
        let mut succs: [Option<usize>; MAX_LEVEL] = [None; MAX_LEVEL];
        let mut current: Option<usize> = None;
        for level in (0..old_height).rev() {
            let mut next = match current {
                None => inner.head[level],
                Some(node) => inner.nodes[node].next[level],
            };
            while let Some(node) = next {
                if Self::key_at(&inner, node) < &key {
                    current = Some(node);
                    next = inner.nodes[node].next[level];
                } else {
                    break;
                }
            }
            preds[level] = current.unwrap_or(usize::MAX);
            succs[level] = next;
        }
        if let Some(node) = succs[0] {
            if Self::key_at(&inner, node) == &key {
                inner.nodes[node].value = value;
                return true;
            }
        }

        let height = Self::random_level(&mut inner);
        for level in old_height..height {
            preds[level] = usize::MAX;
            succs[level] = inner.head[level];
        }
        let node_index = inner.nodes.len();
        inner.nodes.push(Node {
            key,
            value,
            next: succs[..height].to_vec(),
        });
        for (level, pred) in preds.iter().enumerate().take(height) {
            match *pred {
                usize::MAX => inner.head[level] = Some(node_index),
                pred => inner.nodes[pred].next[level] = Some(node_index),
            }
        }
        if height > inner.height {
            inner.height = height;
        }
        true
    }

    /// `SkipList::Search`: exact-key lookup.
    pub fn search(&self, key: &K) -> Option<V> {
        let inner = self.inner.read().unwrap();
        let mut level = inner.height - 1;
        let mut current: Option<usize> = None;
        loop {
            let next = match current {
                None => inner.head[level],
                Some(node) => inner.nodes[node].next[level],
            };
            match next {
                Some(next) if Self::key_at(&inner, next) < key => current = Some(next),
                other => {
                    if level == 0 {
                        return other.and_then(|next| {
                            if Self::key_at(&inner, next) == key {
                                Some(inner.nodes[next].value.clone())
                            } else {
                                None
                            }
                        });
                    }
                    level -= 1;
                }
            }
        }
    }

    fn first_index_ge(&self, key: Option<&K>) -> Option<usize> {
        let inner = self.inner.read().unwrap();
        let mut level = inner.height - 1;
        let mut current: Option<usize> = None;
        loop {
            let next = match current {
                None => inner.head[level],
                Some(node) => inner.nodes[node].next[level],
            };
            let advance = match next {
                Some(next) => match key {
                    Some(key) => Self::key_at(&inner, next) < key,
                    None => true,
                },
                None => false,
            };
            if advance {
                current = next;
            } else if level == 0 {
                return next;
            } else {
                level -= 1;
            }
        }
    }

    /// `SkipList::Begin`: the first node.
    pub fn begin(&self) -> SkipListIterator<K, V> {
        let first = {
            let inner = self.inner.read().unwrap();
            inner.head[0]
        };
        SkipListIterator {
            inner: Arc::clone(&self.inner),
            current: first,
        }
    }

    /// `SkipList::Begin(key)`: the first node whose key is `>= key`.
    pub fn begin_at(&self, key: &K) -> SkipListIterator<K, V> {
        let first = self.first_index_ge(Some(key));
        SkipListIterator {
            inner: Arc::clone(&self.inner),
            current: first,
        }
    }

    /// `SkipList::End`.
    pub fn end(&self) -> SkipListIterator<K, V> {
        SkipListIterator {
            inner: Arc::clone(&self.inner),
            current: None,
        }
    }
}

/// `SkipList::Iterator`: a cursor over the level-0 chain.
pub struct SkipListIterator<K, V> {
    inner: Arc<RwLock<Inner<K, V>>>,
    current: Option<usize>,
}

impl<K, V> SkipListIterator<K, V> {
    pub fn is_valid(&self) -> bool {
        self.current.is_some()
    }
}

impl<K: Clone, V: Clone> SkipListIterator<K, V> {
    pub fn key(&self) -> Option<K> {
        let inner = self.inner.read().unwrap();
        self.current.map(|node| inner.nodes[node].key.clone())
    }

    pub fn value(&self) -> Option<V> {
        let inner = self.inner.read().unwrap();
        self.current.map(|node| inner.nodes[node].value.clone())
    }

    pub fn next(&mut self) {
        if let Some(node) = self.current {
            let inner = self.inner.read().unwrap();
            self.current = inner.nodes[node].next[0];
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashMap;
    use std::sync::Arc;
    use std::thread;

    const CHARS: &[u8] = b"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    struct Rng(u64);

    impl Rng {
        fn new(seed: u64) -> Self {
            Self(seed)
        }

        fn next(&mut self) -> u32 {
            self.0 = self
                .0
                .wrapping_mul(6364136223846793005)
                .wrapping_add(1442695040888963407);
            (self.0 >> 33) as u32
        }

        fn rand_str(&mut self) -> String {
            let len = self.next() % 100 + 50;
            (0..len)
                .map(|_| CHARS[self.next() as usize % CHARS.len()] as char)
                .collect()
        }
    }

    #[test]
    fn skiplist_test1() {
        let list = SkipList::new(NumComparator);
        const N: u32 = 50_000;
        const R: u32 = 50_000;
        let mut rng = Rng::new(1);
        let mut keys: HashMap<u32, u32> = HashMap::new();

        let a = 512u32;
        let b = 111u32;
        list.insert(a, b);
        assert_eq!(list.search(&a), Some(b));

        for _ in 0..N {
            let key = rng.next() % R;
            let value = rng.next() % R;
            if let std::collections::hash_map::Entry::Vacant(entry) = keys.entry(key) {
                entry.insert(value);
                list.insert(key, value);
                assert_eq!(list.search(&key), Some(value));
            }
        }
        for (key, value) in &keys {
            assert_eq!(list.search(key), Some(*value));
        }
        for (key, value) in &keys {
            let iter = list.begin_at(key);
            assert!(iter.is_valid());
            assert_eq!(iter.value(), Some(*value));
        }
    }

    #[test]
    fn skiplist_test2() {
        let list = SkipList::new(KeyComparator);
        const N: usize = 200;
        let mut rng = Rng::new(2);
        let mut keys: HashMap<String, String> = HashMap::new();

        for i in 0..N {
            let key = format!("{}{}", rng.rand_str(), i);
            if let std::collections::hash_map::Entry::Vacant(entry) = keys.entry(key.clone()) {
                entry.insert(key.clone());
                list.insert(key.clone(), key.clone());
                assert_eq!(list.search(&key), Some(key.clone()));
            }
        }
        for (key, value) in &keys {
            assert_eq!(list.search(key), Some(value.clone()));
        }
        for (key, value) in &keys {
            let iter = list.begin_at(key);
            assert!(iter.is_valid());
            assert_eq!(iter.value(), Some(value.clone()));
        }
    }

    #[test]
    fn skiplist_test3() {
        const NUM_THREADS: usize = 4;
        const NUM_OPERATIONS: usize = 1000;
        let list = Arc::new(SkipList::new(KeyComparator));
        let mut handles = Vec::new();
        for thread_id in 0..NUM_THREADS {
            let list = Arc::clone(&list);
            handles.push(thread::spawn(move || {
                let mut rng = Rng::new(thread_id as u64 + 1);
                for _ in 0..NUM_OPERATIONS {
                    let key = rng.rand_str();
                    if thread_id == 0 {
                        list.insert(key.clone(), key.clone());
                        assert_eq!(list.search(&key), Some(key.clone()));
                    } else if let Some(result) = list.search(&key) {
                        assert_eq!(result, key);
                    }
                }
            }));
        }
        for handle in handles {
            handle.join().unwrap();
        }
    }
}
