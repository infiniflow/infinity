//! Query result cache with LRU eviction and column projection.
//!
//! C++: `CacheContent` / `CacheOutput` / `CacheResultMap` / `ResultCacheManager`
//! (`src/storage/result_cache_manager.cppm`, `result_cache_manager_impl.cpp`).

use std::sync::Arc;

use crate::data_block::DataBlock;

/// C++: `CachedNodeBase` — the cache key. The port keeps the three pieces the
/// cache needs: a hash, structural equality, and the output column names.
pub trait CachedNode {
    /// Downcasting hook so `eq_node` can compare the concrete node type, as the
    /// reference's `static_cast` in `MockCachedNode::Eq` does.
    fn as_any(&self) -> &dyn std::any::Any;
    fn hash(&self) -> u64;
    fn eq_node(&self, other: &dyn CachedNode) -> bool;
    fn output_names(&self) -> Vec<String>;
    /// `LogicalNodeType` name; `DropTable` uses it to reach scan nodes.
    fn node_type(&self) -> &'static str;
    fn schema_name(&self) -> &str {
        ""
    }
    fn table_name(&self) -> &str {
        ""
    }
}

/// C++: `CacheContent`.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct CacheContent {
    pub data_blocks: Vec<DataBlock>,
    pub column_names: Vec<String>,
}

impl CacheContent {
    pub fn new(data_blocks: Vec<DataBlock>, column_names: Vec<String>) -> Self {
        Self {
            data_blocks,
            column_names,
        }
    }

    /// C++: `CacheContent::AppendColumns` — `None` when the blocks do not line
    /// up, in which case the caller replaces the entry wholesale.
    pub fn append_columns(
        &self,
        other: &CacheContent,
        column_idxes: &[usize],
    ) -> Option<CacheContent> {
        if self.data_blocks.len() != other.data_blocks.len() {
            return None;
        }
        let mut blocks = self.data_blocks.clone();
        for (old, new) in blocks.iter_mut().zip(&other.data_blocks) {
            if !old.append_columns(new, column_idxes) {
                return None;
            }
        }
        let mut names = self.column_names.clone();
        for &idx in column_idxes {
            names.push(other.column_names.get(idx)?.clone());
        }
        Some(CacheContent::new(blocks, names))
    }

    /// C++: `CacheContent::Clone`.
    pub fn clone_content(&self) -> CacheContent {
        self.clone()
    }
}

/// C++: `CacheOutput` — the cached content plus the projection to read from it.
#[derive(Debug, Clone)]
pub struct CacheOutput {
    pub cache_content: Arc<CacheContent>,
    pub column_map: Vec<usize>,
}

struct Entry {
    node: Box<dyn CachedNode>,
    content: Arc<CacheContent>,
}

/// C++: `CacheResultMap` — a recency-ordered list (the reference's LRU list)
/// with the entry count bounded by `cache_num_capacity`.
pub struct CacheResultMap {
    entries: Vec<Entry>,
    cache_num_capacity: usize,
}

impl CacheResultMap {
    pub fn new(cache_num_capacity: usize) -> Self {
        Self {
            entries: Vec::new(),
            cache_num_capacity,
        }
    }

    fn position(&self, node: &dyn CachedNode) -> Option<usize> {
        self.entries
            .iter()
            .position(|e| e.node.hash() == node.hash() && e.node.eq_node(node))
    }

    fn move_to_front(&mut self, index: usize) {
        if index != 0 {
            let entry = self.entries.remove(index);
            self.entries.insert(0, entry);
        }
    }

