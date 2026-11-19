//! One tree node: a centroid, its radius, and the members it points at.
//!
//! A leaf's members are **row ids**; a branch node's members are the package's
//! **child partition keys** (their codes are the child centroids encoded against
//! this node's centroid). That is the C-SPANN shape (`vecstore/partition.go`):
//! one uniform node type, so the descent and the leaf scan are the same code
//! path.
//!
//! `encode = none` keeps the raw member vectors beside the codes so the tree can
//! be validated against a brute-force scan byte for byte (`SPANN_DESIGN.md`
//! §16 P1 exit criterion).

use crate::vector::{l2_sq, Metric};

use super::quantize::{self, Code, QueryCode, Rotation};
use super::search::SearchSet;
use super::{Encode, PartitionKey};

/// One member of a partition: a row id (leaf) or a child key (branch).
#[derive(Clone, Debug, PartialEq)]
pub struct Member {
    pub id: u64,
    pub code: Code,
}

/// A tree node.
#[derive(Clone, Debug)]
pub struct Partition {
    pub key: PartitionKey,
    /// `1` = leaf, `root.level == height` (§3 I3).
    pub level: u32,
    /// Mean of the members, in the raw space.
    pub centroid: Vec<f32>,
    /// `R_p = max‖o − c‖`, for the radius lower bound (§8.3).
    pub radius: f32,
    pub members: Vec<Member>,
    /// Child keys, empty for a leaf.
    pub children: Vec<PartitionKey>,
    /// Flattened raw member vectors, only for `encode = none`.
    raw: Option<Vec<f32>>,
    /// Duplicated here so the search path does not thread `Options` down.
    rot_centroid: Vec<f32>,
    dim: usize,
}

impl Partition {
    /// Builds a node from `(member id, vector)` pairs.
    ///
    /// The vectors are borrowed (`&[f32]`) rather than owned: a build over a
    /// mapped scratch file must not copy the column into `Vec<f32>` per member
    /// (6 KB each at 1536d), and a leaf's codes are all the node keeps. They
    /// arrive as `AsRef<[f32]>` so a source can hand over its own borrow (`&[f32]`)
    /// or a `Cow` it widened for this member only.
    pub fn build<V: AsRef<[f32]>>(
        key: PartitionKey,
        level: u32,
        dim: usize,
        members: &[(u64, V)],
        rot: &Rotation,
        encode: Encode,
    ) -> Self {
        let centroid = mean_of(members, dim);
        let rot_centroid = rot.apply(&centroid);
        let mut radius = 0.0f32;
        let mut codes = Vec::with_capacity(members.len());
        let mut raw = (encode == Encode::None).then(|| Vec::with_capacity(members.len() * dim));
        for (id, v) in members {
            let v = v.as_ref();
            radius = radius.max(l2_sq(v, &centroid).sqrt());
            let code = quantize::encode(rot, &rot_centroid, v);
            codes.push(Member { id: *id, code });
            if let Some(buf) = raw.as_mut() {
                buf.extend_from_slice(v);
            }
        }
        Self {
            key,
            level,
            centroid,
            radius,
            members: codes,
            children: Vec::new(),
            raw,
            rot_centroid,
            dim,
        }
    }

    /// Rebuilds a node from decoded storage (`SPANN_DESIGN.md` §6.3): the
    /// chunk reader hands us the columns it just parsed, so the search path is
    /// identical to the build-time one.
    #[allow(clippy::too_many_arguments)]
    pub fn from_decoded(
        key: PartitionKey,
        level: u32,
        dim: usize,
        centroid: Vec<f32>,
        radius: f32,
        rot_centroid: Vec<f32>,
        members: Vec<Member>,
        children: Vec<PartitionKey>,
    ) -> Self {
        Self {
            key,
            level,
            centroid,
            radius,
            members,
            children,
            raw: None,
            rot_centroid,
            dim,
        }
    }

    pub fn dim(&self) -> usize {
        self.dim
    }

    pub fn rot_centroid(&self) -> &[f32] {
        &self.rot_centroid
    }

    pub fn is_leaf(&self) -> bool {
        self.level <= 1
    }

    pub fn len(&self) -> usize {
        self.members.len()
    }

    pub fn is_empty(&self) -> bool {
        self.members.is_empty()
    }

    /// Bytes the retained member vectors occupy (`encode = none` keeps them).
    pub fn raw_bytes(&self) -> usize {
        self.raw
            .as_ref()
            .map(|raw| raw.len() * std::mem::size_of::<f32>())
            .unwrap_or(0)
    }

    /// `R_p` recomputed from the stored member vectors (format self-check and
    /// the `encode = none` conformance path).
    pub fn raw_member(&self, index: usize) -> Option<&[f32]> {
        let raw = self.raw.as_ref()?;
        let at = index * self.dim;
        raw.get(at..at + self.dim)
    }

    /// The query side of the codes for **this** centroid (§8.5).
    pub fn make_query(&self, rot: &Rotation, q: &[f32]) -> QueryCode {
        quantize::make_query(rot, &self.rot_centroid, q)
    }

    /// `lb_part(p) = max(0, ‖q − c_p‖ − R_p)` for the L2 metric, and the plain
    /// centroid distance for ip/cosine (a heuristic: radii live in L2 space, so
    /// they cannot bound an inner product).
    pub fn lower_bound(&self, q: &[f32], metric: Metric) -> f32 {
        let d = self.centroid_distance(q, metric);
        if metric.is_l2() {
            (d - self.radius).max(0.0)
        } else {
            d
        }
    }

