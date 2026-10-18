//! Secondary-index segment files, following the reference's
//! `SecondaryIndexDataBase::SaveIndexInner` (`secondary_index_data_impl.cpp:224`)
//! and the PGM serialization in `secondary_index_pgm.cppm:129`.
//!
//! Layout of one chunk (`chunk_{n}.idx`, no header):
//!
//! ```text
//! keys     n * u64            ordered keys (the reference's OrderedKeyType)
//! offsets  n * u32            segment offsets, parallel to `keys`
//! pgm      u32 n
//!          u64 first_key
//!          u32 segment_count, segment_count * (u64 key, f32 slope, i32 intercept)
//!          u32 level_count,   level_count * u64
//! ```
//!
//! The PGM is a piecewise-linear model with `epsilon = 4` and segment size 64
//! (`PGMIndex<T, 64, 4, float>`); a lookup returns a position range that is then
//! resolved with a binary search over the mapped keys.

use memmap2::Mmap;
use std::fs::File;
use std::path::Path;

/// libstdc++'s `std::hash<std::string_view>` (`_Hash_bytes`, MurmurHash64A with
/// seed `0xc70f6907`), which is what the reference stores for VARCHAR keys
/// (`secondary_index_data.cppm:106-110`).
pub fn hash_varchar(text: &str) -> u64 {
    const SEED: u64 = 0xc70f_6907;
    const M: u64 = 0xc6a4_a793_5bd1_e995;
    const R: u32 = 47;
    let data = text.as_bytes();
    let mut h = SEED ^ (data.len() as u64).wrapping_mul(M);
    let mut chunks = data.chunks_exact(8);
    for chunk in &mut chunks {
        let mut k = u64::from_le_bytes(chunk.try_into().unwrap());
        k = k.wrapping_mul(M);
        k ^= k >> R;
        k = k.wrapping_mul(M);
        h ^= k;
        h = h.wrapping_mul(M);
    }
    let tail = chunks.remainder();
    for (i, byte) in tail.iter().enumerate() {
        h ^= u64::from(*byte) << (8 * i);
    }
    if !tail.is_empty() {
        h = h.wrapping_mul(M);
    }
    h ^= h >> R;
    h = h.wrapping_mul(M);
    h ^= h >> R;
    h
}

use crate::cpp_format::FormatError;

const PGM_EPSILON: u64 = 4;
const PGM_SEGMENT_SIZE: usize = 64;

/// One piecewise-linear segment: a key, a slope and an intercept.
#[derive(Debug, Clone, Copy)]
struct PgmSegment {
    key: u64,
    slope: f32,
    intercept: i32,
}

/// A single-level PGM over ascending keys.
#[derive(Debug, Default)]
struct Pgm {
    n: u32,
    first_key: u64,
    segments: Vec<PgmSegment>,
}

impl Pgm {
    fn build(keys: &[u64]) -> Self {
        if keys.is_empty() {
            return Self::default();
        }
        let mut segments = Vec::new();
        // Greedy segmentation: extend a segment while the linear interpolation
        // stays within `epsilon` for every point (the PGM construction rule).
        let mut start = 0usize;
        while start < keys.len() {
            let x0 = keys[start];
            let mut end = start;
            let mut slope = 0.0f32;
            while end + 1 < keys.len() {
                let candidate_end = end + 1;
                let candidate_slope = if keys[candidate_end] > x0 {
                    (candidate_end - start) as f32 / (keys[candidate_end] - x0) as f32
                } else {
                    0.0
                };
                let ok = (start..=candidate_end).all(|i| {
                    let predicted = start as f32 + candidate_slope * (keys[i] as f32 - x0 as f32);
                    (predicted - i as f32).abs() <= PGM_EPSILON as f32
                });
                if !ok {
                    break;
                }
                slope = candidate_slope;
                end = candidate_end;
                if end - start + 1 >= PGM_SEGMENT_SIZE {
                    break;
                }
            }
            segments.push(PgmSegment {
                key: x0,
                slope,
                intercept: start as i32,
            });
            start = end + 1;
        }
        Self {
            n: keys.len() as u32,
            first_key: keys[0],
            segments,
        }
    }

