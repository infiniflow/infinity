//! Residual quantizer of the PLAID multi-vector index.
//!
//! C++: `PlaidQuantizer` (`src/storage/knn_index/plaid/plaid_quantizer.cppm` and
//! `plaid_quantizer_impl.cpp`). Residuals are centred by their global mean,
//! split into `2^nbits` quantile buckets (2- or 4-bit), packed most-significant
//! bit first, and reconstructed as `centroid + bucket_weight` followed by L2
//! normalization.

use infinity_common::{Error, Result};

/// Maximum number of residual *values* used for training; larger inputs are
/// reservoir-sampled down to this bound (`MAX_RESIDUAL_VALUES`).
pub const MAX_RESIDUAL_VALUES: u64 = 50_000 * 1024;

/// C++: `PlaidQuantizer`.
#[derive(Debug, Clone)]
pub struct PlaidQuantizer {
    nbits: u32,
    embedding_dim: u32,
    n_buckets: u32,
    packed_dim: u32,
    /// `n_buckets - 1` quantile boundaries.
    bucket_cutoffs: Vec<f32>,
    /// `n_buckets` reconstruction values.
    bucket_weights: Vec<f32>,
    /// Mean residual removed before bucketing and added back on reconstruction.
    avg_residual: f32,
    /// Bit reversal per byte, used to unpack the MSB-first stream.
    byte_reversed_bits_map: [u8; 256],
    /// `[256, keys_per_byte]` bucket indices of each packed byte.
    bucket_weight_indices_lookup: Vec<u8>,
}

impl PlaidQuantizer {
    /// C++: `PlaidQuantizer::PlaidQuantizer`.
    pub fn new(nbits: u32, embedding_dim: u32) -> Result<Self> {
        if nbits != 2 && nbits != 4 {
            return Err(Error::Invalid(
                "PlaidQuantizer: nbits must be 2 or 4".into(),
            ));
        }
        if embedding_dim == 0 {
            return Err(Error::Invalid(
                "PlaidQuantizer: embedding_dim must be > 0".into(),
            ));
        }
        let n_buckets = 1u32 << nbits;
        let packed_dim = (embedding_dim * nbits).div_ceil(8);
        let (byte_reversed_bits_map, bucket_weight_indices_lookup) = build_lookup_tables(nbits);
        Ok(Self {
            nbits,
            embedding_dim,
            n_buckets,
            packed_dim,
            bucket_cutoffs: Vec::new(),
            bucket_weights: Vec::new(),
            avg_residual: 0.0,
            byte_reversed_bits_map,
            bucket_weight_indices_lookup,
        })
    }

    pub fn nbits(&self) -> u32 {
        self.nbits
    }
    pub fn embedding_dim(&self) -> u32 {
        self.embedding_dim
    }
    pub fn n_buckets(&self) -> u32 {
        self.n_buckets
    }
    pub fn packed_dim(&self) -> u32 {
        self.packed_dim
    }
    pub fn avg_residual(&self) -> f32 {
        self.avg_residual
    }
    pub fn bucket_cutoffs(&self) -> &[f32] {
        &self.bucket_cutoffs
    }
    pub fn bucket_weights(&self) -> &[f32] {
        &self.bucket_weights
    }
    pub fn is_trained(&self) -> bool {
        !self.bucket_weights.is_empty()
    }

