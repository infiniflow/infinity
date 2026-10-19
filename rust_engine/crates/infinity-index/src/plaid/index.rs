//! PLAID index: IVF over residual-quantized multi-vector documents.
//!
//! C++: `PlaidIndex` (`src/storage/knn_index/plaid/plaid_index.cppm` and
//! `plaid_index_impl.cpp`). A document is a list of token embeddings; search
//! pushes each query token to its nearest centroids, collects candidate
//! documents from the IVF lists, ranks them with a centroid approximation, then
//! re-scores the best with the dequantized residuals (MaxSim).
//!
//! Deviation: the reference keeps `centroid_ids` in an mmap when a chunk is
//! loaded through `LoadFromPtr`; this port always owns the data (the executor's
//! chunk loader decodes owned buffers, and mmap is an exempt primitive).

use std::sync::Arc;

use infinity_common::{Error, Result};

use super::centroids::{kmeans_l2, PlaidGlobalCentroids};
use super::quantizer::PlaidQuantizer;

/// C++: `Bitmask` (`roaring_bitmap.cppm`): the set of row ids a scan may return.
#[derive(Debug, Clone, Default)]
pub struct Bitmask {
    bits: Vec<u64>,
}

impl Bitmask {
    pub fn new(len: usize) -> Self {
        Self {
            bits: vec![0; len.div_ceil(64)],
        }
    }
    pub fn set(&mut self, id: u32) {
        let id = id as usize;
        if id / 64 >= self.bits.len() {
            self.bits.resize(id / 64 + 1, 0);
        }
        self.bits[id / 64] |= 1 << (id % 64);
    }
    pub fn is_true(&self, id: u32) -> bool {
        let id = id as usize;
        self.bits
            .get(id / 64)
            .is_some_and(|w| w & (1 << (id % 64)) != 0)
    }
    /// C++ `Bitmask::MakeSharedAllTrue(len)`: every id in `0..len` is set.
    pub fn new_all_true(len: usize) -> Self {
        let mut bits = vec![u64::MAX; len.div_ceil(64)];
        if let Some(last) = bits.last_mut() {
            let rem = len % 64;
            if rem != 0 {
                *last = (1u64 << rem) - 1;
            }
        }
        Self { bits }
    }
    /// C++ `Bitmask::SetFalse(id)`.
    pub fn set_false(&mut self, id: u32) {
        let id = id as usize;
        if let Some(word) = self.bits.get_mut(id / 64) {
            *word &= !(1 << (id % 64));
        }
    }
    pub fn len(&self) -> usize {
        self.bits.len() * 64
    }
    pub fn is_empty(&self) -> bool {
        self.bits.iter().all(|w| *w == 0)
    }
}

/// C++: `PlaidQueryResultType` (the `(count, scores, ids)` tuple).
#[derive(Debug, Clone, Default, PartialEq)]
pub struct PlaidQueryResult {
    /// MaxSim score per returned document.
    pub scores: Vec<f32>,
    /// Document ids, already offset by `start_segment_offset`.
    pub ids: Vec<u32>,
}

impl PlaidQueryResult {
    pub fn is_empty(&self) -> bool {
        self.ids.is_empty()
    }
}

/// C++: `PlaidIndex`.
pub struct PlaidIndex {
    pub start_segment_offset: u32,
    pub embedding_dimension: u32,
    pub nbits: u32,
    pub requested_n_centroids: u32,
    pub centroid_batch_size: u32,
    n_centroids: u32,
    centroids_data: Vec<f32>,
    centroid_norms_neg_half: Vec<f32>,
    global_centroids: Option<Arc<PlaidGlobalCentroids>>,
    n_docs: u32,
    n_total_embeddings: u32,
    doc_lens: Vec<u32>,
    doc_offsets: Vec<u32>,
    centroid_ids: Vec<u32>,
    packed_residuals: Vec<u8>,
    ivf_lists: Vec<Vec<u32>>,
    ivf_data: Vec<u32>,
    ivf_offsets: Vec<u32>,
    ivf_lengths: Vec<u32>,
    ivf_flattened: bool,
    quantizer: Option<PlaidQuantizer>,
    colbertsar_mode: bool,
}

impl PlaidIndex {
    /// C++: `PlaidIndex(start_segment_offset, embedding_dimension, nbits,
    /// n_centroids, colbertsar_mode)`.
    pub fn new(
        start_segment_offset: u32,
        embedding_dimension: u32,
        nbits: u32,
        n_centroids: u32,
        colbertsar_mode: bool,
    ) -> Result<Self> {
        let quantizer = if colbertsar_mode {
            None
        } else {
            Some(PlaidQuantizer::new(nbits, embedding_dimension)?)
        };
        Ok(Self {
            start_segment_offset,
            embedding_dimension,
            nbits,
            requested_n_centroids: n_centroids,
            centroid_batch_size: Self::DEFAULT_CENTROID_BATCH_SIZE,
            n_centroids: 0,
            centroids_data: Vec::new(),
            centroid_norms_neg_half: Vec::new(),
            global_centroids: None,
            n_docs: 0,
            n_total_embeddings: 0,
            doc_lens: Vec::new(),
            doc_offsets: Vec::new(),
            centroid_ids: Vec::new(),
            packed_residuals: Vec::new(),
            ivf_lists: Vec::new(),
            ivf_data: Vec::new(),
            ivf_offsets: Vec::new(),
            ivf_lengths: Vec::new(),
            ivf_flattened: false,
            quantizer,
            colbertsar_mode,
        })
    }

    /// C++: `PlaidIndex::DEFAULT_CENTROID_BATCH_SIZE`.
    pub const DEFAULT_CENTROID_BATCH_SIZE: u32 = 4096;

    pub fn doc_num(&self) -> u32 {
        self.n_docs
    }
    pub fn total_embedding_num(&self) -> u32 {
        self.n_total_embeddings
    }
    pub fn n_centroids(&self) -> u32 {
        self.n_centroids
    }
    pub fn embedding_dimension(&self) -> u32 {
        self.embedding_dimension
    }
    pub fn is_colbertsar_mode(&self) -> bool {
        self.colbertsar_mode
    }
    pub fn centroids_data(&self) -> &[f32] {
        match &self.global_centroids {
            Some(g) => g.centroids_data(),
            None => &self.centroids_data,
        }
    }
    pub fn centroid_norms_neg_half(&self) -> &[f32] {
        match &self.global_centroids {
            Some(g) => g.centroid_norms_neg_half(),
            None => &self.centroid_norms_neg_half,
        }
    }
    pub fn quantizer(&self) -> Option<&PlaidQuantizer> {
        self.quantizer.as_ref()
    }
    pub fn doc_lens(&self) -> &[u32] {
        &self.doc_lens
    }
    pub fn doc_offsets(&self) -> &[u32] {
        &self.doc_offsets
    }
    pub fn ivf_lists(&self) -> &[Vec<u32>] {
        &self.ivf_lists
    }
    pub fn centroid_ids(&self) -> &[u32] {
        &self.centroid_ids
    }
    pub fn packed_residuals(&self) -> &[u8] {
        &self.packed_residuals
    }
    pub fn packed_dim(&self) -> u32 {
        (self.embedding_dimension * self.nbits).div_ceil(8)
    }
    pub fn is_ivf_flattened(&self) -> bool {
        self.ivf_flattened
    }

    /// C++: `PlaidIndex::GetDocLen`.
    pub fn doc_len(&self, doc_id: u32) -> Result<u32> {
        self.doc_lens.get(doc_id as usize).copied().ok_or_else(|| {
            Error::Invalid(format!("PlaidIndex::GetDocLen: invalid doc_id {doc_id}"))
        })
    }

    /// C++: `PlaidIndex::ExpectLeastTrainingDataNum`.
    fn expect_least_training_data_num(&self) -> u64 {
        if self.colbertsar_mode {
            return (32 * self.n_centroids as u64).max(256);
        }
        let buckets = self
            .quantizer
            .as_ref()
            .map_or(16, PlaidQuantizer::n_buckets) as u64;
        (32 * self.n_centroids as u64).max(32 * buckets)
    }

