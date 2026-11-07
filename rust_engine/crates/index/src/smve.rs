//! SMVE: multi-vector → sparse-vector index.
//!
//! C++: `SMVETransform` / `GenerateProjectionMatrix`
//! (`src/storage/knn_index/smve/smve_transform.cppm`) and `SMVEIndexInMem`
//! (`smve_index.cppm`). A random unit-column projection turns each token into a
//! sparse top-k code; query codes are summed, document codes averaged, and the
//! documents are searched with the BMP (block-max) index.

use common::{Error, Result};

use crate::sparse::{BmpAlg, BmpSearchOptions};

/// C++: `SMVEResult`.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct SmveResult {
    pub indices: Vec<u32>,
    pub values: Vec<f32>,
}

/// `std::mt19937` (the reference seeds it with 42).
///
/// The generator itself is standard; what matters is that the reference's
/// sequence is reproduced bit for bit, because the projection matrix is part
/// of the on-disk chunk.
struct Mt19937 {
    state: [u32; 624],
    at: usize,
}

impl Mt19937 {
    fn new(seed: u32) -> Self {
        let mut state = [0u32; 624];
        state[0] = seed;
        for i in 1..624 {
            state[i] = 1_812_433_253u32
                .wrapping_mul(state[i - 1] ^ (state[i - 1] >> 30))
                .wrapping_add(i as u32);
        }
        Self { state, at: 624 }
    }

    fn twist(&mut self) {
        for i in 0..624 {
            let y = (self.state[i] & 0x8000_0000) | (self.state[(i + 1) % 624] & 0x7fff_ffff);
            let mut next = self.state[(i + 397) % 624] ^ (y >> 1);
            if y & 1 != 0 {
                next ^= 0x9908_b0df;
            }
            self.state[i] = next;
        }
        self.at = 0;
    }

    fn next_u32(&mut self) -> u32 {
        if self.at >= 624 {
            self.twist();
        }
        let mut y = self.state[self.at];
        self.at += 1;
        y ^= y >> 11;
        y ^= (y << 7) & 0x9d2c_5680;
        y ^= (y << 15) & 0xefc6_0000;
        y ^= y >> 18;
        y
    }
}

/// `std::generate_canonical<float, 24>` over [`Mt19937`]: with a 32-bit
/// generator and 24 mantissa bits libstdc++ takes one draw and scales it by
/// `2^-32` in `float` arithmetic.
fn canonical(rng: &mut Mt19937) -> f32 {
    (rng.next_u32() as f32) / 4_294_967_296.0f32
}

/// `std::normal_distribution<float>(0, 1)`: the polar (Marsaglia) method, with
/// the second value cached, as libstdc++ implements it.
struct Normal {
    rng: Mt19937,
    saved: Option<f32>,
}

impl Normal {
    fn new(seed: u32) -> Self {
        Self {
            rng: Mt19937::new(seed),
            saved: None,
        }
    }

    fn next(&mut self) -> f32 {
        if let Some(v) = self.saved.take() {
            return v;
        }
        loop {
            let x = 2.0f32 * canonical(&mut self.rng) - 1.0f32;
            let y = 2.0f32 * canonical(&mut self.rng) - 1.0f32;
            let r2 = x * x + y * y;
            if r2 > 1.0 || r2 == 0.0 {
                continue;
            }
            let mult = (-2.0 * r2.ln() / r2).sqrt();
            self.saved = Some(x * mult);
            return y * mult;
        }
    }
}

/// C++: `GenerateProjectionMatrix`.
///
/// `dim × width`, each column normalized to unit length, drawn from
/// `std::mt19937(42)` through `std::normal_distribution<float>` — the same
/// sequence and the same `float` arithmetic, because the matrix is written
/// into the index chunk.
pub fn generate_projection_matrix(dim: usize, width: usize) -> Vec<f32> {
    let mut normal = Normal::new(42);
    let mut mat = vec![0f32; width * dim];
    for j in 0..width {
        let mut sum_sq = 0.0f32;
        for i in 0..dim {
            let val = normal.next();
            mat[j * dim + i] = val;
            sum_sq += val * val;
        }
        let norm = sum_sq.sqrt();
        if norm > 1e-12 {
            for i in 0..dim {
                mat[j * dim + i] /= norm;
            }
        }
    }
    mat
}

