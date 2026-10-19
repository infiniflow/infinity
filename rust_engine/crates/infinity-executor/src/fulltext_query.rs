//! The full-text query-string front end: the reference's `SearchDriver`
//! (`search_driver_impl.cpp`), its lexer/grammar (`search_lexer.l`,
//! `search_parser.y`), and the in-memory column index the query builder reads
//! term postings from.
//!
//! The grammar is Lucene's standard query syntax as implemented by the
//! reference:
//!
//! ```text
//! query       ::= clause | query clause | query OR clause   (juxtaposition is OR)
//! clause      ::= term | clause AND term
//! term        ::= basic_filter | NOT term | '(' query ')' ['^' boost]
//! basic_filter::= STRING ['^' boost]
//!               | STRING ':' STRING ['~' slop] ['^' boost]
//!               | STRING '~' slop ['^' boost]
//! ```
//!
//! A bare word is analysed with the field's analyzer; a quoted group with more
//! than one analysed term becomes a phrase carrying `~slop` (default 0), while
//! an unquoted run becomes an `OR` of its terms.

use std::collections::HashMap;

use crate::analyzer::{Analyzer, AnalyzerPool};
use crate::query_builder::{
    DocIterator, PhraseDocIterator, PhraseQueryNode, QueryNode, TermDocIterator, TermLookup,
    TermQueryNode,
};
use crate::tokenize_index_positions;

/// The reference's `FulltextQueryOperatorOption`. `InfinitySyntax` runs the
/// query-string grammar; `Or`/`And` analyze the whole query text and combine
/// the resulting terms with that operator.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum OperatorOption {
    #[default]
    InfinitySyntax,
    Or,
    And,
}

/// A search-syntax failure. The reference reports these as `SyntaxError`
/// (parse failures return no tree; invalid trees raise a recoverable error).
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum QueryParseError {
    EmptyQuery,
    EmptyDefaultField,
    UnterminatedString(char),
    Expected(&'static str),
    Analyzer(String),
}

impl std::fmt::Display for QueryParseError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            QueryParseError::EmptyQuery => f.write_str("invalid query statement: empty query"),
            QueryParseError::EmptyDefaultField => f.write_str("default_field is empty"),
            QueryParseError::UnterminatedString(quote) => {
                write!(f, "unterminated string starting with {quote}")
            }
            QueryParseError::Expected(what) => write!(f, "expected {what}"),
            QueryParseError::Analyzer(message) => f.write_str(message),
        }
    }
}

impl std::error::Error for QueryParseError {}

/// The reference's `SearchDriver`: parses a query string into a
/// [`QueryNode`] tree.
#[derive(Debug, Clone, Default)]
pub struct SearchDriver {
    /// `column -> index -> analyzer`; an empty inner map means `standard`.
    field2analyzer: HashMap<String, HashMap<String, String>>,
    default_field: String,
    operator_option: OperatorOption,
}

impl SearchDriver {
    pub fn new(
        field2analyzer: HashMap<String, HashMap<String, String>>,
        default_field: impl Into<String>,
    ) -> Self {
        Self {
            field2analyzer,
            default_field: default_field.into(),
            operator_option: OperatorOption::InfinitySyntax,
        }
    }

    pub fn with_operator(mut self, operator_option: OperatorOption) -> Self {
        self.operator_option = operator_option;
        self
    }

    /// `SearchDriver::ParseSingle`.
    pub fn parse_single(&self, query: &str) -> Result<QueryNode, QueryParseError> {
        let default_field = self.default_field.clone();
        self.parse_single_with_default(query, &default_field)
    }

    /// `SearchDriver::ParseSingleWithFields`: a comma-separated field list; a
    /// query over several fields becomes an `OR` of one parse per field.
    pub fn parse_single_with_fields(
        &self,
        fields: &str,
        query: &str,
    ) -> Result<QueryNode, QueryParseError> {
        let fields = parse_fields(fields);
        match fields.len() {
            0 => self.parse_single(query),
            1 => self.parse_single_with_default(query, &fields[0]),
            _ => {
                let mut children = Vec::with_capacity(fields.len());
                for field in fields {
                    if let Ok(node) = self.parse_single_with_default(query, &field) {
                        children.push(node);
                    }
                }
                match children.len() {
                    0 => Err(QueryParseError::EmptyQuery),
                    1 => Ok(children.pop().expect("length checked")),
                    _ => Ok(QueryNode::Or(children)),
                }
            }
        }
    }

