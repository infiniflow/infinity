use super::*;

/// C++ `MeCab::Token` (`libmecab/mecab.h`) — 16 bytes in a binary dictionary.
pub(crate) const MECAB_TOKEN_SIZE: usize = 16;

/// C++ `DEFAULT_MAX_GROUPING_SIZE` (`libmecab/common.h`).
pub(crate) const MECAB_MAX_GROUPING_SIZE: usize = 24;

/// C++ `LIMIT_PARSE_LENGTH` (`jma_analyzer.cpp`): longer inputs are analysed in
/// chunks.
pub(crate) const MECAB_LIMIT_PARSE_LENGTH: usize = 8192;

/// C++ `DictionaryMagicID` (`libmecab/dictionary.cpp`).
pub(crate) const MECAB_DICT_MAGIC: u32 = 0xef71_8f77;

/// C++ `JMA_DICT_VERSION` / `JMA_DICT_BLOCK_SIZE` (`jma_dictionary.cpp`).
pub(crate) const JMA_DICT_VERSION: u32 = 100;

pub(crate) const JMA_DICT_BLOCK_SIZE: usize = 512;

/// C++ `DIC_VERSION` (`libmecab/config.h`, passed by the build).
pub(crate) const MECAB_DIC_VERSION: u32 = 102;

/// The length (in bytes) of the character starting at `at`, following
/// `MeCab::utf8_to_ucs2`: sequences it cannot decode count as one byte.
pub(crate) fn mecab_utf8_char(text: &[u8], at: usize) -> (u16, usize) {
    let tail = &text[at..];
    let first = tail[0];
    if first < 0x80 {
        (u16::from(first), 1)
    } else if tail.len() >= 2 && first & 0xe0 == 0xc0 {
        (
            u16::from((first & 0x1f) << 6) | u16::from(tail[1] & 0x3f),
            2,
        )
    } else if tail.len() >= 3 && first & 0xf0 == 0xe0 {
        (
            (u16::from(first & 0x0f) << 12)
                | (u16::from(tail[1] & 0x3f) << 6)
                | u16::from(tail[2] & 0x3f),
            3,
        )
    } else if tail.len() >= 4 && first & 0xf8 == 0xf0 {
        // "belows are out of UCS2": the reference maps 4..6-byte sequences to 0.
        (0, 4)
    } else if tail.len() >= 5 && first & 0xfc == 0xf8 {
        (0, 5)
    } else if tail.len() >= 6 && first & 0xfe == 0xfc {
        (0, 6)
    } else {
        (0, 1)
    }
}

pub(crate) fn mecab_u32(bytes: &[u8], at: usize) -> Result<u32, String> {
    bytes
        .get(at..at + 4)
        .map(|b| u32::from_le_bytes([b[0], b[1], b[2], b[3]]))
        .ok_or_else(|| "truncated MeCab dictionary".to_string())
}

/// Bit reader in the style of `puff.c`; bits are consumed least-significant
/// first from a little-endian byte stream.
pub(crate) struct InflateBits<'a> {
    pub(crate) data: &'a [u8],
    pub(crate) pos: usize,
    pub(crate) bitbuf: u32,
    pub(crate) bitcnt: u32,
}

impl<'a> InflateBits<'a> {
    pub fn new(data: &'a [u8]) -> Self {
        Self {
            data,
            pos: 0,
            bitbuf: 0,
            bitcnt: 0,
        }
    }

    pub(crate) fn bits(&mut self, need: u32) -> Option<u32> {
        let mut value = self.bitbuf;
        while self.bitcnt < need {
            let byte = *self.data.get(self.pos)?;
            self.pos += 1;
            value |= u32::from(byte) << self.bitcnt;
            self.bitcnt += 8;
        }
        self.bitbuf = value >> need;
        self.bitcnt -= need;
        Some(value & ((1u32 << need) - 1))
    }

    pub(crate) fn align(&mut self) {
        self.bitbuf = 0;
        self.bitcnt = 0;
    }
}

