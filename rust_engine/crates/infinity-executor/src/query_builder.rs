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

/// A posting iterator. `next(doc_id)` advances to the first document whose id
/// is at least `doc_id` and reports whether such a document exists; `doc_id()`
/// is [`INVALID_DOC_ID`] once exhausted.
pub trait DocIterator {
    fn next(&mut self, doc_id: u64) -> bool;
    fn doc_id(&self) -> u64;
    /// `dynamic_cast` support for callers that inspect the iterator tree.
    fn as_any(&self) -> &dyn Any;
}

/// Resolves a term leaf into a posting iterator (the reference's
/// `ColumnIndexReader::Lookup`).
pub trait TermLookup {
    fn lookup(&self, column: &str, term: &str) -> Option<Box<dyn DocIterator>>;
}

/// A term leaf.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct TermQueryNode {
    pub column: String,
    pub term: String,
}

/// The query tree. `Not` only appears in parser output and `AndNot` only in an
/// optimized tree; `QueryNode::optimize` enforces that transition.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum QueryNode {
    Term(TermQueryNode),
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
    fn optimize(self) -> Result<QueryNode, QueryError> {
        match self {
            QueryNode::Term(_) => Ok(self),
            QueryNode::Not(_) => Err(QueryError::NotAtTopLevel),
            QueryNode::AndNot(_) => Err(QueryError::UnexpectedAndNot),
            QueryNode::And(_) | QueryNode::Or(_) => self.optimize_multi(),
        }
    }

    fn optimize_child(child: QueryNode) -> Result<QueryNode, QueryError> {
        match child {
            QueryNode::Term(_) => Ok(child),
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
                        QueryNode::Term(_) | QueryNode::Or(_) => and_list.push(child),
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
                        QueryNode::Term(_) | QueryNode::And(_) | QueryNode::AndNot(_) => {
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
            QueryNode::Term(_) | QueryNode::AndNot(_) => Err(QueryError::UnexpectedAndNot),
        }
    }

    /// Builds the iterator tree from an optimized node. A missing term makes an
    /// `AND` / `AND_NOT` uninhabited, while an `OR` simply drops that branch.
    fn create_search(&self, lookup: &dyn TermLookup) -> Option<Box<dyn DocIterator>> {
        match self {
            QueryNode::Term(term) => lookup.lookup(&term.column, &term.term),
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

/// Input of [`QueryBuilder::create_search`].
pub struct FullTextQueryContext {
    pub query_tree: QueryNode,
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
        Ok(optimized.create_search(self.lookup))
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
        fn lookup(&self, column: &str, term: &str) -> Option<Box<dyn DocIterator>> {
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
            .create_search(FullTextQueryContext { query_tree: tree })
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
            .create_search(FullTextQueryContext { query_tree: tree })
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
            .create_search(FullTextQueryContext { query_tree: tree })
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
            .create_search(FullTextQueryContext { query_tree: tree })
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
