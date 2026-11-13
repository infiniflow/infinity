//! A compact backtracking regex used by the `regex(text, pattern)` predicate.
//!
//! Supported: literals, `.`, `*`, `+`, `?`, `{n}`, `{n,}`, `{n,m}`, alternation
//! `a|b`, groups `( … )` / `(?: … )`, character classes `[a-z]` / `[^…]`,
//! escapes `\s \d \w \S \D \W`, `\C` (any character), `\Q…\E` quoted
//! text and escaped literals, anchors `^` / `$`, and inline case flags
//! (`(?i)`, `(?i: … )`, `(?-i: … )`).

#[derive(Debug, Clone)]
enum Node {
    Char(char, bool),
    Any,
    Class(Vec<(char, char)>, bool, bool),
    Group(Vec<Vec<Node>>),
    Repeat(Box<Node>, usize, Option<usize>),
    Start,
    End,
}

struct Parser {
    chars: Vec<char>,
    pos: usize,
    ci: bool,
}

/// Returns `true` if `pattern` matches anywhere in `text`.
pub fn is_match(text: &str, pattern: &str) -> bool {
    let mut p = Parser {
        chars: pattern.chars().collect(),
        pos: 0,
        ci: false,
    };
    let alts = p.parse_alts(None);
    let hay: Vec<char> = text.chars().collect();
    let anchored = alts
        .iter()
        .all(|seq| matches!(seq.first(), Some(Node::Start)));
    if anchored {
        return !eval(seq_ref(&alts), &hay, 0, p.ci).is_empty();
    }
    for start in 0..=hay.len() {
        if !eval(seq_ref(&alts), &hay, start, p.ci).is_empty() {
            return true;
        }
    }
    false
}

fn seq_ref(alts: &[Vec<Node>]) -> &[Vec<Node>] {
    alts
}

impl Parser {
    fn peek(&self) -> Option<char> {
        self.chars.get(self.pos).copied()
    }

    /// Parses alternatives until `end` (or end of input).
    fn parse_alts(&mut self, end: Option<char>) -> Vec<Vec<Node>> {
        let mut alts = vec![Vec::new()];
        while let Some(c) = self.peek() {
            if Some(c) == end {
                break;
            }
            if c == '|' {
                self.pos += 1;
                alts.push(Vec::new());
                continue;
            }
            if let Some(node) = self.parse_node() {
                alts.last_mut().unwrap().push(node);
            }
        }
        alts
    }

