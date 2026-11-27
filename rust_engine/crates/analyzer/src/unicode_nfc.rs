// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! C++: `unicode_nfc.h` — Unicode NFC normalization, the
//! `unicodedata.normalize("NFC", ...)` step of rag_tokenizer.py, from the
//! tables `tools/generate_unicode_nfc.py` writes for both engines.
//!
//! The input is split into segments, each starting at a code point with ccc 0
//! and NFC_Quick_Check=Yes. A segment that passes the quick check is already
//! NFC; any other one is decomposed, put in canonical order and composed again
//! (UAX #15). Segments normalize independently, so text that is already NFC is
//! never copied. A `&str` is always valid UTF-8, so the invalid-byte handling
//! of the C++ version has no counterpart here.

use crate::unicode_nfc_data::{NFC_COMPOSITIONS, NFC_DECOMPOSITIONS, NFC_NON_BOUNDARY};

// Hangul syllables are composed and decomposed arithmetically (Unicode 3.12).
const HANGUL_S_BASE: u32 = 0xAC00;
const HANGUL_L_BASE: u32 = 0x1100;
const HANGUL_V_BASE: u32 = 0x1161;
const HANGUL_T_BASE: u32 = 0x11A7;
const HANGUL_L_COUNT: u32 = 19;
const HANGUL_V_COUNT: u32 = 21;
const HANGUL_T_COUNT: u32 = 28;
const HANGUL_N_COUNT: u32 = HANGUL_V_COUNT * HANGUL_T_COUNT;
const HANGUL_S_COUNT: u32 = HANGUL_L_COUNT * HANGUL_N_COUNT;

/// C++: `nfc_detail::NFCUnit` — a code point of a segment being normalized,
/// and the input bytes [offset, end) it comes from.
#[derive(Clone, Copy)]
struct Unit {
    ch: char,
    ccc: u8,
    offset: u32,
    end: u32,
}

/// C++: `nfc_detail::FindNonBoundary` — (ccc, quick_check_yes) of a code point
/// that is not an NFC boundary, or `None` for a boundary (ccc 0 and
/// NFC_Quick_Check=Yes), as every code point below U+0300 is.
fn non_boundary(c: char) -> Option<(u8, bool)> {
    if c < NFC_NON_BOUNDARY[0].0 {
        return None;
    }
    let index = NFC_NON_BOUNDARY.partition_point(|range| range.0 <= c) - 1;
    let (_, last, ccc, quick_check_yes) = NFC_NON_BOUNDARY[index];
    (c <= last).then_some((ccc, quick_check_yes))
}

fn canonical_combining_class(c: char) -> u8 {
    non_boundary(c).map_or(0, |(ccc, _)| ccc)
}

/// The Hangul arithmetic below only yields assigned jamo and syllables.
fn hangul(code: u32) -> char {
    char::from_u32(code).expect("Hangul code point")
}

/// C++: `nfc_detail::Decompose` — appends the full canonical decomposition of
/// `c` to `buf`; every code point of it comes from input bytes [offset, end).
fn decompose(c: char, offset: u32, end: u32, buf: &mut Vec<Unit>) {
    let code = c as u32;
    if (HANGUL_S_BASE..HANGUL_S_BASE + HANGUL_S_COUNT).contains(&code) {
        let s = code - HANGUL_S_BASE;
        let jamo = |code| Unit {
            ch: hangul(code),
            ccc: 0,
            offset,
            end,
        };
        buf.push(jamo(HANGUL_L_BASE + s / HANGUL_N_COUNT));
        buf.push(jamo(HANGUL_V_BASE + s % HANGUL_N_COUNT / HANGUL_T_COUNT));
        if !s.is_multiple_of(HANGUL_T_COUNT) {
            buf.push(jamo(HANGUL_T_BASE + s % HANGUL_T_COUNT));
        }
        return;
    }
    if let Ok(index) = NFC_DECOMPOSITIONS.binary_search_by(|entry| entry.0.cmp(&c)) {
        let (_, first, second) = NFC_DECOMPOSITIONS[index];
        decompose(first, offset, end, buf);
        if second != '\0' {
            decompose(second, offset, end, buf);
        }
        return;
    }
    buf.push(Unit {
        ch: c,
        ccc: canonical_combining_class(c),
        offset,
        end,
    });
}

/// C++: `nfc_detail::Compose` — the primary composite of `first` + `second`.
fn compose(first: char, second: char) -> Option<char> {
    let (a, b) = (first as u32, second as u32);
    if (HANGUL_L_BASE..HANGUL_L_BASE + HANGUL_L_COUNT).contains(&a)
        && (HANGUL_V_BASE..HANGUL_V_BASE + HANGUL_V_COUNT).contains(&b)
    {
        return Some(hangul(
            HANGUL_S_BASE
                + ((a - HANGUL_L_BASE) * HANGUL_V_COUNT + b - HANGUL_V_BASE) * HANGUL_T_COUNT,
        ));
    }
    if (HANGUL_S_BASE..HANGUL_S_BASE + HANGUL_S_COUNT).contains(&a)
        && (a - HANGUL_S_BASE).is_multiple_of(HANGUL_T_COUNT)
        && (HANGUL_T_BASE + 1..HANGUL_T_BASE + HANGUL_T_COUNT).contains(&b)
    {
        return Some(hangul(a + b - HANGUL_T_BASE));
    }
    NFC_COMPOSITIONS
        .binary_search_by(|entry| (entry.0, entry.1).cmp(&(first, second)))
        .ok()
        .map(|index| NFC_COMPOSITIONS[index].2)
}

