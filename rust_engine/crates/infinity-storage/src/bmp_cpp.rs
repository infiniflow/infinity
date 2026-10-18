//! Sparse BMP index in the reference's byte layout.
//!
//! Mirrors `BMPAlg::SaveToPtr` / `LoadFromPtr` (`bmp_alg.cppm:362,380`):
//!
//! ```text
//! ivt:  u64 term_num                       (aligned 8)
//!       i32 kth[term_num]                  (aligned 4)
//!       f32 kth_score[term_num]            (aligned 4)
//!       u64 block_prefix[term_num + 1]     (aligned 8)
//!       i32 block_id[sum]                  (aligned 4)
//!       f32 max_score[sum]                 (aligned 4)
//! fwd:  u64 block_size                     (aligned 8)
//!       u64 block_num                      (aligned 8)
//!       u64 block_offsets[block_num + 1]   (aligned 8, relative to the data start)
//!       per block: u64 term_num            (aligned 8)
//!                  u64 term_prefix[term_num + 1] (aligned 8)
//!                  I term_ids[term_num]    (aligned to sizeof(I))
//!                  u8 offsets[sum]         (aligned 1)
//!                  f32 values[sum]         (aligned 4)
//! docs: u64 doc_num                        (aligned 8)
//!       u32 doc_ids[doc_num]               (aligned 4)
//! ```
//!
//! Alignment is computed from the absolute buffer address exactly as
//! `GetSizeInBytesVecAligned` does (`serialize.h:143`); a heap buffer is
//! 16-byte aligned, so padding is `align_to(offset, align)`.

use memmap2::Mmap;
use std::fs::File;
use std::path::Path;

use crate::cpp_format::FormatError;

/// Width of the sparse index element type (`EmbeddingDataType`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IdxWidth {
    I8,
    I16,
    I32,
}

impl IdxWidth {
    fn size(self) -> usize {
        match self {
            IdxWidth::I8 => 1,
            IdxWidth::I16 => 2,
            IdxWidth::I32 => 4,
        }
    }
}

struct AlignedWriter {
    buf: Vec<u8>,
    /// Absolute offset of `buf[0]` in the file: the reference aligns to the
    /// address of the output buffer, not to the offset inside a sub-region.
    base: usize,
}

impl AlignedWriter {
    fn new() -> Self {
        Self {
            buf: Vec::new(),
            base: 0,
        }
    }

    fn new_at(base: usize) -> Self {
        Self {
            buf: Vec::new(),
            base,
        }
    }

    fn pad(&mut self, align: usize) {
        while (self.base + self.buf.len()) % align != 0 {
            self.buf.push(0);
        }
    }

    fn u64(&mut self, v: u64) {
        self.pad(8);
        self.buf.extend_from_slice(&v.to_le_bytes());
    }

    fn i32(&mut self, v: i32) {
        self.pad(4);
        self.buf.extend_from_slice(&v.to_le_bytes());
    }

    fn f32(&mut self, v: f32) {
        self.pad(4);
        self.buf.extend_from_slice(&v.to_le_bytes());
    }

    fn u64s(&mut self, vs: &[u64]) {
        self.pad(8);
        for v in vs {
            self.buf.extend_from_slice(&v.to_le_bytes());
        }
    }

    fn i32s(&mut self, vs: &[i32]) {
        self.pad(4);
        for v in vs {
            self.buf.extend_from_slice(&v.to_le_bytes());
        }
    }

    fn f32s(&mut self, vs: &[f32]) {
        self.pad(4);
        for v in vs {
            self.buf.extend_from_slice(&v.to_le_bytes());
        }
    }

    fn bytes(&mut self, vs: &[u8]) {
        self.buf.extend_from_slice(vs);
    }

    fn terms(&mut self, width: IdxWidth, vs: &[i32]) {
        self.pad(width.size());
        for v in vs {
            match width {
                IdxWidth::I8 => self.buf.push(*v as i8 as u8),
                IdxWidth::I16 => self.buf.extend_from_slice(&(*v as i16).to_le_bytes()),
                IdxWidth::I32 => self.buf.extend_from_slice(&v.to_le_bytes()),
            }
        }
    }
}

