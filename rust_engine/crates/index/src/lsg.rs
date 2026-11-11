//! Local scaling graph (LSG) support for HNSW.
//!
//! Port of `src/storage/knn_index/knn_hnsw/hnsw_lsg_builder.cppm` and
//! `dist_func_lsg_wrapper.cppm`. Every row gets the mean distance to its `ls_k`
//! nearest sampled vectors; HNSW distances are then divided by
//! `sqrt(avg_i * avg_j)^alpha`, which adapts the graph to non-uniform density.
//!
//! The builder keeps the reference's three-step shape, so the incremental path
//! (`InsertSampleVec` → `InsertLSAvg` → `SetLSGParam`) and the one-shot path
//! share it:
//!
//! * [`LsgBuilder::insert_sample_vecs`] samples each row with probability
//!   `sample_ratio` (`HnswLSGBuilder::InsertSampleVec`, `SampleIter`):
//!   `rand() % 1000 / 1000.0 <= ratio`. A deterministic generator is used here
//!   so a build is reproducible.
//! * [`LsgBuilder::insert_ls_avg`] appends one factor per row by keeping the
//!   `ls_k` best sample distances in a bounded heap (`MergeKnn(1, ls_k)` with
//!   `CompareMax` for L2 / `CompareMin` for IP and cosine) and averaging them
//!   (`GetDistMean`). Rows are split across threads like the reference's
//!   `LsgBuildThreadPool` path.
//! * [`LsgBuilder::finish`] is `SetLSGParam(alpha, avg)`: the factors and the
//!   alpha the distance wrapper applies.
//!
//! Unlike the reference, nothing in the engine calls the incremental API, so
//! `avg` is dropped when an index is reloaded or compressed — the same as the
//! reference, whose `avg_` is a borrowed pointer passed to the wrapper and is
//! not part of the chunk format.

use std::cmp::{Ordering, Reverse};
use std::collections::BinaryHeap;

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

impl Metric {
    /// L2 is a distance (smaller is closer); inner product and cosine are
    /// similarities (larger is closer). The reference picks `CompareMax` for
    /// the former and `CompareMin` for the latter when building the top-`ls_k`
    /// heap.
    fn keeps_smallest(self) -> bool {
        matches!(self, Metric::L2)
    }
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

/// A distance with a total order, for the bounded top-`ls_k` heaps.
#[derive(Clone, Copy, PartialEq)]
struct Dist(f32);

impl Eq for Dist {}
impl PartialOrd for Dist {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}
impl Ord for Dist {
    fn cmp(&self, other: &Self) -> Ordering {
        self.0.total_cmp(&other.0)
    }
}

/// Per-row scaling factors and the shared alpha.
#[derive(Clone, Debug, PartialEq)]
pub struct LsgIndex {
    avg: Vec<f32>,
    alpha: f32,
    metric: Metric,
}

/// `HnswLSGBuilder`: collects samples, then appends one factor per row.
pub struct LsgBuilder {
    metric: Metric,
    config: LsgConfig,
    samples: Vec<Vec<f32>>,
    avg: Vec<f32>,
    /// Deterministic generator behind the sampler's `rand() % 1000` draw.
    seed: u64,
}

impl LsgBuilder {
    pub fn new(metric: Metric, config: LsgConfig) -> Self {
        Self {
            metric,
            config,
            samples: Vec::new(),
            avg: Vec::new(),
            seed: 0x9E37_79B9_7F4A_7C15,
        }
    }

    /// `InsertSampleVec`: keeps each row with probability `sample_ratio`.
    ///
    /// Returns how many rows were sampled. A ratio of `0` samples nothing, and
    /// a ratio of `1` (or more) keeps every row, as in the reference.
    pub fn insert_sample_vecs<'a>(
        &mut self,
        vectors: impl IntoIterator<Item = &'a Vec<f32>>,
    ) -> usize {
        let mut count = 0;
        for vector in vectors {
            let keep = if self.config.sample_ratio >= 1.0 {
                true
            } else if self.config.sample_ratio > 0.0 {
                // `rand() % 1000 / 1000.0 <= ratio`: a thousandth-resolution
                // draw, which is what makes `sample_ratio = 0.001` still
                // sample roughly one row in a thousand.
                let draw = (next_rand(&mut self.seed) % 1000) as f32 / 1000.0;
                draw <= self.config.sample_ratio
            } else {
                false
            };
            if keep {
                self.samples.push(vector.clone());
                count += 1;
            }
        }
        count
    }

