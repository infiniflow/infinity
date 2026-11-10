//! `MATCH TEXT` queries that use the reference's query-string grammar.
//!
//! The plain term list (`a b`, `"a b"`, `a^2`) is answered by the block-max WAND
//! path in `statements.rs`, which is the fast path for the common case. The
//! boolean grammar — `a AND b`, `a OR b`, `a AND NOT b`, `(a OR b) AND c`,
//! `column:a`, `"a b"~2` — needs a query tree, exactly like the reference:
//! `SearchDriver::ParseSingleWithFields` builds a [`QueryNode`] and
//! `QueryBuilder::CreateSearch` turns it into `AndIterator` / `OrIterator` /
//! `AndNotIterator` over posting iterators (`query_builder_impl.cpp`).
//!
//! This module supplies the one piece the port was missing: a [`TermLookup`]
//! over the live `TextIndex`es of a table. The reference streams its postings
//! through `ColumnIndexReader::Lookup`; here each leaf materialises its posting
//! list (`TextIndex::merge`, which already resolves sealed/unsealed duplicates)
//! because the iterator tree outlives the index lock. That is a deviation in
//! peak memory, not in results.

use std::collections::HashMap;
use std::collections::HashSet;

use crate::engine::state::SharedTextIndex;
use crate::query_builder::{
    DocIterator, FullTextQueryContext, PhraseDocIterator, QueryBuilder, QueryNode, TermDocIterator,
    TermLookup, INVALID_DOC_ID,
};

/// True when `query` uses anything `query_units` cannot express: a boolean
/// keyword, a group, or a `column:value` qualifier. Quoted text and `^boost` are
/// left to the fast path.
pub(crate) fn has_query_syntax(query: &str) -> bool {
    let mut token = String::new();
    let mut in_quote = false;
    for ch in query.chars() {
        match ch {
            '"' => {
                in_quote = !in_quote;
                token.clear();
            }
            _ if in_quote => {}
            '(' | ')' => return true,
            c if c.is_whitespace() => {
                if matches!(token.as_str(), "AND" | "OR" | "NOT") || token.contains(':') {
                    return true;
                }
                token.clear();
            }
            c => token.push(c),
        }
    }
    matches!(token.as_str(), "AND" | "OR" | "NOT") || token.contains(':')
}

/// `TermLookup` over a table's full-text columns.
///
/// A leaf resolves to a materialised `TermDocIterator`/`PhraseDocIterator`, so
/// the returned iterator owns its postings and the index lock is only held for
/// the duration of the lookup.
pub(crate) struct TextTermLookup {
    /// Column name (`m.column` entry, boost stripped) to its index.
    indexes: HashMap<String, SharedTextIndex>,
    delta_term: f64,
    delta_phrase: f64,
}

impl TextTermLookup {
    pub(crate) fn new(
        indexes: HashMap<String, SharedTextIndex>,
        delta_term: f64,
        delta_phrase: f64,
    ) -> Self {
        Self {
            indexes,
            delta_term,
            delta_phrase,
        }
    }
}

impl TermLookup for TextTermLookup {
    /// `TermQueryNode::CreateSearch` (`query_builder_impl.cpp`): one term's
    /// posting list, scored by `TermDocIterator` (BM25 with the column's corpus
    /// statistics).
    fn lookup(&self, column: &str, term: &str, boost: f64) -> Option<Box<dyn DocIterator>> {
        let index = self.indexes.get(column)?;
        let text = index.read().ok()?;
        // `TextIndex::merge` is the port's `ColumnIndexReader::Lookup`: every
        // sealed segment plus the unsealed tail, with stale duplicates resolved.
        let merged = text.merge(&[term.to_string()]);
        let postings = merged.postings.get(term)?.clone();
        if postings.is_empty() {
            return None;
        }
        let doc_lens: Vec<u32> = postings
            .iter()
            .map(|(row, _)| text.doc_len_opt(*row).unwrap_or(1))
            .collect();
        Some(Box::new(TermDocIterator::new(
            term,
            postings,
            doc_lens,
            merged.n_docs,
            merged.total_len,
            boost,
            self.delta_term,
        )))
    }