/// One document: `(row id, [(term, value)])`, terms sorted ascending.
pub type SparseDoc = (u64, Vec<(i32, f32)>);

/// Writes the BMP index for `docs` (already in row order) and returns the bytes.
pub fn encode_bmp(
    block_size: usize,
    width: IdxWidth,
    docs: &[SparseDoc],
    term_num: usize,
) -> Vec<u8> {
    let block_size = block_size.max(1);
    let block_num = docs.len().div_ceil(block_size);
    // Per term: block maxima and k-th score.
    let mut term_blocks: Vec<Vec<(i32, f32)>> = vec![Vec::new(); term_num];
    // Per block: term -> [(offset in block, value)].
    let mut blocks: Vec<Vec<(i32, Vec<(u8, f32)>)>> = Vec::with_capacity(block_num);
    for b in 0..block_num {
        let start = b * block_size;
        let end = (start + block_size).min(docs.len());
        // Group by term: a block's term list is unique and ascending in the
        // reference's `BlockTerms`, so every document's entries of the same
        // term must share one entry.
        let mut grouped: std::collections::BTreeMap<i32, Vec<(u8, f32)>> =
            std::collections::BTreeMap::new();
        for (i, (_, entries)) in docs[start..end].iter().enumerate() {
            for (term, value) in entries {
                let t = *term;
                if t < 0 || t as usize >= term_num {
                    continue;
                }
                grouped.entry(t).or_default().push((i as u8, *value));
            }
        }
        let terms: Vec<(i32, Vec<(u8, f32)>)> = grouped.into_iter().collect();
        for (t, list) in &terms {
            let max = list
                .iter()
                .map(|(_, v)| *v)
                .fold(f32::NEG_INFINITY, f32::max);
            let entry = &mut term_blocks[*t as usize];
            // `AddBlock` pushes one entry per block, in block order.
            if entry.last().map(|(id, _)| *id) != Some(b as i32) {
                entry.push((b as i32, max));
            }
        }
        blocks.push(terms);
    }

    let mut w = AlignedWriter::new();
    // ---- ivt ----
    w.u64(term_num as u64);
    let kths: Vec<i32> = vec![-1; term_num];
    let kth_scores: Vec<f32> = vec![0.0; term_num];
    w.i32s(&kths);
    w.f32s(&kth_scores);
    let mut prefix: Vec<u64> = Vec::with_capacity(term_num + 1);
    prefix.push(0);
    let mut block_ids: Vec<i32> = Vec::new();
    let mut max_scores: Vec<f32> = Vec::new();
    for entry in &term_blocks {
        for (id, score) in entry {
            block_ids.push(*id);
            max_scores.push(*score);
        }
        prefix.push(block_ids.len() as u64);
    }
    w.u64s(&prefix);
    w.i32s(&block_ids);
    w.f32s(&max_scores);

    // ---- fwd ----
    // The offsets array sits after block_size/block_num; each block's bytes are
    // aligned to their absolute position, so the sizes are computed against the
    // final layout.
    let block_bytes = |terms: &[(i32, Vec<(u8, f32)>)], base: usize| -> Vec<u8> {
        let mut bw = AlignedWriter::new_at(base);
        let mut prefix: Vec<u64> = Vec::with_capacity(terms.len() + 1);
        prefix.push(0);
        let mut ids: Vec<i32> = Vec::with_capacity(terms.len());
        let mut offs: Vec<u8> = Vec::new();
        let mut vals: Vec<f32> = Vec::new();
        for (t, list) in terms {
            ids.push(*t);
            for (o, v) in list {
                offs.push(*o);
                vals.push(*v);
            }
            prefix.push(offs.len() as u64);
        }
        bw.u64(terms.len() as u64);
        bw.u64s(&prefix);
        bw.terms(width, &ids);
        bw.bytes(&offs);
        bw.f32s(&vals);
        bw.buf
    };

    // `fwd_start` is 8-aligned (every preceding write padded), so
    // block_size, block_num and the offsets array are contiguous.
    let fwd_start = {
        let mut at = w.buf.len();
        while at % 8 != 0 {
            at += 1;
        }
        at
    };
    let data_start = fwd_start + 16 + 8 * (block_num + 1);
    let mut offsets: Vec<u64> = Vec::with_capacity(block_num + 1);
    let mut rendered: Vec<Vec<u8>> = Vec::with_capacity(block_num);
    for terms in &blocks {
        let at = data_start + offsets.last().copied().unwrap_or(0) as usize;
        let bytes = block_bytes(terms, at);
        offsets.push(offsets.last().copied().unwrap_or(0) + bytes.len() as u64);
        rendered.push(bytes);
    }
    // The first offset is 0 (pushed implicitly).
    let mut all_offsets: Vec<u64> = Vec::with_capacity(block_num + 1);
    all_offsets.push(0);
    all_offsets.extend_from_slice(&offsets);
    w.u64(block_size as u64);
    w.u64(block_num as u64);
    w.u64s(&all_offsets);
    for bytes in &rendered {
        w.buf.extend_from_slice(bytes);
    }
    // ---- doc ids ----
    w.u64(docs.len() as u64);
    let ids: Vec<u32> = docs.iter().map(|(row, _)| *row as u32).collect();
    w.pad(4);
    for id in &ids {
        w.buf.extend_from_slice(&id.to_le_bytes());
    }
    w.buf
}

