//! Mean-centered i8 scalar quantization (LVQ).
//!
//! Port of `src/storage/knn_index/knn_hnsw/data_store/lvq_vec_store.cppm` and
//! the LVQ distance kernels in `dist_func_{l2,cos,ip}.cppm`.
//!
//! A vector is stored as `Q = scale * code + bias + mean`, where `code` is an
//! i8 per dimension and `scale`/`bias` are chosen from the vector's own range
//! after subtracting the store-wide mean. Distances are computed from the
//! integer dot product plus the per-vector local cache and the mean's global
//! cache, exactly as the reference does.

/// The metric decides the local/global cache contents.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Metric {
    L2,
    Ip,
    Cos,
}

/// One compressed vector: the C++ `LVQData<f32, pair<f32,f32>, i8>` record.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct LvqData {
    pub scale: f32,
    pub bias: f32,
    /// `(norm1 * scale, norm2 * scale^2)` for L2, `(norm1 * scale,
    /// <mean, code> * scale)` for cosine/inner product.
    pub local: (f32, f32),
    pub code: Vec<i8>,
}

/// A compressed query: the reference's `LVQQuery`.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct LvqQuery {
    data: LvqData,
}

impl LvqQuery {
    pub fn data(&self) -> &LvqData {
        &self.data
    }
}

/// The LVQ store of one chunk.
#[derive(Clone, Debug, PartialEq)]
pub struct LvqStore {
    dim: usize,
    metric: Metric,
    normalize: bool,
    /// `MeanType` in the reference is `double`.
    mean: Vec<f64>,
    /// `(sum(mean), sum(mean^2))`, used by cosine/inner product.
    global: Option<(f64, f64)>,
    data: Vec<LvqData>,
}

