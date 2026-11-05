//! Full-text query tree optimization and iterator construction, ported from the
//! reference's `query_node_impl.cpp` and `query_builder_impl.cpp`.
//!
//! The parser emits `AND` / `OR` / `NOT` nodes over term leaves. Optimization
//! first flattens nested `AND`s and `OR`s and rewrites a `NOT` under an `AND`
//! into the negative children of an `AND_NOT`; [`QueryBuilder::create_search`]
//! then builds the matching `AndIterator` / `OrIterator` / `AndNotIterator`
//! over posting iterators supplied by a [`TermLookup`].
//!
//! Leaf resolution is abstracted (`TermLookup`) because the posting source
//! differs between the segment reader and the in-memory merged index.

use std::any::Any;

/// The reference's `INVALID_ROWID`.
pub const INVALID_DOC_ID: u64 = u64::MAX;

/// The reference's `DocIteratorType`, limited to the iterators this crate
/// builds.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DocIteratorType {
    TermDocIterator,
    PhraseIterator,
    AndIterator,
    AndNotIterator,
    OrIterator,
    BmwIterator,
}

/// A posting iterator. `next(doc_id)` advances to the first document whose id
/// is at least `doc_id` and reports whether such a document exists; `doc_id()`
/// is [`INVALID_DOC_ID`] once exhausted.
pub trait DocIterator {
    fn next(&mut self, doc_id: u64) -> bool;
    fn doc_id(&self) -> u64;
    fn iterator_type(&self) -> DocIteratorType;
    /// `DocIterator::Score`; only the leaf terms and the WAND iterator score.
    fn score(&mut self) -> f64 {
        0.0
    }
    /// `TermDocIterator::GetDocFreq` (posting-list length) or
    /// `PhraseDocIterator::GetDocFreq` (documents matched so far).
    fn doc_freq(&self) -> u32 {
        0
    }
    /// `TermDocIterator::GetTermFreq`: frequency accumulated by `score`.
    fn term_freq(&self) -> u64 {
        0
    }
    /// `PhraseDocIterator::GetPhraseFreq`: phrase frequency accumulated by
    /// successful matches.
    fn phrase_freq(&self) -> f64 {
        0.0
    }
    /// `dynamic_cast` support for callers that inspect the iterator tree.
    fn as_any(&self) -> &dyn Any;
}

/// Resolves a term leaf into a posting iterator (the reference's
/// `ColumnIndexReader::Lookup`).
pub trait TermLookup {
    /// `boost` is the leaf's weight (`QueryNode::weight_`), already multiplied
    /// by every enclosing `^boost`.
    fn lookup(&self, column: &str, term: &str, boost: f64) -> Option<Box<dyn DocIterator>>;
    /// Resolves a phrase leaf (the reference's `PhraseQueryNode::CreateSearch`).
    fn lookup_phrase(
        &self,
        column: &str,
        terms: &[String],
        slop: u32,
        boost: f64,
    ) -> Option<Box<dyn DocIterator>> {
        let _ = (column, terms, slop, boost);
        None
    }
    /// Top-`topn` BM25 hits of a top-level OR over single terms, used for the
    /// block-max WAND early-termination path. Each entry carries the term and
    /// its boost. `None` when the lookup cannot score.
    fn bmw_search(
        &self,
        column: &str,
        terms: &[(String, f64)],
        topn: usize,
    ) -> Option<Vec<(u64, f64)>> {
        let _ = (column, terms, topn);
        None
    }
}

/// A term leaf. `boost` is the reference's `QueryNode::weight_` after every
/// `^boost` (on the leaf or an enclosing group) has been pushed down.
#[derive(Debug, Clone, PartialEq)]
pub struct TermQueryNode {
    pub column: String,
    pub term: String,
    pub boost: f32,
}

/// A phrase leaf: the analysed terms in order and the allowed slop
/// (`"a b"~N`). A slop of zero requires consecutive positions. `boost` is the
/// pushed-down weight.
#[derive(Debug, Clone, PartialEq)]
pub struct PhraseQueryNode {
    pub column: String,
    pub terms: Vec<String>,
    pub slop: u32,
    pub boost: f32,
}

/// The query tree. `Not` only appears in parser output and `AndNot` only in an
/// optimized tree; `QueryNode::optimize` enforces that transition.
#[derive(Debug, Clone, PartialEq)]
pub enum QueryNode {
    Term(TermQueryNode),
    Phrase(PhraseQueryNode),
    And(Vec<QueryNode>),
    Or(Vec<QueryNode>),
    Not(Vec<QueryNode>),
    AndNot(Vec<QueryNode>),
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum QueryError {
    NotAtTopLevel,
    UnexpectedAndNot,
    NestedNot,
    OrWithNotAndNonNot,
}

impl std::fmt::Display for QueryError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let message = match self {
            QueryError::NotAtTopLevel => "NotQueryNode should not be on the top level",
            QueryError::UnexpectedAndNot => "AndNotQueryNode should not exist in parser output",
            QueryError::NestedNot => "NotQueryNode should not have a not child",
            QueryError::OrWithNotAndNonNot => {
                "OrQueryNode should not have both not child and non-not child"
            }
        };
        f.write_str(message)
    }
}

impl std::error::Error for QueryError {}

impl QueryNode {
    /// Pushes the tree through `GetOptimizedQueryTree`: children first, then
    /// the node's own rewrite.
    pub fn optimize(self) -> Result<QueryNode, QueryError> {
        match self {
            QueryNode::Term(_) | QueryNode::Phrase(_) => Ok(self),
            QueryNode::Not(_) => Err(QueryError::NotAtTopLevel),
            QueryNode::AndNot(_) => Err(QueryError::UnexpectedAndNot),
            QueryNode::And(_) | QueryNode::Or(_) => self.optimize_multi(),
        }
    }

    fn optimize_child(child: QueryNode) -> Result<QueryNode, QueryError> {
        match child {
            QueryNode::Term(_) | QueryNode::Phrase(_) => Ok(child),
            QueryNode::Not(_) | QueryNode::And(_) | QueryNode::Or(_) => child.optimize_multi(),
            QueryNode::AndNot(_) => Err(QueryError::UnexpectedAndNot),
        }
    }