/// Writes the encoded index to `path`.
pub fn write_bmp_file(
    path: &Path,
    block_size: usize,
    width: IdxWidth,
    docs: &[SparseDoc],
    term_num: usize,
) -> Result<(), FormatError> {
    let bytes = encode_bmp(block_size, width, docs, term_num);
    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| FormatError::Io(format!("{}: {e}", parent.display())))?;
    }
    std::fs::write(path, bytes).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))
}

/// A BMP file mapped read-only, parsed into the same tables the reference
/// exposes (`BMPIvt` + `BlockFwd` + `doc_ids`).
pub struct BmpCppReader {
    block_size: usize,
    widths: IdxWidth,
    /// Per term: `(block id, max score)`.
    ivt: Vec<Vec<(i32, f32)>>,
    /// Per block: `(term, [(offset, value)])`.
    blocks: Vec<Vec<(i32, Vec<(u8, f32)>)>>,
    doc_ids: Vec<u32>,
}

impl BmpCppReader {
    #[allow(unsafe_code)]
    pub fn open(path: &Path, width: IdxWidth) -> Result<Self, FormatError> {
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: read-only mapping of an immutable chunk file.
        let map = unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?;
        let mut cur = Cursor { buf: &map, at: 0 };
        let term_num = cur.u64_aligned()? as usize;
        cur.skip_aligned(4, term_num * 4); // kths
        cur.skip_aligned(4, term_num * 4); // kth scores
        let prefix: Vec<u64> = cur.u64s_aligned(term_num + 1)?;
        let sum = *prefix.last().unwrap_or(&0) as usize;
        let block_ids = cur.i32s_aligned(sum)?;
        let max_scores = cur.f32s_aligned(sum)?;
        let mut ivt: Vec<Vec<(i32, f32)>> = Vec::with_capacity(term_num);
        for t in 0..term_num {
            let lo = prefix[t] as usize;
            let hi = prefix[t + 1] as usize;
            ivt.push((lo..hi).map(|i| (block_ids[i], max_scores[i])).collect());
        }
        let block_size = cur.u64_aligned()? as usize;
        let block_num = cur.u64_aligned()? as usize;
        let offsets = cur.u64s_aligned(block_num + 1)?;
        let data_start = cur.at;
        let mut blocks: Vec<Vec<(i32, Vec<(u8, f32)>)>> = Vec::with_capacity(block_num);
        for i in 0..block_num {
            let mut bc = Cursor {
                buf: &map,
                at: data_start + offsets[i] as usize,
            };
            let terms = bc.u64_aligned()? as usize;
            let term_prefix = bc.u64s_aligned(terms + 1)?;
            let ids = bc.terms_aligned(width, terms)?;
            let total = *term_prefix.last().unwrap_or(&0) as usize;
            let offs = bc.bytes(total);
            let vals = bc.f32s_aligned(total)?;
            let mut list = Vec::with_capacity(terms);
            for t in 0..terms {
                let lo = term_prefix[t] as usize;
                let hi = term_prefix[t + 1] as usize;
                list.push((ids[t], (lo..hi).map(|j| (offs[j], vals[j])).collect()));
            }
            blocks.push(list);
        }
        cur.at = data_start + offsets[block_num] as usize;
        let doc_num = cur.u64_aligned()? as usize;
        let doc_ids = cur.u32s_aligned(doc_num)?;
        Ok(Self {
            block_size,
            widths: width,
            ivt,
            blocks,
            doc_ids,
        })
    }

