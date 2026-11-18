//! The [`SpannMutStore`] over the engine's RocksDB (`MetaStore`).
//!
//! This is the store the incremental index runs on: one metadata row per
//! partition, one row per member, one row per row→partition mapping, all under
//! the `vec|` prefix (`index::spann::kv`). It is deliberately **not** a new
//! database or a new file format — the catalog, the row store and this index
//! share one RocksDB, which is what lets a split's `atomic_batch` land in the
//! same checkpoint as the chunk records it belongs with
//! (`metastore_rocks.rs:9-10`).
//!
//! Durability follows the catalog's: `keeps_wal` only turns the WAL on for the
//! persistence-manager keys, so a `vec|` write becomes durable at the
//! checkpoint's `flush()` (`metastore_rocks.rs:25-32`). A crash therefore loses
//! a whole `atomic_batch`, never half of one — a split either happened or the
//! tree is still the previous, self-consistent version.

use std::sync::Arc;

use index::spann::kv;
use index::spann::mut_store::{KvOp, SpannMutStore};
use index::spann::{Member, MetaValue, PartitionKey, SpannError};

use crate::metastore::MetaStore;

/// A spann index's partition tree in the engine's RocksDB.
pub struct RocksSpannStore {
    db: Arc<dyn MetaStore>,
    index_id: u64,
    dims: usize,
}

impl RocksSpannStore {
    pub fn new(db: Arc<dyn MetaStore>, index_id: u64, dims: usize) -> Self {
        Self { db, index_id, dims }
    }

    pub fn index_id(&self) -> u64 {
        self.index_id
    }

    fn error(context: &str, err: impl std::fmt::Display) -> SpannError {
        SpannError::Storage(format!("{context}: {err}"))
    }

    /// The `child` half of a member row's key, given that the caller scanned
    /// the partition's prefix.
    fn child_of(&self, key: &[u8], partition: PartitionKey) -> Option<u64> {
        let tail = key.len().checked_sub(8)?;
        let at = tail.checked_sub(4)?;
        if u32::from_be_bytes(key[at..tail].try_into().ok()?) != partition {
            return None;
        }
        Some(u64::from_be_bytes(key[tail..].try_into().ok()?))
    }

    /// Deletes every row of `index` — `DROP INDEX` and the rebuild half of
    /// `CREATE INDEX`.
    ///
    /// It scans the three prefix families rather than relying on a range
    /// delete: `MetaStore` has no delete-range, and a spann index's row count
    /// is the table's, so a scan-then-batch is only acceptable here (never on
    /// a query path).
    pub fn clear(&self) -> Result<usize, SpannError> {
        let mut items: Vec<(Vec<u8>, Option<Vec<u8>>)> = Vec::new();
        for prefix in [
            kv::meta_prefix(self.index_id),
            kv::member_prefix(self.index_id),
            kv::row_map_prefix(self.index_id),
        ] {
            let rows = self
                .db
                .scan_prefix(&prefix)
                .map_err(|e| Self::error("scan", e))?;
            items.extend(rows.into_iter().map(|(key, _)| (key, None)));
        }
        for key in [
            kv::config_key(self.index_id),
            kv::counter_key(self.index_id),
            kv::stat_key(self.index_id, "rows"),
        ] {
            items.push((key, None));
        }
        let removed = items.len();
        if !items.is_empty() {
            self.db
                .atomic_batch(&items)
                .map_err(|e| Self::error("clear", e))?;
        }
        Ok(removed)
    }
}

impl SpannMutStore for RocksSpannStore {
    fn dims(&self) -> usize {
        self.dims
    }

    fn get_meta(&self, key: PartitionKey) -> Result<Option<MetaValue>, SpannError> {
        let bytes = self
            .db
            .get(&kv::meta_key(self.index_id, key))
            .map_err(|e| Self::error("get metadata", e))?;
        match bytes {
            Some(bytes) => kv::decode_meta(&bytes, self.dims).map(Some),
            None => Ok(None),
        }
    }

    fn get_members(&self, key: PartitionKey) -> Result<Vec<Member>, SpannError> {
        let prefix = kv::member_partition_prefix(self.index_id, key);
        let rows = self
            .db
            .scan_prefix(&prefix)
            .map_err(|e| Self::error("scan members", e))?;
        let mut out = Vec::with_capacity(rows.len());
        for (row_key, value) in rows {
            let Some(child) = self.child_of(&row_key, key) else {
                continue;
            };
            out.push(kv::decode_member(child, &value, self.dims)?);
        }
        Ok(out)
    }