    fn optimize_multi(self) -> Result<QueryNode, QueryError> {
        match self {
            // NotQueryNode::InnerGetNewOptimizedQueryTree: an `OR` child is
            // flattened into the not-list, since `not (A or B)` is
            // `not A and not B`.
            QueryNode::Not(children) => {
                let mut not_list = Vec::with_capacity(children.len());
                for child in children {
                    match QueryNode::optimize_child(child)? {
                        QueryNode::Or(grandchildren) => not_list.extend(grandchildren),
                        QueryNode::Not(_) => return Err(QueryError::NestedNot),
                        child => not_list.push(child),
                    }
                }
                Ok(QueryNode::Not(not_list))
            }
            // AndQueryNode::InnerGetNewOptimizedQueryTree: flatten nested
            // `AND`s and an `AND_NOT`'s first child; collect every negation.
            QueryNode::And(children) => {
                let mut and_list: Vec<QueryNode> = Vec::new();
                let mut not_list: Vec<QueryNode> = Vec::new();
                for child in children {
                    let child = QueryNode::optimize_child(child)?;
                    match child {
                        QueryNode::And(grandchildren) => and_list.extend(grandchildren),
                        QueryNode::Term(_) | QueryNode::Phrase(_) | QueryNode::Or(_) => {
                            and_list.push(child)
                        }
                        QueryNode::Not(not_children) => not_list.extend(not_children),
                        QueryNode::AndNot(and_not_children) => {
                            let mut remaining = and_not_children.into_iter();
                            if let Some(first) = remaining.next() {
                                match first {
                                    QueryNode::And(grandchildren) => {
                                        and_list.extend(grandchildren);
                                    }
                                    first => and_list.push(first),
                                }
                            }
                            not_list.extend(remaining);
                        }
                    }
                }
                match (and_list.is_empty(), not_list.is_empty()) {
                    (true, _) => Ok(QueryNode::Not(not_list)),
                    (false, true) => Ok(QueryNode::And(and_list)),
                    (false, false) => {
                        let first = if and_list.len() == 1 {
                            // A single positive child is not wrapped in `AND`.
                            and_list.pop().expect("and_list is non-empty")
                        } else {
                            QueryNode::And(and_list)
                        };
                        let mut children = Vec::with_capacity(1 + not_list.len());
                        children.push(first);
                        children.extend(not_list);
                        Ok(QueryNode::AndNot(children))
                    }
                }
            }
            // OrQueryNode::InnerGetNewOptimizedQueryTree: flatten nested `OR`s.
            // An `OR` is either all negations (rewritten to `not (and ...)`) or
            // all positives; mixing them is invalid, as in the reference.
            QueryNode::Or(children) => {
                let mut or_list: Vec<QueryNode> = Vec::new();
                let mut not_list: Vec<QueryNode> = Vec::new();
                for child in children {
                    let child = QueryNode::optimize_child(child)?;
                    match child {
                        QueryNode::Or(grandchildren) => or_list.extend(grandchildren),
                        QueryNode::Term(_)
                        | QueryNode::Phrase(_)
                        | QueryNode::And(_)
                        | QueryNode::AndNot(_) => {
                            or_list.push(child);
                        }
                        QueryNode::Not(_) => not_list.push(child),
                    }
                }
                if or_list.is_empty() {
                    // `not A or not B` == `not (A and B)`.
                    let mut and_children = Vec::with_capacity(not_list.len());
                    for not_child in not_list {
                        let QueryNode::Not(mut not_children) = not_child else {
                            continue;
                        };
                        if not_children.len() == 1 {
                            and_children.push(not_children.pop().expect("length checked"));
                        } else {
                            and_children.push(QueryNode::Or(not_children));
                        }
                    }
                    let optimized = QueryNode::And(and_children).optimize_multi()?;
                    Ok(QueryNode::Not(vec![optimized]))
                } else if not_list.is_empty() {
                    if or_list.len() == 1 {
                        Ok(or_list.pop().expect("length checked"))
                    } else {
                        Ok(QueryNode::Or(or_list))
                    }
                } else {
                    Err(QueryError::OrWithNotAndNonNot)
                }
            }
            QueryNode::Term(_) | QueryNode::Phrase(_) | QueryNode::AndNot(_) => {
                Err(QueryError::UnexpectedAndNot)
            }
        }
    }

    /// Builds the iterator tree from an optimized node. A missing term makes an
    /// `AND` / `AND_NOT` uninhabited, while an `OR` simply drops that branch.
    fn create_search(&self, lookup: &dyn TermLookup) -> Option<Box<dyn DocIterator>> {
        match self {
            QueryNode::Term(term) => lookup.lookup(&term.column, &term.term, f64::from(term.boost)),
            QueryNode::Phrase(phrase) => lookup.lookup_phrase(
                &phrase.column,
                &phrase.terms,
                phrase.slop,
                f64::from(phrase.boost),
            ),
            QueryNode::And(children) => {
                let mut iterators = Vec::with_capacity(children.len());
                for child in children {
                    iterators.push(child.create_search(lookup)?);
                }
                match iterators.len() {
                    0 => None,
                    1 => iterators.pop(),
                    _ => Some(Box::new(AndIterator::new(iterators))),
                }
            }
            QueryNode::Or(children) => {
                let mut iterators = Vec::with_capacity(children.len());
                for child in children {
                    if let Some(iterator) = child.create_search(lookup) {
                        iterators.push(iterator);
                    }
                }
                match iterators.len() {
                    0 => None,
                    1 => iterators.pop(),
                    _ => Some(Box::new(OrIterator::new(iterators))),
                }
            }
            QueryNode::AndNot(children) => {
                let mut children = children.iter();
                let mut iterators = vec![children.next()?.create_search(lookup)?];
                for child in children {
                    if let Some(iterator) = child.create_search(lookup) {
                        iterators.push(iterator);
                    }
                }
                match iterators.len() {
                    1 => iterators.pop(),
                    _ => Some(Box::new(AndNotIterator::new(iterators))),
                }
            }
            // `NOT` must have been consumed by the optimizer.
            QueryNode::Not(_) => None,
        }
    }
}

