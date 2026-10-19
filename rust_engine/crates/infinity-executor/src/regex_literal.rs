//! Mandatory literal extraction from a `regex()` pattern.
//!
//! Port of the reference's `ExtractRegexLiteralRuns`
//! (`src/planner/optimizer/regex_literal_extractor.h`): the returned runs are
//! substrings every match must contain, so a full-text index can narrow the
//! candidates while the exact regular expression still verifies them. The
//! extractor only under-approximates: anything it cannot prove mandatory ends
//! the current run or abandons the pattern.

/// C++ `SPARSE_GRAM_DEFAULT_MIN`.
pub const SPARSE_GRAM_DEFAULT_MIN: u32 = 3;
/// C++ `SPARSE_GRAM_DEFAULT_MAX`.
pub const SPARSE_GRAM_DEFAULT_MAX: u32 = 12;
/// C++ `SPARSE_GRAM_DENSE_SPARSE_MIN`.
const SPARSE_GRAM_DENSE_SPARSE_MIN: u32 = 4;

#[derive(Clone, Copy)]
struct GroupFrame {
    runs_begin: usize,
    has_alternation: bool,
    case_insensitive: bool,
}

/// C++ `IsDenseCodePoint`: scripts whose single code point is already dense.
pub fn is_dense_code_point(cp: u32) -> bool {
    (0x3040..=0x30FF).contains(&cp)     // Hiragana, Katakana
        || (0x3400..=0x4DBF).contains(&cp) // CJK Ext. A
        || (0x4E00..=0x9FFF).contains(&cp) // CJK Unified
        || (0xAC00..=0xD7AF).contains(&cp) // Hangul
        || (0xF900..=0xFAFF).contains(&cp) // CJK Compatibility
        || (0x20000..=0x2FA1F).contains(&cp) // CJK Ext. B..F
}

fn is_unfoldable_cased(cp: u32) -> bool {
    cp >= 0x80 && !is_dense_code_point(cp)
}

/// C++ `SkipClass`: index just past the `]` closing the class at `open`.
fn skip_class(p: &[u8], open: usize) -> Option<usize> {
    let mut i = open + 1;
    if i < p.len() && p[i] == b'^' {
        i += 1;
    }
    if i < p.len() && p[i] == b']' {
        i += 1;
    }
    let mut depth = 1i32;
    while i < p.len() {
        match p[i] {
            b'\\' => {
                i += 2;
                continue;
            }
            b'[' => depth += 1,
            b']' => {
                depth -= 1;
                if depth == 0 {
                    return Some(i + 1);
                }
            }
            _ => {}
        }
        i += 1;
    }
    None
}

/// C++ `RepetitionEnd`: `(end, min_repeat)` for `{n}`, `{n,}` or `{n,m}`.
fn repetition_end(p: &[u8], open: usize) -> Option<(usize, u32)> {
    let mut i = open + 1;
    let digits = |i: &mut usize| -> Option<u32> {
        let start = *i;
        let mut value = 0u32;
        while *i < p.len() && p[*i].is_ascii_digit() {
            value = value * 10 + u32::from(p[*i] - b'0');
            *i += 1;
        }
        (*i > start).then_some(value)
    };
    let lower = digits(&mut i)?;
    if i < p.len() && p[i] == b',' {
        i += 1;
        let _ = digits(&mut i);
    }
    if i >= p.len() || p[i] != b'}' {
        return None;
    }
    Some((i + 1, lower))
}

/// The literal runs a pattern requires, in order of first appearance.
///
/// `allow_folded_index` mirrors the reference: a case-insensitive run can only
/// be required when the index stores ASCII-folded grams.
pub fn extract_regex_literal_runs(
    pattern: &str,
    case_insensitive: bool,
    allow_folded_index: bool,
) -> Vec<String> {
    Extractor::new(pattern.as_bytes(), case_insensitive, allow_folded_index).run()
}

struct Extractor<'a> {
    p: &'a [u8],
    allow_fold: bool,
    frames: Vec<GroupFrame>,
    collected: Vec<String>,
    run: String,
    ci: bool,
    last_atom_len: usize,
    run_open: bool,
    last_atom_is_group: bool,
    last_group_runs_begin: usize,
    after_quantifier: bool,
    top_level_alternation: bool,
}

impl<'a> Extractor<'a> {
    fn new(p: &'a [u8], case_insensitive: bool, allow_folded_index: bool) -> Self {
        Self {
            p,
            allow_fold: allow_folded_index,
            frames: Vec::new(),
            collected: Vec::new(),
            run: String::new(),
            ci: case_insensitive,
            last_atom_len: 0,
            run_open: false,
            last_atom_is_group: false,
            last_group_runs_begin: 0,
            after_quantifier: false,
            top_level_alternation: false,
        }
    }

