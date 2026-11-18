//! Keys and values of the incremental store (`SPANN_DESIGN.md` §9.4's
//! `index_delta_mode = append`, replacing the v2 chunk's whole-segment rebuild).
//!
//! The layout mirrors Cockroach's `vecencoding` (one metadata row per partition,
//! its members as rows under the same prefix, `vecencoding/encoding.go`):
//!
//! ```text
//! vec|<ver>|<index u64 BE>|d|<partition u32 BE>              partition metadata
//! vec|<ver>|<index u64 BE>|m|<partition u32 BE>|<child u64 BE>  one member
//! vec|<ver>|<index u64 BE>|r|<row u64 BE>                    row -> partition
//! vec|<ver>|<index u64 BE>|c                                 options / counter
//! ```
//!
//! Every integer is big-endian so the store's byte order is numeric order: a
//! prefix scan returns a partition's members in child-key order, and the
//! counter's range is one contiguous run.
//!
//! **The metadata does not hold children.** A branch's children are its member
//! rows (`Member::id` is the child partition key), exactly like
//! `cspann.PartitionMetadata` — it stores `Level`, `Centroid`, `StateDetails`
//! and nothing else (`partition_metadata.go:321-331`). `parent` is the one field
//! this port adds: the reference recovers it by searching the level above
//! during a split fixup (`fixup_split.go`), but the split path here is
//! synchronous and already knows the key, so carrying 4 bytes removes a whole
//! tree descent from every split.

use super::partition::Member;
use super::{quantize, PartitionKey, SpannError};

/// Bumped when the encoding changes; a store written by another version is
/// refused rather than misread (it is derived data, so the fix is a rebuild).
pub const KEY_VERSION: u8 = 1;

const PREFIX: &[u8] = b"vec";
const TAG_CONFIG: u8 = b'c';
const TAG_META: u8 = b'd';
const TAG_MEMBER: u8 = b'm';
const TAG_ROW: u8 = b'r';

/// Bytes of a partition key in a key.
const PARTITION_BYTES: usize = 4;
/// Fixed part of an encoded [`MetaValue`], before the centroid.
const META_FIXED: usize = 20;

fn base(index: u64, tag: u8) -> Vec<u8> {
    let mut out = Vec::with_capacity(PREFIX.len() + 1 + 8 + 1);
    out.extend_from_slice(PREFIX);
    out.push(KEY_VERSION);
    out.extend_from_slice(&index.to_be_bytes());
    out.push(tag);
    out
}

/// The single row holding the index's build options (metric/seed/dims/…).
pub fn config_key(index: u64) -> Vec<u8> {
    let mut out = base(index, TAG_CONFIG);
    out.extend_from_slice(b"cfg");
    out
}

/// The monotonic partition-key allocator. Its value is a `u64` in decimal, the
/// same shape [`crate::spann::kv`]'s counter takes; keys are never reused, so a
/// deleted partition's page cannot be aliased by a later one.
pub fn counter_key(index: u64) -> Vec<u8> {
    let mut out = base(index, TAG_CONFIG);
    out.extend_from_slice(b"ctr");
    out
}

/// A tree-level counter (`rows`, …). One row, updated in the same batch as the
/// mutation that changes it, so `open` does not walk every partition.
pub fn stat_key(index: u64, name: &str) -> Vec<u8> {
    let mut out = base(index, TAG_CONFIG);
    out.extend_from_slice(b"st|");
    out.extend_from_slice(name.as_bytes());
    out
}

/// Prefix of every partition metadata row of `index`.
pub fn meta_prefix(index: u64) -> Vec<u8> {
    base(index, TAG_META)
}

/// The metadata row of one partition.
pub fn meta_key(index: u64, partition: PartitionKey) -> Vec<u8> {
    let mut out = meta_prefix(index);
    out.extend_from_slice(&partition.to_be_bytes());
    out
}

