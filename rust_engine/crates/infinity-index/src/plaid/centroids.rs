//! Global centroids shared by every PLAID chunk of a segment.
//!
//! C++: `PlaidGlobalCentroids` (`src/storage/knn_index/plaid/plaid_global_centroids.cppm`
//! and `_impl.cpp`). Centroids are trained once (sampled k-means) and never
//! change, so chunks can share them and merges are a direct data merge. The
//! residual quantizer lives here as well; ColBERTSaR mode has none.

use infinity_common::{Error, Result};

use super::quantizer::PlaidQuantizer;

/// C++: `GetKMeansCentroids` for the L2 metric
/// (`src/storage/knn_index/knn_ivf/kmeans_partition.cppm`).
///
/// Samples down to `max_points_per_centroid * partition_num` vectors, seeds the
/// centroids from a random permutation, then alternates assignment and
/// mean-update. Empty partitions split the largest one with the reference's
/// ±1/1024 perturbation. Stops early when the total distance stops improving.
pub fn kmeans_l2(
    dimension: usize,
    vectors: &[f32],
    vector_count: usize,
    partition_num: usize,
    iteration_max: usize,
) -> Result<Vec<f32>> {
    if dimension == 0 || vector_count == 0 {
        return Err(Error::Invalid(
            "GetKMeansCentroids: dimension and vector_count must be positive".into(),
        ));
    }
    if partition_num == 0 || partition_num > vector_count {
        return Err(Error::Invalid(
            "GetKMeansCentroids: partition_num cannot exceed vector_count".into(),
        ));
    }
    if vectors.len() < vector_count * dimension {
        return Err(Error::Invalid(
            "GetKMeansCentroids: vector buffer too short".into(),
        ));
    }
    let iteration_max = iteration_max.max(1);
    let max_num = 256 * partition_num;
    let (training, training_num) = if vector_count > max_num {
        let ids = random_permutation(vector_count, max_num);
        let mut sampled = Vec::with_capacity(max_num * dimension);
        for id in ids {
            let id = id as usize;
            sampled.extend_from_slice(&vectors[id * dimension..(id + 1) * dimension]);
        }
        (sampled, max_num)
    } else {
        (vectors[..vector_count * dimension].to_vec(), vector_count)
    };

    let seed_ids = random_permutation(training_num, partition_num);
    let mut centroids = Vec::with_capacity(partition_num * dimension);
    for id in seed_ids {
        let id = id as usize;
        centroids.extend_from_slice(&training[id * dimension..(id + 1) * dimension]);
    }

    let mut partition_id = vec![0u32; training_num];
    let mut previous_total = f32::MAX;
    for _ in 0..iteration_max {
        // Assign every training vector to its nearest centroid.
        let mut total_distance = 0.0f32;
        for (i, slot) in partition_id.iter_mut().enumerate() {
            let v = &training[i * dimension..(i + 1) * dimension];
            let mut best = 0usize;
            let mut best_d = f32::INFINITY;
            for k in 0..partition_num {
                let c = &centroids[k * dimension..(k + 1) * dimension];
                let d = l2_sq(v, c);
                if d < best_d {
                    best_d = d;
                    best = k;
                }
            }
            *slot = best as u32;
            total_distance += best_d;
        }
        let mut counts = vec![0u32; partition_num];
        for &p in &partition_id {
            counts[p as usize] += 1;
        }

        // Recompute centroids as the mean of their members.
        let mut sums = vec![0f32; partition_num * dimension];
        for (i, &p) in partition_id.iter().enumerate() {
            let at = p as usize * dimension;
            for d in 0..dimension {
                sums[at + d] += training[i * dimension + d];
            }
        }
        for k in 0..partition_num {
            if counts[k] > 0 {
                let inv = 1.0f32 / counts[k] as f32;
                for d in 0..dimension {
                    centroids[k * dimension + d] = sums[k * dimension + d] * inv;
                }
            }
        }

        // A vacant partition splits the largest one.
        for i in 0..partition_num {
            if counts[i] != 0 {
                continue;
            }
            let mut max_id = 0usize;
            let mut max_count = 0u32;
            for (j, &c) in counts.iter().enumerate() {
                if c > max_count {
                    max_id = j;
                    max_count = c;
                }
            }
            counts[i] = max_count / 2;
            counts[max_id] -= counts[i];
            let src = centroids[max_id * dimension..(max_id + 1) * dimension].to_vec();
            centroids[i * dimension..(i + 1) * dimension].copy_from_slice(&src);
            const EPSILON: f32 = 1.0 / 1024.0;
            let plus = 1.0 + EPSILON;
            let minus = 1.0 - EPSILON;
            for d in 0..dimension {
                centroids[i * dimension + d] *= if d & 1 == 1 { plus } else { minus };
                centroids[max_id * dimension + d] *= if d & 1 == 1 { minus } else { plus };
            }
        }

        if total_distance >= previous_total {
            break;
        }
        previous_total = total_distance;
    }
    Ok(centroids)
}