    /// Distance from the query to this partition's centroid. This is the
    /// best-first walk's ranking key: a partition holds the rows of its own
    /// Voronoi cell, so the centroid distance ranks cells far more tightly than
    /// `lower_bound`, which flattens to 0 for every partition whose radius
    /// exceeds its centroid distance.
    pub fn centroid_distance(&self, q: &[f32], _metric: Metric) -> f32 {
        l2_sq(q, &self.centroid).max(0.0).sqrt()
    }

    /// Scores every member into `out`.
    /// Scores the leaf's members into `out`.
    ///
    /// `allowed` is the statement's filter bitmap. It is tested **here**, per
    /// slot, instead of after the fact: widening the candidate pool by
    /// `1/selectivity` was how a filter reached the tree before, and at 1%
    /// selectivity that asked for the whole table (`§12.5`).
    pub fn search_into(
        &self,
        query: &QueryCode,
        q_raw: &[f32],
        metric: Metric,
        out: &mut SearchSet,
        allowed: Option<&std::collections::HashSet<u64>>,
    ) {
        for (index, member) in self.members.iter().enumerate() {
            if let Some(allowed) = allowed {
                if !allowed.contains(&member.id) {
                    continue;
                }
            }
            let (squared, err) = match self.raw_member(index) {
                Some(raw) => (l2_sq(q_raw, raw), 0.0f32),
                None => (
                    quantize::distance_estimate(query, &member.code, self.dim),
                    quantize::error_bound(query, &member.code),
                ),
            };
            let (score, bound) =
                metric_score(metric, squared, err, query.raw_norm, member.code.raw_norm);
            out.add(score, bound, member.id);
        }
    }
}

/// Turns a squared-L2 estimate into the metric's "smaller is better" score.
///
/// Public because the file-backed store (`crates/storage`) scores slot pages
/// without materialising a [`Partition`], and both paths must rank identically.
pub fn metric_score(metric: Metric, squared: f32, err: f32, q_raw: f32, o_raw: f32) -> (f32, f32) {
    match metric {
        Metric::L2 => (squared, err),
        Metric::Ip => {
            let ip = 0.5 * (q_raw + o_raw - squared);
            (-ip, 0.5 * err)
        }
        Metric::Cos => {
            let denom = (q_raw * o_raw).max(1e-12).sqrt();
            let ip = 0.5 * (q_raw + o_raw - squared);
            (-ip / denom, 0.5 * err / denom)
        }
    }
}

fn mean_of<V: AsRef<[f32]>>(members: &[(u64, V)], dim: usize) -> Vec<f32> {
    let mut out = vec![0.0f32; dim];
    if members.is_empty() {
        return out;
    }
    for (_, v) in members {
        for (o, x) in out.iter_mut().zip(v.as_ref().iter()) {
            *o += *x;
        }
    }
    let n = members.len() as f32;
    for o in &mut out {
        *o /= n;
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    /// The `i * 0.01` term breaks the distance ties the `i % 4` cycle would
    /// otherwise create, so the exact top-k is unique.
    fn points(n: usize) -> Vec<(u64, Vec<f32>)> {
        (0..n)
            .map(|i| {
                (
                    i as u64,
                    vec![i as f32 * 0.5, (i % 4) as f32, i as f32 * 0.01, -1.0],
                )
            })
            .collect()
    }

    /// Borrowed members, the shape `Partition::build` takes.
    fn refs(pts: &[(u64, Vec<f32>)]) -> Vec<(u64, &[f32])> {
        pts.iter().map(|(id, v)| (*id, v.as_slice())).collect()
    }

    #[test]
    fn search_finds_the_nearest_members() {
        let pts = points(32);
        let rot = Rotation::new(4, 5);
        let part = Partition::build(1, 1, 4, &refs(&pts), &rot, Encode::None);
        let q = [7.75, 1.5, 1.0, -1.0];
        let mut set = SearchSet::new(3, 0);
        let qc = part.make_query(&rot, &q);
        part.search_into(&qc, &q, Metric::L2, &mut set, None);
        let ids: Vec<u64> = set.into_sorted().into_iter().map(|(id, _)| id).collect();

        let mut exact: Vec<(u64, f32)> = pts.iter().map(|(id, v)| (*id, l2_sq(&q, v))).collect();
        exact.sort_by(|a, b| a.1.total_cmp(&b.1));
        let want: Vec<u64> = exact.into_iter().take(3).map(|(id, _)| id).collect();
        assert_eq!(ids, want);
    }

    /// `encode = none` scores the raw vectors, so the answer is the exact top-k.
    #[test]
    fn encode_none_matches_the_brute_force_top_k() {
        let pts = points(64);
        let rot = Rotation::new(4, 5);
        let part = Partition::build(1, 1, 4, &refs(&pts), &rot, Encode::None);
        let q = [12.3, 2.2, 0.9, -1.1];
        let mut set = SearchSet::new(5, 0);
        let qc = part.make_query(&rot, &q);
        part.search_into(&qc, &q, Metric::L2, &mut set, None);
        let got: Vec<u64> = set.into_sorted().into_iter().map(|(id, _)| id).collect();

        let mut exact: Vec<(u64, f32)> = pts.iter().map(|(id, v)| (*id, l2_sq(&q, v))).collect();
        exact.sort_by(|a, b| a.1.total_cmp(&b.1));
        let want: Vec<u64> = exact.into_iter().take(5).map(|(id, _)| id).collect();
        assert_eq!(got, want);
    }

    #[test]
    fn radius_bounds_every_member() {
        let pts = points(16);
        let rot = Rotation::new(4, 5);
        let part = Partition::build(1, 1, 4, &refs(&pts), &rot, Encode::Rabitq);
        for (_, v) in &pts {
            assert!(l2_sq(v, &part.centroid).sqrt() <= part.radius + 1e-5);
        }
        let far = [1000.0f32, 0.0, 0.0, 0.0];
        assert!(part.lower_bound(&far, Metric::L2) > 0.0);
    }
}