pub(crate) fn inflate(reader: &mut InflateBits<'_>) -> Result<Vec<u8>, String> {
    let mut out = Vec::new();
    loop {
        let last = reader
            .bits(1)
            .ok_or_else(|| "truncated deflate stream".to_string())?;
        let kind = reader
            .bits(2)
            .ok_or_else(|| "truncated deflate stream".to_string())?;
        match kind {
            0 => {
                reader.align();
                let header = reader
                    .data
                    .get(reader.pos..reader.pos + 4)
                    .ok_or_else(|| "truncated stored block".to_string())?;
                let len = u16::from_le_bytes([header[0], header[1]]);
                let nlen = u16::from_le_bytes([header[2], header[3]]);
                if len != !nlen {
                    return Err("stored block length mismatch".to_string());
                }
                reader.pos += 4;
                let body = reader
                    .data
                    .get(reader.pos..reader.pos + usize::from(len))
                    .ok_or_else(|| "truncated stored block".to_string())?;
                out.extend_from_slice(body);
                reader.pos += usize::from(len);
            }
            1 => {
                let mut lengths = [0u8; 288];
                lengths[..144].fill(8);
                lengths[144..256].fill(9);
                lengths[256..280].fill(7);
                lengths[280..].fill(8);
                let lit = InflateHuffman::new(&lengths)
                    .ok_or_else(|| "invalid fixed literal table".to_string())?;
                let dist = InflateHuffman::new(&[5u8; 30])
                    .ok_or_else(|| "invalid fixed distance table".to_string())?;
                inflate_codes(reader, &mut out, &lit, &dist)?;
            }
            2 => {
                let nlen = reader.bits(5).unwrap_or(0) as usize + 257;
                let ndist = reader.bits(5).unwrap_or(0) as usize + 1;
                let ncode = reader.bits(4).unwrap_or(0) as usize + 4;
                const ORDER: [usize; 19] = [
                    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15,
                ];
                let mut code_lengths = [0u8; 19];
                for &slot in ORDER.iter().take(ncode) {
                    code_lengths[slot] = reader.bits(3).unwrap_or(0) as u8;
                }
                let code_table = InflateHuffman::new(&code_lengths)
                    .ok_or_else(|| "invalid code-length table".to_string())?;
                let mut lengths = vec![0u8; nlen + ndist];
                let mut index = 0usize;
                while index < lengths.len() {
                    let symbol = code_table
                        .decode(reader)
                        .ok_or_else(|| "invalid code-length symbol".to_string())?;
                    match symbol {
                        0..=15 => {
                            lengths[index] = symbol as u8;
                            index += 1;
                        }
                        16 => {
                            let previous = if index == 0 { 0 } else { lengths[index - 1] };
                            let repeat = 3 + reader.bits(2).unwrap_or(0) as usize;
                            for _ in 0..repeat {
                                if index < lengths.len() {
                                    lengths[index] = previous;
                                    index += 1;
                                }
                            }
                        }
                        17 => {
                            let repeat = 3 + reader.bits(3).unwrap_or(0) as usize;
                            index = (index + repeat).min(lengths.len());
                        }
                        _ => {
                            let repeat = 11 + reader.bits(7).unwrap_or(0) as usize;
                            index = (index + repeat).min(lengths.len());
                        }
                    }
                }
                let lit = InflateHuffman::new(&lengths[..nlen])
                    .ok_or_else(|| "invalid literal table".to_string())?;
                let dist = InflateHuffman::new(&lengths[nlen..])
                    .ok_or_else(|| "invalid distance table".to_string())?;
                inflate_codes(reader, &mut out, &lit, &dist)?;
            }
            _ => return Err("invalid deflate block type".to_string()),
        }
        if last == 1 {
            return Ok(out);
        }
    }
}

/// zlib (`RFC 1950`) decompression over the raw DEFLATE decoder above.
pub(crate) fn zlib_decompress(input: &[u8]) -> Result<Vec<u8>, String> {
    if input.len() < 2 {
        return Err("zlib stream is truncated".to_string());
    }
    let (cmf, flg) = (input[0], input[1]);
    if cmf & 0x0f != 8 {
        return Err("unsupported zlib compression method".to_string());
    }
    if (u16::from(cmf) << 8 | u16::from(flg)) % 31 != 0 {
        return Err("bad zlib header checksum".to_string());
    }
    if flg & 0x20 != 0 {
        return Err("zlib preset dictionaries are not supported".to_string());
    }
    let mut reader = InflateBits::new(&input[2..]);
    inflate(&mut reader)
}