/// C++: `SMVETransform`.
///
/// Projects every token, keeps its `topk` largest coordinates, then pools:
/// sum for a query, average of the non-zero values for a document.
pub fn smve_transform(
    tensor: &[f32],
    n_tokens: usize,
    dim: usize,
    projection_matrix: &[f32],
    width: usize,
    topk: usize,
    is_query: bool,
) -> Result<SmveResult> {
    if dim == 0 || width == 0 {
        return Err(Error::Invalid("SMVETransform: empty dimension".into()));
    }
    if tensor.len() < n_tokens * dim || projection_matrix.len() < width * dim {
        return Err(Error::Invalid("SMVETransform: buffer too short".into()));
    }
    let mut sparse = vec![0f32; n_tokens * width];
    let keep = topk.min(width);
    for t in 0..n_tokens {
        let token = &tensor[t * dim..(t + 1) * dim];
        // A token keeps its `topk` best-scoring columns out of `width`, and
        // `width` is 2048 by default. Sorting all of them per token put an
        // O(width log width) sort on the build for a result of `topk` entries;
        // a bounded insertion keeps the same set in O(width * topk).
        let mut best: Vec<(f32, u32)> = Vec::with_capacity(keep + 1);
        let mut worst = f32::NEG_INFINITY;
        for j in 0..width {
            let col = &projection_matrix[j * dim..(j + 1) * dim];
            // SIMD inner product: this is `n_tokens * width` calls per build
            // (2048 columns by default), the whole cost of `smve_transform`.
            let score = crate::simd::ip(token, col);
            if best.len() == keep && score <= worst {
                continue;
            }
            let at = best.partition_point(|(s, _)| *s >= score);
            best.insert(at, (score, j as u32));
            if best.len() > keep {
                best.pop();
                worst = best.last().map(|(s, _)| *s).unwrap_or(f32::NEG_INFINITY);
            }
        }
        for &(score, idx) in best.iter() {
            sparse[t * width + idx as usize] = score;
        }
    }
    let mut indices = Vec::new();
    let mut values = Vec::new();
    if is_query {
        for j in 0..width {
            let sum: f32 = (0..n_tokens).map(|t| sparse[t * width + j]).sum();
            if sum.abs() > 1e-12 {
                indices.push(j as u32);
                values.push(sum);
            }
        }
    } else {
        for j in 0..width {
            let mut sum = 0f32;
            let mut cnt = 0usize;
            for t in 0..n_tokens {
                let v = sparse[t * width + j];
                if v.abs() > 1e-12 {
                    sum += v;
                    cnt += 1;
                }
            }
            if cnt > 0 {
                indices.push(j as u32);
                values.push(sum / cnt as f32);
            }
        }
    }
    Ok(SmveResult { indices, values })
}

/// C++: `SMVEIndexInMem` (the in-memory build and search path).
pub struct SmveIndex {
    width: u32,
    topk: u32,
    embedding_dim: u32,
    projection_matrix: Vec<f32>,
    bmp: BmpAlg,
    /// The sparse code of each document, in insertion order: the BMP writer
    /// needs the documents, and the chunk carries them.
    docs: Vec<(u32, Vec<(u32, f32)>)>,
    /// Row id of each BMP document id.
    labels: Vec<u32>,
    built: bool,
}

impl SmveIndex {
    /// C++: `SMVEIndexInMem(begin_row_id, index_base, column_def)`.
    pub fn new(width: u32, topk: u32, embedding_dim: u32) -> Self {
        Self {
            width,
            topk,
            embedding_dim,
            projection_matrix: generate_projection_matrix(embedding_dim as usize, width as usize),
            bmp: BmpAlg::new(crate::sparse::BMP_BLOCK_SIZE),
            docs: Vec::new(),
            labels: Vec::new(),
            built: false,
        }
    }

    pub fn width(&self) -> u32 {
        self.width
    }
    pub fn topk(&self) -> u32 {
        self.topk
    }
    pub fn embedding_dim(&self) -> u32 {
        self.embedding_dim
    }
    pub fn row_count(&self) -> usize {
        self.labels.len()
    }