    fn parse_single_with_default(
        &self,
        query: &str,
        default_field: &str,
    ) -> Result<QueryNode, QueryParseError> {
        let (field, index) = split_field_index(default_field);
        let analyzer_name = self.analyzer_name(&field, &index);
        if analyzer_name != "keyword" && self.operator_option == OperatorOption::InfinitySyntax {
            let tokens = lex(query)?;
            Parser::new(tokens, self, default_field).parse_top()
        } else {
            // Analyze the whole query text and combine the terms with the
            // configured operator (`SearchDriver::ParseSingle`'s else branch).
            let terms = root_terms(self.analyzer_terms(&analyzer_name, query)?);
            if terms.is_empty() {
                return Err(QueryParseError::EmptyQuery);
            }
            let nodes: Vec<QueryNode> = terms
                .into_iter()
                .map(|(term, _)| {
                    QueryNode::Term(TermQueryNode {
                        column: field.clone(),
                        term,
                    })
                })
                .collect();
            if nodes.len() == 1 && analyzer_name != "keyword" {
                Ok(nodes.into_iter().next().expect("length checked"))
            } else if analyzer_name == "keyword" {
                // `KeywordQueryNode`: every term matches any of the children.
                Ok(QueryNode::Or(nodes))
            } else {
                match self.operator_option {
                    OperatorOption::And => Ok(QueryNode::And(nodes)),
                    _ => Ok(QueryNode::Or(nodes)),
                }
            }
        }
    }

    /// `GetAnalyzerName`: the analyzer of `index` on `field`, defaulting to
    /// `standard`. With no index named, a sparse-gram analyzer is skipped so a
    /// regular analyzer is preferred (as `IndexReader::GetDefaultIndexName`).
    fn analyzer_name(&self, field: &str, index: &str) -> String {
        let Some(index2analyzer) = self.field2analyzer.get(field) else {
            return "standard".to_string();
        };
        if index2analyzer.is_empty() {
            return "standard".to_string();
        }
        if index.is_empty() {
            let mut first_analyzer = String::new();
            for analyzer in index2analyzer.values() {
                if analyzer.starts_with("sparsegram") {
                    if first_analyzer.is_empty() {
                        first_analyzer = analyzer.clone();
                    }
                    continue;
                }
                return analyzer.clone();
            }
            return first_analyzer;
        }
        index2analyzer
            .get(index)
            .cloned()
            .unwrap_or_else(|| "standard".to_string())
    }

    /// Analyzes `text` with the named analyzer, keeping every emitted form at
    /// its position (the standard analyzer emits a word and, when it differs,
    /// its English stem at the same offset).
    fn analyzer_terms(
        &self,
        analyzer_name: &str,
        text: &str,
    ) -> Result<Vec<(String, u32)>, QueryParseError> {
        match analyzer_name.split('-').next().unwrap_or("") {
            "keyword" => Ok(vec![(text.to_string(), 0)]),
            "whitespace" => Ok(text
                .split_whitespace()
                .enumerate()
                .map(|(position, token)| (token.to_string(), position as u32))
                .collect()),
            "chinese" => {
                // The reference's `chinese` analyzer is jieba-based; the Rust
                // engine's Chinese pipeline is the RAG analyzer.
                let mut analyzer = AnalyzerPool::instance()
                    .get_analyzer("rag-chinese")
                    .map_err(QueryParseError::Analyzer)?;
                match &mut analyzer {
                    Analyzer::Rag(rag) => Ok(rag
                        .analyze(text)
                        .into_iter()
                        .map(|term| (term.text, term.word_offset))
                        .collect()),
                    _ => Ok(Vec::new()),
                }
            }
            _ => Ok(tokenize_index_positions(text, &None)),
        }
    }

