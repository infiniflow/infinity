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
    /// query over several fields becomes an `OR` of one parse per field. Each
    /// field's own `^boost` multiplies its subtree (`MultiplyWeight`, pushed
    /// down to the leaves like `PushDownWeight`).
    pub fn parse_single_with_fields(
        &self,
        fields: &str,
        query: &str,
    ) -> Result<QueryNode, QueryParseError> {
        let fields = parse_fields(fields);
        match fields.len() {
            0 => self.parse_single(query),
            1 => {
                let (field, boost) = &fields[0];
                let node = self.parse_single_with_default(query, field)?;
                Ok(apply_boost(node, *boost))
            }
            _ => {
                let mut children = Vec::with_capacity(fields.len());
                for (field, boost) in fields {
                    if let Ok(node) = self.parse_single_with_default(query, &field) {
                        children.push(apply_boost(node, boost));
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
                        boost: 1.0,
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
                boost: 1.0,
            }));
        }
        if terms.len() == 1 {
            return Ok(QueryNode::Term(TermQueryNode {
                column: field,
                term: terms.into_iter().next().expect("length checked").0,
                boost: 1.0,
            }));
        }
        if from_quoted {
            return Ok(QueryNode::Phrase(PhraseQueryNode {
                column: field,
                terms: terms.into_iter().map(|(term, _)| term).collect(),
                slop,
                boost: 1.0,
            }));
        }
        Ok(QueryNode::Or(
            terms
                .into_iter()
                .map(|(term, _)| {
                    QueryNode::Term(TermQueryNode {
                        column: field.clone(),
                        term,
                        boost: 1.0,
                    })
                })
                .collect(),
        ))
    }
}

/// `ParseFields`: comma-separated `field^boost` entries.
fn parse_fields(fields: &str) -> Vec<(String, f32)> {
    if fields.is_empty() {
        return Vec::new();
    }
    fields
        .split(',')
        .filter(|field| !field.is_empty())
        .map(|field| match field.split_once('^') {
            Some((name, boost)) => (name.to_string(), boost.parse::<f32>().unwrap_or(1.0)),
            None => (field.to_string(), 1.0),
        })
        .collect()
}

