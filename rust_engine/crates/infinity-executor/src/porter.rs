//! C++ STEM_LANG_PORT: the original Porter stemmer.
//!
//! The reference does not use Snowball's English (Porter2) for -porter: it
//! runs stem_UTF_8_porter.cpp, the Snowball-generated C for
//! algorithms/porter.sbl (the 1980 algorithm, not the 2001 revision). The
//! regions (R1/R2), the pre-pass that uppercases the y's that act as
//! consonants, and the suffix tables below are transcribed from that file, so
//! a word and its stem agree with the reference byte for byte.

/// Stems a word the way porter_UTF_8_stem does.
///
/// The reference works on UTF-8 **bytes**: its character groups only name
/// ASCII letters, so a byte above 0x7f is a consonant here exactly as it is
/// there.
pub fn stem(word: &str) -> String {
    let mut s = Porter::new(word.as_bytes());
    s.run();
    String::from_utf8_lossy(&s.b).into_owned()
}

/// The reference's g_v group: the vowels plus y. Its bitmap only names ASCII
/// characters, so any other character - including every non-ASCII one the
/// tokenizer can hand over - is a consonant.
fn is_vowel(c: char) -> bool {
    matches!(c, 'a' | 'e' | 'i' | 'o' | 'u' | 'y')
}

/// The reference's g_v_WXY group: what shortv treats as not-a-consonant.
fn in_v_wxy(c: char) -> bool {
    matches!(c, 'a' | 'e' | 'i' | 'o' | 'u' | 'y' | 'w' | 'x' | 'Y')
}

struct Porter {
    b: Vec<u8>,
    /// R1: the position after the first non-vowel following a vowel.
    r1: usize,
    /// R2: the same, computed from R1 onwards.
    r2: usize,
}

impl Porter {
    fn new(word: &[u8]) -> Self {
        Self {
            b: word.to_vec(),
            r1: word.len(),
            r2: word.len(),
        }
    }

    fn run(&mut self) {
        // porter.sbl lines 115-118: a y is a consonant at the start of the
        // word and right after a vowel, so those are uppercased before the
        // regions are measured and every group test runs. The last block of
        // the generated function turns them back.
        // Y_found: the final block only runs when the pre-pass marked a y, so
        // a word that merely contains an uppercase Y keeps it.
        let mut y_found = false;
        if self.b.first() == Some(&b'y') {
            self.b[0] = b'Y';
            y_found = true;
        }
        let mut i = 0;
        while i < self.b.len() {
            let (c, len) = self.char_at(i);
            if is_vowel(c) && self.b.get(i + len) == Some(&b'y') {
                self.b[i + len] = b'Y';
                y_found = true;
                i += len + 1;
            } else {
                i += len;
            }
        }
        self.r1 = self.region(0);
        self.r2 = self.region(self.r1);
        self.step1a();
        self.step1b();
        self.step1c();
        self.step2();
        self.step3();
        self.step4();
        self.step5a();
        self.step5b();
        if y_found {
            for c in self.b.iter_mut() {
                if *c == b'Y' {
                    *c = b'y';
                }
            }
        }
    }

    /// Lines 121-124: the region at `from` starts after the first non-vowel
    /// that follows the first vowel at or after `from`; a word with no such
    /// pair keeps the region at its end.
    fn region(&self, from: usize) -> usize {
        let mut c = from;
        // gopast v: consume up to and including the first vowel.
        loop {
            if c >= self.b.len() {
                return self.b.len();
            }
            let (ch, len) = self.char_at(c);
            c += len;
            if is_vowel(ch) {
                break;
            }
        }
        // gopast non-v: consume the vowels after it, then the non-vowel.
        loop {
            if c >= self.b.len() {
                return self.b.len();
            }
            let (ch, len) = self.char_at(c);
            c += len;
            if !is_vowel(ch) {
                return c;
            }
        }
    }

    /// The character starting at `byte`. Every byte offset this algorithm
    /// produces - a suffix start, a region, the end of the stem - falls on a
    /// character boundary, because only ASCII suffixes are ever removed.
    fn char_at(&self, byte: usize) -> (char, usize) {
        match std::str::from_utf8(&self.b[byte..])
            .ok()
            .and_then(|s| s.chars().next())
        {
            Some(c) => (c, c.len_utf8()),
            None => ('\u{0}', 1),
        }
    }

    /// The character ending at `byte`, with its start offset.
    fn char_before(&self, byte: usize) -> Option<(char, usize)> {
        let c = std::str::from_utf8(&self.b[..byte]).ok()?.chars().next_back()?;
        Some((c, byte - c.len_utf8()))
    }

    fn ends_with(&self, s: &[u8]) -> bool {
        self.b.len() >= s.len() && &self.b[self.b.len() - s.len()..] == s
    }

    /// Is there a vowel before `end`? (The reference's backwards `gopast v`.)
    fn vowel_before(&self, end: usize) -> bool {
        std::str::from_utf8(&self.b[..end])
            .map(|s| s.chars().any(is_vowel))
            .unwrap_or(false)
    }