    /// `SearchDriver::AnalyzeAndBuildQueryNode`.
    fn analyze_and_build(
        &self,
        field_index: &str,
        text: &str,
        from_quoted: bool,
        slop: u32,
    ) -> Result<QueryNode, QueryParseError> {
        let (field, index) = split_field_index(field_index);
        let analyzer_name = self.analyzer_name(&field, &index);
        let terms = root_terms(self.analyzer_terms(&analyzer_name, text)?);
        if terms.is_empty() {
            return Ok(QueryNode::Term(TermQueryNode {
                column: field,
                term: text.to_string(),
            }));
        }
        if terms.len() == 1 {
            return Ok(QueryNode::Term(TermQueryNode {
                column: field,
                term: terms.into_iter().next().expect("length checked").0,
            }));
        }
        if from_quoted {
            return Ok(QueryNode::Phrase(PhraseQueryNode {
                column: field,
                terms: terms.into_iter().map(|(term, _)| term).collect(),
                slop,
            }));
        }
        Ok(QueryNode::Or(
            terms
                .into_iter()
                .map(|(term, _)| {
                    QueryNode::Term(TermQueryNode {
                        column: field.clone(),
                        term,
                    })
                })
                .collect(),
        ))
    }
}

/// `ParseFields`: comma-separated `field^boost` entries; boosts are a scoring
/// concern and are dropped by the structural builder.
fn parse_fields(fields: &str) -> Vec<String> {
    if fields.is_empty() {
        return Vec::new();
    }
    fields
        .split(',')
        .filter(|field| !field.is_empty())
        .map(|field| field.split('^').next().unwrap_or(field).to_string())
        .collect()
}

/// `SplitFieldIndex`: `column@index` names one index of a column.
fn split_field_index(field_index: &str) -> (String, String) {
    match field_index.split_once('@') {
        Some((field, index)) => (field.to_string(), index.to_string()),
        None => (field_index.to_string(), String::new()),
    }
}

/// `GetTermListFromAnalyzer`: keep the shortest emitted form per position (the
/// stem when one was emitted next to the original word).
fn root_terms(tokens: Vec<(String, u32)>) -> Vec<(String, u32)> {
    let mut out: Vec<(String, u32)> = Vec::with_capacity(tokens.len());
    for (text, offset) in tokens {
        match out.last_mut() {
            Some((last_text, last_offset)) if *last_offset == offset => {
                if text.len() < last_text.len() {
                    *last_text = text;
                }
            }
            _ => out.push((text, offset)),
        }
    }
    out
}

#[derive(Debug, Clone, PartialEq)]
enum Token {
    And,
    Or,
    Not,
    LParen,
    RParen,
    Colon,
    Caret,
    Tilde(u32),
    Text { text: String, quoted: bool },
}

/// `SearchDriver::Unescape`: a backslash before a reserved character is
/// removed.
fn unescape(text: &str) -> String {
    let chars: Vec<char> = text.chars().collect();
    let mut out = String::with_capacity(text.len());
    let mut i = 0;
    while i < chars.len() {
        if chars[i] == '\\' && i + 1 < chars.len() && is_escapable(chars[i + 1]) {
            out.push(chars[i + 1]);
            i += 2;
        } else {
            out.push(chars[i]);
            i += 1;
        }
    }
    out
}

fn is_escapable(c: char) -> bool {
    matches!(
        c,
        ' ' | '(' | ')' | '^' | '"' | '\'' | '~' | '*' | '?' | ':' | '\\'
    )
}

/// `NOESCAPE` in `search_lexer.l`: a graphic character minus the reserved set;
/// every non-ASCII code point is also part of a word.
fn is_noescape(c: char) -> bool {
    (c.is_ascii_graphic() || !c.is_ascii())
        && !matches!(
            c,
            '(' | ')' | '^' | '"' | '\'' | '~' | '*' | '?' | ':' | '\\'
        )
}

/// Whether a bare string token can continue with `c` (used to decide whether
/// `AND`/`OR`/`NOT` is a keyword or the prefix of a word).
fn is_word_continuation(c: char) -> bool {
    is_noescape(c) || c == '\\'
}