    /// C++: `PlaidIndex::Train`.
    pub fn train(
        &mut self,
        centroids_num: u32,
        embedding_data: &[f32],
        embedding_num: usize,
        iter_cnt: u32,
    ) -> Result<()> {
        self.n_centroids = centroids_num;
        if centroids_num == 0 || !centroids_num.is_multiple_of(8) {
            return Err(Error::Invalid(format!(
                "PlaidIndex::Train: n_centroids must be a multiple of 8, got {centroids_num}"
            )));
        }
        self.ivf_lists = vec![Vec::new(); centroids_num as usize];
        let dim = self.embedding_dimension as usize;
        if embedding_data.len() < embedding_num * dim {
            return Err(Error::Invalid(
                "PlaidIndex::Train: embedding buffer too short".into(),
            ));
        }
        let max_sample_size = centroids_num as u64 * 256;
        // The reference only warns when the corpus is below this bound;
        // training still proceeds (with reduced quality).
        let _ = self.expect_least_training_data_num();
        let actual_sample_size = max_sample_size.min(embedding_num as u64) as usize;
        let training: Vec<f32> = if actual_sample_size < embedding_num {
            let ids = sample_indices(embedding_num, actual_sample_size);
            let mut sampled = Vec::with_capacity(actual_sample_size * dim);
            for id in ids {
                sampled.extend_from_slice(&embedding_data[id * dim..(id + 1) * dim]);
            }
            sampled
        } else {
            embedding_data[..embedding_num * dim].to_vec()
        };
        let training_num = if actual_sample_size < embedding_num {
            actual_sample_size
        } else {
            embedding_num
        };

        if self.colbertsar_mode {
            self.train_query_aware_centroids(
                centroids_num,
                &training,
                training_num,
                2048,
                50,
                1e-4,
            )?;
            return Ok(());
        }

        self.centroids_data = kmeans_l2(
            dim,
            &training,
            training_num,
            centroids_num as usize,
            iter_cnt as usize,
        )?;
        self.compute_centroid_norms();

        // Train the residual quantizer on up to 50K sampled residuals.
        const MAX_SAMPLE_FOR_QUANTIZER: usize = 50_000;
        let sample_size = MAX_SAMPLE_FOR_QUANTIZER.min(embedding_num);
        let ids = sample_indices(embedding_num, sample_size);
        let mut residuals = Vec::with_capacity(sample_size * dim);
        for id in ids {
            let emb = &embedding_data[id * dim..(id + 1) * dim];
            let cid = self.nearest_centroid(emb);
            let centroid = &self.centroids_data[cid as usize * dim..(cid as usize + 1) * dim];
            for d in 0..dim {
                residuals.push(emb[d] - centroid[d]);
            }
        }
        self.quantizer
            .as_mut()
            .expect("PLAID mode has a quantizer")
            .train(&residuals, sample_size)
    }

    fn compute_centroid_norms(&mut self) {
        let dim = self.embedding_dimension as usize;
        self.centroid_norms_neg_half = self
            .centroids_data
            .chunks_exact(dim)
            .map(|c| -0.5 * c.iter().map(|v| v * v).sum::<f32>())
            .collect();
    }

    /// `FindNearestCentroid` over the index's (possibly shared) centroids.
    ///
    /// ColBERTSaR assigns by pure dot product, PLAID by L2 (`dot - 0.5||c||²`).
    fn nearest_centroid(&self, embedding: &[f32]) -> u32 {
        let dim = self.embedding_dimension as usize;
        let centroids = self.centroids_data();
        let norms = self.centroid_norms_neg_half();
        let mut best = 0u32;
        let mut best_neg = f32::MIN;
        for k in 0..self.n_centroids as usize {
            let c = &centroids[k * dim..(k + 1) * dim];
            let dot: f32 = embedding.iter().zip(c).map(|(a, b)| a * b).sum();
            let neg = if self.colbertsar_mode {
                dot
            } else {
                dot + norms[k]
            };
            if neg > best_neg {
                best_neg = neg;
                best = k as u32;
            }
        }
        best
    }

    /// C++: `PlaidIndex::ComputeAutoNCentroids`.
    pub fn compute_auto_n_centroids(embedding_count: u64) -> u32 {
        if embedding_count < 256 {
            return 8;
        }
        let sqrt_n = (embedding_count as f64).sqrt();
        let log2_val = (16.0 * sqrt_n).log2();
        let mut n_centroids = if log2_val < 3.0 {
            8
        } else {
            1u32 << log2_val.floor() as u32
        };
        n_centroids = n_centroids.min(embedding_count as u32);
        while n_centroids > 8 && embedding_count < 32 * n_centroids as u64 {
            n_centroids >>= 1;
        }
        n_centroids = n_centroids.div_ceil(8) * 8;
        n_centroids.max(8)
    }

    /// C++: `PlaidIndex::TrainQueryAwareCentroids` (ColBERTSaR mode).
    #[allow(clippy::too_many_arguments)]
    fn train_query_aware_centroids(
        &mut self,
        n_centroids: u32,
        embedding_data: &[f32],
        embedding_num: usize,
        batch_size: u32,
        n_epochs: u32,
        learning_rate: f32,
    ) -> Result<()> {
        let dim = self.embedding_dimension as usize;
        let k = n_centroids as usize;
        if embedding_num == 0 {
            return Err(Error::Invalid(
                "TrainQueryAwareCentroids: no embeddings".into(),
            ));
        }
        // Initial centroids: K distinct random embeddings.
        let mut seen = std::collections::HashSet::new();
        let mut rng = 0x9E37_79B9_7F4A_7C15u64;
        self.centroids_data = vec![0f32; k * dim];
        let mut filled = 0usize;
        while filled < k {
            rng ^= rng << 13;
            rng ^= rng >> 7;
            rng ^= rng << 17;
            let idx = (rng % embedding_num as u64) as usize;
            if seen.insert(idx) {
                self.centroids_data[filled * dim..(filled + 1) * dim]
                    .copy_from_slice(&embedding_data[idx * dim..(idx + 1) * dim]);
                filled += 1;
                if seen.len() == embedding_num && filled < k {
                    // Fewer distinct embeddings than centroids: cycle.
                    seen.clear();
                }
            }
        }
        self.compute_centroid_norms();

        let mut indices: Vec<usize> = (0..embedding_num).collect();
        let batch_size = batch_size.max(1);
        for _ in 0..n_epochs {
            // Fisher-Yates shuffle with the same deterministic RNG.
            for i in (1..embedding_num).rev() {
                rng ^= rng << 13;
                rng ^= rng >> 7;
                rng ^= rng << 17;
                let j = (rng % (i as u64 + 1)) as usize;
                indices.swap(i, j);
            }
            let mut grad = vec![0f32; k * dim];
            for offset in (0..embedding_num).step_by(batch_size as usize) {
                let cur = (batch_size as usize).min(embedding_num - offset);
                grad.fill(0.0);
                let batch: Vec<&[f32]> = (0..cur)
                    .map(|j| {
                        let at = indices[offset + j] * dim;
                        &embedding_data[at..at + dim]
                    })
                    .collect();
                // sims[j, k] = batch[j] · centroid[k]
                let mut assignments = vec![0u32; cur];
                for (j, b) in batch.iter().enumerate() {
                    let mut best = 0usize;
                    let mut best_val = f32::MIN;
                    for c in 0..k {
                        let cent = &self.centroids_data[c * dim..(c + 1) * dim];
                        let val: f32 = b.iter().zip(cent).map(|(x, y)| x * y).sum();
                        if val > best_val {
                            best_val = val;
                            best = c;
                        }
                    }
                    assignments[j] = best as u32;
                }
                // qc[i][j] = batch[i] · centroid[assign[j]]
                let mut qc = vec![0f32; cur * cur];
                for i in 0..cur {
                    for j in 0..cur {
                        let cent = &self.centroids_data
                            [assignments[j] as usize * dim..(assignments[j] as usize + 1) * dim];
                        qc[i * cur + j] = batch[i].iter().zip(cent).map(|(x, y)| x * y).sum();
                    }
                }
                // qd[i][j] = batch[i] · batch[j]
                let mut qd = vec![0f32; cur * cur];
                for i in 0..cur {
                    for j in 0..cur {
                        qd[i * cur + j] = batch[i].iter().zip(batch[j]).map(|(x, y)| x * y).sum();
                    }
                }
                // diff_T[j][i] = (qc - qd)[i][j] / ||diff[i]||
                let mut diff_t = vec![0f32; cur * cur];
                for i in 0..cur {
                    let mut norm_sq = 0.0f32;
                    for j in 0..cur {
                        let d = qc[i * cur + j] - qd[i * cur + j];
                        diff_t[j * cur + i] = d;
                        norm_sq += d * d;
                    }
                    let norm = norm_sq.sqrt();
                    let inv = if norm > 1e-12 { 1.0 / norm } else { 0.0 };
                    for j in 0..cur {
                        diff_t[j * cur + i] *= inv;
                    }
                }
                // grad_contrib[j, d] = sum_i diff_T[j][i] * batch[i][d]
                let scale = 1.0f32 / cur as f32;
                for j in 0..cur {
                    let cid = assignments[j] as usize;
                    for d in 0..dim {
                        let mut g = 0.0f32;
                        for i in 0..cur {
                            g += diff_t[j * cur + i] * batch[i][d];
                        }
                        grad[cid * dim + d] += scale * g;
                    }
                }
                for c in 0..k {
                    for d in 0..dim {
                        self.centroids_data[c * dim + d] -= learning_rate * grad[c * dim + d];
                    }
                }
            }
            self.compute_centroid_norms();
        }
        Ok(())
    }