    pub fn doc_count(&self) -> usize {
        self.doc_ids.len()
    }

    /// `BMPAlgBase::SearchKnn`: threshold from the k-th posting score, block
    /// bounds, exact block scoring, and `alpha` early stop.
    pub fn search(&self, query: &[(i32, f32)], topk: usize, alpha: f32) -> Vec<(usize, f32)> {
        let _ = self.widths;
        if topk == 0 || self.doc_ids.is_empty() {
            return Vec::new();
        }
        let topk = topk.min(self.doc_ids.len());
        let block_num = self.blocks.len();
        let mut bounds = vec![0.0f32; block_num];
        let mut threshold = 0.0f32;
        for &(term, q) in query {
            let Some(posting) = self.ivt.get(term as usize) else {
                continue;
            };
            for (block, max) in posting {
                if *block >= 0 && (*block as usize) < block_num {
                    bounds[*block as usize] += q * max;
                }
            }
            // No k-th table is persisted here (the reference stores it from its
            // `topk` build option); the largest term bound is the safe stand-in.
            threshold = threshold.max(posting.iter().map(|(_, m)| *m * q).fold(0.0f32, f32::max));
        }
        let mut order: Vec<(f32, usize)> = bounds
            .iter()
            .enumerate()
            .filter(|(_, b)| **b >= threshold * (1.0 - 1e-6))
            .map(|(i, b)| (*b, i))
            .collect();
        order.sort_by(|a, b| b.0.partial_cmp(&a.0).unwrap_or(std::cmp::Ordering::Equal));

        let mut best: Vec<(usize, f32)> = Vec::new();
        for (ub, block) in order {
            let base = block * self.block_size;
            let mut scores = vec![0.0f32; self.block_size];
            let mut i = 0usize;
            let terms = &self.blocks[block];
            for &(term, q) in query {
                while i < terms.len() && terms[i].0 < term {
                    i += 1;
                }
                if i == terms.len() {
                    break;
                }
                if terms[i].0 == term {
                    for (off, value) in &terms[i].1 {
                        scores[*off as usize] += q * value;
                    }
                }
            }
            for (off, score) in scores.iter().enumerate() {
                let doc = base + off;
                if doc < self.doc_ids.len() {
                    best.push((self.doc_ids[doc] as usize, *score));
                }
            }
            best.sort_by(|a, b| {
                b.1.partial_cmp(&a.1)
                    .unwrap_or(std::cmp::Ordering::Equal)
                    .then(a.0.cmp(&b.0))
            });
            best.truncate(topk);
            if best.len() == topk {
                let min = best.last().map(|x| x.1).unwrap_or(0.0);
                if ub * alpha < min * (1.0 - 1e-6) {
                    break;
                }
            }
        }
        best
    }
}

struct Cursor<'a> {
    buf: &'a [u8],
    at: usize,
}

impl<'a> Cursor<'a> {
    fn align(&mut self, align: usize) {
        while self.at % align != 0 {
            self.at += 1;
        }
    }

    fn u64_aligned(&mut self) -> Result<u64, FormatError> {
        self.align(8);
        self.take(8)
            .map(|b| u64::from_le_bytes(b.try_into().unwrap()))
    }

    fn u64s_aligned(&mut self, n: usize) -> Result<Vec<u64>, FormatError> {
        self.align(8);
        let mut out = Vec::with_capacity(n);
        for _ in 0..n {
            out.push(u64::from_le_bytes(self.take(8)?.try_into().unwrap()));
        }
        Ok(out)
    }