    pub fn sample_count(&self) -> usize {
        self.samples.len()
    }

    /// `InsertLSAvg`: appends the mean distance to the `ls_k` nearest samples
    /// for every row in `rows`.
    pub fn insert_ls_avg(&mut self, rows: &[Vec<f32>]) -> usize {
        let base = self.avg.len();
        self.avg.resize(base + rows.len(), 0.0);
        let ls_k = self.config.ls_k.min(self.samples.len()).max(1);
        let metric = self.metric;
        let samples = &self.samples;
        let avg = &mut self.avg[base..];
        // The reference splits the row iterator across its LSG build pool; the
        // distance loop is the only work, so plain scoped threads are enough.
        let threads = std::thread::available_parallelism()
            .map(|n| n.get())
            .unwrap_or(1)
            .min(4)
            .min(rows.len().max(1));
        let chunk = rows.len().div_ceil(threads).max(1);
        let mut out = avg;
        let mut rows_rest = rows;
        let mut pairs: Vec<(&[Vec<f32>], &mut [f32])> = Vec::new();
        while !rows_rest.is_empty() {
            let take = chunk.min(rows_rest.len());
            let (head, tail) = rows_rest.split_at(take);
            let (out_head, out_tail) = out.split_at_mut(take);
            pairs.push((head, out_head));
            rows_rest = tail;
            out = out_tail;
        }
        std::thread::scope(|scope| {
            for (chunk_rows, chunk_avg) in pairs {
                scope.spawn(move || {
                    let mut heap: BinaryHeap<Dist> = BinaryHeap::with_capacity(ls_k + 1);
                    let mut heap_min: BinaryHeap<Reverse<Dist>> =
                        BinaryHeap::with_capacity(ls_k + 1);
                    for (row, slot) in chunk_rows.iter().zip(chunk_avg.iter_mut()) {
                        if metric.keeps_smallest() {
                            heap.clear();
                            for sample in samples {
                                let d = distance(row, sample, metric);
                                push_smallest(&mut heap, ls_k, d);
                            }
                            *slot = heap.iter().map(|d| d.0).sum::<f32>() / ls_k as f32;
                        } else {
                            heap_min.clear();
                            for sample in samples {
                                let d = distance(row, sample, metric);
                                push_largest(&mut heap_min, ls_k, d);
                            }
                            *slot = heap_min.iter().map(|d| d.0 .0).sum::<f32>() / ls_k as f32;
                        }
                    }
                });
            }
        });
        rows.len()
    }

    /// `SetLSGParam(alpha, avg)`: freezes the factors into the wrapper state.
    pub fn finish(self) -> LsgIndex {
        LsgIndex {
            avg: self.avg,
            alpha: self.config.alpha,
            metric: self.metric,
        }
    }
}

/// Keeps the `cap` smallest values seen (`CompareMax` + `MergeKnn(topk)`).
fn push_smallest(heap: &mut BinaryHeap<Dist>, cap: usize, value: f32) {
    if heap.len() < cap {
        heap.push(Dist(value));
    } else if let Some(top) = heap.peek() {
        if value < top.0 {
            heap.pop();
            heap.push(Dist(value));
        }
    }
}

/// Keeps the `cap` largest values seen (`CompareMin` + `MergeKnn(topk)`).
fn push_largest(heap: &mut BinaryHeap<Reverse<Dist>>, cap: usize, value: f32) {
    if heap.len() < cap {
        heap.push(Reverse(Dist(value)));
    } else if let Some(Reverse(top)) = heap.peek() {
        if value > top.0 {
            heap.pop();
            heap.push(Reverse(Dist(value)));
        }
    }
}

/// One step of the splitmix-style generator behind the sampler's
/// `rand() % 1000` draw: reproducible, and fine-grained enough that
/// `sample_ratio = 0.001` still keeps roughly one row in a thousand.
fn next_rand(state: &mut u64) -> u64 {
    *state = state
        .wrapping_mul(6364136223846793005)
        .wrapping_add(1442695040888963407);
    *state >> 33
}

