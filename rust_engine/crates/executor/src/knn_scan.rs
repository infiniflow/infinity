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

/// Looks up `name` in an option string, case-insensitively.
///
/// Two spellings reach this: the legacy SDK string `k=v;k=v` and the SQL
/// `WITH (k=v, k=v)` clause, which the server also forwards to `with_opts`.
/// Splitting on both separators is what lets `threshold=…` survive the second.
fn option(opts: &str, name: &str) -> Option<String> {
    opts.split([';', ',']).find_map(|p| {
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

use std::collections::{HashMap, HashSet};

use expression::Value;

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
    /// The live row ids, when the caller has them: the coverage check needs the
    /// row set, not the rows, so an indexed query never decodes the table.
    pub live_rows: Option<&'a [usize]>,
    pub cache: Option<&'a SharedVectorCache>,
    pub topn: usize,
    pub metric: &'a str,
    pub options: &'a KnnScanOptions,
}

/// Whether the index covers every live row.
///
/// The HNSW path asks this as a *count*: labels are disjoint across segments
/// and each names one row, so "as many labels as live rows" is the same
/// statement as "every live row is labelled", at no set cost. Building a
/// 20k-entry set per query was 240-360 us of the ~600 us an indexed query
/// spent on the server. The IVF path knows its offsets only as a set, so it
/// keeps the identity test.
fn all_covered(q: &KnnCandidateScan<'_>, labelled: usize) -> bool {
    if let Some(live) = q.live_rows {
        return labelled == live.len();
    }
    if let Some(c) = &q.cache {
        return c.read().map(|g| labelled == g.live.len()).unwrap_or(false);
    }
    labelled == q.rows.len()
}

/// [`all_covered`] over an explicit offset set (the IVF probe's form).
fn covers_set(q: &KnnCandidateScan<'_>, covered: &HashSet<usize>) -> bool {
    if let Some(live) = q.live_rows {
        return live.iter().all(|seq| covered.contains(seq));
    }
    if let Some(c) = &q.cache {
        return c
            .read()
            .map(|g| g.live.iter().all(|s| covered.contains(s)))
            .unwrap_or(false);
    }
    q.rows.iter().all(|(seq, _)| covered.contains(seq))
}

impl Engine {
    pub(crate) fn knn_candidates(&self, q: &KnnCandidateScan<'_>) -> Option<HashSet<usize>> {
        let tkey = self.table_key(q.table);
        // IVF: probe `nprobe` centroids of every sealed chunk (C++
        // `IVF_Index_Storage::SearchIndex`). Candidates are scored exactly
        // below, so ranking matches an exhaustive scan whenever the probe
        // covers the true top-k.
        let ivf = {
            let tables = self.tables.lock().unwrap();
            tables.get(&tkey).and_then(|e| {
                (e.ivf.contains_key(q.column) || e.ivf_mem.contains_key(q.column)).then(|| {
                    (
                        e.ivf.get(q.column).cloned().unwrap_or_default(),
                        e.ivf_mem.get(q.column).cloned().unwrap_or_default(),
                    )
                })
            })
        };
        if let Some((segments, mem)) = ivf {
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
                for offset in seg.offsets() {
                    covered.insert(offset as usize);
                }
                for offset in seg.candidates(q.query, (q.topn * 4).max(64), nprobe, q.metric) {
                    candidates.insert(offset as usize);
                }
            }
            // Rows written after the mirror are not covered; fall back to
            // the exact scan rather than returning an incomplete top-k.
            let all_covered = covers_set(q, &covered);
            if !all_covered {
                return None;
            }
            return Some(candidates);
        }
        // Sealed segments. `CREATE INDEX` builds the graph into the memory
        // segment (`hnsw_mem`) and a checkpoint seals it into a chunk, so a
        // fresh index has no sealed segment at all - an empty list here must not
        // skip the memory segment below, which is what made every query after
        // `CREATE INDEX` fall back to a full table scan.
        let segments = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .and_then(|e| e.vector.get(q.column).cloned())
                .unwrap_or_default()
        };
        // Every sealed segment is its own graph; the query searches each
        // and merges the results through the reference's `MergeKnn`
        // (`src/storage/knn_index/merge_knn.cppm`). The union of their
        // labels must cover the live q.rows, else the exact scan runs.
        // The in-memory segment: rows written after the last dump. The
        // reference searches every segment and merges the results.
        let mem = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .and_then(|e| e.hnsw_mem.get(q.column).cloned());
        // No graph at all: this column is answered by the exact scan. Returning
        // `Some(empty)` here would be read as "the index covers nothing", and
        // the caller (which defers decoding the rows) would then score none of
        // them - that is what made every KNN query on a non-float embedding
        // column return no rows.
        if segments.is_empty() && mem.is_none() {
            return None;
        }
        let mut labelled = 0usize;
        // `KnnSearchOption.ef_`: the candidate list size for the bottom
        // layer (`WITH (ef = ...)`).
        let search_option = index::hnsw_handler::KnnSearchOption { ef: q.options.ef };
        let k = (q.topn * 4).max(64);
        let mut merge = index::merge_knn::MergeKnn::new(1, k, index::merge_knn::Compare::Max, None);
        merge.begin();
        for seg in &segments {
            if seg.index.dims() != q.query.len() {
                return None;
            }
            labelled += seg.labels.len();
            let seg_k = k.min(seg.labels.len().max(1));
            let hits: Vec<(f32, u64)> = seg
                .index
                .search_index(q.query, seg_k, search_option)
                .into_iter()
                .map(|(node, dist)| (dist, seg.labels[node] as u64))
                .collect();
            merge.search(0, &hits);
        }
        if let Some(mem) = mem {
            if let Ok(mem) = mem.lock() {
                // Coverage is what the segment *holds*, not what it returned: a
                // `topn * 4` result set can never cover a table, so adding only
                // the hits made every indexed query declare itself incomplete and
                // fall back to the exact scan.
                labelled += mem.rows().len();
                for (row, dist) in mem.search_with_ef(q.query, k, q.options.ef) {
                    merge.add_result(0, dist, row);
                }
            }
        }
        merge.end();
        let candidates: std::collections::HashSet<usize> = merge
            .ids_by_idx(0)
            .into_iter()
            .map(|r| r as usize)
            .collect();
        let uncovered = !all_covered(q, labelled);
        if uncovered {
            return None;
        }
        Some(candidates)
    }
}