/// Prefix of every member row of `index` (all partitions).
pub fn member_prefix(index: u64) -> Vec<u8> {
    base(index, TAG_MEMBER)
}

/// Prefix of one partition's member rows.
pub fn member_partition_prefix(index: u64, partition: PartitionKey) -> Vec<u8> {
    let mut out = member_prefix(index);
    out.extend_from_slice(&partition.to_be_bytes());
    out
}

/// Prefix of every row → partition mapping of `index`.
pub fn row_map_prefix(index: u64) -> Vec<u8> {
    base(index, TAG_ROW)
}

/// One member row: `child` is a row id in a leaf, or a child partition key in a
/// branch (the same 8-byte spelling the v2 chunk used).
pub fn member_key(index: u64, partition: PartitionKey, child: u64) -> Vec<u8> {
    let mut out = member_partition_prefix(index, partition);
    out.extend_from_slice(&child.to_be_bytes());
    out
}

/// The row → partition mapping's key. This is what makes a delete O(1): the
/// caller does not have to re-descend a vector it may no longer have.
pub fn row_map_key(index: u64, row: u64) -> Vec<u8> {
    let mut out = base(index, TAG_ROW);
    out.extend_from_slice(&row.to_be_bytes());
    out
}

/// Directory part of a key, for a prefix scan. Kept public because the RocksDB
/// implementation configures a prefix extractor with it.
pub fn index_prefix(index: u64) -> Vec<u8> {
    base(index, 0)
}

/// The partition state machine of `PartitionStateDetails.State`
/// (`partition_metadata.go:328-331`). A split or merge is visible to a
/// concurrent writer through this field, so it is persisted in the metadata.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum PartitionState {
    /// Writable and searchable.
    Ready = 0,
    /// Being split: writes must target `target1`/`target2` instead.
    Splitting = 1,
    /// Being merged: the source drains into `target1`.
    Merging = 2,
    /// Being drained into a sibling (`target1`).
    Draining = 3,
}

impl PartitionState {
    pub fn from_u8(value: u8) -> Option<Self> {
        match value {
            0 => Some(Self::Ready),
            1 => Some(Self::Splitting),
            2 => Some(Self::Merging),
            3 => Some(Self::Draining),
            _ => None,
        }
    }
}

/// One partition's metadata row.
#[derive(Clone, Debug, PartialEq)]
pub struct MetaValue {
    /// `1` = leaf; a branch is `max(child) + 1`.
    pub level: u32,
    pub state: PartitionState,
    /// The key of the branch that points here; `ROOT_KEY` for the root.
    pub parent: PartitionKey,
    /// `R_p = max‖o − c‖`. It is only ever written when the partition is
    /// created, so it can under-estimate after an append; `lower_bound` uses
    /// `d − R`, which makes an under-estimate a *looser* bound, never a wrong
    /// answer.
    pub radius: f32,
    /// Fixed at creation (`partition_metadata.go:324-327`): appends encode
    /// against this centroid and never rewrite existing members.
    pub centroid: Vec<f32>,
    /// `target1`/`target2`/`source` of `PartitionStateDetails`, unused while
    /// `state == Ready`.
    pub target1: PartitionKey,
    pub target2: PartitionKey,
    pub source: PartitionKey,
}

impl MetaValue {
    pub fn new(level: u32, parent: PartitionKey, centroid: Vec<f32>) -> Self {
        Self {
            level,
            state: PartitionState::Ready,
            parent,
            radius: 0.0,
            centroid,
            target1: 0,
            target2: 0,
            source: 0,
        }
    }

    pub fn is_leaf(&self) -> bool {
        self.level <= 1
    }
}