fn l2_sq(a: &[f32], b: &[f32]) -> f32 {
    a.iter().zip(b).map(|(x, y)| (x - y) * (x - y)).sum()
}

/// C++: `RandomPermutatePartially` (`kmeans_partition.cppm`).
///
/// C++ returns the whole permutation and callers read its first `random_num`
/// entries; here the truncated prefix is returned instead.
fn random_permutation(vector_count: usize, random_num: usize) -> Vec<u32> {
    let random_num = random_num.min(vector_count);
    let mut perm: Vec<u32> = (0..vector_count as u32).collect();
    let mut rng = 0x2545_F491_4F6C_DD1Du64;
    for i in 0..random_num {
        rng ^= rng << 13;
        rng ^= rng >> 7;
        rng ^= rng << 17;
        let span = (vector_count - i) as u64;
        let j = i + (rng % span) as usize;
        perm.swap(i, j);
    }
    perm.truncate(random_num);
    perm
}

/// C++: `PlaidGlobalCentroids`.
#[derive(Debug, Clone)]
pub struct PlaidGlobalCentroids {
    embedding_dimension: u32,
    nbits: u32,
    n_centroids: u32,
    /// `n_centroids * embedding_dimension` floats, row-major.
    centroids_data: Vec<f32>,
    /// `-0.5 * ||centroid||^2`, for fast L2 comparison.
    centroid_norms_neg_half: Vec<f32>,
    /// `None` in ColBERTSaR mode.
    quantizer: Option<PlaidQuantizer>,
}

impl PlaidGlobalCentroids {
    /// C++: `PlaidGlobalCentroids::PlaidGlobalCentroids`.
    pub fn new(embedding_dimension: u32, nbits: u32, colbertsar_mode: bool) -> Result<Self> {
        let quantizer = if colbertsar_mode {
            None
        } else {
            Some(PlaidQuantizer::new(nbits, embedding_dimension)?)
        };
        Ok(Self {
            embedding_dimension,
            nbits,
            n_centroids: 0,
            centroids_data: Vec::new(),
            centroid_norms_neg_half: Vec::new(),
            quantizer,
        })
    }

    /// C++: `PlaidGlobalCentroids::Train`.
    pub fn train(
        &mut self,
        n_centroids: u32,
        embedding_data: &[f32],
        embedding_num: usize,
        iter_cnt: u32,
    ) -> Result<()> {
        if n_centroids == 0 || !n_centroids.is_multiple_of(8) {
            return Err(Error::Invalid(format!(
                "PlaidGlobalCentroids::Train: n_centroids must be a multiple of 8, got {n_centroids}"
            )));
        }
        let dim = self.embedding_dimension as usize;
        if embedding_data.len() < embedding_num * dim {
            return Err(Error::Invalid(
                "PlaidGlobalCentroids::Train: embedding buffer too short".into(),
            ));
        }
        self.n_centroids = n_centroids;
        // next-plaid's sampling bound: k * 256 embeddings.
        let max_sample_size = n_centroids as u64 * 256;
        let actual_sample_size = max_sample_size.min(embedding_num as u64) as usize;
        let training: Vec<f32> = if actual_sample_size < embedding_num {
            let ids = random_permutation(embedding_num, actual_sample_size);
            let mut sampled = Vec::with_capacity(actual_sample_size * dim);
            for id in ids {
                sampled
                    .extend_from_slice(&embedding_data[id as usize * dim..(id as usize + 1) * dim]);
            }
            sampled
        } else {
            embedding_data[..embedding_num * dim].to_vec()
        };
        self.centroids_data = kmeans_l2(
            dim,
            &training,
            actual_sample_size,
            n_centroids as usize,
            iter_cnt as usize,
        )?;
        self.compute_centroid_norms();

        if self.quantizer.is_none() {
            // ColBERTSaR mode: no residual quantizer.
            return Ok(());
        }
        // Sample up to 50K embeddings, assign each to a centroid, and train the
        // quantizer on the residuals.
        const MAX_SAMPLE_FOR_QUANTIZER: usize = 50_000;
        let sample_size = MAX_SAMPLE_FOR_QUANTIZER.min(embedding_num);
        let ids = random_permutation(embedding_num, sample_size);
        let mut sample = Vec::with_capacity(sample_size * dim);
        for &id in &ids {
            sample.extend_from_slice(&embedding_data[id as usize * dim..(id as usize + 1) * dim]);
        }
        let mut centroid_ids = vec![0u32; sample_size];
        self.find_nearest_centroids(&sample, sample_size, &mut centroid_ids)?;
        let mut residuals = vec![0f32; sample_size * dim];
        self.compute_residuals(&sample, sample_size, &centroid_ids, &mut residuals)?;
        self.quantizer
            .as_mut()
            .expect("checked above")
            .train(&residuals, sample_size)
    }

