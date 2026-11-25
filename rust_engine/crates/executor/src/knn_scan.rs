// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
    /// `WITH (recall_target = …)`: §11.3 item 5's budget-shaped interface. The
    /// target overrides the index's recall monitor for this statement.
    pub recall_target: Option<f32>,
    /// `WITH (latency_budget_ms = …)`: the latency half of the same interface.
    /// Only ever used to *narrow* a search whose recall is already above target.
    pub latency_budget_ms: Option<f32>,
    /// `WITH (beam = …)`: §11.3 item 5's hard override of the search width. It
    /// beats the recall monitor, which is what makes a bench able to hold the
    /// width fixed while it sweeps.
    pub beam: Option<usize>,
    /// `WITH (rerank_multiplier = …)`: Cockroach's session knob
    /// (`vector_search_rerank_multiplier`, 0..=100). It scales how many
    /// candidates the 1-bit ordering hands to the exact rescoring without
    /// touching the search width, which is the cheap half of the recall trade.
    pub rerank_multiplier: Option<usize>,
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
            // Out-of-range values are dropped rather than clamped: a
            // `recall_target` outside (0, 1] is a mistake, and silently
            // searching at 100% recall would hide it. The range check lives in
            // `RecallMonitor::set_target`.
            recall_target: get("recall_target").and_then(|v| v.parse::<f32>().ok()),
            latency_budget_ms: get("latency_budget_ms").and_then(|v| v.parse::<f32>().ok()),
            beam: get("beam")
                .and_then(|v| v.parse::<usize>().ok())
                .filter(|b| *b > 0),
            // Cockroach rejects the same range (0..=100) instead of clamping.
            rerank_multiplier: get("rerank_multiplier")
                .and_then(|v| v.parse::<usize>().ok())
                .filter(|m| *m <= 100),
        }
    }
}

use std::collections::{HashMap, HashSet};

use expression::Value;

use crate::Engine;

/// The candidate phase of a KNN scan: probe the IVF/HNSW indexes and return
/// the row ids to score exactly.
///
/// C++: the per-segment search inside `PhysicalKnnScan` that feeds `MergeKnn`
/// (`src/executor/operator/physical_scan/physical_knn_scan_impl.cpp`).
pub(crate) struct KnnCandidateScan<'a> {
    pub table: &'a str,
    /// The catalog index id in use. The memory and chunk registries are keyed
    /// by it, not by the column: a column may carry several indexes.
    pub index_id: Option<u64>,
    pub query: &'a [f32],
    pub rows: &'a [(usize, Vec<Value>)],
    /// The live row ids, when the caller has them: the coverage check needs the
    /// row set, not the rows, so an indexed query never decodes the table.
    pub live_rows: Option<&'a [usize]>,
    /// How many live rows the table has, when that is known without counting
    /// them. The query path passes the published counter, so an indexed query
    /// can prove the graph covers the table without decoding it.
    pub live_count: Option<usize>,
    pub topn: usize,
    pub metric: &'a str,
    pub options: &'a KnnScanOptions,
    /// The rows the statement's predicates admit, when they could be evaluated
    /// column-at-a-time. `None` means "no push-down": either the query has no
    /// filter, or its shape is not columnar-evaluable, and then a filtered
    /// query must not use the index at all (its candidates could all be
    /// rejected, leaving a short top-k).
    pub allowed: Option<&'a common::RowMask>,
    /// The search width the recall monitor wants (§11.3 item 4). `None` keeps
    /// the index's own `base_beam`.
    pub beam: Option<usize>,
}

/// What the index probe answers with.
///
/// C++: the per-segment state `PhysicalKnnScan` keeps between `PlanWithIndex`
/// and `MergeKnn`. The reference never has to say "scan the whole table":
/// every segment either answers or is skipped, and a row no segment holds is
/// scored by the block scan. This type is that contract - the candidates plus
/// the live rows no segment covers (`docs/SPANN_DESIGN.md` §9.7).
pub(crate) struct KnnProbe {
    /// Rows the index says can win; the caller scores them exactly.
    pub candidates: HashSet<usize>,
    /// Live rows the index does not hold. They are scored exactly as well, so a
    /// partly covered table costs its uncovered rows instead of a decode of
    /// every row - and they may still be in the append buffer, so the caller
    /// reads them by id (block *and* buffer), not through a block pass.
    pub uncovered: Vec<usize>,
    /// Pre-scored `(row_id, sim, dist)` hits when an `encode=plain` HNSW
    /// already computed exact `f32` distances and no rows are uncovered.
    pub scored: Option<Vec<(usize, f64, f64)>>,
}

impl KnnProbe {
    /// Candidates and uncovered rows in one set: every row the exact rescoring
    /// has to look at.
    pub(crate) fn rows(&self) -> HashSet<usize> {
        let mut out = self.candidates.clone();
        out.extend(self.uncovered.iter().copied());
        out
    }
}