/// `QueryNode::PushDownWeight`: multiplies a subtree by `factor`. Leaves keep
/// the weight; a multi-node propagates it to every child, exactly like the
/// reference's recursive push-down (its own weight is reset afterwards).
fn apply_boost(node: QueryNode, factor: f32) -> QueryNode {
    if factor == 1.0 {
        return node;
    }
    match node {
        QueryNode::Term(mut term) => {
            term.boost *= factor;
            QueryNode::Term(term)
        }
        QueryNode::Phrase(mut phrase) => {
            phrase.boost *= factor;
            QueryNode::Phrase(phrase)
        }
        QueryNode::And(children) => QueryNode::And(
            children
                .into_iter()
                .map(|child| apply_boost(child, factor))
                .collect(),
        ),
        QueryNode::Or(children) => QueryNode::Or(
            children
                .into_iter()
                .map(|child| apply_boost(child, factor))
                .collect(),
        ),
        QueryNode::Not(children) => QueryNode::Not(
            children
                .into_iter()
                .map(|child| apply_boost(child, factor))
                .collect(),
        ),
        QueryNode::AndNot(children) => QueryNode::AndNot(
            children
                .into_iter()
                .map(|child| apply_boost(child, factor))
                .collect(),
        ),
    }
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
    Caret(f32),
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
                Some((boost, next)) => {
                    tokens.push(Token::Caret(boost));
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
                    } else {
                        // A backslash before a non-escapable character: flex's
                        // default action echoes (drops) it, then scanning
                        // resumes with the next character. Advance so the
                        // scanner always makes progress.
                        i += 1;
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

    /// Consumes an optional `^boost` and returns its value.
    fn eat_caret(&mut self) -> Option<f32> {
        if let Some(Token::Caret(boost)) = self.peek() {
            let boost = *boost;
            self.position += 1;
            Some(boost)
        } else {
            None
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
            // `(...)^boost` multiplies the whole subtree.
            return Ok(match self.eat_caret() {
                Some(boost) => apply_boost(node, boost),
                None => node,
            });
        }
        let node = self.parse_basic_filter()?;
        // `term^boost`
        Ok(match self.eat_caret() {
            Some(boost) => apply_boost(node, boost),
            None => node,
        })
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
    fn lookup(&self, column: &str, term: &str, boost: f64) -> Option<Box<dyn DocIterator>> {
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
            boost,
            0.0,
        )))
    }

    fn lookup_phrase(
        &self,
        column: &str,
        terms: &[String],
        slop: u32,
        boost: f64,
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
            boost,
            0.0,
        )))
    }

    fn bmw_search(
        &self,
        column: &str,
        terms: &[(String, f64)],
        topn: usize,
    ) -> Option<Vec<(u64, f64)>> {
        let lens = self.doc_lens(column);
        let mut postings = HashMap::new();
        for (term, _) in terms {
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
        let weights: Vec<f64> = terms.iter().map(|(_, boost)| *boost).collect();
        let term_names: Vec<String> = terms.iter().map(|(term, _)| term.clone()).collect();
        let fields = vec![(index, weights)];
        let dead = std::collections::HashSet::new();
        let hits = crate::wand::BlockMaxWandIterator::new(
            &fields,
            &term_names,
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
                Token::Caret(1.2),
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
                Token::Caret(1.3),
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
                    term: "dune".into(),
                    boost: 1.0,
                }),
                QueryNode::Term(TermQueryNode {
                    column: "body".into(),
                    term: "god".into(),
                    boost: 1.0,
                }),
            ])
        );
        assert_eq!(
            driver.parse_single("\"harmful chemical\"~2").unwrap(),
            QueryNode::Phrase(PhraseQueryNode {
                column: "body".into(),
                terms: vec!["harm".into(), "chemic".into()],
                slop: 2,
                boost: 1.0,
            })
        );
        assert!(driver.parse_single("NOT a").is_ok());
        assert!(driver.parse_single("\"unterminated").is_err());
        assert!(lex("\"unterminated").is_err());
    }

    #[test]
    fn parser_carries_and_pushes_down_boosts() {
        let driver = SearchDriver::new(HashMap::new(), "body");
        let term = |term: &str, boost: f32| {
            QueryNode::Term(TermQueryNode {
                column: "body".into(),
                term: term.into(),
                boost,
            })
        };
        // `a^3 OR b`: the leaf keeps its own weight.
        assert_eq!(
            driver.parse_single("a^3 OR b").unwrap(),
            QueryNode::Or(vec![term("a", 3.0), term("b", 1.0)])
        );
        // `(a OR b^2)^3`: the group weight multiplies the children's own.
        assert_eq!(
            driver.parse_single("(a OR b^2)^3").unwrap(),
            QueryNode::Or(vec![term("a", 3.0), term("b", 6.0)])
        );
        // Phrase boost.
        assert_eq!(
            driver.parse_single("\"a b\"^2").unwrap(),
            QueryNode::Phrase(PhraseQueryNode {
                column: "body".into(),
                terms: vec!["a".into(), "b".into()],
                slop: 0,
                boost: 2.0
            })
        );
        // Field-list boost (`col^w`) scales the subtree too.
        assert_eq!(
            driver.parse_single_with_fields("body^2", "a").unwrap(),
            term("a", 2.0)
        );
    }

    /// C++ `ParseStream` (`search_driver_ut.cpp`): skip blank lines and
    /// comments, parse every remaining line, fail on the first bad one.
    fn parse_stream(driver: &SearchDriver, text: &str) -> Result<usize, QueryParseError> {
        let mut parsed = 0;
        for raw in text.lines() {
            let line = raw.trim_start_matches([' ', '\t']);
            if line.is_empty() || line.starts_with('#') {
                continue;
            }
            driver.parse_single(line)?;
            parsed += 1;
        }
        Ok(parsed)
    }

    // C++: src/unit_test/parser/search_driver_ut.cpp:good_test1
    #[test]
    fn search_driver_good_test1() {
        let queries = r##"
#basic_filter with implicit field
dune
123.456

#basic_filter with explicit field
name:dune
num:123.456

#basic_filter_boost with implicit field
dune^1.2
123.456^7.8

#basic_filter_boost with explicit field
name:dune^1.2
num:123.456^7.8

#term
NOT name:microsoft^1.2
-name:google^1.2
!name:microsoft^1.2
(name:star OR name:duna)
(dune god)
(dune god)^1.3
!(dune god)^1.3

#clause
foo AND name:duna^1.2
foo +bar
duna^2 AND foo
duna AND foo
(dune god) AND (foo bar)
_exists_:"author" AND page_count:xxx AND name:star^1.3

#query
dune god
dune OR god
name:star OR name:duna
_exists_:"author" AND page_count:yyy AND (name:star OR name:duna)
_exists_:"author" AND page_count:zzz^1.3 AND (name:star^0.1 OR name:duna^1.2)^1.2

#query
(dune^1.2 AND NOT name:god^2 AND NOT kddd:ss^4 OR ee:ff^1.2)^1.3
(dune^1.2 AND NOT (name:god^2 || kddd:ss^4) OR ee:ff^1.2)^1.3
(dune^1.2 AND (NOT name:god^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3

#escape reserved characters per https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-query-string-query.html
first\ name:Alice
book\*:quick
DS\-K3AJ303/Dm140
DS\-K3AJ303\/Dm140
\(DS\-K3AJ303\/Dm140\)

#quote
"first name":Alice
"book*":quick
"DS-K3AJ303/Dm140"

#sloppy phrase
"a b"
"a b"~0
"a b"~1
"a b"~2
"a b c"~4
    "##;

        let driver = SearchDriver::new(HashMap::new(), "body");
        assert_eq!(parse_stream(&driver, queries), Ok(42));
    }

    // C++: src/unit_test/parser/search_driver_ut.cpp:good_test2
    #[test]
    fn search_driver_good_test2() {
        let queries = r##"
#basic_filter with implicit field
芯片

#basic_filter with explicit field
name:芯片

#basic_filter_boost with implicit field
芯片^1.2

#basic_filter_boost with explicit field
name:芯片^1.2

#term
NOT name:microsoft^1.2
-name:芯片^1.2
!name:微软^1.2
(name:星空 OR name:邓肯)
(邓肯 上帝)
(邓肯 上帝)^1.3
!(邓肯 上帝)^1.3

#clause
foo AND name:邓肯^1.2
foo +bar
邓肯^2 AND foo
邓肯 AND foo
(邓肯 上帝) AND (foo bar)
_exists_:"author" AND page_count:xxx AND name:star^1.3

#query
邓肯 上帝
邓肯 OR 上帝
name:星空 OR name:邓肯
_exists_:"author" AND page_count:yyy AND (name:星空 OR name:邓肯)
_exists_:"author" AND page_count:zzz^1.3 AND (name:星空^0.1 OR name:邓肯^1.2)^1.2

#query
(邓肯^1.2 AND NOT name:上帝^2 AND NOT kddd:ss^4 OR ee:ff^1.2)^1.3
(邓肯^1.2 AND NOT (name:上帝^2 || kddd:ss^4) OR ee:ff^1.2)^1.3
(邓肯^1.2 AND (NOT name:上帝^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3

#quote
吉祥物“羽宝”头部
nanjing吉祥物"羽宝"头部head "DS-K3AJ303/Dm140"
吉祥物nanjing"DS-K3AJ303/Dm140"头部
graphic cards

#complex query
((互联网 OR "联网" OR ("联网"~2)^0.5)^0.37996928602305424 (服务)^0.37996928602305424 (文章)^0.13455624904801738 (系统)^0.10443628332697895 (\+)^0.0004189177488833171 (监管)^0.00020561620385239324 (提到)^0.0001805935548173221 (提供)^0.00013985485008990281 (主要)^0.00012391322125226468 ("文章 提到 互联网 \+ 监管 系统 主要 提供 服务"~4)^1.5)
    "##;

        let mut body = HashMap::new();
        body.insert("ft_index".to_string(), "chinese".to_string());
        let driver = SearchDriver::new(HashMap::from([("body".to_string(), body)]), "body");
        assert_eq!(parse_stream(&driver, queries), Ok(30));
    }

    // C++: src/unit_test/parser/search_driver_ut.cpp:operator_option_test
    #[test]
    fn search_driver_operator_option_test() {
        let queries = r##"
#basic_filter_boost with explicit field
name:芯片^1.2
name:dune^1.2
num:123.456^7.8

#clause
(邓肯 上帝) AND (foo bar)
_exists_:"author" AND page_count:xxx AND name:star^1.3

#query
(邓肯^1.2 AND (NOT name:上帝^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3

#quote
TO BE OR NOT TO BE
吉祥物“羽宝”头部
nanjing吉祥物"羽宝"头部head "DS-K3AJ303/Dm140"
吉祥物nanjing"DS-K3AJ303/Dm140"头部
graphic cards
    "##;

        let mut body = HashMap::new();
        body.insert("ft_index".to_string(), "chinese".to_string());
        for operator in [OperatorOption::Or, OperatorOption::And] {
            let driver =
                SearchDriver::new(HashMap::from([("body".to_string(), body.clone())]), "body")
                    .with_operator(operator);
            assert_eq!(parse_stream(&driver, queries), Ok(11), "{operator:?}");
        }
    }

    // C++: src/unit_test/parser/search_driver_ut.cpp:whitespace_analyzer_test
    #[test]
    fn search_driver_whitespace_analyzer_test() {
        let queries = r##"
#basic_filter_boost with explicit field
name:芯片^1.2
name:dune^1.2
num:123.456^7.8
label:DS-K3AJ303/Dm140^1.2
date:2025-01-01^1.2

#clause
DS-K3AJ303/Dm140^1.2 OR 2025-01-01^1.2
(邓肯 上帝) AND (foo bar)
_exists_:"author" AND page_count:xxx AND name:star^1.3

#quote
"TO BE OR NOT TO BE"
"nanjing吉祥物\"羽宝\"头部head" "DS-K3AJ303/Dm140"
"吉祥物nanjing\"DS-K3AJ303/Dm140\"头部"
    "##;

        let mut column2analyzer = HashMap::new();
        for column in ["name", "num", "label", "date", "_exists_", "body"] {
            column2analyzer.insert(
                column.to_string(),
                HashMap::from([("ft_index".to_string(), "whitespace".to_string())]),
            );
        }
        let driver = SearchDriver::new(column2analyzer, "body");
        assert_eq!(parse_stream(&driver, queries), Ok(11));
    }
}
