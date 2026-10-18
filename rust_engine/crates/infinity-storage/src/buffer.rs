//! A bounded LRU cache of decoded column blocks — the counterpart of the
//! reference's `BufferManager` (`src/storage/buffer/buffer_manager*.cppm`).
//!
//! Reads go through [`BufferManager::get`]; a miss loads the block from disk
//! and [`BufferManager::put`] evicts the least-recently-used blocks until the
//! configured byte budget holds. The executor wires `[buffer]
//! buffer_manager_size` to it, so RSS is bounded by the budget instead of the
//! total size of the tables read (Gate C/E).

use std::collections::HashMap;
use std::sync::Arc;

use crate::cpp_format::{BlockId, RawBlockColumn};

/// Bounded LRU cache keyed by [`BlockId`].
pub struct BufferManager<V> {
    budget: usize,
    used: usize,
    tick: u64,
    entries: HashMap<BlockId, (Arc<V>, usize, u64)>,
}

impl<V> BufferManager<V> {
    pub fn new(budget: usize) -> Self {
        Self {
            budget,
            used: 0,
            tick: 0,
            entries: HashMap::new(),
        }
    }

    pub fn set_budget(&mut self, budget: usize) {
        self.budget = budget;
        self.evict();
    }

    pub fn budget(&self) -> usize {
        self.budget
    }

    pub fn used_bytes(&self) -> usize {
        self.used
    }

    /// Looks a block up and marks it most-recently used.
    pub fn get(&mut self, key: &BlockId) -> Option<Arc<V>> {
        self.tick += 1;
        let tick = self.tick;
        let entry = self.entries.get_mut(key)?;
        entry.2 = tick;
        Some(entry.0.clone())
    }

    /// Inserts a block, evicting LRU entries until it fits.
    pub fn put(&mut self, key: BlockId, value: Arc<V>, bytes: usize) {
        self.tick += 1;
        if let Some((_, old, _)) = self.entries.remove(&key) {
            self.used = self.used.saturating_sub(old);
        }
        let bytes = bytes.min(self.budget);
        self.entries.insert(key, (value, bytes, self.tick));
        self.used += bytes;
        self.evict();
    }

    pub fn invalidate(&mut self, key: &BlockId) {
        if let Some((_, bytes, _)) = self.entries.remove(key) {
            self.used = self.used.saturating_sub(bytes);
        }
    }

    pub fn clear(&mut self) {
        self.entries.clear();
        self.used = 0;
    }

    fn evict(&mut self) {
        while self.used > self.budget && !self.entries.is_empty() {
            let Some(key) = self
                .entries
                .iter()
                .min_by_key(|(_, (_, _, last))| *last)
                .map(|(k, _)| *k)
            else {
                break;
            };
            self.invalidate(&key);
        }
    }
}

/// Byte size of a decoded block, for the budget.
pub fn block_bytes(columns: &[RawBlockColumn]) -> usize {
    columns.iter().map(|c| c.body.len() + c.outline.len()).sum()
}

#[cfg(test)]
mod tests {
    use super::*;

    fn key(block: u16) -> BlockId {
        BlockId {
            db_id: 1,
            table_id: 2,
            segment_id: 0,
            block_id: block,
        }
    }

    #[test]
    fn evicts_least_recently_used() {
        let mut cache: BufferManager<Vec<u8>> = BufferManager::new(100);
        cache.put(key(0), Arc::new(vec![0]), 40);
        cache.put(key(1), Arc::new(vec![1]), 40);
        assert!(cache.get(&key(0)).is_some());
        // Adding a third 40-byte block exceeds 100; block 1 is the LRU.
        cache.put(key(2), Arc::new(vec![2]), 40);
        assert_eq!(cache.used_bytes(), 80);
        assert!(cache.get(&key(0)).is_some());
        assert!(cache.get(&key(1)).is_none());
        assert!(cache.get(&key(2)).is_some());
    }

    #[test]
    fn shrinking_the_budget_evicts() {
        let mut cache: BufferManager<Vec<u8>> = BufferManager::new(100);
        for b in 0..4 {
            cache.put(key(b), Arc::new(vec![b as u8]), 25);
        }
        assert_eq!(cache.used_bytes(), 100);
        cache.set_budget(50);
        assert_eq!(cache.used_bytes(), 50);
    }
}