    /// C++: `PlaidGlobalCentroids::ComputeCentroidNorms`.
    fn compute_centroid_norms(&mut self) {
        let dim = self.embedding_dimension as usize;
        self.centroid_norms_neg_half = self
            .centroids_data
            .chunks_exact(dim)
            .map(|c| -0.5 * c.iter().map(|v| v * v).sum::<f32>())
            .collect();
    }

    pub fn is_trained(&self) -> bool {
        self.n_centroids > 0
    }
    pub fn n_centroids(&self) -> u32 {
        self.n_centroids
    }
    pub fn embedding_dimension(&self) -> u32 {
        self.embedding_dimension
    }
    pub fn nbits(&self) -> u32 {
        self.nbits
    }
    pub fn centroids_data(&self) -> &[f32] {
        &self.centroids_data
    }
    pub fn centroid_norms_neg_half(&self) -> &[f32] {
        &self.centroid_norms_neg_half
    }
    pub fn quantizer(&self) -> Option<&PlaidQuantizer> {
        self.quantizer.as_ref()
    }
    pub fn quantizer_mut(&mut self) -> Option<&mut PlaidQuantizer> {
        self.quantizer.as_mut()
    }

    /// C++: `PlaidGlobalCentroids::FindNearestCentroid`.
    pub fn find_nearest_centroid(&self, embedding: &[f32]) -> Result<u32> {
        if self.n_centroids == 0 {
            return Err(Error::Invalid(
                "PlaidGlobalCentroids::FindNearestCentroid: Centroids not trained".into(),
            ));
        }
        let dim = self.embedding_dimension as usize;
        if embedding.len() < dim {
            return Err(Error::Invalid(
                "PlaidGlobalCentroids::FindNearestCentroid: buffer too short".into(),
            ));
        }
        let mut max_neg = f32::MIN;
        let mut nearest = 0u32;
        for k in 0..self.n_centroids as usize {
            let c = &self.centroids_data[k * dim..(k + 1) * dim];
            let dot: f32 = embedding.iter().zip(c).map(|(a, b)| a * b).sum();
            let neg = dot + self.centroid_norms_neg_half[k];
            if neg > max_neg {
                max_neg = neg;
                nearest = k as u32;
            }
        }
        Ok(nearest)
    }

    /// C++: `PlaidGlobalCentroids::FindNearestCentroids`.
    pub fn find_nearest_centroids(
        &self,
        embeddings: &[f32],
        n_embeddings: usize,
        centroid_ids: &mut [u32],
    ) -> Result<()> {
        if self.n_centroids == 0 {
            return Err(Error::Invalid(
                "PlaidGlobalCentroids::FindNearestCentroids: Centroids not trained".into(),
            ));
        }
        let dim = self.embedding_dimension as usize;
        if embeddings.len() < n_embeddings * dim || centroid_ids.len() < n_embeddings {
            return Err(Error::Invalid(
                "PlaidGlobalCentroids::FindNearestCentroids: buffer too short".into(),
            ));
        }
        for i in 0..n_embeddings {
            let emb = &embeddings[i * dim..(i + 1) * dim];
            let mut max_neg = f32::MIN;
            let mut nearest = 0u32;
            for k in 0..self.n_centroids as usize {
                let c = &self.centroids_data[k * dim..(k + 1) * dim];
                let dot: f32 = emb.iter().zip(c).map(|(a, b)| a * b).sum();
                let neg = dot + self.centroid_norms_neg_half[k];
                if neg > max_neg {
                    max_neg = neg;
                    nearest = k as u32;
                }
            }
            centroid_ids[i] = nearest;
        }
        Ok(())
    }

