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
    /// (`sample_raito`, `ls_k`, `alpha`).
    ///
    /// The reference spells the ratio key `sample_raito` (a typo it has kept,
    /// `index_hnsw_impl.cpp:115`) and rejects anything else with
    /// `InvalidIndexParam`. That spelling is what its catalogue record carries
    /// (`LSGConfig::ToString`) and what `C++` re-reads, so a port that only
    /// accepted `sample_ratio` could write records the reference aborts on.
    /// `sample_ratio` is additionally accepted here: statements written
    /// against this port before the spelling was aligned used it.
    pub fn parse(text: &str) -> Option<Self> {
        let mut config = Self::default();
        for part in text.split(',') {
            let (key, value) = part.split_once('=')?;
            let key = key.trim().to_ascii_lowercase();
            let value = value.trim();
            match key.as_str() {
                "sample_raito" | "sample_ratio" => config.sample_ratio = value.parse().ok()?,
                "ls_k" => config.ls_k = value.parse().ok()?,
                "alpha" => config.alpha = value.parse().ok()?,
                _ => return None,
            }
        }
        Some(config)
    }
}

/// C++ `LSGConfig::ToString`: `sample_raito = <f>, ls_k = <n>, alpha = <f>`.
///
/// The spacing and the misspelt key are both the reference's; `show_index`
/// reports this text and the catalogue stores it, so a port that wrote
/// `sample_ratio=...` would hand the reference a record it refuses to parse.
impl std::fmt::Display for LsgConfig {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "sample_raito = {}, ls_k = {}, alpha = {}",
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
        Metric::L2 => crate::simd::l2_sq(a, b),
        Metric::Ip => crate::simd::ip(a, b),
        Metric::Cos => crate::simd::cosine(a, b),
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
    /// `sample_ratio` (`rand() % 1000 / 1000.0 <= ratio`); the deterministic LCG
    /// here keeps a build reproducible.
    pub fn build<V: AsRef<[f32]> + Sync>(vectors: &[V], metric: Metric, config: LsgConfig) -> Self {
        let mut state = 0x9E37_79B9_7F4A_7C15u64;
        let mut samples: Vec<&[f32]> = Vec::new();
        if config.sample_ratio >= 1.0 {
            samples.extend(vectors.iter().map(|v| v.as_ref()));
        } else if config.sample_ratio > 0.0 {
            for v in vectors {
                state = state
                    .wrapping_mul(6364136223846793005)
                    .wrapping_add(1442695040888963407);
                // The top 32 bits scaled to `[0, 1)`. Taking `>> 33` and
                // dividing by `u32::MAX` only reached `[0, 0.5)`, so a
                // `sample_ratio` of 0.01 sampled 2% of the rows - twice the
                // sample set (and twice the build's dominant cost) the
                // statement asked for.
                let r = ((state >> 32) as u32) as f32 / (u32::MAX as f32 + 1.0);
                if r <= config.sample_ratio {
                    samples.push(v.as_ref());
                }
            }
        }
        if samples.is_empty() {
            // A too-small ratio must not leave the graph unscaled.
            samples.extend(vectors.iter().map(|v| v.as_ref()));
        }
        let ls_k = config.ls_k.min(samples.len()).max(1);
        // Each row's average is independent of the others, and this loop is the
        // dominant cost of an LSG build: `rows x samples x dims` distance
        // computations (100k rows x 25k samples x 128 dims is ~3e11 flops). The
        // rows are split over the machine's cores; every row keeps its own
        // sample order, so the averages - and the build - stay reproducible.
        let row_avg =
            |v: &[f32]| -> f32 {
                let mut distances: Vec<f32> = samples
                    .iter()
                    .map(|sample| distance(v, sample, metric))
                    .collect();
                match metric {
                    Metric::L2 => distances
                        .sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal)),
                    Metric::Ip | Metric::Cos => distances
                        .sort_by(|a, b| b.partial_cmp(a).unwrap_or(std::cmp::Ordering::Equal)),
                }
                distances.iter().take(ls_k).sum::<f32>() / ls_k as f32
            };
        let workers = std::thread::available_parallelism()
            .map(|count| count.get())
            .unwrap_or(1)
            .clamp(1, 16);
        let avg: Vec<f32> = if workers == 1 || vectors.len() < 2048 {
            vectors.iter().map(|v| row_avg(v.as_ref())).collect()
        } else {
            let chunk = vectors.len().div_ceil(workers);
            let mut avg = vec![0.0f32; vectors.len()];
            std::thread::scope(|scope| {
                for (slots, part) in avg.chunks_mut(chunk).zip(vectors.chunks(chunk)) {
                    let row_avg = &row_avg;
                    scope.spawn(move || {
                        for (slot, v) in slots.iter_mut().zip(part) {
                            *slot = row_avg(v.as_ref());
                        }
                    });
                }
            });
            avg
        };
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
        let c = LsgConfig::parse("sample_raito=0.25,ls_k=7,alpha=0.5").unwrap();
        assert_eq!(
            c,
            LsgConfig {
                sample_ratio: 0.25,
                ls_k: 7,
                alpha: 0.5
            }
        );
        // The pre-alignment spelling is still accepted, so a statement written
        // against an older build keeps working.
        assert_eq!(
            LsgConfig::parse("sample_ratio=0.25,ls_k=7,alpha=0.5"),
            Some(c)
        );
        assert_eq!(LsgConfig::parse("bogus=1"), None);
        // `LSGConfig::ToString`'s exact text: misspelt key, spaces around `=`.
        assert_eq!(c.to_string(), "sample_raito = 0.25, ls_k = 7, alpha = 0.5");
        // The reference prints a whole alpha without a decimal point.
        assert_eq!(
            LsgConfig::default().to_string(),
            "sample_raito = 0.01, ls_k = 10, alpha = 1"
        );
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
