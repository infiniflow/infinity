//! On-disk form of a multi-vector segment: the token vectors of every document,
//! mapped read-only so `MaxSim` runs without materialising the column.
//!
//! Layout (little endian):
//!
//! ```text
//! magic  b"IMVEC\x01\0\0"
//! u32    dims
//! u32    doc_count
//! u64    doc_offsets_offset
//! docs   per doc: u32 token_count, token_count * dims * f32
//! offs   doc_count * u64 byte offset of each document's first token
//! ```

use memmap2::Mmap;
use std::fs::File;
use std::path::Path;

use crate::cpp_format::FormatError;

const MAGIC: &[u8; 8] = b"IMVEC\x01\0\0";
const HEADER_LEN: usize = 8 + 4 + 4 + 8;

/// Writes one segment's multi-vector column.
pub fn write_multivector_segment(
    path: &Path,
    dims: usize,
    docs: &[Vec<Vec<f32>>],
) -> Result<(), FormatError> {
    let mut out = Vec::new();
    out.extend_from_slice(MAGIC);
    out.extend_from_slice(&(dims as u32).to_le_bytes());
    out.extend_from_slice(&(docs.len() as u32).to_le_bytes());
    let offs_at = out.len();
    out.extend_from_slice(&0u64.to_le_bytes());

    let mut offsets: Vec<u64> = Vec::with_capacity(docs.len());
    for doc in docs {
        offsets.push(out.len() as u64);
        out.extend_from_slice(&(doc.len() as u32).to_le_bytes());
        for token in doc {
            for v in token {
                out.extend_from_slice(&v.to_le_bytes());
            }
        }
    }
    let offs = out.len() as u64;
    for o in &offsets {
        out.extend_from_slice(&o.to_le_bytes());
    }
    out[offs_at..offs_at + 8].copy_from_slice(&offs.to_le_bytes());

    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| FormatError::Io(format!("{}: {e}", parent.display())))?;
    }
    std::fs::write(path, out).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))
}

/// A multi-vector segment mapped read-only.
pub struct MultiVectorSegmentReader {
    map: Mmap,
    dims: usize,
    doc_count: usize,
    offsets: usize,
}

impl MultiVectorSegmentReader {
    #[allow(unsafe_code)]
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: read-only mapping of an immutable segment file.
        let map = unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?;
        if map.len() < HEADER_LEN || &map[..8] != MAGIC {
            return Err(FormatError::BadMagic(0));
        }
        let u32_at = |o: usize| u32::from_le_bytes(map[o..o + 4].try_into().unwrap()) as usize;
        let u64_at = |o: usize| u64::from_le_bytes(map[o..o + 8].try_into().unwrap()) as usize;
        Ok(Self {
            dims: u32_at(8),
            doc_count: u32_at(12),
            offsets: u64_at(16),
            map,
        })
    }

    pub fn dims(&self) -> usize {
        self.dims
    }

    pub fn doc_count(&self) -> usize {
        self.doc_count
    }

    fn doc_at(&self, doc: usize) -> usize {
        let at = self.offsets + doc * 8;
        u64::from_le_bytes(self.map[at..at + 8].try_into().unwrap()) as usize
    }

    /// Token count of a document.
    pub fn token_count(&self, doc: usize) -> usize {
        if doc >= self.doc_count {
            return 0;
        }
        let at = self.doc_at(doc);
        u32::from_le_bytes(self.map[at..at + 4].try_into().unwrap()) as usize
    }

    /// Reads token `t` of document `doc` into `out` (dims floats).
    fn token_into(&self, doc: usize, t: usize, out: &mut Vec<f32>) {
        out.clear();
        let at = self.doc_at(doc) + 4 + t * self.dims * 4;
        for i in 0..self.dims {
            out.push(f32::from_le_bytes(
                self.map[at + i * 4..at + i * 4 + 4].try_into().unwrap(),
            ));
        }
    }

    /// `MaxSim` of one document, computed straight from the mapping.
    pub fn maxsim(&self, query: &[Vec<f32>], doc: usize) -> f32 {
        let n = self.token_count(doc);
        if n == 0 {
            return 0.0;
        }
        let mut token = Vec::with_capacity(self.dims);
        let mut total = 0.0f32;
        for q in query {
            let mut best = f32::NEG_INFINITY;
            for t in 0..n {
                self.token_into(doc, t, &mut token);
                let score: f32 = q.iter().zip(token.iter()).map(|(a, b)| a * b).sum();
                if score > best {
                    best = score;
                }
            }
            total += best;
        }
        total
    }

    /// Top-k by `MaxSim`, tie-broken by document id.
    pub fn search(&self, query: &[Vec<f32>], k: usize) -> Vec<(usize, f32)> {
        let mut scored: Vec<(usize, f32)> = (0..self.doc_count)
            .map(|d| (d, self.maxsim(query, d)))
            .collect();
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(k);
        scored
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn roundtrips_and_scores() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("seg_0.mvec");
        let docs = vec![vec![vec![1.0, 0.0], vec![0.0, 1.0]], vec![vec![0.5, 0.5]]];
        write_multivector_segment(&path, 2, &docs).unwrap();
        let reader = MultiVectorSegmentReader::open(&path).unwrap();
        assert_eq!(reader.doc_count(), 2);
        assert_eq!(reader.token_count(0), 2);
        let q = vec![vec![1.0, 0.0], vec![0.0, 1.0]];
        assert!((reader.maxsim(&q, 0) - 2.0).abs() < 1e-6);
        assert!((reader.maxsim(&q, 1) - 1.0).abs() < 1e-6);
        let hits = reader.search(&q, 2);
        assert_eq!(hits[0].0, 0);
        assert_eq!(hits[1].0, 1);
    }
}