#[derive(Clone, Copy)]
pub(crate) struct MecabToken {
    pub(crate) lc: u16,
    pub(crate) rc: u16,
    pub(crate) posid: u16,
    pub(crate) wcost: i16,
    pub(crate) feature: u32,
}

/// C++ `MeCab::Dictionary` — a Darts double array plus the token and feature
/// pools it indexes.
pub(crate) struct MecabDict {
    /// `unit_t { int base; unsigned check; }`, 8 bytes per unit.
    pub(crate) base: Vec<i32>,
    pub(crate) check: Vec<u32>,
    pub(crate) tokens: Vec<u8>,
    pub(crate) features: Vec<u8>,
}

impl MecabDict {
    pub(crate) fn parse(bytes: &[u8]) -> Result<Self, String> {
        if bytes.len() < 72 {
            return Err("MeCab dictionary is truncated".to_string());
        }
        let magic = mecab_u32(bytes, 0)?;
        if magic ^ MECAB_DICT_MAGIC != bytes.len() as u32 {
            return Err("broken MeCab dictionary magic".to_string());
        }
        let version = mecab_u32(bytes, 4)?;
        if version != MECAB_DIC_VERSION {
            return Err(format!("unsupported MeCab dictionary version {version}"));
        }
        let dsize = mecab_u32(bytes, 24)? as usize;
        let tsize = mecab_u32(bytes, 28)? as usize;
        let fsize = mecab_u32(bytes, 32)? as usize;
        let da_start = 72;
        let token_start = da_start + dsize;
        let feature_start = token_start + tsize;
        if feature_start + fsize != bytes.len()
            || !dsize.is_multiple_of(8)
            || !tsize.is_multiple_of(MECAB_TOKEN_SIZE)
        {
            return Err("broken MeCab dictionary layout".to_string());
        }
        let units = &bytes[da_start..token_start];
        let mut base = Vec::with_capacity(units.len() / 8);
        let mut check = Vec::with_capacity(units.len() / 8);
        for unit in units.chunks_exact(8) {
            base.push(i32::from_le_bytes([unit[0], unit[1], unit[2], unit[3]]));
            check.push(u32::from_le_bytes([unit[4], unit[5], unit[6], unit[7]]));
        }
        Ok(Self {
            base,
            check,
            tokens: bytes[token_start..feature_start].to_vec(),
            features: bytes[feature_start..].to_vec(),
        })
    }

    pub(crate) fn token(&self, index: usize) -> Option<MecabToken> {
        let at = index.checked_mul(MECAB_TOKEN_SIZE)?;
        let raw = self.tokens.get(at..at + MECAB_TOKEN_SIZE)?;
        Some(MecabToken {
            lc: u16::from_le_bytes([raw[0], raw[1]]),
            rc: u16::from_le_bytes([raw[2], raw[3]]),
            posid: u16::from_le_bytes([raw[4], raw[5]]),
            wcost: i16::from_le_bytes([raw[6], raw[7]]),
            feature: u32::from_le_bytes([raw[8], raw[9], raw[10], raw[11]]),
        })
    }

    pub(crate) fn feature_raw(&self, feature: u32) -> &[u8] {
        let start = feature as usize;
        let rest = self.features.get(start..).unwrap_or(&[]);
        let len = rest.iter().position(|&b| b == 0).unwrap_or(rest.len());
        &rest[..len]
    }

    /// C++ `Darts::DoubleArrayImpl::commonPrefixSearch` — `(value, length)`
    /// pairs ordered by increasing key length.
    pub(crate) fn common_prefix_search(&self, key: &[u8]) -> Vec<(i32, usize)> {
        let mut results = Vec::new();
        let mut b = self.base[0];
        for (index, &byte) in key.iter().enumerate() {
            let p = b as usize;
            let n = *self.base.get(p).unwrap_or(&0);
            if b as u32 == *self.check.get(p).unwrap_or(&u32::MAX) && n < 0 {
                results.push((-n - 1, index));
            }
            let p = b.wrapping_add(i32::from(byte)).wrapping_add(1) as usize;
            if p >= self.check.len() || b as u32 != self.check[p] {
                return results;
            }
            b = self.base[p];
        }
        let p = b as usize;
        let n = *self.base.get(p).unwrap_or(&0);
        if b as u32 == *self.check.get(p).unwrap_or(&u32::MAX) && n < 0 {
            results.push((-n - 1, key.len()));
        }
        results
    }