    fn flush(&mut self) {
        if !self.run.is_empty() {
            if self.ci && !self.allow_fold {
                // The index has no folded terms; nothing in this run can be
                // required without risking a missed match.
            } else if !self.ci {
                self.collected.push(std::mem::take(&mut self.run));
            } else {
                // Keep only what ASCII folding can answer for: caseless code
                // points stay verbatim, cased non-ASCII splits the run.
                let text = std::mem::take(&mut self.run);
                let mut piece = String::new();
                for ch in text.chars() {
                    if is_unfoldable_cased(ch as u32) {
                        if !piece.is_empty() {
                            self.collected.push(std::mem::take(&mut piece));
                        }
                    } else if ch.is_ascii_uppercase() {
                        piece.push(ch.to_ascii_lowercase());
                    } else {
                        piece.push(ch);
                    }
                }
                if !piece.is_empty() {
                    self.collected.push(piece);
                }
            }
            self.run.clear();
        }
        self.run_open = false;
        self.last_atom_len = 0;
        self.last_atom_is_group = false;
    }

    fn push_atom(&mut self, text: &[u8]) {
        // Only ASCII escapes reach here; literal code points are pushed by
        // `push_char`.
        for &b in text {
            self.run.push(b as char);
        }
        self.last_atom_len = text.len();
        self.run_open = true;
        self.last_atom_is_group = false;
    }

    fn push_char(&mut self, ch: char) {
        self.run.push(ch);
        self.last_atom_len = ch.len_utf8();
        self.run_open = true;
        self.last_atom_is_group = false;
    }

    fn bail(&self) -> Vec<String> {
        Vec::new()
    }

