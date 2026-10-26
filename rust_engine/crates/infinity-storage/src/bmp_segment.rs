//! On-disk form of the sparse BMP segment index (the reference's
//! `bmp_index_file_worker` + `bmp_ivt`/`bmp_fwd` tables).
//!
//! Layout (little endian):
//!
//! ```text
//! magic  b"IBMP\x01\0\0\0"
//! u32    block_size
//! u32    doc_count
//! u32    term_count
//! u64    term_index_offset
//! u64    doc_region_offset
//! docs   doc_count * (u32 nnz, nnz * (u32 term, f32 score))
//! index  term_count * (u32 term, u64 block_offset, u32 block_count)
//! blocks block_count * (u32 block_id, f32 max_score, u32 n, n * (u16 off, f32 score))
//! ```
//!
//! The term index is sorted by term so a lookup is a binary search; block data
//! is read on demand, so a query touches only the postings of its terms.

use memmap2::Mmap;
use std::fs::File;
use std::path::Path;

const MAGIC: &[u8; 8] = b"IBMP\x01\0\0\0";
const HEADER_LEN: usize = 8 + 4 + 4 + 4 + 8 + 8;

use crate::cpp_format::FormatError;

/// Writes `docs` (each a sparse vector sorted by index) as one segment file.
pub fn write_bmp_segment(
    path: &Path,
    block_size: usize,
    docs: &[Vec<(u32, f32)>],
) -> Result<(), FormatError> {
    let block_size = block_size.max(1) as u32;
    // term -> (block, offset) -> score, in block order.
    #[allow(clippy::type_complexity)]
    let mut terms: std::collections::BTreeMap<u32, Vec<(u32, Vec<(u16, f32)>)>> =
        std::collections::BTreeMap::new();
    for (doc_id, doc) in docs.iter().enumerate() {
        let block = (doc_id / block_size as usize) as u32;
        let offset = (doc_id % block_size as usize) as u16;
        for &(term, score) in doc {
            let entry = terms.entry(term).or_default();
            match entry.last_mut() {
                Some((b, list)) if *b == block => list.push((offset, score)),
                _ => entry.push((block, vec![(offset, score)])),
            }
        }
    }

    let mut out = Vec::new();
    out.extend_from_slice(MAGIC);
    out.extend_from_slice(&block_size.to_le_bytes());
    out.extend_from_slice(&(docs.len() as u32).to_le_bytes());
    out.extend_from_slice(&(terms.len() as u32).to_le_bytes());
    let index_offset_at = out.len();
    out.extend_from_slice(&0u64.to_le_bytes());
    let doc_offset_at = out.len();
    out.extend_from_slice(&0u64.to_le_bytes());

    let doc_region_offset = out.len() as u64;
    for doc in docs {
        out.extend_from_slice(&(doc.len() as u32).to_le_bytes());
        for &(term, score) in doc {
            out.extend_from_slice(&term.to_le_bytes());
            out.extend_from_slice(&score.to_le_bytes());
        }
    }

    let term_index_offset = out.len() as u64;
    let mut block_region = Vec::new();
    for (term, blocks) in &terms {
        let rel = block_region.len() as u64;
        out.extend_from_slice(&term.to_le_bytes());
        out.extend_from_slice(&rel.to_le_bytes());
        out.extend_from_slice(&(blocks.len() as u32).to_le_bytes());
        for (block, list) in blocks {
            let max = list
                .iter()
                .map(|(_, s)| *s)
                .fold(f32::NEG_INFINITY, f32::max);
            block_region.extend_from_slice(&block.to_le_bytes());
            block_region.extend_from_slice(&max.to_le_bytes());
            block_region.extend_from_slice(&(list.len() as u32).to_le_bytes());
            for (off, score) in list {
                block_region.extend_from_slice(&off.to_le_bytes());
                block_region.extend_from_slice(&score.to_le_bytes());
            }
        }
    }
    out.extend_from_slice(&block_region);

    out[doc_offset_at..doc_offset_at + 8].copy_from_slice(&doc_region_offset.to_le_bytes());
    out[index_offset_at..index_offset_at + 8].copy_from_slice(&term_index_offset.to_le_bytes());

    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| FormatError::Io(format!("{}: {e}", parent.display())))?;
    }
    std::fs::write(path, out).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))
}

/// A segment file mapped read-only.
pub struct BmpSegmentReader {
    map: Mmap,
    block_size: usize,
    doc_count: usize,
    term_count: usize,
    term_index: usize,
    doc_region: usize,
}