/// The index's answer, or `None` when it has nothing to say.
///
/// `None` sends the statement to the exact scan. An index that names no row at
/// all - a segment whose publication is in flight, a graph whose build holds the
/// lock, a probe that landed between two dumps - must not be read as "the table
/// has no matches". A search that answers nothing for a table that has rows is
/// the one failure a caller cannot notice or repair, so the empty answer is
/// handed back as "the index cannot help" and the rows are scored directly.
fn answered(candidates: HashSet<usize>, uncovered: Vec<usize>) -> Option<KnnProbe> {
    answered_scored(candidates, uncovered, None)
}

fn answered_scored(
    candidates: HashSet<usize>,
    uncovered: Vec<usize>,
    scored: Option<Vec<(usize, f64, f64)>>,
) -> Option<KnnProbe> {
    if candidates.is_empty() && uncovered.is_empty() && scored.as_ref().is_none_or(Vec::is_empty) {
        return None;
    }
    Some(KnnProbe {
        candidates,
        uncovered,
        scored,
    })
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
    if let Some(count) = q.live_count {
        return labelled == count;
    }
    labelled == q.rows.len()
}

/// [`all_covered`] over an explicit offset set (the IVF probe's form).
fn covers_set(q: &KnnCandidateScan<'_>, covered: &HashSet<usize>) -> bool {
    if let Some(live) = q.live_rows {
        return live.iter().all(|seq| covered.contains(seq));
    }
    // Counting only: the caller's `rows` are still empty at this point (an
    // indexed query decodes its candidates afterwards), so comparing against
    // `q.rows` said "covered" for every table - an incomplete probe answered a
    // short top-k instead of falling back to the exact scan.
    if let Some(count) = q.live_count {
        return covered.len() >= count;
    }
    q.rows.iter().all(|(seq, _)| covered.contains(seq))
}

impl Engine {
    /// The beam a statement runs with when it does not name one (M4's pacer in its
    /// static form): a fraction of the tree's leaves, clamped.
    ///
    /// Measured on 1536d clustered data: 1452 leaves holds recall 1.000 at a width
    /// of 32, and 2856 leaves needs 64 - `leaves / 45` reproduces both. The cap
    /// stops a large index from turning one query into a scan of a sizeable slice of
    /// the table, and a statement's own beam always wins.
    fn search_width(explicit: Option<usize>, partitions: usize) -> usize {
        /// Past this the query is better served by a wider rerank pool than by more
        /// partitions.
        const MAX_SEARCH_BEAM: usize = 256;
        if let Some(beam) = explicit {
            return beam.max(1);
        }
        let leaves = (partitions / 2).max(1);
        (leaves / 45).clamp(crate::spann_mem::DEFAULT_SEARCH_BEAM, MAX_SEARCH_BEAM)
    }

    /// The live rows `covered` does not name.
    ///
    /// The ids come from the block versions - the same walk `count(*)` does -
    /// and never from a decode, so a partly covered index costs a version pass
    /// where it used to cost the whole table.
    fn uncovered_rows(&self, q: &KnnCandidateScan<'_>, covered: &HashSet<usize>) -> Vec<usize> {
        if let Some(live) = q.live_rows {
            return live
                .iter()
                .copied()
                .filter(|seq| !covered.contains(seq))
                .collect();
        }
        let mut out: Vec<usize> = self
            .live_row_ids(q.table)
            .into_iter()
            .filter(|seq| !covered.contains(seq))
            .collect();
        out.sort_unstable();
        out
    }

    /// The candidate budget for one probe: `topn * 4` (at least 64), widened by
    /// the filter's selectivity (`§12.5` step 5).
    ///
    /// The reference does not widen and simply returns a short top-k under a
    /// 1%-selective filter; a `k`-sized candidate list cannot fill `k` results
    /// after the mask rejects most of its entries, so the list is grown by the
    /// inverse selectivity, bounded by the table.
    fn candidate_budget(&self, q: &KnnCandidateScan<'_>) -> usize {
        let base = (q.topn * 4).max(64);
        let Some(allowed) = q.allowed else {
            return base;
        };
        // `RowMask::len()` is the admitted count, and `None` for "everything"
        // (no universe to subtract from), which has no selectivity to widen by.
        let Some(admitted) = allowed.len() else {
            return base;
        };
        let len = q
            .live_count
            .or_else(|| q.live_rows.map(<[usize]>::len))
            .unwrap_or(0)
            .max(admitted as usize)
            .max(1);
        let selectivity = (admitted as f64 / len as f64).max(1e-3);
        let widened = ((base as f64) / selectivity).ceil() as usize;
        // A candidate list longer than the table is pure work: no index can
        // return more rows than it holds.
        let cap = q
            .live_count
            .or_else(|| q.live_rows.map(<[usize]>::len))
            .unwrap_or(usize::MAX);
        widened.min(cap).max(base)
    }