    fn run(mut self) -> Vec<String> {
        let p = self.p;
        let mut i = 0usize;
        while i < p.len() {
            match p[i] {
                b'\\' => {
                    if i + 1 >= p.len() {
                        return self.bail();
                    }
                    let next = p[i + 1];
                    match next {
                        b'd' | b'D' | b'w' | b'W' | b's' | b'S' | b'b' | b'B' | b'A' | b'z'
                        | b'Z' => {
                            self.flush();
                            i += 2;
                            self.after_quantifier = false;
                            continue;
                        }
                        b'1'..=b'9' | b'k' | b'g' | b'K' | b'G' => return self.bail(),
                        b'x' => {
                            let hex = |c: u8| -> Option<u32> {
                                match c {
                                    b'0'..=b'9' => Some(u32::from(c - b'0')),
                                    b'a'..=b'f' => Some(u32::from(c - b'a' + 10)),
                                    b'A'..=b'F' => Some(u32::from(c - b'A' + 10)),
                                    _ => None,
                                }
                            };
                            if i + 3 < p.len() {
                                if let (Some(hi), Some(lo)) = (hex(p[i + 2]), hex(p[i + 3])) {
                                    let value = hi * 16 + lo;
                                    if value < 0x80 {
                                        self.push_atom(&[value as u8]);
                                        i += 4;
                                        self.after_quantifier = false;
                                        continue;
                                    }
                                }
                            }
                            return self.bail();
                        }
                        b'C' => {
                            // `\C` matches any single byte, like `.`.
                            self.flush();
                            i += 2;
                            self.after_quantifier = false;
                            continue;
                        }
                        b'Q' | b'E' | b'u' | b'p' | b'P' => return self.bail(),
                        b'n' | b'r' | b't' | b'f' | b'v' | b'a' => {
                            let value = match next {
                                b'n' => b'\n',
                                b'r' => b'\r',
                                b't' => b'\t',
                                b'f' => 0x0c,
                                b'v' => 0x0b,
                                _ => 0x07,
                            };
                            self.push_atom(&[value]);
                            i += 2;
                            self.after_quantifier = false;
                            continue;
                        }
                        b'0' => return self.bail(),
                        _ => {
                            self.push_atom(&[next]);
                            i += 2;
                            self.after_quantifier = false;
                            continue;
                        }
                    }
                }
                b'[' => {
                    let Some(end) = skip_class(p, i) else {
                        return self.bail();
                    };
                    self.flush();
                    i = end;
                    self.after_quantifier = false;
                    continue;
                }
                b'(' => {
                    let mut group_open_len = 1usize;
                    if i + 1 < p.len() && p[i + 1] == b'?' {
                        if i + 2 >= p.len() {
                            return self.bail();
                        }
                        let kind = p[i + 2];
                        if kind == b'=' || kind == b'!' {
                            // Lookahead: zero width, nothing inside is collected.
                            let mut depth = 1i32;
                            let mut j = i + 2;
                            while j < p.len() && depth > 0 {
                                match p[j] {
                                    b'\\' => {
                                        j += 2;
                                        continue;
                                    }
                                    b'[' => {
                                        let Some(end) = skip_class(p, j) else {
                                            return self.bail();
                                        };
                                        j = end;
                                        continue;
                                    }
                                    b'(' => depth += 1,
                                    b')' => depth -= 1,
                                    _ => {}
                                }
                                j += 1;
                            }
                            if depth != 0 {
                                return self.bail();
                            }
                            self.flush();
                            i = j;
                            self.after_quantifier = false;
                            continue;
                        }
                        if kind == b'<' {
                            if i + 3 < p.len() && (p[i + 3] == b'=' || p[i + 3] == b'!') {
                                // Lookbehind: same reasoning as lookahead.
                                let mut depth = 1i32;
                                let mut j = i + 3;
                                while j < p.len() && depth > 0 {
                                    match p[j] {
                                        b'\\' => {
                                            j += 2;
                                            continue;
                                        }
                                        b'(' => depth += 1,
                                        b')' => depth -= 1,
                                        _ => {}
                                    }
                                    j += 1;
                                }
                                if depth != 0 {
                                    return self.bail();
                                }
                                self.flush();
                                i = j;
                                self.after_quantifier = false;
                                continue;
                            }
                            return self.bail();
                        }
                        if kind == b':' || kind == b'>' {
                            group_open_len = 3;
                        } else if matches!(
                            kind,
                            b'i' | b'm' | b's' | b'u' | b'U' | b'R' | b'-' | b'x'
                        ) {
                            // Inline flags: `x` changes how the rest is lexed.
                            let mut j = i + 2;
                            let mut enable = true;
                            let mut saw_x = false;
                            while j < p.len() && p[j] != b')' && p[j] != b':' {
                                match p[j] {
                                    b'-' => enable = false,
                                    b'x' => saw_x = true,
                                    b'i' | b'm' | b's' | b'u' | b'U' | b'R' => {}
                                    _ => return self.bail(),
                                }
                                j += 1;
                            }
                            if j >= p.len() {
                                return self.bail();
                            }
                            if saw_x {
                                return self.bail();
                            }
                            self.flush();
                            let scoped = p[j] == b':';
                            if scoped {
                                self.frames.push(GroupFrame {
                                    runs_begin: self.collected.len(),
                                    has_alternation: false,
                                    case_insensitive: self.ci,
                                });
                            }
                            let mut sets_case = false;
                            let mut case_value = false;
                            for &c in &p[i + 2..j] {
                                if c == b'i' {
                                    sets_case = true;
                                    case_value = enable;
                                }
                            }
                            if sets_case {
                                self.ci = case_value;
                            }
                            i = j + 1;
                            self.after_quantifier = false;
                            continue;
                        } else {
                            return self.bail();
                        }
                    }
                    self.flush();
                    self.frames.push(GroupFrame {
                        runs_begin: self.collected.len(),
                        has_alternation: false,
                        case_insensitive: self.ci,
                    });
                    i += group_open_len;
                    self.after_quantifier = false;
                    continue;
                }
                b')' => {
                    if let Some(frame) = self.frames.pop() {
                        self.flush();
                        self.ci = frame.case_insensitive;
                        if frame.has_alternation {
                            self.collected.truncate(frame.runs_begin);
                        }
                        self.last_atom_is_group = true;
                        self.last_group_runs_begin = frame.runs_begin;
                    }
                    i += 1;
                    self.after_quantifier = false;
                    continue;
                }
                b'|' => {
                    self.flush();
                    if let Some(frame) = self.frames.last_mut() {
                        frame.has_alternation = true;
                    } else {
                        self.collected.clear();
                        self.top_level_alternation = true;
                    }
                    i += 1;
                    self.after_quantifier = false;
                    continue;
                }
                b'*' | b'?' | b'+' | b'{' => {
                    let ch = p[i];
                    if self.after_quantifier && ch == b'?' {
                        i += 1;
                        continue;
                    }
                    if self.after_quantifier && ch == b'+' {
                        return self.bail();
                    }
                    let mut min_repeat = 1u32;
                    let mut next = i + 1;
                    if ch == b'{' {
                        match repetition_end(p, i) {
                            Some((end, lower)) => {
                                min_repeat = lower;
                                next = end;
                            }
                            None => {
                                self.push_atom(b"{");
                                i += 1;
                                self.after_quantifier = false;
                                continue;
                            }
                        }
                    }
                    if ch == b'*' || ch == b'?' {
                        min_repeat = 0;
                    }
                    if min_repeat == 0 {
                        if self.last_atom_is_group {
                            if self.collected.len() > self.last_group_runs_begin {
                                self.collected.truncate(self.last_group_runs_begin);
                            }
                        } else if self.run_open && self.last_atom_len > 0 {
                            let keep = self.run.len().saturating_sub(self.last_atom_len);
                            self.run.truncate(keep);
                        }
                    }
                    self.flush();
                    i = next;
                    self.after_quantifier = true;
                    continue;
                }
                b'.' | b'^' | b'$' => {
                    self.flush();
                    i += 1;
                    self.after_quantifier = false;
                    continue;
                }
                b'}' | b']' => {
                    self.push_atom(&[p[i]]);
                    i += 1;
                    self.after_quantifier = false;
                    continue;
                }
                _ => {
                    let Some(ch) = std::str::from_utf8(&p[i..])
                        .ok()
                        .and_then(|s| s.chars().next())
                    else {
                        return self.bail();
                    };
                    self.push_char(ch);
                    i += ch.len_utf8();
                    self.after_quantifier = false;
                    continue;
                }
            }
        }
        if !self.frames.is_empty() {
            return self.bail();
        }
        self.flush();
        if self.top_level_alternation {
            return self.bail();
        }
        std::mem::take(&mut self.collected)
    }
}