    /// C++ `Darts::DoubleArrayImpl::exactMatchSearch`.
    pub(crate) fn exact_match(&self, key: &[u8]) -> Option<i32> {
        let mut b = self.base[0];
        for &byte in key {
            let p = b.wrapping_add(i32::from(byte)).wrapping_add(1) as usize;
            if p >= self.check.len() || b as u32 != self.check[p] {
                return None;
            }
            b = self.base[p];
        }
        let p = b as usize;
        let n = *self.base.get(p)?;
        if b as u32 == *self.check.get(p)? && n < 0 {
            Some(-n - 1)
        } else {
            None
        }
    }
}

/// C++ `MeCab::CharInfo` — the bit-field record stored per code point.
#[derive(Clone, Copy, Default)]
pub(crate) struct MecabCharInfo {
    pub(crate) mask: u32,
    pub(crate) default_type: u8,
    pub(crate) length: u8,
    pub(crate) group: bool,
    pub(crate) invoke: bool,
}

impl MecabCharInfo {
    pub(crate) fn is_kind_of(&self, other: &MecabCharInfo) -> bool {
        self.mask & other.mask != 0
    }
}

/// C++ `MeCab::CharProperty` (`char.bin`).
pub(crate) struct MecabCharProperty {
    pub(crate) names: Vec<String>,
    pub(crate) map: Vec<MecabCharInfo>,
}

impl MecabCharProperty {
    pub(crate) fn parse(bytes: &[u8]) -> Result<Self, String> {
        let size = mecab_u32(bytes, 0)? as usize;
        let expected = 4 + 32 * size + 4 * 0xffff;
        if bytes.len() != expected {
            return Err("broken MeCab char.bin".to_string());
        }
        let mut names = Vec::with_capacity(size);
        for index in 0..size {
            let at = 4 + 32 * index;
            let raw = &bytes[at..at + 32];
            let len = raw.iter().position(|&b| b == 0).unwrap_or(raw.len());
            names.push(String::from_utf8_lossy(&raw[..len]).into_owned());
        }
        let table = &bytes[4 + 32 * size..];
        let mut map = Vec::with_capacity(0xffff);
        for entry in table.chunks_exact(4) {
            // `unsigned int type:18; default_type:8; length:4; group:1; invoke:1`
            let word = u32::from_le_bytes([entry[0], entry[1], entry[2], entry[3]]);
            map.push(MecabCharInfo {
                mask: word & 0x3_ffff,
                default_type: ((word >> 18) & 0xff) as u8,
                length: ((word >> 26) & 0xf) as u8,
                group: (word >> 30) & 1 == 1,
                invoke: (word >> 31) & 1 == 1,
            });
        }
        Ok(Self { names, map })
    }

    pub(crate) fn get(&self, code: u16) -> MecabCharInfo {
        self.map[usize::from(code).min(self.map.len() - 1)]
    }
}

/// One morpheme produced by the Viterbi pass.
pub(crate) struct MecabMorpheme {
    pub(crate) start: usize,
    pub(crate) length: usize,
    pub(crate) posid: u16,
    pub(crate) feature_start: u32,
    pub(crate) feature_len: usize,
    /// Unknown-word nodes take their feature from `unk.dic`, not `sys.dic`.
    pub(crate) unknown: bool,
}

/// C++ `TokenizerImpl` + `Viterbi` over one system dictionary.
pub(crate) struct MecabEngine {
    pub(crate) sys: MecabDict,
    pub(crate) unk: MecabDict,
    /// `short matrix[lsize * rsize]` without the two size words.
    pub(crate) matrix: Vec<i16>,
    pub(crate) lsize: usize,
    pub(crate) property: MecabCharProperty,
    /// `unk_tokens_`: for each character category, `(first token, count)`.
    pub(crate) unk_tokens: Vec<(usize, usize)>,
    pub(crate) space: MecabCharInfo,
}