/// Early-termination strategy for a top-level OR of term leaves
/// (the reference's `EarlyTermAlgo`; only the two used by the engine are
/// modelled).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum EarlyTermAlgo {
    /// Plain `OrIterator`.
    #[default]
    Naive,
    /// `BlockMaxWandIterator` over the term leaves.
    Bmw,
}

/// Input of [`QueryBuilder::create_search`].
pub struct FullTextQueryContext {
    pub query_tree: QueryNode,
    /// Result size for early termination; `0` disables it.
    pub topn: usize,
    pub early_term_algo: EarlyTermAlgo,
}

impl FullTextQueryContext {
    pub fn new(query_tree: QueryNode) -> Self {
        Self {
            query_tree,
            topn: 0,
            early_term_algo: EarlyTermAlgo::Naive,
        }
    }

    pub fn with_topn(mut self, topn: usize) -> Self {
        self.topn = topn;
        self
    }

    pub fn with_early_term(mut self, early_term_algo: EarlyTermAlgo) -> Self {
        self.early_term_algo = early_term_algo;
        self
    }
}

/// Optimizes a query tree and turns it into a document iterator.
pub struct QueryBuilder<'a> {
    lookup: &'a dyn TermLookup,
}

impl<'a> QueryBuilder<'a> {
    pub fn new(lookup: &'a dyn TermLookup) -> Self {
        Self { lookup }
    }

    pub fn create_search(
        &self,
        context: FullTextQueryContext,
    ) -> Result<Option<Box<dyn DocIterator>>, QueryError> {
        let optimized = context.query_tree.optimize()?;
        // `OrQueryNode::CreateSearch`: a top-level OR whose children are all
        // term leaves may use block-max WAND when a result size is known.
        if context.early_term_algo == EarlyTermAlgo::Bmw && context.topn > 0 {
            if let QueryNode::Or(children) = &optimized {
                if let Some(iter) = self.bmw_search(children, context.topn) {
                    return Ok(Some(iter));
                }
            }
        }
        Ok(optimized.create_search(self.lookup))
    }

    /// Builds the WAND iterator when every child is a term on one column and
    /// the lookup can score.
    fn bmw_search(&self, children: &[QueryNode], topn: usize) -> Option<Box<dyn DocIterator>> {
        let mut column: Option<&str> = None;
        let mut terms: Vec<(String, f64)> = Vec::with_capacity(children.len());
        for child in children {
            let QueryNode::Term(term) = child else {
                return None;
            };
            match column {
                None => column = Some(&term.column),
                Some(existing) if existing == term.column => {}
                Some(_) => return None,
            }
            terms.push((term.term.clone(), f64::from(term.boost)));
        }
        if terms.len() < 2 {
            return None;
        }
        let hits = self.lookup.bmw_search(column?, &terms, topn)?;
        Some(Box::new(BmwIterator::new(hits)))
    }
}

/// The reference's `BlockMaxWandIterator` result stream: the lookup runs the
/// scan up front, this adapter exposes the ranked hits as a document iterator.
pub struct BmwIterator {
    /// `(doc id, BM25 score)`, ascending by doc id.
    hits: Vec<(u64, f64)>,
    index: usize,
    doc_id: u64,
}

impl BmwIterator {
    pub fn new(mut hits: Vec<(u64, f64)>) -> Self {
        hits.sort_unstable_by_key(|hit| hit.0);
        Self {
            hits,
            index: 0,
            doc_id: INVALID_DOC_ID,
        }
    }

    pub fn hits(&self) -> &[(u64, f64)] {
        &self.hits
    }

    /// The score of the current document (`BlockMaxWandIterator::Score`).
    pub fn score(&self) -> f64 {
        self.hits
            .get(self.index)
            .filter(|hit| hit.0 == self.doc_id)
            .map(|hit| hit.1)
            .unwrap_or(0.0)
    }
}

impl DocIterator for BmwIterator {
    fn next(&mut self, doc_id: u64) -> bool {
        while self.index < self.hits.len() && self.hits[self.index].0 < doc_id {
            self.index += 1;
        }
        match self.hits.get(self.index) {
            Some(hit) => {
                self.doc_id = hit.0;
                true
            }
            None => {
                self.doc_id = INVALID_DOC_ID;
                false
            }
        }
    }

    fn doc_id(&self) -> u64 {
        self.doc_id
    }

    fn iterator_type(&self) -> DocIteratorType {
        DocIteratorType::BmwIterator
    }

    fn score(&mut self) -> f64 {
        BmwIterator::score(self)
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}

/// The BM25 constants of `BM25Ranker`.
const K1: f64 = 1.2;
const B: f64 = 0.75;

fn bm25_idf(n_docs: usize, doc_freq: usize) -> f64 {
    let n = n_docs.max(1) as f64;
    let df = doc_freq.max(1) as f64;
    (1.0 + (n - df + 0.5) / (df + 0.5)).ln()
}

/// A term leaf's posting iterator (`TermDocIterator`): the documents of one
/// term, their term frequencies and the corpus statistics BM25 needs.
pub struct TermDocIterator {
    term: String,
    /// `(doc id, term frequency)`, ascending by doc id.
    postings: Vec<(u64, u32)>,
    /// Document length aligned with `postings`.
    doc_lens: Vec<u32>,
    index: usize,
    doc_id: u64,
    doc_freq: u32,
    term_freq: u64,
    n_docs: usize,
    avg_len: f64,
    weight: f64,
    delta: f64,
    scored_doc: u64,
    score_cache: f64,
}

impl TermDocIterator {
    /// `n_docs`/`total_len` are the column's corpus statistics (all documents
    /// and their total token count), not just the matching ones.
    pub fn new(
        term: impl Into<String>,
        postings: Vec<(u64, u32)>,
        doc_lens: Vec<u32>,
        n_docs: usize,
        total_len: u64,
        weight: f64,
        delta: f64,
    ) -> Self {
        debug_assert_eq!(postings.len(), doc_lens.len());
        let doc_freq = postings.len() as u32;
        Self {
            term: term.into(),
            postings,
            doc_lens,
            index: 0,
            doc_id: INVALID_DOC_ID,
            doc_freq,
            term_freq: 0,
            n_docs,
            avg_len: if n_docs == 0 {
                1.0
            } else {
                (total_len as f64 / n_docs as f64).max(1e-9)
            },
            weight,
            delta,
            scored_doc: INVALID_DOC_ID,
            score_cache: 0.0,
        }
    }