impl LvqStore {
    /// C++ `LVQVecStoreMeta::Make(dim, normalize)`: a zero mean and the
    /// matching global cache.
    pub fn new(dim: usize, metric: Metric, normalize: bool) -> Self {
        let mean = vec![0.0; dim];
        let global = match metric {
            Metric::L2 => None,
            Metric::Ip | Metric::Cos => Some(global_cache(&mean)),
        };
        Self {
            dim,
            metric,
            normalize,
            mean,
            global,
            data: Vec::new(),
        }
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

    pub fn mean(&self) -> &[f64] {
        &self.mean
    }

    pub fn data(&self) -> &[LvqData] {
        &self.data
    }

    /// Bytes one compressed vector occupies (C++ `compress_data_size`).
    pub fn record_size(&self) -> usize {
        16 + self.dim
    }

    fn normalize_vec(&self, v: &[f32]) -> Vec<f32> {
        if !self.normalize {
            return v.to_vec();
        }
        let norm = v
            .iter()
            .map(|x| f64::from(*x) * f64::from(*x))
            .sum::<f64>()
            .sqrt();
        if norm == 0.0 {
            return vec![0.0; v.len()];
        }
        v.iter().map(|x| (*x as f64 / norm) as f32).collect()
    }

    /// C++ `CompressTo`: mean-center, scalar-quantize to i8 and build the
    /// local cache. `v` must have `dim` elements.
    pub fn compress(&mut self, v: &[f32]) -> &LvqData {
        let src = self.normalize_vec(v);
        let mut lower = f32::MAX;
        let mut upper = -f32::MAX;
        for (j, x) in src.iter().enumerate() {
            let x = *x - self.mean[j] as f32;
            lower = lower.min(x);
            upper = upper.max(x);
        }
        let scale = (upper - lower) / 255.0;
        let bias = lower + 128.0 * scale;
        let mut code = vec![0i8; self.dim];
        if scale != 0.0 {
            let inv = 1.0 / scale;
            for (j, x) in src.iter().enumerate() {
                let c = ((*x - self.mean[j] as f32 - bias) * inv + 0.5).floor();
                code[j] = c.clamp(-128.0, 127.0) as i8;
            }
        }
        let local = self.local_cache(&code, scale);
        let data = LvqData {
            scale,
            bias,
            local,
            code,
        };
        self.data.push(data);
        self.data.last().unwrap()
    }

    fn local_cache(&self, code: &[i8], scale: f32) -> (f32, f32) {
        match self.metric {
            Metric::L2 => {
                let mut norm1 = 0i64;
                let mut norm2 = 0i64;
                for c in code {
                    norm1 += i64::from(*c);
                    norm2 += i64::from(*c) * i64::from(*c);
                }
                (norm1 as f32 * scale, norm2 as f32 * scale * scale)
            }
            Metric::Ip | Metric::Cos => {
                let mut norm1 = 0i64;
                let mut mean_c = 0.0f64;
                for (j, c) in code.iter().enumerate() {
                    norm1 += i64::from(*c);
                    mean_c += self.mean[j] * f64::from(*c);
                }
                (norm1 as f32 * scale, (mean_c as f32) * scale)
            }
        }
    }

    /// Sets the mean/global cache without compressing any vector
    /// (`OptAddVec`'s training half).
    pub fn train(&mut self, vectors: &[Vec<f32>]) {
        if vectors.is_empty() {
            return;
        }
        let mut mean = vec![0.0f64; self.dim];
        for v in vectors {
            for (j, x) in v.iter().enumerate() {
                if j < mean.len() {
                    mean[j] += *x as f64;
                }
            }
        }
        let n = vectors.len() as f64;
        for m in &mut mean {
            *m /= n;
        }
        self.mean = mean;
        self.global = match self.metric {
            Metric::L2 => None,
            Metric::Ip | Metric::Cos => Some(global_cache(&self.mean)),
        };
    }

    /// C++ `LVQVecStoreMeta::Optimize`: recenter the store on `vectors` and
    /// recompress every vector.
    pub fn build(&mut self, vectors: &[Vec<f32>]) {
        self.train(vectors);
        self.data.clear();
        for v in vectors {
            self.compress(v);
        }
    }

    /// A query built from a stored vector's own code (C++ `GetVecToQuery`).
    pub fn query_at(&self, index: usize) -> Option<LvqQuery> {
        self.data
            .get(index)
            .map(|data| LvqQuery { data: data.clone() })
    }

    /// C++ `LVQVecStoreMeta::MakeQuery`.
    pub fn make_query(&self, v: &[f32]) -> LvqQuery {
        let src = self.normalize_vec(v);
        let mut lower = f32::MAX;
        let mut upper = -f32::MAX;
        for (j, x) in src.iter().enumerate() {
            let x = *x - self.mean[j] as f32;
            lower = lower.min(x);
            upper = upper.max(x);
        }
        let scale = (upper - lower) / 255.0;
        let bias = lower + 128.0 * scale;
        let mut code = vec![0i8; self.dim];
        if scale != 0.0 {
            let inv = 1.0 / scale;
            for (j, x) in src.iter().enumerate() {
                let c = ((*x - self.mean[j] as f32 - bias) * inv + 0.5).floor();
                code[j] = c.clamp(-128.0, 127.0) as i8;
            }
        }
        let local = self.local_cache(&code, scale);
        LvqQuery {
            data: LvqData {
                scale,
                bias,
                local,
                code,
            },
        }
    }

    /// C++ `DecompressTo`: `scale * code + bias + mean`.
    pub fn decompress(&self, index: usize) -> Option<Vec<f64>> {
        let d = self.data.get(index)?;
        Some(
            d.code
                .iter()
                .enumerate()
                .map(|(j, c)| f64::from(d.scale) * f64::from(*c) + f64::from(d.bias) + self.mean[j])
                .collect(),
        )
    }

    /// The LVQ distance used by the HNSW graph (smaller is closer):
    /// squared L2, or the negated cosine/inner product.
    pub fn distance(&self, query: &LvqQuery, index: usize) -> Option<f32> {
        let q = &query.data;
        let v = self.data.get(index)?;
        let ip: i32 = q
            .code
            .iter()
            .zip(&v.code)
            .map(|(a, b)| i32::from(*a) * i32::from(*b))
            .sum();
        match self.metric {
            Metric::L2 => {
                let beta = q.bias - v.bias;
                Some(
                    q.local.1 + v.local.1 + beta * beta * self.dim as f32
                        - 2.0 * q.scale * v.scale * ip as f32
                        + 2.0 * beta * q.local.0
                        - 2.0 * beta * v.local.0,
                )
            }
            Metric::Ip | Metric::Cos => {
                let (n1_mean, n2_mean) = self.global.unwrap_or((0.0, 0.0));
                let dist = f64::from(q.scale) * f64::from(v.scale) * f64::from(ip)
                    + f64::from(v.bias) * f64::from(q.local.0)
                    + f64::from(q.bias) * f64::from(v.local.0)
                    + self.dim as f64 * f64::from(q.bias) * f64::from(v.bias)
                    + n2_mean
                    + (f64::from(q.bias) + f64::from(v.bias)) * n1_mean
                    + f64::from(q.local.1)
                    + f64::from(v.local.1);
                Some(-(dist as f32))
            }
        }
    }

    /// C++ `LVQVecStoreMeta::Save`: dim, mean and the metric's global cache.
    pub fn meta_bytes(&self) -> Vec<u8> {
        let mut out = Vec::with_capacity(8 + 8 * self.dim + 16);
        out.extend_from_slice(&(self.dim as u64).to_le_bytes());
        for m in &self.mean {
            out.extend_from_slice(&m.to_le_bytes());
        }
        if let Some((a, b)) = self.global {
            match self.metric {
                Metric::Ip => {
                    out.extend_from_slice(&a.to_le_bytes());
                    out.extend_from_slice(&b.to_le_bytes());
                }
                Metric::Cos => {
                    out.extend_from_slice(&(a as f32).to_le_bytes());
                    out.extend_from_slice(&(b as f32).to_le_bytes());
                }
                Metric::L2 => {}
            }
        }
        out
    }

    /// C++ `LVQVecStoreInner::Save`: the per-vector records.
    pub fn records_bytes(&self) -> Vec<u8> {
        let mut out = Vec::with_capacity(self.len() * self.record_size());
        for d in &self.data {
            out.extend_from_slice(&d.scale.to_le_bytes());
            out.extend_from_slice(&d.bias.to_le_bytes());
            out.extend_from_slice(&d.local.0.to_le_bytes());
            out.extend_from_slice(&d.local.1.to_le_bytes());
            for c in &d.code {
                out.push(*c as u8);
            }
        }
        out
    }

    /// C++ `LVQVecStoreMeta::Save` + `Inner::Save`.
    pub fn to_bytes(&self) -> Vec<u8> {
        let mut out = self.meta_bytes();
        out.extend_from_slice(&self.records_bytes());
        out
    }

    /// Loads the meta blob alone (C++ `LoadFromPtr`).
    pub fn from_meta(metric: Metric, normalize: bool, bytes: &[u8]) -> Option<(Self, usize)> {
        if bytes.len() < 8 {
            return None;
        }
        let dim = u64::from_le_bytes(bytes[0..8].try_into().ok()?) as usize;
        let mut at = 8;
        let mean_bytes = dim.checked_mul(8)?;
        if bytes.len() < at + mean_bytes {
            return None;
        }
        let mut mean = Vec::with_capacity(dim);
        for i in 0..dim {
            mean.push(f64::from_le_bytes(
                bytes[at + i * 8..at + i * 8 + 8].try_into().ok()?,
            ));
        }
        at += mean_bytes;
        let global = match metric {
            Metric::L2 => None,
            Metric::Ip => {
                if bytes.len() < at + 16 {
                    return None;
                }
                let a = f64::from_le_bytes(bytes[at..at + 8].try_into().ok()?);
                let b = f64::from_le_bytes(bytes[at + 8..at + 16].try_into().ok()?);
                at += 16;
                Some((a, b))
            }
            Metric::Cos => {
                if bytes.len() < at + 8 {
                    return None;
                }
                let a = f32::from_le_bytes(bytes[at..at + 4].try_into().ok()?);
                let b = f32::from_le_bytes(bytes[at + 4..at + 8].try_into().ok()?);
                at += 8;
                Some((f64::from(a), f64::from(b)))
            }
        };
        Some((
            Self {
                dim,
                metric,
                normalize,
                mean,
                global,
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
            let scale = f32::from_le_bytes(records[at..at + 4].try_into().ok()?);
            let bias = f32::from_le_bytes(records[at + 4..at + 8].try_into().ok()?);
            let l0 = f32::from_le_bytes(records[at + 8..at + 12].try_into().ok()?);
            let l1 = f32::from_le_bytes(records[at + 12..at + 16].try_into().ok()?);
            at += 16;
            let code = records[at..at + self.dim]
                .iter()
                .map(|b| *b as i8)
                .collect();
            at += self.dim;
            self.data.push(LvqData {
                scale,
                bias,
                local: (l0, l1),
                code,
            });
        }
        Some(())
    }

    /// Loads a complete store from the meta and record blobs.
    pub fn from_parts(
        metric: Metric,
        normalize: bool,
        meta: &[u8],
        records: &[u8],
        n: usize,
    ) -> Option<Self> {
        let (mut store, _) = Self::from_meta(metric, normalize, meta)?;
        store.load_records(records, n)?;
        Some(store)
    }

    /// Loads a meta-and-records blob (the standalone unit-test layout).
    pub fn from_bytes(
        metric: Metric,
        normalize: bool,
        bytes: &[u8],
        n: usize,
    ) -> Option<(Self, usize)> {
        let (mut store, at) = Self::from_meta(metric, normalize, bytes)?;
        let record = store.record_size();
        store.load_records(&bytes[at..], n)?;
        Some((store, at + n * record))
    }
}

fn global_cache(mean: &[f64]) -> (f64, f64) {
    let mut n1 = 0.0;
    let mut n2 = 0.0;
    for m in mean {
        n1 += m;
        n2 += m * m;
    }
    (n1, n2)
}

#[cfg(test)]
mod tests {
    use super::*;

    struct Rng(u64);
    impl Rng {
        fn next_f32(&mut self) -> f32 {
            self.0 = self.0.wrapping_mul(6364136223846793005).wrapping_add(1);
            let bits = (self.0 >> 33) as u32;
            bits as f32 / u32::MAX as f32
        }
        fn vector(&mut self, dim: usize, lo: f32, hi: f32) -> Vec<f32> {
            (0..dim).map(|_| lo + (hi - lo) * self.next_f32()).collect()
        }
    }

    fn plain_l2(a: &[f32], b: &[f32]) -> f32 {
        a.iter().zip(b).map(|(x, y)| (x - y) * (x - y)).sum()
    }

    /// Port of `test_lvq_ut.cpp`: the reconstruction error after the batch
    /// mean and after a recenter is below 2% per element.
    #[test]
    fn reconstruction_error_is_below_two_percent() {
        let dim = 16;
        let mut rng = Rng(42);
        let vectors: Vec<Vec<f32>> = (0..32).map(|_| rng.vector(dim, 100.0, 200.0)).collect();
        let mut store = LvqStore::new(dim, Metric::L2, false);
        store.build(&vectors);
        for (i, v) in vectors.iter().enumerate() {
            let d = store.decompress(i).unwrap();
            for (j, x) in v.iter().enumerate() {
                let error = ((d[j] as f32 - x) / x).abs();
                assert!(error <= 2e-2, "vector {i} dim {j}: error {error}");
            }
        }
    }

    /// `AddVec` after a first batch keeps the mean; the distances stay within
    /// the quantization error of the plain metric.
    #[test]
    fn distances_track_the_plain_metric() {
        let dim = 16;
        let mut rng = Rng(7);
        let vectors: Vec<Vec<f32>> = (0..64).map(|_| rng.vector(dim, -1.0, 1.0)).collect();
        let mut store = LvqStore::new(dim, Metric::L2, false);
        store.build(&vectors);
        for _ in 0..8 {
            let q = rng.vector(dim, -1.0, 1.0);
            let query = store.make_query(&q);
            for (i, v) in vectors.iter().enumerate() {
                let truth = plain_l2(&q, v);
                let estimate = store.distance(&query, i).unwrap();
                let tolerance = truth.abs().max(1.0) * 0.15;
                assert!(
                    (estimate - truth).abs() <= tolerance,
                    "i {i}: truth {truth}, estimate {estimate}"
                );
            }
        }
    }

    /// Save/load round-trips the meta and every record.
    #[test]
    fn save_and_load_roundtrip() {
        let dim = 8;
        let mut rng = Rng(3);
        let vectors: Vec<Vec<f32>> = (0..10).map(|_| rng.vector(dim, 1.0, 2.0)).collect();
        for metric in [Metric::L2, Metric::Ip, Metric::Cos] {
            let mut store = LvqStore::new(dim, metric, metric == Metric::Cos);
            store.build(&vectors);
            let bytes = store.to_bytes();
            let (loaded, at) =
                LvqStore::from_bytes(metric, store.normalize, &bytes, store.len()).unwrap();
            assert_eq!(at, bytes.len());
            assert_eq!(loaded.mean, store.mean);
            assert_eq!(loaded.data, store.data);
            // The cosine global cache is stored as `pair<float,float>` in C++.
            if let (Some(a), Some(b)) = (store.global, loaded.global) {
                assert!((a.0 - b.0).abs() <= a.0.abs().max(1.0) * 1e-6);
                assert!((a.1 - b.1).abs() <= a.1.abs().max(1.0) * 1e-6);
            }
        }
    }
}