/// C++ `HasAnySparseGram`: whether the analyzer would emit a gram for `text`.
///
/// The boundary-bigram selection only narrows the windows further, so the
/// length/script test is a safe over-approximation: a run it accepts may still
/// produce no gram, but one it rejects never does.
pub fn has_any_sparse_gram(text: &str, min_n: u32, max_n: u32) -> bool {
    let chars: Vec<char> = text.chars().collect();
    if chars.is_empty() {
        return false;
    }
    let all_dense = chars.iter().all(|c| is_dense_code_point(*c as u32));
    let effective_min = if all_dense {
        min_n.max(SPARSE_GRAM_DENSE_SPARSE_MIN)
    } else {
        min_n
    };
    let len = chars.len() as u32;
    len >= effective_min && len <= max_n
}

/// Parse `sparsegram[-<min>[-<max>]][-fold]`.
pub fn parse_sparsegram_analyzer(name: &str) -> Option<(u32, u32, bool)> {
    let name = name.trim().to_ascii_lowercase();
    let rest = name.strip_prefix("sparsegram")?;
    let mut min_n = SPARSE_GRAM_DEFAULT_MIN;
    let mut max_n = SPARSE_GRAM_DEFAULT_MAX;
    let mut fold = false;
    let mut numbers = 0usize;
    for part in rest.split('-').filter(|p| !p.is_empty()) {
        if part == "fold" {
            fold = true;
        } else if let Ok(value) = part.parse::<u32>() {
            match numbers {
                0 => min_n = value,
                1 => max_n = value,
                _ => return None,
            }
            numbers += 1;
        } else {
            return None;
        }
    }
    Some((min_n, max_n, fold))
}

#[cfg(test)]
mod tests {
    use super::*;

    fn runs(p: &str, fold: bool) -> Vec<String> {
        extract_regex_literal_runs(p, false, fold)
    }

    /// Port of `regex_literal_extractor_ut.cpp`: the mandatory-run cases that
    /// the regex index rewrite relies on.
    #[test]
    fn extracts_only_mandatory_runs() {
        assert!(runs("Nobel|nothing relevant", false).is_empty());
        assert!(runs("a|bcd|efg", false).is_empty());
        assert_eq!(runs("(?:Nobel|nothing) Prize", false), [" Prize"]);
        assert_eq!(runs("(?i:THE NOBEL) Prize", false), [" Prize"]);
        assert_eq!(runs("(?i:THE NOBEL) Prize", true), ["the nobel", " Prize"]);
        assert!(runs(r"\Qmain()\E", false).is_empty());
        assert_eq!(runs(r"ma\Cn", false), ["ma", "n"]);
        assert_eq!(runs(r"x\ay", false), ["x\u{7}y"]);
        assert_eq!(
            runs("colou?r of the (sky|sea)", false),
            ["colo", "r of the "]
        );
        assert_eq!(
            runs("error (reading|writing) file", false),
            ["error ", " file"]
        );
        assert_eq!(runs("fn (?:main)", false), ["fn ", "main"]);
        assert_eq!(runs("error\\Creading", false), ["error", "reading"]);
    }

    #[test]
    fn sparsegram_analyzer_names() {
        assert_eq!(
            parse_sparsegram_analyzer("sparsegram-3-12"),
            Some((3, 12, false))
        );
        assert_eq!(
            parse_sparsegram_analyzer("sparsegram-3-12-fold"),
            Some((3, 12, true))
        );
        assert_eq!(
            parse_sparsegram_analyzer("sparsegram"),
            Some((3, 12, false))
        );
        assert_eq!(parse_sparsegram_analyzer("jieba"), None);
    }

    #[test]
    fn gram_floor_follows_the_script() {
        assert!(!has_any_sparse_gram("ab", 3, 12));
        assert!(has_any_sparse_gram("abc", 3, 12));
        // Dense scripts need four code points.
        assert!(!has_any_sparse_gram("数据模", 3, 12));
        assert!(has_any_sparse_gram("数据模型", 3, 12));
    }
}