fn lex(query: &str) -> Result<Vec<Token>, QueryParseError> {
    let chars: Vec<char> = query.chars().collect();
    let mut tokens = Vec::new();
    let mut i = 0;
    let keyword = |at: usize, keyword: &str| -> bool {
        let end = at + keyword.chars().count();
        if end > chars.len() {
            return false;
        }
        let candidate: String = chars[at..end].iter().collect();
        candidate == keyword && (end == chars.len() || !is_word_continuation(chars[end]))
    };
    while i < chars.len() {
        let c = chars[i];
        if c.is_whitespace() {
            i += 1;
            continue;
        }
        if keyword(i, "AND") {
            tokens.push(Token::And);
            i += 3;
            continue;
        }
        if keyword(i, "OR") {
            tokens.push(Token::Or);
            i += 2;
            continue;
        }
        if keyword(i, "NOT") {
            tokens.push(Token::Not);
            i += 3;
            continue;
        }
        match c {
            '(' => {
                tokens.push(Token::LParen);
                i += 1;
            }
            ')' => {
                tokens.push(Token::RParen);
                i += 1;
            }
            ':' => {
                tokens.push(Token::Colon);
                i += 1;
            }
            '^' => match caret_number(&chars, i + 1) {
                Some((_, next)) => {
                    // Boosts affect scoring only; the tree shape ignores them.
                    tokens.push(Token::Caret);
                    i = next;
                }
                // An unmatched `^` is echoed and dropped by the scanner.
                None => i += 1,
            },
            '~' => match digits(&chars, i + 1) {
                Some((value, next)) => {
                    tokens.push(Token::Tilde(value));
                    i = next;
                }
                None => i += 1,
            },
            '"' | '\'' => {
                let (text, next) = quoted(&chars, i, c)?;
                tokens.push(Token::Text { text, quoted: true });
                i = next;
            }
            _ => {
                if is_noescape(c) || (c == '\\' && i + 1 < chars.len()) {
                    let mut text = String::new();
                    while i < chars.len() {
                        if chars[i] == '\\' && i + 1 < chars.len() && is_escapable(chars[i + 1]) {
                            text.push('\\');
                            text.push(chars[i + 1]);
                            i += 2;
                        } else if is_noescape(chars[i]) {
                            text.push(chars[i]);
                            i += 1;
                        } else {
                            break;
                        }
                    }
                    if !text.is_empty() {
                        tokens.push(Token::Text {
                            text,
                            quoted: false,
                        });
                    }
                } else {
                    // Default scanner action: the character is echoed and
                    // produces no token.
                    i += 1;
                }
            }
        }
    }
    Ok(tokens)
}

/// `"^"[0-9]+("."[0-9]*)?` and `"^."[0-9]+`; returns the value and the next
/// index.
fn caret_number(chars: &[char], start: usize) -> Option<(f32, usize)> {
    let mut j = start;
    while j < chars.len() && chars[j].is_ascii_digit() {
        j += 1;
    }
    if j == start {
        // `^.5`
        if start + 1 < chars.len() && chars[start] == '.' && chars[start + 1].is_ascii_digit() {
            j = start + 1;
            while j < chars.len() && chars[j].is_ascii_digit() {
                j += 1;
            }
        } else {
            return None;
        }
    } else if j < chars.len() && chars[j] == '.' {
        j += 1;
        while j < chars.len() && chars[j].is_ascii_digit() {
            j += 1;
        }
    }
    let text: String = chars[start..j].iter().collect();
    Some((text.parse().ok()?, j))
}

/// `"~"[0-9]+`; returns the value and the next index.
fn digits(chars: &[char], start: usize) -> Option<(u32, usize)> {
    let mut j = start;
    while j < chars.len() && chars[j].is_ascii_digit() {
        j += 1;
    }
    if j == start {
        return None;
    }
    let text: String = chars[start..j].iter().collect();
    Some((text.parse().ok()?, j))
}

/// Quoted strings keep a backslash and the following character verbatim; the
/// grammar unescapes them later.
fn quoted(chars: &[char], start: usize, quote: char) -> Result<(String, usize), QueryParseError> {
    let mut text = String::new();
    let mut i = start + 1;
    while i < chars.len() {
        let c = chars[i];
        if c == quote {
            return Ok((text, i + 1));
        }
        if c == '\\' && i + 1 < chars.len() {
            text.push(c);
            text.push(chars[i + 1]);
            i += 2;
        } else {
            text.push(c);
            i += 1;
        }
    }
    Err(QueryParseError::UnterminatedString(quote))
}