    pub fn term(&self) -> &str {
        &self.term
    }

    /// `GetDocFreq`: the term's document frequency.
    pub fn doc_freq(&self) -> u32 {
        self.doc_freq
    }

    /// `GetTermFreq`: the term frequency accumulated over the documents whose
    /// `score` was taken (the reference adds `tf` inside `BM25Score`).
    pub fn term_freq(&self) -> u64 {
        self.term_freq
    }

    /// `BM25Score`. Cached per document, like the reference, and accumulates
    /// `term_freq` exactly once per document.
    pub fn score(&mut self) -> f64 {
        if self.doc_id == INVALID_DOC_ID {
            return 0.0;
        }
        if self.scored_doc == self.doc_id {
            return self.score_cache;
        }
        let (tf, dl) = self
            .postings
            .get(self.index)
            .map(|(_, tf)| {
                (
                    *tf as f64,
                    self.doc_lens.get(self.index).copied().unwrap_or(1) as f64,
                )
            })
            .unwrap_or((0.0, 1.0));
        let idf = bm25_idf(self.n_docs, self.postings.len());
        let norm = (K1 + 1.0) * tf / (tf + K1 * ((1.0 - B) + B * dl / self.avg_len));
        self.score_cache = idf * (norm + self.delta) * self.weight;
        self.term_freq += tf as u64;
        self.scored_doc = self.doc_id;
        self.score_cache
    }

    /// `BM25ScoreUpperBound`: `tf/(tf+p) <= 1`.
    pub fn upper_bound(&self) -> f64 {
        bm25_idf(self.n_docs, self.postings.len()) * (K1 + 1.0 + self.delta.max(0.0)) * self.weight
    }
}

impl DocIterator for TermDocIterator {
    fn next(&mut self, doc_id: u64) -> bool {
        if self.doc_id != INVALID_DOC_ID && self.doc_id >= doc_id {
            return true;
        }
        let rest = &self.postings[self.index..];
        let offset = rest.partition_point(|(doc, _)| *doc < doc_id);
        self.index += offset;
        match self.postings.get(self.index) {
            Some((doc, _)) => {
                self.doc_id = *doc;
                true
            }
            None => {
                self.doc_id = INVALID_DOC_ID;
                false
            }
        }
    }

    fn doc_id(&self) -> u64 {
        self.doc_id
    }

    fn iterator_type(&self) -> DocIteratorType {
        DocIteratorType::TermDocIterator
    }

    fn score(&mut self) -> f64 {
        TermDocIterator::score(self)
    }

    fn doc_freq(&self) -> u32 {
        self.doc_freq
    }

    fn term_freq(&self) -> u64 {
        self.term_freq
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}

/// A phrase leaf's iterator (`PhraseDocIterator`): only documents where the
/// analysed terms occur in order within `slop` positions.
pub struct PhraseDocIterator {
    terms: Vec<String>,
    /// Per query term: `(doc id, positions)`, ascending by doc id.
    postings: Vec<Vec<(u64, Vec<u32>)>>,
    cursors: Vec<usize>,
    doc_id: u64,
    doc_freq: u32,
    term_freq: f64,
    phrase_freq: f64,
    slop: u32,
    doc_lens: std::collections::HashMap<u64, u32>,
    n_docs: usize,
    avg_len: f64,
    weight: f64,
    delta: f64,
    scored_doc: u64,
    score_cache: f64,
}

impl PhraseDocIterator {
    #[allow(clippy::too_many_arguments)]
    pub fn new(
        terms: Vec<String>,
        postings: Vec<Vec<(u64, Vec<u32>)>>,
        slop: u32,
        doc_lens: std::collections::HashMap<u64, u32>,
        n_docs: usize,
        total_len: u64,
        weight: f64,
        delta: f64,
    ) -> Self {
        let cursors = vec![0; postings.len()];
        Self {
            terms,
            postings,
            cursors,
            doc_id: INVALID_DOC_ID,
            doc_freq: 0,
            term_freq: 0.0,
            phrase_freq: 0.0,
            slop,
            doc_lens,
            n_docs,
            avg_len: if n_docs == 0 {
                1.0
            } else {
                (total_len as f64 / n_docs as f64).max(1e-9)
            },
            weight,
            delta,
            scored_doc: INVALID_DOC_ID,
            score_cache: 0.0,
        }
    }

    pub fn terms(&self) -> &[String] {
        &self.terms
    }

    /// `GetDocFreq`: documents matched so far.
    pub fn doc_freq(&self) -> u32 {
        self.doc_freq
    }

    /// `GetPhraseFreq`: sum of the per-document phrase frequencies matched so
    /// far.
    pub fn phrase_freq(&self) -> f64 {
        self.phrase_freq
    }

    /// `BM25Score` of the current document.
    pub fn score(&mut self) -> f64 {
        if self.doc_id == INVALID_DOC_ID {
            return 0.0;
        }
        if self.scored_doc == self.doc_id {
            return self.score_cache;
        }
        let tf = self.term_freq;
        let dl = self.doc_lens.get(&self.doc_id).copied().unwrap_or(1) as f64;
        // `PhraseDocIterator::InitBM25Info` sums the terms' idf.
        let idf_sum: f64 = self
            .postings
            .iter()
            .map(|list| bm25_idf(self.n_docs, list.len()))
            .sum();
        let norm = (K1 + 1.0) * tf / (tf + K1 * ((1.0 - B) + B * dl / self.avg_len));
        self.score_cache = idf_sum * (norm + self.delta) * self.weight;
        self.scored_doc = self.doc_id;
        self.score_cache
    }

