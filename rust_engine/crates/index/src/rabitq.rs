//! RaBitQ: randomized binary quantization for HNSW.
//!
//! Port of `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm`
//! and the RaBitQ distance kernels in `dist_func_{l2,cos,ip}.cppm`.
//!
//! A vector is rotated by a random orthogonal matrix, centered on the rotated
//! centroid, normalized and stored as a 1-bit sign code plus four scalars
//! (`raw_norm`, `norm`, `sum`, `error`). The query is quantized to u8 per
//! dimension (mean scalar quantization); both sides recover the inner product
//! with the RaBitQ estimators before the metric is recomputed.

/// The metric decides the recovery formula.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Metric {
    L2,
    Ip,
    Cos,
}

/// One compressed vector: C++ `RabitqStoreData<f32, u8>`.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct RabitqData {
    /// `||o||^2` of the raw (padded) vector.
    pub raw_norm: f32,
    /// `||o_r - c||` after the rotation.
    pub norm: f32,
    /// Number of set bits in the code.
    pub sum: f32,
    /// `<bar{o}, o>`: the quantization error factor.
    pub error: f32,
    /// One sign bit per dimension, LSB first.
    pub code: Vec<u8>,
}

/// One quantized query: C++ `RabitqQueryData<f32, u8>`.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct RabitqQuery {
    pub raw_norm: f32,
    pub norm: f32,
    pub sum: f32,
    pub lower_bound: f32,
    pub delta: f32,
    pub code: Vec<u8>,
}

const TOLERANCE: f32 = 1e-5;
const COMPRESS_BUCKET_SIZE: f32 = 255.0;

fn is_approx_zero(x: f32) -> bool {
    x.abs() < TOLERANCE
}

/// C++ `AlignUp(origin_dim, 8)`.
fn align_up(n: usize, a: usize) -> usize {
    n.div_ceil(a) * a
}

/// C++ `GenerateRandomOrthogonalMatrix`: identity composed with random Givens
/// rotations. The reference seeds from `std::random_device`; the seed here is
/// explicit so a built index is reproducible (the matrix is then persisted).
pub fn random_orthogonal_matrix(dim: usize, seed: u64) -> Vec<f32> {
    let mut rom = vec![0.0f32; dim * dim];
    for i in 0..dim {
        rom[i * dim + i] = 1.0;
    }
    let mut state = seed | 1;
    let mut next = || {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        (state >> 11) as f32 / (1u64 << 53) as f32
    };
    for i in 0..dim {
        for j in (i + 1)..dim {
            let angle = 2.0 * std::f32::consts::PI * (next() * 2.0 - 1.0);
            let (s, c) = angle.sin_cos();
            for k in 0..dim {
                let q_ik = rom[k * dim + i] * c - rom[k * dim + j] * s;
                let q_jk = rom[k * dim + i] * s + rom[k * dim + j] * c;
                rom[k * dim + i] = q_ik;
                rom[k * dim + j] = q_jk;
            }
        }
    }
    rom
}

/// `row_vector * matrix` (C++ `matrixA_multiply_matrixB_output_to_C` with
/// `1 x dim` by `dim x dim`, row-major).
fn rotate(v: &[f32], rot: &[f32], dim: usize) -> Vec<f32> {
    let mut out = vec![0.0f32; dim];
    for (j, o) in out.iter_mut().enumerate() {
        let mut sum = 0.0f32;
        for (i, x) in v.iter().enumerate().take(dim) {
            sum += x * rot[i * dim + j];
        }
        *o = sum;
    }
    out
}

/// `row_vector * matrix^T` (the inverse rotation).
fn inverse_rotate(v: &[f32], rot: &[f32], dim: usize) -> Vec<f32> {
    let mut out = vec![0.0f32; dim];
    for (j, o) in out.iter_mut().enumerate() {
        let mut sum = 0.0f32;
        for (i, x) in v.iter().enumerate().take(dim) {
            sum += x * rot[j * dim + i];
        }
        *o = sum;
    }
    out
}

/// The RaBitQ store of one chunk.
#[derive(Clone, Debug, PartialEq)]
pub struct RabitqStore {
    origin_dim: usize,
    dim: usize,
    rom: Vec<f32>,
    rot_centroid: Vec<f32>,
    metric: Metric,
    data: Vec<RabitqData>,
}