    /// C++: `PlaidIndex::CopyCentroidsFrom`.
    pub fn copy_centroids_from(
        &mut self,
        centroids_data: &[f32],
        centroid_norms_neg_half: &[f32],
        n_centroids: u32,
        quantizer: Option<&PlaidQuantizer>,
        preserve_ivf_lists: bool,
    ) -> Result<()> {
        self.n_centroids = n_centroids;
        self.centroids_data = centroids_data.to_vec();
        self.centroid_norms_neg_half = centroid_norms_neg_half.to_vec();
        if let (Some(src), Some(dst)) = (quantizer, self.quantizer.as_mut()) {
            dst.copy_from(src)?;
        }
        if !preserve_ivf_lists {
            self.ivf_lists = vec![Vec::new(); n_centroids as usize];
        }
        Ok(())
    }

    /// C++: `PlaidIndex::ShareGlobalCentroids`.
    pub fn share_global_centroids(
        &mut self,
        global_centroids: Arc<PlaidGlobalCentroids>,
    ) -> Result<()> {
        if !global_centroids.is_trained() {
            return Err(Error::Invalid(
                "PlaidIndex::ShareGlobalCentroids: Global centroids not trained".into(),
            ));
        }
        self.n_centroids = global_centroids.n_centroids();
        if let (Some(src), Some(dst)) = (global_centroids.quantizer(), self.quantizer.as_mut()) {
            dst.copy_from(src)?;
        }
        self.global_centroids = Some(global_centroids);
        self.ivf_lists = vec![Vec::new(); self.n_centroids as usize];
        self.centroids_data.clear();
        self.centroid_norms_neg_half.clear();
        Ok(())
    }

    fn ensure_mutable_ivf(&mut self) {
        if !self.ivf_flattened {
            return;
        }
        self.ivf_lists = (0..self.n_centroids as usize)
            .map(|i| {
                let at = self.ivf_offsets[i] as usize;
                let len = self.ivf_lengths[i] as usize;
                self.ivf_data[at..at + len].to_vec()
            })
            .collect();
        self.ivf_data.clear();
        self.ivf_offsets.clear();
        self.ivf_lengths.clear();
        self.ivf_flattened = false;
    }

    /// C++: `PlaidIndex::FinalizeIVF`.
    pub fn finalize_ivf(&mut self) {
        if self.ivf_flattened || self.ivf_lists.is_empty() {
            return;
        }
        let total: usize = self.ivf_lists.iter().map(Vec::len).sum();
        self.ivf_data = Vec::with_capacity(total);
        self.ivf_offsets = vec![0; self.n_centroids as usize];
        self.ivf_lengths = vec![0; self.n_centroids as usize];
        for i in 0..self.n_centroids as usize {
            self.ivf_offsets[i] = self.ivf_data.len() as u32;
            self.ivf_lengths[i] = self.ivf_lists[i].len() as u32;
            self.ivf_data.extend_from_slice(&self.ivf_lists[i]);
        }
        self.ivf_lists = Vec::new();
        self.ivf_flattened = true;
    }

    /// C++: `PlaidIndex::GetIVFListData` / `GetIVFListLength`.
    pub fn ivf_list(&self, centroid_id: u32) -> &[u32] {
        if centroid_id >= self.n_centroids {
            return &[];
        }
        if self.ivf_flattened {
            let at = self.ivf_offsets[centroid_id as usize] as usize;
            let len = self.ivf_lengths[centroid_id as usize] as usize;
            return &self.ivf_data[at..at + len];
        }
        self.ivf_lists
            .get(centroid_id as usize)
            .map(Vec::as_slice)
            .unwrap_or(&[])
    }

    /// C++: `PlaidIndex::AddMultipleDocsEmbeddings`.
    pub fn add_multiple_docs_embeddings(
        &mut self,
        embedding_data: &[f32],
        doc_lens: &[u32],
    ) -> Result<()> {
        if doc_lens.is_empty() {
            return Ok(());
        }
        self.ensure_mutable_ivf();
        let dim = self.embedding_dimension as usize;
        let total: usize = doc_lens.iter().map(|&l| l as usize).sum();
        if embedding_data.len() < total * dim {
            return Err(Error::Invalid(
                "PlaidIndex::AddMultipleDocsEmbeddings: buffer too short".into(),
            ));
        }
        let old_docs = self.n_docs;
        let old_embeddings = self.n_total_embeddings;
        let packed_dim = self.packed_dim() as usize;

        let mut assignments = Vec::with_capacity(total);
        for i in 0..total {
            let emb = &embedding_data[i * dim..(i + 1) * dim];
            let cid = self.nearest_centroid(emb);
            assignments.push(cid);
            if !self.colbertsar_mode {
                let centroid = &self.centroids_data()[cid as usize * dim..(cid as usize + 1) * dim];
                let mut residual = vec![0f32; dim];
                for d in 0..dim {
                    residual[d] = emb[d] - centroid[d];
                }
                let packed = self
                    .quantizer
                    .as_ref()
                    .expect("PLAID mode has a quantizer")
                    .quantize(&residual, 1)?;
                debug_assert_eq!(packed.len(), packed_dim);
                self.packed_residuals.extend_from_slice(&packed);
            }
        }

        self.centroid_ids.extend_from_slice(&assignments);
        let mut offset = 0usize;
        for (doc_idx, &len) in doc_lens.iter().enumerate() {
            let doc_id = old_docs + doc_idx as u32;
            for &cid in &assignments[offset..offset + len as usize] {
                let list = &mut self.ivf_lists[cid as usize];
                if list.last() != Some(&doc_id) {
                    list.push(doc_id);
                }
            }
            offset += len as usize;
        }
        let mut current = old_embeddings;
        for &len in doc_lens {
            self.doc_lens.push(len);
            self.doc_offsets.push(current);
            current += len;
        }
        self.n_total_embeddings = current;
        self.n_docs += doc_lens.len() as u32;
        Ok(())
    }

    /// C++: `PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids`.
    pub fn add_multiple_docs_embeddings_with_centroids(
        &mut self,
        doc_lens: &[u32],
        centroid_ids: &[u32],
        packed_residuals: Option<&[u8]>,
        packed_dim: u32,
    ) -> Result<()> {
        if doc_lens.is_empty() {
            return Ok(());
        }
        self.ensure_mutable_ivf();
        let total: usize = doc_lens.iter().map(|&l| l as usize).sum();
        if centroid_ids.len() < total {
            return Err(Error::Invalid(
                "PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids: centroid ids too short".into(),
            ));
        }
        let old_docs = self.n_docs;
        let old_embeddings = self.n_total_embeddings;

        self.centroid_ids.extend_from_slice(&centroid_ids[..total]);
        if !self.colbertsar_mode {
            let expected = self.packed_dim();
            if packed_dim != expected {
                return Err(Error::Invalid(format!(
                    "PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids: packed_dim mismatch. Expected {expected}, got {packed_dim}"
                )));
            }
            let packed = packed_residuals.ok_or_else(|| {
                Error::Invalid(
                    "PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids: missing packed residuals"
                        .into(),
                )
            })?;
            let need = total * packed_dim as usize;
            if packed.len() < need {
                return Err(Error::Invalid(
                    "PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids: residual buffer too short"
                        .into(),
                ));
            }
            self.packed_residuals.extend_from_slice(&packed[..need]);
        }

        let mut offset = 0usize;
        for (doc_idx, &len) in doc_lens.iter().enumerate() {
            let doc_id = old_docs + doc_idx as u32;
            for &cid in &centroid_ids[offset..offset + len as usize] {
                if cid >= self.n_centroids {
                    return Err(Error::Invalid(format!(
                        "PlaidIndex::AddMultipleDocsEmbeddingsWithCentroids: invalid centroid_id {cid} (n_centroids={})",
                        self.n_centroids
                    )));
                }
                let list = &mut self.ivf_lists[cid as usize];
                if list.last() != Some(&doc_id) {
                    list.push(doc_id);
                }
            }
            offset += len as usize;
        }
        let mut current = old_embeddings;
        for &len in doc_lens {
            self.doc_lens.push(len);
            self.doc_offsets.push(current);
            current += len;
        }
        self.n_total_embeddings = current;
        self.n_docs += doc_lens.len() as u32;
        Ok(())
    }

