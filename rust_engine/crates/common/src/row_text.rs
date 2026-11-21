//! The stored text form of one row.
//!
//! A row is carried between the executor, the WAL and the block writer as one
//! string: each cell's stored text, joined by TAB. That join is **not
//! injective** — a VARCHAR value may itself contain a TAB, which is ordinary in
//! source code, CSV-ish data and JSON — and a stray TAB silently shifts every
//! following column of the row, so a reader sees values that belong to other
//! columns.
//!
//! Rows that need it are therefore written in a marked form:
//!
//! ```text
//! MARKER '1' <body> MARKER
//! ```
//!
//! `MARKER '1'` is the prologue (the `1` is the format's version) and the
//! trailing `MARKER` closes it. Inside the body the escapes are `\\`, `\t` and
//! `\m`, so a `MARKER` never appears raw between the two — which is what makes
//! the form recognisable: [`split`] and [`decode_single`] accept a string as
//! marked only when both the prologue **and** the closing marker are there, and
//! the writer guarantees the closing marker cannot come from a cell. A row
//! without a TAB, a backslash or a marker keeps the plain form, exactly what
//! earlier builds wrote, so old records still read back.
//!
//! Escaping on the write side and unescaping on every read side must stay in
//! step: [`encode`] is the only writer, [`split`] and [`decode_single`] the only
//! readers.

use std::borrow::Cow;

/// The format's marker character. [`encode`] escapes it wherever a cell carries
/// one, so it only ever appears as the prologue's first byte and the terminator.
const MARKER: char = '\u{1}';

/// Prologue byte after `MARKER`: the format version a marked row is written in.
const VERSION: char = '1';

/// Escape for a `MARKER` inside a cell (`\m`), since a raw one would end the
/// marked form early.
const ESCAPED_MARKER: &str = "\\m";

/// True when a cell would break the plain TAB-joined form, or when writing it
/// plainly would make the row look marked.
fn needs_escape(cell: &str) -> bool {
    cell.contains('\t') || cell.contains('\\') || cell.contains(MARKER)
}

/// The body of a marked row, or `None` when the text is a plain row.
///
/// Both the prologue and the terminator have to be present: a cell that starts
/// with a `MARKER` is escaped by [`encode`], so a marked row never carries a raw
/// one inside, and requiring the pair keeps a plain row that happens to begin
/// with the marker (a value written by a build that did not escape it) from
/// being unescaped as if it were marked.
fn marked_body(text: &str) -> Option<&str> {
    let body = text
        .strip_prefix(MARKER)?
        .strip_prefix(VERSION)?
        .strip_suffix(MARKER)?;
    Some(body)
}

/// Joins a row's cells into its stored text.
pub fn encode(cells: &[String]) -> String {
    if !cells.iter().any(|cell| needs_escape(cell)) {
        return cells.join("\t");
    }
    let size: usize = cells.iter().map(|c| c.len() + 1).sum();
    let mut out = String::with_capacity(size + 2);
    out.push(MARKER);
    out.push(VERSION);
    for (i, cell) in cells.iter().enumerate() {
        if i > 0 {
            out.push('\t');
        }
        for ch in cell.chars() {
            match ch {
                '\\' => out.push_str("\\\\"),
                '\t' => out.push_str("\\t"),
                MARKER => out.push_str(ESCAPED_MARKER),
                other => out.push(other),
            }
        }
    }
    out.push(MARKER);
    out
}

/// True when `text` is in the marked form, i.e. it *is* a whole row's stored
/// text rather than one rendered cell.
pub fn is_marked(text: &str) -> bool {
    marked_body(text).is_some()
}

/// Splits a row's stored text back into cells.
pub fn split(text: &str) -> Vec<Cow<'_, str>> {
    let Some(body) = marked_body(text) else {
        return text.split('\t').map(Cow::Borrowed).collect();
    };
    let mut out: Vec<Cow<'_, str>> = Vec::new();
    let mut cur = String::new();
    let mut chars = body.chars();
    while let Some(ch) = chars.next() {
        match ch {
            '\\' => unescape_one(&mut chars, &mut cur),
            '\t' => out.push(Cow::Owned(std::mem::take(&mut cur))),
            other => cur.push(other),
        }
    }
    out.push(Cow::Owned(cur));
    out
}