    /// Approximate position of `key`, clamped to `[0, n]`.
    fn approx(&self, key: u64) -> usize {
        if self.segments.is_empty() {
            return 0;
        }
        // Last segment whose first key is <= key.
        let idx = match self.segments.binary_search_by_key(&key, |s| s.key) {
            Ok(i) => i,
            Err(0) => 0,
            Err(i) => i - 1,
        };
        let seg = &self.segments[idx];
        let pos = seg.intercept as f32 + seg.slope * (key as f32 - seg.key as f32);
        let pos = pos.round();
        if pos < 0.0 {
            0
        } else if pos as usize > self.n as usize {
            self.n as usize
        } else {
            pos as usize
        }
    }

    fn encode(&self) -> Vec<u8> {
        let mut out = Vec::new();
        out.extend_from_slice(&self.n.to_le_bytes());
        out.extend_from_slice(&self.first_key.to_le_bytes());
        out.extend_from_slice(&(self.segments.len() as u32).to_le_bytes());
        for s in &self.segments {
            out.extend_from_slice(&s.key.to_le_bytes());
            out.extend_from_slice(&s.slope.to_le_bytes());
            out.extend_from_slice(&s.intercept.to_le_bytes());
        }
        // One level: an empty level offset table, as the leaf-level index.
        out.extend_from_slice(&0u32.to_le_bytes());
        out
    }
}

/// Writes one secondary-index chunk for `(key, offset)` pairs sorted by key.
pub fn write_secondary_segment(path: &Path, entries: &[(u64, u32)]) -> Result<(), FormatError> {
    let keys: Vec<u64> = entries.iter().map(|(k, _)| *k).collect();
    let pgm = Pgm::build(&keys);
    let mut out = Vec::with_capacity(entries.len() * 12 + 64);
    for (k, _) in entries {
        out.extend_from_slice(&k.to_le_bytes());
    }
    for (_, off) in entries {
        out.extend_from_slice(&off.to_le_bytes());
    }
    out.extend_from_slice(&pgm.encode());

    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| FormatError::Io(format!("{}: {e}", parent.display())))?;
    }
    std::fs::write(path, out).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))
}

/// A chunk mapped read-only; lookups use the PGM then a binary search.
pub struct SecondarySegmentReader {
    map: Mmap,
    row_count: usize,
    pgm: Pgm,
}

