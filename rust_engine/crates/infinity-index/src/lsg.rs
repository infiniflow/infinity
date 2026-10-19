//! Local scaling graph (LSG) support for HNSW.
//!
//! Port of `src/storage/knn_index/knn_hnsw/hnsw_lsg_builder.cppm` and
//! `dist_func_lsg_wrapper.cppm`. Every row gets an average distance to its
//! `ls_k` nearest sampled vectors; HNSW distances are divided by
//! `sqrt(avg_i * avg_j)^alpha`, which adapts the graph to non-uniform density.

/// C++ `LSGConfig`.
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct LsgConfig {
    pub sample_ratio: f32,
    pub ls_k: usize,
    pub alpha: f32,
}

impl Default for LsgConfig {
    fn default() -> Self {
        Self {
            sample_ratio: 0.01,
            ls_k: 10,
            alpha: 1.0,
        }
    }
}

impl LsgConfig {
    /// C++ `LSGConfig::FromString`: `key=value` pairs separated by commas
    /// (`sample_ratio`, `ls_k`, `alpha`).
    pub fn parse(text: &str) -> Option<Self> {
        let mut config = Self::default();
        for part in text.split(',') {
            let (key, value) = part.split_once('=')?;
            let key = key.trim().to_ascii_lowercase();
            let value = value.trim();
            match key.as_str() {
                "sample_ratio" => config.sample_ratio = value.parse().ok()?,
                "ls_k" => config.ls_k = value.parse().ok()?,
                "alpha" => config.alpha = value.parse().ok()?,
                _ => return None,
            }
        }
        Some(config)
    }
}

impl std::fmt::Display for LsgConfig {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "sample_ratio={},ls_k={},alpha={}",
            self.sample_ratio, self.ls_k, self.alpha
        )
    }
}

/// The metric used for the per-row average distance (same as the index).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Metric {
    L2,
    Ip,
    Cos,
}

/// The same values as C++ `KnnDistance1`: squared L2 (smaller is closer),
/// inner product and cosine similarity (larger is closer).
fn distance(a: &[f32], b: &[f32], metric: Metric) -> f32 {
    match metric {
        Metric::L2 => a.iter().zip(b).map(|(x, y)| (x - y) * (x - y)).sum(),
        Metric::Ip => a.iter().zip(b).map(|(x, y)| x * y).sum::<f32>(),
        Metric::Cos => {
            let dot: f32 = a.iter().zip(b).map(|(x, y)| x * y).sum();
            let na: f32 = a.iter().map(|x| x * x).sum::<f32>().sqrt();
            let nb: f32 = b.iter().map(|x| x * x).sum::<f32>().sqrt();
            if na == 0.0 || nb == 0.0 {
                0.0
            } else {
                dot / (na * nb)
            }
        }
    }
}

/// Per-row scaling factors and the shared alpha.
#[derive(Clone, Debug, PartialEq)]
pub struct LsgIndex {
    avg: Vec<f32>,
    alpha: f32,
    metric: Metric,
}

impl LsgIndex {
    /// Builds the sample set and the average distances.
    ///
    /// C++ `InsertSampleVec` samples each row independently with probability
    /// `sample_ratio`; the deterministic LCG here keeps a build reproducible.
    pub fn build(vectors: &[Vec<f32>], metric: Metric, config: LsgConfig) -> Self {
        let mut state = 0x9E37_79B9_7F4A_7C15u64;
        let mut samples: Vec<&Vec<f32>> = Vec::new();
        if config.sample_ratio >= 1.0 {
            samples.extend(vectors.iter());
        } else if config.sample_ratio > 0.0 {
            for v in vectors {
                state = state
                    .wrapping_mul(6364136223846793005)
                    .wrapping_add(1442695040888963407);
                let r = ((state >> 33) as u32) as f32 / u32::MAX as f32;
                if r <= config.sample_ratio {
                    samples.push(v);
                }
            }
        }
        if samples.is_empty() {
            // A too-small ratio must not leave the graph unscaled.
            samples.extend(vectors.iter());
        }
        let ls_k = config.ls_k.min(samples.len()).max(1);
        let mut avg = Vec::with_capacity(vectors.len());
        for v in vectors {
            let mut distances: Vec<f32> = samples
                .iter()
                .map(|sample| distance(v, sample, metric))
                .collect();
            match metric {
                Metric::L2 => {
                    distances.sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal))
                }
                Metric::Ip | Metric::Cos => {
                    distances.sort_by(|a, b| b.partial_cmp(a).unwrap_or(std::cmp::Ordering::Equal))
                }
            }
            let mean = distances.iter().take(ls_k).sum::<f32>() / ls_k as f32;
            avg.push(mean);
        }
        Self {
            avg,
            alpha: config.alpha,
            metric,
        }
    }

    pub fn avg(&self) -> &[f32] {
        &self.avg
    }

    pub fn alpha(&self) -> f32 {
        self.alpha
    }

    pub fn metric(&self) -> Metric {
        self.metric
    }

    /// C++ `LSGDistWrapper::Inner`: `d / sqrt(avg_i * avg_j)^alpha`.
    pub fn adjust(&self, d: f32, i: usize, j: usize) -> f32 {
        let (Some(a), Some(b)) = (self.avg.get(i), self.avg.get(j)) else {
            return d;
        };
        let k = (a * b).sqrt().powf(self.alpha);
        if k == 0.0 {
            return f32::MAX;
        }
        d / k
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
        fn vector(&mut self, dim: usize) -> Vec<f32> {
            (0..dim).map(|_| self.next_f32()).collect()
        }
    }

    /// Port of `test_lsg_build_ut.cpp::test_avg`: sampling can only make the
    /// top-k mean larger than the exhaustive ground truth.
    #[test]
    fn avg_is_at_least_the_exhaustive_topk_mean() {
        let dim = 128;
        let n = 128;
        let mut rng = Rng(0);
        let vectors: Vec<Vec<f32>> = (0..n).map(|_| rng.vector(dim)).collect();
        let config = LsgConfig {
            sample_ratio: 0.1,
            ls_k: 10,
            alpha: 1.0,
        };
        let lsg = LsgIndex::build(&vectors, Metric::L2, config);
        for i in 0..n {
            let mut all: Vec<f32> = vectors
                .iter()
                .map(|v| distance(&vectors[i], v, Metric::L2))
                .collect();
            all.sort_by(|a, b| a.partial_cmp(b).unwrap());
            let gt: f32 = all.iter().take(config.ls_k).sum::<f32>() / config.ls_k as f32;
            assert!(
                lsg.avg()[i] >= gt - 1e-3,
                "row {i}: avg {} < gt {gt}",
                lsg.avg()[i]
            );
        }
    }

    #[test]
    fn config_parses_the_reference_format() {
        let c = LsgConfig::parse("sample_ratio=0.25,ls_k=7,alpha=0.5").unwrap();
        assert_eq!(
            c,
            LsgConfig {
                sample_ratio: 0.25,
                ls_k: 7,
                alpha: 0.5
            }
        );
        assert_eq!(LsgConfig::parse("bogus=1"), None);
        assert_eq!(c.to_string(), "sample_ratio=0.25,ls_k=7,alpha=0.5");
    }

    #[test]
    fn adjustment_divides_by_the_scaling_factor() {
        let lsg = LsgIndex {
            avg: vec![4.0, 9.0],
            alpha: 1.0,
            metric: Metric::L2,
        };
        // sqrt(4*9) = 6.
        assert!((lsg.adjust(12.0, 0, 1) - 2.0).abs() < 1e-6);
        assert_eq!(lsg.adjust(1.0, 0, 9), 1.0);
    }
}