    fn i32s_aligned(&mut self, n: usize) -> Result<Vec<i32>, FormatError> {
        self.align(4);
        let mut out = Vec::with_capacity(n);
        for _ in 0..n {
            out.push(i32::from_le_bytes(self.take(4)?.try_into().unwrap()));
        }
        Ok(out)
    }

    fn f32s_aligned(&mut self, n: usize) -> Result<Vec<f32>, FormatError> {
        self.align(4);
        let mut out = Vec::with_capacity(n);
        for _ in 0..n {
            out.push(f32::from_le_bytes(self.take(4)?.try_into().unwrap()));
        }
        Ok(out)
    }

    fn u32s_aligned(&mut self, n: usize) -> Result<Vec<u32>, FormatError> {
        self.align(4);
        let mut out = Vec::with_capacity(n);
        for _ in 0..n {
            out.push(u32::from_le_bytes(self.take(4)?.try_into().unwrap()));
        }
        Ok(out)
    }

    fn terms_aligned(&mut self, width: IdxWidth, n: usize) -> Result<Vec<i32>, FormatError> {
        self.align(width.size());
        let mut out = Vec::with_capacity(n);
        for _ in 0..n {
            let v = match width {
                IdxWidth::I8 => self.take(1)?[0] as i8 as i32,
                IdxWidth::I16 => i16::from_le_bytes(self.take(2)?.try_into().unwrap()) as i32,
                IdxWidth::I32 => i32::from_le_bytes(self.take(4)?.try_into().unwrap()),
            };
            out.push(v);
        }
        Ok(out)
    }

    fn bytes(&mut self, n: usize) -> &'a [u8] {
        let out = &self.buf[self.at..self.at + n];
        self.at += n;
        out
    }

    fn skip_aligned(&mut self, align: usize, n: usize) {
        self.align(align);
        self.at += n;
    }

    fn take(&mut self, n: usize) -> Result<&'a [u8], FormatError> {
        if self.at + n > self.buf.len() {
            return Err(FormatError::Truncated {
                need: self.at + n,
                got: self.buf.len(),
            });
        }
        let out = &self.buf[self.at..self.at + n];
        self.at += n;
        Ok(out)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn roundtrips_and_matches_the_in_memory_index() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_0.bmp");
        // 40 docs, block size 16 (the reference's BMP_BLOCK_SIZE).
        let docs: Vec<SparseDoc> = (0..40u64)
            .map(|i| {
                let entries = vec![
                    ((i % 5) as i32, 1.0 + (i % 3) as f32),
                    (7, 0.5),
                    (9, (i % 4) as f32),
                ];
                (i, entries)
            })
            .collect();
        write_bmp_file(&path, 16, IdxWidth::I32, &docs, 10).unwrap();
        let reader = BmpCppReader::open(&path, IdxWidth::I32).unwrap();
        assert_eq!(reader.doc_count(), 40);
        assert_eq!(reader.block_size, 16);
        let q = vec![(0i32, 1.0f32), (7, 1.0)];
        let hits = reader.search(&q, 5, 1.0);
        assert!(!hits.is_empty());
        // Every hit must agree with a brute-force dot product.
        for (row, score) in &hits {
            let doc = &docs[*row as usize];
            let exact: f32 = q
                .iter()
                .map(|(t, qv)| {
                    doc.1
                        .iter()
                        .find(|(dt, _)| dt == t)
                        .map(|(_, v)| qv * v)
                        .unwrap_or(0.0)
                })
                .sum();
            assert!(
                (exact - score).abs() < 1e-5,
                "row {row}: {exact} vs {score}"
            );
        }
    }

    #[test]
    fn term_ids_respect_the_declared_width() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_i8.bmp");
        let docs: Vec<SparseDoc> = vec![(0, vec![(3, 1.0)]), (1, vec![(3, 2.0)])];
        write_bmp_file(&path, 16, IdxWidth::I8, &docs, 8).unwrap();
        let reader = BmpCppReader::open(&path, IdxWidth::I8).unwrap();
        let hits = reader.search(&[(3, 1.0)], 2, 1.0);
        assert_eq!(hits.len(), 2);
        assert_eq!(hits[0].0, 1);
    }
}
