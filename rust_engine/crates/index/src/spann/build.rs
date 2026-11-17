//! Tree construction (`SPANN_DESIGN.md` §4 `build.rs`, §5, §11.2).
//!
//! Cockroach's `BuildSubtree` recursively bisects a partition until it fits
//! `max_partition_size`, then assigns levels bottom-up (`leaf = 1`, parent =
//! `max(child) + 1`). The Rust port does the same, which gives the two
//! properties the format depends on:
//!
//! * `height` is derived, not configured (`D3`) — a bisection tree over `n`
//!   rows is `ceil(log2(n / max_partition_size)) + 1` deep;
//! * `num_centroids` is implied by the fill factor: the leaf count lands near
//!   `n / (fill_factor · max_partition_size)`.
//!
//! Keys are assigned in DFS pre-order, so the root is always `ROOT_KEY == 0`
//! and a parent's key is smaller than its children's.

use std::collections::HashMap;

use super::kmeans::balanced_kmeans_2;
use super::partition::Partition;
use super::quantize::Rotation;
use super::source::VectorSource;
use super::{Encode, Options, PartitionKey, ROOT_KEY};

/// The tree as built, before any storage decision is made.
#[derive(Debug)]
pub struct BuiltTree {
    /// Indexed by `key as usize` (pre-order numbering leaves no holes).
    pub partitions: Vec<Partition>,
    pub root: PartitionKey,
    pub height: u32,
}

impl BuiltTree {
    pub fn get(&self, key: PartitionKey) -> Option<&Partition> {
        self.partitions.get(key as usize)
    }

    pub fn leaves(&self) -> impl Iterator<Item = &Partition> {
        self.partitions.iter().filter(|p| p.is_leaf())
    }

    pub fn row_count(&self) -> usize {
        self.leaves().map(|p| p.len()).sum()
    }
}

/// Rows below which a subtree is no longer worth a thread: one bisection of this
/// many rows costs tens of microseconds - above a thread spawn, and the subtree
/// that would run beside it is no bigger.
const PARALLEL_MIN_ROWS: usize = 256;

struct Builder<'a> {
    /// Random-read access to the vectors: the caller decides whether they live
    /// in RAM or in a mapped scratch file (see `source`).
    src: &'a dyn VectorSource,
    dim: usize,
    opts: &'a Options,
    rot: &'a Rotation,
    /// Subtrees currently being built on their own thread, and the ceiling.
    /// Work-conserving: a split only forks while there is an idle core, and a
    /// subtree that cannot fork just builds inline (same result, same keys).
    active: std::sync::atomic::AtomicUsize,
    workers: usize,
}

/// One subtree, built bottom-up and handed to its parent.
///
/// The parent needs three things from a child: its level (for `max(child) + 1`),
/// its centroid (to encode itself) and its nodes. Returning them instead of
/// writing into a shared `Vec<Option<Partition>>` is what lets two subtrees build
/// at the same time without a lock.
struct Subtree {
    level: u32,
    centroid: Vec<f32>,
    parts: Vec<(PartitionKey, Partition)>,
}

/// Builds the whole tree over `src`.
pub fn build_tree(src: &dyn VectorSource, dim: usize, opts: &Options, rot: &Rotation) -> BuiltTree {
    let builder = Builder {
        src,
        dim,
        opts,
        rot,
        active: std::sync::atomic::AtomicUsize::new(0),
        workers: workers(),
    };
    let mut indices: Vec<usize> = (0..src.len()).collect();
    let mut built = builder.build_subtree(&mut indices, ROOT_KEY);
    // `parts` is in build order (any order, now that keys are explicit): the
    // tree vector is indexed by key, so place each node where it belongs.
    let mut slots: Vec<Option<Partition>> = (0..built.parts.len()).map(|_| None).collect();
    for (key, part) in built.parts.drain(..) {
        let slot = slots
            .get_mut(key as usize)
            .expect("partition key outside the tree");
        *slot = Some(part);
    }
    let partitions: Vec<Partition> = slots
        .into_iter()
        .map(|p| p.expect("every key below the node count is filled"))
        .collect();
    debug_assert_eq!(
        partitions.len(),
        subtree_nodes(src.len(), opts.max_partition_size)
    );
    BuiltTree {
        partitions,
        root: ROOT_KEY,
        height: built.level,
    }
}