impl BmpSegmentReader {
    #[allow(unsafe_code)]
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: the file is read-only and never modified while mapped; a new
        // version is written under a different name.
        let map = unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?;
        if map.len() < HEADER_LEN || &map[..8] != MAGIC {
            return Err(FormatError::BadMagic(0));
        }
        let u32_at = |o: usize| u32::from_le_bytes(map[o..o + 4].try_into().unwrap()) as usize;
        let u64_at = |o: usize| u64::from_le_bytes(map[o..o + 8].try_into().unwrap()) as usize;
        Ok(Self {
            block_size: u32_at(8),
            doc_count: u32_at(12),
            term_count: u32_at(16),
            term_index: u64_at(20),
            doc_region: u64_at(28),
            map,
        })
    }

    pub fn doc_count(&self) -> usize {
        self.doc_count
    }

    /// Lookup `(block_offset, block_count)` for a term.
    fn term_blocks(&self, term: u32) -> Option<(usize, usize)> {
        let (mut lo, mut hi) = (0usize, self.term_count);
        while lo < hi {
            let mid = (lo + hi) / 2;
            let at = self.term_index + mid * 16;
            if at + 16 > self.map.len() {
                return None;
            }
            let t = u32::from_le_bytes(self.map[at..at + 4].try_into().unwrap());
            match t.cmp(&term) {
                std::cmp::Ordering::Less => lo = mid + 1,
                std::cmp::Ordering::Greater => hi = mid,
                std::cmp::Ordering::Equal => {
                    let rel =
                        u64::from_le_bytes(self.map[at + 4..at + 12].try_into().unwrap()) as usize;
                    let n =
                        u32::from_le_bytes(self.map[at + 12..at + 16].try_into().unwrap()) as usize;
                    return Some((self.term_index + self.term_count * 16 + rel, n));
                }
            }
        }
        None
    }

    fn doc(&self, doc: usize) -> Vec<(u32, f32)> {
        let mut at = self.doc_region;
        for d in 0..self.doc_count {
            if at + 4 > self.map.len() {
                return Vec::new();
            }
            let n = u32::from_le_bytes(self.map[at..at + 4].try_into().unwrap()) as usize;
            at += 4;
            if d == doc {
                let mut out = Vec::with_capacity(n);
                for _ in 0..n {
                    let term = u32::from_le_bytes(self.map[at..at + 4].try_into().unwrap());
                    let score = f32::from_le_bytes(self.map[at + 4..at + 8].try_into().unwrap());
                    out.push((term, score));
                    at += 8;
                }
                return out;
            }
            at += n * 8;
        }
        Vec::new()
    }

    /// `BMPAlgBase::SearchKnn` over the mapped tables.
    pub fn search(&self, query: &[(u32, f32)], topk: usize) -> Vec<(usize, f32)> {
        let block_num = self.doc_count.div_ceil(self.block_size);
        if block_num == 0 || topk == 0 {
            return Vec::new();
        }
        let mut bounds = vec![0.0f32; block_num];
        let mut threshold = 0.0f32;
        for &(term, q_score) in query {
            let Some((off, n)) = self.term_blocks(term) else {
                continue;
            };
            let mut at = off;
            for _ in 0..n {
                let block = u32::from_le_bytes(self.map[at..at + 4].try_into().unwrap()) as usize;
                let max = f32::from_le_bytes(self.map[at + 4..at + 8].try_into().unwrap());
                let cnt =
                    u32::from_le_bytes(self.map[at + 8..at + 12].try_into().unwrap()) as usize;
                if block < block_num {
                    bounds[block] += max * q_score;
                }
                at += 12 + cnt * 6;
            }
            // The term's k-th best score bounds the threshold; without a
            // precomputed table use its largest block bound.
            threshold = threshold.max(
                bounds
                    .iter()
                    .copied()
                    .fold(0.0f32, f32::max)
                    .min(q_score.abs()),
            );
        }
        let mut blocks: Vec<(f32, u32)> = bounds
            .iter()
            .enumerate()
            .filter(|(_, ub)| **ub >= threshold * (1.0 - 1e-6))
            .map(|(i, ub)| (*ub, i as u32))
            .collect();
        blocks.sort_by(|a, b| b.0.partial_cmp(&a.0).unwrap_or(std::cmp::Ordering::Equal));

        let mut best: Vec<(usize, f32)> = Vec::new();
        for (ub, block) in blocks {
            let start = block as usize * self.block_size;
            for doc in start..(start + self.block_size).min(self.doc_count) {
                let d = self.doc(doc);
                let score = dot(query, &d);
                best.push((doc, score));
            }
            best.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
            best.truncate(topk);
            if best.len() == topk {
                let min = best.last().map(|x| x.1).unwrap_or(0.0);
                if ub < min * (1.0 - 1e-6) {
                    break;
                }
            }
        }
        best.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        best
    }
}

fn dot(a: &[(u32, f32)], b: &[(u32, f32)]) -> f32 {
    let (mut i, mut j, mut s) = (0, 0, 0.0f32);
    while i < a.len() && j < b.len() {
        match a[i].0.cmp(&b[j].0) {
            std::cmp::Ordering::Less => i += 1,
            std::cmp::Ordering::Greater => j += 1,
            std::cmp::Ordering::Equal => {
                s += a[i].1 * b[j].1;
                i += 1;
                j += 1;
            }
        }
    }
    s
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn roundtrips_and_searches() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("seg_0.bmp");
        let docs: Vec<Vec<(u32, f32)>> = vec![
            vec![(1, 0.5), (4, 0.2)],
            vec![(2, 0.9)],
            vec![(1, 0.1), (2, 0.3)],
            vec![(4, 0.8)],
        ];
        write_bmp_segment(&path, 16, &docs).unwrap();
        let reader = BmpSegmentReader::open(&path).unwrap();
        assert_eq!(reader.doc_count(), 4);
        let hits = reader.search(&[(1, 1.0)], 2);
        assert_eq!(hits.len(), 2);
        assert_eq!(hits[0].0, 0); // 0.5 > 0.1
        assert!((hits[0].1 - 0.5).abs() < 1e-6);
        let hits = reader.search(&[(4, 1.0)], 2);
        assert_eq!(hits[0].0, 3);
        assert!((hits[0].1 - 0.8).abs() < 1e-6);
    }
}
