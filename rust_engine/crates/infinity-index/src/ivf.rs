//! IVF retrieval over the reference's chunk layout (plan #89).
//!
//! The chunk stores `centroid_count` centroids and one part per centroid: the
//! part's `SegmentOffset`s and its raw vectors (plain storage). A search picks
//! the `nprobe` nearest centroids, scans their parts, and returns the top-k
//! candidate row offsets — the executor then scores those exactly, the same
//! shape as the HNSW candidate path.
//!
//! C++ reference: `src/storage/knn_index/knn_ivf/ivf_index_storage.cpp:291`
//! (`SearchIndex`: centroid selection + part scan) and
//! `ivf_index_storage_parts_impl.cpp` (plain part layout).

use crate::vector::l2_sq;

/// One IVF chunk in memory (plain storage).
pub struct IvfIndex {
    pub dims: usize,
    /// `centroid_count * dims` floats, row-major.
    pub centroids: Vec<f32>,
    pub centroid_count: usize,
    /// Per centroid: `(segment_offset, vector)` for every assigned embedding.
    pub parts: Vec<Vec<(u32, Vec<f32>)>>,
}

fn dot(a: &[f32], b: &[f32]) -> f32 {
    a.iter().zip(b).map(|(x, y)| x * y).sum()
}

fn cosine(a: &[f32], b: &[f32]) -> f32 {
    let na = a.iter().map(|x| x * x).sum::<f32>().sqrt();
    let nb = b.iter().map(|x| x * x).sum::<f32>().sqrt();
    if na > 0.0 && nb > 0.0 {
        dot(a, b) / (na * nb)
    } else {
        0.0
    }
}

impl IvfIndex {
    pub fn new(
        dims: usize,
        centroid_count: usize,
        centroids: Vec<f32>,
        parts: Vec<Vec<(u32, Vec<f32>)>>,
    ) -> Self {
        Self {
            dims,
            centroids,
            centroid_count,
            parts,
        }
    }

    /// Higher-is-better score for one vector (negated L2, or the similarity).
    fn score(&self, query: &[f32], vector: &[f32], metric: &str) -> f32 {
        match metric {
            "ip" => dot(query, vector),
            "cosine" => cosine(query, vector),
            _ => -l2_sq(query, vector),
        }
    }

    /// Top-k candidate row offsets: scan the `nprobe` best centroids' parts.
    pub fn candidates(&self, query: &[f32], k: usize, nprobe: usize, metric: &str) -> Vec<u32> {
        if k == 0
            || self.dims == 0
            || query.len() != self.dims
            || self.centroid_count == 0
            || self.parts.is_empty()
        {
            return Vec::new();
        }
        let nprobe = nprobe.clamp(1, self.centroid_count);
        let mut centroids: Vec<(usize, f32)> = (0..self.centroid_count)
            .map(|c| {
                let at = c * self.dims;
                let slice = &self.centroids[at..at + self.dims];
                (c, self.score(query, slice, metric))
            })
            .collect();
        // Higher score first; ties by centroid id.
        centroids.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        let mut hits: Vec<(u32, f32)> = Vec::new();
        for (c, _) in centroids.into_iter().take(nprobe) {
            if let Some(part) = self.parts.get(c) {
                for (offset, vector) in part {
                    if vector.len() == self.dims {
                        hits.push((*offset, self.score(query, vector, metric)));
                    }
                }
            }
        }
        hits.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        hits.truncate(k);
        hits.into_iter().map(|(offset, _)| offset).collect()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn sample() -> IvfIndex {
        // Two centroids; the first owns vectors near [1, 0], the second [0, 1].
        IvfIndex::new(
            2,
            2,
            vec![1.0, 0.0, 0.0, 1.0],
            vec![
                vec![(10, vec![1.0, 0.0]), (11, vec![0.9, 0.1])],
                vec![(20, vec![0.0, 1.0]), (21, vec![0.1, 0.9])],
            ],
        )
    }

    #[test]
    fn picks_the_nearest_part() {
        let index = sample();
        let hits = index.candidates(&[0.0, 1.0], 1, 1, "l2");
        assert_eq!(hits[0], 20);
        let hits = index.candidates(&[1.0, 0.0], 2, 1, "ip");
        assert_eq!(hits, vec![10, 11]);
    }

    #[test]
    fn nprobe_covers_every_part() {
        let index = sample();
        let hits = index.candidates(&[1.0, 0.0], 4, 2, "l2");
        assert_eq!(hits.len(), 4);
    }
}