/// Encodes a metadata row:
///
/// ```text
/// level u32 BE | state u8 | pad[3] | parent u32 BE | radius f32 LE
/// | dims u32 LE | centroid (dims × f32 LE)
/// ```
pub fn encode_meta(meta: &MetaValue) -> Vec<u8> {
    let mut out = Vec::with_capacity(META_FIXED + meta.centroid.len() * 4);
    out.extend_from_slice(&meta.level.to_be_bytes());
    out.push(meta.state as u8);
    out.extend_from_slice(&[0u8; 3]);
    out.extend_from_slice(&meta.parent.to_be_bytes());
    out.extend_from_slice(&meta.radius.to_le_bytes());
    out.extend_from_slice(&(meta.centroid.len() as u32).to_le_bytes());
    for x in &meta.centroid {
        out.extend_from_slice(&x.to_le_bytes());
    }
    out
}

/// Decodes [`encode_meta`]'s output. `dims` is the index's dimension: a row
/// whose centroid disagrees means the store and the catalog are out of sync.
pub fn decode_meta(bytes: &[u8], dims: usize) -> Result<MetaValue, SpannError> {
    if bytes.len() < META_FIXED {
        return Err(SpannError::Corrupt(format!(
            "metadata row is {} B, expected at least {META_FIXED}",
            bytes.len()
        )));
    }
    let level = u32::from_be_bytes(bytes[0..4].try_into().unwrap());
    let state = PartitionState::from_u8(bytes[4])
        .ok_or_else(|| SpannError::Corrupt(format!("unknown partition state {}", bytes[4])))?;
    let parent = u32::from_be_bytes(bytes[8..12].try_into().unwrap());
    let radius = f32::from_le_bytes(bytes[12..16].try_into().unwrap());
    let found = u32::from_le_bytes(bytes[16..20].try_into().unwrap()) as usize;
    if found != dims {
        return Err(SpannError::DimensionMismatch {
            expected: dims,
            actual: found,
        });
    }
    let want = META_FIXED + dims * 4;
    if bytes.len() != want {
        return Err(SpannError::Corrupt(format!(
            "metadata row is {} B, expected {want} for {dims} dims",
            bytes.len()
        )));
    }
    let mut centroid = Vec::with_capacity(dims);
    for chunk in bytes[META_FIXED..].as_chunks::<4>().0 {
        centroid.push(f32::from_le_bytes(*chunk));
    }
    Ok(MetaValue {
        level,
        state,
        parent,
        radius,
        centroid,
        // The state machine's targets are only meaningful during a fixup; this
        // port performs those synchronously, so they are not on the wire yet.
        target1: 0,
        target2: 0,
        source: 0,
    })
}

/// Bytes one member row occupies: the four `Code` scalars plus the bit vector.
pub fn member_value_bytes(dims: usize) -> usize {
    quantize::META_BYTES + quantize::code_bytes(dims)
}

/// Encodes one member row. The child key lives in the *key* (so a prefix scan
/// yields ids without decoding), and the value is the code only.
pub fn encode_member(member: &Member) -> Vec<u8> {
    let mut out = Vec::with_capacity(quantize::META_BYTES + member.code.bits.len());
    out.extend_from_slice(&member.code.norm.to_le_bytes());
    out.extend_from_slice(&member.code.raw_norm.to_le_bytes());
    out.extend_from_slice(&member.code.sum.to_le_bytes());
    out.extend_from_slice(&member.code.error.to_le_bytes());
    out.extend_from_slice(&member.code.bits);
    out
}

/// Decodes a member value whose child key the caller read from the row's key.
pub fn decode_member(child: u64, bytes: &[u8], dims: usize) -> Result<Member, SpannError> {
    let want = member_value_bytes(dims);
    if bytes.len() != want {
        return Err(SpannError::Corrupt(format!(
            "member row is {} B, expected {want} for {dims} dims",
            bytes.len()
        )));
    }
    let f32_at = |at: usize| f32::from_le_bytes(bytes[at..at + 4].try_into().unwrap());
    Ok(Member {
        id: child,
        code: quantize::Code {
            norm: f32_at(0),
            raw_norm: f32_at(4),
            sum: f32_at(8),
            error: f32_at(12),
            bits: bytes[quantize::META_BYTES..].to_vec(),
        },
    })
}