    /// C++: `PlaidGlobalCentroids::ComputeCentroidScores`.
    pub fn compute_centroid_scores(
        &self,
        query: &[f32],
        n_query_tokens: usize,
    ) -> Result<Vec<f32>> {
        let dim = self.embedding_dimension as usize;
        if query.len() < n_query_tokens * dim {
            return Err(Error::Invalid(
                "PlaidGlobalCentroids::ComputeCentroidScores: buffer too short".into(),
            ));
        }
        let k = self.n_centroids as usize;
        let mut scores = vec![0f32; n_query_tokens * k];
        for i in 0..n_query_tokens {
            let q = &query[i * dim..(i + 1) * dim];
            for c in 0..k {
                let cent = &self.centroids_data[c * dim..(c + 1) * dim];
                let dot: f32 = q.iter().zip(cent).map(|(a, b)| a * b).sum();
                scores[i * k + c] = dot + self.centroid_norms_neg_half[c];
            }
        }
        Ok(scores)
    }

    /// C++: `PlaidGlobalCentroids::ComputeResiduals`.
    pub fn compute_residuals(
        &self,
        embeddings: &[f32],
        n_embeddings: usize,
        centroid_ids: &[u32],
        residuals: &mut [f32],
    ) -> Result<()> {
        let dim = self.embedding_dimension as usize;
        if embeddings.len() < n_embeddings * dim
            || centroid_ids.len() < n_embeddings
            || residuals.len() < n_embeddings * dim
        {
            return Err(Error::Invalid(
                "PlaidGlobalCentroids::ComputeResiduals: buffer too short".into(),
            ));
        }
        for i in 0..n_embeddings {
            let emb = &embeddings[i * dim..(i + 1) * dim];
            let cid = centroid_ids[i] as usize;
            let centroid = &self.centroids_data[cid * dim..(cid + 1) * dim];
            for j in 0..dim {
                residuals[i * dim + j] = emb[j] - centroid[j];
            }
        }
        Ok(())
    }

    /// C++: `PlaidGlobalCentroids::Save` / `SaveToPtr`.
    pub fn to_bytes(&self) -> Vec<u8> {
        let mut out = Vec::new();
        out.extend_from_slice(&self.embedding_dimension.to_le_bytes());
        out.extend_from_slice(&self.nbits.to_le_bytes());
        out.extend_from_slice(&self.n_centroids.to_le_bytes());
        out.push(u8::from(self.quantizer.is_some()));
        for v in &self.centroids_data {
            out.extend_from_slice(&v.to_le_bytes());
        }
        for v in &self.centroid_norms_neg_half {
            out.extend_from_slice(&v.to_le_bytes());
        }
        if let Some(q) = &self.quantizer {
            out.extend_from_slice(&q.to_bytes());
        }
        out
    }

    /// C++: `PlaidGlobalCentroids::Load` / `LoadFromPtr`.
    pub fn from_bytes(
        embedding_dimension: u32,
        nbits: u32,
        colbertsar_mode: bool,
        bytes: &[u8],
    ) -> Result<(Self, usize)> {
        let mut this = Self::new(embedding_dimension, nbits, colbertsar_mode)?;
        let mut at = 0usize;
        let read_u32 = |at: &mut usize| -> Result<u32> {
            let v = bytes
                .get(*at..*at + 4)
                .ok_or_else(|| Error::Invalid("PlaidGlobalCentroids::Load: truncated".into()))?;
            *at += 4;
            Ok(u32::from_le_bytes(v.try_into().unwrap()))
        };
        let stored_dim = read_u32(&mut at)?;
        let stored_nbits = read_u32(&mut at)?;
        if stored_dim != embedding_dimension || stored_nbits != nbits {
            return Err(Error::Invalid(
                "PlaidGlobalCentroids::Load: header mismatch".into(),
            ));
        }
        this.n_centroids = read_u32(&mut at)?;
        let has_quantizer = *bytes
            .get(at)
            .ok_or_else(|| Error::Invalid("PlaidGlobalCentroids::Load: truncated".into()))?
            != 0;
        at += 1;
        let count = this.n_centroids as usize * embedding_dimension as usize;
        this.centroids_data = (0..count)
            .map(|_| {
                let v = bytes.get(at..at + 4).ok_or_else(|| {
                    Error::Invalid("PlaidGlobalCentroids::Load: truncated".into())
                })?;
                at += 4;
                Ok(f32::from_le_bytes(v.try_into().unwrap()))
            })
            .collect::<Result<_>>()?;
        this.centroid_norms_neg_half = (0..this.n_centroids as usize)
            .map(|_| {
                let v = bytes.get(at..at + 4).ok_or_else(|| {
                    Error::Invalid("PlaidGlobalCentroids::Load: truncated".into())
                })?;
                at += 4;
                Ok(f32::from_le_bytes(v.try_into().unwrap()))
            })
            .collect::<Result<_>>()?;
        if has_quantizer {
            let quantizer = this.quantizer.as_ref().ok_or_else(|| {
                Error::Invalid(
                    "PlaidGlobalCentroids::Load: file has quantizer but instance was created without one"
                        .into(),
                )
            })?;
            let (loaded, used) = PlaidQuantizer::from_bytes(
                quantizer.nbits(),
                quantizer.embedding_dim(),
                &bytes[at..],
            )?;
            at += used;
            this.quantizer = Some(loaded);
        }
        Ok((this, at))
    }