impl MecabEngine {
    pub(crate) fn from_files(
        sys: &[u8],
        unk: &[u8],
        chars: &[u8],
        matrix: &[u8],
    ) -> Result<Self, String> {
        let sys = MecabDict::parse(sys)?;
        let unk = MecabDict::parse(unk)?;
        let property = MecabCharProperty::parse(chars)?;
        if matrix.len() < 4 {
            return Err("broken MeCab matrix.bin".to_string());
        }
        let lsize = usize::from(u16::from_le_bytes([matrix[0], matrix[1]]));
        let rsize = usize::from(u16::from_le_bytes([matrix[2], matrix[3]]));
        if matrix.len() != 4 + lsize * rsize * 2 {
            return Err("broken MeCab matrix.bin".to_string());
        }
        let matrix = matrix[4..]
            .chunks_exact(2)
            .map(|b| i16::from_le_bytes([b[0], b[1]]))
            .collect();
        let mut unk_tokens = Vec::with_capacity(property.names.len());
        for name in &property.names {
            let value = unk
                .exact_match(name.as_bytes())
                .ok_or_else(|| format!("cannot find UNK category: {name}"))?;
            unk_tokens.push(((value >> 8) as usize, (value & 0xff) as usize));
        }
        let space = property.get(0x20);
        Ok(Self {
            sys,
            unk,
            matrix,
            lsize,
            property,
            unk_tokens,
            space,
        })
    }

    /// C++ `JMA_Dictionary::open` for `mecab/ipadic/sys.bin`: a zlib stream of
    /// 512-byte-headed file blocks holding `sys.dic`, `unk.dic`, `char.bin`
    /// and `matrix.bin`.
    pub(crate) fn from_jma_archive(path: &Path) -> Result<Self, String> {
        let data = std::fs::read(path)
            .map_err(|err| format!("failed to read {}: {err}", path.display()))?;
        if data.len() < JMA_DICT_BLOCK_SIZE {
            return Err(format!("dictionary file {} is broken", path.display()));
        }
        let version = mecab_u32(&data, 0)?;
        if version != JMA_DICT_VERSION {
            return Err(format!(
                "dictionary file {} has unsupported version {version}",
                path.display()
            ));
        }
        let file_count = mecab_u32(&data, 4)? as usize;
        let total_size = mecab_u32(&data, 8)? as usize;
        if file_count == 0 || data.len() < JMA_DICT_BLOCK_SIZE * 2 {
            return Err(format!("dictionary file {} is broken", path.display()));
        }
        let raw = zlib_decompress(&data[JMA_DICT_BLOCK_SIZE..])?;
        if raw.len() != total_size {
            return Err(format!(
                "uncompressed size {} does not match {total_size}",
                raw.len()
            ));
        }
        let mut content = JMA_DICT_BLOCK_SIZE * file_count;
        let mut files: HashMap<String, &[u8]> = HashMap::new();
        for index in 0..file_count {
            let head = &raw[index * JMA_DICT_BLOCK_SIZE..(index + 1) * JMA_DICT_BLOCK_SIZE];
            let name_len = head.iter().position(|&b| b == 0).unwrap_or(500);
            let name = String::from_utf8_lossy(&head[..name_len]).into_owned();
            let length = u32::from_le_bytes([head[500], head[501], head[502], head[503]]) as usize;
            let body = raw
                .get(content..content + length)
                .ok_or_else(|| "broken dictionary archive".to_string())?;
            files.insert(name, body);
            content += length.div_ceil(JMA_DICT_BLOCK_SIZE) * JMA_DICT_BLOCK_SIZE;
        }
        let get = |name: &str| -> Result<&[u8], String> {
            files
                .get(name)
                .copied()
                .ok_or_else(|| format!("dictionary archive lacks {name}"))
        };
        Self::from_files(
            get("sys.dic")?,
            get("unk.dic")?,
            get("char.bin")?,
            get("matrix.bin")?,
        )
    }

    /// C++ `MeCab::Tokenizer::open` over a plain dictionary directory.
    pub(crate) fn from_dir(dir: &Path) -> Result<Self, String> {
        let read = |name: &str| -> Result<Vec<u8>, String> {
            let path = dir.join(name);
            std::fs::read(&path).map_err(|err| format!("failed to read {}: {err}", path.display()))
        };
        Self::from_files(
            &read("sys.dic")?,
            &read("unk.dic")?,
            &read("char.bin")?,
            &read("matrix.bin")?,
        )
    }

