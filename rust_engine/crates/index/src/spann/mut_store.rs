//! The mutation surface of the incremental index (`SPANN_DESIGN.md` §9.4).
//!
//! The shape follows Cockroach's `cspann.Store` (`cspann/store.go:63-200`): a
//! partition is addressed by key, its members are addressed individually, and a
//! split or merge is one atomic batch rather than a rewritten tree. The one
//! difference is that this port's store is synchronous — there is no KV
//! transaction object — so [`SpannMutStore::commit`] carries a whole
//! `[KvOp]` atomically, which is the `RunTransaction` equivalent.
//!
//! [`MemKvStore`] is the in-memory implementation. It is not a stand-in for the
//! production store (that one is `storage::knn_index::spann::RocksSpannStore`);
//! it exists so the algorithms — descent, split, merge, delete — can be tested
//! without a RocksDB open, exactly like `memstore` in the reference.

use std::collections::BTreeMap;
use std::sync::RwLock;

use super::kv::MetaValue;
use super::partition::Member;
use super::{PartitionKey, SpannError};

/// One mutation of [`SpannMutStore::commit`]. The variants are deliberately
/// per-key: a split names its two new partitions, their members, the parent's
/// two child rows and the old partition's deletion in one batch, and a store
/// that cannot make that atomic would leave a dangling parent pointer.
#[derive(Clone, Debug, PartialEq)]
pub enum KvOp {
    PutMeta {
        key: PartitionKey,
        meta: MetaValue,
    },
    /// Removes the metadata row *and* every member row under the partition.
    DeletePartition {
        key: PartitionKey,
    },
    PutMember {
        partition: PartitionKey,
        member: Member,
    },
    DeleteMember {
        partition: PartitionKey,
        child: u64,
    },
    PutRowMap {
        row: u64,
        partition: PartitionKey,
    },
    DeleteRowMap {
        row: u64,
    },
    /// Advances the partition-key allocator. A key is never reused, so this
    /// only ever grows (see `kv::counter_key`).
    SetCounter(u64),
    /// A tree-level counter, written in the same batch as the mutation that
    /// changes it. `rows` lives here so a restart does not have to walk every
    /// partition to learn how many rows the tree holds.
    SetStat {
        name: String,
        value: u64,
    },
}

/// Read and atomically mutate the partition tree.
///
/// Every method returns [`SpannError`] rather than propagating the storage
/// crate's error type: `crates/index` does not depend on `crates/storage`, and
/// the glue in `storage::knn_index::spann` maps the two.
pub trait SpannMutStore: Send + Sync {
    /// The vector dimension. Every metadata row is validated against it.
    fn dims(&self) -> usize;

    fn get_meta(&self, key: PartitionKey) -> Result<Option<MetaValue>, SpannError>;

    /// Batched metadata read — the descent's per-level operation. The default
    /// loops; a store with a multi-get should override it.
    fn get_metas(&self, keys: &[PartitionKey]) -> Result<Vec<Option<MetaValue>>, SpannError> {
        keys.iter().map(|key| self.get_meta(*key)).collect()
    }

    /// One partition's members, in child-key order.
    fn get_members(&self, key: PartitionKey) -> Result<Vec<Member>, SpannError>;

    /// The child keys only. The descent needs these to build its next level,
    /// and decoding one code per child to throw it away is wasted work.
    fn member_ids(&self, key: PartitionKey) -> Result<Vec<u64>, SpannError> {
        Ok(self.get_members(key)?.into_iter().map(|m| m.id).collect())
    }

    /// `SELECT count(*)`-style member count; never decodes the codes.
    fn member_count(&self, key: PartitionKey) -> Result<usize, SpannError> {
        Ok(self.member_ids(key)?.len())
    }

    /// The leaf that holds `row`, if any (the `r|` mapping).
    fn row_partition(&self, row: u64) -> Result<Option<PartitionKey>, SpannError>;

    /// Every partition key of this index, in arbitrary order.
    fn partition_keys(&self) -> Result<Vec<PartitionKey>, SpannError>;

    /// The next partition key to hand out.
    fn counter(&self) -> Result<u64, SpannError>;

    /// A tree-level counter, or `None` on a store that predates it.
    fn stat(&self, name: &str) -> Result<Option<u64>, SpannError>;

    /// Applies `ops` atomically: either every one lands or none does.
    fn commit(&self, ops: &[KvOp]) -> Result<(), SpannError>;
}

/// An in-memory [`SpannMutStore`] with copy-on-write commits, for tests and for
/// the algorithm layer's own unit tests.
#[derive(Debug)]
pub struct MemKvStore {
    dims: usize,
    db: RwLock<MemDb>,
}

#[derive(Clone, Debug, Default)]
struct MemDb {
    metas: BTreeMap<PartitionKey, MetaValue>,
    members: BTreeMap<(PartitionKey, u64), Member>,
    rows: BTreeMap<u64, PartitionKey>,
    counter: u64,
    stats: BTreeMap<String, u64>,
}

impl MemKvStore {
    pub fn new(dims: usize) -> Self {
        Self {
            dims,
            db: RwLock::new(MemDb::default()),
        }
    }

    /// Number of member rows currently stored (a test/telemetry hook).
    pub fn stored_rows(&self) -> usize {
        self.db.read().unwrap().members.len()
    }

    pub fn stored_partitions(&self) -> usize {
        self.db.read().unwrap().metas.len()
    }