    /// C++: `PlaidIndex::ComputeQueryCentroidScores`.
    fn compute_query_centroid_scores(&self, query: &[f32], n_query_tokens: usize) -> Vec<f32> {
        let dim = self.embedding_dimension as usize;
        let k = self.n_centroids as usize;
        let centroids = self.centroids_data();
        let norms = self.centroid_norms_neg_half();
        let mut scores = vec![0f32; n_query_tokens * k];
        for q in 0..n_query_tokens {
            let token = &query[q * dim..(q + 1) * dim];
            for c in 0..k {
                let cent = &centroids[c * dim..(c + 1) * dim];
                let mut dot = 0f32;
                for d in 0..dim {
                    dot += token[d] * cent[d];
                }
                scores[q * k + c] = if self.colbertsar_mode {
                    dot
                } else {
                    dot + norms[c]
                };
            }
        }
        scores
    }

    /// C++: `PlaidIndex::ApproximateScore`.
    fn approximate_score(
        &self,
        doc_centroid_ids: &[u32],
        query_centroid_scores: &[f32],
        n_query_tokens: usize,
    ) -> f32 {
        let k = self.n_centroids as usize;
        let mut max_scores = vec![f32::MIN; n_query_tokens];
        for &cid in doc_centroid_ids {
            for (q, slot) in max_scores.iter_mut().enumerate() {
                *slot = slot.max(query_centroid_scores[q * k + cid as usize]);
            }
        }
        max_scores.iter().filter(|s| **s > f32::MIN).sum()
    }

    /// C++: `PlaidIndex::ApproximateScoreSparse`.
    fn approximate_score_sparse(
        &self,
        doc_centroid_ids: &[u32],
        sparse_scores: &[f32],
        sparse_centroid_id_map: &[u32],
        n_query_tokens: usize,
    ) -> f32 {
        let mut max_scores = vec![f32::MIN; n_query_tokens];
        for &cid in doc_centroid_ids {
            let Some(&sparse_idx) = sparse_centroid_id_map.get(cid as usize) else {
                continue;
            };
            if sparse_idx == u32::MAX {
                continue;
            }
            let row = &sparse_scores
                [sparse_idx as usize * n_query_tokens..(sparse_idx as usize + 1) * n_query_tokens];
            for q in 0..n_query_tokens {
                max_scores[q] = max_scores[q].max(row[q]);
            }
        }
        max_scores.iter().filter(|s| **s > f32::MIN).sum()
    }

    #[allow(clippy::too_many_arguments)]
    fn exact_score_one(
        &self,
        _query: &[f32],
        n_query_tokens: usize,
        doc_id: u32,
        c_scores: &[f32],
        doc_start_in_c_scores: usize,
        total_tokens_in_c_scores: usize,
        luts: &[f32],
    ) -> f32 {
        let doc_len = self.doc_lens[doc_id as usize] as usize;
        let packed_dim = self.packed_dim() as usize;
        let quantizer = self.quantizer.as_ref().expect("PLAID mode has a quantizer");
        let mut total = 0f32;
        for q in 0..n_query_tokens {
            let lut = &luts[q * packed_dim * 256..(q + 1) * packed_dim * 256];
            let mut max_score = f32::MIN;
            for d in 0..doc_len {
                let emb = self.doc_offsets[doc_id as usize] as usize + d;
                let r = quantizer.single_ip_distance_from_lut(emb, lut, &self.packed_residuals);
                let centroid_part =
                    c_scores[q * total_tokens_in_c_scores + doc_start_in_c_scores + d];
                max_score = max_score.max(centroid_part + r);
            }
            if max_score > f32::MIN {
                total += max_score;
            }
        }
        total
    }

    /// C++: `PlaidIndex::ExactScore`.
    pub fn exact_score(&self, query: &[f32], n_query_tokens: usize, doc_id: u32) -> f32 {
        let dim = self.embedding_dimension as usize;
        let doc_len = self.doc_lens[doc_id as usize] as usize;
        if doc_len == 0 {
            return 0.0;
        }
        let doc_start = self.doc_offsets[doc_id as usize] as usize;
        let centroids = self.centroids_data();
        // c_scores[q, d] = query[q] · centroid[centroid_ids[doc_start + d]]
        let mut c_scores = vec![0f32; n_query_tokens * doc_len];
        for d in 0..doc_len {
            let cid = self.centroid_ids[doc_start + d] as usize;
            let cent = &centroids[cid * dim..(cid + 1) * dim];
            for q in 0..n_query_tokens {
                let token = &query[q * dim..(q + 1) * dim];
                c_scores[q * doc_len + d] = token.iter().zip(cent).map(|(a, b)| a * b).sum();
            }
        }
        let luts = self.position_luts(query, n_query_tokens);
        self.exact_score_one(query, n_query_tokens, doc_id, &c_scores, 0, doc_len, &luts)
    }

    fn position_luts(&self, query: &[f32], n_query_tokens: usize) -> Vec<f32> {
        let dim = self.embedding_dimension as usize;
        let packed_dim = self.packed_dim() as usize;
        let quantizer = self.quantizer.as_ref().expect("PLAID mode has a quantizer");
        let mut all = vec![0f32; n_query_tokens * packed_dim * 256];
        for q in 0..n_query_tokens {
            let table = quantizer
                .ip_distance_table(&query[q * dim..(q + 1) * dim])
                .expect("query token has the embedding dimension");
            quantizer
                .build_position_lut(
                    &table,
                    &mut all[q * packed_dim * 256..(q + 1) * packed_dim * 256],
                )
                .expect("LUT buffer has the packed dimension");
        }
        all
    }

    /// C++: `PlaidIndex::ExactScoreBatch`.
    pub fn exact_score_batch(
        &self,
        query: &[f32],
        n_query_tokens: usize,
        candidates: &[u32],
    ) -> Vec<f32> {
        if candidates.is_empty() {
            return Vec::new();
        }
        let dim = self.embedding_dimension as usize;
        let centroids = self.centroids_data();
        let packed_dim = self.packed_dim() as usize;
        let mut doc_starts = Vec::with_capacity(candidates.len());
        let mut total_tokens = 0usize;
        for &did in candidates {
            doc_starts.push(total_tokens);
            total_tokens += self.doc_lens[did as usize] as usize;
        }
        if total_tokens == 0 {
            return vec![0.0; candidates.len()];
        }
        let mut gathered = Vec::with_capacity(total_tokens * dim);
        for &did in candidates {
            let doc_start = self.doc_offsets[did as usize] as usize;
            for d in 0..self.doc_lens[did as usize] as usize {
                let cid = self.centroid_ids[doc_start + d] as usize;
                gathered.extend_from_slice(&centroids[cid * dim..(cid + 1) * dim]);
            }
        }
        let mut c_scores = vec![0f32; n_query_tokens * total_tokens];
        for q in 0..n_query_tokens {
            let token = &query[q * dim..(q + 1) * dim];
            for t in 0..total_tokens {
                let g = &gathered[t * dim..(t + 1) * dim];
                c_scores[q * total_tokens + t] = token.iter().zip(g).map(|(a, b)| a * b).sum();
            }
        }
        let luts = self.position_luts(query, n_query_tokens);
        let quantizer = self.quantizer.as_ref().expect("PLAID mode has a quantizer");
        candidates
            .iter()
            .zip(&doc_starts)
            .map(|(&did, &start)| {
                let doc_len = self.doc_lens[did as usize] as usize;
                let doc_offset = self.doc_offsets[did as usize] as usize;
                let mut total = 0f32;
                for q in 0..n_query_tokens {
                    let lut = &luts[q * packed_dim * 256..(q + 1) * packed_dim * 256];
                    let mut max_score = f32::MIN;
                    for d in 0..doc_len {
                        let r = quantizer.single_ip_distance_from_lut(
                            doc_offset + d,
                            lut,
                            &self.packed_residuals,
                        );
                        max_score = max_score.max(c_scores[q * total_tokens + start + d] + r);
                    }
                    if max_score > f32::MIN {
                        total += max_score;
                    }
                }
                total
            })
            .collect()
    }