/// How many nodes a bisection subtree over `n` rows has.
///
/// Data-independent, which is why the keys can be handed out before either child
/// is built: the balance pass keeps `|left − right| <= 1`, so the shape only
/// depends on `n` (and `s(k) + s(k+1)` is the same whichever side got the extra
/// row). Every split halves, so callers (internal nodes only) pay
/// `O(nodes)` per call and `O(nodes · depth)` in total.
fn subtree_nodes(n: usize, max_partition_size: usize) -> usize {
    if n <= max_partition_size {
        return 1;
    }
    1 + subtree_nodes(n / 2, max_partition_size) + subtree_nodes(n - n / 2, max_partition_size)
}

fn workers() -> usize {
    std::thread::available_parallelism()
        .map(|n| n.get())
        .unwrap_or(1)
}

impl Builder<'_> {
    /// Takes a worker permit, or `false` when every core is busy.
    fn acquire(&self) -> bool {
        use std::sync::atomic::Ordering;
        let mut current = self.active.load(Ordering::Relaxed);
        loop {
            if current >= self.workers {
                return false;
            }
            match self.active.compare_exchange_weak(
                current,
                current + 1,
                Ordering::AcqRel,
                Ordering::Relaxed,
            ) {
                Ok(_) => return true,
                Err(seen) => current = seen,
            }
        }
    }

    fn release(&self) {
        self.active
            .fetch_sub(1, std::sync::atomic::Ordering::AcqRel);
    }

    /// Builds the subtree rooted at `key`, whose rows are `indices`.
    ///
    /// `key` is passed in rather than allocated here, so a parent can compute
    /// both children's keys from the split sizes and then build them
    /// concurrently - the numbering stays the DFS pre-order the format expects
    /// (`SPANN_DESIGN.md` §4), it just no longer needs a mutable counter.
    fn build_subtree(&self, indices: &mut [usize], key: PartitionKey) -> Subtree {
        if indices.len() <= self.opts.max_partition_size {
            let members: Vec<(u64, &[f32])> = indices
                .iter()
                .map(|i| (self.src.id(*i), self.src.vector(*i)))
                .collect();
            let part = Partition::build(key, 1, self.dim, &members, self.rot, self.opts.encode);
            return Subtree {
                level: 1,
                centroid: part.centroid.clone(),
                parts: vec![(key, part)],
            };
        }

        // Bisect: `BalancedKMeans(k=2)` over the slice's vectors.
        let vectors: Vec<&[f32]> = indices.iter().map(|i| self.src.vector(*i)).collect();
        let split = balanced_kmeans_2(
            &vectors,
            self.dim,
            self.opts.kmeans_iterations,
            self.opts.seed ^ u64::from(key),
        );
        let (mut left, mut right): (Vec<usize>, Vec<usize>) = (Vec::new(), Vec::new());
        for (pos, second) in split.second.iter().enumerate() {
            if *second {
                right.push(indices[pos]);
            } else {
                left.push(indices[pos]);
            }
        }
        if left.is_empty() || right.is_empty() {
            // Degenerate split (all vectors identical): a half/half cut still
            // terminates and keeps the tree balanced.
            let mid = indices.len() / 2;
            left = indices[..mid].to_vec();
            right = indices[mid..].to_vec();
        }

        // Both keys are known before either child is built: the left one is the
        // next pre-order key, the right one is past the whole left subtree.
        let left_key = key + 1;
        let right_key =
            left_key + subtree_nodes(left.len(), self.opts.max_partition_size) as PartitionKey;

        let forked = right.len() >= PARALLEL_MIN_ROWS && self.acquire();
        let (left, right) = if forked {
            std::thread::scope(|scope| {
                let handle = scope.spawn(|| self.build_subtree(&mut right, right_key));
                let left = self.build_subtree(&mut left, left_key);
                let right = match handle.join() {
                    Ok(subtree) => subtree,
                    Err(payload) => std::panic::resume_unwind(payload),
                };
                (left, right)
            })
        } else {
            (
                self.build_subtree(&mut left, left_key),
                self.build_subtree(&mut right, right_key),
            )
        };
        if forked {
            self.release();
        }

        let level = left.level.max(right.level) + 1;
        let members: Vec<(u64, &[f32])> = vec![
            (u64::from(left_key), left.centroid.as_slice()),
            (u64::from(right_key), right.centroid.as_slice()),
        ];
        let mut part = Partition::build(key, level, self.dim, &members, self.rot, self.opts.encode);
        part.children = vec![left_key, right_key];

        let centroid = part.centroid.clone();
        let mut parts = Vec::with_capacity(left.parts.len() + right.parts.len() + 1);
        parts.extend(left.parts);
        parts.extend(right.parts);
        parts.push((key, part));
        Subtree {
            level,
            centroid,
            parts,
        }
    }
}