    pub(crate) async fn knn_candidates(&self, q: &KnnCandidateScan<'_>) -> Option<KnnProbe> {
        let tkey = self.table_key(q.table);
        // IVF: probe `nprobe` centroids of every sealed chunk (C++
        // `IVF_Index_Storage::SearchIndex`). Candidates are scored exactly
        // below, so ranking matches an exhaustive scan whenever the probe
        // covers the true top-k.
        let index_id = q.index_id?;
        // spann: one tree per index id, held in memory (`TableEntry::spann`).
        // The tree already speaks row ids, and the caller re-scores every
        // candidate exactly, so the only failure mode is *coverage*: rows
        // written after the tree was built are not in it and the exact scan
        // has to run for those rows alone (`docs/SPANN_DESIGN.md` §9.7).
        let mut spann = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .and_then(|e| e.spann.get(&index_id).cloned())
        };
        if spann.is_none() {
            // After a restart the tree only exists in its chunk, and this is the
            // first place that knows somebody wants it.
            spann = self.spann_lazy_load(q.table, index_id);
        }
        if let Some(index) = spann {
            if index.dim() != q.query.len() {
                return None;
            }
            // §8.6: a 1-bit code orders distances to ~1%, so the index hands
            // over a rerank pool sized by the reference's formula (the beam and
            // the multiplier both widen it) and the exact rescoring picks the
            // winner. The pool is an I/O budget: the caller streams it.
            //
            // `nprobe` is the IVF spelling of the same knob and the SDK sends it
            // to every index type, so it is honoured as the width when the
            // A statement may name a beam (`beam` or `nprobe`); otherwise the
            // width follows the tree's size (`search_width`), not a constant. A
            // constant is what made recall fall off as the table grew: the
            // budget is a *fraction* of the leaves, so it has to grow with them.
            let width = Self::search_width(
                q.beam.or(q.options.nprobe.filter(|n| *n > 0)),
                index.shape().0,
            );
            let multiplier = q
                .options
                .rerank_multiplier
                .unwrap_or(crate::spann_mem::DEFAULT_RERANK_MULTIPLIER);
            let budget = crate::spann_mem::candidate_budget(q.topn, width, multiplier);
            // How many rows the tree names, and how many of the table's rows are
            // live. The tail is looked up first because it contributes to both.
            let tail = {
                let tables = self.tables.lock().unwrap();
                tables
                    .get(&tkey)
                    .and_then(|e| e.spann_mem.get(&index_id))
                    .cloned()
            };
            let mem_len = tail.as_ref().map(|t| t.len()).unwrap_or(0);
            let members = index.len().saturating_add(mem_len);
            let live = q.live_count.or_else(|| q.live_rows.map(<[usize]>::len));
            // The members are **slots** (`Rows::Chunk` keeps the slot of a
            // deleted row, `blocks::vector_column`), and a candidate that lands
            // on a tombstone decodes to nothing at rerank time. With most of the
            // segment deleted, a beam and a pool sized for `topn` probe too few of
            // the leaves that still hold a live row: the answer comes up short
            // although live rows exist. Both are widened by the share of members
            // that is still live - 1:1 for an undamaged tree - and both are work
            // rather than rows, so the beam stops at the tree's leaf count and the
            // pool at the I/O ceiling.
            let (width, budget) = match live {
                Some(live) if live > 0 && members > live => {
                    let widen = |value: usize, ceiling: usize| {
                        let scaled = ((value as u128) * (members as u128)).div_ceil(live as u128);
                        (scaled as usize).clamp(value, ceiling.max(value))
                    };
                    (
                        widen(width, index.shape().0),
                        widen(
                            budget,
                            crate::spann_mem::MAX_RERANK_CANDIDATES.saturating_mul(4),
                        ),
                    )
                }
                _ => (width, budget),
            };
            let hits = match q.allowed {
                Some(allowed) => {
                    index
                        .search_filtered(q.query, budget, Some(width), allowed)
                        .await
                }
                None => index.search(q.query, budget, Some(width)).await,
            };
            // §16.4 telemetry: what this query actually cost. `hits.len() <
            // budget` means the beam ran out of partitions before the pool
            // filled - the search stopped early instead of being cut off by the
            // budget.
            self.spann_stats(index_id).record(hits.len(), width, budget);
            let mut candidates: HashSet<usize> =
                hits.into_iter().map(|(id, _)| id as usize).collect();
            // The tail (`tail` above): rows inserted since the last dump, held as
            // a live tree. They are candidates (the exact rescoring decides) and
            // they are covered, so an insert stays visible without turning the
            // query into a scan of the whole table (`docs/SPANN_DESIGN.md` §9.2) -
            // a bounded descent, not a re-score of every pending row (§0.0.3).
            //
            // Every row here is re-scored, which is why the list is bounded
            // (`max_partition_size × 64` triggers a dump) and why a delete prunes
            // it. It is memory-only: a restart rebuilds it only for the rows the
            // WAL replay re-buffers, so rows that were already in a block file
            // fall into `uncovered` below - an exact scan of exactly those rows,
            // which is correct and costs the gap, not the table.
            if let Some(tail) = &tail {
                let tail_hits = match q.allowed {
                    Some(allowed) => tail.search_filtered(q.query, budget, None, allowed),
                    None => tail.search(q.query, budget, None),
                };
                candidates.extend(tail_hits.into_iter().map(|(id, _)| id as usize));
            }
            // The counts agree, so every member is a live row and the tree
            // covers the table. The shortcut is worth keeping: asking which rows
            // the tree holds means `row_ids()`, which walks every reachable
            // leaf's slot page - tens of milliseconds at 200k rows and ~1 s at
            // 2M - and this is the state of a table nobody deletes from.
            if all_covered(q, members) {
                return answered(candidates, Vec::new());
            }
            // The counts disagree: the tree's own row list says exactly which
            // rows it holds, and the rest are scored by the exact scan.
            let mut covered: HashSet<usize> =
                index.row_ids().into_iter().map(|id| id as usize).collect();
            if let Some(tail) = &tail {
                covered.extend(tail.row_ids().into_iter().map(|id| id as usize));
            }
            let uncovered = self.uncovered_rows(q, &covered);
            return answered(candidates, uncovered);
        }
        let mut ivf = {
            let tables = self.tables.lock().unwrap();
            tables.get(&tkey).and_then(|e| {
                (e.ivf.contains_key(&index_id) || e.ivf_mem.contains_key(&index_id)).then(|| {
                    (
                        e.ivf.get(&index_id).cloned().unwrap_or_default(),
                        e.ivf_mem.get(&index_id).cloned().unwrap_or_default(),
                    )
                })
            })
        };
        if ivf.as_ref().is_none_or(|(segs, _)| segs.is_empty()) {
            let has_loaded_hnsw = {
                let tables = self.tables.lock().unwrap();
                tables
                    .get(&tkey)
                    .and_then(|e| e.vector.get(&index_id))
                    .is_some_and(|v| !v.is_empty())
            };
            if !has_loaded_hnsw {
                if let Ok(indexes) = self.storage.catalog.indexes_of_table(q.table) {
                    if let Some(im) = indexes.into_iter().find(|im| im.id == index_id) {
                        if im.index_type.eq_ignore_ascii_case("IVF")
                            || im.index_type.eq_ignore_ascii_case("HNSW")
                        {
                            self.reload_vector_index_for(&im);
                            if im.index_type.eq_ignore_ascii_case("IVF") {
                                let tables = self.tables.lock().unwrap();
                                ivf = tables.get(&tkey).and_then(|e| {
                                    (e.ivf.contains_key(&index_id)
                                        || e.ivf_mem.contains_key(&index_id))
                                    .then(|| {
                                        (
                                            e.ivf.get(&index_id).cloned().unwrap_or_default(),
                                            e.ivf_mem.get(&index_id).cloned().unwrap_or_default(),
                                        )
                                    })
                                });
                            }
                        }
                    }
                }
            }
        }
        if let Some((segments, mem)) = ivf {
            if segments.is_empty() && mem.is_empty() {
                return None;
            }
            let nprobe = q.options.nprobe.unwrap_or(1);
            let budget = self.candidate_budget(q);
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
                // The mask goes **into** the part scan (`IVF_Filter` in
                // `ivf_index_search.cppm`): filtering the truncated list here
                // instead left `topn * 4` slots filled with deleted rows, so a
                // mostly-deleted segment answered an empty top-k.
                for offset in seg.candidates_filtered(q.query, budget, nprobe, q.metric, q.allowed)
                {
                    candidates.insert(offset as usize);
                }
            }
            // Rows written after the mirror are not covered; only those are
            // read by the exact scan, not the table.
            if covers_set(q, &covered) {
                return answered(candidates, Vec::new());
            }
            let uncovered = self.uncovered_rows(q, &covered);
            return answered(candidates, uncovered);
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
                .and_then(|e| e.vector.get(&index_id).cloned())
                .unwrap_or_default()
        };
        // Every sealed segment is its own graph; the query searches each
        // and merges the results through the reference's `MergeKnn`
        // (`src/storage/knn_index/merge_knn.cppm`). The union of their
        // labels must cover the live q.rows, else the exact scan runs.
        // The in-memory segment: rows written after the last dump. The
        // reference searches every segment and merges the results.
        // Every **segment's** memory graph, not one per index: the reference
        // keys `MemIndex` by `(index_id, segment)`, and a query merges what
        // every segment answers (`MergeKnn`).
        let mems: Vec<std::sync::Arc<tokio::sync::Mutex<crate::hnsw_mem::HnswIndexInMem>>> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| {
                e.hnsw_mem
                    .iter()
                    .filter(|((id, _), _)| *id == index_id)
                    .map(|(_, m)| std::sync::Arc::clone(m))
                    .collect()
            })
            .unwrap_or_default();
        // No graph at all: this column is answered by the exact scan. Returning
        // `Some(empty)` here would be read as "the index covers nothing", and
        // the caller (which defers decoding the rows) would then score none of
        // them - that is what made every KNN query on a non-float embedding
        // column return no rows.
        if segments.is_empty() && mems.is_empty() {
            return None;
        }
        let mut labelled = 0usize;
        // `KnnSearchOption.ef_`: the candidate list size for the bottom
        // layer (`WITH (ef = ...)`).
        let search_option = index::hnsw_handler::KnnSearchOption { ef: q.options.ef };
        // `encode=plain` HNSW already computes exact `f32` distances inside the
        // graph, so it does not need a `topn * 4` rerank pool (which would
        // also silently inflate `ef.max(k)` above the user's `WITH (ef = ...)`).
        // Pre-scored hits are valid only when every graph is plain AND its
        // metric matches the query metric.
        let query_metric = index::vector::Metric::parse(q.metric);
        let mut can_prescore = segments.iter().all(|seg| {
            let idx = seg.index.index();
            idx.encoding() == index::vector::HnswEncoding::Plain && idx.metric() == query_metric
        });
        for mem in &mems {
            if let Ok(mem) = mem.try_lock() {
                let idx = mem.index();
                can_prescore &= idx.encoding() == index::vector::HnswEncoding::Plain
                    && idx.metric() == query_metric;
            }
        }
        let k = if can_prescore {
            q.topn.max(1)
        } else {
            self.candidate_budget(q)
        };
        let plain_scored = |hits: &[(f32, u64)]| -> Vec<(usize, f64, f64)> {
            hits.iter()
                .map(|&(d, row)| {
                    let df = f64::from(d);
                    let (sim, dist) = match query_metric {
                        index::vector::Metric::Ip => (-df, df),
                        index::vector::Metric::Cos => {
                            let c = -df;
                            (c, 1.0 - c)
                        }
                        index::vector::Metric::L2 => (1.0 / (1.0 + df), df),
                    };
                    (row as usize, sim, dist)
                })
                .collect()
        };
        // Fast path for a single sealed plain segment with no in-memory tail:
        // skip `MergeKnn` and intermediate `hits` allocation.
        if segments.len() == 1 && mems.is_empty() && can_prescore {
            let seg = &segments[0];
            if seg.index.dims() != q.query.len() {
                return None;
            }
            labelled = seg.labels.len();
            let seg_k = k.min(seg.labels.len().max(1));
            let raw: Vec<(usize, f32)> = match q.allowed {
                Some(allowed) => {
                    seg.index
                        .search_index_filtered(q.query, seg_k, search_option, &|node| {
                            seg.labels
                                .get(node)
                                .is_some_and(|label| allowed.contains(u64::from(*label)))
                        })
                }
                None => seg.index.search_index(q.query, seg_k, search_option),
            };
            let candidates: std::collections::HashSet<usize> = raw
                .iter()
                .map(|&(node, _)| seg.labels[node] as usize)
                .collect();
            if all_covered(q, labelled) {
                let scored: Vec<(usize, f64, f64)> = raw
                    .iter()
                    .map(|&(node, d)| {
                        let df = f64::from(d);
                        let (sim, dist) = match query_metric {
                            index::vector::Metric::Ip => (-df, df),
                            index::vector::Metric::Cos => {
                                let c = -df;
                                (c, 1.0 - c)
                            }
                            index::vector::Metric::L2 => (1.0 / (1.0 + df), df),
                        };
                        (seg.labels[node] as usize, sim, dist)
                    })
                    .collect();
                return answered_scored(candidates, Vec::new(), Some(scored));
            }
            let mut covered: HashSet<usize> = HashSet::with_capacity(labelled);
            covered.extend(seg.labels.iter().map(|label| *label as usize));
            let uncovered = self.uncovered_rows(q, &covered);
            return answered(candidates, uncovered);
        }
        let mut merge = index::merge_knn::MergeKnn::new(1, k, index::merge_knn::Compare::Max, None);
        merge.begin();
        for seg in &segments {
            if seg.index.dims() != q.query.len() {
                return None;
            }
            labelled += seg.labels.len();
            let seg_k = k.min(seg.labels.len().max(1));
            // The filter is pushed into the traversal (`hnsw_alg.cppm:133-148`):
            // a vertex the mask rejects is still walked through and only kept
            // out of the result heap, so the graph stays connected.
            let raw: Vec<(usize, f32)> = match q.allowed {
                Some(allowed) => {
                    seg.index
                        .search_index_filtered(q.query, seg_k, search_option, &|node| {
                            seg.labels
                                .get(node)
                                .is_some_and(|label| allowed.contains(u64::from(*label)))
                        })
                }
                None => seg.index.search_index(q.query, seg_k, search_option),
            };
            let hits: Vec<(f32, u64)> = raw
                .into_iter()
                .map(|(node, dist)| (dist, seg.labels[node] as u64))
                .collect();
            merge.search(0, &hits);
        }
        for mem in &mems {
            // `try_lock`: the query path is synchronous. A graph the build is
            // filling is skipped, which is what an empty one would report.
            if let Ok(mem) = mem.try_lock() {
                if mem.dims != q.query.len() {
                    return None;
                }
                let idx = mem.index();
                can_prescore &= idx.encoding() == index::vector::HnswEncoding::Plain
                    && idx.metric() == query_metric;
                // Coverage is what the segment *holds*, not what it returned: a
                // `topn * 4` result set can never cover a table, so adding only
                // the hits made every indexed query declare itself incomplete and
                // fall back to the exact scan.
                labelled += mem.rows().len();
                let hits = match q.allowed {
                    Some(allowed) => {
                        mem.search_filtered_with_ef(q.query, k, q.options.ef, &|row| {
                            allowed.contains(row)
                        })
                    }
                    None => mem.search_with_ef(q.query, k, q.options.ef),
                };
                for (row, dist) in hits {
                    merge.add_result(0, dist, row);
                }
            }
        }
        merge.end();
        let candidates: std::collections::HashSet<usize> = merge
            .hits_by_idx(0)
            .iter()
            .map(|&(_, r)| r as usize)
            .collect();
        if all_covered(q, labelled) {
            let scored = can_prescore.then(|| plain_scored(merge.hits_by_idx(0)));
            return answered_scored(candidates, Vec::new(), scored);
        }
        // The counts disagree, so which rows are missing has to be answered with
        // the labels themselves: a delete leaves its row in the graph (the
        // segments stay append-only until compaction), which makes `labelled`
        // exceed the live count for a graph that still covers every live row.
        let mut covered: HashSet<usize> = HashSet::with_capacity(labelled);
        for seg in &segments {
            covered.extend(seg.labels.iter().map(|label| *label as usize));
        }
        for mem in &mems {
            if let Ok(mem) = mem.try_lock() {
                covered.extend(mem.rows().iter().map(|row| *row as usize));
            }
        }
        let uncovered = self.uncovered_rows(q, &covered);
        answered(candidates, uncovered)
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
    pub(crate) async fn execute(
        &self,
        // `+ Sync`: the execute future is awaited on a runtime worker and
        // boxed as `Send`, so every borrow it captures has to be `Sync`.
        keep: &(dyn Fn(&[Value]) -> bool + Sync),
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

        Ok(KnnScanOperatorState {
            scored,
            use_sim: self.use_sim(),
        })
    }

    /// C++ `MergeKnn`/result-handler finish: rank in the scan's direction,
    /// apply `threshold`, keep `topn`.
    ///
    /// `select_nth_unstable_by` rather than a full sort: the exact scan scores
    /// every live row, so `scored` holds a million entries for a top-100 answer
    /// and sorting all of them was most of a brute-force query's time (SIFT 1M:
    /// 947 ms per query, `sort_by` at the top of the profile). Partitioning at
    /// `topn` is O(n), and only the kept prefix is sorted.
    ///
    /// The comparator is the one this function always used - `(score, row id)`
    /// with the score direction taken from the metric - so the answer is
    /// identical to the full-sort version, including the order of ties.
    /// [`Self::finish`] without the final sort: the top-n, unordered.
    ///
    /// A block-at-a-time scan reduces its running set once per block, and only
    /// the last reduction has to be ordered - the caller's final `finish` does
    /// that. Sorting every block's set (123 blocks on SIFT 1M) was wasted work
    /// on a monotone key.
    ///
    /// The exact scan now feeds [`Self::topn_collector`] instead, which drops
    /// the per-block selection entirely. This stays as the reference the
    /// collector is tested against.
    #[cfg(test)]
    pub(crate) fn keep_topn(&self, mut scored: Vec<(usize, f64, f64)>) -> Vec<(usize, f64, f64)> {
        if self.threshold.is_some() {
            let is_dist =
                !self.is_sparse && !self.is_tensor && matches!(self.metric, "l2" | "hamming");
            let use_sim = self.is_sparse || self.is_tensor || self.is_knn_min_heap();
            scored.retain(|e| match self.threshold {
                Some(th) if is_dist => e.2 <= th,
                Some(th) if use_sim => e.1 >= th,
                Some(th) => e.2 >= th,
                None => true,
            });
        }
        if scored.len() > self.topn {
            let use_sim = self.is_sparse || self.is_tensor || self.is_knn_min_heap();
            let better = |a: &(usize, f64, f64), b: &(usize, f64, f64)| -> std::cmp::Ordering {
                let (x, y) = if use_sim { (b.1, a.1) } else { (a.2, b.2) };
                x.partial_cmp(&y)
                    .unwrap_or(std::cmp::Ordering::Equal)
                    .then(a.0.cmp(&b.0))
            };
            scored.select_nth_unstable_by(self.topn - 1, better);
            scored.truncate(self.topn);
        }
        scored
    }

    /// A running top-n across every block of a scan.
    ///
    /// The block-at-a-time exact scan used to call [`Self::keep_topn`] once per
    /// block, which ran a full `select_nth_unstable_by` over that block's 8192
    /// candidates - 123 of those on SIFT 1M, and `perf` put 16% of the query
    /// there. A scan visits each row exactly once, so the running set only has
    /// to answer "is this row better than the current n-th?", which one
    /// comparison against a cached cutoff does.
    ///
    /// `cutoff` is `None` until the set holds `topn` rows; after that it is the
    /// score of the current n-th best, and a row that cannot beat it is dropped
    /// without touching the set. Ties keep the smaller row id, the same rule
    /// [`Self::keep_topn`] uses, so the answer is unchanged.
    pub(crate) fn topn_collector(&self) -> TopN {
        TopN::new(
            self.topn,
            self.use_sim(),
            self.threshold,
            self.metric.to_string(),
        )
    }

    pub(crate) fn finish(&self, state: KnnScanOperatorState) -> KnnScanOperatorState {
        let use_sim = state.use_sim;
        // For a distance metric the threshold is an upper bound; for a
        // similarity metric it is a lower bound.
        let is_dist = !self.is_sparse && !self.is_tensor && matches!(self.metric, "l2" | "hamming");
        let passes = |sim: f64, dist: f64| -> bool {
            match self.threshold {
                None => true,
                Some(th) if is_dist => dist <= th,
                Some(th) if use_sim => sim >= th,
                Some(th) => dist >= th,
            }
        };
        // `a` sorts before `b` when `a` is the better answer.
        let better = |a: &(usize, f64, f64), b: &(usize, f64, f64)| -> std::cmp::Ordering {
            let (x, y) = if use_sim { (b.1, a.1) } else { (a.2, b.2) };
            x.partial_cmp(&y)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        };
        let mut scored: Vec<(usize, f64, f64)> = if self.threshold.is_some() {
            state
                .scored
                .into_iter()
                .filter(|e| passes(e.1, e.2))
                .collect()
        } else {
            state.scored
        };
        if scored.len() > self.topn {
            // Partition so the first `topn` are the best ones (unordered).
            scored.select_nth_unstable_by(self.topn - 1, better);
            scored.truncate(self.topn);
        }
        scored.sort_by(better);
        KnnScanOperatorState { scored, use_sim }
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
            vector_candidates: None,
        }
    }

    /// C++ `PhysicalKnnScan::Execute` finish: similarity metrics rank
    /// descending, distance metrics ascending; equal scores keep the lower
    /// row id; `threshold` and `topn` apply after ordering.
    #[test]
    fn finishing_batches_then_merging_equals_finishing_everything() {
        // The streamed rerank (statements.rs) scores the candidate pool one
        // batch at a time and finishes each batch to its own top-n. This is the
        // property that makes it exact: the global top-n is the top-n of the
        // union of the per-batch top-ns, for any monotone ranking key.
        let scored: Vec<(usize, f64, f64)> = (0..40)
            .map(|i| (i, 1.0 / (i as f64 + 1.0), i as f64))
            .collect();
        let scan = scan("l2", None);

        let all = scan.finish(KnnScanOperatorState {
            scored: scored.clone(),
            use_sim: false,
        });

        let mut merged: Vec<(usize, f64, f64)> = Vec::new();
        for batch in scored.chunks(7) {
            let part = scan.finish(KnnScanOperatorState {
                scored: batch.to_vec(),
                use_sim: false,
            });
            merged.extend(part.scored);
        }
        let merged = scan.finish(KnnScanOperatorState {
            scored: merged,
            use_sim: false,
        });
        assert_eq!(merged.scored, all.scored);
    }

    /// The running collector and the per-block `keep_topn` must agree: it
    /// replaced the latter on the exact scan's hot path, and a difference would
    /// silently change which rows a query returns.
    #[test]
    fn the_running_collector_matches_keep_topn() {
        for (metric, threshold) in [
            ("l2", None),
            ("ip", None),
            ("cosine", None),
            ("l2", Some(2.0)),
            ("ip", Some(0.5)),
        ] {
            let scan = scan(metric, threshold);
            // Ties on both the score and the id, so the tie-break rule is
            // exercised too.
            let scored: Vec<(usize, f64, f64)> = (0..50)
                .map(|i| {
                    let sim = (i % 5) as f64 / 5.0;
                    (i, sim, 1.0 - sim)
                })
                .collect();

            let mut expected = scan.keep_topn(scored.clone());
            let mut got = {
                let mut top = scan.topn_collector();
                for row in scored.iter().copied() {
                    top.push(row);
                }
                top.into_rows()
            };
            // Both are unordered; the caller's `finish` is what orders them.
            expected.sort_unstable_by_key(|r| r.0);
            got.sort_unstable_by_key(|r| r.0);
            assert_eq!(got, expected, "metric={metric} threshold={threshold:?}");
        }
    }

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