    /// C++: `PlaidGlobalCentroids::MemUsage`.
    pub fn mem_usage(&self) -> usize {
        std::mem::size_of::<Self>()
            + self.centroids_data.len() * 4
            + self.centroid_norms_neg_half.len() * 4
            + self.quantizer.as_ref().map_or(0, PlaidQuantizer::mem_usage)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn seeded_data(n: usize, dim: usize, seed: u64) -> Vec<f32> {
        let mut state = seed.max(1);
        (0..n * dim)
            .map(|_| {
                state ^= state << 13;
                state ^= state >> 7;
                state ^= state << 17;
                ((state >> 40) as f32 / (1u64 << 24) as f32) - 0.5
            })
            .collect()
    }

    #[test]
    fn trains_centroids_and_assigns_nearest() {
        let dim = 8u32;
        let data = seeded_data(512, dim as usize, 42);
        let mut c = PlaidGlobalCentroids::new(dim, 4, false).unwrap();
        c.train(8, &data, 512, 4).unwrap();
        assert_eq!(c.n_centroids(), 8);
        assert_eq!(c.centroids_data().len(), 8 * dim as usize);
        assert!(c.quantizer().unwrap().is_trained());

        let mut ids = vec![0u32; 512];
        c.find_nearest_centroids(&data, 512, &mut ids).unwrap();
        // The nearest-centroid assignment must reproduce the k-means means.
        let mut counts = [0usize; 8];
        for &id in &ids {
            counts[id as usize] += 1;
        }
        assert_eq!(counts.iter().sum::<usize>(), 512);
        assert!(counts.iter().all(|&n| n > 0));
        assert_eq!(
            c.find_nearest_centroid(&data[..dim as usize]).unwrap(),
            ids[0]
        );
    }

    #[test]
    fn residuals_reconstruct_the_embedding() {
        let dim = 4u32;
        let data = seeded_data(64, dim as usize, 7);
        let mut c = PlaidGlobalCentroids::new(dim, 4, false).unwrap();
        c.train(8, &data, 64, 2).unwrap();
        let ids = [0u32, 1, 2, 3];
        let mut residuals = vec![0f32; 4 * dim as usize];
        c.compute_residuals(&data, 4, &ids, &mut residuals).unwrap();
        for i in 0..4 {
            let cid = ids[i] as usize;
            for d in 0..dim as usize {
                let centroid = c.centroids_data()[cid * dim as usize + d];
                assert!(
                    (residuals[i * dim as usize + d] - (data[i * dim as usize + d] - centroid))
                        .abs()
                        < 1e-6
                );
            }
        }
    }

    #[test]
    fn save_and_load_roundtrip() {
        let dim = 8u32;
        let data = seeded_data(256, dim as usize, 11);
        let mut c = PlaidGlobalCentroids::new(dim, 4, false).unwrap();
        c.train(8, &data, 256, 2).unwrap();
        let bytes = c.to_bytes();
        let (loaded, used) = PlaidGlobalCentroids::from_bytes(dim, 4, false, &bytes).unwrap();
        assert_eq!(used, bytes.len());
        assert_eq!(loaded.n_centroids(), c.n_centroids());
        assert_eq!(loaded.centroids_data(), c.centroids_data());
        assert_eq!(
            loaded.centroid_norms_neg_half(),
            c.centroid_norms_neg_half()
        );
        assert_eq!(
            loaded.quantizer().unwrap().bucket_weights(),
            c.quantizer().unwrap().bucket_weights()
        );
    }

    #[test]
    fn colbertsar_mode_has_no_quantizer() {
        let dim = 8u32;
        let data = seeded_data(64, dim as usize, 3);
        let mut c = PlaidGlobalCentroids::new(dim, 4, true).unwrap();
        c.train(8, &data, 64, 2).unwrap();
        assert!(c.quantizer().is_none());
        assert!(c.is_trained());
    }
}
