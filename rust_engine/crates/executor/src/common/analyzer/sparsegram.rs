use super::*;

/// C++: `SparsegramAnalyzer` (`sparsegram_analyzer_impl.cpp`) over
/// `ForEachSparseGram` (`common/utility/sparse_gram.h`).
#[derive(Debug, Clone, Copy)]
pub struct SparseGramAnalyzer {
    pub min_n: u32,
    pub max_n: u32,
    pub fold: bool,
}

impl SparseGramAnalyzer {
    /// C++: `ParseSparseGramAnalyzerName`.
    pub fn from_name(name: &str) -> Option<Self> {
        crate::regex_literal::parse_sparsegram_analyzer(name).map(|(min_n, max_n, fold)| Self {
            min_n,
            max_n,
            fold,
        })
    }

    /// Streams the grams: `for_each_sparse_gram` already produces them by
    /// reference, so the index path interns them without a `String` each.
    pub fn for_each_term(&self, input: &str, mut f: impl FnMut(TermView<'_>)) {
        let mut offset = 0u32;
        for_each_sparse_gram(input, self.min_n, self.max_n, self.fold, |gram| {
            f(TermView {
                text: gram,
                word_offset: offset,
                char_offset: 0,
                end_offset: offset + 1,
            });
            offset += 1;
        });
    }

    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut out = Vec::new();
        self.for_each_term(input, |term| {
            out.push(Term {
                text: term.text.to_string(),
                word_offset: term.word_offset,
                char_offset: term.char_offset,
                end_offset: term.end_offset,
                is_index: false,
                payload: 0,
            })
        });
        out
    }
}

/// C++: `SparseGramBigramWeight` — splitmix64 over the two (ASCII-folded)
/// code points.
pub(crate) fn sparse_gram_bigram_weight(first: char, second: char) -> u64 {
    fn fold(c: char) -> u32 {
        if c.is_ascii_uppercase() {
            (c as u32) - ('A' as u32) + ('a' as u32)
        } else {
            c as u32
        }
    }
    let mut x = ((fold(first) as u64) << 32) | fold(second) as u64;
    x = x.wrapping_add(0x9E37_79B9_7F4A_7C15);
    x = (x ^ (x >> 30)).wrapping_mul(0xBF58_476D_1CE4_E5B9);
    x = (x ^ (x >> 27)).wrapping_mul(0x94D0_49BB_1331_11EB);
    x ^ (x >> 31)
}

/// C++: `ForEachSparseGram` — emits content-defined grams in
/// `[min_n, max_n]` code points, plus the one/two character dense floors.
pub(crate) fn for_each_sparse_gram(
    text: &str,
    min_n: u32,
    max_n: u32,
    fold: bool,
    mut emit: impl FnMut(&str),
) {
    use crate::regex_literal::is_dense_code_point;

    if min_n == 0 || max_n < min_n || text.is_empty() {
        return;
    }
    let chars: Vec<(usize, char)> = text.char_indices().collect();
    let n = chars.len();
    let offset = |index: usize| chars.get(index).map_or(text.len(), |(at, _)| *at);

    let emit_window = |begin: usize, end: usize, fold: bool, emit: &mut dyn FnMut(&str)| {
        let range = offset(begin)..offset(end);
        let window = &text[range.clone()];
        if window.as_bytes().contains(&0) {
            return;
        }
        emit(window);
        if !fold {
            return;
        }
        if window.bytes().any(|b| b.is_ascii_uppercase()) {
            emit(&window.to_ascii_lowercase());
        }
    };

    let selected = |begin: usize, end: usize| -> bool {
        let length = end - begin;
        if length < 3 {
            return false;
        }
        let all_dense = chars[begin..end]
            .iter()
            .all(|(_, c)| is_dense_code_point(*c as u32));
        let effective_min = if all_dense { min_n.max(4) } else { min_n };
        if (length as u32) < effective_min || (length as u32) > max_n {
            return false;
        }
        let head = sparse_gram_bigram_weight(chars[begin].1, chars[begin + 1].1);
        let tail = sparse_gram_bigram_weight(chars[end - 2].1, chars[end - 1].1);
        let weaker = head.min(tail);
        for index in begin + 1..end - 2 {
            if sparse_gram_bigram_weight(chars[index].1, chars[index + 1].1) > weaker {
                return false;
            }
        }
        true
    };

    for begin in 0..n {
        if is_dense_code_point(chars[begin].1 as u32) {
            emit_window(begin, begin + 1, fold, &mut emit);
            if begin + 1 < n && is_dense_code_point(chars[begin + 1].1 as u32) {
                emit_window(begin, begin + 2, fold, &mut emit);
            }
        }
        let limit = (begin + max_n as usize).min(n);
        for end in begin + 3..=limit {
            if selected(begin, end) {
                emit_window(begin, end, fold, &mut emit);
            }
        }
    }
}
