//! SMVE: multi-vector → sparse-vector index.
//!
//! C++: `SMVETransform` / `GenerateProjectionMatrix`
//! (`src/storage/knn_index/smve/smve_transform.cppm`) and `SMVEIndexInMem`
//! (`smve_index.cppm`). A random unit-column projection turns each token into a
//! sparse top-k code; query codes are summed, document codes averaged, and the
//! documents are searched with the BMP (block-max) index.

use infinity_common::{Error, Result};

use crate::sparse::{BmpAlg, BmpSearchOptions};

/// C++: `SMVEResult`.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct SmveResult {
    pub indices: Vec<u32>,
    pub values: Vec<f32>,
}

/// C++: `GenerateProjectionMatrix`.
///
/// `dim × width` row-major, each column normalized to unit length. The
/// reference seeds `std::mt19937` with 42; the deterministic xorshift here
/// plays the same role (the matrix only has to be reproducible within a build
/// and shared by writes and queries, which it is).
pub fn generate_projection_matrix(dim: usize, width: usize) -> Vec<f32> {
    let mut state = 42u64;
    let mut mat = vec![0f32; width * dim];
    for j in 0..width {
        let mut sum_sq = 0.0f32;
        for i in 0..dim {
            // Box-Muller on two xorshift draws.
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            let u = (((state >> 11) as f64) / ((1u64 << 53) as f64)).max(1e-12);
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            let v = ((state >> 11) as f64) / ((1u64 << 53) as f64);
            let val = ((-2.0 * u.ln()).sqrt() * (2.0 * std::f64::consts::PI * v).cos()) as f32;
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
    for t in 0..n_tokens {
        let token = &tensor[t * dim..(t + 1) * dim];
        let mut scored: Vec<(f32, u32)> = (0..width)
            .map(|j| {
                let col = &projection_matrix[j * dim..(j + 1) * dim];
                let score: f32 = token.iter().zip(col).map(|(a, b)| a * b).sum();
                (score, j as u32)
            })
            .collect();
        scored.sort_by(|a, b| b.0.total_cmp(&a.0));
        for &(score, idx) in scored.iter().take(topk.min(width)) {
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
        for (row_id, tokens) in docs {
            self.add_document(*row_id, tokens)?;
        }
        self.bmp.build();
        self.built = true;
        Ok(self.labels.len())
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
}
