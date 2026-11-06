use super::*;

/// C++ `INFINITY_INT` (`opencc/utils.h`).
pub(crate) const OPENCC_INFINITY: u64 = 0x7fff_ffff;

/// C++ `DATRIE_UNUSED` (`opencc/dictionary/datrie.h`).
pub(crate) const OPENCC_DATRIE_UNUSED: i32 = -1;

/// C++ `datrie_dictionary` — the `OPENCCDATRIE` binary dictionary format.
pub(crate) struct OpenCcDict {
    pub(crate) items: Vec<OpenCcItem>,
    pub(crate) lexicon: Vec<u32>,
    pub(crate) lexicon_index: Vec<u32>,
    pub(crate) lexicon_count: usize,
}

impl OpenCcDict {
    pub(crate) fn parse(bytes: &[u8]) -> Result<Self, String> {
        const HEADER: &[u8] = b"OPENCCDATRIE";
        if !bytes.starts_with(HEADER) {
            return Err("not an OpenCC DATRIE dictionary".to_string());
        }
        let mut offset = HEADER.len();
        let lexicon_length = mecab_u32(bytes, offset)? as usize;
        offset += 4;
        let lexicon = bytes
            .get(offset..offset + lexicon_length * 4)
            .ok_or_else(|| "truncated OpenCC lexicon".to_string())?
            .chunks_exact(4)
            .map(|c| u32::from_le_bytes([c[0], c[1], c[2], c[3]]))
            .collect::<Vec<_>>();
        offset += lexicon_length * 4;
        let index_length = mecab_u32(bytes, offset)? as usize;
        offset += 4;
        let lexicon_index = bytes
            .get(offset..offset + index_length * 4)
            .ok_or_else(|| "truncated OpenCC lexicon index".to_string())?
            .chunks_exact(4)
            .map(|c| u32::from_le_bytes([c[0], c[1], c[2], c[3]]))
            .collect::<Vec<_>>();
        offset += index_length * 4;
        let lexicon_count = mecab_u32(bytes, offset)? as usize;
        offset += 4;
        let item_count = mecab_u32(bytes, offset)? as usize;
        offset += 4;
        let items = bytes
            .get(offset..offset + item_count * 12)
            .ok_or_else(|| "truncated OpenCC DATRIE".to_string())?
            .chunks_exact(12)
            .map(|c| OpenCcItem {
                base: i32::from_le_bytes([c[0], c[1], c[2], c[3]]),
                parent: i32::from_le_bytes([c[4], c[5], c[6], c[7]]),
                word: i32::from_le_bytes([c[8], c[9], c[10], c[11]]),
            })
            .collect();
        Ok(Self {
            items,
            lexicon,
            lexicon_index,
            lexicon_count,
        })
    }

    /// C++ `datrie_match` — returns the consumed prefix length and the trie
    /// item it stopped on.
    pub(crate) fn dat_match(&self, word: &[u32], limit: usize) -> (usize, usize) {
        let mut i = 0usize;
        let mut p = 0usize;
        while p < word.len()
            && (limit == 0 || p < limit)
            && self.items[i].base != OPENCC_DATRIE_UNUSED
        {
            let j = self.items[i].base as i64 + i64::from(word[p]);
            if j < 0 || j as usize >= self.items.len() {
                break;
            }
            let j = j as usize;
            if self.items[j].parent != i as i32 {
                break;
            }
            i = j;
            p += 1;
        }
        (p, i)
    }

    /// The conversion string of `word`-th entry, or `None`.
    pub(crate) fn lexicon_entry(&self, word: i32) -> Option<&[u32]> {
        if word < 0 || word as usize >= self.lexicon_count {
            return None;
        }
        let mut first = 0usize;
        for index in 0..=word as usize {
            let mut end = first;
            while end < self.lexicon_index.len() && self.lexicon_index[end] != u32::MAX {
                end += 1;
            }
            if index == word as usize {
                let start = *self.lexicon_index.get(first)? as usize;
                let len = end - first;
                return self.lexicon.get(start..start + len);
            }
            first = end + 1;
        }
        None
    }

    /// C++ `dictionary_datrie_match_longest`.
    pub(crate) fn match_longest(&self, word: &[u32], maxlen: usize) -> Option<(usize, &[u32])> {
        let (mut position, mut item) = self.dat_match(word, maxlen);
        while self.items[item].word == -1 && position > 1 {
            let (new_position, new_item) = self.dat_match(word, position - 1);
            if new_position == position && new_item == item {
                break;
            }
            position = new_position;
            item = new_item;
        }
        if position == 0 || self.items[item].word == -1 {
            return None;
        }
        let conversion = self.lexicon_entry(self.items[item].word)?;
        Some((position, conversion))
    }

    /// C++ `dictionary_datrie_get_all_match_lengths`.
    pub(crate) fn all_match_lengths(&self, word: &[u32], out: &mut Vec<usize>) {
        let mut i = 0usize;
        let mut p = 0usize;
        while p < word.len() && self.items[i].base != OPENCC_DATRIE_UNUSED {
            let j = self.items[i].base as i64 + i64::from(word[p]);
            if j < 0 || j as usize >= self.items.len() {
                break;
            }
            let j = j as usize;
            if self.items[j].parent != i as i32 {
                break;
            }
            i = j;
            p += 1;
            if self.items[i].word != -1 {
                out.push(p);
            }
        }
    }
}

