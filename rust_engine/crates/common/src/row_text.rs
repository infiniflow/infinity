//! The stored text form of one row.
//!
//! A row is carried between the executor, the WAL and the block writer as one
//! string: each cell's stored text, joined by TAB. That join is **not
//! injective** — a VARCHAR value may itself contain a TAB, which is ordinary in
//! source code, CSV-ish data and JSON — and a stray TAB silently shifts every
//! following column of the row, so a reader sees values that belong to other
//! columns.
//!
//! Rows that need it are therefore written with a leading `\u{1}` marker and
//! their cells escaped (`\\` and `\t`). The marker cannot appear in a stored
//! cell: cells are rendered SQL text, which never starts a row with a control
//! character, and a row without a TAB or a backslash keeps the plain form —
//! exactly what earlier builds wrote, so old records still read back.
//!
//! Escaping on the write side and unescaping on every read side must stay in
//! step: [`encode`] is the only writer, [`split`] the only reader.

use std::borrow::Cow;

/// Marks a row whose cells are escaped. Never part of a rendered cell.
const MARKER: char = '\u{1}';

/// True when a cell would break the plain TAB-joined form.
fn needs_escape(cell: &str) -> bool {
    cell.contains('\t') || cell.contains('\\')
}

/// Joins a row's cells into its stored text.
pub fn encode(cells: &[String]) -> String {
    if !cells.iter().any(|cell| needs_escape(cell)) {
        return cells.join("\t");
    }
    let size: usize = cells.iter().map(|c| c.len() + 1).sum();
    let mut out = String::with_capacity(size + 1);
    out.push(MARKER);
    for (i, cell) in cells.iter().enumerate() {
        if i > 0 {
            out.push('\t');
        }
        for ch in cell.chars() {
            match ch {
                '\\' => out.push_str("\\\\"),
                '\t' => out.push_str("\\t"),
                other => out.push(other),
            }
        }
    }
    out
}

/// Splits a row's stored text back into cells.
pub fn split(text: &str) -> Vec<Cow<'_, str>> {
    let Some(body) = text.strip_prefix(MARKER) else {
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
    let Some(body) = text.strip_prefix(MARKER) else {
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

    #[test]
    fn a_row_of_old_data_is_read_unchanged() {
        // What an earlier build wrote: no marker, tabs are separators.
        let cells = split("varchar-one\t42\tvarchar-three");
        assert_eq!(cells.len(), 3);
        assert_eq!(cells[1].as_ref(), "42");
    }
}
