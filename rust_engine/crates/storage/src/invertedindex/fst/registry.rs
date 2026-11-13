//! The FST builder's fixed-capacity hash registry.
//!
//! Port of `src/storage/invertedindex/fst/registry.cppm`: a direct-mapped
//! table used while compiling an FST to share identical suffix nodes. Slots
//! collide destructively, so inserting into an occupied slot overwrites it.

/// `Key::Hash()`: the application supplies the hash.
pub trait HashKey {
    fn hash_key(&self) -> u64;
}

/// `RegistryEntry<Val>`: either the value found in a slot or the slot a new
/// key should be inserted into.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum RegistryEntry<V> {
    Found(V),
    Vacant(usize),
}

/// `Registry<Key, Val>` (`fst/registry.cppm:41-68`).
#[derive(Debug, Clone)]
pub struct FstRegistry<K, V> {
    cells: Vec<(Option<K>, V)>,
    mask: usize,
}

impl<K, V> FstRegistry<K, V>
where
    K: PartialEq + HashKey + Clone,
    V: Copy + Default,
{
    /// `Registry(table_size_shift)`: `2^shift` slots.
    pub fn new(table_size_shift: u32) -> Self {
        let n = 1usize << table_size_shift;
        Self {
            cells: vec![(None, V::default()); n],
            mask: n - 1,
        }
    }

    /// `Registry::Find`: the value when the slot holds `key`, else the slot.
    pub fn find(&self, key: &K) -> RegistryEntry<V> {
        let slot = (key.hash_key() as usize) & self.mask;
        match &self.cells[slot].0 {
            Some(existing) if existing == key => RegistryEntry::Found(self.cells[slot].1),
            _ => RegistryEntry::Vacant(slot),
        }
    }

    /// `Registry::Insert`: overwrite `slot`, assuming `key.hash_key() == slot`.
    pub fn insert(&mut self, slot: usize, key: K, value: V) {
        self.cells[slot] = (Some(key), value);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[derive(Debug, Clone, PartialEq, Eq)]
    struct Key {
        k: i32,
    }

    impl HashKey for Key {
        fn hash_key(&self) -> u64 {
            self.k as u64
        }
    }

    fn key(k: i32) -> Key {
        Key { k }
    }

    /// C++: `RegistryTest.InsertAndFind` (`registry_ut.cpp`).
    #[test]
    fn insert_and_find() {
        let mut registry: FstRegistry<Key, usize> = FstRegistry::new(3);

        // Slot 1 starts empty.
        assert_eq!(registry.find(&key(1)), RegistryEntry::Vacant(1));
        registry.insert(1, key(1), 100);
        assert_eq!(registry.find(&key(1)), RegistryEntry::Found(100));

        assert_eq!(registry.find(&key(2)), RegistryEntry::Vacant(2));
        registry.insert(2, key(2), 200);
        assert_eq!(registry.find(&key(2)), RegistryEntry::Found(200));

        assert_eq!(registry.find(&key(3)), RegistryEntry::Vacant(3));
        registry.insert(3, key(3), 300);
        assert_eq!(registry.find(&key(3)), RegistryEntry::Found(300));

        // 8 & 7 == 0.
        assert_eq!(registry.find(&key(8)), RegistryEntry::Vacant(0));
        registry.insert(0, key(8), 800);
        assert_eq!(registry.find(&key(8)), RegistryEntry::Found(800));

        // 16 collides with 8 and overwrites it.
        assert_eq!(registry.find(&key(16)), RegistryEntry::Vacant(0));
        registry.insert(0, key(16), 1600);
        assert_eq!(registry.find(&key(16)), RegistryEntry::Found(1600));

        // The previous occupant of slot 0 is gone.
        assert_eq!(registry.find(&key(8)), RegistryEntry::Vacant(0));
    }
}