/// The physical KNN scan: exact scoring and ranking of the scanned rows.
///
/// C++: `PhysicalKnnScan` (`src/executor/operator/physical_scan/physical_knn_scan.cppm`)
/// with `Execute` → `ExecuteInternalByColumnLogicalType` →
/// `ExecuteInternalByColumnDataTypeAndQueryDataType`
/// (`physical_knn_scan_impl.cpp`). The index probe is [`KnnCandidateScan`];
/// this type owns the reference's scoring/output phase.
pub(crate) struct PhysicalKnnScan<'a> {
    /// C++ `GetColumnID()`: the vector column's position in the row.
    pub column_index: usize,
    /// C++ `column_logical_type_`/`column_elem_type_`: the upper-case column
    /// declaration (e.g. `EMBEDDING(FLOAT,3)`).
    pub column_type: &'a str,
    /// `SEARCH MATCH SPARSE`.
    pub is_sparse: bool,
    /// `SEARCH MATCH TENSOR`.
    pub is_tensor: bool,
    /// A `bit` sparse column scores as an integer.
    pub is_bit_sparse: bool,
    /// C++ `real_knn_query_embedding_*`: the query as written.
    pub query_items: &'a [String],
    /// The query's declared element type (`MATCH TENSOR`'s second argument).
    pub query_elem_type: Option<&'a str>,
    pub topn: usize,
    /// `l2` | `cosine` | `ip` | `hamming`.
    pub metric: &'a str,
    pub threshold: Option<f64>,
    /// The decoded rows (`KnnScanSharedData`'s blocks).
    pub rows: &'a [(usize, Vec<Value>)],
    /// Pre-decoded rows used instead of `rows` on the cache path.
    pub cache: Option<&'a SharedVectorCache>,
    /// `PlanWithIndex()` result: rows the sealed index says can win.
    pub vector_candidates: Option<HashSet<usize>>,
}