    /// `PhraseQueryNode::CreateSearch`: the terms' position lists (`"a b"~N`
    /// carries the slop), matched by `PhraseDocIterator`.
    fn lookup_phrase(
        &self,
        column: &str,
        terms: &[String],
        slop: u32,
        boost: f64,
    ) -> Option<Box<dyn DocIterator>> {
        let index = self.indexes.get(column)?;
        let text = index.read().ok()?;
        let mut postings: Vec<Vec<(u64, Vec<u32>)>> = Vec::with_capacity(terms.len());
        let mut rows: HashSet<u64> = HashSet::new();
        for term in terms {
            let mut list: Vec<(u64, Vec<u32>)> = Vec::new();
            let mut stream = text.term_stream_capture(term, true);
            while let Some(doc) = stream.doc() {
                list.push((doc, stream.positions().to_vec()));
                rows.insert(doc);
                stream.advance(doc + 1);
            }
            if list.is_empty() {
                return None;
            }
            postings.push(list);
        }
        let doc_lens: HashMap<u64, u32> = rows
            .into_iter()
            .map(|row| (row, text.doc_len_opt(row).unwrap_or(1)))
            .collect();
        Some(Box::new(PhraseDocIterator::new(
            terms.to_vec(),
            postings,
            slop,
            doc_lens,
            text.total_docs(),
            text.corpus_len(),
            boost,
            self.delta_phrase,
        )))
    }

    // The block-max WAND path is the fast path and stays in `statements.rs`; a
    // tree query is not a flat OR of term leaves.
}

/// Runs a parsed query tree and returns the top `topn` `(row, score)` pairs,
/// ordered by score descending and row ascending (the reference's
/// `ScoreResultHeap` order).
///
/// `None` means the tree produced no iterator (an empty or unbuildable query),
/// which the caller treats as "no syntax match, fall back".
pub(crate) fn run_query_tree(
    root: QueryNode,
    indexes: HashMap<String, SharedTextIndex>,
    topn: usize,
    dead: &HashSet<u64>,
    delta_term: f64,
    delta_phrase: f64,
) -> Option<Vec<(u64, f64)>> {
    let lookup = TextTermLookup::new(indexes, delta_term, delta_phrase);
    let mut iterator = QueryBuilder::new(&lookup)
        .create_search(FullTextQueryContext::new(root).with_topn(topn))
        .ok()??;
    let mut hits: Vec<(u64, f64)> = Vec::new();
    let mut target = 0u64;
    while iterator.next(target) {
        let doc = iterator.doc_id();
        if doc == INVALID_DOC_ID {
            break;
        }
        target = doc + 1;
        if dead.contains(&doc) {
            continue;
        }
        hits.push((doc, iterator.score()));
    }
    hits.sort_by(|a, b| {
        b.1.partial_cmp(&a.1)
            .unwrap_or(std::cmp::Ordering::Equal)
            .then(a.0.cmp(&b.0))
    });
    hits.truncate(topn.max(1));
    Some(hits)
}

#[cfg(test)]
mod tests {
    use super::has_query_syntax;

    #[test]
    fn plain_queries_stay_on_the_fast_path() {
        assert!(!has_query_syntax("alpha beta"));
        assert!(!has_query_syntax("\"alpha beta\""));
        assert!(!has_query_syntax("\"alpha beta\"~2"));
        assert!(!has_query_syntax("alpha^2 beta"));
    }

    #[test]
    fn boolean_and_field_queries_need_the_tree() {
        assert!(has_query_syntax("alpha AND beta"));
        assert!(has_query_syntax("alpha OR beta"));
        assert!(has_query_syntax("alpha NOT beta"));
        assert!(has_query_syntax("(alpha OR beta) AND gamma"));
        assert!(has_query_syntax("body:alpha"));
        // A keyword inside quotes is text, not an operator.
        assert!(!has_query_syntax("\"alpha AND beta\""));
    }
}