    /// C++: `PlaidQuantizer::Train`.
    ///
    /// Computes the global mean in one pass, then reserves the (possibly
    /// sampled) residuals, sorts them and takes the quantile boundaries and
    /// midpoints used by next-plaid.
    pub fn train(&mut self, residuals: &[f32], n_embeddings: usize) -> Result<()> {
        if n_embeddings == 0 {
            return Err(Error::Invalid(
                "PlaidQuantizer::Train: n_embeddings must be > 0".into(),
            ));
        }
        let dim = self.embedding_dim as usize;
        let total_values = n_embeddings * dim;
        if residuals.len() < total_values {
            return Err(Error::Invalid(format!(
                "PlaidQuantizer::Train: expected {total_values} values, got {}",
                residuals.len()
            )));
        }
        let sum: f64 = residuals[..total_values].iter().map(|&v| v as f64).sum();
        self.avg_residual = (sum / total_values as f64) as f32;

        let max_values = MAX_RESIDUAL_VALUES;
        let max_embeddings_for_limit = (max_values / dim as u64).max(1) as usize;
        let mut all: Vec<f32> = Vec::new();
        if total_values as u64 <= max_values {
            all.extend(
                residuals[..total_values]
                    .iter()
                    .map(|&v| v - self.avg_residual),
            );
        } else {
            // Reservoir sampling over embedding rows (`Train`'s memory bound).
            let count = max_embeddings_for_limit.min(n_embeddings);
            let mut indices: Vec<usize> = (0..count).collect();
            let mut rng = 0x9E37_79B9_7F4A_7C15u64;
            for i in count..n_embeddings {
                rng ^= rng << 13;
                rng ^= rng >> 7;
                rng ^= rng << 17;
                let j = (rng % (i as u64 + 1)) as usize;
                if j < count {
                    indices[j] = i;
                }
            }
            all.reserve(count * dim);
            for &idx in &indices {
                let base = idx * dim;
                all.extend(
                    residuals[base..base + dim]
                        .iter()
                        .map(|&v| v - self.avg_residual),
                );
            }
        }
        all.sort_by(f32::total_cmp);

        let len = all.len();
        let last = len - 1;
        self.bucket_cutoffs = (1..self.n_buckets)
            .map(|i| {
                let q = i as f64 / self.n_buckets as f64;
                all[((q * len as f64) as usize).min(last)]
            })
            .collect();
        self.bucket_weights = (0..self.n_buckets)
            .map(|i| {
                let q = (i as f64 + 0.5) / self.n_buckets as f64;
                all[((q * len as f64) as usize).min(last)] + self.avg_residual
            })
            .collect();
        Ok(())
    }

    /// C++: `PlaidQuantizer::Quantize`.
    ///
    /// Packs each element's bucket index most-significant bit first into the
    /// row's `packed_dim` bytes.
    pub fn quantize(&self, residuals: &[f32], n_embeddings: usize) -> Result<Vec<u8>> {
        if !self.is_trained() {
            return Err(Error::Invalid(
                "PlaidQuantizer::Quantize: must call Train first".into(),
            ));
        }
        let dim = self.embedding_dim as usize;
        let packed_dim = self.packed_dim as usize;
        if residuals.len() < n_embeddings * dim {
            return Err(Error::Invalid(
                "PlaidQuantizer::Quantize: residual buffer too short".into(),
            ));
        }
        let mut packed = vec![0u8; n_embeddings * packed_dim];
        for i in 0..n_embeddings {
            let residual = &residuals[i * dim..(i + 1) * dim];
            let row = &mut packed[i * packed_dim..(i + 1) * packed_dim];
            let mut bit_idx = 0usize;
            for &r in residual {
                let val = r - self.avg_residual;
                let mut bucket = 0u32;
                for (k, cutoff) in self.bucket_cutoffs.iter().enumerate() {
                    if val > *cutoff {
                        bucket = k as u32 + 1;
                    } else {
                        break;
                    }
                }
                for b in 0..self.nbits {
                    let bit = ((bucket >> b) & 1) as u8;
                    let byte_idx = bit_idx / 8;
                    let bit_pos = 7 - (bit_idx % 8);
                    row[byte_idx] |= bit << bit_pos;
                    bit_idx += 1;
                }
            }
        }
        Ok(packed)
    }