/// C++ `KnnScanOperatorState`: the scored hits of one scan.
pub(crate) struct KnnScanOperatorState {
    /// `(row_id, similarity, distance)`.
    pub scored: Vec<(usize, f64, f64)>,
    /// Similarity metrics rank descending (`CompareMin`).
    pub use_sim: bool,
}

impl PhysicalKnnScan<'_> {
    /// C++ `IsKnnMinHeap()`: cosine/inner product rank by similarity.
    pub(crate) fn is_knn_min_heap(&self) -> bool {
        matches!(self.metric, "cosine" | "ip")
    }

    fn use_sim(&self) -> bool {
        self.is_sparse || self.is_tensor || self.is_knn_min_heap()
    }

    /// C++ `Execute()`: score every scanned row (or cache entry) and return
    /// the operator state, before sorting/thresholding.
    pub(crate) fn execute(
        &self,
        keep: &dyn Fn(&[Value]) -> bool,
        bmp_scores: &HashMap<usize, f64>,
        mv_scores: &HashMap<usize, f64>,
    ) -> crate::Result<KnnScanOperatorState> {
        let mut scored: Vec<(usize, f64, f64)> = Vec::new();
        for (seq, row) in self.rows {
            // The WHERE/filter predicates prune candidates before ranking.
            if !keep(row) {
                continue;
            }
            if let Some(cands) = &self.vector_candidates {
                if !cands.contains(seq) {
                    continue;
                }
            }
            // Sparse: score with the BMP block-max index (the reference's
            // `bmp_alg.cppm:SearchKnn`) instead of a per-row dot product.
            let bmp_dot = if self.is_sparse && !self.is_bit_sparse {
                bmp_scores.get(seq).copied()
            } else {
                None
            };
            if let Some(dot) = bmp_dot {
                scored.push((*seq, dot, -dot));
                continue;
            }
            let Some(Value::Str(v)) = row.get(self.column_index) else {
                continue;
            };
            if self.is_tensor {
                if let Some(score) = mv_scores.get(seq) {
                    scored.push((*seq, *score, -*score));
                    continue;
                }
                let qtext = format!("[{}]", self.query_items.join(","));
                let qtext = match crate::type_dim(self.column_type) {
                    Some(d) => crate::reshape_tensor(&qtext, d),
                    None => qtext,
                };
                let q = crate::parse_tensor_leaves(&qtext);
                // The column's declared element type and the query's declared
                // element type both drive the rounding precision.
                let col_elem = crate::tensor_elem_type(self.column_type);
                let qry_elem = match self.query_elem_type {
                    None => crate::ElemType::Float32,
                    Some(s) => crate::elem_type_named(s).ok_or_else(|| {
                        crate::Error::Invalid(format!("invalid tensor data type '{s}'"))
                    })?,
                };
                let score = crate::tensor_score(
                    &q,
                    v,
                    col_elem,
                    qry_elem,
                    self.column_type.starts_with("TENSORARRAY"),
                );
                scored.push((*seq, score, -score));
            } else if self.is_sparse {
                // A `bit` sparse vector holds bare indices; its inner product
                // is the number of shared positions.
                let dot = if self.is_bit_sparse {
                    let qidx: Vec<i64> = self
                        .query_items
                        .iter()
                        .filter_map(|x| x.trim().parse::<i64>().ok())
                        .collect();
                    let ridx: Vec<i64> = v
                        .trim()
                        .trim_start_matches('[')
                        .trim_end_matches(']')
                        .split(',')
                        .filter_map(|x| x.trim().parse::<i64>().ok())
                        .collect();
                    qidx.iter().filter(|i| ridx.contains(i)).count() as f64
                } else {
                    let (qv, rv) = (
                        crate::parse_sparse(self.query_items),
                        crate::parse_sparse_str(v),
                    );
                    crate::sparse_dot(&qv, &rv)
                };
                scored.push((*seq, dot, -dot));
            } else {
                // A `bit` embedding column stores a bit string, so both the
                // query and the stored value are read as 0/1 positions.
                let bit_col = crate::is_bit_type(self.column_type);
                let mut qv: Vec<f64> = self
                    .query_items
                    .iter()
                    .filter_map(|x| x.parse::<f64>().ok())
                    .collect();
                // The SDK packs a bit query into bytes (LSB first); expand it
                // to one bit per position unless it is already un-packed.
                if bit_col {
                    let dim = crate::type_dim(self.column_type).unwrap_or(0);
                    let multi = self.column_type.starts_with("MULTIVECTOR");
                    // Un-packed bits already line up on `dim` boundaries;
                    // only a byte-packed query needs expanding.
                    if dim > 0
                        && !qv.len().is_multiple_of(dim)
                        && (qv.len() * 8).is_multiple_of(dim)
                    {
                        let mut bits = Vec::with_capacity(qv.len() * 8);
                        for b in &qv {
                            let byte = *b as u8;
                            for k in 0..8 {
                                bits.push(((byte >> k) & 1) as f64);
                            }
                        }
                        // A multivector query packs several `dim`-sized rows.
                        if !multi {
                            bits.truncate(dim);
                        }
                        qv = bits;
                    } else {
                        qv = qv
                            .into_iter()
                            .map(|x| if x != 0.0 { 1.0 } else { 0.0 })
                            .collect();
                    }
                }
                if qv.is_empty() {
                    continue;
                }
                // a MULTIVECTOR holds several vectors; the row scores by the
                // best-matching sub-vector
                let multivector = self.column_type.starts_with("MULTIVECTOR");
                if multivector {
                    // MAXSIM: split both sides into `dim`-sized rows and sum,
                    // over the query rows, the best similarity to a stored row.
                    let dim = crate::type_dim(self.column_type).unwrap_or(qv.len()).max(1);
                    // A bit multivector stores one bit string per row.
                    let doc_rows =
                        if crate::tensor_elem_type(self.column_type) == crate::ElemType::Bit {
                            crate::parse_bit_leaves(v)
                        } else {
                            crate::parse_tensor_leaves(v)
                        };
                    if doc_rows.is_empty() || doc_rows.iter().all(|r| r.is_empty()) {
                        continue;
                    }
                    let qrows: Vec<&[f64]> = qv.chunks(dim).collect();
                    let (mut total, mut total_dist) = (0.0f64, 0.0f64);
                    for qr in qrows {
                        let mut best: Option<(f64, f64)> = None;
                        for dr in &doc_rows {
                            if dr.len() != qr.len() {
                                continue;
                            }
                            let (sim, dist) = crate::vector_score(qr, dr, self.metric);
                            if best.map(|(b, _)| sim > b).unwrap_or(true) {
                                best = Some((sim, dist));
                            }
                        }
                        if let Some((sim, dist)) = best {
                            total += sim;
                            total_dist += dist;
                        }
                    }
                    // MAXSIM distance is the sum of the best per-row
                    // distances; the similarity is its `1/(1+d)` form.
                    scored.push((*seq, total, total_dist));
                    continue;
                }
                let rv: Vec<f64> = if bit_col {
                    crate::parse_bit_leaves(v).into_iter().flatten().collect()
                } else {
                    crate::parse_vec_str(v)
                };
                if rv.len() != qv.len() {
                    continue;
                }
                let (sim, dist) = crate::vector_score(&qv, &rv, self.metric);
                scored.push((*seq, sim, dist));
            }
        }
        self.score_cache(&mut scored);
        Ok(KnnScanOperatorState {
            scored,
            use_sim: self.use_sim(),
        })
    }

    /// The rows decoded once into the scalar row cache (the reference's
    /// in-memory `DataBlock`s): score them without re-reading the store.
    fn score_cache(&self, scored: &mut Vec<(usize, f64, f64)>) {
        let Some(shared) = self.cache else {
            return;
        };
        let qv: Vec<f64> = self
            .query_items
            .iter()
            .filter_map(|x| x.parse::<f64>().ok())
            .collect();
        let guard = shared.read().unwrap();
        if qv.is_empty() || qv.len() != guard.dims {
            return;
        }
        // An index already said which rows can win: walk the candidates, not the
        // cache. Iterating the table and testing membership is the same answer
        // for O(n) hash lookups instead of O(k), and on a 20k-row table that was
        // most of the query's time.
        if let Some(cands) = &self.vector_candidates {
            // One sorted walk, not a lookup per candidate: the cache is in id
            // order (`fetch_rows_with_ids_projected` returns sorted rows and the
            // update path only replaces values in place), so both sides can be
            // swept together. 64 binary searches over 20k rows was costing 1.27 ms
            // of the query - more than the HNSW search itself.
            let mut wanted: Vec<usize> = cands.iter().copied().collect();
            wanted.sort_unstable();
            let mut at = 0usize;
            for seq in wanted {
                while at < guard.rows.len() && guard.rows[at].0 < seq {
                    at += 1;
                }
                if at == guard.rows.len() {
                    break;
                }
                if guard.rows[at].0 != seq {
                    continue;
                }
                let v = &guard.rows[at].1;
                let (sim, dist) = crate::vector_score(&qv, v, self.metric);
                scored.push((seq, sim, dist));
            }
            return;
        }
        for (seq, v) in &guard.rows {
            if !guard.live.contains(seq) {
                continue;
            }
            let (sim, dist) = crate::vector_score(&qv, v, self.metric);
            scored.push((*seq, sim, dist));
        }
    }

    /// C++ `MergeKnn`/result-handler finish: sort in the scan's direction,
    /// apply `threshold`, keep `topn`.
    pub(crate) fn finish(&self, mut state: KnnScanOperatorState) -> KnnScanOperatorState {
        let use_sim = state.use_sim;
        state.scored.sort_by(|a, b| {
            let (x, y) = if use_sim { (b.1, a.1) } else { (a.2, b.2) };
            x.partial_cmp(&y)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        if let Some(th) = self.threshold {
            // For a distance metric the threshold is an upper bound; for a
            // similarity metric it is a lower bound.
            let is_dist =
                !self.is_sparse && !self.is_tensor && matches!(self.metric, "l2" | "hamming");
            state.scored.retain(|(_, sim, dist)| {
                if is_dist {
                    *dist <= th
                } else if use_sim {
                    *sim >= th
                } else {
                    *dist >= th
                }
            });
        }
        state.scored.truncate(self.topn);
        state
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

    fn scan(metric: &str, threshold: Option<f64>) -> PhysicalKnnScan<'_> {
        PhysicalKnnScan {
            column_index: 0,
            column_type: "EMBEDDING(FLOAT,2)",
            is_sparse: false,
            is_tensor: false,
            is_bit_sparse: false,
            query_items: &[],
            query_elem_type: None,
            topn: 2,
            metric,
            threshold,
            rows: &[],
            cache: None,
            vector_candidates: None,
        }
    }

    /// C++ `PhysicalKnnScan::Execute` finish: similarity metrics rank
    /// descending, distance metrics ascending; equal scores keep the lower
    /// row id; `threshold` and `topn` apply after ordering.
    #[test]
    fn finish_ranks_by_metric_and_applies_threshold_and_topn() {
        let state = KnnScanOperatorState {
            scored: vec![(3, 0.2, 0.8), (1, 0.9, 0.1), (2, 0.7, 0.3), (4, 0.9, 0.1)],
            use_sim: true,
        };
        let out = scan("cosine", Some(0.5)).finish(state);
        assert_eq!(out.scored, vec![(1, 0.9, 0.1), (4, 0.9, 0.1)]);

        let state = KnnScanOperatorState {
            scored: vec![(3, 0.2, 0.8), (1, 0.9, 0.1), (2, 0.7, 0.3), (4, 0.9, 0.1)],
            use_sim: false,
        };
        let out = scan("l2", Some(0.5)).finish(state);
        assert_eq!(out.scored, vec![(1, 0.9, 0.1), (4, 0.9, 0.1)]);
    }
}