    /// Moves `cursor` to the first posting whose doc id is at least `target`.
    fn seek_term(&mut self, term: usize, target: u64) -> Option<u64> {
        let list = &self.postings[term];
        let mut cursor = self.cursors[term];
        while cursor < list.len() && list[cursor].0 < target {
            cursor += 1;
        }
        self.cursors[term] = cursor;
        list.get(cursor).map(|(doc, _)| *doc)
    }

    /// `GetExactPhraseMatchData`.
    fn exact_match(&mut self) -> bool {
        let starts: Vec<u32> = self.postings[0][self.cursors[0]].1.clone();
        let mut matched = 0u32;
        for start in starts {
            let mut found = true;
            for i in 1..self.postings.len() {
                let doc_positions = &self.postings[i][self.cursors[i]].1;
                if doc_positions.binary_search(&(start + i as u32)).is_err() {
                    found = false;
                    break;
                }
            }
            if found {
                matched += 1;
            }
        }
        if matched == 0 {
            return false;
        }
        self.doc_freq += 1;
        self.term_freq = f64::from(matched);
        self.phrase_freq += f64::from(matched);
        true
    }

    /// `GetSloppyPhraseMatchData`, including its `sloppyWeight`
    /// `1 / (1 + matchLength)` per solution.
    fn sloppy_match(&mut self) -> bool {
        #[derive(Clone, Copy)]
        struct Step {
            pos_i: usize,
            term_pos_idx: usize,
            min_phrase_pos: i32,
            max_phrase_pos: i32,
        }
        let positions: Vec<Vec<i32>> = self
            .postings
            .iter()
            .enumerate()
            .map(|(i, list)| {
                list[self.cursors[i]]
                    .1
                    .iter()
                    .map(|position| *position as i32)
                    .collect()
            })
            .collect();
        if positions.iter().any(Vec::is_empty) {
            return false;
        }
        let slop = self.slop as i32;
        let calibrate = |steps: &mut Vec<Step>, positions: &[Vec<i32>]| -> bool {
            let last = steps.len() - 1;
            if steps[last].term_pos_idx >= positions[steps[last].pos_i].len() {
                return false;
            }
            let phrase_pos_i =
                positions[steps[last].pos_i][steps[last].term_pos_idx] - steps[last].pos_i as i32;
            if steps[last].pos_i == 0 {
                steps[last].min_phrase_pos = phrase_pos_i;
                steps[last].max_phrase_pos = phrase_pos_i;
            } else {
                let prev_min = steps[last - 1].min_phrase_pos;
                let prev_max = steps[last - 1].max_phrase_pos;
                let step = &mut steps[last];
                step.min_phrase_pos = phrase_pos_i.min(prev_min);
                step.max_phrase_pos = phrase_pos_i.max(prev_max);
                if step.max_phrase_pos - step.min_phrase_pos > slop {
                    return false;
                }
            }
            true
        };
        let mut steps = vec![Step {
            pos_i: 0,
            term_pos_idx: 0,
            min_phrase_pos: positions[0][0],
            max_phrase_pos: positions[0][0],
        }];
        let mut solutions = 0usize;
        let mut tf = 0.0f64;
        while !steps.is_empty() {
            if calibrate(&mut steps, &positions) {
                if steps.len() == positions.len() {
                    let step = steps.last().expect("non-empty");
                    tf +=
                        1.0 / (1.0 + f64::from((step.max_phrase_pos - step.min_phrase_pos) as u32));
                    solutions += 1;
                    steps.last_mut().expect("non-empty").term_pos_idx += 1;
                } else {
                    let prev_min = steps.last().expect("non-empty").min_phrase_pos;
                    let pos_i = steps.len();
                    let lower = prev_min - slop + pos_i as i32;
                    let term_pos_idx =
                        positions[pos_i].partition_point(|position| *position < lower);
                    steps.push(Step {
                        pos_i,
                        term_pos_idx,
                        min_phrase_pos: 0,
                        max_phrase_pos: 0,
                    });
                }
            } else {
                steps.pop();
                if steps.is_empty() {
                    break;
                }
                if steps.len() != positions.len() {
                    steps.last_mut().expect("non-empty").term_pos_idx += 1;
                }
            }
        }
        if solutions == 0 {
            return false;
        }
        self.doc_freq += 1;
        self.term_freq = tf;
        self.phrase_freq += tf;
        true
    }
}

impl DocIterator for PhraseDocIterator {
    fn next(&mut self, doc_id: u64) -> bool {
        if self.doc_id != INVALID_DOC_ID && self.doc_id >= doc_id {
            return true;
        }
        if self.postings.is_empty() {
            self.doc_id = INVALID_DOC_ID;
            return false;
        }
        let mut target = doc_id;
        loop {
            for term in 0..self.postings.len() {
                match self.seek_term(term, target) {
                    Some(doc) => target = doc,
                    None => {
                        self.doc_id = INVALID_DOC_ID;
                        return false;
                    }
                }
            }
            if target == self.postings[0][self.cursors[0]].0 {
                if self.exact_or_sloppy_match() {
                    self.doc_id = target;
                    return true;
                }
                target += 1;
            }
        }
    }

    fn doc_id(&self) -> u64 {
        self.doc_id
    }

    fn iterator_type(&self) -> DocIteratorType {
        DocIteratorType::PhraseIterator
    }

    fn score(&mut self) -> f64 {
        PhraseDocIterator::score(self)
    }

    fn doc_freq(&self) -> u32 {
        self.doc_freq
    }

    fn phrase_freq(&self) -> f64 {
        self.phrase_freq
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}

impl PhraseDocIterator {
    fn exact_or_sloppy_match(&mut self) -> bool {
        if self.slop == 0 {
            self.exact_match()
        } else {
            self.sloppy_match()
        }
    }
}

/// `AndIterator`: advances every child to the same document.
pub struct AndIterator {
    children: Vec<Box<dyn DocIterator>>,
    doc_id: u64,
}

impl AndIterator {
    pub fn new(children: Vec<Box<dyn DocIterator>>) -> Self {
        Self {
            children,
            doc_id: INVALID_DOC_ID,
        }
    }