    /// C++: `CacheResultMap::AddCache`. Returns `false` when the node was
    /// already cached (its content is updated instead).
    pub fn add_cache(&mut self, node: Box<dyn CachedNode>, data_blocks: Vec<DataBlock>) -> bool {
        if let Some(index) = self.position(node.as_ref()) {
            let new_content = CacheContent::new(data_blocks, node.output_names());
            let old = &self.entries[index].content;
            let mut add_columns = Vec::new();
            for (i, name) in new_content.column_names.iter().enumerate() {
                if !old.column_names.contains(name) {
                    add_columns.push(i);
                }
            }
            if !add_columns.is_empty() {
                let updated = old
                    .append_columns(&new_content, &add_columns)
                    .unwrap_or(new_content);
                self.entries[index].content = Arc::new(updated);
            }
            self.move_to_front(index);
            return false;
        }
        if self.entries.len() >= self.cache_num_capacity {
            self.entries.pop();
        }
        let content = Arc::new(CacheContent::new(data_blocks, node.output_names()));
        self.entries.insert(0, Entry { node, content });
        true
    }

    /// C++: `CacheResultMap::GetCache` — also promotes the entry.
    pub fn get_cache(&mut self, node: &dyn CachedNode) -> Option<Arc<CacheContent>> {
        let index = self.position(node)?;
        self.move_to_front(index);
        Some(self.entries[0].content.clone())
    }

    /// C++: `CacheResultMap::DropIF`.
    pub fn drop_if(&mut self, pred: impl Fn(&dyn CachedNode) -> bool) -> usize {
        let before = self.entries.len();
        self.entries.retain(|e| !pred(e.node.as_ref()));
        before - self.entries.len()
    }

    /// C++: `CacheResultMap::ResetCacheNumCapacity`.
    pub fn reset_cache_num_capacity(&mut self, capacity: usize) {
        self.cache_num_capacity = capacity;
        while self.entries.len() > capacity {
            self.entries.pop();
        }
    }

    /// C++: `CacheResultMap::ClearCache`.
    pub fn clear_cache(&mut self) {
        self.entries.clear();
    }

    pub fn cache_num_capacity(&self) -> usize {
        self.cache_num_capacity
    }

    pub fn cache_num_used(&self) -> usize {
        self.entries.len()
    }
}

/// C++: `ResultCacheManager`.
pub struct ResultCacheManager {
    cache_map: CacheResultMap,
}

impl ResultCacheManager {
    pub fn new(cache_num_capacity: usize) -> Self {
        Self {
            cache_map: CacheResultMap::new(cache_num_capacity),
        }
    }

    /// C++: `ResultCacheManager::AddCache`.
    pub fn add_cache(&mut self, node: Box<dyn CachedNode>, data_blocks: Vec<DataBlock>) -> bool {
        self.cache_map.add_cache(node, data_blocks)
    }

    /// C++: `ResultCacheManager::GetCache` — projects the cached columns onto
    /// the node's output names; a missing column is a miss.
    pub fn get_cache(&mut self, node: &dyn CachedNode) -> Option<CacheOutput> {
        let content = self.cache_map.get_cache(node)?;
        let mut column_map = Vec::new();
        for name in node.output_names() {
            let index = content.column_names.iter().position(|n| *n == name)?;
            column_map.push(index);
        }
        Some(CacheOutput {
            cache_content: content,
            column_map,
        })
    }

    /// C++: `ResultCacheManager::DropTable`.
    pub fn drop_table(&mut self, schema_name: &str, table_name: &str) -> usize {
        self.cache_map.drop_if(|node| {
            node.node_type() == "Match"
                && node.schema_name() == schema_name
                && node.table_name() == table_name
        })
    }

    pub fn reset_cache_num_capacity(&mut self, capacity: usize) {
        self.cache_map.reset_cache_num_capacity(capacity);
    }

    pub fn clear_cache(&mut self) {
        self.cache_map.clear_cache();
    }

    pub fn cache_num_capacity(&self) -> usize {
        self.cache_map.cache_num_capacity()
    }