    fn member_ids(&self, key: PartitionKey) -> Result<Vec<u64>, SpannError> {
        let prefix = kv::member_partition_prefix(self.index_id, key);
        let rows = self
            .db
            .scan_prefix(&prefix)
            .map_err(|e| Self::error("scan member keys", e))?;
        Ok(rows
            .into_iter()
            .filter_map(|(row_key, _)| self.child_of(&row_key, key))
            .collect())
    }

    fn member_count(&self, key: PartitionKey) -> Result<usize, SpannError> {
        // `count_prefix` counts keys without reading values, which matters: a
        // leaf's values are its codes (208 B each at 1536d) and the count is
        // read on every flush's overflow check.
        let prefix = kv::member_partition_prefix(self.index_id, key);
        self.db
            .count_prefix(&prefix)
            .map_err(|e| Self::error("count members", e))
    }

    fn row_partition(&self, row: u64) -> Result<Option<PartitionKey>, SpannError> {
        let bytes = self
            .db
            .get(&kv::row_map_key(self.index_id, row))
            .map_err(|e| Self::error("get row map", e))?;
        match bytes {
            Some(bytes) => kv::decode_row_map(&bytes).map(Some),
            None => Ok(None),
        }
    }

    fn partition_keys(&self) -> Result<Vec<PartitionKey>, SpannError> {
        let prefix = kv::meta_prefix(self.index_id);
        let rows = self
            .db
            .scan_prefix(&prefix)
            .map_err(|e| Self::error("scan partitions", e))?;
        Ok(rows
            .into_iter()
            .filter_map(|(key, _)| {
                let tail = key.len().checked_sub(4)?;
                Some(u32::from_be_bytes(key[tail..].try_into().ok()?))
            })
            .collect())
    }

    fn counter(&self) -> Result<u64, SpannError> {
        let bytes = self
            .db
            .get(&kv::counter_key(self.index_id))
            .map_err(|e| Self::error("get counter", e))?;
        Ok(bytes
            .and_then(|bytes| String::from_utf8(bytes).ok())
            .and_then(|text| text.trim().parse::<u64>().ok())
            .unwrap_or(0))
    }

    fn stat(&self, name: &str) -> Result<Option<u64>, SpannError> {
        let bytes = self
            .db
            .get(&kv::stat_key(self.index_id, name))
            .map_err(|e| Self::error("get stat", e))?;
        Ok(bytes
            .and_then(|bytes| String::from_utf8(bytes).ok())
            .and_then(|text| text.trim().parse::<u64>().ok()))
    }

    fn commit(&self, ops: &[KvOp]) -> Result<(), SpannError> {
        let mut items: Vec<(Vec<u8>, Option<Vec<u8>>)> = Vec::with_capacity(ops.len() * 2);
        for op in ops {
            match op {
                KvOp::PutMeta { key, meta } => {
                    items.push((
                        kv::meta_key(self.index_id, *key),
                        Some(kv::encode_meta(meta)),
                    ));
                }
                KvOp::DeletePartition { key } => {
                    // The member rows are named by the scan that runs *before*
                    // the batch: `atomic_batch` cannot express a range delete,
                    // and deleting the metadata row alone would orphan the
                    // members (a later partition reusing the key would read
                    // them as its own).
                    let prefix = kv::member_partition_prefix(self.index_id, *key);
                    let members = self
                        .db
                        .scan_prefix(&prefix)
                        .map_err(|e| Self::error("scan for delete", e))?;
                    items.push((kv::meta_key(self.index_id, *key), None));
                    items.extend(members.into_iter().map(|(key, _)| (key, None)));
                }
                KvOp::PutMember { partition, member } => {
                    items.push((
                        kv::member_key(self.index_id, *partition, member.id),
                        Some(kv::encode_member(member)),
                    ));
                }
                KvOp::DeleteMember { partition, child } => {
                    items.push((kv::member_key(self.index_id, *partition, *child), None));
                }
                KvOp::PutRowMap { row, partition } => {
                    items.push((
                        kv::row_map_key(self.index_id, *row),
                        Some(kv::encode_row_map(*partition).to_vec()),
                    ));
                }
                KvOp::DeleteRowMap { row } => {
                    items.push((kv::row_map_key(self.index_id, *row), None));
                }
                KvOp::SetCounter(value) => {
                    items.push((
                        kv::counter_key(self.index_id),
                        Some(value.to_string().into_bytes()),
                    ));
                }
                KvOp::SetStat { name, value } => {
                    items.push((
                        kv::stat_key(self.index_id, name),
                        Some(value.to_string().into_bytes()),
                    ));
                }
            }
        }
        if items.is_empty() {
            return Ok(());
        }
        self.db
            .atomic_batch(&items)
            .map_err(|e| Self::error("commit", e))
    }
}