/// Recursive-descent parser for the grammar in the module docs.
struct Parser<'a> {
    tokens: Vec<Token>,
    position: usize,
    driver: &'a SearchDriver,
    default_field: &'a str,
}

impl<'a> Parser<'a> {
    fn new(tokens: Vec<Token>, driver: &'a SearchDriver, default_field: &'a str) -> Self {
        Self {
            tokens,
            position: 0,
            driver,
            default_field,
        }
    }

    fn parse_top(&mut self) -> Result<QueryNode, QueryParseError> {
        if self.tokens.is_empty() {
            return Err(QueryParseError::EmptyQuery);
        }
        let node = self.parse_query()?;
        if self.position != self.tokens.len() {
            return Err(QueryParseError::Expected("end of query"));
        }
        Ok(node)
    }

    fn peek(&self) -> Option<&Token> {
        self.tokens.get(self.position)
    }

    fn eat(&mut self, token: &Token) -> bool {
        if self.peek() == Some(token) {
            self.position += 1;
            true
        } else {
            false
        }
    }

    fn parse_query(&mut self) -> Result<QueryNode, QueryParseError> {
        let mut node = self.parse_clause()?;
        loop {
            if self.eat(&Token::Or) {
                let right = self.parse_clause()?;
                node = QueryNode::Or(vec![node, right]);
            } else if self.starts_term() {
                // Juxtaposed clauses are an implicit OR.
                let right = self.parse_clause()?;
                node = QueryNode::Or(vec![node, right]);
            } else {
                break;
            }
        }
        Ok(node)
    }

    fn starts_term(&self) -> bool {
        !matches!(
            self.peek(),
            None | Some(Token::RParen) | Some(Token::And) | Some(Token::Or)
        )
    }

    fn parse_clause(&mut self) -> Result<QueryNode, QueryParseError> {
        let mut node = self.parse_term()?;
        while self.eat(&Token::And) {
            let right = self.parse_term()?;
            node = QueryNode::And(vec![node, right]);
        }
        Ok(node)
    }

    fn parse_term(&mut self) -> Result<QueryNode, QueryParseError> {
        if self.eat(&Token::Not) {
            return Ok(QueryNode::Not(vec![self.parse_term()?]));
        }
        if self.eat(&Token::LParen) {
            let node = self.parse_query()?;
            if !self.eat(&Token::RParen) {
                return Err(QueryParseError::Expected("')'"));
            }
            // `(...)^boost`
            self.eat(&Token::Caret);
            return Ok(node);
        }
        let node = self.parse_basic_filter()?;
        // `term^boost`
        self.eat(&Token::Caret);
        Ok(node)
    }

    fn parse_basic_filter(&mut self) -> Result<QueryNode, QueryParseError> {
        let Some(Token::Text { text, quoted }) = self.peek().cloned() else {
            return Err(QueryParseError::Expected("query term"));
        };
        self.position += 1;
        let text = unescape(&text);
        if self.eat(&Token::Colon) {
            let Some(Token::Text {
                text: value,
                quoted: value_quoted,
            }) = self.peek().cloned()
            else {
                return Err(QueryParseError::Expected("term after ':'"));
            };
            self.position += 1;
            let value = unescape(&value);
            let slop = match self.peek() {
                Some(Token::Tilde(slop)) => {
                    let slop = *slop;
                    self.position += 1;
                    slop
                }
                _ => 0,
            };
            return self
                .driver
                .analyze_and_build(&text, &value, value_quoted, slop);
        }
        let slop = match self.peek() {
            Some(Token::Tilde(slop)) => {
                let slop = *slop;
                self.position += 1;
                slop
            }
            _ => 0,
        };
        if self.default_field.is_empty() {
            return Err(QueryParseError::EmptyDefaultField);
        }
        self.driver
            .analyze_and_build(self.default_field, &text, quoted, slop)
    }
}