    pub fn cache_num_used(&self) -> usize {
        self.cache_map.cache_num_used()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// C++: `MockCachedNode` (`result_cache_manager_ut.cpp`): the key is the
    /// string plus the output names.
    struct MockNode {
        key: String,
        names: Vec<String>,
    }

    impl MockNode {
        fn new(key: &str, names: &[&str]) -> Box<dyn CachedNode> {
            Box::new(Self {
                key: key.to_string(),
                names: names.iter().map(|n| n.to_string()).collect(),
            })
        }
    }

    impl CachedNode for MockNode {
        fn as_any(&self) -> &dyn std::any::Any {
            self
        }

        fn hash(&self) -> u64 {
            let mut h = self.key.len() as u64;
            for b in self.key.bytes() {
                h = h.wrapping_mul(131).wrapping_add(b as u64);
            }
            h
        }

        fn eq_node(&self, other: &dyn CachedNode) -> bool {
            match other.as_any().downcast_ref::<Self>() {
                Some(other) => other.key == self.key,
                None => false,
            }
        }

        fn output_names(&self) -> Vec<String> {
            self.names.clone()
        }

        fn node_type(&self) -> &'static str {
            "Mock"
        }
    }

    fn block(n: usize) -> DataBlock {
        let columns: Vec<crate::data_block::Column> = (0..n)
            .map(|_| crate::data_block::Column::Int(Default::default()))
            .collect();
        DataBlock::new(columns, 1)
    }

    /// `src/unit_test/storage/result_cache_manager_ut.cpp::test1`.
    #[test]
    fn cache_projects_columns_and_merges_new_ones() {
        let mut cache = ResultCacheManager::new(100);

        assert!(cache
            .get_cache(MockNode::new("key1", &["col1", "col2"]).as_ref())
            .is_none());

        assert!(cache.add_cache(MockNode::new("key1", &["col1", "col2"]), vec![block(2)]));
        let hit = cache
            .get_cache(MockNode::new("key1", &["col1", "col2"]).as_ref())
            .expect("cached");
        assert_eq!(hit.column_map, vec![0, 1]);

        let hit = cache
            .get_cache(MockNode::new("key1", &["col2"]).as_ref())
            .expect("projection hit");
        assert_eq!(hit.column_map, vec![1]);

        assert!(cache
            .get_cache(MockNode::new("key1", &["col1", "col2", "col3"]).as_ref())
            .is_none());
        assert!(!cache.add_cache(
            MockNode::new("key1", &["col1", "col2", "col3"]),
            vec![block(3)]
        ));
        let hit = cache
            .get_cache(MockNode::new("key1", &["col3"]).as_ref())
            .expect("merged column");
        assert_eq!(hit.column_map, vec![2]);
    }

    /// `result_cache_manager_ut.cpp::test2` — LRU eviction at capacity.
    #[test]
    fn cache_evicts_the_least_recently_used_entry() {
        let mut cache = ResultCacheManager::new(2);
        assert!(cache.add_cache(MockNode::new("key1", &["col1"]), vec![]));
        assert!(cache.add_cache(MockNode::new("key2", &["col2"]), vec![]));
        assert!(cache
            .get_cache(MockNode::new("key1", &["col1"]).as_ref())
            .is_some());
        assert!(cache.add_cache(MockNode::new("key3", &["col1", "col2", "col3"]), vec![]));
        assert!(cache
            .get_cache(MockNode::new("key2", &["col2"]).as_ref())
            .is_none());
    }

    #[test]
    fn drop_table_removes_only_matching_scan_nodes() {
        struct ScanNode;
        impl CachedNode for ScanNode {
            fn as_any(&self) -> &dyn std::any::Any {
                self
            }

            fn hash(&self) -> u64 {
                7
            }
            fn eq_node(&self, other: &dyn CachedNode) -> bool {
                other.node_type() == "Match"
            }
            fn output_names(&self) -> Vec<String> {
                vec!["col".to_string()]
            }
            fn node_type(&self) -> &'static str {
                "Match"
            }
            fn schema_name(&self) -> &str {
                "default_db"
            }
            fn table_name(&self) -> &str {
                "t"
            }
        }
        let mut cache = ResultCacheManager::new(4);
        assert!(cache.add_cache(Box::new(ScanNode), vec![]));
        assert_eq!(cache.drop_table("default_db", "t"), 1);
        assert_eq!(cache.cache_num_used(), 0);
    }
}
