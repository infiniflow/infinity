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

//! Ported from `src/unit_test/storage/invertedindex/search/bmw_long_query_ut.cpp`
//! (`BmwLongQueryTest`, TEST_AUDIT rows 1022-1028) plus
//! `query_match_ut.cpp::bmw_heap_differential` (row 1029).
//!
//! The reference builds one real full-text index over several segments of a
//! random corpus, then drives `HeapBlockMaxWandIterator` through long OR queries
//! (64..400 terms) and compares it against the no-pruning path: same documents,
//! same scores, same top-k, and the pruning counters must show that block skips
//! really happened. The cases are `DISABLED_SLOW_*` there because every one of
//! them loads a corpus and builds an index; here they carry `#[ignore = "slow: …"]`
//! and run under `cargo test -- --ignored`.
//!
//! The port drives the same iterator (`blockmax_wand_iterator::BlockMaxWandIterator`)
//! over the same source (`TextIndex`'s lazy posting streams through
//! `Engine::text_index_for`), so a regression in the pruning control flow shows
//! up as a missing document or a wrong score, exactly as in the reference.
//!
//! Corpus size knobs keep the reference's names and defaults where the port has
//! an equivalent: `INFINITY_BMW_IT_SEGMENTS` (4), `INFINITY_BMW_IT_DOCS` (4000
//! per segment), `INFINITY_BMW_IT_VOCAB` (5000) and `INFINITY_BMW_IT_WORDS` (40
//! per document). `INFINITY_BMW_IT_SCALE`, `INFINITY_BMW_IT_PERF` and
//! `INFINITY_BMW_IT_RANDOM` gate the three opt-in cases like the reference does.

use std::collections::HashMap;

use crate::invertedindex::search::blockmax_wand_iterator::{
    BlockMaxWandIterator, WandMode, WandStats,
};
use crate::{Engine, WandField, WandStream, WandUnit};
use parser::Literal;
use tempfile::tempdir;

const TABLE: &str = "bmw_it_table";
const INDEX: &str = "bmw_it_index";

fn env_u32(name: &str, default: u32) -> u32 {
    std::env::var(name)
        .ok()
        .and_then(|raw| raw.trim().parse().ok())
        .unwrap_or(default)
}

/// `BM25Ranker::idf`, spelled out so the corpus statistics of the expected
/// scores are visible at the call site (`1 + (N - df + 0.5) / (df + 0.5)`).
fn idf(n_docs: f64, df: f64) -> f64 {
    (1.0 + (n_docs - df + 0.5) / (df + 0.5)).ln()
}

/// One long OR query: `terms` distinct vocabulary words, each appearing once.
fn make_query(terms: u32, seed: u64, vocab: u32) -> String {
    // A small deterministic generator (xorshift64*): the reference uses
    // `std::mt19937_64`, and the exact sequence only has to be *a* sequence -
    // the assertions compare two algorithms on the same query, not against a
    // recorded answer.
    let mut state = seed | 1;
    let mut next = || {
        state ^= state >> 12;
        state ^= state << 25;
        state ^= state >> 27;
        state.wrapping_mul(0x2545_f491_4f6c_dd1d)
    };
    let mut words: Vec<u32> = (0..terms)
        .map(|_| (next() % u64::from(vocab)) as u32)
        .collect();
    // A WAND only exists with at least two distinct terms (the reference's
    // duplicate-term collapse), so repair the degenerate draw.
    if vocab > 1 && words.iter().all(|w| *w == words[0]) {
        let last = words.len() - 1;
        words[last] = (words[0] + 1) % vocab;
    }
    words
        .into_iter()
        .map(|w| format!("w{w:05} "))
        .collect::<String>()
        .trim_end()
        .to_string()
}

/// The corpus generator's own deterministic stream, so a rerun loads the same
/// documents: `w00000..w{vocab-1}`, `words` per row.
fn corpus_row(row: u64, words: u32, vocab: u32, seed: u64) -> String {
    let mut state = seed
        .wrapping_add(row.wrapping_mul(0x9e37_79b9_7f4a_7c15))
        .wrapping_mul(0x2545_f491_4f6c_dd1d)
        | 1;
    let mut next = || {
        state ^= state >> 12;
        state ^= state << 25;
        state ^= state >> 27;
        state.wrapping_mul(0x2545_f491_4f6c_dd1d)
    };
    (0..words)
        .map(|_| format!("w{:05} ", next() % u64::from(vocab)))
        .collect::<String>()
        .trim_end()
        .to_string()
}

struct Corpus {
    segments: u32,
    docs_per_segment: u32,
    vocab: u32,
    words: u32,
}

impl Corpus {
    fn from_env() -> Self {
        Self {
            segments: env_u32("INFINITY_BMW_IT_SEGMENTS", 4).max(1),
            docs_per_segment: env_u32("INFINITY_BMW_IT_DOCS", 4000).max(1),
            vocab: env_u32("INFINITY_BMW_IT_VOCAB", 5000).max(1),
            words: env_u32("INFINITY_BMW_IT_WORDS", 40).max(1),
        }
    }