/// Encodes the row → partition mapping (a `u32`, big-endian so a scan is
/// ordered).
pub fn encode_row_map(partition: PartitionKey) -> [u8; PARTITION_BYTES] {
    partition.to_be_bytes()
}

pub fn decode_row_map(bytes: &[u8]) -> Result<PartitionKey, SpannError> {
    if bytes.len() != PARTITION_BYTES {
        return Err(SpannError::Corrupt(format!(
            "row map value is {} B, expected {PARTITION_BYTES}",
            bytes.len()
        )));
    }
    Ok(u32::from_be_bytes(bytes[0..4].try_into().unwrap()))
}

/// The child key of a member row, read back from its key.
pub fn member_child_of(key: &[u8], partition: PartitionKey) -> Option<u64> {
    let tail = key.len().checked_sub(8)?;
    let at = tail.checked_sub(PARTITION_BYTES)?;
    if u32::from_be_bytes(key[at..tail].try_into().ok()?) != partition {
        return None;
    }
    Some(u64::from_be_bytes(key[tail..].try_into().ok()?))
}

#[cfg(test)]
mod tests {
    use super::*;

    fn meta() -> MetaValue {
        MetaValue {
            level: 2,
            state: PartitionState::Ready,
            parent: 7,
            radius: 3.5,
            centroid: vec![1.0, -2.0, 0.25],
            target1: 0,
            target2: 0,
            source: 0,
        }
    }

    #[test]
    fn metadata_round_trips() {
        let m = meta();
        let got = decode_meta(&encode_meta(&m), 3).unwrap();
        assert_eq!(got, m);
    }

    #[test]
    fn metadata_rejects_a_dimension_mismatch() {
        let m = meta();
        let err = decode_meta(&encode_meta(&m), 4).unwrap_err();
        assert!(matches!(err, SpannError::DimensionMismatch { .. }));
    }

    #[test]
    fn member_round_trips_and_the_child_key_is_in_the_key() {
        let member = Member {
            id: 0xDEAD_BEEF,
            code: quantize::Code {
                norm: 1.5,
                raw_norm: 2.5,
                sum: 3.5,
                error: 0.5,
                bits: vec![0xAB; quantize::code_bytes(13)],
            },
        };
        let value = encode_member(&member);
        let key = member_key(9, 42, member.id);
        assert_eq!(
            member_child_of(&key, 42),
            Some(member.id),
            "the id must be recoverable from the key alone"
        );
        let got = decode_member(member.id, &value, 13).unwrap();
        assert_eq!(got, member);
    }

    #[test]
    fn keys_are_numerically_ordered() {
        // A prefix scan is the store's only iteration order, so `9 < 10` and
        // `2 < 256` must hold on the bytes, not just on the numbers.
        let mut keys: Vec<Vec<u8>> = [2u32, 9, 10, 256]
            .into_iter()
            .map(|p| meta_key(1, p))
            .collect();
        keys.sort();
        let order: Vec<u32> = keys
            .iter()
            .map(|k| u32::from_be_bytes(k[k.len() - 4..].try_into().unwrap()))
            .collect();
        assert_eq!(order, vec![2, 9, 10, 256]);
    }

    #[test]
    fn partitions_and_indexes_do_not_share_a_prefix() {
        assert!(member_partition_prefix(1, 2).starts_with(&member_partition_prefix(1, 2)));
        assert!(!member_partition_prefix(1, 2).starts_with(&member_partition_prefix(1, 3)));
        assert!(!meta_prefix(1).starts_with(&meta_prefix(2)));
        assert!(member_key(1, 2, 3).starts_with(&member_partition_prefix(1, 2)));
    }

    #[test]
    fn row_map_round_trips() {
        assert_eq!(decode_row_map(&encode_row_map(1234)).unwrap(), 1234);
        assert!(decode_row_map(&[0u8; 3]).is_err());
    }
}