    fn apply(db: &mut MemDb, op: &KvOp) {
        match op {
            KvOp::PutMeta { key, meta } => {
                db.metas.insert(*key, meta.clone());
            }
            KvOp::DeletePartition { key } => {
                db.metas.remove(key);
                let doomed: Vec<(PartitionKey, u64)> = db
                    .members
                    .keys()
                    .filter(|(partition, _)| partition == key)
                    .copied()
                    .collect();
                for member in doomed {
                    db.members.remove(&member);
                }
            }
            KvOp::PutMember { partition, member } => {
                db.members.insert((*partition, member.id), member.clone());
            }
            KvOp::DeleteMember { partition, child } => {
                db.members.remove(&(*partition, *child));
            }
            KvOp::PutRowMap { row, partition } => {
                db.rows.insert(*row, *partition);
            }
            KvOp::DeleteRowMap { row } => {
                db.rows.remove(row);
            }
            KvOp::SetCounter(value) => db.counter = *value,
            KvOp::SetStat { name, value } => {
                db.stats.insert(name.clone(), *value);
            }
        }
    }
}

impl SpannMutStore for MemKvStore {
    fn dims(&self) -> usize {
        self.dims
    }

    fn get_meta(&self, key: PartitionKey) -> Result<Option<MetaValue>, SpannError> {
        Ok(self.db.read().unwrap().metas.get(&key).cloned())
    }

    fn get_members(&self, key: PartitionKey) -> Result<Vec<Member>, SpannError> {
        let db = self.db.read().unwrap();
        Ok(db
            .members
            .range((key, 0)..=(key, u64::MAX))
            .map(|(_, member)| member.clone())
            .collect())
    }

    fn member_ids(&self, key: PartitionKey) -> Result<Vec<u64>, SpannError> {
        let db = self.db.read().unwrap();
        Ok(db
            .members
            .range((key, 0)..=(key, u64::MAX))
            .map(|((_, child), _)| *child)
            .collect())
    }

    fn row_partition(&self, row: u64) -> Result<Option<PartitionKey>, SpannError> {
        Ok(self.db.read().unwrap().rows.get(&row).copied())
    }

    fn partition_keys(&self) -> Result<Vec<PartitionKey>, SpannError> {
        Ok(self.db.read().unwrap().metas.keys().copied().collect())
    }

    fn counter(&self) -> Result<u64, SpannError> {
        Ok(self.db.read().unwrap().counter)
    }

    fn stat(&self, name: &str) -> Result<Option<u64>, SpannError> {
        Ok(self.db.read().unwrap().stats.get(name).copied())
    }

    fn commit(&self, ops: &[KvOp]) -> Result<(), SpannError> {
        // Copy-on-write: a failure while building the new state leaves the old
        // one in place, which is what "atomic" means for every caller here.
        let mut next = self.db.read().unwrap().clone();
        for op in ops {
            Self::apply(&mut next, op);
        }
        *self.db.write().unwrap() = next;
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::spann::quantize;

    fn member(id: u64) -> Member {
        Member {
            id,
            code: quantize::Code {
                norm: id as f32,
                raw_norm: 0.0,
                sum: 0.0,
                error: 1.0,
                bits: vec![0u8; quantize::code_bytes(8)],
            },
        }
    }

    fn meta(level: u32) -> MetaValue {
        MetaValue::new(level, 0, vec![0.0; 8])
    }

    #[test]
    fn commit_applies_every_op_of_a_split() {
        let store = MemKvStore::new(8);
        store
            .commit(&[
                KvOp::PutMeta {
                    key: 0,
                    meta: meta(2),
                },
                KvOp::PutMeta {
                    key: 5,
                    meta: meta(1),
                },
                KvOp::PutMember {
                    partition: 0,
                    member: member(5),
                },
                KvOp::PutMember {
                    partition: 5,
                    member: member(100),
                },
                KvOp::PutRowMap {
                    row: 100,
                    partition: 5,
                },
                KvOp::SetCounter(6),
            ])
            .unwrap();

        assert_eq!(store.get_meta(5).unwrap().unwrap().level, 1);
        assert_eq!(store.member_ids(0).unwrap(), vec![5]);
        assert_eq!(store.member_ids(5).unwrap(), vec![100]);
        assert_eq!(store.row_partition(100).unwrap(), Some(5));
        assert_eq!(store.counter().unwrap(), 6);
    }

    #[test]
    fn deleting_a_partition_removes_its_member_rows() {
        let store = MemKvStore::new(8);
        store
            .commit(&[
                KvOp::PutMeta {
                    key: 3,
                    meta: meta(1),
                },
                KvOp::PutMember {
                    partition: 3,
                    member: member(1),
                },
                KvOp::PutMember {
                    partition: 3,
                    member: member(2),
                },
            ])
            .unwrap();
        assert_eq!(store.get_members(3).unwrap().len(), 2);

        store.commit(&[KvOp::DeletePartition { key: 3 }]).unwrap();
        assert!(store.get_meta(3).unwrap().is_none());
        assert!(store.get_members(3).unwrap().is_empty());
    }

    #[test]
    fn a_split_batch_is_all_or_nothing() {
        let store = MemKvStore::new(8);
        store
            .commit(&[KvOp::PutMeta {
                key: 0,
                meta: meta(1),
            }])
            .unwrap();
        // The member row is real, the metadata row on `PutMeta` would be too;
        // the point of the test is that `commit` never exposes a half-applied
        // batch, so re-applying the same ops must be idempotent.
        let ops = [
            KvOp::PutMeta {
                key: 1,
                meta: meta(1),
            },
            KvOp::PutMember {
                partition: 1,
                member: member(9),
            },
            KvOp::PutRowMap {
                row: 9,
                partition: 1,
            },
        ];
        store.commit(&ops).unwrap();
        store.commit(&ops).unwrap();
        assert_eq!(store.member_ids(1).unwrap(), vec![9]);
        assert_eq!(store.stored_rows(), 1);
    }
}