    pub(crate) fn char_info_at(&self, text: &[u8], at: usize) -> (MecabCharInfo, usize) {
        // The reference reads the terminating NUL when the lattice reaches the
        // end of the sentence; that decodes as code point 0 (`DEFAULT`).
        if at >= text.len() {
            return (self.property.get(0), 1);
        }
        let (code, len) = mecab_utf8_char(text, at);
        (self.property.get(code), len)
    }

    /// C++ `CharProperty::seekToOtherType`: skip characters that share a type
    /// bit with `current`.
    pub(crate) fn seek_to_other_type(
        &self,
        text: &[u8],
        begin: usize,
        end: usize,
        current: MecabCharInfo,
    ) -> (usize, MecabCharInfo, usize, usize) {
        let mut position = begin;
        let mut count = 0usize;
        let mut kind = current;
        let mut mblen = 0usize;
        let mut fail = current;
        while position < end {
            let (info, len) = self.char_info_at(text, position);
            fail = info;
            mblen = len;
            if !kind.is_kind_of(&info) {
                break;
            }
            position += len;
            count += 1;
            kind = info;
        }
        (position, fail, mblen, count)
    }

    pub(crate) fn unknown_node(
        &self,
        category: u8,
        start: usize,
        length: usize,
        rlength: usize,
    ) -> Vec<LatticeNode> {
        let Some(&(first, count)) = self.unk_tokens.get(usize::from(category)) else {
            return Vec::new();
        };
        (0..count)
            .filter_map(|offset| self.unk.token(first + offset))
            .map(|token| LatticeNode {
                start,
                length,
                rlength,
                lc: token.lc,
                rc: token.rc,
                posid: token.posid,
                wcost: token.wcost,
                feature: token.feature,
                unknown: true,
                cost: 0,
                prev: None,
            })
            .collect()
    }

    /// C++ `TokenizerImpl::lookup` — dictionary entries plus the unknown-word
    /// candidates for the character at `begin`. Nodes are returned in creation
    /// order; the reference links them head-first, i.e. in reverse.
    pub(crate) fn lookup(&self, text: &[u8], begin: usize) -> Vec<LatticeNode> {
        let end = (begin + 65535).min(text.len());
        let (begin2, cinfo, mblen, _clen) = self.seek_to_other_type(text, begin, end, self.space);
        let mut nodes = Vec::new();
        for (value, length) in self.sys.common_prefix_search(&text[begin2..end]) {
            let first = (value >> 8) as usize;
            let count = (value & 0xff) as usize;
            for offset in 0..count {
                let Some(token) = self.sys.token(first + offset) else {
                    continue;
                };
                nodes.push(LatticeNode {
                    start: begin2,
                    length,
                    rlength: begin2 - begin + length,
                    lc: token.lc,
                    rc: token.rc,
                    posid: token.posid,
                    wcost: token.wcost,
                    feature: token.feature,
                    unknown: false,
                    cost: 0,
                    prev: None,
                });
            }
        }
        if !nodes.is_empty() && !cinfo.invoke {
            return nodes;
        }

        let mut begin3 = begin2 + mblen;
        if begin3 > end {
            // The reference hands a pointer past the buffer to the unknown-word
            // branch; the surface it reads there is just the terminating NUL,
            // so the token is empty and `AnalyzeImpl` drops it.
            nodes.extend(self.unknown_node(cinfo.default_type, begin2, 0, end - begin));
            return nodes;
        }

        let mut group_begin3 = None;
        if cinfo.group {
            let saved = begin3;
            let (group_end, _fail, _len, clen) = self.seek_to_other_type(text, begin3, end, cinfo);
            if clen <= MECAB_MAX_GROUPING_SIZE {
                nodes.extend(self.unknown_node(
                    cinfo.default_type,
                    begin2,
                    group_end - begin2,
                    group_end - begin,
                ));
            }
            group_begin3 = Some(group_end);
            begin3 = saved;
        }

        for _ in 1..=usize::from(cinfo.length) {
            if begin3 > end {
                break;
            }
            if Some(begin3) == group_begin3 {
                continue;
            }
            nodes.extend(self.unknown_node(
                cinfo.default_type,
                begin2,
                begin3 - begin2,
                begin3 - begin,
            ));
            let (next, len) = self.char_info_at(text, begin3);
            if !cinfo.is_kind_of(&next) {
                break;
            }
            begin3 += len;
        }

        if nodes.is_empty() {
            nodes.extend(self.unknown_node(
                cinfo.default_type,
                begin2,
                begin3 - begin2,
                begin3 - begin,
            ));
        }
        nodes
    }