    /// C++: `PlaidQuantizer::Dequantize`.
    ///
    /// Reconstructs `centroid + bucket_weight` and L2-normalizes each vector.
    pub fn dequantize(
        &self,
        centroid_ids: &[u32],
        packed_residuals: &[u8],
        n_embeddings: usize,
        output: &mut [f32],
        centroids_data: &[f32],
        n_centroids: u32,
    ) -> Result<()> {
        if !self.is_trained() {
            return Err(Error::Invalid(
                "PlaidQuantizer::Dequantize: must call Train first".into(),
            ));
        }
        let dim = self.embedding_dim as usize;
        let packed_dim = self.packed_dim as usize;
        let keys_per_byte = (8 / self.nbits) as usize;
        if centroid_ids.len() < n_embeddings
            || packed_residuals.len() < n_embeddings * packed_dim
            || output.len() < n_embeddings * dim
            || (n_centroids as usize) * dim > centroids_data.len()
        {
            return Err(Error::Invalid(
                "PlaidQuantizer::Dequantize: buffer too short".into(),
            ));
        }
        for i in 0..n_embeddings {
            let centroid_id = centroid_ids[i] as usize;
            if centroid_id >= n_centroids as usize {
                return Err(Error::Invalid(format!(
                    "PlaidQuantizer::Dequantize: centroid id {centroid_id} out of range"
                )));
            }
            let packed = &packed_residuals[i * packed_dim..(i + 1) * packed_dim];
            let out = &mut output[i * dim..(i + 1) * dim];
            let centroid = &centroids_data[centroid_id * dim..(centroid_id + 1) * dim];
            let mut residual_idx = 0usize;
            for &byte_val in packed {
                let reversed = self.byte_reversed_bits_map[byte_val as usize];
                let indices = &self.bucket_weight_indices_lookup[reversed as usize * keys_per_byte
                    ..reversed as usize * keys_per_byte + keys_per_byte];
                for &bucket in indices {
                    if residual_idx >= dim {
                        break;
                    }
                    out[residual_idx] =
                        centroid[residual_idx] + self.bucket_weights[bucket as usize];
                    residual_idx += 1;
                }
            }
            let norm_sq: f32 = out.iter().map(|v| v * v).sum();
            let norm = norm_sq.sqrt();
            if norm > 1e-12 {
                for v in out.iter_mut() {
                    *v /= norm;
                }
            }
        }
        Ok(())
    }

    /// C++: `PlaidQuantizer::GetIPDistanceTable`.
    pub fn ip_distance_table(&self, query: &[f32]) -> Result<Vec<f32>> {
        let mut table = vec![0.0f32; self.embedding_dim as usize * self.n_buckets as usize];
        self.ip_distance_table_to(query, &mut table)?;
        Ok(table)
    }

    /// C++: `PlaidQuantizer::GetIPDistanceTableTo`.
    pub fn ip_distance_table_to(&self, query: &[f32], output: &mut [f32]) -> Result<()> {
        if !self.is_trained() {
            return Err(Error::Invalid(
                "PlaidQuantizer::GetIPDistanceTableTo: must call Train first".into(),
            ));
        }
        let dim = self.embedding_dim as usize;
        let n_buckets = self.n_buckets as usize;
        if query.len() < dim || output.len() < dim * n_buckets {
            return Err(Error::Invalid(
                "PlaidQuantizer::GetIPDistanceTableTo: buffer too short".into(),
            ));
        }
        for d in 0..dim {
            for b in 0..n_buckets {
                output[d * n_buckets + b] = query[d] * self.bucket_weights[b];
            }
        }
        Ok(())
    }

    /// C++: `PlaidQuantizer::BuildPositionLUT`.
    pub fn build_position_lut(&self, ip_table: &[f32], lut_out: &mut [f32]) -> Result<()> {
        let keys_per_byte = (8 / self.nbits) as usize;
        let n_buckets = self.n_buckets as usize;
        let packed_dim = self.packed_dim as usize;
        if ip_table.len() < self.embedding_dim as usize * n_buckets
            || lut_out.len() < packed_dim * 256
        {
            return Err(Error::Invalid(
                "PlaidQuantizer::BuildPositionLUT: buffer too short".into(),
            ));
        }
        for j in 0..packed_dim {
            for v in 0..256 {
                let reversed = self.byte_reversed_bits_map[v] as usize;
                let mut contrib = 0.0f32;
                for k in 0..keys_per_byte {
                    let bucket =
                        self.bucket_weight_indices_lookup[reversed * keys_per_byte + k] as usize;
                    contrib += ip_table[(j * keys_per_byte + k) * n_buckets + bucket];
                }
                lut_out[j * 256 + v] = contrib;
            }
        }
        Ok(())
    }