/// The whole row when it cannot hold a separator: a one-column table, or the
/// stored row text itself (which the port keeps as one varchar cell). Escapes
/// are still undone, because the writer may have escaped that cell.
pub fn decode_single(text: &str) -> Cow<'_, str> {
    let Some(body) = marked_body(text) else {
        return Cow::Borrowed(text);
    };
    let mut out = String::with_capacity(body.len());
    let mut chars = body.chars();
    while let Some(ch) = chars.next() {
        match ch {
            '\\' => unescape_one(&mut chars, &mut out),
            other => out.push(other),
        }
    }
    Cow::Owned(out)
}

/// Undoes one escape sequence, keeping an unknown one verbatim: nothing else is
/// produced here, and guessing would lose a character.
fn unescape_one(chars: &mut std::str::Chars<'_>, out: &mut String) {
    match chars.next() {
        Some('\\') => out.push('\\'),
        Some('t') => out.push('\t'),
        Some('m') => out.push(MARKER),
        Some(other) => {
            out.push('\\');
            out.push(other);
        }
        None => out.push('\\'),
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn round_trip(cells: &[&str]) {
        let owned: Vec<String> = cells.iter().map(|c| c.to_string()).collect();
        let text = encode(&owned);
        let back = split(&text);
        assert_eq!(back.len(), cells.len(), "cells: {cells:?} text: {text:?}");
        for (got, want) in back.iter().zip(cells) {
            assert_eq!(got.as_ref(), *want, "text: {text:?}");
        }
        // The single-cell reader has to agree with the split reader.
        if cells.len() == 1 {
            assert_eq!(decode_single(&text).as_ref(), cells[0]);
        }
    }

    fn marked(text: &str) -> bool {
        text.starts_with(MARKER) && text.ends_with(MARKER)
    }

    #[test]
    fn plain_rows_keep_the_old_form() {
        let cells = vec!["a".to_string(), "b".to_string()];
        assert_eq!(encode(&cells), "a\tb");
        assert!(!encode(&cells).starts_with(MARKER));
    }

    #[test]
    fn a_tab_inside_a_cell_survives() {
        round_trip(&["path", "line one\n\tline two", "code"]);
        round_trip(&["\t", "\t\t", ""]);
    }

    #[test]
    fn a_backslash_survives() {
        round_trip(&[r"C:\temp\new", r"a\tb", r"\\"]);
    }

    #[test]
    fn empty_cells_keep_their_positions() {
        round_trip(&["", "", ""]);
        round_trip(&["", "x", ""]);
    }

    /// A first cell that begins with the marker used to be written plainly and
    /// then read back as a marked row, which dropped it and unescaped the rest.
    #[test]
    fn a_first_cell_that_begins_with_the_marker_is_escaped() {
        round_trip(&["\u{1}plain-looking", "b"]);
        round_trip(&["\u{1}", ""]);
        round_trip(&["\u{1}1", "b"]);
        // Even a lone cell, where the plain form has no separator to protect.
        round_trip(&["\u{1}only"]);
    }

    /// A marker anywhere in a cell would be read as the closing marker.
    #[test]
    fn a_marker_inside_a_cell_is_escaped() {
        round_trip(&["a\u{1}b", "c"]);
        round_trip(&["a", "b\u{1}"]);
        round_trip(&["\u{1}", "\u{1}"]);
        let text = encode(&["a\u{1}b".to_string()]);
        assert_eq!(text.matches(MARKER).count(), 2, "text: {text:?}");
    }

    #[test]
    fn a_row_of_old_data_is_read_unchanged() {
        // What an earlier build wrote: no marker, tabs are separators.
        let cells = split("varchar-one\t42\tvarchar-three");
        assert_eq!(cells.len(), 3);
        assert_eq!(cells[1].as_ref(), "42");
        assert!(!marked("varchar-one\t42\tvarchar-three"));
    }

    /// A plain row that merely starts with the marker is not unescaped: the
    /// prologue and the terminator are both required, and the writer never
    /// emits a marked row without them.
    #[test]
    fn a_plain_row_that_starts_with_the_marker_is_not_marked() {
        let text = "\u{1}legacy\tvalue";
        let cells = split(text);
        assert_eq!(cells.len(), 2, "text: {text:?}");
        assert_eq!(cells[0].as_ref(), "\u{1}legacy");
        assert_eq!(decode_single(text).as_ref(), text);
    }

    /// The prologue alone is not enough: a cell that carried the version byte
    /// after the marker used to end the body early instead of the row's end.
    #[test]
    fn a_prologue_without_the_closing_marker_is_plain() {
        let cells = split("\u{1}1no-terminator");
        assert_eq!(cells.len(), 1);
        assert_eq!(cells[0].as_ref(), "\u{1}1no-terminator");
    }
}