/// C++: `nfc_detail::ReorderAndCompose` — puts a decomposed segment in
/// canonical order and composes it (UAX #15). A composite keeps its starter's
/// offset and comes from the input bytes of everything it took in.
fn reorder_and_compose(buf: &mut Vec<Unit>) {
    // Canonical ordering: a stable sort of each run of non-starters, so a long
    // run of marks stays O(n log n).
    let mut start = 0;
    while start < buf.len() {
        if buf[start].ccc == 0 {
            start += 1;
            continue;
        }
        let end = buf[start..]
            .iter()
            .position(|unit| unit.ccc == 0)
            .map_or(buf.len(), |len| start + len);
        buf[start..end].sort_by_key(|unit| unit.ccc);
        start = end;
    }
    // Canonical composition. A code point is blocked from the last starter when
    // a code point kept in between has a ccc of 0 or not lower than its own.
    let mut starter = 0;
    let mut kept = 0;
    let mut has_starter = false;
    let mut last_ccc = 0;
    for k in 0..buf.len() {
        let unit = buf[k];
        if has_starter && (last_ccc == 0 || last_ccc < unit.ccc) {
            if let Some(composite) = compose(buf[starter].ch, unit.ch) {
                buf[starter].ch = composite;
                buf[starter].end = buf[starter].end.max(unit.end);
                continue;
            }
        }
        if unit.ccc == 0 {
            starter = kept;
            has_starter = true;
        }
        last_ccc = unit.ccc;
        buf[kept] = unit;
        kept += 1;
    }
    buf.truncate(kept);
}

/// C++: `nfc_detail::NormalizeSegment` — normalizes `segment`, which starts at
/// input byte `start`, into `buf`. Returns false when it is already NFC.
fn normalize_segment(segment: &str, start: usize, buf: &mut Vec<Unit>) -> bool {
    buf.clear();
    for (index, c) in segment.char_indices() {
        let offset = start + index;
        decompose(c, offset as u32, (offset + c.len_utf8()) as u32, buf);
    }
    reorder_and_compose(buf);
    !buf.iter().map(|unit| unit.ch).eq(segment.chars())
}

/// C++: `nfc_detail::AppendSegment` — appends a normalized segment. A code
/// point maps no lower than the end of the input taken in by the code points
/// before it, so the map never decreases, and a token that ends inside an
/// expansion or a reordered run of marks still spans its whole source.
fn append_segment(buf: &[Unit], out: &mut String, mut pos_map: Option<&mut Vec<u32>>) {
    let mut covered = 0;
    for unit in buf {
        out.push(unit.ch);
        if let Some(pos_map) = pos_map.as_deref_mut() {
            pos_map.extend(std::iter::repeat_n(
                unit.offset.max(covered),
                unit.ch.len_utf8(),
            ));
        }
        covered = covered.max(unit.end);
    }
}

/// C++: `NormalizeNFC` — the NFC form of `input`, or `None` when it is
/// already NFC.
///
/// When a string is returned and `pos_map` is set, it gets `out.len() + 1`
/// entries: entry `i` is the byte of `input` that output byte `i` comes from,
/// the last entry is `input.len()`, and the entries never decrease. An
/// unchanged byte maps to itself. Every byte of a rewritten code point maps to
/// the input offset of its source code point (a composite: its starter's),
/// raised to the end of the input taken in by the code points before it in
/// the segment, so a span `[pos_map[a], pos_map[b])` reaches the end of every
/// input code point that output bytes `[a, b)` come from. Known limit: a mark
/// that canonical reordering moved after marks from later in the input maps
/// past its own bytes, so a token of that mark alone gets an empty span; only
/// the C++ and Rust tokenizers emit such tokens (PCRE2 counts every Mn as
/// `\w`, Rust keeps Other_Alphabetic marks such as U+05C1), rag_tokenizer.py
/// never does.
pub(crate) fn normalize_nfc(input: &str, mut pos_map: Option<&mut Vec<u32>>) -> Option<String> {
    let mut out: Option<String> = None;
    let mut copied = 0; // input[..copied] is already in out
    let mut buf = Vec::new();
    let mut flush_segment = |start: usize, end: usize, pos_map: &mut Option<&mut Vec<u32>>| {
        if !normalize_segment(&input[start..end], start, &mut buf) {
            return;
        }
        let out = out.get_or_insert_with(|| {
            if let Some(pos_map) = pos_map.as_deref_mut() {
                pos_map.clear();
                pos_map.reserve(input.len() + 1);
            }
            String::with_capacity(input.len())
        });
        out.push_str(&input[copied..start]);
        if let Some(pos_map) = pos_map.as_deref_mut() {
            pos_map.extend(copied as u32..start as u32);
        }
        append_segment(&buf, out, pos_map.as_deref_mut());
        copied = end;
    };

    // The quick check of the current segment: every code point has
    // NFC_Quick_Check=Yes and the non-starters are in canonical order.
    let mut segment_start = 0;
    let mut quick_check_yes = true;
    let mut last_ccc = 0;
    for (index, c) in input.char_indices() {
        let Some((ccc, yes)) = non_boundary(c) else {
            if !quick_check_yes {
                flush_segment(segment_start, index, &mut pos_map);
            }
            segment_start = index;
            quick_check_yes = true;
            last_ccc = 0;
            continue;
        };
        if !yes || (ccc != 0 && last_ccc > ccc) {
            quick_check_yes = false;
        }
        last_ccc = ccc;
    }
    if !quick_check_yes {
        flush_segment(segment_start, input.len(), &mut pos_map);
    }
    let mut out = out?;
    out.push_str(&input[copied..]);
    if let Some(pos_map) = pos_map {
        pos_map.extend(copied as u32..=input.len() as u32);
    }
    Some(out)
}