/// Collapses the top levels into **one wide root** (§11.2).
///
/// A pure bisection tree has a fan-out of two at every level, so a beam of
/// `nprobe` partitions would cover only `2^k` of the `n / max` leaves. C-SPANN's
/// root instead holds every centroid of one level (`num_centroids`), and the
/// beam over it *is* the design's `nprobe`. Collapsing is free: the intermediate
/// centroids already exist, so this only re-parents the nodes at
/// `level == height - d` under a new root and drops the levels above them.
pub fn collapse_top(tree: BuiltTree, dim: usize, opts: &Options, rot: &Rotation) -> BuiltTree {
    let target = opts.root_fanout.max(2) as u64;
    let mut d = 0u32;
    while u64::from(d + 1) < u64::from(tree.height) && (1u64 << (d + 1)) <= target {
        d += 1;
    }
    if d == 0 {
        return tree;
    }
    let cut = tree.height - d;

    let mut cut_keys: Vec<PartitionKey> = Vec::new();
    let mut rest_keys: Vec<PartitionKey> = Vec::new();
    for p in &tree.partitions {
        if p.level == cut {
            cut_keys.push(p.key);
        } else if p.level < cut {
            rest_keys.push(p.key);
        }
    }

    let mut new_key: HashMap<PartitionKey, PartitionKey> = HashMap::new();
    let mut next: PartitionKey = 1;
    for key in &cut_keys {
        new_key.insert(*key, next);
        next += 1;
    }
    for key in &rest_keys {
        new_key.insert(*key, next);
        next += 1;
    }

    let mut parts: Vec<Partition> = Vec::with_capacity(next as usize);
    // The wide root's members are the cut level's centroids; they are borrowed
    // for the one `Partition::build` call, which keeps only their codes.
    let centroids: Vec<(u64, Vec<f32>)> = cut_keys
        .iter()
        .filter_map(|key| {
            tree.get(*key)
                .map(|p| (u64::from(new_key[key]), p.centroid.clone()))
        })
        .collect();
    let members: Vec<(u64, &[f32])> = centroids
        .iter()
        .map(|(id, c)| (*id, c.as_slice()))
        .collect();
    let mut root = Partition::build(ROOT_KEY, cut + 1, dim, &members, rot, opts.encode);
    root.children = cut_keys.iter().map(|key| new_key[key]).collect();
    parts.push(root);

    for key in cut_keys.iter().chain(rest_keys.iter()) {
        let Some(old) = tree.get(*key) else { continue };
        let mut part = old.clone();
        part.key = new_key[key];
        if !part.is_leaf() {
            for member in &mut part.members {
                if let Some(mapped) = new_key.get(&(member.id as PartitionKey)) {
                    member.id = u64::from(*mapped);
                }
            }
            part.children = part
                .children
                .iter()
                .filter_map(|child| new_key.get(child).copied())
                .collect();
        }
        parts.push(part);
    }

    BuiltTree {
        partitions: parts,
        root: ROOT_KEY,
        height: cut + 1,
    }
}

/// Maps each partition key to its index in the flat `partitions` vector.
pub fn key_index(partitions: &[Partition]) -> HashMap<PartitionKey, usize> {
    partitions
        .iter()
        .enumerate()
        .map(|(i, p)| (p.key, i))
        .collect()
}