    /// r_shortv at `end`: consonant (not w/x/y), vowel, non-vowel read
    /// backwards from the end of the current stem.
    fn shortv(&self, end: usize) -> bool {
        let Some((last, before)) = self.char_before(end) else {
            return false;
        };
        if in_v_wxy(last) {
            return false;
        }
        let Some((mid, first_start)) = self.char_before(before) else {
            return false;
        };
        if !is_vowel(mid) {
            return false;
        }
        match self.char_before(first_start) {
            Some((first, _)) => !is_vowel(first),
            None => false,
        }
    }

    /// Step 1a (a_0): sses -> ss, ies -> i, ss stays, s is dropped.
    fn step1a(&mut self) {
        if self.ends_with(b"sses") {
            let n = self.b.len();
            self.b.truncate(n - 2);
        } else if self.ends_with(b"ies") {
            let n = self.b.len();
            self.b.truncate(n - 2);
        } else if self.ends_with(b"ss") {
            // In the table with no action.
        } else if self.ends_with(b"s") {
            let n = self.b.len();
            self.b.truncate(n - 1);
        }
    }

    /// Step 1b (a_2 and a_1): eed -> ee behind R1; ed/ing are dropped when the
    /// stem holds a vowel, which then re-examines the new ending.
    fn step1b(&mut self) {
        if self.ends_with(b"eed") {
            if self.b.len() - 3 >= self.r1 {
                let n = self.b.len();
                self.b.truncate(n - 1);
            }
            return;
        }
        let suffix = if self.ends_with(b"ed") {
            2
        } else if self.ends_with(b"ing") {
            3
        } else {
            return;
        };
        let start = self.b.len() - suffix;
        if !self.vowel_before(start) {
            return;
        }
        self.b.truncate(start);
        if self.ends_with(b"at") || self.ends_with(b"bl") || self.ends_with(b"iz") {
            self.b.push(b'e');
        } else if self.ends_with(b"bb")
            || self.ends_with(b"dd")
            || self.ends_with(b"ff")
            || self.ends_with(b"gg")
            || self.ends_with(b"mm")
            || self.ends_with(b"nn")
            || self.ends_with(b"pp")
            || self.ends_with(b"rr")
            || self.ends_with(b"tt")
        {
            self.b.pop();
        } else if self.b.len() == self.r1 && self.shortv(self.b.len()) {
            self.b.push(b'e');
        }
    }

    /// Step 1c: y (or a marked Y) becomes i when the stem holds a vowel.
    fn step1c(&mut self) {
        let n = self.b.len();
        if n == 0 {
            return;
        }
        if (self.b[n - 1] == b'y' || self.b[n - 1] == b'Y') && self.vowel_before(n - 1) {
            self.b[n - 1] = b'i';
        }
    }

    /// Step 2 (a_3): the derivational suffixes that end in a vowel + R1.
    fn step2(&mut self) {
        let table: &[(&[u8], &[u8])] = &[
            (b"ational", b"ate"),
            (b"tional", b"tion"),
            (b"enci", b"ence"),
            (b"anci", b"ance"),
            (b"izer", b"ize"),
            (b"abli", b"able"),
            (b"alli", b"al"),
            (b"entli", b"ent"),
            (b"eli", b"e"),
            (b"ousli", b"ous"),
            (b"ization", b"ize"),
            (b"ation", b"ate"),
            (b"ator", b"ate"),
            (b"alism", b"al"),
            (b"iveness", b"ive"),
            (b"fulness", b"ful"),
            (b"ousness", b"ous"),
            (b"aliti", b"al"),
            (b"iviti", b"ive"),
            (b"biliti", b"ble"),
        ];
        self.match_and_replace(table, self.r1);
    }

    /// Step 3 (a_4): the suffixes that leave a shorter stem behind R1.
    fn step3(&mut self) {
        let table: &[(&[u8], &[u8])] = &[
            (b"icate", b"ic"),
            (b"ative", b""),
            (b"alize", b"al"),
            (b"iciti", b"ic"),
            (b"ical", b"ic"),
            (b"ness", b""),
            (b"ful", b""),
        ];
        self.match_and_replace(table, self.r1);
    }

    /// Step 4 (a_5): drop a suffix outright when it starts behind R2. `ion`
    /// only counts after s or t.
    fn step4(&mut self) {
        let table: &[(&[u8], &[u8])] = &[
            (b"al", b""),
            (b"ance", b""),
            (b"ence", b""),
            (b"er", b""),
            (b"ic", b""),
            (b"able", b""),
            (b"ible", b""),
            (b"ant", b""),
            (b"ement", b""),
            (b"ment", b""),
            (b"ent", b""),
            (b"ism", b""),
            (b"ate", b""),
            (b"iti", b""),
            (b"ous", b""),
            (b"ive", b""),
            (b"ize", b""),
            (b"ou", b""),
        ];
        if self.ends_with(b"ion") {
            let start = self.b.len() - 3;
            if start >= self.r2 {
                if start >= 1 && matches!(self.b[start - 1], b's' | b't') {
                    self.b.truncate(start);
                }
                return;
            }
        }
        self.match_and_replace(table, self.r2);
    }