/// An in-memory segment of one text column: document lengths and
/// `term -> [(doc, tf, positions)]`, the query view of the reference's
/// `MemoryIndexer`.
#[derive(Debug, Clone)]
pub struct MemorySegment {
    column: String,
    docs: HashMap<u64, u32>,
    postings: HashMap<String, Vec<(u64, u32, Vec<u32>)>>,
    total_len: u64,
}

impl MemorySegment {
    pub fn new(column: impl Into<String>) -> Self {
        Self {
            column: column.into(),
            docs: HashMap::new(),
            postings: HashMap::new(),
            total_len: 0,
        }
    }

    pub fn column(&self) -> &str {
        &self.column
    }

    /// Indexes one document with the standard analyzer, recording both the
    /// word and its stem at the word's position.
    pub fn insert(&mut self, doc: u64, text: &str) {
        let tokens = tokenize_index_positions(text, &None);
        self.total_len += tokens.len() as u64;
        self.docs.insert(doc, tokens.len() as u32);
        let mut positions: HashMap<&str, Vec<u32>> = HashMap::new();
        for (term, position) in &tokens {
            positions.entry(term.as_str()).or_default().push(*position);
        }
        for (term, positions) in positions {
            self.postings.entry(term.to_string()).or_default().push((
                doc,
                positions.len() as u32,
                positions,
            ));
        }
    }

    pub fn doc_count(&self) -> usize {
        self.docs.len()
    }

    pub fn doc_len(&self, doc: u64) -> Option<u32> {
        self.docs.get(&doc).copied()
    }
}

/// Several segments of one column, queried as one (the reference's
/// `ColumnIndexReader` over `IndexSegmentReader`s).
#[derive(Debug, Clone, Default)]
pub struct MemoryColumnIndex {
    segments: Vec<MemorySegment>,
}

impl MemoryColumnIndex {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn push(&mut self, segment: MemorySegment) {
        self.segments.push(segment);
    }

    fn doc_count(&self, column: &str) -> usize {
        self.segments
            .iter()
            .filter(|segment| segment.column == column)
            .map(MemorySegment::doc_count)
            .sum()
    }

    fn total_len(&self, column: &str) -> u64 {
        self.segments
            .iter()
            .filter(|segment| segment.column == column)
            .map(|segment| segment.total_len)
            .sum()
    }

    fn doc_lens(&self, column: &str) -> HashMap<u64, u32> {
        let mut lens = HashMap::new();
        for segment in &self.segments {
            if segment.column != column {
                continue;
            }
            for (doc, len) in &segment.docs {
                lens.insert(*doc, *len);
            }
        }
        lens
    }

    /// Per term: `(doc, tf, positions)` across the column's segments, ascending
    /// by doc.
    fn postings(&self, column: &str, term: &str) -> Vec<(u64, u32, Vec<u32>)> {
        let mut out = Vec::new();
        for segment in &self.segments {
            if segment.column != column {
                continue;
            }
            if let Some(list) = segment.postings.get(term) {
                out.extend_from_slice(list);
            }
        }
        out.sort_unstable_by_key(|(doc, _, _)| *doc);
        out
    }
}

impl TermLookup for MemoryColumnIndex {
    fn lookup(&self, column: &str, term: &str) -> Option<Box<dyn DocIterator>> {
        let postings = self.postings(column, term);
        if postings.is_empty() {
            return None;
        }
        let lens = self.doc_lens(column);
        let doc_lens: Vec<u32> = postings
            .iter()
            .map(|(doc, _, _)| lens.get(doc).copied().unwrap_or(1))
            .collect();
        let hits: Vec<(u64, u32)> = postings.into_iter().map(|(doc, tf, _)| (doc, tf)).collect();
        Some(Box::new(TermDocIterator::new(
            term,
            hits,
            doc_lens,
            self.doc_count(column),
            self.total_len(column),
            1.0,
            0.0,
        )))
    }

    fn lookup_phrase(
        &self,
        column: &str,
        terms: &[String],
        slop: u32,
    ) -> Option<Box<dyn DocIterator>> {
        let mut per_term = Vec::with_capacity(terms.len());
        for term in terms {
            let postings = self.postings(column, term);
            if postings.is_empty() {
                // `PhraseQueryNode::CreateSearch` fails when a term is absent.
                return None;
            }
            per_term.push(
                postings
                    .into_iter()
                    .map(|(doc, _, positions)| (doc, positions))
                    .collect(),
            );
        }
        Some(Box::new(PhraseDocIterator::new(
            terms.to_vec(),
            per_term,
            slop,
            self.doc_lens(column),
            self.doc_count(column),
            self.total_len(column),
            1.0,
            0.0,
        )))
    }