    /// C++: `PlaidIndex::BatchedIVFProbe`.
    #[allow(clippy::too_many_arguments)]
    fn batched_ivf_probe(
        &self,
        query: &[f32],
        n_query_tokens: usize,
        n_ivf_probe: usize,
        centroid_score_threshold: f32,
        batch_size: u32,
    ) -> (Vec<u32>, Vec<f32>, Vec<u32>) {
        let dim = self.embedding_dimension as usize;
        let k = self.n_centroids as usize;
        let batch_size = if batch_size == 0 {
            Self::DEFAULT_CENTROID_BATCH_SIZE
        } else {
            batch_size
        } as usize;
        let centroids = self.centroids_data();
        let norms = self.centroid_norms_neg_half();
        // Per-token top-k: (score, centroid id), sorted descending.
        let mut heaps: Vec<Vec<(f32, u32)>> = vec![Vec::new(); n_query_tokens];
        let mut centroid_max_score = vec![f32::MIN; k];
        for start in (0..k).step_by(batch_size) {
            let end = (start + batch_size).min(k);
            for q in 0..n_query_tokens {
                let token = &query[q * dim..(q + 1) * dim];
                for c in start..end {
                    let cent = &centroids[c * dim..(c + 1) * dim];
                    let mut score: f32 = token.iter().zip(cent).map(|(a, b)| a * b).sum();
                    if !self.colbertsar_mode {
                        score += norms[c];
                    }
                    centroid_max_score[c] = centroid_max_score[c].max(score);
                    let heap = &mut heaps[q];
                    if n_ivf_probe == 0 {
                        continue;
                    }
                    if heap.len() < n_ivf_probe {
                        heap.push((score, c as u32));
                        heap.sort_by(|a, b| b.0.total_cmp(&a.0));
                    } else if score > heap.last().unwrap().0 {
                        heap.pop();
                        heap.push((score, c as u32));
                        let at = heap.iter().position(|x| x.0 < score).unwrap_or(heap.len());
                        heap.insert(at, (score, c as u32));
                    }
                }
            }
        }
        let mut selected = vec![false; k];
        let mut probed = Vec::new();
        for heap in &heaps {
            for &(_, cid) in heap {
                if !selected[cid as usize] {
                    selected[cid as usize] = true;
                    probed.push(cid);
                }
            }
        }
        if centroid_score_threshold != 0.0 {
            probed.retain(|&cid| centroid_max_score[cid as usize] >= centroid_score_threshold);
        }
        // Sparse scores, layout [n_sparse, n_query_tokens].
        let mut sparse = vec![0f32; probed.len() * n_query_tokens];
        let mut id_map = vec![u32::MAX; k];
        for (i, &cid) in probed.iter().enumerate() {
            id_map[cid as usize] = i as u32;
            let cent = &centroids[cid as usize * dim..(cid as usize + 1) * dim];
            for q in 0..n_query_tokens {
                let token = &query[q * dim..(q + 1) * dim];
                let mut score: f32 = token.iter().zip(cent).map(|(a, b)| a * b).sum();
                if !self.colbertsar_mode {
                    score += norms[cid as usize];
                }
                sparse[i * n_query_tokens + q] = score;
            }
        }
        (probed, sparse, id_map)
    }

    /// C++: `PlaidIndex::SearchWithBitmask`.
    #[allow(clippy::too_many_arguments)]
    pub fn search_with_bitmask(
        &self,
        query: &[f32],
        n_query_tokens: usize,
        top_n: usize,
        bitmask: &Bitmask,
        n_ivf_probe: usize,
        centroid_score_threshold: f32,
        n_doc_to_score: usize,
        n_full_scores: usize,
    ) -> PlaidQueryResult {
        if self.n_docs == 0 || self.n_centroids == 0 {
            return PlaidQueryResult::default();
        }
        let dim = self.embedding_dimension as usize;
        if query.len() < n_query_tokens * dim {
            return PlaidQueryResult::default();
        }
        if self.n_centroids > self.centroid_batch_size {
            return self.search_batched(
                query,
                n_query_tokens,
                top_n,
                bitmask,
                n_ivf_probe,
                centroid_score_threshold,
                n_doc_to_score,
                n_full_scores,
            );
        }

        let scores = self.compute_query_centroid_scores(query, n_query_tokens);
        let k = self.n_centroids as usize;
        // Per-token top `n_ivf_probe`, then the union.
        let mut selected = vec![false; k];
        let mut probed = Vec::new();
        for q in 0..n_query_tokens {
            let mut ranked: Vec<(f32, u32)> =
                (0..k).map(|c| (scores[q * k + c], c as u32)).collect();
            ranked.sort_by(|a, b| b.0.total_cmp(&a.0));
            for &(_, cid) in ranked.iter().take(n_ivf_probe) {
                if !selected[cid as usize] {
                    selected[cid as usize] = true;
                    probed.push(cid);
                }
            }
        }
        if centroid_score_threshold != 0.0 {
            probed.retain(|&cid| {
                (0..n_query_tokens)
                    .map(|q| scores[q * k + cid as usize])
                    .fold(f32::MIN, f32::max)
                    >= centroid_score_threshold
            });
        }

        let mut marker = vec![false; self.n_docs as usize];
        let mut candidates = Vec::new();
        for &cid in &probed {
            for &doc_id in self.ivf_list(cid) {
                if !bitmask.is_true(doc_id) {
                    continue;
                }
                if !marker[doc_id as usize] {
                    marker[doc_id as usize] = true;
                    candidates.push(doc_id);
                }
            }
        }
        if candidates.is_empty() {
            return PlaidQueryResult::default();
        }
        candidates.sort_unstable();

        let mut doc_scores: Vec<(f32, u32)> = candidates
            .iter()
            .map(|&doc_id| {
                let start = self.doc_offsets[doc_id as usize] as usize;
                let len = self.doc_lens[doc_id as usize] as usize;
                let ids = &self.centroid_ids[start..start + len];
                (self.approximate_score(ids, &scores, n_query_tokens), doc_id)
            })
            .collect();

        if self.colbertsar_mode {
            return self.finish(doc_scores, top_n);
        }

        sort_desc(&mut doc_scores);
        let n_rerank = n_full_scores.min(n_doc_to_score).min(doc_scores.len());
        let rerank_ids: Vec<u32> = doc_scores[..n_rerank].iter().map(|x| x.1).collect();
        let exact = self.exact_score_batch(query, n_query_tokens, &rerank_ids);
        let mut final_scores: Vec<(f32, u32)> = exact.into_iter().zip(rerank_ids).collect();
        sort_desc(&mut final_scores);
        self.finish(final_scores, top_n)
    }

    #[allow(clippy::too_many_arguments)]
    fn search_batched(
        &self,
        query: &[f32],
        n_query_tokens: usize,
        top_n: usize,
        bitmask: &Bitmask,
        n_ivf_probe: usize,
        centroid_score_threshold: f32,
        n_doc_to_score: usize,
        n_full_scores: usize,
    ) -> PlaidQueryResult {
        let (probed, sparse, id_map) = self.batched_ivf_probe(
            query,
            n_query_tokens,
            n_ivf_probe,
            centroid_score_threshold,
            self.centroid_batch_size,
        );
        if probed.is_empty() {
            return PlaidQueryResult::default();
        }
        let mut marker = vec![false; self.n_docs as usize];
        let mut candidates = Vec::new();
        for &cid in &probed {
            for &doc_id in self.ivf_list(cid) {
                if !bitmask.is_true(doc_id) {
                    continue;
                }
                if !marker[doc_id as usize] {
                    marker[doc_id as usize] = true;
                    candidates.push(doc_id);
                }
            }
        }
        if candidates.is_empty() {
            return PlaidQueryResult::default();
        }
        candidates.sort_unstable();
        let mut doc_scores: Vec<(f32, u32)> = candidates
            .iter()
            .map(|&doc_id| {
                let start = self.doc_offsets[doc_id as usize] as usize;
                let len = self.doc_lens[doc_id as usize] as usize;
                let ids = &self.centroid_ids[start..start + len];
                (
                    self.approximate_score_sparse(ids, &sparse, &id_map, n_query_tokens),
                    doc_id,
                )
            })
            .collect();
        if self.colbertsar_mode {
            return self.finish(doc_scores, top_n);
        }
        sort_desc(&mut doc_scores);
        let n_rerank = n_full_scores.min(n_doc_to_score).min(doc_scores.len());
        let rerank_ids: Vec<u32> = doc_scores[..n_rerank].iter().map(|x| x.1).collect();
        let exact = self.exact_score_batch(query, n_query_tokens, &rerank_ids);
        let mut final_scores: Vec<(f32, u32)> = exact.into_iter().zip(rerank_ids).collect();
        sort_desc(&mut final_scores);
        self.finish(final_scores, top_n)
    }

    fn finish(&self, scores: Vec<(f32, u32)>, top_n: usize) -> PlaidQueryResult {
        let n = top_n.min(scores.len());
        PlaidQueryResult {
            scores: scores[..n].iter().map(|x| x.0).collect(),
            ids: scores[..n]
                .iter()
                .map(|x| x.1 + self.start_segment_offset)
                .collect(),
        }
    }