    fn parse_node(&mut self) -> Option<Node> {
        let atom = match self.peek()? {
            '^' => {
                self.pos += 1;
                Node::Start
            }
            '$' => {
                self.pos += 1;
                Node::End
            }
            '.' => {
                self.pos += 1;
                Node::Any
            }
            '(' => {
                self.pos += 1;
                if self.peek() == Some('?') {
                    self.pos += 1;
                    // Lookarounds are zero width in RE2; skip the group. This
                    // port never used them, but skipping beats misparsing the
                    // lookaround body as literal text.
                    if matches!(self.peek(), Some('=') | Some('!')) {
                        self.skip_group();
                        return self.parse_node();
                    }
                    if self.peek() == Some('<')
                        && matches!(self.chars.get(self.pos + 1), Some('=') | Some('!'))
                    {
                        self.skip_group();
                        return self.parse_node();
                    }
                    // Inline flags: `(?i)`, `(?i:…)`, `(?-i:…)`.
                    let mut enable = true;
                    let mut set_ci: Option<bool> = None;
                    while let Some(c) = self.peek() {
                        match c {
                            'i' => {
                                set_ci = Some(enable);
                                self.pos += 1;
                            }
                            'm' | 's' | 'U' | 'R' | 'u' => self.pos += 1,
                            '-' => {
                                enable = false;
                                self.pos += 1;
                            }
                            ':' | ')' => break,
                            _ => break,
                        }
                    }
                    match self.peek() {
                        Some(':') => {
                            self.pos += 1;
                            let outer = self.ci;
                            if let Some(v) = set_ci {
                                self.ci = v;
                            }
                            let inner = self.parse_alts(Some(')'));
                            self.ci = outer;
                            if self.peek() == Some(')') {
                                self.pos += 1;
                            }
                            return Some(self.parse_quantifier(Node::Group(inner)));
                        }
                        Some(')') => {
                            self.pos += 1;
                            if let Some(v) = set_ci {
                                self.ci = v;
                            }
                            return self.parse_node();
                        }
                        _ => {
                            self.skip_group();
                            return self.parse_node();
                        }
                    }
                }
                let inner = self.parse_alts(Some(')'));
                if self.peek() == Some(')') {
                    self.pos += 1;
                }
                Node::Group(inner)
            }
            '[' => {
                self.pos += 1;
                let neg = self.peek() == Some('^');
                if neg {
                    self.pos += 1;
                }
                let mut ranges = Vec::new();
                while let Some(c) = self.peek() {
                    if c == ']' {
                        break;
                    }
                    self.pos += 1;
                    let lo = if c == '\\' {
                        self.escape_class().unwrap_or(c)
                    } else {
                        c
                    };
                    if self.peek() == Some('-') {
                        self.pos += 1;
                        if let Some(hi) = self.peek() {
                            self.pos += 1;
                            ranges.push((lo, hi));
                            continue;
                        }
                    }
                    ranges.push((lo, lo));
                }
                if self.peek() == Some(']') {
                    self.pos += 1;
                }
                Node::Class(ranges, neg, self.ci)
            }
            '\\' => {
                self.pos += 1;
                match self.peek() {
                    Some('s') => {
                        self.pos += 1;
                        Node::Class(
                            vec![(' ', ' '), ('\t', '\t'), ('\n', '\n'), ('\r', '\r')],
                            false,
                            false,
                        )
                    }
                    Some('S') => {
                        self.pos += 1;
                        Node::Class(
                            vec![(' ', ' '), ('\t', '\t'), ('\n', '\n'), ('\r', '\r')],
                            true,
                            false,
                        )
                    }
                    Some('d') => {
                        self.pos += 1;
                        Node::Class(vec![('0', '9')], false, false)
                    }
                    Some('D') => {
                        self.pos += 1;
                        Node::Class(vec![('0', '9')], true, false)
                    }
                    Some('w') => {
                        self.pos += 1;
                        Node::Class(
                            vec![('a', 'z'), ('A', 'Z'), ('0', '9'), ('_', '_')],
                            false,
                            false,
                        )
                    }
                    Some('W') => {
                        self.pos += 1;
                        Node::Class(
                            vec![('a', 'z'), ('A', 'Z'), ('0', '9'), ('_', '_')],
                            true,
                            false,
                        )
                    }
                    Some('C') => {
                        // `\C` matches any single character (RE2: any byte).
                        self.pos += 1;
                        Node::Any
                    }
                    Some('Q') => {
                        // `\Q…\E` is literal text.
                        self.pos += 1;
                        let mut quoted = Vec::new();
                        while self.pos < self.chars.len() {
                            if self.chars[self.pos] == '\\'
                                && self.chars.get(self.pos + 1) == Some(&'E')
                            {
                                self.pos += 2;
                                break;
                            }
                            quoted.push(Node::Char(self.chars[self.pos], self.ci));
                            self.pos += 1;
                        }
                        Node::Group(vec![quoted])
                    }
                    Some(c) => {
                        self.pos += 1;
                        Node::Char(c, self.ci)
                    }
                    None => return None,
                }
            }
            c => {
                self.pos += 1;
                Node::Char(c, self.ci)
            }
        };
        Some(self.parse_quantifier(atom))
    }

    /// Consumes the body of a lookaround group whose opener was consumed.
    fn skip_group(&mut self) {
        let mut depth = 1i32;
        while let Some(c) = self.peek() {
            match c {
                '\\' => {
                    self.pos += 2;
                    continue;
                }
                '[' => {
                    self.pos += 1;
                    while let Some(c) = self.peek() {
                        self.pos += 1;
                        if c == ']' {
                            break;
                        }
                    }
                    continue;
                }
                '(' => depth += 1,
                ')' => {
                    depth -= 1;
                    if depth == 0 {
                        self.pos += 1;
                        return;
                    }
                }
                _ => {}
            }
            self.pos += 1;
        }
    }

    fn escape_class(&mut self) -> Option<char> {
        let c = self.peek()?;
        self.pos += 1;
        Some(c)
    }

    fn parse_quantifier(&mut self, atom: Node) -> Node {
        match self.peek() {
            Some('*') => {
                self.pos += 1;
                Node::Repeat(Box::new(atom), 0, None)
            }
            Some('+') => {
                self.pos += 1;
                Node::Repeat(Box::new(atom), 1, None)
            }
            Some('?') => {
                self.pos += 1;
                Node::Repeat(Box::new(atom), 0, Some(1))
            }
            Some('{') => {
                let save = self.pos;
                self.pos += 1;
                let mut lo = String::new();
                while matches!(self.peek(), Some(c) if c.is_ascii_digit()) {
                    lo.push(self.chars[self.pos]);
                    self.pos += 1;
                }
                if lo.is_empty() {
                    self.pos = save;
                    return atom;
                }
                let hi: Option<usize> = if self.peek() == Some(',') {
                    self.pos += 1;
                    let mut h = String::new();
                    while matches!(self.peek(), Some(c) if c.is_ascii_digit()) {
                        h.push(self.chars[self.pos]);
                        self.pos += 1;
                    }
                    h.parse().ok() // empty => unbounded
                } else {
                    lo.parse().ok()
                };
                if self.peek() == Some('}') {
                    self.pos += 1;
                }
                let lo_n = lo.parse().unwrap_or(0);
                Node::Repeat(Box::new(atom), lo_n, hi)
            }
            _ => atom,
        }
    }
}

