//! The parsed options of one KNN scan.
//!
//! C++: `KnnScanSharedData` (`src/function/table/knn_scan_data.cppm`) and
//! `KnnSearchOption` (`src/storage/knn_index/knn_hnsw/hnsw_common.cppm`). The
//! scan loop itself lives in the executor's row pipeline; this type owns the
//! option surface it consumes.

/// `nprobe` (IVF), `ef` (HNSW) and `threshold` (distance/score cut-off).
#[derive(Clone, Debug, Default, PartialEq)]
pub(crate) struct KnnScanOptions {
    pub nprobe: Option<usize>,
    pub ef: Option<usize>,
    pub threshold: Option<f64>,
}

/// Looks up `name` in a `k=v;k=v` option string, case-insensitively.
fn option(opts: &str, name: &str) -> Option<String> {
    opts.split(';').find_map(|p| {
        let (k, v) = p.split_once('=')?;
        k.trim()
            .eq_ignore_ascii_case(name)
            .then(|| v.trim().to_string())
    })
}

impl KnnScanOptions {
    /// `with_opts` is the SQL `WITH (…)` clause; `params` is the legacy
    /// `'topn=…;threshold=…'` string the SDK sends. The former wins.
    pub fn parse(with_opts: Option<&str>, params: &str) -> Self {
        let get = |name: &str| -> Option<String> {
            with_opts
                .and_then(|s| option(s, name))
                .or_else(|| option(params, name))
        };
        Self {
            nprobe: get("nprobe").and_then(|v| v.parse().ok()),
            ef: get("ef").and_then(|v| v.parse().ok()),
            threshold: get("threshold").and_then(|v| v.parse().ok()),
        }
    }
}

use std::collections::HashSet;

use infinity_expression::Value;

use crate::{Engine, SharedVectorCache};

/// The candidate phase of a KNN scan: probe the IVF/HNSW indexes and return
/// the row ids to score exactly.
///
/// C++: the per-segment search inside `PhysicalKnnScan` that feeds `MergeKnn`
/// (`src/executor/operator/physical_scan/physical_knn_scan_impl.cpp`).
pub(crate) struct KnnCandidateScan<'a> {
    pub table: &'a str,
    pub column: &'a str,
    pub query: &'a [f32],
    pub rows: &'a [(usize, Vec<Value>)],
    pub cache: Option<&'a SharedVectorCache>,
    pub topn: usize,
    pub metric: &'a str,
    pub options: &'a KnnScanOptions,
}

impl Engine {
    pub(crate) fn knn_candidates(&self, q: &KnnCandidateScan<'_>) -> Option<HashSet<usize>> {
        // IVF: probe `nprobe` centroids of every sealed chunk (C++
        // `IVF_Index_Storage::SearchIndex`). Candidates are scored exactly
        // below, so ranking matches an exhaustive scan whenever the probe
        // covers the true top-k.
        if let Some(segments) = self
            .ivf_indexes
            .lock()
            .unwrap()
            .get(&super::index_key(q.table, q.column))
            .cloned()
        {
            let mem: Vec<(u64, Vec<f32>)> = self
                .ivf_mem
                .lock()
                .unwrap()
                .get(&super::index_key(q.table, q.column))
                .cloned()
                .unwrap_or_default();
            if segments.is_empty() && mem.is_empty() {
                return None;
            }
            let nprobe = q.options.nprobe.unwrap_or(1);
            let mut covered: std::collections::HashSet<usize> = Default::default();
            let mut candidates: std::collections::HashSet<usize> = Default::default();
            // The memory segment: q.rows written since the last mirror dump.
            for (seq, _) in &mem {
                covered.insert(*seq as usize);
                candidates.insert(*seq as usize);
            }
            for seg in &segments {
                for part in &seg.parts {
                    for (offset, _) in part {
                        covered.insert(*offset as usize);
                    }
                }
                for offset in seg.candidates(q.query, (q.topn * 4).max(64), nprobe, q.metric) {
                    candidates.insert(offset as usize);
                }
            }
            // Rows written after the mirror are not covered; fall back to
            // the exact scan rather than returning an incomplete top-k.
            let all_covered = if let Some(c) = &q.cache {
                c.read()
                    .map(|g| g.live.iter().all(|s| covered.contains(s)))
                    .unwrap_or(false)
            } else {
                q.rows.iter().all(|(seq, _)| covered.contains(seq))
            };
            if !all_covered {
                return None;
            }
            return Some(candidates);
        }
        if q.metric != "l2" {
            return None;
        }
        let segments = self
            .vector_indexes
            .lock()
            .unwrap()
            .get(&super::index_key(q.table, q.column))
            .cloned()?;
        if segments.is_empty() {
            return None;
        }
        // Every sealed segment is its own graph; the query searches each
        // and merges the results through the reference's `MergeKnn`
        // (`src/storage/knn_index/merge_knn.cppm`). The union of their
        // labels must cover the live q.rows, else the exact scan runs.
        let mut covered: std::collections::HashSet<usize> = Default::default();
        // `KnnSearchOption.ef_`: the candidate list size for the bottom
        // layer (`WITH (ef = ...)`).
        let search_option = infinity_index::hnsw_handler::KnnSearchOption { ef: q.options.ef };
        let k = (q.topn * 4).max(64);
        let mut merge = infinity_index::merge_knn::MergeKnn::new(
            1,
            k,
            infinity_index::merge_knn::Compare::Max,
            None,
        );
        merge.begin();
        for seg in &segments {
            if seg.index.dims() != q.query.len() {
                return None;
            }
            covered.extend(seg.labels.iter().map(|s| *s as usize));
            let seg_k = k.min(seg.labels.len().max(1));
            let hits: Vec<(f32, u64)> = seg
                .index
                .search_index(q.query, seg_k, search_option)
                .into_iter()
                .map(|(node, dist)| (dist, seg.labels[node] as u64))
                .collect();
            merge.search(0, &hits);
        }
        // The in-memory segment: q.rows written after the last dump. The
        // reference searches every segment and merges the results.
        if let Some(mem) = self
            .hnsw_mem
            .lock()
            .unwrap()
            .get(&super::index_key(q.table, q.column))
        {
            for (row, dist) in mem.search(q.query, k) {
                merge.add_result(0, dist, row);
                covered.insert(row as usize);
            }
        }
        merge.end();
        let candidates: std::collections::HashSet<usize> = merge
            .ids_by_idx(0)
            .into_iter()
            .map(|r| r as usize)
            .collect();
        let uncovered = if let Some(c) = &q.cache {
            c.read()
                .map(|g| g.live.iter().any(|s| !covered.contains(s)))
                .unwrap_or(true)
        } else {
            q.rows.iter().any(|(seq, _)| !covered.contains(seq))
        };
        if uncovered {
            return None;
        }
        Some(candidates)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parses_both_option_forms() {
        let o = KnnScanOptions::parse(Some("nprobe = 16; ef=64"), "topn=3;threshold=0.85");
        assert_eq!(o.nprobe, Some(16));
        assert_eq!(o.ef, Some(64));
        assert_eq!(o.threshold, Some(0.85));
        // WITH wins over the legacy string.
        let o = KnnScanOptions::parse(Some("threshold=1.5"), "threshold=0.5");
        assert_eq!(o.threshold, Some(1.5));
        assert_eq!(KnnScanOptions::parse(None, ""), KnnScanOptions::default());
    }
}