/// The number of leaves a bisection tree over `n` rows produces (§11.2).
pub fn expected_partitions(n: usize, opts: &Options) -> usize {
    if n <= opts.max_partition_size {
        return 1;
    }
    let fill = (opts.max_partition_size as f32 * opts.fill_factor).max(1.0);
    (n as f32 / fill).ceil() as usize
}

/// Whether `encode` needs the raw vectors retained (`encode = none` does).
pub fn keeps_raw(encode: Encode) -> bool {
    encode == Encode::None
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::spann::source::Memory;

    /// Rows under `key`, counted from the leaves.
    fn rows_under(tree: &BuiltTree, key: PartitionKey) -> usize {
        let part = tree.get(key).expect("a partition");
        if part.children.is_empty() {
            return part.members.len();
        }
        part.children
            .iter()
            .map(|child| rows_under(tree, *child))
            .sum()
    }

    /// The format numbers nodes in DFS pre-order (`SPANN_DESIGN.md` §4), and the
    /// parallel build hands both children their keys *before* either exists - so
    /// this is the property that keeps a parallel tree byte-identical to a serial
    /// one. It also checks `subtree_nodes` against the tree actually built.
    #[test]
    fn keys_are_dfs_preorder() {
        for (n, max) in [(0usize, 8usize), (1, 8), (40, 16), (1500, 16), (1500, 128)] {
            let rows: Vec<(u64, Vec<f32>)> = (0..n)
                .map(|i| {
                    (
                        i as u64,
                        vec![
                            (i % 23) as f32,
                            (i % 7) as f32 * 0.5,
                            ((i * 13) % 11) as f32,
                        ],
                    )
                })
                .collect();
            let opts = Options {
                max_partition_size: max,
                kmeans_iterations: 3,
                ..Options::default()
            };
            let rot = Rotation::new(3, opts.seed);
            let tree = build_tree(&Memory::new(rows), 3, &opts, &rot);
            assert_eq!(
                tree.partitions.len(),
                subtree_nodes(n, max),
                "n={n} max={max}: the node count is the balanced-bisection one"
            );
            for (i, part) in tree.partitions.iter().enumerate() {
                assert_eq!(part.key as usize, i, "n={n}: partitions are sorted by key");
            }
            for part in &tree.partitions {
                let [left, right] = part.children[..] else {
                    continue;
                };
                assert_eq!(left, part.key + 1, "n={n}: the left child is the next key");
                let left_nodes = subtree_nodes(rows_under(&tree, left), max) as PartitionKey;
                assert_eq!(
                    right,
                    left + left_nodes,
                    "n={n}: the right child starts past the whole left subtree"
                );
            }
            let leaves = tree.leaves().map(|p| p.members.len()).sum::<usize>();
            assert_eq!(leaves, n, "n={n}: every row is in exactly one leaf");
        }
    }

    /// Two builds of the same rows are the same tree, whether or not the build
    /// forked (the scheduling must not reach the output).
    #[test]
    fn the_tree_does_not_depend_on_scheduling() {
        let rows: Vec<(u64, Vec<f32>)> = (0..2000)
            .map(|i| {
                let c = i % 13;
                (
                    i as u64,
                    vec![(c * 20) as f32, (i % 97) as f32, ((i * 7) % 89) as f32],
                )
            })
            .collect();
        let opts = Options {
            max_partition_size: 32,
            kmeans_iterations: 4,
            ..Options::default()
        };
        let rot = Rotation::new(3, opts.seed);
        let a = build_tree(&Memory::new(rows.clone()), 3, &opts, &rot);
        let b = build_tree(&Memory::new(rows), 3, &opts, &rot);
        assert_eq!(a.height, b.height);
        assert_eq!(a.partitions.len(), b.partitions.len());
        for (x, y) in a.partitions.iter().zip(&b.partitions) {
            assert_eq!(x.key, y.key);
            assert_eq!(x.level, y.level);
            assert_eq!(x.centroid, y.centroid);
            assert_eq!(x.children, y.children);
            let ids = |p: &Partition| p.members.iter().map(|m| m.id).collect::<Vec<_>>();
            assert_eq!(ids(x), ids(y));
        }
    }
}