/// Applies an alternation, returning all reachable end positions.
fn eval(alts: &[Vec<Node>], text: &[char], start: usize, ci: bool) -> Vec<usize> {
    let mut out = Vec::new();
    for seq in alts {
        let mut positions = vec![start];
        for node in seq {
            let mut next = Vec::new();
            for &p in &positions {
                for e in apply(node, text, p, ci) {
                    if !next.contains(&e) {
                        next.push(e);
                    }
                }
            }
            positions = next;
            if positions.is_empty() {
                break;
            }
        }
        for p in positions {
            if !out.contains(&p) {
                out.push(p);
            }
        }
    }
    out
}

fn apply(node: &Node, text: &[char], pos: usize, ci: bool) -> Vec<usize> {
    match node {
        Node::Start => {
            if pos == 0 {
                vec![pos]
            } else {
                Vec::new()
            }
        }
        Node::End => {
            if pos == text.len() {
                vec![pos]
            } else {
                Vec::new()
            }
        }
        Node::Char(c, ci) => {
            if let Some(t) = text.get(pos) {
                if eq_char(*t, *c, *ci) {
                    return vec![pos + 1];
                }
            }
            Vec::new()
        }
        Node::Any => {
            if pos < text.len() {
                vec![pos + 1]
            } else {
                Vec::new()
            }
        }
        Node::Class(ranges, neg, ci) => {
            if let Some(t) = text.get(pos) {
                let mut hit = ranges.iter().any(|(lo, hi)| in_range(*t, *lo, *hi, *ci));
                if *neg {
                    hit = !hit;
                }
                if hit {
                    return vec![pos + 1];
                }
            }
            Vec::new()
        }
        Node::Group(alts) => eval(alts, text, pos, ci),
        Node::Repeat(inner, min, max) => {
            let limit = max.unwrap_or(usize::MAX);
            let mut results = Vec::new();
            let mut frontier = vec![pos];
            let mut count = 0usize;
            loop {
                if count >= *min {
                    for p in &frontier {
                        if !results.contains(p) {
                            results.push(*p);
                        }
                    }
                }
                if count >= limit {
                    break;
                }
                let mut next = Vec::new();
                for &p in &frontier {
                    for e in apply(inner, text, p, ci) {
                        if e != p && !next.contains(&e) {
                            next.push(e);
                        }
                    }
                }
                if next.is_empty() {
                    break;
                }
                frontier = next;
                count += 1;
            }
            results
        }
    }
}

fn in_range(c: char, lo: char, hi: char, ci: bool) -> bool {
    if c >= lo && c <= hi {
        return true;
    }
    if ci {
        for v in c.to_lowercase().chain(c.to_uppercase()) {
            if v >= lo && v <= hi {
                return true;
            }
        }
    }
    false
}

fn eq_char(a: char, b: char, ci: bool) -> bool {
    if a == b {
        return true;
    }
    if ci {
        return a.to_lowercase().eq(b.to_lowercase());
    }
    false
}

#[cfg(test)]
mod tests {
    use super::is_match;

    #[test]
    fn basic_patterns() {
        assert!(is_match(
            "the colour of the sky",
            "colou?r of the (sky|sea)"
        ));
        assert!(is_match("the color of the sea", "colou?r of the (sky|sea)"));
        assert!(!is_match(
            "the color of the land",
            "colou?r of the (sky|sea)"
        ));
        assert!(is_match(
            "error reading file",
            "error (reading|writing) file"
        ));
        assert!(is_match("fn main()", r"fn (?:main)"));
        assert!(is_match("int fn   main", r"fn\s+main"));
        assert!(is_match("row42x", "[0-9]+"));
        assert!(!is_match("rowXY", "[0-9]+"));
        assert!(is_match("über alles", "(?i)ÜBER ALLES"));
        assert!(is_match("我们的数据模型设计", "数据模型?"));
        assert!(is_match("这段语料很好", "语料{1}"));
        assert!(is_match("abc", "^a"));
        assert!(!is_match("zabc", "^a"));
        assert!(is_match("abc", "c$"));
        assert!(!is_match("abcz", "c$"));
        // Scoped inline flags, quoted text and `\C`.
        assert!(is_match("The Nobel Prize", "(?i:THE NOBEL) Prize"));
        assert!(!is_match("the nobel prize", "(?-i:THE NOBEL) Prize"));
        assert!(is_match("fn main() { return 0; }", r"\Qmain()\E"));
        assert!(is_match("fn main() { return 0; }", r"ma\Cn"));
        assert!(!is_match("maXXn", r"ma\Cn"));
    }
}