    pub fn children(&self) -> &[Box<dyn DocIterator>] {
        &self.children
    }
}

impl DocIterator for AndIterator {
    fn next(&mut self, doc_id: u64) -> bool {
        if self.doc_id != INVALID_DOC_ID && self.doc_id >= doc_id {
            return true;
        }
        if self.children.is_empty() {
            self.doc_id = INVALID_DOC_ID;
            return false;
        }
        let mut target = doc_id;
        loop {
            for child in &mut self.children {
                if !child.next(target) {
                    self.doc_id = INVALID_DOC_ID;
                    return false;
                }
                target = child.doc_id();
            }
            if target == INVALID_DOC_ID {
                self.doc_id = INVALID_DOC_ID;
                return false;
            }
            if target == self.children[0].doc_id() {
                self.doc_id = target;
                return true;
            }
        }
    }

    fn doc_id(&self) -> u64 {
        self.doc_id
    }

    fn iterator_type(&self) -> DocIteratorType {
        DocIteratorType::AndIterator
    }

    fn score(&mut self) -> f64 {
        self.children.iter_mut().map(|child| child.score()).sum()
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}

/// `OrIterator`: the heap's current minimum is the next document.
pub struct OrIterator {
    children: Vec<Box<dyn DocIterator>>,
    doc_id: u64,
}

impl OrIterator {
    pub fn new(children: Vec<Box<dyn DocIterator>>) -> Self {
        Self {
            children,
            doc_id: INVALID_DOC_ID,
        }
    }

    pub fn children(&self) -> &[Box<dyn DocIterator>] {
        &self.children
    }
}

impl DocIterator for OrIterator {
    fn next(&mut self, doc_id: u64) -> bool {
        if self.doc_id == INVALID_DOC_ID {
            // First use (or re-use after exhaustion): position every child at
            // the beginning, like the reference rebuilding its heap.
            for child in &mut self.children {
                child.next(0);
            }
            self.doc_id = self
                .children
                .iter()
                .map(|child| child.doc_id())
                .min()
                .unwrap_or(INVALID_DOC_ID);
        }
        if self.doc_id != INVALID_DOC_ID && self.doc_id >= doc_id {
            return true;
        }
        loop {
            let min = self
                .children
                .iter()
                .map(|child| child.doc_id())
                .min()
                .unwrap_or(INVALID_DOC_ID);
            if min == INVALID_DOC_ID {
                self.doc_id = INVALID_DOC_ID;
                return false;
            }
            if min >= doc_id {
                self.doc_id = min;
                return true;
            }
            for child in &mut self.children {
                if child.doc_id() == min {
                    child.next(doc_id);
                }
            }
        }
    }

    fn doc_id(&self) -> u64 {
        self.doc_id
    }

    fn iterator_type(&self) -> DocIteratorType {
        DocIteratorType::OrIterator
    }

    fn score(&mut self) -> f64 {
        // `OrIterator::Score`: only children positioned at the current document
        // contribute; the others sit on later postings.
        let doc_id = self.doc_id;
        self.children
            .iter_mut()
            .filter(|child| child.doc_id() == doc_id)
            .map(|child| child.score())
            .sum()
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}

/// `AndNotIterator`: the first child is the candidate set, every other child
/// removes documents.
pub struct AndNotIterator {
    children: Vec<Box<dyn DocIterator>>,
    doc_id: u64,
}

impl AndNotIterator {
    pub fn new(children: Vec<Box<dyn DocIterator>>) -> Self {
        Self {
            children,
            doc_id: INVALID_DOC_ID,
        }
    }

    pub fn children(&self) -> &[Box<dyn DocIterator>] {
        &self.children
    }
}

impl DocIterator for AndNotIterator {
    fn next(&mut self, mut doc_id: u64) -> bool {
        if self.children.is_empty() {
            self.doc_id = INVALID_DOC_ID;
            return false;
        }
        loop {
            let first = &mut self.children[0];
            first.next(doc_id);
            doc_id = first.doc_id();
            if doc_id == INVALID_DOC_ID {
                break;
            }
            let mut advance = false;
            for child in &mut self.children[1..] {
                child.next(doc_id);
                if child.doc_id() == doc_id {
                    doc_id += 1;
                    advance = true;
                    break;
                }
            }
            if !advance {
                break;
            }
        }
        self.doc_id = doc_id;
        doc_id != INVALID_DOC_ID
    }

    fn doc_id(&self) -> u64 {
        self.doc_id
    }

    fn iterator_type(&self) -> DocIteratorType {
        DocIteratorType::AndNotIterator
    }