    fn bmw_search(&self, column: &str, terms: &[String], topn: usize) -> Option<Vec<(u64, f64)>> {
        let lens = self.doc_lens(column);
        let mut postings = HashMap::new();
        for term in terms {
            let hits: Vec<(u64, u32)> = self
                .postings(column, term)
                .into_iter()
                .map(|(doc, tf, _)| (doc, tf))
                .collect();
            if hits.is_empty() {
                // A missing term simply drops that OR branch.
                continue;
            }
            postings.insert(term.clone(), hits);
        }
        if postings.len() < 2 {
            return None;
        }
        let index = crate::MergedIndex {
            n_docs: self.doc_count(column),
            total_len: self.total_len(column),
            postings,
            doc_len: lens,
        };
        let weights = vec![1.0; terms.len()];
        let fields = vec![(index, weights)];
        let dead = std::collections::HashSet::new();
        let hits = crate::wand::BlockMaxWandIterator::new(
            &fields,
            terms,
            topn,
            &dead,
            0.0,
            1,
            crate::wand::WandMode::Bm25,
        )
        .run();
        Some(hits)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn lexer_matches_the_reference_tokens() {
        let tokens = lex("name:dune^1.2 AND NOT (dune god)^1.3").unwrap();
        assert_eq!(
            tokens,
            vec![
                Token::Text {
                    text: "name".into(),
                    quoted: false
                },
                Token::Colon,
                Token::Text {
                    text: "dune".into(),
                    quoted: false
                },
                Token::Caret,
                Token::And,
                Token::Not,
                Token::LParen,
                Token::Text {
                    text: "dune".into(),
                    quoted: false
                },
                Token::Text {
                    text: "god".into(),
                    quoted: false
                },
                Token::RParen,
                Token::Caret,
            ]
        );
        assert_eq!(
            lex("\"one of\"~2").unwrap(),
            vec![
                Token::Text {
                    text: "one of".into(),
                    quoted: true
                },
                Token::Tilde(2)
            ]
        );
        // `||` is a word, not an operator: the reference's invalid-not query.
        assert_eq!(
            lex("NOT (a || b)").unwrap(),
            vec![
                Token::Not,
                Token::LParen,
                Token::Text {
                    text: "a".into(),
                    quoted: false
                },
                Token::Text {
                    text: "||".into(),
                    quoted: false
                },
                Token::Text {
                    text: "b".into(),
                    quoted: false
                },
                Token::RParen
            ]
        );
        assert_eq!(lex("TO BE OR NOT TO BE").unwrap().len(), 6);
    }

    #[test]
    fn root_terms_keep_the_shortest_form_per_position() {
        // `tokenize_index_positions` emits `harmful` and `harm` at one offset.
        let terms = root_terms(tokenize_index_positions("harmful chemical", &None));
        assert_eq!(
            terms,
            vec![("harm".to_string(), 0), ("chemic".to_string(), 1)]
        );
    }

    #[test]
    fn parser_builds_the_expected_tree() {
        let driver = SearchDriver::new(HashMap::new(), "body");
        assert_eq!(
            driver.parse_single("dune god").unwrap(),
            QueryNode::Or(vec![
                QueryNode::Term(TermQueryNode {
                    column: "body".into(),
                    term: "dune".into()
                }),
                QueryNode::Term(TermQueryNode {
                    column: "body".into(),
                    term: "god".into()
                }),
            ])
        );
        assert_eq!(
            driver.parse_single("\"harmful chemical\"~2").unwrap(),
            QueryNode::Phrase(PhraseQueryNode {
                column: "body".into(),
                terms: vec!["harm".into(), "chemic".into()],
                slop: 2
            })
        );
        assert!(driver.parse_single("NOT a").is_ok());
        assert!(driver.parse_single("\"unterminated").is_err());
        assert!(lex("\"unterminated").is_err());
    }
}