    /// C++ `Connector::cost` (`matrix[left.rc + lsize * right.lc] + right.wcost`).
    pub(crate) fn connection_cost(&self, left: &LatticeNode, right: &LatticeNode) -> i64 {
        let index = usize::from(left.rc) + self.lsize * usize::from(right.lc);
        let matrix = self.matrix.get(index).copied().unwrap_or(0);
        i64::from(matrix) + i64::from(right.wcost)
    }

    pub(crate) fn connect(
        &self,
        position: usize,
        incoming: &[usize],
        nodes: &mut [LatticeNode],
        end_at: &mut [Vec<usize>],
    ) {
        for &index in incoming.iter().rev() {
            let mut best: Option<usize> = None;
            let mut best_cost = i64::MAX;
            for &left in end_at[position].iter().rev() {
                let cost = nodes[left].cost + self.connection_cost(&nodes[left], &nodes[index]);
                if cost < best_cost {
                    best_cost = cost;
                    best = Some(left);
                }
            }
            nodes[index].prev = best;
            nodes[index].cost = best_cost;
            let target = position + nodes[index].rlength;
            if let Some(slot) = end_at.get_mut(target) {
                slot.push(index);
            }
        }
    }

    pub(crate) fn tokenize_one(&self, text: &[u8]) -> Vec<MecabMorpheme> {
        let len = text.len();
        let mut nodes: Vec<LatticeNode> = Vec::new();
        let mut end_at: Vec<Vec<usize>> = vec![Vec::new(); len + 1];
        nodes.push(LatticeNode {
            start: 0,
            length: 0,
            rlength: 0,
            lc: 0,
            rc: 0,
            posid: 0,
            wcost: 0,
            feature: 0,
            unknown: false,
            cost: 0,
            prev: None,
        });
        end_at[0].push(0);

        for position in 0..len {
            if end_at[position].is_empty() {
                continue;
            }
            let found = self.lookup(text, position);
            let mut incoming = Vec::with_capacity(found.len());
            for node in found {
                incoming.push(nodes.len());
                nodes.push(node);
            }
            self.connect(position, &incoming, &mut nodes, &mut end_at);
        }

        let eos = nodes.len();
        nodes.push(LatticeNode {
            start: len,
            length: 0,
            rlength: 0,
            lc: 0,
            rc: 0,
            posid: 0,
            wcost: 0,
            feature: 0,
            unknown: false,
            cost: 0,
            prev: None,
        });
        if let Some(position) = (0..=len).rev().find(|&p| !end_at[p].is_empty()) {
            self.connect(position, &[eos], &mut nodes, &mut end_at);
        }

        let mut path = Vec::new();
        let mut current = eos;
        while let Some(previous) = nodes[current].prev {
            path.push(current);
            current = previous;
        }
        path.reverse();
        path.into_iter()
            .filter(|&index| nodes[index].length > 0)
            .map(|index| {
                let node = &nodes[index];
                let feature = if node.unknown {
                    self.unk.feature_raw(node.feature)
                } else {
                    self.sys.feature_raw(node.feature)
                };
                MecabMorpheme {
                    start: node.start,
                    length: node.length.min(len - node.start),
                    posid: node.posid,
                    feature_start: node.feature,
                    feature_len: feature.len(),
                    unknown: node.unknown,
                }
            })
            .collect()
    }

    /// C++ `JMA_Analyzer::runOneBest` — `splitLimitSize` chunks the input at
    /// `LIMIT_PARSE_LENGTH`; short inputs are analysed in one pass.
    pub fn tokenize(&self, text: &str) -> Vec<MecabMorpheme> {
        let bytes = text.as_bytes();
        let mut morphemes = Vec::new();
        let mut start = 0usize;
        while start < bytes.len() {
            let mut end = (start + MECAB_LIMIT_PARSE_LENGTH).min(bytes.len());
            while end < bytes.len() && (bytes[end] & 0xc0) == 0x80 {
                end -= 1;
            }
            morphemes.extend(self.tokenize_one(&bytes[start..end]));
            start = end;
        }
        morphemes
    }