#[cfg(test)]
mod tests {
    use index::spann::mut_store::KvOp;
    use index::spann::{IncrementalIndex, MetaValue, Options};

    use super::*;
    use crate::metastore_rocks::RocksMetaStore;

    fn options() -> Options {
        Options {
            max_partition_size: 16,
            min_partition_size: 4,
            ..Options::default()
        }
    }

    fn open(dir: &std::path::Path) -> (Arc<RocksMetaStore>, IncrementalIndex<RocksSpannStore>) {
        let db = Arc::new(RocksMetaStore::open(dir).unwrap());
        let store = RocksSpannStore::new(db.clone(), 7, 8);
        let index = IncrementalIndex::open(store, options()).unwrap();
        (db, index)
    }

    #[test]
    fn the_tree_survives_a_reopen_without_a_rebuild() {
        let dir = tempfile::tempdir().unwrap();
        let data: Vec<(u64, Vec<f32>)> = (0..200)
            .map(|i| (i as u64, (0..8).map(|d| i as f32 + d as f32).collect()))
            .collect();
        let fetch = |row: u64| data.get(row as usize).map(|(_, v)| v.clone());
        {
            let (_db, index) = open(dir.path());
            let batch: Vec<(u64, &[f32])> =
                data.iter().map(|(id, v)| (*id, v.as_slice())).collect();
            index.insert(&batch, &fetch).unwrap();
            index.maintain(&fetch, usize::MAX).unwrap();
            assert_eq!(index.stats().unwrap().rows, 200);
        }
        // Re-opening reads the root's metadata and nothing else; the partitions
        // are read on demand. No vector is re-read and no k-means runs.
        let (_db, index) = open(dir.path());
        assert_eq!(index.stats().unwrap().rows, 200);
        assert!(index.height().unwrap() >= 2);
        assert_eq!(index.row_ids().unwrap().len(), 200);
    }

    #[test]
    fn delete_removes_both_the_member_and_the_mapping() {
        let dir = tempfile::tempdir().unwrap();
        let (db, index) = open(dir.path());
        let vectors = [(1u64, vec![1.0f32; 8]), (2, vec![2.0; 8])];
        let batch: Vec<(u64, &[f32])> = vectors.iter().map(|(id, v)| (*id, v.as_slice())).collect();
        let fetch = |row: u64| {
            vectors
                .iter()
                .find(|(id, _)| *id == row)
                .map(|(_, v)| v.clone())
        };
        index.insert(&batch, &fetch).unwrap();
        assert!(index.delete(1).unwrap());

        let store = RocksSpannStore::new(db.clone(), 7, 8);
        assert_eq!(store.row_partition(1).unwrap(), None);
        assert_eq!(store.row_partition(2).unwrap(), Some(0));
        assert_eq!(store.member_count(0).unwrap(), 1);
    }

    #[test]
    fn clear_removes_every_row_of_the_index() {
        let dir = tempfile::tempdir().unwrap();
        let (db, index) = open(dir.path());
        let batch = [(5u64, vec![0.5f32; 8])];
        let refs: Vec<(u64, &[f32])> = batch.iter().map(|(id, v)| (*id, v.as_slice())).collect();
        let fetch = |row: u64| {
            batch
                .iter()
                .find(|(id, _)| *id == row)
                .map(|(_, v)| v.clone())
        };
        index.insert(&refs, &fetch).unwrap();

        let store = RocksSpannStore::new(db.clone(), 7, 8);
        let removed = store.clear().unwrap();
        assert!(removed >= 3, "metadata + member + row map: {removed}");
        assert!(store.partition_keys().unwrap().is_empty());
        assert!(store.get_meta(0).unwrap().is_none());
    }

    #[test]
    fn a_commit_is_visible_only_after_it_returns() {
        let dir = tempfile::tempdir().unwrap();
        let (db, _index) = open(dir.path());
        let store = RocksSpannStore::new(db, 7, 8);
        let meta = MetaValue::new(1, 0, vec![0.0; 8]);
        store.commit(&[KvOp::PutMeta { key: 3, meta }]).unwrap();
        assert!(store.get_meta(3).unwrap().is_some());
        store.commit(&[KvOp::DeletePartition { key: 3 }]).unwrap();
        assert!(store.get_meta(3).unwrap().is_none());
    }
}
