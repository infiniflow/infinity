use super::*;

/// C++: `NGramAnalyzer` (`ngram_analyzer.cppm`, `ngram_analyzer_impl.cpp`).
///
/// Slides a window of `n` code points over each whitespace-delimited run; a run
/// shorter than `n` at the end of the input produces nothing.
#[derive(Debug, Clone)]
pub struct NGramAnalyzer {
    pub(crate) ngram: usize,
}

impl NGramAnalyzer {
    pub fn new(ngram: usize) -> Self {
        Self { ngram }
    }

    /// Streams the n-grams; see [`StandardAnalyzer::for_each_term`] for why the
    /// index path wants the text borrowed. An n-gram index emits one term per
    /// code point, so the per-term allocation this avoids is amplified.
    pub fn for_each_term(&self, input: &str, mut f: impl FnMut(TermView<'_>)) {
        let chars: Vec<(usize, char)> = input.char_indices().collect();
        let mut offset = 0u32;
        let mut i = 0usize;
        while i < chars.len() {
            if chars[i].1.is_whitespace() {
                i += 1;
                continue;
            }
            let mut end = i;
            let mut count = 0usize;
            let mut hit_space = false;
            while end < chars.len() && count < self.ngram {
                if chars[end].1.is_whitespace() {
                    hit_space = true;
                    break;
                }
                end += 1;
                count += 1;
            }
            if count < self.ngram {
                if hit_space {
                    i = end;
                    while i < chars.len() && chars[i].1.is_whitespace() {
                        i += 1;
                    }
                    continue;
                }
                break;
            }
            let start_byte = chars[i].0;
            let end_byte = chars.get(end).map(|(b, _)| *b).unwrap_or(input.len());
            f(TermView {
                text: &input[start_byte..end_byte],
                word_offset: offset,
                char_offset: start_byte as u32,
                end_offset: end_byte as u32,
            });
            offset += 1;
            i += 1;
        }
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