impl RabitqStore {
    /// C++ `RabitqVecStoreMeta::Make(origin_dim)`.
    pub fn new(origin_dim: usize, metric: Metric, seed: u64) -> Self {
        let dim = align_up(origin_dim, 8);
        Self {
            origin_dim,
            dim,
            rom: random_orthogonal_matrix(dim, seed),
            rot_centroid: vec![0.0f32; dim],
            metric,
            data: Vec::new(),
        }
    }

    pub fn origin_dim(&self) -> usize {
        self.origin_dim
    }

    pub fn dim(&self) -> usize {
        self.dim
    }

    pub fn metric(&self) -> Metric {
        self.metric
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn is_empty(&self) -> bool {
        self.data.is_empty()
    }

    pub fn rom(&self) -> &[f32] {
        &self.rom
    }

    pub fn rot_centroid(&self) -> &[f32] {
        &self.rot_centroid
    }

    pub fn data(&self) -> &[RabitqData] {
        &self.data
    }

    /// Bytes one compressed vector occupies (C++ `compress_data_size`).
    pub fn record_size(&self) -> usize {
        16 + self.dim / 8
    }

    fn padded(&self, v: &[f32]) -> Vec<f32> {
        let mut out = vec![0.0f32; self.dim];
        out[..v.len().min(self.origin_dim)].copy_from_slice(&v[..v.len().min(self.origin_dim)]);
        out
    }

    /// Sets the rotated centroid without storing any code (`Optimize`'s
    /// training half).
    pub fn train<V: AsRef<[f32]>>(&mut self, vectors: &[V]) {
        if vectors.is_empty() {
            return;
        }
        let mut centroid = vec![0.0f32; self.dim];
        for v in vectors {
            let p = self.padded(v.as_ref());
            for (j, x) in p.iter().enumerate() {
                centroid[j] += x;
            }
        }
        let n = vectors.len() as f32;
        for c in &mut centroid {
            *c /= n;
        }
        self.rot_centroid = rotate(&centroid, &self.rom, self.dim);
    }

    /// C++ `RabitqVecStoreMeta::Optimize`: the rotated centroid is the mean of
    /// the raw vectors, rotated; then the store is (re)compressed.
    pub fn build(&mut self, vectors: &[Vec<f32>]) {
        self.train(vectors);
        self.data.clear();
        for v in vectors {
            self.compress(v);
        }
    }

    /// C++ `CompressToCode`.
    pub fn compress(&mut self, v: &[f32]) -> &RabitqData {
        let src = self.padded(v);
        let raw_norm: f32 = src.iter().map(|x| x * x).sum();
        let mut rot = rotate(&src, &self.rom, self.dim);
        let mut norm: f32 = rot
            .iter()
            .zip(&self.rot_centroid)
            .map(|(a, b)| (a - b) * (a - b))
            .sum();
        norm = if is_approx_zero(norm) {
            1.0
        } else {
            norm.sqrt()
        };
        for (r, c) in rot.iter_mut().zip(&self.rot_centroid) {
            *r = (*r - *c) / norm;
        }
        let mut code = vec![0u8; self.dim / 8];
        let mut sum = 0.0f32;
        for (d, x) in rot.iter().enumerate() {
            if *x >= 0.0 {
                sum += 1.0;
                code[d / 8] |= 1 << (d % 8);
            }
        }
        let inv_sqrt_d = 1.0 / (self.dim as f32).sqrt();
        let mut error = 0.0f32;
        for (d, x) in rot.iter().enumerate() {
            let bit = (code[d / 8] >> (d % 8)) & 1;
            let x_i = if bit == 1 { inv_sqrt_d } else { -inv_sqrt_d };
            error += x_i * *x;
        }
        error = if is_approx_zero(error) { 0.8 } else { error };
        self.data.push(RabitqData {
            raw_norm,
            norm,
            sum,
            error,
            code,
        });
        self.data.last().unwrap()
    }

    /// C++ `CompressToQuery`.
    pub fn make_query(&self, v: &[f32]) -> RabitqQuery {
        let src = self.padded(v);
        let raw_norm: f32 = src.iter().map(|x| x * x).sum();
        let mut rot = rotate(&src, &self.rom, self.dim);
        let mut norm: f32 = rot
            .iter()
            .zip(&self.rot_centroid)
            .map(|(a, b)| (a - b) * (a - b))
            .sum();
        norm = if is_approx_zero(norm) {
            1.0
        } else {
            norm.sqrt()
        };
        for (r, c) in rot.iter_mut().zip(&self.rot_centroid) {
            *r = (*r - *c) / norm;
        }
        let mut lower_bound = f32::MAX;
        let mut upper_bound = f32::MIN;
        for x in &rot {
            lower_bound = lower_bound.min(*x);
            upper_bound = upper_bound.max(*x);
        }
        let delta = (upper_bound - lower_bound) / COMPRESS_BUCKET_SIZE;
        let inv_delta = if is_approx_zero(delta) {
            0.0
        } else {
            1.0 / delta
        };
        let mut code = vec![0u8; self.dim];
        let mut sum = 0.0f32;
        for (d, x) in rot.iter().enumerate() {
            let val = ((*x - lower_bound) * inv_delta).round();
            sum += val;
            code[d] = val.clamp(0.0, 255.0) as u8;
        }
        RabitqQuery {
            raw_norm,
            norm,
            sum,
            lower_bound,
            delta,
            code,
        }
    }

    fn ip_estimate(query: &RabitqQuery, base: &RabitqData, dim: usize) -> f32 {
        let mut estimate = 0.0f32;
        for d in 0..dim {
            if (base.code[d / 8] >> (d % 8)) & 1 == 1 {
                estimate += f32::from(query.code[d]);
            }
        }
        estimate
    }

    fn recover_ip(
        ip: f32,
        dim: usize,
        base_sum: f32,
        query_sum: f32,
        lower: f32,
        delta: f32,
    ) -> f32 {
        let inv_sqrt_d = 1.0 / (dim as f32).sqrt();
        let d = dim as f32;
        let p1 = inv_sqrt_d * 2.0 * delta * ip;
        let p2 = inv_sqrt_d * 2.0 * lower * base_sum;
        let p3 = inv_sqrt_d * delta * query_sum;
        let p4 = d * inv_sqrt_d * lower;
        p1 + p2 - p3 - p4
    }

    fn recover_l2(ip: f32, base_norm: f32, query_norm: f32) -> f32 {
        let p1 = base_norm * base_norm;
        let p2 = query_norm * query_norm;
        let p3 = 2.0 * base_norm * query_norm * ip;
        p1 + p2 - p3
    }

    /// The RaBitQ distance used by the HNSW graph (smaller is closer).
    pub fn distance(&self, query: &RabitqQuery, index: usize) -> Option<f32> {
        let base = self.data.get(index)?;
        let estimate = Self::ip_estimate(query, base, self.dim);
        let ip = Self::recover_ip(
            estimate,
            self.dim,
            base.sum,
            query.sum,
            query.lower_bound,
            query.delta,
        );
        let res = Self::recover_l2(ip / base.error, base.norm, query.norm);
        Some(match self.metric {
            Metric::L2 => res,
            Metric::Ip => {
                if is_approx_zero(base.raw_norm) || is_approx_zero(query.raw_norm) {
                    1.0
                } else {
                    1.0 - (query.raw_norm * query.raw_norm + base.raw_norm * base.raw_norm - res)
                        * 0.5
                }
            }
            Metric::Cos => {
                if is_approx_zero(base.raw_norm) || is_approx_zero(query.raw_norm) {
                    1.0
                } else {
                    1.0 - (query.raw_norm * query.raw_norm + base.raw_norm * base.raw_norm - res)
                        * 0.5
                        / (query.raw_norm * base.raw_norm)
                }
            }
        })
    }

    /// C++ `DecompressCode`: 1-bit code -> rotated vector -> inverse rotation.
    pub fn decompress(&self, index: usize) -> Option<Vec<f32>> {
        let d = self.data.get(index)?;
        let inv_sqrt_d = 1.0 / (self.dim as f32).sqrt();
        let mut rot = vec![0.0f32; self.dim];
        for (j, r) in rot.iter_mut().enumerate() {
            let bit = (d.code[j / 8] >> (j % 8)) & 1;
            *r = if bit == 1 { inv_sqrt_d } else { -inv_sqrt_d };
        }
        for (r, c) in rot.iter_mut().zip(&self.rot_centroid) {
            *r = *r * d.norm + *c;
        }
        Some(inverse_rotate(&rot, &self.rom, self.dim))
    }

    /// C++ `Meta::Save`: origin dim, rotation matrix and rotated centroid.
    pub fn meta_bytes(&self) -> Vec<u8> {
        let mut out = Vec::with_capacity(8 + 4 * self.dim * self.dim + 4 * self.dim);
        out.extend_from_slice(&(self.origin_dim as u64).to_le_bytes());
        for x in &self.rom {
            out.extend_from_slice(&x.to_le_bytes());
        }
        for x in &self.rot_centroid {
            out.extend_from_slice(&x.to_le_bytes());
        }
        out
    }

    /// C++ `Inner::Save`: the per-vector records.
    pub fn records_bytes(&self) -> Vec<u8> {
        let mut out = Vec::with_capacity(self.len() * self.record_size());
        for d in &self.data {
            out.extend_from_slice(&d.raw_norm.to_le_bytes());
            out.extend_from_slice(&d.norm.to_le_bytes());
            out.extend_from_slice(&d.sum.to_le_bytes());
            out.extend_from_slice(&d.error.to_le_bytes());
            out.extend_from_slice(&d.code);
        }
        out
    }

    /// C++ `Meta::Save` + `Inner::Save`.
    pub fn to_bytes(&self) -> Vec<u8> {
        let mut out = self.meta_bytes();
        out.extend_from_slice(&self.records_bytes());
        out
    }

    /// Loads the meta blob (C++ `LoadFromPtr`).
    pub fn from_meta(metric: Metric, bytes: &[u8]) -> Option<(Self, usize)> {
        if bytes.len() < 8 {
            return None;
        }
        let origin_dim = u64::from_le_bytes(bytes[0..8].try_into().ok()?) as usize;
        let dim = align_up(origin_dim, 8);
        let mut at = 8;
        if bytes.len() < at + 4 * dim * dim + 4 * dim {
            return None;
        }
        let mut rom = Vec::with_capacity(dim * dim);
        for i in 0..dim * dim {
            rom.push(f32::from_le_bytes(
                bytes[at + i * 4..at + i * 4 + 4].try_into().ok()?,
            ));
        }
        at += 4 * dim * dim;
        let mut rot_centroid = Vec::with_capacity(dim);
        for i in 0..dim {
            rot_centroid.push(f32::from_le_bytes(
                bytes[at + i * 4..at + i * 4 + 4].try_into().ok()?,
            ));
        }
        at += 4 * dim;
        Some((
            Self {
                origin_dim,
                dim,
                rom,
                rot_centroid,
                metric,
                data: Vec::new(),
            },
            at,
        ))
    }

    /// Loads the per-vector records into an existing meta.
    pub fn load_records(&mut self, records: &[u8], n: usize) -> Option<()> {
        let record = self.record_size();
        if records.len() < n * record {
            return None;
        }
        let mut at = 0;
        self.data.clear();
        for _ in 0..n {
            let raw_norm = f32::from_le_bytes(records[at..at + 4].try_into().ok()?);
            let norm = f32::from_le_bytes(records[at + 4..at + 8].try_into().ok()?);
            let sum = f32::from_le_bytes(records[at + 8..at + 12].try_into().ok()?);
            let error = f32::from_le_bytes(records[at + 12..at + 16].try_into().ok()?);
            at += 16;
            let code = records[at..at + self.dim / 8].to_vec();
            at += self.dim / 8;
            self.data.push(RabitqData {
                raw_norm,
                norm,
                sum,
                error,
                code,
            });
        }
        Some(())
    }

    /// Loads a complete store from the meta and record blobs.
    pub fn from_parts(metric: Metric, meta: &[u8], records: &[u8], n: usize) -> Option<Self> {
        let (mut store, _) = Self::from_meta(metric, meta)?;
        store.load_records(records, n)?;
        Some(store)
    }

    /// Loads a meta-and-records blob (the standalone unit-test layout).
    pub fn from_bytes(metric: Metric, bytes: &[u8], n: usize) -> Option<(Self, usize)> {
        let (mut store, at) = Self::from_meta(metric, bytes)?;
        let record = store.record_size();
        store.load_records(&bytes[at..], n)?;
        Some((store, at + n * record))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    struct Rng(u64);
    impl Rng {
        fn next_f32(&mut self) -> f32 {
            self.0 = self.0.wrapping_mul(6364136223846793005).wrapping_add(1);
            ((self.0 >> 33) as u32) as f32 / u32::MAX as f32
        }
        fn vector(&mut self, dim: usize, lo: f32, hi: f32) -> Vec<f32> {
            (0..dim).map(|_| lo + (hi - lo) * self.next_f32()).collect()
        }
    }

    fn plain_l2(a: &[f32], b: &[f32]) -> f32 {
        a.iter().zip(b).map(|(x, y)| (x - y) * (x - y)).sum()
    }

    /// Port of `test_rabitq_ut.cpp::CheckOrthogonalMatrix`.
    #[test]
    fn orthogonal_matrix_is_orthonormal() {
        let dim = 32;
        let rom = random_orthogonal_matrix(dim, 9);
        for i in 0..dim {
            let mut dot = 0.0f32;
            for k in 0..dim {
                dot += rom[k * dim + i] * rom[k * dim + i];
            }
            assert!((dot - 1.0).abs() < 1e-4, "column {i} norm^2 {dot}");
            for j in (i + 1)..dim {
                let mut dot = 0.0f32;
                for k in 0..dim {
                    dot += rom[k * dim + i] * rom[k * dim + j];
                }
                assert!(dot.abs() < 1e-4, "columns {i}/{j} dot {dot}");
            }
        }
    }

    /// C++ `Optimize`: the saved rotated centroid is the rotated raw mean.
    #[test]
    fn centroid_is_the_rotated_mean() {
        let dim = 128;
        let mut rng = Rng(11);
        let vectors: Vec<Vec<f32>> = (0..512).map(|_| rng.vector(dim, 100.0, 200.0)).collect();
        let mut store = RabitqStore::new(dim, Metric::L2, 5);
        store.build(&vectors);
        let mut mean = vec![0.0f32; dim];
        for v in &vectors {
            for (j, x) in v.iter().enumerate() {
                mean[j] += x / vectors.len() as f32;
            }
        }
        let back = inverse_rotate(store.rot_centroid(), store.rom(), store.dim());
        for d in 0..dim {
            assert!(
                (back[d] - mean[d]).abs() < mean[d].abs().max(1.0) * 1e-4,
                "dim {d}: {} vs {}",
                back[d],
                mean[d]
            );
        }
    }

    /// C++ `RabitqTest.test_compress` (`test_rabitq_ut.cpp:229`): every
    /// compressed vector's code has `dim` bits, and the number of set bits
    /// (read MSB-first, the reference's `align_size` layout) equals the
    /// record's stored `sum_`; the quantized query's u8 codes sum to
    /// `query_sum_`.
    #[test]
    fn test_compress() {
        const DIM: usize = 128;
        const VEC_N: usize = 8192;
        let mut rng = Rng(29);
        let vectors: Vec<Vec<f32>> = (0..VEC_N).map(|_| rng.vector(DIM, 100.0, 200.0)).collect();
        let query = rng.vector(DIM, 100.0, 200.0);

        let mut store = RabitqStore::new(DIM, Metric::L2, 31);
        store.build(&vectors);
        assert_eq!(store.len(), VEC_N);
        assert_eq!(store.dim(), 128);
        for (i, record) in store.data().iter().enumerate() {
            assert_eq!(record.code.len(), store.dim() / 8);
            let mut sum = 0usize;
            for d in 0..store.dim() {
                let bit = (record.code[d / 8] >> (7 - d % 8)) & 1;
                sum += bit as usize;
            }
            assert_eq!(sum, record.sum as usize, "vector {i} bit sum");
        }

        let query_code = store.make_query(&query);
        assert_eq!(query_code.code.len(), store.dim());
        let sum: f32 = query_code.code.iter().map(|&c| f32::from(c)).sum();
        assert_eq!(sum, query_code.sum, "query code sum");
        assert!(query_code.delta > 0.0);
    }

    #[test]
    fn save_and_load_roundtrip() {
        let dim = 24;
        let mut rng = Rng(17);
        let vectors: Vec<Vec<f32>> = (0..16).map(|_| rng.vector(dim, -1.0, 1.0)).collect();
        for metric in [Metric::L2, Metric::Ip, Metric::Cos] {
            let mut store = RabitqStore::new(dim, metric, 3);
            store.build(&vectors);
            let bytes = store.to_bytes();
            let (loaded, at) = RabitqStore::from_bytes(metric, &bytes, store.len()).unwrap();
            assert_eq!(at, bytes.len());
            assert_eq!(loaded, store);
        }
    }

    /// The 1-bit + u8-query estimator keeps the true nearest vector inside a
    /// small candidate set on random data.
    #[test]
    fn l2_estimator_keeps_the_true_neighbour() {
        let dim = 128;
        let mut rng = Rng(23);
        let vectors: Vec<Vec<f32>> = (0..256).map(|_| rng.vector(dim, 0.0, 1.0)).collect();
        let mut store = RabitqStore::new(dim, Metric::L2, 7);
        store.build(&vectors);
        let mut hits = 0;
        for _ in 0..20 {
            let q = rng.vector(dim, 0.0, 1.0);
            let query = store.make_query(&q);
            let mut truth: Vec<usize> = (0..vectors.len()).collect();
            truth.sort_by(|a, b| {
                plain_l2(&q, &vectors[*a])
                    .partial_cmp(&plain_l2(&q, &vectors[*b]))
                    .unwrap()
            });
            let mut estimate: Vec<usize> = (0..vectors.len()).collect();
            estimate.sort_by(|a, b| {
                store
                    .distance(&query, *a)
                    .unwrap()
                    .partial_cmp(&store.distance(&query, *b).unwrap())
                    .unwrap()
            });
            if estimate[..10].contains(&truth[0]) {
                hits += 1;
            }
        }
        assert!(hits >= 16, "true nearest in top-10 only {hits}/20 times");
    }

    /// Diagnostic: how far the estimator is from the exact distance, against
    /// what the 1-bit code can theoretically carry.
    ///
    /// Prints metrics and asserts nothing, so it stays out of the unit lane;
    /// `--ignored` still runs it.
    #[ignore = "diagnostic: prints metrics, asserts nothing"]
    #[test]
    fn diag_estimator_accuracy() {
        let dim = 128;
        let n = 1024;
        let mut rng = Rng(41);
        let vectors: Vec<Vec<f32>> = (0..n).map(|_| rng.vector(dim, 100.0, 200.0)).collect();
        let mut store = RabitqStore::new(dim, Metric::L2, 13);
        store.build(&vectors);
        let q = rng.vector(dim, 100.0, 200.0);
        let query = store.make_query(&q);
        store.make_query(&q);

        // Reconstructed vectors: the 1-bit code plus norm/centroid must land
        // near the original (the sign-only reconstruction error is ~0.36 of
        // the norm: 1 - 2/pi).
        let mut recon_err = 0.0f64;
        let mut norm_sum = 0.0f64;
        for (i, v) in vectors.iter().take(64).enumerate() {
            let back = store.decompress(i).unwrap();
            let e: f32 = back.iter().zip(v).map(|(a, b)| (a - b) * (a - b)).sum();
            recon_err += e as f64;
            norm_sum += v.iter().map(|x| x * x).sum::<f32>() as f64;
        }
        eprintln!("reconstruction MSE / ||v||^2 = {:.3}", recon_err / norm_sum);

        // Estimated vs exact distance.
        let mut sum_rel = 0.0f64;
        let mut sum_sq = 0.0f64;
        let mut cnt = 0usize;
        for (i, v) in vectors.iter().enumerate().take(256) {
            let exact: f32 = v.iter().zip(&q).map(|(a, b)| (a - b) * (a - b)).sum();
            let est = store.distance(&query, i).unwrap();
            let rel = (est - exact) / exact;
            sum_rel += rel as f64;
            sum_sq += (rel as f64) * (rel as f64);
            cnt += 1;
            if i < 3 {
                eprintln!("  d{i}: exact {exact:.3} est {est:.3}");
            }
        }
        let mean = sum_rel / cnt as f64;
        let rms = (sum_sq / cnt as f64).sqrt();
        eprintln!("relative error: mean {mean:.4}, rms {rms:.4}");

        // The exact 1-bit estimate (query not u8-quantized): isolates the
        // quantization of the query from the 1-bit code's own error.
        let mut sum_rel_bar = 0.0f64;
        let mut sum_sq_bar = 0.0f64;
        let c = store.rot_centroid().to_vec();
        let mut qo = rotate(&store.padded(&q), &store.rom, dim);
        for (r, cc) in qo.iter_mut().zip(&c) {
            *r -= *cc;
        }
        let qn = qo.iter().map(|x| x * x).sum::<f32>().sqrt();
        for x in qo.iter_mut() {
            *x /= qn;
        }
        for (i, v) in vectors.iter().enumerate().take(256) {
            let data = &store.data()[i];
            let inv_sqrt_d = 1.0 / (dim as f32).sqrt();
            let mut ip_bar = 0.0f32;
            for d in 0..dim {
                let bit = (data.code[d / 8] >> (d % 8)) & 1;
                let x = if bit == 1 { inv_sqrt_d } else { -inv_sqrt_d };
                ip_bar += x * qo[d];
            }
            let ip = ip_bar / data.error;
            let est = (data.norm * data.norm + qn * qn - 2.0 * data.norm * qn * ip).max(0.0);
            let exact: f32 = v.iter().zip(&q).map(|(a, b)| (a - b) * (a - b)).sum();
            let rel = (est - exact) / exact;
            sum_rel_bar += rel as f64;
            sum_sq_bar += (rel as f64) * (rel as f64);
        }
        eprintln!(
            "exact-query 1-bit: mean {:.4}, rms {:.4}",
            sum_rel_bar / 256.0,
            (sum_sq_bar / 256.0).sqrt()
        );
    }

    /// Diagnostic: recall@10 of the estimator against the recall@10 of ranking
    /// by the reconstructed vector (centroid + norm * sign code).
    ///
    /// Prints a table, asserts nothing, and ranks among the slowest cases in
    /// the lane; `--ignored` still runs it.
    #[ignore = "diagnostic: prints metrics, asserts nothing"]
    #[test]
    fn diag_ranking_quality() {
        let dim = 128;
        let n = 8192;
        let queries = 20;
        let mut rng = Rng(51);
        let vectors: Vec<Vec<f32>> = (0..n).map(|_| rng.vector(dim, 100.0, 200.0)).collect();
        let mut store = RabitqStore::new(dim, Metric::L2, 17);
        store.build(&vectors);
        let mut hits_est = 0usize;
        let mut hits_recon = 0usize;
        for _ in 0..queries {
            let q = rng.vector(dim, 100.0, 200.0);
            let query = store.make_query(&q);
            let mut truth: Vec<usize> = (0..n).collect();
            truth.sort_by(|a, b| {
                plain_l2(&q, &vectors[*a])
                    .partial_cmp(&plain_l2(&q, &vectors[*b]))
                    .unwrap()
            });
            let mut est: Vec<usize> = (0..n).collect();
            est.sort_by(|a, b| {
                store
                    .distance(&query, *a)
                    .unwrap()
                    .partial_cmp(&store.distance(&query, *b).unwrap())
                    .unwrap()
            });
            let mut rec: Vec<usize> = (0..n).collect();
            rec.sort_by(|a, b| {
                plain_l2(&q, &store.decompress(*a).unwrap())
                    .partial_cmp(&plain_l2(&q, &store.decompress(*b).unwrap()))
                    .unwrap()
            });
            hits_est += est[..10].iter().filter(|i| truth[..10].contains(i)).count();
            hits_recon += rec[..10].iter().filter(|i| truth[..10].contains(i)).count();
        }
        let total = (queries * 10) as f64;
        eprintln!(
            "recall@10: estimator {:.3}, reconstructed {:.3}",
            hits_est as f64 / total,
            hits_recon as f64 / total
        );
    }

    /// Diagnostic: what extra magnitude bits per dimension would buy, on the
    /// same data and the same rotation. `ex = 0` is the shipped 1-bit code.
    ///
    /// Prints a table, asserts nothing, and is the slowest case of the crate in
    /// the lane; `--ignored` still runs it.
    #[ignore = "diagnostic: prints metrics, asserts nothing"]
    #[test]
    fn diag_extra_bits() {
        let dim = 128;
        let n = 8192;
        let queries = 40;
        let mut rng = Rng(61);
        let vectors: Vec<Vec<f32>> = (0..n).map(|_| rng.vector(dim, 100.0, 200.0)).collect();
        let qs: Vec<Vec<f32>> = (0..queries)
            .map(|_| rng.vector(dim, 100.0, 200.0))
            .collect();
        let mut store = RabitqStore::new(dim, Metric::L2, 19);
        // The centroid must be trained, otherwise every [100, 200] vector
        // normalizes to nearly the same direction and all recall is noise.
        store.train(&vectors);
        let rom = store.rom().to_vec();
        let centroid = store.rot_centroid().to_vec();

        // Rotated, centered, unit-normalized base and queries.
        let prep = |v: &[f32]| -> (Vec<f32>, f32, f32) {
            let mut r = rotate(&store.padded(v), &rom, dim);
            for (x, c) in r.iter_mut().zip(&centroid) {
                *x -= *c;
            }
            let norm = r.iter().map(|x| x * x).sum::<f32>().sqrt();
            for x in r.iter_mut() {
                *x /= norm;
            }
            let raw: f32 = store.padded(v).iter().map(|x| x * x).sum();
            (r, norm, raw)
        };
        let base: Vec<(Vec<f32>, f32)> = vectors
            .iter()
            .map(|v| {
                let (u, norm, _) = prep(v);
                (u, norm)
            })
            .collect();
        let qry: Vec<(Vec<f32>, f32)> = qs
            .iter()
            .map(|v| {
                let (u, norm, _) = prep(v);
                (u, norm)
            })
            .collect();

        // Ground truth is query-independent of the codec, so sort once.
        let truths: Vec<Vec<usize>> = qs
            .iter()
            .map(|q| {
                let mut t: Vec<usize> = (0..n).collect();
                t.sort_by(|a, b| {
                    plain_l2(q, &vectors[*a])
                        .partial_cmp(&plain_l2(q, &vectors[*b]))
                        .unwrap()
                });
                t.truncate(10);
                t
            })
            .collect();

        // Code every base vector for one configuration: extra magnitude bits
        // `ex` give `r_d = sign * bucket_center(|u_d|)`. `None` is the exact
        // (unquantized) code, kept as a harness check.
        let make_codes = |ex: Option<usize>| -> Vec<(Vec<f32>, f32)> {
            base.iter()
                .map(|(u, _)| {
                    let r = match ex {
                        None => u.clone(),
                        Some(ex) => {
                            let levels = (1u32 << ex) as f32;
                            let mut r = vec![0.0f32; dim];
                            for (d, x) in u.iter().enumerate() {
                                let bucket = ((x.abs() * levels).floor()).min(levels - 1.0);
                                let center = (bucket + 0.5) / levels;
                                r[d] = if *x >= 0.0 { center } else { -center };
                            }
                            r
                        }
                    };
                    let rr: f32 = r.iter().map(|x| x * x).sum();
                    let ro: f32 = r.iter().zip(u).map(|(a, b)| a * b).sum();
                    // Least-squares rescale: `r` is not unit norm, so
                    // `<u, q> ~= (ro / rr) * <r, q>`.
                    (r, ro / rr)
                })
                .collect()
        };
        // Base ids ordered by the estimated distance.
        let rank = |codes: &[(Vec<f32>, f32)], qu: &[f32], qn: f32| -> Vec<usize> {
            let mut est: Vec<(f32, usize)> = codes
                .iter()
                .enumerate()
                .map(|(i, (r, c))| {
                    let ip: f32 = r.iter().zip(qu).map(|(a, b)| a * b).sum();
                    let cos = ip * c;
                    let norm = base[i].1;
                    (norm * norm + qn * qn - 2.0 * norm * qn * cos, i)
                })
                .collect();
            est.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
            est.into_iter().map(|(_, i)| i).collect()
        };

        let configs = [
            None,
            Some(0usize),
            Some(1),
            Some(2),
            Some(3),
            Some(4),
            Some(5),
        ];
        for ex in configs {
            let codes = make_codes(ex);
            let mut hits = 0usize;
            for (qi, (qu, qn)) in qry.iter().enumerate() {
                let order = rank(&codes, qu, *qn);
                hits += order[..10]
                    .iter()
                    .filter(|i| truths[qi].contains(i))
                    .count();
            }
            let label = match ex {
                None => "exact".to_string(),
                Some(ex) => format!("ex={ex} ({} bits/dim)", 1 + ex),
            };
            eprintln!(
                "{label}: recall@10 {:.3}",
                hits as f64 / (queries * 10) as f64
            );
        }

        // The shipped 1-bit code under oversample-then-rerank: how often does
        // the true top-10 land inside the estimator's own top-`m`?
        let codes = make_codes(Some(0));
        for m in [10usize, 20, 50, 100, 200, 400] {
            let mut keep = 0usize;
            for (qi, (qu, qn)) in qry.iter().enumerate() {
                let order = rank(&codes, qu, *qn);
                keep += truths[qi].iter().filter(|i| order[..m].contains(i)).count();
            }
            eprintln!(
                "1-bit top-{m} contains the true top-10: {:.3}",
                keep as f64 / (queries * 10) as f64
            );
        }
    }
}