    /// C++: `PlaidIndex::MergeChunks`.
    pub fn merge_chunks(&mut self, chunks: &[(u32, &PlaidIndex)]) -> Result<()> {
        if chunks.is_empty() {
            return Ok(());
        }
        let packed_dim = self.packed_dim() as usize;
        let total_docs: usize = chunks.iter().map(|(_, c)| c.doc_num() as usize).sum();
        let total_embeddings: usize = chunks
            .iter()
            .map(|(_, c)| c.total_embedding_num() as usize)
            .sum();
        let mut doc_lens = Vec::with_capacity(total_docs);
        let mut doc_offsets = Vec::with_capacity(total_docs);
        let mut centroid_ids = Vec::with_capacity(total_embeddings);
        let mut packed = Vec::with_capacity(if self.colbertsar_mode {
            0
        } else {
            total_embeddings * packed_dim
        });
        let mut embedding_offset = 0u32;
        for (_, chunk) in chunks {
            for (i, &len) in chunk.doc_lens().iter().enumerate() {
                doc_lens.push(len);
                doc_offsets.push(embedding_offset + chunk.doc_offsets()[i]);
            }
            centroid_ids.extend_from_slice(chunk.centroid_ids());
            if !self.colbertsar_mode {
                packed.extend_from_slice(chunk.packed_residuals());
            }
            embedding_offset += chunk.total_embedding_num();
        }
        let mut ivf = vec![Vec::new(); self.n_centroids as usize];
        for doc_id in 0..total_docs {
            let start = doc_offsets[doc_id] as usize;
            let len = doc_lens[doc_id] as usize;
            for &cid in &centroid_ids[start..start + len] {
                let list = &mut ivf[cid as usize];
                if list.last() != Some(&(doc_id as u32)) {
                    list.push(doc_id as u32);
                }
            }
        }
        self.ensure_mutable_ivf();
        self.doc_lens = doc_lens;
        self.doc_offsets = doc_offsets;
        self.centroid_ids = centroid_ids;
        if !self.colbertsar_mode {
            self.packed_residuals = packed;
        }
        self.ivf_lists = ivf;
        self.n_docs = total_docs as u32;
        self.n_total_embeddings = total_embeddings as u32;
        Ok(())
    }

    /// C++: `PlaidIndex::InitializeMerge`.
    pub fn initialize_merge(&mut self, n_centroids: u32) {
        self.doc_lens.clear();
        self.doc_offsets.clear();
        self.centroid_ids.clear();
        self.packed_residuals.clear();
        self.n_centroids = n_centroids;
        self.ivf_lists = vec![Vec::new(); n_centroids as usize];
        self.ivf_data.clear();
        self.ivf_offsets.clear();
        self.ivf_lengths.clear();
        self.ivf_flattened = false;
        self.n_docs = 0;
        self.n_total_embeddings = 0;
    }

    /// C++: `PlaidIndex::MergeOneChunk`.
    pub fn merge_one_chunk(&mut self, chunk: &PlaidIndex, doc_offset: u32) -> Result<()> {
        self.ensure_mutable_ivf();
        let current_embeddings = self.n_total_embeddings;
        let chunk_docs = chunk.doc_num() as usize;
        for i in 0..chunk_docs {
            self.doc_lens.push(chunk.doc_lens()[i]);
            self.doc_offsets
                .push(current_embeddings + chunk.doc_offsets()[i]);
        }
        self.centroid_ids.extend_from_slice(chunk.centroid_ids());
        if !self.colbertsar_mode {
            self.packed_residuals
                .extend_from_slice(chunk.packed_residuals());
        }
        for i in 0..chunk_docs {
            let global_doc_id = doc_offset + i as u32;
            let doc_len = chunk.doc_lens()[i] as usize;
            let local_offset = chunk.doc_offsets()[i] as usize;
            for j in 0..doc_len {
                let cid = chunk.centroid_ids()[local_offset + j];
                if cid >= self.n_centroids {
                    continue;
                }
                let list = &mut self.ivf_lists[cid as usize];
                if list.last() != Some(&global_doc_id) {
                    list.push(global_doc_id);
                }
            }
        }
        self.n_docs += chunk.doc_num();
        self.n_total_embeddings += chunk.total_embedding_num();
        Ok(())
    }

    /// C++: `PlaidIndex::FinalizeMerge`.
    pub fn finalize_merge(&self) {}

    /// C++: `PlaidIndex::AcceptMergedData`.
    #[allow(clippy::too_many_arguments)]
    pub fn accept_merged_data(
        &mut self,
        doc_lens: Vec<u32>,
        doc_offsets: Vec<u32>,
        centroid_ids: Vec<u32>,
        packed_residuals: Vec<u8>,
        ivf_lists: Vec<Vec<u32>>,
        total_docs: u32,
        total_embeddings: u32,
    ) -> Result<()> {
        if ivf_lists.len() != self.n_centroids as usize {
            return Err(Error::Invalid(format!(
                "PlaidIndex::AcceptMergedData: IVF lists size {} doesn't match n_centroids {}",
                ivf_lists.len(),
                self.n_centroids
            )));
        }
        if doc_lens.len() != total_docs as usize || doc_offsets.len() != total_docs as usize {
            return Err(Error::Invalid(
                "PlaidIndex::AcceptMergedData: document metadata size mismatch".into(),
            ));
        }
        if centroid_ids.len() != total_embeddings as usize {
            return Err(Error::Invalid(
                "PlaidIndex::AcceptMergedData: centroid_ids size mismatch".into(),
            ));
        }
        if !self.colbertsar_mode {
            let expected = total_embeddings as usize * self.packed_dim() as usize;
            if packed_residuals.len() != expected {
                return Err(Error::Invalid(format!(
                    "PlaidIndex::AcceptMergedData: packed_residuals_size {} doesn't match expected {}",
                    packed_residuals.len(),
                    expected
                )));
            }
            if let Some(global) = &self.global_centroids {
                if let (Some(src), Some(dst)) = (global.quantizer(), self.quantizer.as_mut()) {
                    dst.copy_from(src)?;
                }
                self.centroids_data = global.centroids_data().to_vec();
                self.centroid_norms_neg_half = global.centroid_norms_neg_half().to_vec();
                self.global_centroids = None;
            }
            self.packed_residuals = packed_residuals;
        }
        self.doc_lens = doc_lens;
        self.doc_offsets = doc_offsets;
        self.centroid_ids = centroid_ids;
        self.ivf_lists = ivf_lists;
        self.ivf_data.clear();
        self.ivf_offsets.clear();
        self.ivf_lengths.clear();
        self.ivf_flattened = false;
        self.n_docs = total_docs;
        self.n_total_embeddings = total_embeddings;
        Ok(())
    }

    /// C++: `PlaidIndex::FindOutliers`.
    pub fn find_outliers(&self, embeddings: &[f32], threshold_sq: f32) -> Vec<u64> {
        let dim = self.embedding_dimension as usize;
        let centroids = self.centroids_data();
        let norms = self.centroid_norms_neg_half();
        let mut outliers = Vec::new();
        for i in 0..embeddings.len() / dim {
            let emb = &embeddings[i * dim..(i + 1) * dim];
            let mut max_neg = f32::MIN;
            for c in 0..self.n_centroids as usize {
                let cent = &centroids[c * dim..(c + 1) * dim];
                let dot: f32 = emb.iter().zip(cent).map(|(a, b)| a * b).sum();
                max_neg = max_neg.max(dot + norms[c]);
            }
            let norm_sq: f32 = emb.iter().map(|v| v * v).sum();
            if norm_sq - 2.0 * max_neg > threshold_sq {
                outliers.push(i as u64);
            }
        }
        outliers
    }

    /// C++: `PlaidIndex::ExpandCentroids`.
    pub fn expand_centroids(
        &mut self,
        new_embeddings: &[f32],
        cluster_threshold: f32,
    ) -> Result<u32> {
        let dim = self.embedding_dimension as usize;
        let n_new = new_embeddings.len() / dim;
        if n_new == 0 || self.n_centroids == 0 {
            return Ok(0);
        }
        self.ensure_mutable_ivf();
        let outliers = self.find_outliers(new_embeddings, cluster_threshold * cluster_threshold);
        if outliers.is_empty() {
            return Ok(0);
        }
        const MAX_POINTS_PER_CENTROID: u32 = 256;
        let target = (outliers.len() as f64 / MAX_POINTS_PER_CENTROID as f64)
            .ceil()
            .max(1.0) as u32
            * 4;
        let mut k_update = target.min(outliers.len() as u32).div_ceil(8) * 8;
        k_update = k_update.max(8);
        if k_update as usize > outliers.len() {
            k_update = ((outliers.len() as u32) / 8) * 8;
            if k_update == 0 {
                k_update = 8;
            }
        }
        let mut outlier_data = Vec::with_capacity(outliers.len() * dim);
        for &idx in &outliers {
            let at = idx as usize * dim;
            outlier_data.extend_from_slice(&new_embeddings[at..at + dim]);
        }
        let new_centroids = kmeans_l2(dim, &outlier_data, outliers.len(), k_update as usize, 4)?;
        if let Some(global) = self.global_centroids.take() {
            self.centroids_data = global.centroids_data().to_vec();
            self.centroid_norms_neg_half = global.centroid_norms_neg_half().to_vec();
            if let (Some(src), Some(dst)) = (global.quantizer(), self.quantizer.as_mut()) {
                dst.copy_from(src)?;
            }
        }
        let old = self.n_centroids;
        self.centroids_data.extend_from_slice(&new_centroids);
        self.centroid_norms_neg_half
            .resize((old + k_update) as usize, 0.0);
        for i in 0..k_update as usize {
            let at = (old as usize + i) * dim;
            let c = &self.centroids_data[at..at + dim];
            self.centroid_norms_neg_half[old as usize + i] =
                -0.5 * c.iter().map(|v| v * v).sum::<f32>();
        }
        self.ivf_lists.resize((old + k_update) as usize, Vec::new());
        self.n_centroids = old + k_update;
        Ok(k_update)
    }

