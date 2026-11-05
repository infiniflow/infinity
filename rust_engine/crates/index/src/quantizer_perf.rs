//! Port of `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp`.
//!
//! Every case of the reference is a `DISABLED_SLOW_*` case: it builds an
//! 8192 x 128 random data set with values in `[100, 200)`, computes the
//! brute-force 100-NN ground truth, then measures the top-10 recall of one
//! quantizer (flat or HNSW, LVQ / RaBitQ / LSG) and prints it. The reference's
//! `EXPECT_GE(recall, 0.9)` lines are commented out, so it only reports; the
//! same 0.9 bound is asserted here to make `cargo test -- --ignored` useful.
//!
//! The reference seeds the data with `std::random_device`; a fixed seed is
//! used here so a run is reproducible. These cases are ignored by default
//! because building the ground truth is a brute-force scan of 100 x 8192
//! vectors.

use std::cmp::Ordering;

use crate::lsg::LsgConfig;
use crate::lvq::LvqStore;
use crate::rabitq::RabitqStore;
use crate::vector::{l2_sq, HnswEncoding, HnswIndex, Metric};

const BASE_NUM: usize = 8192;
const DIM: usize = 128;
const QUERY_NUM: usize = 100;
const GT_DIM: usize = 100;
const RECALL_AT: usize = 10;
const TOP_K: usize = 10;
const M: usize = 16;
const EF_CONSTRUCTION: usize = 200;
const EF_SEARCH: usize = 200;

struct Dataset {
    base: Vec<Vec<f32>>,
    query: Vec<Vec<f32>>,
    /// `gt[i][0..RECALL_AT]` are the true top-10 ids of query `i`.
    ground_truth: Vec<Vec<usize>>,
}

/// xorshift64* in `[0, 1)`, matching the crate's other test generators.
fn next_unit(state: &mut u64) -> f32 {
    let mut x = *state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *state = x;
    ((x >> 11) as f64 / (1u64 << 53) as f64) as f32
}

fn dataset() -> Dataset {
    let mut state = 0x5EED_1234_9ABC_DEF1u64;
    let vector = |state: &mut u64| -> Vec<f32> {
        (0..DIM).map(|_| 100.0 + 100.0 * next_unit(state)).collect()
    };
    let base: Vec<Vec<f32>> = (0..BASE_NUM).map(|_| vector(&mut state)).collect();
    let query: Vec<Vec<f32>> = (0..QUERY_NUM).map(|_| vector(&mut state)).collect();
    let ground_truth = query
        .iter()
        .map(|q| top_k(BASE_NUM, GT_DIM, |i| l2_sq(q, &base[i])))
        .collect();
    Dataset {
        base,
        query,
        ground_truth,
    }
}

/// The `k` smallest ids by `distance`, ties broken by the smaller id — the
/// same selection `MaxHeap<f32, i32>` performs in the reference.
fn top_k<F>(n: usize, k: usize, distance: F) -> Vec<usize>
where
    F: Fn(usize) -> f32,
{
    let mut all: Vec<(usize, f32)> = (0..n).map(|i| (i, distance(i))).collect();
    all.sort_by(|a, b| {
        a.1.partial_cmp(&b.1)
            .unwrap_or(Ordering::Equal)
            .then(a.0.cmp(&b.0))
    });
    all.truncate(k);
    all.into_iter().map(|(i, _)| i).collect()
}

fn count_hits(ids: &[usize], ground_truth: &[usize]) -> usize {
    let gt = &ground_truth[..RECALL_AT];
    ids.iter().filter(|id| gt.contains(id)).count()
}

fn report(name: &str, hits: usize) -> f32 {
    let recall = hits as f32 / (QUERY_NUM * RECALL_AT) as f32;
    println!("{name}: Recall_10@1 = {recall}");
    recall
}

fn recall_of_flat<Q, P, D>(data: &Dataset, name: &str, prepare: P, distance: D) -> f32
where
    P: Fn(&[f32]) -> Q,
    D: Fn(&Q, usize) -> f32,
{
    let mut hits = 0;
    for (i, query) in data.query.iter().enumerate() {
        let q = prepare(query);
        let ids = top_k(BASE_NUM, TOP_K, |j| distance(&q, j));
        hits += count_hits(&ids, &data.ground_truth[i]);
    }
    report(name, hits)
}

fn recall_of_index(data: &Dataset, name: &str, index: &HnswIndex, ef: usize) -> f32 {
    let mut hits = 0;
    for (i, query) in data.query.iter().enumerate() {
        let ids: Vec<usize> = index
            .search_with_ef(query, TOP_K, ef)
            .into_iter()
            .map(|(id, _)| id)
            .collect();
        hits += count_hits(&ids, &data.ground_truth[i]);
    }
    report(name, hits)
}

fn build_hnsw(data: &Dataset, encoding: HnswEncoding, lsg: Option<LsgConfig>) -> HnswIndex {
    let mut index =
        HnswIndex::with_encoding(DIM, M, EF_CONSTRUCTION, EF_SEARCH, Metric::L2, encoding);
    index.train_store(&data.base, lsg);
    for v in &data.base {
        index.add(v);
    }
    index
}