impl LsgIndex {
    /// Builds the sample set and the average distances in one pass.
    pub fn build(vectors: &[Vec<f32>], metric: Metric, config: LsgConfig) -> Self {
        let mut builder = LsgBuilder::new(metric, config);
        let mut sampled = builder.insert_sample_vecs(vectors.iter());
        if sampled == 0 && !vectors.is_empty() {
            // A ratio below the sampler's resolution must not leave the graph
            // unscaled: fall back to the full set, as the port always did.
            builder.samples.extend(vectors.iter().cloned());
            sampled = builder.samples.len();
        }
        let _ = sampled;
        builder.insert_ls_avg(vectors);
        builder.finish()
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

    /// The bounded heap keeps the same `ls_k` values as a full sort: L2 keeps
    /// the smallest, inner product the largest (`MergeKnn` + `CompareMax` /
    /// `CompareMin`).
    #[test]
    fn top_k_heap_matches_a_full_sort_for_both_comparators() {
        let mut rng = Rng(7);
        let dim = 32;
        let n = 64;
        let vectors: Vec<Vec<f32>> = (0..n).map(|_| rng.vector(dim)).collect();
        let config = LsgConfig {
            sample_ratio: 1.0,
            ls_k: 7,
            alpha: 1.0,
        };
        for metric in [Metric::L2, Metric::Ip, Metric::Cos] {
            let lsg = LsgIndex::build(&vectors, metric, config);
            for (i, row) in vectors.iter().enumerate() {
                let mut all: Vec<f32> = vectors.iter().map(|v| distance(row, v, metric)).collect();
                if metric.keeps_smallest() {
                    all.sort_by(|a, b| a.partial_cmp(b).unwrap());
                } else {
                    all.sort_by(|a, b| b.partial_cmp(a).unwrap());
                }
                let expected: f32 = all.iter().take(config.ls_k).sum::<f32>() / config.ls_k as f32;
                assert!(
                    (lsg.avg()[i] - expected).abs() < 1e-3,
                    "{metric:?} row {i}: {} != {expected}",
                    lsg.avg()[i]
                );
            }
        }
    }

    /// `sample_ratio = 0` samples nothing, which the one-shot build then
    /// papers over by using every row; the incremental builder keeps it empty
    /// so a caller can see the difference (the reference's `SampleIter` does
    /// the same).
    #[test]
    fn sample_ratio_controls_the_sample_set() {
        let mut rng = Rng(3);
        let vectors: Vec<Vec<f32>> = (0..256).map(|_| rng.vector(8)).collect();
        let mut builder = LsgBuilder::new(
            Metric::L2,
            LsgConfig {
                sample_ratio: 0.0,
                ls_k: 4,
                alpha: 1.0,
            },
        );
        assert_eq!(builder.insert_sample_vecs(vectors.iter()), 0);
        assert_eq!(builder.sample_count(), 0);

        let mut builder = LsgBuilder::new(
            Metric::L2,
            LsgConfig {
                sample_ratio: 1.0,
                ls_k: 4,
                alpha: 1.0,
            },
        );
        assert_eq!(builder.insert_sample_vecs(vectors.iter()), 256);
        assert_eq!(builder.sample_count(), 256);

        let mut builder = LsgBuilder::new(
            Metric::L2,
            LsgConfig {
                sample_ratio: 0.25,
                ls_k: 4,
                alpha: 1.0,
            },
        );
        let kept = builder.insert_sample_vecs(vectors.iter());
        assert!(kept > 20 && kept < 100, "kept {kept} of 256 at ratio 0.25");
    }

    /// The incremental path (`InsertSampleVec` → `InsertLSAvg` → `SetLSGParam`)
    /// has to produce the same factors as the one-shot build.
    #[test]
    fn incremental_build_matches_the_one_shot_build() {
        let mut rng = Rng(11);
        let vectors: Vec<Vec<f32>> = (0..64).map(|_| rng.vector(16)).collect();
        let config = LsgConfig {
            sample_ratio: 1.0,
            ls_k: 5,
            alpha: 0.5,
        };
        let one_shot = LsgIndex::build(&vectors, Metric::L2, config);
        let mut builder = LsgBuilder::new(Metric::L2, config);
        builder.insert_sample_vecs(vectors.iter());
        builder.insert_ls_avg(&vectors[..32]);
        builder.insert_ls_avg(&vectors[32..]);
        let incremental = builder.finish();
        assert_eq!(one_shot.avg(), incremental.avg());
        assert_eq!(one_shot.alpha(), incremental.alpha());
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