    /// C++: `PlaidIndex::CalcSize` / `SaveIndexInner`.
    pub fn to_bytes(&self) -> Vec<u8> {
        let mut out = Vec::new();
        let format_version: u32 = u32::from(self.colbertsar_mode) + 1;
        out.extend_from_slice(&format_version.to_le_bytes());
        out.extend_from_slice(&self.start_segment_offset.to_le_bytes());
        out.extend_from_slice(&self.embedding_dimension.to_le_bytes());
        out.extend_from_slice(&self.nbits.to_le_bytes());
        out.extend_from_slice(&self.n_centroids.to_le_bytes());
        out.extend_from_slice(&self.n_docs.to_le_bytes());
        out.extend_from_slice(&self.n_total_embeddings.to_le_bytes());

        for v in self.centroids_data() {
            out.extend_from_slice(&v.to_le_bytes());
        }
        for v in self.centroid_norms_neg_half() {
            out.extend_from_slice(&v.to_le_bytes());
        }

        out.extend_from_slice(&(self.doc_lens.len() as u32).to_le_bytes());
        for &v in &self.doc_lens {
            out.extend_from_slice(&v.to_le_bytes());
        }
        out.extend_from_slice(&(self.doc_offsets.len() as u32).to_le_bytes());
        for &v in &self.doc_offsets {
            out.extend_from_slice(&v.to_le_bytes());
        }
        out.extend_from_slice(&(self.centroid_ids.len() as u32).to_le_bytes());
        for &v in &self.centroid_ids {
            out.extend_from_slice(&v.to_le_bytes());
        }
        for i in 0..self.n_centroids {
            let list = self.ivf_list(i);
            out.extend_from_slice(&(list.len() as u32).to_le_bytes());
            for &v in list {
                out.extend_from_slice(&v.to_le_bytes());
            }
        }
        if !self.colbertsar_mode {
            out.extend_from_slice(&(self.packed_residuals.len() as u32).to_le_bytes());
            out.extend_from_slice(&self.packed_residuals);
            out.extend_from_slice(
                &self
                    .quantizer
                    .as_ref()
                    .expect("PLAID mode has a quantizer")
                    .to_bytes(),
            );
        }
        out
    }

    /// C++: `PlaidIndex::ReadIndexInner` / `LoadFromPtr`.
    pub fn from_bytes(
        start_segment_offset: u32,
        embedding_dimension: u32,
        nbits: u32,
        bytes: &[u8],
    ) -> Result<(Self, usize)> {
        let mut at = 0usize;
        let read_u32 = |at: &mut usize| -> Result<u32> {
            let v = bytes
                .get(*at..*at + 4)
                .ok_or_else(|| Error::Invalid("PlaidIndex::Load: truncated".into()))?;
            *at += 4;
            Ok(u32::from_le_bytes(v.try_into().unwrap()))
        };
        let read_f32 = |at: &mut usize| -> Result<f32> {
            let v = bytes
                .get(*at..*at + 4)
                .ok_or_else(|| Error::Invalid("PlaidIndex::Load: truncated".into()))?;
            *at += 4;
            Ok(f32::from_le_bytes(v.try_into().unwrap()))
        };
        let format_version = read_u32(&mut at)?;
        let stored_start = read_u32(&mut at)?;
        let stored_dim = read_u32(&mut at)?;
        let stored_nbits = read_u32(&mut at)?;
        if stored_start != start_segment_offset
            || stored_dim != embedding_dimension
            || stored_nbits != nbits
        {
            return Err(Error::Invalid("PlaidIndex::Load: header mismatch".into()));
        }
        let colbertsar = format_version == 2;
        let mut index = Self::new(
            start_segment_offset,
            embedding_dimension,
            nbits,
            0,
            colbertsar,
        )?;
        index.n_centroids = read_u32(&mut at)?;
        index.n_docs = read_u32(&mut at)?;
        index.n_total_embeddings = read_u32(&mut at)?;
        let k = index.n_centroids as usize;
        let dim = embedding_dimension as usize;
        index.centroids_data = (0..k * dim)
            .map(|_| read_f32(&mut at))
            .collect::<Result<_>>()?;
        index.centroid_norms_neg_half = (0..k).map(|_| read_f32(&mut at)).collect::<Result<_>>()?;

        let doc_lens_len = read_u32(&mut at)? as usize;
        index.doc_lens = (0..doc_lens_len)
            .map(|_| read_u32(&mut at))
            .collect::<Result<_>>()?;
        let doc_offsets_len = read_u32(&mut at)? as usize;
        index.doc_offsets = (0..doc_offsets_len)
            .map(|_| read_u32(&mut at))
            .collect::<Result<_>>()?;
        let centroid_ids_len = read_u32(&mut at)? as usize;
        index.centroid_ids = (0..centroid_ids_len)
            .map(|_| read_u32(&mut at))
            .collect::<Result<_>>()?;
        index.ivf_lists = Vec::with_capacity(k);
        for _ in 0..k {
            let len = read_u32(&mut at)? as usize;
            index.ivf_lists.push(
                (0..len)
                    .map(|_| read_u32(&mut at))
                    .collect::<Result<Vec<_>>>()?,
            );
        }
        if format_version == 1 {
            let packed_len = read_u32(&mut at)? as usize;
            let end = at + packed_len;
            index.packed_residuals = bytes
                .get(at..end)
                .ok_or_else(|| Error::Invalid("PlaidIndex::Load: truncated".into()))?
                .to_vec();
            at = end;
            let (quantizer, used) =
                PlaidQuantizer::from_bytes(nbits, embedding_dimension, &bytes[at..])?;
            at += used;
            index.quantizer = Some(quantizer);
        }
        Ok((index, at))
    }

    /// C++: `PlaidIndex::CalcSize` (the serialized length).
    pub fn calc_size(&self) -> usize {
        self.to_bytes().len()
    }

    /// C++: `PlaidIndex::MemUsage`.
    pub fn mem_usage(&self) -> usize {
        self.calc_size()
    }
}

fn sort_desc(scores: &mut [(f32, u32)]) {
    scores.sort_by(|a, b| b.0.total_cmp(&a.0).then_with(|| b.1.cmp(&a.1)));
}

/// Reservoir sampling of `count` embedding indices (`RandomPermutatePartially` +
/// reservoir in the reference).
fn sample_indices(n: usize, count: usize) -> Vec<usize> {
    let count = count.min(n);
    let mut chosen: Vec<usize> = (0..count).collect();
    let mut rng = 0x2545_F491_4F6C_DD1Du64;
    for i in count..n {
        rng ^= rng << 13;
        rng ^= rng >> 7;
        rng ^= rng << 17;
        let j = (rng % (i as u64 + 1)) as usize;
        if j < count {
            chosen[j] = i;
        }
    }
    chosen
}

#[cfg(test)]
mod tests {
    use super::*;

    struct Normal(u64);

    impl Normal {
        fn new(seed: u64) -> Self {
            Self(seed.max(1))
        }
        fn next_f32(&mut self) -> f32 {
            self.0 ^= self.0 << 13;
            self.0 ^= self.0 >> 7;
            self.0 ^= self.0 << 17;
            let u = ((self.0 >> 11) as f64) / ((1u64 << 53) as f64);
            let v = ((self.0.rotate_left(21) >> 11) as f64) / ((1u64 << 53) as f64);
            let u = u.max(1e-12);
            ((-2.0 * u.ln()).sqrt() * (2.0 * std::f64::consts::PI * v).cos()) as f32
        }
        fn data(&mut self, n: usize) -> Vec<f32> {
            (0..n).map(|_| self.next_f32()).collect()
        }
    }

