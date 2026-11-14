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

use crate::mapped::MappedFile;
use std::path::Path;

use crate::format::FormatError;

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
    map: MappedFile,
    dims: usize,
    doc_count: usize,
    offsets: usize,
}

impl MultiVectorSegmentReader {
    #[allow(unsafe_code)]
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        // The chunk store owns the bytes: a persistence profile may hold this
        // file only as an object part.
        let map = crate::chunk_store::chunk_store()
            .open(path)
            .map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
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
    /// `MaxSim` of one document, computed straight from the mapping.
    ///
    /// The document is decoded once into a flat buffer. Decoding inside the
    /// `(query, token)` loop re-read the same bytes `nq` times, and the decode
    /// (four bounds-checked byte reads per element) cost more than the dot
    /// product it fed.
    pub fn maxsim(&self, query: &[Vec<f32>], doc: usize) -> f32 {
        let n = self.token_count(doc);
        if n == 0 || self.dims == 0 {
            return 0.0;
        }
        let mut tokens = vec![0f32; n * self.dims];
        let at = self.doc_at(doc) + 4;
        let bytes = &self.map[at..at + n * self.dims * 4];
        let (words, _) = bytes.as_chunks::<4>();
        for (slot, word) in tokens.iter_mut().zip(words) {
            *slot = f32::from_le_bytes(*word);
        }
        let mut total = 0.0f32;
        for q in query {
            let mut best = f32::NEG_INFINITY;
            for token in tokens.chunks_exact(self.dims) {
                let score = crate::simd::dot_f32(q, token);
                if score > best {
                    best = score;
                }
            }
            if best != f32::NEG_INFINITY {
                total += best;
            }
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

    /// Timing harness for the mmap `MaxSim`: the current reader against the
    /// per-`(query, token)` decode it replaced, measured in one run.
    ///
    /// ```text
    /// cargo test --release -p storage --lib maxsim_throughput -- --ignored --nocapture
    /// ```
    #[test]
    #[ignore = "timing"]
    fn maxsim_throughput() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("seg_bench.mvec");
        let (dim, tokens, nq) = (128usize, 32usize, 32usize);
        let doc: Vec<Vec<f32>> = (0..tokens)
            .map(|t| (0..dim).map(|i| ((t * 7 + i) % 11) as f32 - 5.0).collect())
            .collect();
        let docs = vec![doc];
        write_multivector_segment(&path, dim, &docs).unwrap();
        let reader = MultiVectorSegmentReader::open(&path).unwrap();
        let query: Vec<Vec<f32>> = (0..nq)
            .map(|q| (0..dim).map(|i| ((q + i) % 13) as f32 * 0.25).collect())
            .collect();

        let rounds = 2000;
        let t0 = std::time::Instant::now();
        let mut acc = 0f32;
        for _ in 0..rounds {
            acc += reader.maxsim(&query, 0);
        }
        let flat = t0.elapsed() / rounds;

        // The loop this replaced: decode inside the (query, token) pair.
        let t0 = std::time::Instant::now();
        let mut old_acc = 0f32;
        for _ in 0..rounds {
            let n = reader.token_count(0);
            let mut token = Vec::with_capacity(dim);
            let mut total = 0f32;
            for q in &query {
                let mut best = f32::NEG_INFINITY;
                for t in 0..n {
                    token.clear();
                    let at = reader.doc_at(0) + 4 + t * dim * 4;
                    for i in 0..dim {
                        token.push(f32::from_le_bytes(
                            reader.map[at + i * 4..at + i * 4 + 4].try_into().unwrap(),
                        ));
                    }
                    let score: f32 = q.iter().zip(token.iter()).map(|(a, b)| a * b).sum();
                    if score > best {
                        best = score;
                    }
                }
                total += best;
            }
            old_acc += total;
        }
        let per_pair = t0.elapsed() / rounds;
        eprintln!(
            "mmap maxsim nq={nq} tokens={tokens} dim={dim}: per_pair_decode={per_pair:?} \
             flat_decode+simd={flat:?} ({:.2}x) acc={acc} {old_acc}",
            per_pair.as_secs_f64() / flat.as_secs_f64()
        );
    }

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