/// C++: `QuantizerPerformanceTest.DISABLED_SLOW_flat_lvq`.
#[test]
#[ignore = "C++ DISABLED_SLOW_flat_lvq (run with --ignored)"]
fn disabled_slow_flat_lvq() {
    let data = dataset();
    let mut store = LvqStore::new(DIM, crate::lvq::Metric::L2, false);
    store.build(&data.base);
    let recall = recall_of_flat(
        &data,
        "flat_lvq",
        |query| store.make_query(query),
        |q, j| store.distance(q, j).unwrap_or(f32::MAX),
    );
    assert!(recall >= 0.9, "flat_lvq recall {recall}");
}

/// C++: `QuantizerPerformanceTest.DISABLED_SLOW_flat_rabitq`.
#[test]
#[ignore = "C++ DISABLED_SLOW_flat_rabitq (run with --ignored)"]
fn disabled_slow_flat_rabitq() {
    let data = dataset();
    let mut store = RabitqStore::new(DIM, crate::rabitq::Metric::L2, 0x5EED);
    store.build(&data.base);
    let recall = recall_of_flat(
        &data,
        "flat_rabitq",
        |query| store.make_query(query),
        |q, j| store.distance(q, j).unwrap_or(f32::MAX),
    );
    assert!(recall >= 0.9, "flat_rabitq recall {recall}");
}

/// C++: `QuantizerPerformanceTest.DISABLED_SLOW_hnsw_lvq`. The reference
/// inserts without `optimize_`, so the LVQ store keeps its zero mean.
#[test]
#[ignore = "C++ DISABLED_SLOW_hnsw_lvq (run with --ignored)"]
fn disabled_slow_hnsw_lvq() {
    let data = dataset();
    let mut index = HnswIndex::with_encoding(
        DIM,
        M,
        EF_CONSTRUCTION,
        EF_SEARCH,
        Metric::L2,
        HnswEncoding::Lvq,
    );
    for v in &data.base {
        index.add(v);
    }
    let recall = recall_of_index(&data, "hnsw_lvq", &index, EF_SEARCH);
    assert!(recall >= 0.9, "hnsw_lvq recall {recall}");
}

/// C++: `QuantizerPerformanceTest.DISABLED_SLOW_hnsw_rabitq` (inserted with
/// `optimize_`, which trains the rotated centroid first).
#[test]
#[ignore = "C++ DISABLED_SLOW_hnsw_rabitq (run with --ignored)"]
fn disabled_slow_hnsw_rabitq() {
    let data = dataset();
    let index = build_hnsw(&data, HnswEncoding::Rabitq, None);
    let recall = recall_of_index(&data, "hnsw_rabitq", &index, EF_SEARCH);
    assert!(recall >= 0.9, "hnsw_rabitq recall {recall}");
}

/// C++: `QuantizerPerformanceTest.DISABLED_SLOW_hnsw_lsg_rabitq` (RaBitQ
/// store scaled by LSG: `sample_ratio=0.1`, `ls_k=10`, `alpha=1.0`).
#[test]
#[ignore = "C++ DISABLED_SLOW_hnsw_lsg_rabitq (run with --ignored)"]
fn disabled_slow_hnsw_lsg_rabitq() {
    let data = dataset();
    let lsg = LsgConfig {
        sample_ratio: 0.1,
        ls_k: 10,
        alpha: 1.0,
    };
    let index = build_hnsw(&data, HnswEncoding::Rabitq, Some(lsg));
    let recall = recall_of_index(&data, "hnsw_lsg_rabitq", &index, EF_SEARCH);
    assert!(recall >= 0.9, "hnsw_lsg_rabitq recall {recall}");
}

/// C++: `QuantizerPerformanceTest.DISABLED_SLOW_compress_hnsw_to_rabitq`:
/// search the plain graph at `ef=200`, then re-encode it to RaBitQ and search
/// at `ef=400`.
#[test]
#[ignore = "C++ DISABLED_SLOW_compress_hnsw_to_rabitq (run with --ignored)"]
fn disabled_slow_compress_hnsw_to_rabitq() {
    let data = dataset();
    let mut index = build_hnsw(&data, HnswEncoding::Plain, None);
    let plain = recall_of_index(&data, "hnsw_plain", &index, EF_SEARCH);
    index.compress_to_rabitq();
    assert_eq!(index.encoding(), HnswEncoding::Rabitq);
    let compressed = recall_of_index(&data, "hnsw_to_rabitq", &index, EF_SEARCH * 2);
    assert!(plain >= 0.9, "plain recall {plain}");
    assert!(compressed >= 0.9, "compressed recall {compressed}");
}

/// C++: `QuantizerPerformanceTest.DISABLED_SLOW_compress_lsg_to_rabitq`.
#[test]
#[ignore = "C++ DISABLED_SLOW_compress_lsg_to_rabitq (run with --ignored)"]
fn disabled_slow_compress_lsg_to_rabitq() {
    let data = dataset();
    let lsg = LsgConfig {
        sample_ratio: 0.1,
        ls_k: 10,
        alpha: 1.0,
    };
    let mut index = build_hnsw(&data, HnswEncoding::Plain, Some(lsg));
    let plain = recall_of_index(&data, "hnsw_lsg", &index, EF_SEARCH);
    index.compress_to_rabitq();
    assert_eq!(index.encoding(), HnswEncoding::Rabitq);
    assert!(index.lsg().is_none(), "LSG is discarded by compression");
    let compressed = recall_of_index(&data, "lsg_to_rabitq", &index, EF_SEARCH * 2);
    assert!(plain >= 0.9, "plain LSG recall {plain}");
    assert!(compressed >= 0.9, "compressed recall {compressed}");
}
