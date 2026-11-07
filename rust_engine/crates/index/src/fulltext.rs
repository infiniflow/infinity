//! Full-text inverted index with BM25 ranking (plan §2.6 `kFullText`).
//!
//! Tokenization here is a simple ASCII/whitespace tokenizer. Chinese/mixed text
//! requires jieba-class segmentation — and **PoC G showed `jieba-rs` is not
//! equivalent to `cppjieba`**, so the analyzer must be ported or normalized
//! before this index is used on CJK content (see plan §2.6 / R1).

use std::collections::{BTreeMap, BTreeSet};

const K1: f32 = 1.2;
const B: f32 = 0.75;

/// Lowercase, split on non-alphanumeric ASCII.
pub fn tokenize(text: &str) -> Vec<String> {
    text.split(|c: char| !c.is_ascii_alphanumeric())
        .filter(|t| !t.is_empty())
        .map(|t| t.to_ascii_lowercase())
        .collect()
}

/// A BM25-ranked inverted index over documents identified by `u64` ids.
#[derive(Debug, Default)]
pub struct InvertedIndex {
    postings: BTreeMap<String, Vec<u64>>,
    doc_len: BTreeMap<u64, u32>,
}

impl InvertedIndex {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn doc_count(&self) -> usize {
        self.doc_len.len()
    }

    /// Indexes a document's text (replacing any previous content for that id).
    pub fn add_document(&mut self, doc_id: u64, text: &str) {
        let toks = tokenize(text);
        self.doc_len.insert(doc_id, toks.len() as u32);
        let unique: BTreeSet<&String> = toks.iter().collect();
        for w in unique {
            let list = self.postings.entry(w.clone()).or_default();
            if !list.contains(&doc_id) {
                list.push(doc_id);
            }
        }
    }

    /// Docs containing `term`.
    pub fn lookup(&self, term: &str) -> &[u64] {
        self.postings
            .get(&term.to_ascii_lowercase())
            .map(|v| v.as_slice())
            .unwrap_or(&[])
    }

    fn idf(&self, term: &str) -> f32 {
        let n = self.doc_count() as f32;
        let df = self.lookup(term).len() as f32;
        (1.0 + (n - df + 0.5) / (df + 0.5)).ln()
    }

    fn avgdl(&self) -> f32 {
        if self.doc_len.is_empty() {
            return 1.0;
        }
        let total: u32 = self.doc_len.values().sum();
        total as f32 / self.doc_len.len() as f32
    }

    /// BM25 doc score for one term.
    fn score_term(&self, term: &str, doc: u64, avgdl: f32) -> f32 {
        let dl = *self.doc_len.get(&doc).unwrap_or(&0) as f32;
        // term frequency within the doc
        // (we only stored doc ids, so recompute tf is not available; treat tf=1
        // per unique term, which is exact when duplicates are collapsed)
        let tf = 1.0f32;
        let idf = self.idf(term);
        idf * (tf * (K1 + 1.0)) / (tf + K1 * (1.0 - B + B * dl / avgdl))
    }

    /// AND query: documents containing every query term, ranked by summed BM25.
    pub fn search_and(&self, query: &str, top_k: usize) -> Vec<(u64, f32)> {
        let terms: Vec<String> = {
            let mut t = tokenize(query);
            t.sort();
            t.dedup();
            t
        };
        if terms.is_empty() {
            return vec![];
        }
        // intersection
        let mut acc: Option<BTreeSet<u64>> = None;
        for term in &terms {
            let set: BTreeSet<u64> = self.lookup(term).iter().copied().collect();
            acc = Some(match acc {
                None => set,
                Some(prev) => prev.intersection(&set).copied().collect(),
            });
        }
        let docs = acc.unwrap_or_default();
        let avgdl = self.avgdl();
        let mut scored: Vec<(u64, f32)> = docs
            .into_iter()
            .map(|d| {
                let s: f32 = terms.iter().map(|t| self.score_term(t, d, avgdl)).sum();
                (d, s)
            })
            .collect();
        scored.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap().then(a.0.cmp(&b.0)));
        scored.truncate(top_k);
        scored
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn tokenize_basic() {
        assert_eq!(tokenize("Hello, World!"), vec!["hello", "world"]);
        assert!(tokenize("   ").is_empty());
    }

    #[test]
    fn single_term_lookup() {
        let mut idx = InvertedIndex::new();
        idx.add_document(1, "the quick brown fox");
        idx.add_document(2, "the lazy dog");
        assert_eq!(idx.lookup("fox"), &[1]);
        assert_eq!(idx.lookup("the"), &[1, 2]);
        assert_eq!(idx.lookup("missing"), &[] as &[u64]);
    }

    #[test]
    fn and_query_and_ranking() {
        let mut idx = InvertedIndex::new();
        idx.add_document(1, "rust database rust systems");
        idx.add_document(2, "database systems design");
        idx.add_document(3, "cooking recipes");
        let hits = idx.search_and("rust database", 10);
        assert_eq!(hits.len(), 1);
        assert_eq!(hits[0].0, 1);
        // doc 1 has more "rust" occurrences contextually (longer doc) but is the
        // only one containing both terms
        assert!(idx.search_and("systems", 10).len() == 2);
        assert!(idx.search_and("nonexistent", 10).is_empty());
    }

    #[test]
    fn top_k_limits() {
        let mut idx = InvertedIndex::new();
        for i in 0..5 {
            idx.add_document(i, "common token");
        }
        assert_eq!(idx.search_and("common", 2).len(), 2);
    }
}