    /// C++: `SMVEIndexInMem::BuildFromColumn` — the sparse code of each
    /// document is its average-pooled SMVE transform.
    pub fn build_from_column(&mut self, docs: &[(u32, Vec<Vec<f32>>)]) -> Result<usize> {
        // `add_document` rebuilds the BMP so the incremental insert path can
        // query right after one row; a bulk build must not pay that per row -
        // 20k rows of full rebuilds was 4.2 s, the whole SMVE build.
        //
        // The transform only reads the projection matrix, so the documents are
        // independent: encode them across the available parallelism and add
        // the codes in order, which keeps the BMP's document ids stable.
        let threads = std::thread::available_parallelism()
            .map(|n| n.get())
            .unwrap_or(1);
        let encoded: Vec<Result<Vec<(u32, f32)>>> = if threads <= 1 || docs.len() < 4096 {
            docs.iter()
                .map(|(_, tokens)| self.encode_tokens(tokens))
                .collect()
        } else {
            let chunk = docs.len().div_ceil(threads);
            let index = &*self;
            let mut slots: Vec<Option<Result<Vec<(u32, f32)>>>> =
                (0..docs.len()).map(|_| None).collect();
            std::thread::scope(|scope| {
                let mut rest: &mut [Option<Result<Vec<(u32, f32)>>>] = &mut slots;
                let mut start = 0usize;
                let mut handles = Vec::new();
                while start < docs.len() {
                    let end = (start + chunk).min(docs.len());
                    let (mine, tail) = rest.split_at_mut(end - start);
                    rest = tail;
                    handles.push(scope.spawn(move || {
                        for (offset, slot) in mine.iter_mut().enumerate() {
                            *slot = Some(index.encode_tokens(&docs[start + offset].1));
                        }
                    }));
                    start = end;
                }
                for h in handles {
                    h.join().unwrap();
                }
            });
            slots
                .into_iter()
                .map(|s| s.expect("every slot is filled"))
                .collect()
        };
        for ((row_id, _), toks) in docs.iter().zip(encoded) {
            let toks = toks?;
            self.bmp.add_doc(&toks);
            self.docs.push((*row_id, toks));
            self.labels.push(*row_id);
        }
        self.bmp.build();
        self.built = true;
        Ok(self.labels.len())
    }

    /// The SMVE sparse code of one document's tokens.
    fn encode_tokens(&self, tokens: &[Vec<f32>]) -> Result<Vec<(u32, f32)>> {
        let flat: Vec<f32> = tokens.iter().flatten().copied().collect();
        let smve = smve_transform(
            &flat,
            tokens.len(),
            self.embedding_dim as usize,
            &self.projection_matrix,
            self.width as usize,
            self.topk as usize,
            false,
        )?;
        Ok(smve
            .indices
            .into_iter()
            .zip(smve.values)
            .filter(|(_, v)| *v != 0.0)
            .collect())
    }

    /// One row of `BuildFromColumn` (also the incremental insert path).
    pub fn add_document(&mut self, row_id: u32, tokens: &[Vec<f32>]) -> Result<()> {
        let flat: Vec<f32> = tokens.iter().flatten().copied().collect();
        let smve = smve_transform(
            &flat,
            tokens.len(),
            self.embedding_dim as usize,
            &self.projection_matrix,
            self.width as usize,
            self.topk as usize,
            false,
        )?;
        let entries: Vec<(u32, f32)> = smve
            .indices
            .into_iter()
            .zip(smve.values)
            .filter(|(_, v)| *v != 0.0)
            .collect();
        self.bmp.add_doc(&entries);
        self.docs.push((row_id, entries));
        self.labels.push(row_id);
        self.bmp.build();
        self.built = true;
        Ok(())
    }

    /// C++: `SMVEIndexInMem::Search`.
    ///
    /// The query transform is sum-pooled; BMP returns `top_k * overfetch`
    /// candidates, the bitmask filters them, and the best `top_k` are kept.
    pub fn search(
        &self,
        query: &[Vec<f32>],
        top_k: usize,
        overfetch: usize,
        allowed: Option<&[bool]>,
    ) -> (Vec<f32>, Vec<u32>) {
        if !self.built || top_k == 0 {
            return (Vec::new(), Vec::new());
        }
        let flat: Vec<f32> = query.iter().flatten().copied().collect();
        let Ok(smve) = smve_transform(
            &flat,
            query.len(),
            self.embedding_dim as usize,
            &self.projection_matrix,
            self.width as usize,
            self.topk as usize,
            true,
        ) else {
            return (Vec::new(), Vec::new());
        };
        if smve.indices.is_empty() {
            return (Vec::new(), Vec::new());
        }
        let query: Vec<(u32, f32)> = smve.indices.into_iter().zip(smve.values).collect();
        let candidate = top_k.saturating_mul(overfetch.max(1));
        let options = BmpSearchOptions {
            alpha: 1.0,
            beta: 1.0,
            use_tail: true,
        };
        // Translate the row-id filter to BMP document ids.
        let bmp_allowed: Option<Vec<bool>> = allowed.map(|a| {
            self.labels
                .iter()
                .map(|label| a.get(*label as usize).copied().unwrap_or(false))
                .collect()
        });
        let mut hits: Vec<(u32, f32)> = self
            .bmp
            .search_knn_filtered(&query, candidate, options, bmp_allowed.as_deref())
            .into_iter()
            .filter_map(|(doc, score)| self.labels.get(doc).map(|label| (*label, score)))
            .collect();
        hits.sort_by(|a, b| b.1.total_cmp(&a.1).then_with(|| a.0.cmp(&b.0)));
        hits.truncate(top_k);
        (
            hits.iter().map(|(_, score)| *score).collect(),
            hits.iter().map(|(id, _)| *id).collect(),
        )
    }

