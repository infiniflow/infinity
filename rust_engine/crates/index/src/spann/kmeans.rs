//! Balanced 2-means: the structural primitive of the tree.
//!
//! Cockroach's `spann/kmeans.go` builds `BalancedKMeans` and always calls it
//! with `k = 2` (it bisects until a partition fits `max_partition_size`); the
//! Rust port keeps that shape (`SPANN_DESIGN.md` §4 `kmeans.rs`), which makes
//! the tree a balanced binary one and keeps `num_centroids` implied by the data
//! volume instead of fixed (`D3`).
//!
//! Everything is deterministic: the k-means++ seeding draws from an explicit
//! LCG seeded by `Options::seed`, so two builds of the same rows produce the
//! same tree (the chunk format relies on it for reproducible tests).

use crate::vector::l2_sq;

/// One split: two centroids and, per point, `true` for the second cluster.
#[derive(Clone, Debug, PartialEq)]
pub struct Split {
    pub centroids: [Vec<f32>; 2],
    pub second: Vec<bool>,
}

impl Split {
    pub fn sizes(&self) -> (usize, usize) {
        let a = self.second.iter().filter(|s| !**s).count();
        (a, self.second.len() - a)
    }
}

/// `n` deterministic pseudo-random numbers in `[0, 1)`.
struct Lcg(u64);

impl Lcg {
    fn next(&mut self) -> f32 {
        self.0 = self.0.wrapping_mul(6364136223846793005).wrapping_add(1);
        ((self.0 >> 33) as f32) / ((1u64 << 31) as f32)
    }
}

fn mean(points: &[&[f32]], dim: usize) -> Vec<f32> {
    let mut out = vec![0.0f32; dim];
    if points.is_empty() {
        return out;
    }
    for p in points {
        for (o, x) in out.iter_mut().zip(*p) {
            *o += *x;
        }
    }
    let n = points.len() as f32;
    for o in &mut out {
        *o /= n;
    }
    out
}

/// Squared distance from `p` to `c`, ignoring any trailing dimensions.
fn d2(p: &[f32], c: &[f32]) -> f32 {
    l2_sq(p, c)
}

/// Balanced 2-means (`BalancedKMeans(k=2)`): k-means++ seeding, Lloyd
/// iterations, then a **balance** pass that moves the points farthest from
/// their own centroid until the two clusters differ by at most one.
///
/// The balance pass is what makes the tree's depth predictable: an unbalanced
/// split repeats until `max_partition_size` is met, and one heavy child turns
/// the tree into a list.
pub fn balanced_kmeans_2(points: &[&[f32]], dim: usize, iterations: usize, seed: u64) -> Split {
    if points.is_empty() {
        return Split {
            centroids: [vec![0.0f32; dim], vec![0.0f32; dim]],
            second: Vec::new(),
        };
    }
    if points.len() == 1 {
        return Split {
            centroids: [points[0].to_vec(), points[0].to_vec()],
            second: vec![false],
        };
    }

    // k-means++: the first centre is drawn uniformly, the second with
    // probability proportional to `d²`. The seed goes through a multiplication
    // so adjacent seeds (`k ^ key`) do not collapse onto the same stream.
    let mut rng = Lcg(seed
        .wrapping_mul(0x9E37_79B9_7F4A_7C15)
        .wrapping_add(0x2545_F491_4F6C_DD1D)
        | 1);
    let first = ((rng.next() as usize) * points.len()) % points.len();
    let c0 = points[first].to_vec();
    let weights: Vec<f32> = points.iter().map(|p| d2(p, &c0)).collect();
    let total: f32 = weights.iter().sum();
    let mut c1 = points[(first + 1) % points.len()].to_vec();
    if total > 0.0 {
        let target = rng.next() * total;
        let mut acc = 0.0f32;
        for (i, w) in weights.iter().enumerate() {
            acc += *w;
            if acc >= target {
                c1 = points[i].to_vec();
                break;
            }
        }
        if l2_sq(&c0, &c1) == 0.0 {
            // Degenerate data (all points identical): any split is as good.
            c1 = points[points.len() - 1].to_vec();
        }
    }

    let mut centroids = [c0, c1];
    let mut second = vec![false; points.len()];
    for _ in 0..iterations.max(1) {
        assign(points, &centroids, &mut second);
        let (a, b) = groups(points, &second);
        if a.is_empty() || b.is_empty() {
            break;
        }
        centroids = [mean(&a, dim), mean(&b, dim)];
    }

    assign(points, &centroids, &mut second);
    rebalance(points, dim, &mut centroids, &mut second);
    Split { centroids, second }
}