#[cfg(test)]
mod beam_tests {
    use super::Engine;

    #[test]
    fn the_beam_follows_the_tree_and_never_undercuts_a_statement() {
        // A small tree keeps the floor.
        assert_eq!(Engine::search_width(None, 12), 32);
        // 1452 leaves (2904 partitions) held recall 1.000 at 32.
        assert_eq!(Engine::search_width(None, 2904), 32);
        // 2856 leaves needed 64.
        assert!(Engine::search_width(None, 5712) >= 63);
        // A huge tree is capped rather than scanning a slice of the table.
        assert_eq!(Engine::search_width(None, 4_000_000), 256);
        // A statement's own beam always wins, and never becomes zero.
        assert_eq!(Engine::search_width(Some(8), 4_000_000), 8);
        assert_eq!(Engine::search_width(Some(0), 100), 1);
    }
}

/// A bounded top-n set, fed one scored row at a time.
///
/// See [`PhysicalKnnScan::topn_collector`]: the scan compares each row against
/// the set's current cutoff instead of re-selecting per block.
pub(crate) struct TopN {
    topn: usize,
    /// Higher is better: the score is a similarity, so the worst kept row has
    /// the smallest score. Otherwise the score is a distance.
    use_sim: bool,
    threshold: Option<f64>,
    metric: String,
    rows: Vec<(usize, f64, f64)>,
    /// The score a row has to beat to enter the full set, cached so the common
    /// reject costs one comparison.
    cutoff: Option<f64>,
    /// Index of the row that currently holds `cutoff`.
    worst: usize,
}

