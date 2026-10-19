//! The reference's EMVB search stages over a trained residual-PQ index.
//!
//! [`EmvbView`] borrows the persisted arrays of one EMVB segment (coarse
//! centroids, centroid assignments, the residual product quantizer and the
//! per-document layout) and runs the same three-stage search as the reference:
//!
//!   1. `find_candidate_docs` — per query token, keep the best `nprobe`
//!      centroids above `threshold_first`; their documents are the candidates.
//!   2. `compute_hit_frequency` — rank candidates by how many query tokens
//!      their embeddings' centroids cover (`n_doc_to_score`).
//!   3. `second_stage_filtering` — approximate MaxSim with the centroid inner
//!      products only (`out_second_stage`).
//!   4. `compute_topk_documents_selected` — refine with the residual PQ code
//!      and keep the top-k.
//!
//! C++: `EMVBIndex::GetQueryResult` / `EMVBSearch` (`src/storage/knn_index/
//! emvb/emvb_search_impl.cpp`).

/// Search options; the defaults mirror `PLAID_*` in
/// `src/common/default_values.cppm:154`.
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct EmvbSearchOptions {
    pub centroid_nprobe: usize,
    pub threshold_first: f32,
    pub n_doc_to_score: usize,
    pub out_second_stage: usize,
    pub threshold_final: f32,
}

impl EmvbSearchOptions {
    /// `PLAID_N_IVF_PROBE = 8`, `PLAID_CENTROID_SCORE_THRESHOLD = -0.1`,
    /// `PLAID_N_DOC_TO_SCORE_FACTOR = 100`, `PLAID_N_FULL_SCORES_FACTOR = 10`.
    pub fn for_topk(topk: usize) -> Self {
        Self {
            centroid_nprobe: 8,
            threshold_first: -0.1,
            n_doc_to_score: topk.saturating_mul(100),
            out_second_stage: topk.saturating_mul(10),
            threshold_final: 0.0,
        }
    }
}

impl Default for EmvbSearchOptions {
    fn default() -> Self {
        Self::for_topk(1)
    }
}

/// A borrowed EMVB segment.
pub struct EmvbView<'a> {
    pub dims: usize,
    pub subspace_num: usize,
    pub sub_dim: usize,
    pub k: usize,
    /// Coarse centroids, `n_centroids * dims`.
    pub centroids: &'a [f32],
    /// Residual codebook, `subspace_num * k * sub_dim`.
    pub codebook: &'a [f32],
    /// One codeword per (embedding, subspace).
    pub codes: &'a [u8],
    /// Coarse centroid of every embedding.
    pub assignments: &'a [u32],
    pub doc_lens: &'a [u32],
    pub doc_segment_offsets: &'a [u32],
    /// Documents per coarse centroid (the reference's `centroids_to_docid_`).
    pub centroids_to_docid: &'a [Vec<u32>],
}