    fn with_vocab(mut self, vocab: u32) -> Self {
        self.vocab = vocab.max(1);
        self
    }

    fn total_docs(&self) -> u64 {
        u64::from(self.segments) * u64::from(self.docs_per_segment)
    }
}

/// Creates the table, the full-text index and loads `corpus` segment by
/// segment (`PrepareIndex`).
async fn prepare(engine: &Engine, corpus: &Corpus) {
    engine.execute("CREATE DATABASE db1;").await.unwrap();
    engine
        .execute(&format!(
            "CREATE TABLE db1.{TABLE} (id VARCHAR, title VARCHAR, text VARCHAR);"
        ))
        .await
        .unwrap();
    // `IMPORT` is the reference's "one segment per import": loading segment by
    // segment is what makes the index cover more than one posting family.
    for segment in 0..corpus.segments {
        let start = u64::from(segment) * u64::from(corpus.docs_per_segment);
        let docs: Vec<Vec<Literal>> = (start..start + u64::from(corpus.docs_per_segment))
            .map(|row| {
                vec![
                    Literal::Str(row.to_string()),
                    Literal::Str("t".to_string()),
                    Literal::Str(corpus_row(row, corpus.words, corpus.vocab, 20260601)),
                ]
            })
            .collect();
        engine
            .import_literals(&format!("db1.{TABLE}"), None, docs)
            .await
            .unwrap();
    }
    engine
        .execute(&format!(
            "CREATE INDEX {INDEX} ON db1.{TABLE} (text) USING FULLTEXT WITH (analyzer='standard');"
        ))
        .await
        .unwrap();
}

/// One scan's shape: `topn == 0` is the reference's full scan (no threshold
/// feedback), otherwise the top-k heap.
#[derive(Clone, Copy)]
struct RunConfig {
    topn: usize,
    min_should: usize,
    naive: bool,
}

/// What one scan produced.
struct RunResult {
    /// Ascending by document when `topn == 0`; by score (desc) then doc
    /// (asc) otherwise.
    docs: Vec<(u64, f64)>,
    stats: WandStats,
    has_stats: bool,
}

/// Runs one query against the live index. The `TextIndex` borrows the engine's
/// registry, so the guard stays alive for the whole scan.
async fn run(engine: &Engine, query: &str, config: RunConfig) -> RunResult {
    let tokenizer = engine.index_tokenizer(&format!("db1.{TABLE}"), "text");
    let index = engine
        .text_index_for(&format!("db1.{TABLE}"), "text", tokenizer)
        .await
        .unwrap();
    let guard = index.read().await;

    let n_docs = guard.total_docs();
    let n = n_docs.max(1) as f64;
    let mut units: Vec<WandUnit<'_>> = Vec::new();
    for term in query.split_whitespace() {
        let stream = guard.term_stream(term);
        if stream.df() == 0 {
            // A missing term drops that OR branch, like the reference's
            // `QueryBuilder` skipping a term with no postings.
            continue;
        }
        units.push(WandUnit {
            idf: idf(n, stream.df() as f64),
            stream: WandStream::Term(stream),
            weight: 1.0,
            delta: None,
        });
    }
    let fields = vec![WandField {
        n_docs,
        total_len: guard.corpus_len(),
        units,
    }];
    let dead = common::RowBitmap::new();
    let k = config.topn.max(1);
    let mut iterator =
        BlockMaxWandIterator::new(fields, k, &dead, 0.0, config.min_should, WandMode::Bm25);
    if config.naive {
        iterator = iterator.with_naive();
    }

    if config.topn == 0 {
        // Full scan: `Next(doc + 1)` until exhaustion, collecting every hit in
        // document order.
        let mut docs = Vec::new();
        let mut target = None;
        while iterator.step(target) {
            let (doc, score) = iterator.current().expect("step() lands on a document");
            docs.push((doc, score));
            target = Some(doc + 1);
        }
        RunResult {
            docs,
            stats: iterator.stats(),
            has_stats: true,
        }
    } else {
        let (docs, stats) = iterator.run_with_stats();
        RunResult {
            docs,
            stats,
            has_stats: true,
        }
    }
}

/// `FullScanParity`: doc-by-doc equality with a relative tolerance, plus the
/// exhaustive top-N that a full scan implies.
fn assert_same_scan(naive: &[(u64, f64)], bmw: &[(u64, f64)], label: &str) {
    assert_eq!(
        naive.len(),
        bmw.len(),
        "{label}: hit count {} vs {}",
        naive.len(),
        bmw.len()
    );
    for (i, ((naive_doc, naive_score), (bmw_doc, bmw_score))) in
        naive.iter().zip(bmw.iter()).enumerate()
    {
        assert_eq!(naive_doc, bmw_doc, "{label}: doc mismatch at {i}");
        let tolerance = 1e-4 * naive_score.abs().max(1.0);
        assert!(
            (naive_score - bmw_score).abs() <= tolerance,
            "{label}: score mismatch at {i}: {naive_score} vs {bmw_score}"
        );
    }
}