    /// C++: `SMVEIndexInMem::GetSizeInBytes`.
    pub fn size_in_bytes(&self) -> usize {
        std::mem::size_of::<Self>()
            + self.width as usize * self.embedding_dim as usize * 4
            + self.labels.len() * 4
            + self.bmp.len() * 16
    }

    /// The `dim × width` projection matrix (the reference's own sequence).
    pub fn projection(&self) -> &[f32] {
        &self.projection_matrix
    }

    /// Each document's sparse code, in insertion order: `(row id, [(term, value)])`.
    pub fn docs(&self) -> &[(u32, Vec<(u32, f32)>)] {
        &self.docs
    }

    /// C++: `SMVEIndexInMem::GetChunkIndexMetaInfo` (row count part).
    pub fn chunk_row_count(&self) -> u32 {
        self.labels.len() as u32
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn docs(n: usize) -> Vec<(u32, Vec<Vec<f32>>)> {
        let mut state = 7u64;
        (0..n)
            .map(|i| {
                let tokens = (0..4)
                    .map(|_| {
                        (0..8)
                            .map(|_| {
                                state ^= state << 13;
                                state ^= state >> 7;
                                state ^= state << 17;
                                ((state >> 40) as f32 / (1u64 << 24) as f32) - 0.5
                            })
                            .collect()
                    })
                    .collect();
                (i as u32, tokens)
            })
            .collect()
    }

    #[test]
    fn projection_columns_are_unit_vectors() {
        let mat = generate_projection_matrix(8, 4);
        assert_eq!(mat.len(), 32);
        for j in 0..4 {
            let norm: f32 = mat[j * 8..(j + 1) * 8].iter().map(|v| v * v).sum();
            assert!((norm - 1.0).abs() < 1e-5, "{norm}");
        }
        // Deterministic: the same matrix is regenerated for queries.
        assert_eq!(mat, generate_projection_matrix(8, 4));
    }

    #[test]
    fn query_codes_are_sum_pooled_and_docs_averaged() {
        let projection = generate_projection_matrix(4, 8);
        let tokens = [[1.0f32, 0.0, 0.0, 0.0], [0.0, 1.0, 0.0, 0.0]];
        let flat: Vec<f32> = tokens.iter().flatten().copied().collect();
        let q = smve_transform(&flat, 2, 4, &projection, 8, 4, true).unwrap();
        let d = smve_transform(&flat, 2, 4, &projection, 8, 4, false).unwrap();
        assert!(q.indices.len() <= 8 && !q.indices.is_empty());
        // Averaging divides each per-token non-zero by its count.
        for (idx, value) in d.indices.iter().zip(&d.values) {
            if let Some(pos) = q.indices.iter().position(|x| x == idx) {
                assert!(value.abs() <= q.values[pos].abs() + 1e-5);
            }
        }
    }

    #[test]
    fn search_returns_the_closest_document_and_honours_the_filter() {
        let data = docs(40);
        let mut index = SmveIndex::new(64, 8, 8);
        index.build_from_column(&data).unwrap();
        assert_eq!(index.row_count(), 40);
        let query = data[7].1.clone();
        let (scores, ids) = index.search(&query, 3, 4, None);
        assert_eq!(ids.len(), 3);
        assert_eq!(ids[0], 7);
        assert!(scores[0] >= scores[1]);
        let mut allowed = vec![false; 40];
        allowed[5] = true;
        let (_, ids) = index.search(&query, 3, 4, Some(&allowed));
        assert_eq!(ids, vec![5]);
        assert!(index.size_in_bytes() > 0);
    }

    /// The projection matrix is written into the chunk, so it has to be the
    /// reference's sequence: `std::mt19937(42)` through
    /// `std::normal_distribution<float>` (polar method, second value cached),
    /// with the same `float` arithmetic. Expected values taken from the
    /// reference's own `chunk_0.idx` for a `TENSOR(FLOAT,4)` column.
    #[test]
    fn the_projection_matrix_matches_the_reference_sequence() {
        let matrix = generate_projection_matrix(4, 2048);
        let expected = [
            0.701173961f32,
            -0.296649039,
            0.499022365,
            0.413921654,
            0.618849814,
            0.630404711,
            -0.0728976876,
            -0.462926388,
        ];
        for (i, want) in expected.iter().enumerate() {
            assert_eq!(matrix[i], *want, "element {i}");
        }
    }
}