impl SecondarySegmentReader {
    #[allow(unsafe_code)]
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: read-only mapping of an immutable chunk file.
        let map = unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?;
        if map.len() < 16 {
            return Err(FormatError::Truncated {
                need: 16,
                got: map.len(),
            });
        }
        // The PGM is the tail; its size is derived by walking its header and
        // segment table, so read it after the fixed-size key/offset arrays.
        let mut row_count = 0usize;
        // `n` is duplicated in the PGM header; the arrays are `n` keys + offsets.
        // Find the PGM by trying `n` from the plausible range: the reference
        // stores the row count implicitly through the chunk metadata, so here we
        // locate it from the PGM header at the tail of the arrays.
        // The chunk length is `n*12 + 12 + segments*16 + 4`.
        // Solve for n given the segment count read from the header.
        // Simpler: scan candidate n (arrays must fit) and validate the PGM header.
        for n in 0..=map.len() / 12 {
            let pgm_at = n * 12;
            if pgm_at + 16 > map.len() {
                break;
            }
            let pgm_n = u32::from_le_bytes(map[pgm_at..pgm_at + 4].try_into().unwrap()) as usize;
            let segs =
                u32::from_le_bytes(map[pgm_at + 12..pgm_at + 16].try_into().unwrap()) as usize;
            let end = pgm_at + 16 + segs * 16 + 4;
            if pgm_n == n && end == map.len() {
                row_count = n;
                break;
            }
        }
        let pgm_at = row_count * 12;
        if pgm_at + 16 > map.len() {
            return Err(FormatError::Truncated {
                need: pgm_at + 16,
                got: map.len(),
            });
        }
        let n = u32::from_le_bytes(map[pgm_at..pgm_at + 4].try_into().unwrap());
        let first_key = u64::from_le_bytes(map[pgm_at + 4..pgm_at + 12].try_into().unwrap());
        let seg_count =
            u32::from_le_bytes(map[pgm_at + 12..pgm_at + 16].try_into().unwrap()) as usize;
        let mut segments = Vec::with_capacity(seg_count);
        for i in 0..seg_count {
            let at = pgm_at + 16 + i * 16;
            segments.push(PgmSegment {
                key: u64::from_le_bytes(map[at..at + 8].try_into().unwrap()),
                slope: f32::from_le_bytes(map[at + 8..at + 12].try_into().unwrap()),
                intercept: i32::from_le_bytes(map[at + 12..at + 16].try_into().unwrap()),
            });
        }
        Ok(Self {
            map,
            row_count,
            pgm: Pgm {
                n,
                first_key,
                segments,
            },
        })
    }

    pub fn len(&self) -> usize {
        self.row_count
    }

    pub fn is_empty(&self) -> bool {
        self.row_count == 0
    }

    fn key(&self, i: usize) -> u64 {
        let at = i * 8;
        u64::from_le_bytes(self.map[at..at + 8].try_into().unwrap())
    }

    fn offset(&self, i: usize) -> u32 {
        let at = self.row_count * 8 + i * 4;
        u32::from_le_bytes(self.map[at..at + 4].try_into().unwrap())
    }

    /// All segment offsets whose key equals `key`.
    pub fn lookup(&self, key: u64) -> Vec<u32> {
        if self.row_count == 0 {
            return Vec::new();
        }
        let approx = self.pgm.approx(key);
        // The PGM guarantees the answer lies near the estimate; a slightly
        // wider window keeps the binary search safe for the greedy build.
        let lo = approx.saturating_sub(2 * PGM_EPSILON as usize + 1);
        let hi = (approx + 2 * PGM_EPSILON as usize + 2).min(self.row_count);
        let mut i = self.lower_bound(lo, hi, key);
        let mut out = Vec::new();
        while i < self.row_count && self.key(i) == key {
            out.push(self.offset(i));
            i += 1;
        }
        // A model miss (key far outside the window) falls back to a full search.
        if out.is_empty() {
            let mut i = self.lower_bound(0, self.row_count, key);
            while i < self.row_count && self.key(i) == key {
                out.push(self.offset(i));
                i += 1;
            }
        }
        out
    }

    /// `partition_point` over the mapped keys: first index in `[lo, hi)` whose
    /// key is `>= key`.
    fn lower_bound(&self, lo: usize, hi: usize, key: u64) -> usize {
        let (mut lo, mut hi) = (lo, hi);
        while lo < hi {
            let mid = (lo + hi) / 2;
            if self.key(mid) < key {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        lo
    }

    /// First key (useful for range scans).
    pub fn first_key(&self) -> u64 {
        self.pgm.first_key
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn varchar_hash_matches_libstdcxx() {
        // Reference values from g++ `std::hash<std::string_view>`.
        assert_eq!(hash_varchar(""), 6142509188972423790);
        assert_eq!(hash_varchar("a"), 4993892634952068459);
        assert_eq!(hash_varchar("abc"), 3663726644998027833);
        assert_eq!(hash_varchar("hello world"), 5577293430985752569);
        assert_eq!(hash_varchar("default text"), 7918694999526961961);
    }

    #[test]
    fn pgm_lookup_matches_binary_search() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_0.idx");
        let entries: Vec<(u64, u32)> = (0..500u64).map(|i| (i * 7 + 3, i as u32)).collect();
        write_secondary_segment(&path, &entries).unwrap();
        let reader = SecondarySegmentReader::open(&path).unwrap();
        assert_eq!(reader.len(), 500);
        for i in [0usize, 1, 63, 64, 250, 499] {
            let key = entries[i].0;
            let hits = reader.lookup(key);
            assert_eq!(hits, vec![entries[i].1], "key {key}");
        }
        assert!(reader.lookup(1).is_empty());
    }

    #[test]
    fn duplicate_keys_return_every_offset() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_1.idx");
        let entries = vec![(1u64, 10u32), (1, 11), (2, 12), (3, 13)];
        write_secondary_segment(&path, &entries).unwrap();
        let reader = SecondarySegmentReader::open(&path).unwrap();
        assert_eq!(reader.lookup(1), vec![10, 11]);
        assert_eq!(reader.lookup(3), vec![13]);
    }
}