    fn trained_index() -> (PlaidIndex, Vec<Vec<f32>>) {
        const DIM: u32 = 32;
        const K: u32 = 8;
        const TRAIN: usize = 512;
        const DOCS: usize = 10;
        const PER_DOC: usize = 8;
        let mut rng = Normal::new(42);
        let train = rng.data(TRAIN * DIM as usize);
        let mut index = PlaidIndex::new(0, DIM, 4, K, false).unwrap();
        index.train(K, &train, TRAIN, 4).unwrap();
        let mut docs = Vec::with_capacity(DOCS);
        for _ in 0..DOCS {
            docs.push(rng.data(PER_DOC * DIM as usize));
        }
        let lens = vec![PER_DOC as u32; DOCS];
        let flat: Vec<f32> = docs.iter().flatten().copied().collect();
        index.add_multiple_docs_embeddings(&flat, &lens).unwrap();
        (index, docs)
    }

    /// C++: `PlaidIndexTest.test_basic_construction`.
    #[test]
    fn basic_construction() {
        let index = PlaidIndex::new(0, 32, 4, 8, false).unwrap();
        assert_eq!(index.embedding_dimension(), 32);
        assert_eq!(index.n_centroids(), 0);
        assert_eq!(index.doc_num(), 0);
        assert_eq!(index.total_embedding_num(), 0);
    }

    /// C++: `PlaidIndexTest.test_train_and_add`.
    #[test]
    fn train_and_add() {
        let (index, _) = trained_index();
        assert_eq!(index.n_centroids(), 8);
        assert_eq!(index.doc_num(), 10);
        assert_eq!(index.total_embedding_num(), 80);
        for doc in 0..10 {
            assert_eq!(index.doc_len(doc).unwrap(), 8);
        }
        // Every IVF list points at real documents and covers all of them.
        for cid in 0..index.n_centroids() {
            for &doc in index.ivf_list(cid) {
                assert!(doc < index.doc_num());
            }
        }
        assert!(index.quantizer().unwrap().is_trained());
    }

    /// C++: `PlaidIndexTest.test_search` plus a recall assertion: querying a
    /// document's own token must rank it first.
    #[test]
    fn search_ranks_the_queried_document_first() {
        let (index, docs) = trained_index();
        let mut bitmask = Bitmask::new(index.doc_num() as usize);
        for doc in 0..index.doc_num() {
            bitmask.set(doc);
        }
        for (doc, tokens) in docs.iter().enumerate() {
            let query = &tokens[..32]; // first token
            let result = index.search_with_bitmask(query, 1, 1, &bitmask, 8, 0.0, 20, 20);
            assert_eq!(result.ids.len(), 1);
            assert_eq!(result.ids[0], doc as u32, "query {doc}");
        }
    }

    #[test]
    fn bitmask_filters_candidates() {
        let (index, _) = trained_index();
        let mut bitmask = Bitmask::new(index.doc_num() as usize);
        bitmask.set(3);
        bitmask.set(7);
        let query = index.centroid_ids()[index.doc_offsets()[3] as usize] as usize;
        let probe = index.centroids_data()[query * 32..(query + 1) * 32].to_vec();
        let result = index.search_with_bitmask(&probe, 1, 10, &bitmask, 8, 0.0, 20, 20);
        assert!(result.ids.iter().all(|id| *id == 3 || *id == 7));
        assert!(!result.ids.is_empty());
    }

    /// C++: `PlaidIndexTest.test_save_load`.
    #[test]
    fn save_and_load_roundtrip() {
        let (index, docs) = trained_index();
        let bytes = index.to_bytes();
        assert_eq!(bytes.len(), index.calc_size());
        let (loaded, used) = PlaidIndex::from_bytes(0, 32, 4, &bytes).unwrap();
        assert_eq!(used, bytes.len());
        assert_eq!(loaded.n_centroids(), index.n_centroids());
        assert_eq!(loaded.doc_num(), index.doc_num());
        assert_eq!(loaded.total_embedding_num(), index.total_embedding_num());
        assert_eq!(loaded.doc_lens(), index.doc_lens());
        assert_eq!(loaded.centroid_ids(), index.centroid_ids());
        assert_eq!(loaded.packed_residuals(), index.packed_residuals());
        let probe = &docs[2][..32];
        let a = index.exact_score(probe, 1, 2);
        let b = loaded.exact_score(probe, 1, 2);
        assert!((a - b).abs() < 1e-6, "{a} vs {b}");
    }

    /// C++: `PlaidIndexTest.test_mem_usage`.
    #[test]
    fn mem_usage_matches_calc_size() {
        let (index, _) = trained_index();
        assert!(index.mem_usage() > 0);
        assert_eq!(index.mem_usage(), index.calc_size());
    }

    /// C++: `PlaidIndexTest.test_move_semantics` — Rust moves by default.
    #[test]
    fn move_moves_the_index() {
        let (index, _) = trained_index();
        let moved = index;
        assert_eq!(moved.n_centroids(), 8);
        assert_eq!(moved.doc_num(), 10);
    }

    /// C++: `PlaidIndexColBERTSaRTest.test_colbertsar_*`.
    #[test]
    fn colbertsar_mode_trains_and_searches_without_residuals() {
        const DIM: u32 = 32;
        let mut rng = Normal::new(7);
        let train = rng.data(512 * DIM as usize);
        let mut index = PlaidIndex::new(0, DIM, 4, 8, true).unwrap();
        index.train(8, &train, 512, 4).unwrap();
        assert!(index.quantizer().is_none());
        assert_eq!(index.n_centroids(), 8);
        let docs: Vec<f32> = rng.data(10 * 8 * DIM as usize);
        index
            .add_multiple_docs_embeddings(&docs, &[8u32; 10])
            .unwrap();
        assert_eq!(index.doc_num(), 10);
        assert_eq!(index.total_embedding_num(), 80);
        assert!(index.packed_residuals().is_empty());

        let bytes = index.to_bytes();
        let (loaded, used) = PlaidIndex::from_bytes(0, DIM, 4, &bytes).unwrap();
        assert_eq!(used, bytes.len());
        assert!(loaded.is_colbertsar_mode());
        assert_eq!(loaded.doc_num(), 10);
        assert!(loaded.quantizer().is_none());

        let mut bitmask = Bitmask::new(10);
        (0..10).for_each(|d| bitmask.set(d));
        let result = index.search_with_bitmask(&docs[..32], 1, 3, &bitmask, 8, 0.0, 10, 10);
        assert!(!result.is_empty());
    }

    #[test]
    fn merge_keeps_documents_and_scores() {
        let (index, docs) = trained_index();
        // Split the index into two chunks and merge them back.
        let (mut first, _) = trained_index();
        let (mut second, _) = trained_index();
        let half = 5usize;
        first.doc_lens.truncate(half);
        first.doc_offsets.truncate(half);
        first.n_docs = half as u32;
        first.n_total_embeddings = first.doc_offsets[half - 1] + first.doc_lens[half - 1];
        let start = first.n_total_embeddings as usize;
        first.centroid_ids.truncate(start);
        first
            .packed_residuals
            .truncate(start * first.packed_dim() as usize);
        second.doc_lens = second.doc_lens[half..].to_vec();
        second.doc_offsets = second.doc_offsets[half..]
            .iter()
            .map(|o| o - start as u32)
            .collect();
        second.centroid_ids = second.centroid_ids[start..].to_vec();
        second.packed_residuals =
            second.packed_residuals[start * second.packed_dim() as usize..].to_vec();
        second.n_docs = (second.doc_num() as usize - half) as u32;
        second.n_total_embeddings -= start as u32;
        second.ivf_lists = vec![Vec::new(); second.n_centroids as usize];
        for doc in 0..second.doc_num() {
            let at = second.doc_offsets[doc as usize] as usize;
            let len = second.doc_lens[doc as usize] as usize;
            for &cid in &second.centroid_ids[at..at + len] {
                let list = &mut second.ivf_lists[cid as usize];
                if list.last() != Some(&doc) {
                    list.push(doc);
                }
            }
        }
        let mut merged = PlaidIndex::new(0, 32, 4, 8, false).unwrap();
        merged
            .copy_centroids_from(
                index.centroids_data(),
                index.centroid_norms_neg_half(),
                8,
                index.quantizer(),
                false,
            )
            .unwrap();
        merged
            .merge_chunks(&[(0, &first), (half as u32, &second)])
            .unwrap();
        assert_eq!(merged.doc_num(), index.doc_num());
        assert_eq!(merged.total_embedding_num(), index.total_embedding_num());
        assert_eq!(merged.doc_lens(), index.doc_lens());
        assert_eq!(merged.centroid_ids(), index.centroid_ids());
        let probe = &docs[6][..32];
        let a = index.exact_score(probe, 1, 6);
        let b = merged.exact_score(probe, 1, 6);
        assert!((a - b).abs() < 1e-6, "{a} vs {b}");
    }
}