fn assign(points: &[&[f32]], centroids: &[Vec<f32>; 2], second: &mut [bool]) {
    for (i, p) in points.iter().enumerate() {
        second[i] = d2(p, &centroids[1]) < d2(p, &centroids[0]);
    }
}

fn groups<'a>(points: &[&'a [f32]], second: &[bool]) -> (Vec<&'a [f32]>, Vec<&'a [f32]>) {
    let mut a = Vec::new();
    let mut b = Vec::new();
    for (p, s) in points.iter().zip(second) {
        if *s {
            b.push(*p);
        } else {
            a.push(*p);
        }
    }
    (a, b)
}

/// Moves the farthest point of the larger cluster to the other one until the
/// sizes differ by at most one, then recomputes the centroids.
///
/// The reference moves one point per pass and rescans the whole cluster each
/// time (`O(n · moves)`); since **the centroid does not move while points are
/// being moved**, the distance of every point to it is fixed, so the m-th move
/// takes the m-th farthest point of the cluster it started from. One sort is the
/// same set of moves in the same order, for `O(n log n)` - at 20k × 128d the
/// rescans were the dominant cost of the whole build.
fn rebalance(points: &[&[f32]], dim: usize, centroids: &mut [Vec<f32>; 2], second: &mut [bool]) {
    let na = second.iter().filter(|s| !**s).count();
    let nb = second.len() - na;
    let (from_second, from_centroid) = if na > nb { (false, 0) } else { (true, 1) };
    let moves = na.abs_diff(nb) / 2;
    if moves > 0 {
        let mut order: Vec<(f32, usize)> = points
            .iter()
            .enumerate()
            .filter(|(i, _)| second[*i] == from_second)
            .map(|(i, p)| (d2(p, &centroids[from_centroid]), i))
            // A NaN distance is never "the farthest" in the reference's
            // `d > worst` comparison, so it never moves.
            .filter(|(d, _)| !d.is_nan())
            .collect();
        // Farthest first; ties keep the reference's lowest-index-first order.
        order.sort_by(|a, b| b.0.total_cmp(&a.0).then(a.1.cmp(&b.1)));
        for (_, idx) in order.into_iter().take(moves) {
            second[idx] = !from_second;
        }
    }
    let (a, b) = groups(points, second);
    *centroids = [mean(&a, dim), mean(&b, dim)];
}

#[cfg(test)]
mod tests {
    use super::*;

    fn clusters() -> Vec<Vec<f32>> {
        let mut out = Vec::new();
        for i in 0..40 {
            let base = if i % 2 == 0 { 0.0 } else { 10.0 };
            out.push(vec![
                base + (i % 5) as f32 * 0.1,
                base + (i % 3) as f32 * 0.1,
            ]);
        }
        out
    }

    #[test]
    fn splits_two_well_separated_clusters() {
        let data = clusters();
        let refs: Vec<&[f32]> = data.iter().map(|v| v.as_slice()).collect();
        let split = balanced_kmeans_2(&refs, 2, 16, 7);
        let (a, b) = split.sizes();
        assert_eq!(a, b);
        // Every even row is near 0, every odd one near 10.
        for (i, second) in split.second.iter().enumerate() {
            assert_eq!(*second, i % 2 == 1, "row {i} landed in the wrong half");
        }
    }

