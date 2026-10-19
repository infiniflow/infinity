//! Flat (brute-force) top-k search over dense vectors.
//!
//! C++: the deprecated `KnnFlatL2` / `KnnFlatIp` / `KnnFlatL2Top1` /
//! `KnnFlatIpTop1` classes (`deprecated_knn_flat_*.cppm`). The rewrite keeps
//! their behavior on top of the shared distance primitives instead of a family
//! of template classes (registered as a structural merge in `CLASS_MAP.md`).

use crate::vector::Metric;

/// One search result: the base vector's index and its distance.
#[derive(Debug, Clone, Copy, PartialEq)]
pub struct FlatHit {
    pub index: usize,
    pub distance: f32,
}

fn distance(metric: Metric, query: &[f32], base: &[f32]) -> f32 {
    match metric {
        Metric::L2 => crate::vector::l2_sq(query, base),
        // Inner product: the reference reports and ranks by the similarity,
        // largest first.
        Metric::Ip => query.iter().zip(base).map(|(a, b)| a * b).sum::<f32>(),
        Metric::Cos => {
            let dot: f32 = query.iter().zip(base).map(|(a, b)| a * b).sum();
            let na: f32 = query.iter().map(|a| a * a).sum::<f32>().sqrt();
            let nb: f32 = base.iter().map(|b| b * b).sum::<f32>().sqrt();
            if na == 0.0 || nb == 0.0 {
                0.0
            } else {
                -dot / (na * nb)
            }
        }
    }
}

/// C++: `KnnFlatL2::Search` / `KnnFlatIp::Search` — top-`k` over `base`,
/// skipping masked-out slots, ties broken by the lower index.
pub fn top_k(
    query: &[f32],
    base: &[Vec<f32>],
    k: usize,
    metric: Metric,
    mask: Option<&[bool]>,
) -> Vec<FlatHit> {
    let mut hits: Vec<FlatHit> = base
        .iter()
        .enumerate()
        .filter(|(i, _)| mask.map(|m| *m.get(*i).unwrap_or(&false)).unwrap_or(true))
        .map(|(index, vector)| FlatHit {
            index,
            distance: distance(metric, query, vector),
        })
        .collect();
    // Every metric ranks by "smaller is closer" except inner product, whose
    // reported value is a similarity (larger is closer).
    let rank = |hit: &FlatHit| {
        if metric == Metric::Ip {
            -hit.distance
        } else {
            hit.distance
        }
    };
    hits.sort_by(|a, b| {
        rank(a)
            .partial_cmp(&rank(b))
            .unwrap_or(std::cmp::Ordering::Equal)
            .then(a.index.cmp(&b.index))
    });
    hits.truncate(k);
    hits
}

/// C++: `KnnFlatL2Top1::Search` / `KnnFlatIpTop1::Search` — the single best hit.
pub fn top_1(
    query: &[f32],
    base: &[Vec<f32>],
    metric: Metric,
    mask: Option<&[bool]>,
) -> Option<FlatHit> {
    top_k(query, base, 1, metric, mask).into_iter().next()
}

#[cfg(test)]
mod tests {
    use super::*;

    fn base_vectors() -> Vec<Vec<f32>> {
        vec![
            vec![0.1, 0.2, 0.3, 0.4],
            vec![0.2, 0.1, 0.3, 0.4],
            vec![0.3, 0.2, 0.1, 0.4],
            vec![0.4, 0.3, 0.2, 0.1],
        ]
    }

    fn query() -> Vec<f32> {
        vec![0.1, 0.2, 0.3, 0.4]
    }

    fn assert_hits(hits: &[FlatHit], expected: &[(usize, f32)]) {
        assert_eq!(hits.len(), expected.len());
        for (hit, (index, distance)) in hits.iter().zip(expected) {
            assert_eq!(hit.index, *index);
            assert!(
                (hit.distance - distance).abs() < 1e-6,
                "{hit:?} vs {distance}"
            );
        }
    }