impl EmvbView<'_> {
    fn n_centroids(&self) -> usize {
        if self.dims == 0 {
            0
        } else {
            self.centroids.len() / self.dims
        }
    }

    fn doc_offsets(&self) -> Vec<u32> {
        let mut offsets = Vec::with_capacity(self.doc_lens.len());
        let mut at = 0u32;
        for len in self.doc_lens {
            offsets.push(at);
            at += *len;
        }
        offsets
    }

    /// C++: `EMVBSearch::GetQueryResult`.
    ///
    /// Returns `(segment offset, score)` best-first.
    pub fn search(
        &self,
        query: &[Vec<f32>],
        top_n: usize,
        options: EmvbSearchOptions,
    ) -> Vec<(u32, f32)> {
        let n_centroids = self.n_centroids();
        let n_docs = self.doc_lens.len();
        if top_n == 0 || n_docs == 0 || query.is_empty() || n_centroids == 0 {
            return Vec::new();
        }
        let dims = self.dims;
        let nqt = query.len();
        let doc_offsets = self.doc_offsets();

        // Stage 1a: `q_t · centroid_c` for every query token.
        let mut scores: Vec<Vec<f32>> = Vec::with_capacity(nqt);
        for q in query {
            let mut row = vec![f32::NEG_INFINITY; n_centroids];
            if q.len() == dims {
                for (c, score) in row.iter_mut().enumerate() {
                    let base = c * dims;
                    *score = q
                        .iter()
                        .zip(&self.centroids[base..base + dims])
                        .map(|(a, b)| a * b)
                        .sum();
                }
            }
            scores.push(row);
        }

        // Stage 1b: `find_candidate_docs`.
        let nprobe = options.centroid_nprobe.clamp(1, n_centroids);
        let words = nqt.div_ceil(64);
        let mut masks = vec![vec![0u64; words]; n_centroids];
        let mut selected: Vec<Vec<u32>> = Vec::with_capacity(nqt);
        for (qt, row) in scores.iter().enumerate() {
            let mut filtered: Vec<usize> = (0..n_centroids)
                .filter(|&c| row[c] >= options.threshold_first)
                .collect();
            if filtered.is_empty() {
                // No centroid passes the threshold: take the closest one.
                let mut argmax = 0usize;
                for c in 1..n_centroids {
                    if row[c] > row[argmax] {
                        argmax = c;
                    }
                }
                selected.push(vec![argmax as u32]);
            } else {
                if filtered.len() > nprobe {
                    filtered.sort_by(|&a, &b| {
                        row[b]
                            .partial_cmp(&row[a])
                            .unwrap_or(std::cmp::Ordering::Equal)
                            .then(a.cmp(&b))
                    });
                    filtered.truncate(nprobe);
                }
                selected.push(filtered.into_iter().map(|c| c as u32).collect());
            }
            for &c in selected.last().expect("just pushed") {
                masks[c as usize][qt / 64] |= 1u64 << (qt % 64);
            }
        }

        // Candidate documents: the union of the selected centroids' doc lists.
        let mut seen_centroid = vec![false; n_centroids];
        let mut seen_doc = vec![false; n_docs];
        let mut candidates: Vec<u32> = Vec::new();
        for qsel in &selected {
            for &c in qsel {
                let c = c as usize;
                if seen_centroid[c] {
                    continue;
                }
                seen_centroid[c] = true;
                if let Some(docs) = self.centroids_to_docid.get(c) {
                    for &doc in docs {
                        let doc = doc as usize;
                        if doc < n_docs && !seen_doc[doc] {
                            seen_doc[doc] = true;
                            candidates.push(doc as u32);
                        }
                    }
                }
            }
        }

        // Stage 2: `compute_hit_frequency`.
        let n_doc_to_score = options.n_doc_to_score.max(top_n);
        let scored_docs: Vec<u32> = if candidates.len() <= n_doc_to_score {
            candidates
        } else {
            let mut hits: Vec<(u32, u32)> = candidates
                .iter()
                .map(|&doc| {
                    (
                        self.query_token_hits(doc as usize, &doc_offsets, &masks),
                        doc,
                    )
                })
                .collect();
            hits.sort_by(|a, b| b.0.cmp(&a.0).then(a.1.cmp(&b.1)));
            hits.truncate(n_doc_to_score);
            hits.into_iter().map(|(_, doc)| doc).collect()
        };

        // Stage 3: `second_stage_filtering` — MaxSim over centroid scores.
        let out_second_stage = options.out_second_stage.max(top_n);
        let mut stage2: Vec<(f32, u32)> = scored_docs
            .iter()
            .map(|&doc| {
                (
                    self.centroid_maxsim(doc as usize, &doc_offsets, &scores),
                    doc,
                )
            })
            .collect();
        stage2.sort_by(|a, b| {
            b.0.partial_cmp(&a.0)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.1.cmp(&b.1))
        });
        stage2.truncate(out_second_stage);

        // Stage 4: `compute_topk_documents_selected` — residual-PQ refine.
        let table = self.ip_table(query);
        let mut refined: Vec<(f32, u32)> = stage2
            .iter()
            .map(|&(_, doc)| {
                (
                    self.residual_maxsim(doc as usize, &doc_offsets, &scores, &table, options),
                    doc,
                )
            })
            .collect();
        refined.sort_by(|a, b| {
            b.0.partial_cmp(&a.0)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.1.cmp(&b.1))
        });
        refined.truncate(top_n);
        refined
            .into_iter()
            .map(|(score, doc)| {
                (
                    self.doc_segment_offsets
                        .get(doc as usize)
                        .copied()
                        .unwrap_or(doc),
                    score,
                )
            })
            .collect()
    }

    /// Number of distinct query tokens covered by the centroids of a document's
    /// embeddings (`compute_hit_frequency`'s `mask.count()`).
    fn query_token_hits(&self, doc: usize, offsets: &[u32], masks: &[Vec<u64>]) -> u32 {
        let len = self.doc_lens[doc] as usize;
        let start = offsets[doc] as usize;
        let words = masks.first().map(Vec::len).unwrap_or(0);
        let mut mask = vec![0u64; words];
        for e in start..start + len {
            let Some(&c) = self.assignments.get(e) else {
                break;
            };
            if let Some(m) = masks.get(c as usize) {
                for (acc, w) in mask.iter_mut().zip(m) {
                    *acc |= *w;
                }
            }
        }
        mask.iter().map(|w| w.count_ones()).sum()
    }

    /// `second_stage_filtering`: `sum_qt max_j score[qt][centroid(embedding_j)]`.
    fn centroid_maxsim(&self, doc: usize, offsets: &[u32], scores: &[Vec<f32>]) -> f32 {
        let len = self.doc_lens[doc] as usize;
        let start = offsets[doc] as usize;
        let mut total = 0.0f32;
        for row in scores {
            let mut best = f32::NEG_INFINITY;
            for e in start..start + len {
                let Some(&c) = self.assignments.get(e) else {
                    break;
                };
                if let Some(&s) = row.get(c as usize) {
                    if s > best {
                        best = s;
                    }
                }
            }
            if best != f32::NEG_INFINITY {
                total += best;
            }
        }
        total
    }

    /// `EMVBProductQuantizer::GetIPDistanceTable`: `table[qt][sub][code]`.
    fn ip_table(&self, query: &[Vec<f32>]) -> Vec<Vec<Vec<f32>>> {
        let sn = self.subspace_num;
        let mut table = Vec::with_capacity(query.len());
        for q in query {
            let mut per_sub = Vec::with_capacity(sn);
            for sub in 0..sn {
                let lo = sub * self.sub_dim;
                let hi = (lo + self.sub_dim).min(self.dims);
                let mut row = vec![0.0f32; self.k];
                if lo < self.dims && q.len() == self.dims {
                    for (code, slot) in row.iter_mut().enumerate() {
                        let base = (sub * self.k + code) * self.sub_dim;
                        *slot = q[lo..hi]
                            .iter()
                            .zip(&self.codebook[base..base + (hi - lo)])
                            .map(|(a, b)| a * b)
                            .sum();
                    }
                }
                per_sub.push(row);
            }
            table.push(per_sub);
        }
        table
    }

    fn pq_ip(&self, table: &[Vec<Vec<f32>>], embedding: usize, qt: usize) -> f32 {
        let mut sum = 0.0f32;
        for sub in 0..self.subspace_num {
            let Some(&code) = self.codes.get(embedding * self.subspace_num + sub) else {
                break;
            };
            if let Some(&v) = table
                .get(qt)
                .and_then(|t| t.get(sub))
                .and_then(|r| r.get(code as usize))
            {
                sum += v;
            }
        }
        sum
    }

    /// `compute_topk_documents_selected`: per query token keep the best
    /// embedding score (centroid IP + residual PQ), then sum over tokens.
    fn residual_maxsim(
        &self,
        doc: usize,
        offsets: &[u32],
        scores: &[Vec<f32>],
        table: &[Vec<Vec<f32>>],
        options: EmvbSearchOptions,
    ) -> f32 {
        let len = self.doc_lens[doc] as usize;
        let start = offsets[doc] as usize;
        let mut total = 0.0f32;
        for (qt, row) in scores.iter().enumerate() {
            let mut eligible = Vec::new();
            let mut best = f32::NEG_INFINITY;
            for e in start..start + len {
                let Some(&c) = self.assignments.get(e) else {
                    break;
                };
                let centroid = row.get(c as usize).copied().unwrap_or(f32::NEG_INFINITY);
                if centroid >= options.threshold_final {
                    eligible.push((e, centroid));
                }
            }
            if eligible.is_empty() {
                // `filter_scores_output_ids` left nothing: exhaustive refine.
                for e in start..start + len {
                    let Some(&c) = self.assignments.get(e) else {
                        break;
                    };
                    let centroid = row.get(c as usize).copied().unwrap_or(0.0);
                    let s = centroid + self.pq_ip(table, e, qt);
                    if s > best {
                        best = s;
                    }
                }
            } else {
                for (e, centroid) in eligible {
                    let s = centroid + self.pq_ip(table, e, qt);
                    if s > best {
                        best = s;
                    }
                }
            }
            if best != f32::NEG_INFINITY {
                total += best;
            }
        }
        total
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// A hand-built segment: two coarse centroids, one subspace with two
    /// codewords, so every score can be checked by hand.
    struct Fixture {
        centroids: Vec<f32>,
        codebook: Vec<f32>,
        codes: Vec<u8>,
        assignments: Vec<u32>,
        doc_lens: Vec<u32>,
        doc_segment_offsets: Vec<u32>,
        centroids_to_docid: Vec<Vec<u32>>,
    }

    fn fixture() -> Fixture {
        // dims = 2, subspace_num = 1, sub_dim = 2, k = 2.
        let centroids = vec![1.0, 0.0, 0.0, 1.0];
        // Codeword 0 = (0,0); codeword 1 = (0.5, 0.5).
        let codebook = vec![0.0, 0.0, 0.5, 0.5];
        // doc 0: two embeddings, both near centroid 0, residual codes 1 and 0.
        // doc 1: one embedding near centroid 1, residual code 0.
        let codes = vec![1, 0, 0];
        let assignments = vec![0, 0, 1];
        let doc_lens = vec![2, 1];
        let doc_segment_offsets = vec![0, 1];
        let centroids_to_docid = vec![vec![0], vec![1]];
        Fixture {
            centroids,
            codebook,
            codes,
            assignments,
            doc_lens,
            doc_segment_offsets,
            centroids_to_docid,
        }
    }

    fn view<'a>(f: &'a Fixture) -> EmvbView<'a> {
        EmvbView {
            dims: 2,
            subspace_num: 1,
            sub_dim: 2,
            k: 2,
            centroids: &f.centroids,
            codebook: &f.codebook,
            codes: &f.codes,
            assignments: &f.assignments,
            doc_lens: &f.doc_lens,
            doc_segment_offsets: &f.doc_segment_offsets,
            centroids_to_docid: &f.centroids_to_docid,
        }
    }

    #[test]
    fn ranks_the_document_whose_centroid_matches_the_query() {
        let f = fixture();
        let got = view(&f).search(&[vec![1.0, 0.0]], 2, EmvbSearchOptions::for_topk(2));
        assert_eq!(got.len(), 2);
        // Doc 0's embedding 0 sits on centroid 0 with residual code 1, so its
        // score is `1.0 + 0.5 = 1.5`; doc 1 only reaches centroid 1 (0.0).
        assert_eq!(got[0].0, 0);
        assert!((got[0].1 - 1.5).abs() < 1e-6, "score = {}", got[0].1);
        assert_eq!(got[1].0, 1);
    }

    #[test]
    fn empty_query_and_zero_topk_return_nothing() {
        let f = fixture();
        let v = view(&f);
        assert!(v.search(&[], 5, EmvbSearchOptions::default()).is_empty());
        assert!(v
            .search(&[vec![1.0, 0.0]], 0, EmvbSearchOptions::default())
            .is_empty());
    }

    #[test]
    fn threshold_first_falls_back_to_the_closest_centroid() {
        let f = fixture();
        let opts = EmvbSearchOptions {
            threshold_first: 100.0,
            ..EmvbSearchOptions::for_topk(2)
        };
        let got = view(&f).search(&[vec![1.0, 0.0]], 2, opts);
        // Nothing passes the threshold, so the closest centroid is used.
        assert_eq!(got[0].0, 0);
    }

    /// C++: `EMVBTest.test_fakepq` (`test_emvb_ut.cpp`). A zero PQ distance
    /// table (`FakePQ`) reduces the final stage to the coarse-centroid MaxSim,
    /// so the ten documents of centroid 3 tie at `32 * 1.0` and come back in
    /// document order.
    #[test]
    fn fake_pq_ranks_the_matching_centroids_documents() {
        const DIM: usize = 128;
        const CENTROID_NUM: usize = 8;
        const DOCS_IN_ONE_CENTROID: usize = 10;
        const N_DOCS: usize = CENTROID_NUM * DOCS_IN_ONE_CENTROID;
        const EMBEDDING_IN_ONE_DOC: usize = 10;
        const QUERY_TOKEN_NUM: usize = 32;

        let doc_lens = vec![EMBEDDING_IN_ONE_DOC as u32; N_DOCS];
        // `doc_offsets` is implied by `doc_lens` in `EmvbView`.
        let mut assignments = vec![0u32; N_DOCS * EMBEDDING_IN_ONE_DOC];
        for doc in 0..N_DOCS {
            for emb in 0..EMBEDDING_IN_ONE_DOC {
                assignments[doc * EMBEDDING_IN_ONE_DOC + emb] =
                    (doc / DOCS_IN_ONE_CENTROID) as u32;
            }
        }
        let mut centroids = vec![0.0f32; CENTROID_NUM * DIM];
        for c in 0..CENTROID_NUM {
            centroids[c * DIM + c] = 1.0;
        }
        let centroids_to_docid: Vec<Vec<u32>> = (0..CENTROID_NUM)
            .map(|c| {
                (0..DOCS_IN_ONE_CENTROID)
                    .map(|j| (c * DOCS_IN_ONE_CENTROID + j) as u32)
                    .collect()
            })
            .collect();
        let doc_segment_offsets: Vec<u32> = (0..N_DOCS as u32).collect();
        // FakePQ: one subspace whose only codeword is the zero vector, so every
        // residual inner product is zero.
        let codebook = vec![0.0f32; DIM];
        let codes = vec![0u8; N_DOCS * EMBEDDING_IN_ONE_DOC];
        let view = EmvbView {
            dims: DIM,
            subspace_num: 1,
            sub_dim: DIM,
            k: 1,
            centroids: &centroids,
            codebook: &codebook,
            codes: &codes,
            assignments: &assignments,
            doc_lens: &doc_lens,
            doc_segment_offsets: &doc_segment_offsets,
            centroids_to_docid: &centroids_to_docid,
        };
        let mut query = vec![vec![0.0f32; DIM]; QUERY_TOKEN_NUM];
        for q in &mut query {
            q[3] = 1.0;
        }
        let options = EmvbSearchOptions {
            centroid_nprobe: 1,
            threshold_first: 0.5,
            n_doc_to_score: 100,
            out_second_stage: 50,
            threshold_final: 0.8,
        };
        let got = view.search(&query, 10, options);
        assert_eq!(got.len(), 10);
        let mut doc_ids: Vec<u32> = got.iter().map(|(doc, _)| *doc).collect();
        doc_ids.sort_unstable();
        for (i, (_, score)) in got.iter().enumerate() {
            assert!((score - 32.0).abs() < 1e-5, "score {i} = {score}");
        }
        assert_eq!(doc_ids, (30..40).collect::<Vec<u32>>());
    }
}