    pub(crate) fn feature(&self, morpheme: &MecabMorpheme) -> &str {
        let start = morpheme.feature_start as usize;
        let pool = if morpheme.unknown {
            &self.unk.features
        } else {
            &self.sys.features
        };
        let rest = pool.get(start..).unwrap_or(&[]);
        let len = morpheme.feature_len.min(rest.len());
        std::str::from_utf8(&rest[..len]).unwrap_or("")
    }
}

/// C++ `JapaneseAnalyzer` (`japanese_analyzer.cppm`).
///
/// Loads `mecab/ipadic/sys.bin`; index mode (one-best, no compound morphology,
/// no user-noun decomposition) is the default.
#[derive(Clone)]
pub struct JapaneseAnalyzer {
    pub(crate) engine: Arc<MecabEngine>,
}

impl JapaneseAnalyzer {
    pub fn load(resource_dir: impl AsRef<Path>) -> Result<Self, String> {
        let archive = resource_dir.as_ref().join("mecab/ipadic/sys.bin");
        if !archive.is_file() {
            return Err(format!("missing analyzer file: {}", archive.display()));
        }
        Ok(Self {
            engine: Arc::new(MecabEngine::from_jma_archive(&archive)?),
        })
    }

    /// C++ `CommonLanguageAnalyzer::AnalyzeImpl` over `JapaneseAnalyzer`'s
    /// token stream: spaces are filtered by `iterateNode`, alphabet runs are
    /// lower-cased, and stemming is never initialised for this family.
    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut terms = Vec::new();
        let mut offset = 0u32;
        for morpheme in self.engine.tokenize(input) {
            let token = &input[morpheme.start..morpheme.start + morpheme.length];
            if token.is_empty() || token.len() >= MAX_TUPLE_LENGTH || is_mecab_space(token) {
                continue;
            }
            let text = if morpheme.posid == JAPANESE_ALPHA_POS {
                token.to_ascii_lowercase()
            } else {
                token.to_string()
            };
            terms.push(Term {
                text,
                word_offset: offset,
                char_offset: 0,
                end_offset: 0,
                is_index: true,
                payload: 0,
            });
            offset += 1;
        }
        terms
    }
}

/// C++ `KoreanAnalyzer` (`korean_analyzer.cppm`) over `mecab/ko-dic`.
#[derive(Clone)]
pub struct KoreanAnalyzer {
    pub(crate) engine: Arc<MecabEngine>,
}

impl KoreanAnalyzer {
    pub fn load(resource_dir: impl AsRef<Path>) -> Result<Self, String> {
        let dir = resource_dir.as_ref().join("mecab/ko-dic");
        for name in ["sys.dic", "unk.dic", "char.bin", "matrix.bin"] {
            let path = dir.join(name);
            if !path.is_file() {
                return Err(format!("missing analyzer file: {}", path.display()));
            }
        }
        Ok(Self {
            engine: Arc::new(MecabEngine::from_dir(&dir)?),
        })
    }

    /// C++ `KoreanAnalyzer::IsAlpha` — `MeCab::IsAlpha`: a two-byte-or-longer
    /// token whose feature starts with `SL`.
    pub(crate) fn is_alpha(engine: &MecabEngine, morpheme: &MecabMorpheme) -> bool {
        morpheme.length >= 2 && engine.feature(morpheme).starts_with("SL")
    }

    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut terms = Vec::new();
        let mut offset = 0u32;
        for morpheme in self.engine.tokenize(input) {
            let token = &input[morpheme.start..morpheme.start + morpheme.length];
            if token.is_empty() || token.len() >= MAX_TUPLE_LENGTH {
                continue;
            }
            let text = if Self::is_alpha(&self.engine, &morpheme) {
                token.to_ascii_lowercase()
            } else {
                token.to_string()
            };
            terms.push(Term {
                text,
                word_offset: offset,
                char_offset: 0,
                end_offset: 0,
                is_index: true,
                payload: 0,
            });
            offset += 1;
        }
        terms
    }
}