    /// C++: `PlaidQuantizer::GetSingleIPDistanceFromLUT`.
    pub fn single_ip_distance_from_lut(
        &self,
        embedding_id: usize,
        position_lut: &[f32],
        packed_residuals: &[u8],
    ) -> f32 {
        let packed_dim = self.packed_dim as usize;
        let packed = &packed_residuals[embedding_id * packed_dim..(embedding_id + 1) * packed_dim];
        let mut result = 0.0f32;
        let mut j = 0usize;
        while j + 4 <= packed_dim {
            for k in 0..4 {
                result += position_lut[(j + k) * 256 + packed[j + k] as usize];
            }
            j += 4;
        }
        while j < packed_dim {
            result += position_lut[j * 256 + packed[j] as usize];
            j += 1;
        }
        result
    }

    /// C++: `PlaidQuantizer::GetSingleIPDistance`.
    pub fn single_ip_distance(
        &self,
        embedding_id: usize,
        ip_table: &[f32],
        packed_residuals: &[u8],
    ) -> f32 {
        let dim = self.embedding_dim as usize;
        let packed_dim = self.packed_dim as usize;
        let keys_per_byte = (8 / self.nbits) as usize;
        let n_buckets = self.n_buckets as usize;
        let packed = &packed_residuals[embedding_id * packed_dim..(embedding_id + 1) * packed_dim];
        let mut result = 0.0f32;
        let mut residual_idx = 0usize;
        for &byte_val in packed {
            let reversed = self.byte_reversed_bits_map[byte_val as usize];
            let indices = &self.bucket_weight_indices_lookup[reversed as usize * keys_per_byte
                ..reversed as usize * keys_per_byte + keys_per_byte];
            for &bucket in indices {
                if residual_idx >= dim {
                    break;
                }
                result += ip_table[residual_idx * n_buckets + bucket as usize];
                residual_idx += 1;
            }
        }
        result
    }

    /// C++: `PlaidQuantizer::GetMultipleIPDistance`.
    pub fn multiple_ip_distance(
        &self,
        embedding_offset: usize,
        n_embeddings: usize,
        ip_table: &[f32],
        packed_residuals: &[u8],
        output: &mut [f32],
    ) {
        for (i, slot) in output.iter_mut().take(n_embeddings).enumerate() {
            *slot = self.single_ip_distance(embedding_offset + i, ip_table, packed_residuals);
        }
    }

    /// C++: `PlaidQuantizer::Save` / `SaveToPtr`.
    pub fn to_bytes(&self) -> Vec<u8> {
        let mut out =
            Vec::with_capacity(32 + self.bucket_cutoffs.len() * 4 + self.bucket_weights.len() * 4);
        out.extend_from_slice(&self.nbits.to_le_bytes());
        out.extend_from_slice(&self.embedding_dim.to_le_bytes());
        out.extend_from_slice(&self.avg_residual.to_le_bytes());
        let n_cutoffs = self.bucket_cutoffs.len() as u32;
        out.extend_from_slice(&n_cutoffs.to_le_bytes());
        for c in &self.bucket_cutoffs {
            out.extend_from_slice(&c.to_le_bytes());
        }
        out.extend_from_slice(&self.n_buckets.to_le_bytes());
        for w in &self.bucket_weights {
            out.extend_from_slice(&w.to_le_bytes());
        }
        out
    }