impl TopN {
    fn new(topn: usize, use_sim: bool, threshold: Option<f64>, metric: String) -> Self {
        Self {
            topn: topn.max(1),
            use_sim,
            threshold,
            metric,
            rows: Vec::new(),
            cutoff: None,
            worst: 0,
        }
    }

    /// Whether `row` would enter a full set: `true` while the set still has
    /// room, then a comparison against the cached cutoff.
    #[inline]
    fn admits(&self, row: &(usize, f64, f64)) -> bool {
        if self.rows.len() < self.topn {
            return true;
        }
        let Some(cutoff) = self.cutoff else {
            return true;
        };
        // Compare exactly as `keep_topn` orders rows: score first, then the
        // row id, so a tie on the score keeps the smaller id. A similarity
        // improves upward, a distance downward.
        let mine = if self.use_sim { row.1 } else { row.2 };
        let better = if self.use_sim {
            mine.partial_cmp(&cutoff)
        } else {
            cutoff.partial_cmp(&mine)
        };
        match better {
            Some(std::cmp::Ordering::Greater) => true,
            Some(std::cmp::Ordering::Equal) => row.0 < self.rows[self.worst].0,
            _ => false,
        }
    }

    /// Offers one scored row. Cheap when the set is full and the row loses.
    pub(crate) fn push(&mut self, row: (usize, f64, f64)) {
        if self.threshold.is_some() && !self.passes_threshold(&row) {
            return;
        }
        if !self.admits(&row) {
            return;
        }
        if self.rows.len() < self.topn {
            self.rows.push(row);
            if self.rows.len() == self.topn {
                self.refresh();
            }
            return;
        }
        self.rows[self.worst] = row;
        self.refresh();
    }