    /// The longest matching suffix of `table` is the one the reference's
    /// `find_among_b` picks; the region test is applied to that match alone,
    /// never to a shorter suffix that also fits.
    fn match_and_replace(&mut self, table: &[(&[u8], &[u8])], region: usize) {
        let mut best: Option<(&[u8], &[u8])> = None;
        for (from, to) in table {
            if self.ends_with(from) && best.map_or(true, |(b, _)| b.len() < from.len()) {
                best = Some((from, to));
            }
        }
        let Some((from, to)) = best else {
            return;
        };
        let start = self.b.len() - from.len();
        if start < region {
            return;
        }
        self.b.truncate(start);
        self.b.extend_from_slice(to);
    }

    /// Step 5a: drop a final e behind R2, or behind R1 when it does not leave a
    /// short syllable.
    fn step5a(&mut self) {
        if !self.ends_with(b"e") {
            return;
        }
        let start = self.b.len() - 1;
        if start >= self.r2 || (start >= self.r1 && !self.shortv(start)) {
            self.b.truncate(start);
        }
    }

    /// Step 5b: ll -> l behind R2.
    fn step5b(&mut self) {
        let n = self.b.len();
        if n >= 2 && self.b[n - 1] == b'l' && self.b[n - 2] == b'l' && n - 1 >= self.r2 {
            self.b.truncate(n - 1);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::stem;

    /// The vocabulary of Porter's paper: every pair is a rule the paper names,
    /// so a regression in one rule shows up as one failing word.
    const PAPER: &[(&str, &str)] = &[
        ("caresses", "caress"),
        ("ponies", "poni"),
        ("ties", "ti"),
        ("caress", "caress"),
        ("cats", "cat"),
        ("feed", "feed"),
        ("agreed", "agre"),
        ("plastered", "plaster"),
        ("bled", "bled"),
        ("motoring", "motor"),
        ("sing", "sing"),
        ("conflated", "conflat"),
        ("troubled", "troubl"),
        ("sized", "size"),
        ("hopping", "hop"),
        ("tanned", "tan"),
        ("falling", "fall"),
        ("hissing", "hiss"),
        ("fizzed", "fizz"),
        ("failing", "fail"),
        ("filing", "file"),
        ("happy", "happi"),
        ("sky", "sky"),
        ("relational", "relat"),
        ("conditional", "condit"),
        ("rational", "ration"),
        ("valenci", "valenc"),
        ("hesitanci", "hesit"),
        ("digitizer", "digit"),
        ("conformabli", "conform"),
        ("radicalli", "radic"),
        ("differentli", "differ"),
        ("vileli", "vile"),
        ("analogousli", "analog"),
        ("vietnamization", "vietnam"),
        ("predication", "predic"),
        ("operator", "oper"),
        ("feudalism", "feudal"),
        ("decisional", "decision"),
        ("hopefulness", "hope"),
        ("callousness", "callous"),
        ("formaliti", "formal"),
        ("sensitiviti", "sensit"),
        ("sensibiliti", "sensibl"),
        ("triplicate", "triplic"),
        ("formative", "form"),
        ("formalize", "formal"),
        ("electriciti", "electr"),
        ("electrical", "electr"),
        ("hopeful", "hope"),
        ("goodness", "good"),
        ("revival", "reviv"),
        ("allowance", "allow"),
        ("inference", "infer"),
        ("airliner", "airlin"),
        ("gyroscopic", "gyroscop"),
        ("adjustable", "adjust"),
        ("defensible", "defens"),
        ("irritant", "irrit"),
        ("replacement", "replac"),
        ("adjustment", "adjust"),
        ("dependent", "depend"),
        ("adoption", "adopt"),
        ("homologou", "homolog"),
        ("communism", "commun"),
        ("activate", "activ"),
        ("angulariti", "angular"),
        ("homologous", "homolog"),
        ("effective", "effect"),
        ("bowdlerize", "bowdler"),
        ("probate", "probat"),
        ("rate", "rate"),
        ("cease", "ceas"),
        ("controll", "control"),
        ("roll", "roll"),
    ];

    #[test]
    fn the_paper_vocabulary_stems_as_the_reference_does() {
        for (word, want) in PAPER {
            assert_eq!(&stem(word), want, "stem({word})");
        }
    }

    #[test]
    fn short_and_empty_words_stay_put() {
        assert_eq!(stem(""), "");
        assert_eq!(stem("a"), "a");
        assert_eq!(stem("as"), "a");
        assert_eq!(stem("is"), "i");
        assert_eq!(stem("y"), "y");
        assert_eq!(stem("by"), "by");
    }
}