    /// C++: `PlaidQuantizer::Load` / `LoadFromPtr`.
    pub fn from_bytes(nbits: u32, embedding_dim: u32, bytes: &[u8]) -> Result<(Self, usize)> {
        let mut quantizer = Self::new(nbits, embedding_dim)?;
        let mut at = 0usize;
        let read_u32 = |at: &mut usize| -> Result<u32> {
            let v = bytes
                .get(*at..*at + 4)
                .ok_or_else(|| Error::Invalid("PlaidQuantizer::Load: truncated".into()))?;
            *at += 4;
            Ok(u32::from_le_bytes(v.try_into().unwrap()))
        };
        let read_f32 = |at: &mut usize| -> Result<f32> {
            let v = bytes
                .get(*at..*at + 4)
                .ok_or_else(|| Error::Invalid("PlaidQuantizer::Load: truncated".into()))?;
            *at += 4;
            Ok(f32::from_le_bytes(v.try_into().unwrap()))
        };
        let read_nbits = read_u32(&mut at)?;
        let read_dim = read_u32(&mut at)?;
        if read_nbits != nbits || read_dim != embedding_dim {
            return Err(Error::Invalid(
                "PlaidQuantizer::Load: dimension mismatch".into(),
            ));
        }
        quantizer.avg_residual = read_f32(&mut at)?;
        let n_cutoffs = read_u32(&mut at)? as usize;
        quantizer.bucket_cutoffs = (0..n_cutoffs)
            .map(|_| read_f32(&mut at))
            .collect::<Result<_>>()?;
        let n_buckets = read_u32(&mut at)?;
        if n_buckets != quantizer.n_buckets {
            return Err(Error::Invalid(
                "PlaidQuantizer::Load: n_buckets mismatch".into(),
            ));
        }
        quantizer.bucket_weights = (0..n_buckets)
            .map(|_| read_f32(&mut at))
            .collect::<Result<_>>()?;
        Ok((quantizer, at))
    }

    /// C++: `PlaidQuantizer::MemUsage`.
    pub fn mem_usage(&self) -> usize {
        std::mem::size_of::<Self>()
            + self.bucket_cutoffs.len() * 4
            + self.bucket_weights.len() * 4
            + self.byte_reversed_bits_map.len()
            + self.bucket_weight_indices_lookup.len()
    }

    /// C++: `PlaidQuantizer::CopyFrom`.
    pub fn copy_from(&mut self, other: &Self) -> Result<()> {
        if other.nbits != self.nbits || other.embedding_dim != self.embedding_dim {
            return Err(Error::Invalid(
                "PlaidQuantizer::CopyFrom: dimension mismatch".into(),
            ));
        }
        self.avg_residual = other.avg_residual;
        self.bucket_cutoffs.clone_from(&other.bucket_cutoffs);
        self.bucket_weights.clone_from(&other.bucket_weights);
        Ok(())
    }
}