    fn score(&mut self) -> f64 {
        self.children
            .first_mut()
            .map(|child| child.score())
            .unwrap_or(0.0)
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashMap;

    const DOC_ID_MAX_N: u32 = 100_000;
    const TEST_AND_VEC_N: u32 = 70_000;
    const TEST_OR_VEC_N: u32 = 10_000;
    const TEST_NOT_VEC_N: u32 = 5_000;

    /// Deterministic `mt19937` stand-in. The reference seeds from
    /// `random_device`; a fixed seed keeps this port reproducible.
    struct Rng(u64);

    impl Rng {
        fn new(seed: u64) -> Self {
            Self(seed)
        }

        fn next_u64(&mut self) -> u64 {
            self.0 = self.0.wrapping_add(0x9E37_79B9_7F4A_7C15);
            let mut z = self.0;
            z = (z ^ (z >> 30)).wrapping_mul(0xBF58_476D_1CE4_E5B9);
            z = (z ^ (z >> 27)).wrapping_mul(0x94D0_49BB_1331_11EB);
            z ^ (z >> 31)
        }

        /// `uniform_int_distribution<u32>(0, max)`, inclusive.
        fn uniform_inclusive(&mut self, max: u32) -> u32 {
            (self.next_u64() % (u64::from(max) + 1)) as u32
        }
    }

    fn random_doc_ids(rng: &mut Rng, max_len: u32) -> Vec<u64> {
        let size = rng.uniform_inclusive(max_len);
        let mut doc_ids: Vec<u64> = (0..size)
            .map(|_| u64::from(rng.uniform_inclusive(DOC_ID_MAX_N)))
            .collect();
        doc_ids.sort_unstable();
        doc_ids.dedup();
        doc_ids
    }

    fn intersect(left: &[u64], right: &[u64]) -> Vec<u64> {
        left.iter()
            .copied()
            .filter(|id| right.binary_search(id).is_ok())
            .collect()
    }

    fn union(left: &[u64], right: &[u64]) -> Vec<u64> {
        let mut out = Vec::with_capacity(left.len() + right.len());
        out.extend_from_slice(left);
        out.extend_from_slice(right);
        out.sort_unstable();
        out.dedup();
        out
    }

    fn difference(left: &[u64], right: &[u64]) -> Vec<u64> {
        left.iter()
            .copied()
            .filter(|id| right.binary_search(id).is_err())
            .collect()
    }

    fn term_node(term: &str) -> QueryNode {
        QueryNode::Term(TermQueryNode {
            column: "body".to_string(),
            term: term.to_string(),
            boost: 1.0,
        })
    }

    struct MockVectorDocIterator {
        doc_ids: Vec<u64>,
        index: usize,
        doc_id: u64,
    }

    impl MockVectorDocIterator {
        fn new(doc_ids: Vec<u64>) -> Self {
            Self {
                doc_ids,
                index: 0,
                doc_id: INVALID_DOC_ID,
            }
        }
    }

    impl DocIterator for MockVectorDocIterator {
        fn next(&mut self, doc_id: u64) -> bool {
            while self.index < self.doc_ids.len() && self.doc_ids[self.index] < doc_id {
                self.index += 1;
            }
            if self.index < self.doc_ids.len() {
                self.doc_id = self.doc_ids[self.index];
                true
            } else {
                self.doc_id = INVALID_DOC_ID;
                false
            }
        }

        fn doc_id(&self) -> u64 {
            self.doc_id
        }

        fn iterator_type(&self) -> DocIteratorType {
            DocIteratorType::TermDocIterator
        }

        fn as_any(&self) -> &dyn Any {
            self
        }
    }

    #[derive(Default)]
    struct MockTermLookup {
        docs: HashMap<(String, String), Vec<u64>>,
    }

    impl MockTermLookup {
        fn insert(&mut self, column: &str, term: &str, doc_ids: Vec<u64>) {
            self.docs
                .insert((column.to_string(), term.to_string()), doc_ids);
        }
    }

    impl TermLookup for MockTermLookup {
        fn lookup(&self, column: &str, term: &str, _boost: f64) -> Option<Box<dyn DocIterator>> {
            let doc_ids = self
                .docs
                .get(&(column.to_string(), term.to_string()))?
                .clone();
            Some(Box::new(MockVectorDocIterator::new(doc_ids)))
        }
    }

    fn assert_same_docs(
        actual: &mut dyn DocIterator,
        expected: &mut dyn DocIterator,
        break_on_end: bool,
    ) {
        for doc_id in 0..=u64::from(DOC_ID_MAX_N) + 1_000 {
            let actual_ok = actual.next(doc_id);
            let expected_ok = expected.next(doc_id);
            assert_eq!(actual_ok, expected_ok, "Next({doc_id})");
            assert_eq!(
                actual.doc_id(),
                expected.doc_id(),
                "DocID after Next({doc_id})"
            );
            if break_on_end && (!actual_ok || !expected_ok) {
                break;
            }
        }
    }

    #[test]
    fn test_and() {
        let mut rng = Rng::new(0x5EED_0001);
        let vec_a = random_doc_ids(&mut rng, TEST_AND_VEC_N);
        let vec_b = random_doc_ids(&mut rng, TEST_AND_VEC_N);
        let vec_ab_and = intersect(&vec_a, &vec_b);
        let vec_c = random_doc_ids(&mut rng, TEST_AND_VEC_N);
        let vec_d = random_doc_ids(&mut rng, TEST_AND_VEC_N);
        let vec_cd_and = intersect(&vec_c, &vec_d);
        let vec_e = random_doc_ids(&mut rng, TEST_AND_VEC_N);
        let vec_cde_and = intersect(&vec_cd_and, &vec_e);
        let expected = intersect(&vec_ab_and, &vec_cde_and);

        let mut lookup = MockTermLookup::default();
        lookup.insert("body", "A", vec_a);
        lookup.insert("body", "B", vec_b);
        lookup.insert("body", "C", vec_c);
        lookup.insert("body", "D", vec_d);
        lookup.insert("body", "E", vec_e);

        // (A and B) and ((C and D) and E): flattened to five children.
        let tree = QueryNode::And(vec![
            QueryNode::And(vec![term_node("A"), term_node("B")]),
            QueryNode::And(vec![
                QueryNode::And(vec![term_node("C"), term_node("D")]),
                term_node("E"),
            ]),
        ]);
        let builder = QueryBuilder::new(&lookup);
        let mut result = builder
            .create_search(FullTextQueryContext::new(tree))
            .unwrap()
            .unwrap();

        let and_iter = result
            .as_any()
            .downcast_ref::<AndIterator>()
            .expect("root should be an AndIterator");
        assert_eq!(and_iter.children().len(), 5);

        let mut expected_iter = MockVectorDocIterator::new(expected);
        assert_same_docs(&mut *result, &mut expected_iter, false);
    }

    #[test]
    fn test_or() {
        let mut rng = Rng::new(0x5EED_0002);
        let vec_a = random_doc_ids(&mut rng, TEST_OR_VEC_N);
        let vec_b = random_doc_ids(&mut rng, TEST_OR_VEC_N);
        let vec_ab_or = union(&vec_a, &vec_b);
        let vec_c = random_doc_ids(&mut rng, TEST_OR_VEC_N);
        let vec_d = random_doc_ids(&mut rng, TEST_OR_VEC_N);
        let vec_cd_or = union(&vec_c, &vec_d);
        let vec_e = random_doc_ids(&mut rng, TEST_OR_VEC_N);
        let vec_cde_or = union(&vec_cd_or, &vec_e);
        let expected = union(&vec_ab_or, &vec_cde_or);

        let mut lookup = MockTermLookup::default();
        lookup.insert("body", "A", vec_a);
        lookup.insert("body", "B", vec_b);
        lookup.insert("body", "C", vec_c);
        lookup.insert("body", "D", vec_d);
        lookup.insert("body", "E", vec_e);

        // (A or B) or ((C or D) or E): flattened to five children.
        let tree = QueryNode::Or(vec![
            QueryNode::Or(vec![term_node("A"), term_node("B")]),
            QueryNode::Or(vec![
                QueryNode::Or(vec![term_node("C"), term_node("D")]),
                term_node("E"),
            ]),
        ]);
        let builder = QueryBuilder::new(&lookup);
        let mut result = builder
            .create_search(FullTextQueryContext::new(tree))
            .unwrap()
            .unwrap();

        let or_iter = result
            .as_any()
            .downcast_ref::<OrIterator>()
            .expect("root should be an OrIterator");
        assert_eq!(or_iter.children().len(), 5);

        let mut expected_iter = MockVectorDocIterator::new(expected);
        assert_same_docs(&mut *result, &mut expected_iter, true);
    }

    #[test]
    fn test_and_not() {
        let mut rng = Rng::new(0x5EED_0003);
        let vec_a = random_doc_ids(&mut rng, TEST_OR_VEC_N);
        let vec_b = random_doc_ids(&mut rng, TEST_OR_VEC_N);
        let vec_ab_or = union(&vec_a, &vec_b);
        let vec_c = random_doc_ids(&mut rng, TEST_OR_VEC_N);
        let vec_abc_or = union(&vec_ab_or, &vec_c);
        let vec_d = random_doc_ids(&mut rng, TEST_NOT_VEC_N);
        let vec_abc_d_and_not = difference(&vec_abc_or, &vec_d);
        let vec_e = random_doc_ids(&mut rng, TEST_NOT_VEC_N);
        let expected = difference(&vec_abc_d_and_not, &vec_e);

        let mut lookup = MockTermLookup::default();
        lookup.insert("body", "A", vec_a);
        lookup.insert("body", "B", vec_b);
        lookup.insert("body", "C", vec_c);
        lookup.insert("body", "D", vec_d);
        lookup.insert("body", "E", vec_e);

        // ((A or B or C) and not D) and not E.
        let tree = QueryNode::And(vec![
            QueryNode::And(vec![
                QueryNode::Or(vec![
                    QueryNode::Or(vec![term_node("A"), term_node("B")]),
                    term_node("C"),
                ]),
                QueryNode::Not(vec![term_node("D")]),
            ]),
            QueryNode::Not(vec![term_node("E")]),
        ]);
        let builder = QueryBuilder::new(&lookup);
        let mut result = builder
            .create_search(FullTextQueryContext::new(tree))
            .unwrap()
            .unwrap();

        let and_not_iter = result
            .as_any()
            .downcast_ref::<AndNotIterator>()
            .expect("root should be an AndNotIterator");
        assert_eq!(and_not_iter.children().len(), 3);
        // The nested "or" is flattened into one three-child OrIterator.
        let child_or = and_not_iter.children()[0]
            .as_any()
            .downcast_ref::<OrIterator>()
            .expect("first child should be an OrIterator");
        assert_eq!(child_or.children().len(), 3);

        let mut expected_iter = MockVectorDocIterator::new(expected);
        assert_same_docs(&mut *result, &mut expected_iter, true);
    }

    #[test]
    fn test_and_not2() {
        let mut rng = Rng::new(0x5EED_0004);
        let vec_a = random_doc_ids(&mut rng, TEST_AND_VEC_N);
        let vec_b = random_doc_ids(&mut rng, TEST_AND_VEC_N);
        let vec_ab_and = intersect(&vec_a, &vec_b);
        let vec_c = random_doc_ids(&mut rng, TEST_NOT_VEC_N);
        let vec_ab_c_and_not = difference(&vec_ab_and, &vec_c);
        let vec_d = random_doc_ids(&mut rng, TEST_AND_VEC_N);
        let vec_abc_d_and = intersect(&vec_ab_c_and_not, &vec_d);
        let vec_e = random_doc_ids(&mut rng, TEST_AND_VEC_N);
        let expected = difference(&vec_abc_d_and, &vec_e);

        let mut lookup = MockTermLookup::default();
        lookup.insert("body", "A", vec_a);
        lookup.insert("body", "B", vec_b);
        lookup.insert("body", "C", vec_c);
        lookup.insert("body", "D", vec_d);
        lookup.insert("body", "E", vec_e);

        // ((A and B and D) and not C) and not E.
        let tree = QueryNode::And(vec![
            QueryNode::And(vec![
                QueryNode::And(vec![
                    QueryNode::And(vec![term_node("A"), term_node("B")]),
                    QueryNode::Not(vec![term_node("C")]),
                ]),
                term_node("D"),
            ]),
            QueryNode::Not(vec![term_node("E")]),
        ]);
        let builder = QueryBuilder::new(&lookup);
        let mut result = builder
            .create_search(FullTextQueryContext::new(tree))
            .unwrap()
            .unwrap();

        let and_not_iter = result
            .as_any()
            .downcast_ref::<AndNotIterator>()
            .expect("root should be an AndNotIterator");
        assert_eq!(and_not_iter.children().len(), 3);
        // The nested "and" is flattened into one three-child AndIterator.
        let child_and = and_not_iter.children()[0]
            .as_any()
            .downcast_ref::<AndIterator>()
            .expect("first child should be an AndIterator");
        assert_eq!(child_and.children().len(), 3);

        let mut expected_iter = MockVectorDocIterator::new(expected);
        assert_same_docs(&mut *result, &mut expected_iter, true);
    }
}