fn sorted_by_score(docs: &[(u64, f64)]) -> Vec<(u64, f64)> {
    let mut out = docs.to_vec();
    out.sort_by(|a, b| {
        b.1.partial_cmp(&a.1)
            .unwrap_or(std::cmp::Ordering::Equal)
            .then(a.0.cmp(&b.0))
    });
    out
}

/// `LongQueryFullScanParity`: with no threshold, the naive OR and the pruned
/// scan must agree on the document set, order and every score.
#[tokio::test]
#[ignore = "slow: C++ BmwLongQueryTest.DISABLED_SLOW_LongQueryFullScanParity"]
async fn long_query_full_scan_parity() {
    let corpus = Corpus::from_env();
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare(&engine, &corpus).await;

    for terms in [64u32, 200, 400] {
        let query = make_query(terms, 1000 + u64::from(terms), corpus.vocab);
        let naive = run(
            &engine,
            &query,
            RunConfig {
                topn: 0,
                min_should: 1,
                naive: true,
            },
        )
        .await;
        let bmw = run(
            &engine,
            &query,
            RunConfig {
                topn: 0,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert!(!naive.docs.is_empty(), "terms={terms}: no hits");
        assert_same_scan(&naive.docs, &bmw.docs, &format!("terms={terms}"));
        eprintln!(
            "[BMW-IT] full-scan terms={} matched={} iter={} scored={} advances={} block_seeks={} heap_ops={}",
            terms,
            naive.docs.len(),
            bmw.stats.iterations,
            bmw.stats.docs_scored,
            bmw.stats.child_advances,
            bmw.stats.block_seeks,
            bmw.stats.heap_ops
        );
    }
}

/// `LongQueryTopKAndPruningEffectiveness`: top-k must equal "full scan, sort,
/// take k", be deterministic, and the pruning counters must show that the scan
/// did not visit every document.
#[tokio::test]
#[ignore = "slow: C++ BmwLongQueryTest.DISABLED_SLOW_LongQueryTopKAndPruningEffectiveness"]
async fn long_query_top_k_and_pruning_effectiveness() {
    let corpus = Corpus::from_env();
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare(&engine, &corpus).await;
    let total_docs = corpus.total_docs();

    let query = make_query(200, 424242, corpus.vocab);
    let reference = run(
        &engine,
        &query,
        RunConfig {
            topn: 0,
            min_should: 1,
            naive: true,
        },
    )
    .await;
    assert!(!reference.docs.is_empty());
    let reference_scores: HashMap<u64, f64> = reference.docs.iter().copied().collect();
    let reference_sorted = sorted_by_score(&reference.docs);

    // `topn=100` is the historical regression point for block skipping: a
    // looser threshold skips more aggressively, so it is the value that lost
    // the tail documents when the resume point was advanced wrongly.
    for topn in [1usize, 10, 100] {
        let bmw = run(
            &engine,
            &query,
            RunConfig {
                topn,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert_eq!(
            bmw.docs.len(),
            topn.min(reference.docs.len()),
            "topn={topn}: wrong result size"
        );
        // (1) every returned document exists in the reference with that score
        for (doc, score) in &bmw.docs {
            let expected = reference_scores
                .get(doc)
                .unwrap_or_else(|| panic!("topn={topn}: unexpected doc {doc}"));
            let tolerance = 1e-4 * expected.abs().max(1.0);
            assert!(
                (expected - score).abs() <= tolerance,
                "topn={topn}: doc {doc} scored {score}, reference {expected}"
            );
        }
        // (2) the descending score sequence equals the reference's top-k,
        // **document ids included**: two rows with the same score are separated
        // by the smaller id, so a score-only check passes on a result whose
        // tie-break went the other way.
        for (rank, (doc, score)) in bmw.docs.iter().enumerate() {
            let (expected_doc, expected_score) = reference_sorted[rank];
            let tolerance = 1e-4 * expected_score.abs().max(1.0);
            assert!(
                (expected_score - score).abs() <= tolerance,
                "topn={topn} rank {rank}: got doc {doc} score {score}, \
                 expected doc {expected_doc} score {expected_score}"
            );
            assert_eq!(
                *doc, expected_doc,
                "topn={topn} rank {rank}: got doc {doc}, expected doc {expected_doc} \
                 at the same score {score}"
            );
        }
        // (3) determinism
        let repeat = run(
            &engine,
            &query,
            RunConfig {
                topn,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert_eq!(repeat.docs, bmw.docs, "topn={topn}: not deterministic");
        // (4) pruning happened: the scan visited fewer documents than the table
        // holds, and top-10 needed far fewer exact scores than rounds.
        assert!(
            bmw.has_stats,
            "topn={topn}: the pruned path must report statistics"
        );
        assert!(
            bmw.stats.iterations < total_docs,
            "topn={topn}: {} iterations for {total_docs} documents",
            bmw.stats.iterations
        );
        if topn <= 10 && total_docs >= 4000 {
            assert!(
                bmw.stats.iterations < total_docs / 2,
                "topn={topn}: {} iterations is not a small fraction of {total_docs}",
                bmw.stats.iterations
            );
            assert!(
                bmw.stats.docs_scored < total_docs as usize / 2,
                "topn={topn}: {} exact scores is not a small fraction of {total_docs}",
                bmw.stats.docs_scored
            );
            assert!(
                bmw.stats.docs_scored < bmw.stats.iterations as usize,
                "topn={topn}: {} scores over {} rounds",
                bmw.stats.docs_scored,
                bmw.stats.iterations
            );
        }
        eprintln!(
            "[BMW-IT] topk={} docs={} iter={} scored={} advances={} block_seeks={} heap_ops={} iter/total={:.3}",
            topn,
            bmw.docs.len(),
            bmw.stats.iterations,
            bmw.stats.docs_scored,
            bmw.stats.child_advances,
            bmw.stats.block_seeks,
            bmw.stats.heap_ops,
            bmw.stats.iterations as f64 / total_docs as f64
        );
    }
}

/// `LongQueryDenseVocabulary`: a tiny vocabulary makes almost every document
/// match almost every query term, which is the worst case for block-max
/// pruning. The answers must stay correct under it.
#[tokio::test]
#[ignore = "slow: C++ BmwLongQueryTest.DISABLED_SLOW_LongQueryDenseVocabulary"]
async fn long_query_dense_vocabulary() {
    let corpus = Corpus::from_env().with_vocab(200);
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare(&engine, &corpus).await;
    let total_docs = corpus.total_docs();

    let query = make_query(200, 555, corpus.vocab);
    let reference = run(
        &engine,
        &query,
        RunConfig {
            topn: 0,
            min_should: 1,
            naive: true,
        },
    )
    .await;
    let full_scan = run(
        &engine,
        &query,
        RunConfig {
            topn: 0,
            min_should: 1,
            naive: false,
        },
    )
    .await;
    assert!(!reference.docs.is_empty());
    assert_same_scan(&reference.docs, &full_scan.docs, "dense");

    // The top-k must still equal the exhaustive result.
    let mut expected: Vec<f64> = reference.docs.iter().map(|(_, score)| *score).collect();
    expected.sort_by(|a, b| b.partial_cmp(a).unwrap_or(std::cmp::Ordering::Equal));
    let top10 = run(
        &engine,
        &query,
        RunConfig {
            topn: 10,
            min_should: 1,
            naive: false,
        },
    )
    .await;
    assert_eq!(top10.docs.len(), 10);
    for (rank, (doc, score)) in top10.docs.iter().enumerate() {
        let want = expected[rank];
        let tolerance = 1e-4 * want.abs().max(1.0);
        assert!(
            (want - score).abs() <= tolerance,
            "dense top rank {rank} (doc {doc}): {score} vs {want}"
        );
    }
    eprintln!(
        "[BMW-IT] dense(vocab={}) matched={} full_scan_iter={} top10_iter={} scored={} iter/total={:.3}",
        corpus.vocab,
        reference.docs.len(),
        full_scan.stats.iterations,
        top10.stats.iterations,
        top10.stats.docs_scored,
        top10.stats.iterations as f64 / total_docs as f64
    );
}

/// `LongQueryAtScale` (opt-in): one index build, then full-scan parity, top-k
/// parity, determinism and pruning statistics at the configured size.
///
/// `INFINITY_BMW_IT_SCALE=1` enables it; the size comes from the same
/// environment variables as the other cases (the reference documents 20
/// segments × 500k documents × 20000 vocabulary).
#[tokio::test]
#[ignore = "slow: C++ BmwLongQueryTest.DISABLED_SLOW_LongQueryAtScale"]
async fn long_query_at_scale() {
    if std::env::var_os("INFINITY_BMW_IT_SCALE").is_none() {
        eprintln!(
            "skipped: set INFINITY_BMW_IT_SCALE=1 (with INFINITY_BMW_IT_SEGMENTS/DOCS/VOCAB) to run \
             the large-scale validation"
        );
        return;
    }
    let corpus = Corpus::from_env();
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare(&engine, &corpus).await;
    let total_docs = corpus.total_docs();
    eprintln!(
        "[BMW-IT] scale corpus: {total_docs} docs ({} seg x {}), vocab={}, words/doc={}",
        corpus.segments, corpus.docs_per_segment, corpus.vocab, corpus.words
    );

    for terms in [100u32, 200] {
        let query = make_query(terms, 20261031 + u64::from(terms), corpus.vocab);
        // Full-scan parity, kept in `topn=0` form so the hit set is comparable.
        let naive = run(
            &engine,
            &query,
            RunConfig {
                topn: 0,
                min_should: 1,
                naive: true,
            },
        )
        .await;
        let bmw = run(
            &engine,
            &query,
            RunConfig {
                topn: 0,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert!(!naive.docs.is_empty(), "terms={terms}: no hits");
        assert_same_scan(&naive.docs, &bmw.docs, &format!("scale terms={terms}"));

        // The exhaustive top-10 of the full scan is the referee for top-k.
        let exhaustive = sorted_by_score(&naive.docs);
        let top = run(
            &engine,
            &query,
            RunConfig {
                topn: 10,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert_eq!(top.docs.len(), 10, "terms={terms}");
        for (rank, (doc, score)) in top.docs.iter().enumerate() {
            let (expected_doc, expected_score) = exhaustive[rank];
            let tolerance = 1e-4 * expected_score.abs().max(1.0);
            assert!(
                (expected_score - score).abs() <= tolerance,
                "terms={terms} rank {rank}: got doc {doc} score {score}, \
                 expected doc {expected_doc} score {expected_score}"
            );
        }
        let repeat = run(
            &engine,
            &query,
            RunConfig {
                topn: 10,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert_eq!(repeat.docs, top.docs, "terms={terms}: not deterministic");
        assert!(
            top.stats.iterations < total_docs,
            "terms={terms}: {} iterations for {total_docs} documents",
            top.stats.iterations
        );
        eprintln!(
            "[BMW-IT] scale terms={} matched={} full_iter={} top10_iter={} scored={} advances={} iter/total={:.4}",
            terms,
            naive.docs.len(),
            bmw.stats.iterations,
            top.stats.iterations,
            top.stats.docs_scored,
            top.stats.child_advances,
            top.stats.iterations as f64 / total_docs as f64
        );
    }
}

/// `LongQueryStability`: `Next(target)` jump semantics, repeated early give-up,
/// `minimum_should_match`, and long queries from several threads.
#[tokio::test]
#[ignore = "slow: C++ BmwLongQueryTest.DISABLED_SLOW_LongQueryStability"]
async fn long_query_stability() {
    let corpus = Corpus::from_env();
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare(&engine, &corpus).await;

    let query = make_query(200, 777, corpus.vocab);
    let reference = run(
        &engine,
        &query,
        RunConfig {
            topn: 0,
            min_should: 1,
            naive: true,
        },
    )
    .await;
    assert!(reference.docs.len() > 10, "the query must match many rows");

    // (a) `Next(target)` must land on the first reference document >= target.
    {
        let tokenizer = engine.index_tokenizer(&format!("db1.{TABLE}"), "text");
        let index = engine
            .text_index_for(&format!("db1.{TABLE}"), "text", tokenizer)
            .await
            .unwrap();
        let guard = index.read().await;
        let n = guard.total_docs().max(1) as f64;
        let mut units: Vec<WandUnit<'_>> = Vec::new();
        for term in query.split_whitespace() {
            let stream = guard.term_stream(term);
            if stream.df() == 0 {
                continue;
            }
            units.push(WandUnit {
                idf: idf(n, stream.df() as f64),
                stream: WandStream::Term(stream),
                weight: 1.0,
                delta: None,
            });
        }
        let fields = vec![WandField {
            n_docs: guard.total_docs(),
            total_len: guard.corpus_len(),
            units,
        }];
        let dead = common::RowBitmap::new();
        let mut iterator = BlockMaxWandIterator::new(fields, 1, &dead, 0.0, 1, WandMode::Bm25);
        let mut cursor = 0usize;
        let mut probe = 0usize;
        while probe < reference.docs.len() {
            let target = reference.docs[probe].0;
            assert!(
                iterator.step(Some(target)),
                "probe {probe}: Next({target}) ran off the end"
            );
            let (doc, score) = iterator.current().unwrap();
            while cursor < reference.docs.len() && reference.docs[cursor].0 < target {
                cursor += 1;
            }
            assert!(cursor < reference.docs.len());
            assert_eq!(
                doc, reference.docs[cursor].0,
                "probe {probe}: Next({target}) landed on {doc}"
            );
            let expected = reference.docs[cursor].1;
            let tolerance = 1e-4 * expected.abs().max(1.0);
            assert!(
                (expected - score).abs() <= tolerance,
                "probe {probe}: score {score} vs {expected}"
            );
            // A `Next()` that does not move keeps the cached score: a
            // threshold-feedback caller re-asks for the current document.
            assert!(iterator.step(Some(doc)), "probe {probe}: re-ask failed");
            let (same_doc, same_score) = iterator.current().unwrap();
            assert_eq!(same_doc, doc, "probe {probe}: re-ask moved");
            assert_eq!(same_score, score, "probe {probe}: re-ask rescored");
            probe += 17;
        }
    }

    // (b) early give-up: destroying a top-k scan after its first hits must not
    // corrupt anything.
    for round in 0..20u32 {
        let partial = run(
            &engine,
            &make_query(120, 900 + u64::from(round), corpus.vocab),
            RunConfig {
                topn: 10,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert!(partial.docs.len() <= 10);
    }

    // (c) minimum_should_match: naive and pruned must agree on top-10.
    {
        let naive = run(
            &engine,
            &query,
            RunConfig {
                topn: 10,
                min_should: 2,
                naive: true,
            },
        )
        .await;
        let bmw = run(
            &engine,
            &query,
            RunConfig {
                topn: 10,
                min_should: 2,
                naive: false,
            },
        )
        .await;
        assert_eq!(naive.docs.len(), bmw.docs.len(), "msm=2: result size");
        assert!(!naive.docs.is_empty(), "msm=2 matched nothing");
        for (rank, ((_, expected), (_, actual))) in
            naive.docs.iter().zip(bmw.docs.iter()).enumerate()
        {
            let tolerance = 1e-4 * expected.abs().max(1.0);
            assert!(
                (expected - actual).abs() <= tolerance,
                "msm=2 rank {rank}: {actual} vs {expected}"
            );
        }
    }

    // (d) the same index answers long queries from several threads; every
    // result must be complete.
    {
        const THREADS: usize = 4;
        const ROUNDS: usize = 5;
        let failures = std::sync::atomic::AtomicUsize::new(0);
        std::thread::scope(|scope| {
            for _ in 0..THREADS {
                scope.spawn(|| {
                    // The query path is async; each scoped OS thread drives its
                    // own runtime for the scans it owns.
                    let runtime = tokio::runtime::Builder::new_current_thread()
                        .enable_all()
                        .build()
                        .unwrap();
                    for _ in 0..ROUNDS {
                        let result = runtime.block_on(run(
                            &engine,
                            &query,
                            RunConfig {
                                topn: 10,
                                min_should: 1,
                                naive: false,
                            },
                        ));
                        if result.docs.len() != 10 {
                            failures.fetch_add(1, std::sync::atomic::Ordering::Relaxed);
                        }
                    }
                });
            }
        });
        assert_eq!(
            failures.load(std::sync::atomic::Ordering::Relaxed),
            0,
            "{THREADS} threads x {ROUNDS} rounds returned short results"
        );
    }
}

/// `LongQueryPerfComparison` (opt-in): naive vs pruned on the same index.
///
/// The reference also measures its legacy sort-based BMW and records the
/// difference; the port has one block-max iterator, so the comparison is naive
/// against pruned, which is what `block_max=false` exercises.
#[tokio::test]
#[ignore = "slow: C++ BmwLongQueryTest.DISABLED_SLOW_LongQueryPerfComparison"]
async fn long_query_perf_comparison() {
    if std::env::var_os("INFINITY_BMW_IT_PERF").is_none() {
        eprintln!(
            "skipped: set INFINITY_BMW_IT_PERF=1 (with INFINITY_BMW_IT_SEGMENTS/DOCS/VOCAB) to run \
             the perf comparison"
        );
        return;
    }
    let corpus = Corpus::from_env();
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare(&engine, &corpus).await;
    let total_docs = corpus.total_docs();
    let repeats = env_u32("INFINITY_BMW_IT_REPEAT", 5).max(1);

    let terms: Vec<u32> = std::env::var("INFINITY_BMW_IT_PERF_TERMS")
        .ok()
        .map(|raw| {
            raw.split(',')
                .filter_map(|t| t.trim().parse().ok())
                .filter(|t| *t > 0)
                .collect()
        })
        .unwrap_or_else(|| vec![100, 200]);
    assert!(!terms.is_empty(), "INFINITY_BMW_IT_PERF_TERMS parsed empty");

    let best = async |query: &str, config: RunConfig| -> (RunResult, f64) {
        let mut best: Option<(RunResult, f64)> = None;
        for _ in 0..repeats {
            let started = std::time::Instant::now();
            let result = run(&engine, query, config).await;
            let elapsed = started.elapsed().as_secs_f64() * 1e3;
            if best
                .as_ref()
                .is_none_or(|(_, previous)| elapsed < *previous)
            {
                best = Some((result, elapsed));
            }
        }
        best.unwrap()
    };

    for count in terms {
        let query = make_query(count, 20261031 + u64::from(count), corpus.vocab);
        let (naive, naive_ms) = best(
            &query,
            RunConfig {
                topn: 0,
                min_should: 1,
                naive: true,
            },
        )
        .await;
        assert!(!naive.docs.is_empty(), "terms={count}: no hits");
        for topn in [10usize, 100, 1000] {
            let (pruned, pruned_ms) = best(
                &query,
                RunConfig {
                    topn,
                    min_should: 1,
                    naive: false,
                },
            )
            .await;
            assert_eq!(
                pruned.docs.len(),
                topn.min(naive.docs.len()),
                "terms={count} topn={topn}: wrong result size"
            );
            eprintln!(
                "[BMW-PERF] terms={} topn={} | naive={:.1}ms | pruned={:.1}ms ({:.2}x) iter={} ({:.3}/doc) scored={} advances={} block_seeks={}",
                count,
                topn,
                naive_ms,
                pruned_ms,
                naive_ms / pruned_ms.max(f64::MIN_POSITIVE),
                pruned.stats.iterations,
                pruned.stats.iterations as f64 / total_docs as f64,
                pruned.stats.docs_scored,
                pruned.stats.child_advances,
                pruned.stats.block_seeks
            );
        }
        // Pure scan cost (threshold feedback off): the pruned path must answer
        // the same rows and the same score sum.
        let (pruned_full, pruned_ms) = best(
            &query,
            RunConfig {
                topn: 0,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert_eq!(
            pruned_full.docs.len(),
            naive.docs.len(),
            "terms={count}: full scan hit count"
        );
        let naive_sum: f64 = naive.docs.iter().map(|(_, score)| score).sum();
        let pruned_sum: f64 = pruned_full.docs.iter().map(|(_, score)| score).sum();
        assert!(
            (naive_sum - pruned_sum).abs() <= 1e-9 * naive_sum.abs().max(1.0),
            "terms={count}: score sums {pruned_sum} vs {naive_sum}"
        );
        eprintln!(
            "[BMW-PERF] terms={} topn=0 (no threshold feedback) | naive={:.1}ms | pruned={:.1}ms ({:.2}x)",
            count,
            naive_ms,
            pruned_ms,
            naive_ms / pruned_ms.max(f64::MIN_POSITIVE)
        );
    }
}

/// `LongQueryRandomizedDifferential` (opt-in): random query length, top-k and
/// `minimum_should_match`, plus random `Next(RowID)` jumps - the jump variant is
/// what exercises a block-level seek followed by a score on the same iterator,
/// the "stale posting block" failure class.
///
/// `INFINITY_BMW_IT_RANDOM=1` enables it; `INFINITY_BMW_IT_RANDOM_ROUNDS`
/// (default 12) sets the number of rounds.
#[tokio::test]
#[ignore = "slow: C++ BmwLongQueryTest.DISABLED_SLOW_LongQueryRandomizedDifferential"]
async fn long_query_randomized_differential() {
    if std::env::var_os("INFINITY_BMW_IT_RANDOM").is_none() {
        eprintln!(
            "skipped: set INFINITY_BMW_IT_RANDOM=1 (with INFINITY_BMW_IT_SEGMENTS/DOCS/VOCAB) to run \
             the randomized differential"
        );
        return;
    }
    let corpus = Corpus::from_env();
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    prepare(&engine, &corpus).await;

    let rounds = env_u32("INFINITY_BMW_IT_RANDOM_ROUNDS", 12).max(1);
    let mut rng = 0x2026_1116_dead_beefu64;
    let mut next = || {
        rng ^= rng >> 12;
        rng ^= rng << 25;
        rng ^= rng >> 27;
        rng.wrapping_mul(0x2545_f491_4f6c_dd1d)
    };

    for round in 0..rounds {
        let terms = 1 + (next() % 200) as u32;
        let query = make_query(terms, next(), corpus.vocab);
        let min_should = 1 + (next() % 2) as usize;
        let topn = [0usize, 1, 10, 100][(next() % 4) as usize];

        let naive = run(
            &engine,
            &query,
            RunConfig {
                topn,
                min_should,
                naive: true,
            },
        )
        .await;
        let pruned = run(
            &engine,
            &query,
            RunConfig {
                topn,
                min_should,
                naive: false,
            },
        )
        .await;
        assert_eq!(
            naive.docs.len(),
            pruned.docs.len(),
            "round {round} (terms={terms} topn={topn} msm={min_should}): result size"
        );
        for (rank, ((naive_doc, naive_score), (pruned_doc, pruned_score))) in
            naive.docs.iter().zip(pruned.docs.iter()).enumerate()
        {
            assert_eq!(
                naive_doc, pruned_doc,
                "round {round} rank {rank}: doc mismatch"
            );
            let tolerance = 1e-4 * naive_score.abs().max(1.0);
            assert!(
                (naive_score - pruned_score).abs() <= tolerance,
                "round {round} rank {rank}: {pruned_score} vs {naive_score}"
            );
        }

        // The jump variant: `Next(target)` for random targets over the same
        // reference scan.
        let reference = run(
            &engine,
            &query,
            RunConfig {
                topn: 0,
                min_should,
                naive: true,
            },
        )
        .await;
        if !reference.docs.is_empty() {
            let tokenizer = engine.index_tokenizer(&format!("db1.{TABLE}"), "text");
            let index = engine
                .text_index_for(&format!("db1.{TABLE}"), "text", tokenizer)
                .await
                .unwrap();
            let guard = index.read().await;
            let n = guard.total_docs().max(1) as f64;
            let mut units: Vec<WandUnit<'_>> = Vec::new();
            for term in query.split_whitespace() {
                let stream = guard.term_stream(term);
                if stream.df() == 0 {
                    continue;
                }
                units.push(WandUnit {
                    idf: idf(n, stream.df() as f64),
                    stream: WandStream::Term(stream),
                    weight: 1.0,
                    delta: None,
                });
            }
            let fields = vec![WandField {
                n_docs: guard.total_docs(),
                total_len: guard.corpus_len(),
                units,
            }];
            let dead = common::RowBitmap::new();
            let mut iterator =
                BlockMaxWandIterator::new(fields, 10, &dead, 0.0, min_should, WandMode::Bm25);
            // `Next(target)` is **forward-only**: both paths return the current
            // document when it is already at or past `target`
            // (`next_naive`/`next` both open with `current >= target`). Drawing
            // a target behind the cursor is therefore legal and lands on the
            // cached row, so the expectation is "the first hit at or after
            // max(target, current)", not "the first hit at or after target".
            let mut cursor = 0u64;
            for _ in 0..8 {
                let pick = (next() % reference.docs.len() as u64) as usize;
                let target = reference.docs[pick].0;
                assert!(
                    iterator.step(Some(target)),
                    "round {round}: Next({target}) ran off the end"
                );
                let (doc, score) = iterator.current().unwrap();
                let from = target.max(cursor);
                let at = reference.docs.partition_point(|(row, _)| *row < from);
                assert!(
                    at < reference.docs.len(),
                    "round {round}: target {target} past the last hit"
                );
                assert_eq!(
                    doc, reference.docs[at].0,
                    "round {round}: Next({target}) from {cursor} landed on {doc}"
                );
                let expected = reference.docs[at].1;
                let tolerance = 1e-4 * expected.abs().max(1.0);
                assert!(
                    (expected - score).abs() <= tolerance,
                    "round {round}: jump score {score} vs {expected}"
                );
                cursor = doc;
            }
        }
    }
}

/// `QueryMatchTest.bmw_heap_differential`
/// (`query_match_ut.cpp:215`): two segments of 800 rows each, a 200-term and a
/// 40-term query, full scan and top-10 - the naive OR and the pruned scan must
/// return the same documents and scores.
#[tokio::test]
#[ignore = "slow: C++ QueryMatchTest.bmw_heap_differential"]
async fn bmw_heap_differential() {
    let dir = tempdir().unwrap();
    let engine = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    engine.execute("CREATE DATABASE db1;").await.unwrap();
    engine
        .execute(&format!(
            "CREATE TABLE db1.{TABLE} (id VARCHAR, title VARCHAR, text VARCHAR);"
        ))
        .await
        .unwrap();
    // Two imports = two segments; every row is 40 words drawn from a
    // 1..120-wide vocabulary, which is what makes the query lists dense enough
    // for pruning to matter.
    let mut seed = 2024u64;
    let mut next = || {
        seed ^= seed >> 12;
        seed ^= seed << 25;
        seed ^= seed >> 27;
        seed.wrapping_mul(0x2545_f491_4f6c_dd1d)
    };
    let pick = |next: &mut dyn FnMut() -> u64| {
        // The reference is `rng() % (1 + rng() % 120)`, i.e. 0..120 inclusive.
        let bound = 1 + next() % 120;
        next() % bound
    };
    for segment in 0..2u64 {
        let docs: Vec<Vec<Literal>> = (0..800u64)
            .map(|i| {
                let text: String = (0..40)
                    .map(|_| format!("term{:03} ", pick(&mut next)))
                    .collect();
                vec![
                    Literal::Str((segment * 800 + i).to_string()),
                    Literal::Str("title".to_string()),
                    Literal::Str(text.trim_end().to_string()),
                ]
            })
            .collect();
        engine
            .import_literals(&format!("db1.{TABLE}"), None, docs)
            .await
            .unwrap();
    }
    engine
        .execute(&format!(
            "CREATE INDEX {INDEX} ON db1.{TABLE} (text) USING FULLTEXT WITH (analyzer='standard');"
        ))
        .await
        .unwrap();

    // The queries draw from the same vocabulary the corpus uses.
    let mut query_seed = 2024u64 ^ 0x5eed;
    let mut query_next = || {
        query_seed ^= query_seed >> 12;
        query_seed ^= query_seed << 25;
        query_seed ^= query_seed >> 27;
        query_seed.wrapping_mul(0x2545_f491_4f6c_dd1d)
    };
    let word = |next: &mut dyn FnMut() -> u64| {
        let bound = 1 + next() % 120;
        format!("term{:03} ", next() % bound)
    };
    let long_query: String = (0..200).map(|_| word(&mut query_next)).collect::<String>();
    let mid_query: String = (0..40).map(|_| word(&mut query_next)).collect::<String>();

    for (label, query) in [("long", long_query.trim()), ("mid", mid_query.trim())] {
        let naive = run(
            &engine,
            query,
            RunConfig {
                topn: 0,
                min_should: 1,
                naive: true,
            },
        )
        .await;
        let pruned = run(
            &engine,
            query,
            RunConfig {
                topn: 0,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert!(!naive.docs.is_empty(), "{label}: no hits");
        assert_same_scan(&naive.docs, &pruned.docs, label);

        let naive_top = run(
            &engine,
            query,
            RunConfig {
                topn: 10,
                min_should: 1,
                naive: true,
            },
        )
        .await;
        let pruned_top = run(
            &engine,
            query,
            RunConfig {
                topn: 10,
                min_should: 1,
                naive: false,
            },
        )
        .await;
        assert_eq!(
            naive_top.docs.len(),
            pruned_top.docs.len(),
            "{label}: top size"
        );
        assert!(!naive_top.docs.is_empty(), "{label}: top is empty");
        for (rank, ((_, expected), (_, actual))) in naive_top
            .docs
            .iter()
            .zip(pruned_top.docs.iter())
            .enumerate()
        {
            let tolerance = 1e-4 * expected.abs().max(1.0);
            assert!(
                (expected - actual).abs() <= tolerance,
                "{label} rank {rank}: {actual} vs {expected}"
            );
        }
        eprintln!(
            "[BMW-IT] differential {label}: matched={} top10_iter={} scored={}",
            naive.docs.len(),
            pruned_top.stats.iterations,
            pruned_top.stats.docs_scored
        );
    }
}