    /// `src/unit_test/storage/knnindex/knn_flat/knn_flat_l2_ut.cpp::test1`.
    #[test]
    fn l2_top_k_matches_the_reference() {
        let base = base_vectors();
        let hits = top_k(&query(), &base, 4, Metric::L2, None);
        assert_hits(&hits, &[(0, 0.0), (1, 0.02), (2, 0.08), (3, 0.2)]);

        let hits = top_k(
            &query(),
            &base,
            4,
            Metric::L2,
            Some(&[true, false, true, true]),
        );
        assert_hits(&hits, &[(0, 0.0), (2, 0.08), (3, 0.2)]);

        let hits = top_k(
            &query(),
            &base,
            4,
            Metric::L2,
            Some(&[false, false, true, true]),
        );
        assert_hits(&hits, &[(2, 0.08), (3, 0.2)]);

        let hits = top_k(
            &query(),
            &base,
            4,
            Metric::L2,
            Some(&[false, false, false, true]),
        );
        assert_hits(&hits, &[(3, 0.2)]);
    }

    /// `knn_flat_ip_ut.cpp::test1` — inner product ranks by similarity.
    #[test]
    fn ip_top_k_matches_the_reference() {
        let base = base_vectors();
        let hits = top_k(&query(), &base, 4, Metric::Ip, None);
        assert_hits(&hits, &[(0, 0.3), (1, 0.29), (2, 0.26), (3, 0.20)]);

        let hits = top_k(
            &query(),
            &base,
            4,
            Metric::Ip,
            Some(&[true, false, true, true]),
        );
        assert_hits(&hits, &[(0, 0.3), (2, 0.26), (3, 0.2)]);

        let hits = top_k(
            &query(),
            &base,
            4,
            Metric::Ip,
            Some(&[false, false, true, true]),
        );
        assert_hits(&hits, &[(2, 0.26), (3, 0.2)]);

        let hits = top_k(
            &query(),
            &base,
            4,
            Metric::Ip,
            Some(&[false, false, false, true]),
        );
        assert_hits(&hits, &[(3, 0.2)]);
    }

    /// `knn_flat_l2_top1_ut.cpp::test1`.
    #[test]
    fn l2_top_1_matches_the_reference() {
        let base = base_vectors();
        assert_hits(
            &[top_1(&query(), &base, Metric::L2, None).unwrap()],
            &[(0, 0.0)],
        );
        assert_hits(
            &[top_1(
                &query(),
                &base,
                Metric::L2,
                Some(&[true, false, true, true]),
            )
            .unwrap()],
            &[(0, 0.0)],
        );
        assert_hits(
            &[top_1(
                &query(),
                &base,
                Metric::L2,
                Some(&[false, false, true, true]),
            )
            .unwrap()],
            &[(2, 0.08)],
        );
        assert_hits(
            &[top_1(
                &query(),
                &base,
                Metric::L2,
                Some(&[false, false, false, true]),
            )
            .unwrap()],
            &[(3, 0.2)],
        );
    }

    /// `knn_flat_ip_top1_ut.cpp::test1`.
    #[test]
    fn ip_top_1_matches_the_reference() {
        let base = base_vectors();
        assert_hits(
            &[top_1(&query(), &base, Metric::Ip, None).unwrap()],
            &[(0, 0.3)],
        );
        assert_hits(
            &[top_1(
                &query(),
                &base,
                Metric::Ip,
                Some(&[true, false, true, true]),
            )
            .unwrap()],
            &[(0, 0.3)],
        );
        assert_hits(
            &[top_1(
                &query(),
                &base,
                Metric::Ip,
                Some(&[false, false, true, true]),
            )
            .unwrap()],
            &[(2, 0.26)],
        );
        assert_hits(
            &[top_1(
                &query(),
                &base,
                Metric::Ip,
                Some(&[false, false, false, true]),
            )
            .unwrap()],
            &[(3, 0.2)],
        );
    }
}