/// C++ `OpenCC` (`openccxx.cpp`) with the `zht2zhs.ini` dictionary set: the
/// phrase table first, then the character table, both in one dictionary group.
pub struct OpenCcConverter {
    pub(crate) phrases: OpenCcDict,
    pub(crate) characters: OpenCcDict,
}

impl OpenCcConverter {
    pub fn load(resource_dir: impl AsRef<Path>) -> Result<Self, String> {
        let dir = resource_dir.as_ref().join("opencc");
        for name in [
            "zht2zhs.ini",
            "trad_to_simp_phrases.ocd",
            "trad_to_simp_characters.ocd",
        ] {
            let path = dir.join(name);
            if !path.is_file() {
                return Err(format!("missing analyzer file: {}", path.display()));
            }
        }
        let read = |name: &str| -> Result<Vec<u8>, String> {
            let path = dir.join(name);
            std::fs::read(&path).map_err(|err| format!("failed to read {}: {err}", path.display()))
        };
        Ok(Self {
            phrases: OpenCcDict::parse(&read("trad_to_simp_phrases.ocd")?)?,
            characters: OpenCcDict::parse(&read("trad_to_simp_characters.ocd")?)?,
        })
    }

    /// C++ `dictionary_group_match_longest`: the longest match across both
    /// dictionaries wins, ties keep the first (the phrase table).
    pub(crate) fn match_longest(&self, word: &[u32], maxlen: usize) -> Option<(usize, &[u32])> {
        let mut best: Option<(usize, &[u32])> = None;
        for dict in [&self.phrases, &self.characters] {
            if let Some((length, conversion)) = dict.match_longest(word, maxlen) {
                if best.is_none_or(|(best_len, _)| length > best_len) {
                    best = Some((length, conversion));
                }
            }
        }
        best
    }

    /// C++ `dictionary_group_get_all_match_lengths`: merged, sorted, deduped.
    pub(crate) fn all_match_lengths(&self, word: &[u32]) -> Vec<usize> {
        let mut lengths = Vec::new();
        self.phrases.all_match_lengths(word, &mut lengths);
        self.characters.all_match_lengths(word, &mut lengths);
        lengths.sort_unstable();
        lengths.dedup();
        lengths
    }

    /// C++ `sp_seg` — shortest-path segmentation of one ambiguous region, then
    /// longest-match conversion of each segment.
    pub(crate) fn sp_seg(&self, word: &[u32], out: &mut Vec<u32>) {
        if word.len() == 1 {
            match self.match_longest(word, 1) {
                Some((_, conversion)) => out.extend_from_slice(conversion),
                None => out.push(word[0]),
            }
            return;
        }

        let length = word.len();
        let mut min_len = vec![OPENCC_INFINITY; length + 1];
        let mut parent = vec![0usize; length + 1];
        min_len[0] = 0;
        for i in 0..length {
            let mut matches = self.all_match_lengths(&word[i..]);
            if matches.first() != Some(&1) {
                matches.push(1);
            }
            for k in matches {
                if k > 1 {
                    if min_len[i] < min_len[i + k] {
                        min_len[i + k] = min_len[i] + 1;
                        parent[i + k] = i;
                    }
                } else if min_len[i] + 1 < min_len[i + k] {
                    min_len[i + k] = min_len[i] + 1;
                    parent[i + k] = i;
                }
            }
        }

        let segments = min_len[length] as usize;
        let mut path = vec![0usize; segments];
        let mut position = length;
        let mut slot = segments;
        while position != 0 {
            slot -= 1;
            path[slot] = position;
            position = parent[position];
        }

        let mut begin = 0usize;
        let mut consumed = 0usize;
        for end in path {
            match self.match_longest(&word[consumed..], end - begin) {
                Some((length, conversion)) => {
                    out.extend_from_slice(conversion);
                    consumed += length;
                }
                None => {
                    if let Some(&character) = word.get(consumed) {
                        out.push(character);
                    }
                    consumed += 1;
                }
            }
            begin = end;
        }
    }

    /// C++ `converter_convert` -> `segment` with the default FAST mode.
    pub fn convert(&self, input: &str) -> String {
        let word: Vec<u32> = input.chars().map(u32::from).collect();
        let mut out = Vec::with_capacity(word.len());
        let mut bound = 0usize;
        let mut start = 0usize;
        for i in 0..word.len() {
            if i != 0 && i == bound {
                self.sp_seg(&word[start..bound], &mut out);
                start = i;
            }
            let mut match_len = self
                .match_longest(&word[i..], 0)
                .map_or(0, |(length, _)| length);
            if match_len == 0 {
                match_len = 1;
            }
            if i + match_len > bound {
                bound = i + match_len;
            }
        }
        if word.len() > start {
            self.sp_seg(&word[start..bound.min(word.len())], &mut out);
        }
        out.into_iter()
            .filter_map(char::from_u32)
            .collect::<String>()
    }
}