    fn passes_threshold(&self, row: &(usize, f64, f64)) -> bool {
        let Some(th) = self.threshold else {
            return true;
        };
        let is_dist = !matches!(self.metric.as_str(), "cosine" | "ip");
        if is_dist && !self.use_sim {
            row.2 <= th
        } else if self.use_sim {
            row.1 >= th
        } else {
            row.2 >= th
        }
    }

    /// Finds the worst kept row and caches its score as the cutoff. Only the
    /// set (`topn` rows) is scanned, never a whole block.
    fn refresh(&mut self) {
        let mut worst = 0;
        for i in 1..self.rows.len() {
            if self.is_worse(&self.rows[i], &self.rows[worst]) {
                worst = i;
            }
        }
        self.worst = worst;
        self.cutoff = Some(if self.use_sim {
            self.rows[worst].1
        } else {
            self.rows[worst].2
        });
    }

    /// `a` is worse than `b` for this scan's direction: a similarity improves
    /// upward, a distance downward. Equal scores are broken by the row id, the
    /// larger id being worse.
    fn is_worse(&self, a: &(usize, f64, f64), b: &(usize, f64, f64)) -> bool {
        // "a loses to b" is the same comparison `admits` makes against the
        // cutoff, so both use one ordering rule.
        let (a_score, b_score) = if self.use_sim { (a.1, b.1) } else { (a.2, b.2) };
        let a_better = if self.use_sim {
            a_score.partial_cmp(&b_score)
        } else {
            b_score.partial_cmp(&a_score)
        };
        match a_better {
            Some(std::cmp::Ordering::Less) => true,
            Some(std::cmp::Ordering::Equal) => a.0 > b.0,
            _ => false,
        }
    }

    /// The kept rows, unordered. The caller's `finish` sorts them.
    pub(crate) fn into_rows(self) -> Vec<(usize, f64, f64)> {
        self.rows
    }
}