    #[test]
    fn is_balanced_and_deterministic() {
        let data: Vec<Vec<f32>> = (0..101)
            .map(|i| vec![i as f32, (i * 7 % 13) as f32])
            .collect();
        let refs: Vec<&[f32]> = data.iter().map(|v| v.as_slice()).collect();
        let a = balanced_kmeans_2(&refs, 2, 8, 3);
        let b = balanced_kmeans_2(&refs, 2, 8, 3);
        assert_eq!(a, b);
        let (na, nb) = a.sizes();
        assert!(na.abs_diff(nb) <= 1, "unbalanced: {na} vs {nb}");
        assert_eq!(na + nb, 101);
    }

    #[test]
    fn handles_degenerate_input() {
        let one = [[1.0f32, 2.0]];
        let split = balanced_kmeans_2(&[one[0].as_slice()], 2, 4, 1);
        assert_eq!(split.second, vec![false]);

        let same: Vec<Vec<f32>> = (0..8).map(|_| vec![1.0, 1.0]).collect();
        let refs: Vec<&[f32]> = same.iter().map(|v| v.as_slice()).collect();
        let split = balanced_kmeans_2(&refs, 2, 4, 1);
        assert_eq!(split.second.len(), 8);
        let (na, nb) = split.sizes();
        assert!(na.abs_diff(nb) <= 1);
    }

    /// The reference's one-move-per-pass loop, kept as the oracle for the sorted
    /// form below.
    fn rebalance_old(
        points: &[&[f32]],
        dim: usize,
        centroids: &mut [Vec<f32>; 2],
        second: &mut [bool],
    ) {
        loop {
            let (na, nb) = {
                let a = second.iter().filter(|s| !**s).count();
                (a, second.len() - a)
            };
            if na.abs_diff(nb) <= 1 {
                break;
            }
            let (from_second, from_centroid) = if na > nb { (false, 0) } else { (true, 1) };
            let mut worst: Option<(usize, f32)> = None;
            for (i, p) in points.iter().enumerate() {
                if second[i] != from_second {
                    continue;
                }
                let d = d2(p, &centroids[from_centroid]);
                if worst.is_none_or(|(_, w)| d > w) {
                    worst = Some((i, d));
                }
            }
            let Some((idx, _)) = worst else { break };
            second[idx] = !from_second;
        }
        let (a, b) = groups(points, second);
        *centroids = [mean(&a, dim), mean(&b, dim)];
    }

    /// The sorted form moves the same points in the same order, so the split it
    /// hands the tree is the one the reference's rescans produce.
    #[test]
    fn sorted_rebalance_matches_the_pass_loop() {
        let mut state = 12345u64;
        let mut rand = move || {
            state = state.wrapping_mul(6364136223846793005).wrapping_add(1);
            ((state >> 33) as f32) / ((1u64 << 31) as f32)
        };
        for trial in 0..200 {
            let n = 4 + trial % 37;
            let dim = 3;
            let data: Vec<Vec<f32>> = (0..n)
                .map(|_| (0..dim).map(|_| rand() * 10.0).collect())
                .collect();
            let refs: Vec<&[f32]> = data.iter().map(|v| v.as_slice()).collect();
            for (c0, c1) in [
                (data[0].clone(), data[n - 1].clone()),
                (
                    (0..dim).map(|_| rand()).collect::<Vec<f32>>(),
                    (0..dim).map(|_| rand()).collect::<Vec<f32>>(),
                ),
            ] {
                // The unbalanced shape the pass loop exists for: most points on
                // one side. `rebalance` is what evens them out.
                let mut old_second = vec![false; n];
                old_second[n - 1] = true;
                if n > 3 {
                    old_second[n - 2] = true;
                }
                let mut new_second = old_second.clone();
                let (mut old_c, mut new_c) = ([c0.clone(), c1.clone()], [c0, c1]);
                rebalance_old(&refs, dim, &mut old_c, &mut old_second);
                rebalance(&refs, dim, &mut new_c, &mut new_second);
                assert_eq!(old_second, new_second, "trial {trial}: mask differs");
                assert_eq!(old_c, new_c, "trial {trial}: centroids differ");
            }
        }
    }
}