/// C++: `PlaidQuantizer::BuildLookupTables`.
fn build_lookup_tables(nbits: u32) -> ([u8; 256], Vec<u8>) {
    let nbits_mask = (1u32 << nbits) - 1;
    let mut reversed_map = [0u8; 256];
    for (i, slot) in reversed_map.iter_mut().enumerate() {
        let val = i as u32;
        let mut out = 0u32;
        let mut pos = 8i32;
        while pos >= nbits as i32 {
            let segment = (val >> (pos - nbits as i32)) & nbits_mask;
            let mut rev_segment = 0u32;
            for k in 0..nbits {
                if segment & (1u32 << k) != 0 {
                    rev_segment |= 1u32 << (nbits - 1 - k);
                }
            }
            out |= rev_segment;
            if pos > nbits as i32 {
                out <<= nbits;
            }
            pos -= nbits as i32;
        }
        *slot = out as u8;
    }
    let keys_per_byte = (8 / nbits) as usize;
    let mut indices = vec![0u8; 256 * keys_per_byte];
    let mask = (1u32 << nbits) - 1;
    for byte_val in 0..256u32 {
        for k in 0..keys_per_byte {
            let shift = (keys_per_byte - 1 - k) * nbits as usize;
            let index = (byte_val >> shift) & mask;
            indices[byte_val as usize * keys_per_byte + k] = index as u8;
        }
    }
    (reversed_map, indices)
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
    }

    /// C++: `PlaidQuantizerTest.test_train_quantize_dequantize`.
    #[test]
    fn train_quantize_dequantize() {
        let (nbits, dim, n) = (4u32, 32u32, 256usize);
        let mut q = PlaidQuantizer::new(nbits, dim).unwrap();
        assert_eq!(q.nbits(), nbits);
        assert_eq!(q.embedding_dim(), dim);
        assert_eq!(q.n_buckets(), 16);
        assert_eq!(q.packed_dim(), (dim * nbits).div_ceil(8));

        let mut rng = Normal::new(42);
        let residuals: Vec<f32> = (0..n * dim as usize)
            .map(|_| rng.next_f32() * 0.1)
            .collect();
        q.train(&residuals, n).unwrap();
        let packed = q.quantize(&residuals, n).unwrap();
        assert_eq!(packed.len(), n * q.packed_dim() as usize);

        let centroid_ids = vec![0u32; n];
        let centroids = vec![0f32; dim as usize];
        let mut reconstructed = vec![0f32; n * dim as usize];
        q.dequantize(&centroid_ids, &packed, n, &mut reconstructed, &centroids, 1)
            .unwrap();
        assert!(reconstructed.iter().all(|v| v.is_finite()));
    }

    /// C++: `PlaidQuantizerTest.test_2bit_quantization`.
    #[test]
    fn two_bit_quantization() {
        let (nbits, dim, n) = (2u32, 16u32, 64usize);
        let mut q = PlaidQuantizer::new(nbits, dim).unwrap();
        assert_eq!(q.n_buckets(), 4);
        let mut rng = Normal::new(42);
        let residuals: Vec<f32> = (0..n * dim as usize)
            .map(|_| rng.next_f32() * 0.1)
            .collect();
        q.train(&residuals, n).unwrap();
        let packed = q.quantize(&residuals, n).unwrap();
        assert_eq!(packed.len(), n * ((dim * nbits).div_ceil(8) as usize));
    }

    #[test]
    fn ip_table_and_position_lut_agree() {
        let (nbits, dim, n) = (4u32, 8u32, 4usize);
        let mut q = PlaidQuantizer::new(nbits, dim).unwrap();
        let residuals: Vec<f32> = (0..n * dim as usize).map(|i| i as f32 * 0.01).collect();
        q.train(&residuals, n).unwrap();
        let packed = q.quantize(&residuals, n).unwrap();
        let query: Vec<f32> = (0..dim).map(|i| i as f32 + 0.5).collect();
        let table = q.ip_distance_table(&query).unwrap();
        let mut lut = vec![0f32; q.packed_dim() as usize * 256];
        q.build_position_lut(&table, &mut lut).unwrap();
        for id in 0..n {
            let direct = q.single_ip_distance(id, &table, &packed);
            let fast = q.single_ip_distance_from_lut(id, &lut, &packed);
            assert!((direct - fast).abs() < 1e-5, "{direct} vs {fast}");
        }
    }

    #[test]
    fn save_and_load_roundtrip() {
        let mut q = PlaidQuantizer::new(4, 8).unwrap();
        let residuals: Vec<f32> = (0..32).map(|i| (i as f32).sin()).collect();
        q.train(&residuals, 4).unwrap();
        let bytes = q.to_bytes();
        let (loaded, used) = PlaidQuantizer::from_bytes(4, 8, &bytes).unwrap();
        assert_eq!(used, bytes.len());
        assert_eq!(loaded.avg_residual(), q.avg_residual());
        assert_eq!(loaded.bucket_cutoffs(), q.bucket_cutoffs());
        assert_eq!(loaded.bucket_weights(), q.bucket_weights());
    }
}
