//! Flattens a JSON document into the secondary-index terms the reference keeps
//! for a `json` column (`src/storage/json_index/json_flattener_impl.cpp`).
//!
//! The reference does not index a JSON document as one value: it expands the
//! document into one term per scalar and per path, and the column's index is a
//! **low-cardinality** chunk over those terms (a `json` column is forced onto
//! the low-cardinality path - `index_secondary.cppm:56` - and the
//! high-cardinality writer rejects it outright). A JSON index therefore reuses
//! the same roaring-bitmap segment a `BOOL` or a `cardinality='low'` column
//! uses; only the keys differ, by being variable-length term strings.
//!
//! Term grammar, byte-for-byte with the reference:
//!
//! * `$` is the root. An object member appends `.key`, an array element
//!   `[index]` (`$.a[0]`, never `[*]`).
//! * A scalar is `<path>:<tag>:<value>` with `s` for a string, `i` for an
//!   integer, `d` for a double, `b` for a boolean (`0`/`1`) and `n` for null
//!   (`<path>:n:`, no value).
//! * Integers and doubles are order-mapped into 16 uppercase hex digits, so
//!   lexicographic term order is numeric order - the same trick the chunk
//!   writers use for their numeric keys, applied to a text term.
//! * `<path>:p:` means "the path exists" (`json_exists_path`). Emitted for an
//!   object, for an array, and for every non-null scalar.
//! * An array additionally re-emits each of its elements once **without** the
//!   index, under the array's own path - that is what
//!   `json_contains(doc, value)` matches.
//!
//! Object members are visited in key order (the reference uses `nlohmann::json`,
//! whose object is a `std::map`; `serde_json`'s default map is a `BTreeMap`, so
//! the term order matches too).

use serde::Deserialize;
use serde_json::Value;

/// One flattened term: a key in the column's low-cardinality chunk.
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub struct FlattenedTerm {
    pub term: String,
}

/// The deepest nesting [`flatten_json`] parses: four times the reference
/// suite's ~130-level case, and far below the depth at which the recursive walk
/// would exhaust the stack.
const MAX_JSON_DEPTH: usize = 512;

/// The single term a document nested deeper than [`MAX_JSON_DEPTH`] carries.
///
/// Such a document is *valid* JSON that the flattener refuses to walk (the
/// depth bound is what protects the recursive walk's stack). It therefore has
/// no `path:tag:value` terms, and writing none at all would be a lie the index
/// acts on: a term-range probe would find no row for the document and drop it
/// before the residual filter above the index scan ever saw it - a false
/// negative for a row that may well match.
///
/// The row instead carries this sentinel, and every JSON candidate set unions
/// it in, so an over-depth document always survives to the residual filter
/// that answers the predicate exactly. It is an ordinary chunk key, so a
/// rebuild and a cold restart both keep it; nothing about the representation
/// has to be recovered separately.
///
/// `\u{0}` cannot start a real term - every path the flattener emits begins
/// with `$`, `[`, `.` or `:` (a root scalar) - so the sentinel sorts before
/// every real term and no predicate's range reaches it by accident.
pub const OVERFLOW_TERM: &str = "\u{0}overflow";

/// Whether `text` nests `{`/`[` more than `limit` deep.
///
/// The check walks the raw text, so it has to ignore brackets that are part of
/// a string - `"a{b"`, `"[1]"`, `"escaped \" quote {"` - or it would measure
/// characters instead of structure. A closing bracket without an opener only
/// lowers the count, and the parser rejects such input on its own.
fn exceeds_max_depth(text: &str, limit: usize) -> bool {
    let mut depth = 0usize;
    let mut in_string = false;
    let mut escaped = false;
    for byte in text.bytes() {
        if in_string {
            if escaped {
                escaped = false;
            } else if byte == b'\\' {
                escaped = true;
            } else if byte == b'"' {
                in_string = false;
            }
            continue;
        }
        match byte {
            b'"' => in_string = true,
            b'{' | b'[' => {
                depth += 1;
                if depth > limit {
                    return true;
                }
            }
            b'}' | b']' => depth = depth.saturating_sub(1),
            _ => {}
        }
    }
    false
}

/// Flattens the JSON text stored in a `json` cell.
///
/// Invalid JSON yields no terms rather than an error, matching the reference's
/// `if (json_obj)` guard around the recursive walk: such a cell cannot satisfy
/// a JSON predicate, so having no term is accurate.
///
/// A document nested past [`MAX_JSON_DEPTH`] is different - it is valid JSON
/// that may match, only its terms were never expanded - and yields
/// [`OVERFLOW_TERM`] so the index keeps it as a candidate instead of claiming
/// it matches nothing.
pub fn flatten_json(text: &str) -> Vec<FlattenedTerm> {
    // `serde_json` stops at 128 levels of nesting by default, while the
    // reference parses with `nlohmann::json`, which has no limit: a document
    // nested deeper (`test/sql/ddl/index/test_json_index.slt`'s long-term case
    // nests ~130 levels) is valid JSON and must index, not silently yield no
    // terms at all. Lifting the limit removes the guard against a stack
    // overflow on a hostile document, so the nesting is bounded here instead -
    // well above the reference's ~130 levels, well below what exhausts the
    // stack of the recursive walk below.
    if exceeds_max_depth(text, MAX_JSON_DEPTH) {
        // Valid JSON this walk will not enter. Emitting no term would let the
        // index claim the row matches nothing and drop it before the residual
        // filter ran, so the row carries the sentinel instead and stays a
        // candidate for every JSON predicate on the column.
        return vec![FlattenedTerm {
            term: OVERFLOW_TERM.to_string(),
        }];
    }
    let mut deserializer = serde_json::Deserializer::from_str(text);
    deserializer.disable_recursion_limit();
    let Ok(value) = Value::deserialize(&mut deserializer) else {
        return Vec::new();
    };
    let mut terms = Vec::new();
    flatten_recursive(&value, "", &mut terms);
    terms
}

/// The term strings only, which is what the index writer needs.
pub fn flatten_json_terms(text: &str) -> Vec<String> {
    flatten_json(text)
        .into_iter()
        .map(|term| term.term)
        .collect()
}

fn flatten_recursive(value: &Value, current_path: &str, terms: &mut Vec<FlattenedTerm>) {
    match value {
        Value::Object(members) => {
            for (key, child) in members {
                let child_path = if current_path.is_empty() {
                    format!("$.{key}")
                } else {
                    format!("{current_path}.{key}")
                };
                flatten_recursive(child, &child_path, terms);
            }
            let object_path = if current_path.is_empty() {
                "$"
            } else {
                current_path
            };
            terms.push(FlattenedTerm {
                term: format!("{object_path}:p:"),
            });
        }
        Value::Array(items) => {
            for (index, item) in items.iter().enumerate() {
                let child_path = format!("{current_path}[{index}]");
                flatten_recursive(item, &child_path, terms);
            }
            // The array's own path, for the two-argument `json_contains`.
            let parent_path = if current_path.is_empty() {
                "$"
            } else {
                current_path
            };
            for item in items {
                add_scalar_term(item, parent_path, terms);
                // `json_contains` compares an element against a literal, and
                // `50` and `50.0` are the same number to a caller: emit the
                // element under the other numeric tag as well, so either
                // spelling of the probe finds it.
                add_numeric_alias(item, parent_path, terms);
            }
            terms.push(FlattenedTerm {
                term: format!("{parent_path}:p:"),
            });
        }
        scalar => {
            add_scalar_term(scalar, current_path, terms);
            if !scalar.is_null() {
                terms.push(FlattenedTerm {
                    term: format!("{current_path}:p:"),
                });
            }
        }
    }
}

fn add_scalar_term(value: &Value, path: &str, terms: &mut Vec<FlattenedTerm>) {
    let term = match value {
        Value::String(text) => format!("{path}:s:{text}"),
        Value::Number(number) => {
            if let Some(integer) = number.as_i64() {
                format!("{path}:i:{}", encode_integer(integer))
            } else if let Some(integer) = number.as_u64() {
                // A JSON integer above `i64::MAX` is still an `:i:` term in the
                // reference (`is_number_unsigned`, cast to `int64_t`).
                format!("{path}:i:{}", encode_integer(integer as i64))
            } else if let Some(double) = number.as_f64() {
                format!("{path}:d:{}", encode_double(double))
            } else {
                return;
            }
        }
        Value::Bool(flag) => format!("{path}:b:{}", u8::from(*flag)),
        Value::Null => format!("{path}:n:"),
        // An object or array here goes through `flatten_recursive`, not this.
        _ => return,
    };
    terms.push(FlattenedTerm { term });
}

/// The same number under the other numeric tag, so that `json_contains` finds
/// an array element whichever way its literal was spelled.
///
/// An element written `50` is stored as `<path>:i:…` and a probe written `50.0`
/// as `<path>:d:…`, so the two would never meet. A value with no exact
/// counterpart (a fractional double, a non-finite one, an integer beyond
/// `2^53`) gets no alias: the predicate then falls back to the row filter
/// rather than selecting nothing.
fn add_numeric_alias(value: &Value, path: &str, terms: &mut Vec<FlattenedTerm>) {
    let Value::Number(number) = value else {
        return;
    };
    if let Some(integer) = number
        .as_i64()
        .or_else(|| number.as_u64().map(|u| u as i64))
    {
        terms.push(FlattenedTerm {
            term: format!("{path}:d:{}", encode_double(integer as f64)),
        });
    } else if let Some(double) = number.as_f64() {
        // `2^53` is the largest magnitude an `i64` holds exactly as an `f64`.
        if double.is_finite() && double.fract() == 0.0 && double.abs() <= 9_007_199_254_740_992.0 {
            terms.push(FlattenedTerm {
                term: format!("{path}:i:{}", encode_integer(double as i64)),
            });
        }
    }
}

/// `std::bit_cast<uint64_t>(value) ^ (1 << 63)`, printed as 16 uppercase hex
/// digits: signed order becomes lexicographic order.
fn encode_integer(value: i64) -> String {
    format!("{:016X}", (value as u64) ^ (1u64 << 63))
}

/// Sortable IEEE-754 bits, 16 uppercase hex digits. `-0.0` is normalised to
/// `0.0` first, so both spell the same term.
fn encode_double(value: f64) -> String {
    let value = if value == 0.0 { 0.0 } else { value };
    let bits = value.to_bits();
    let sortable = if bits & (1u64 << 63) != 0 {
        !bits
    } else {
        bits ^ (1u64 << 63)
    };
    format!("{sortable:016X}")
}

#[cfg(test)]
mod tests {
    use super::*;

    fn terms(text: &str) -> Vec<String> {
        flatten_json_terms(text)
    }

    #[test]
    fn scalar_document_emits_value_and_path_terms() {
        // A root scalar keeps the reference's empty path prefix: `$` is only
        // introduced inside an object or an array (`FlattenRecursive(value, "",
        // …)` -> `"" + ":i:" + …`). Matching it keeps the terms identical.
        assert_eq!(terms("1"), vec![":i:8000000000000001", ":p:"]);
        assert_eq!(terms("\"hi\""), vec![":s:hi", ":p:"]);
        assert_eq!(terms("true"), vec![":b:1", ":p:"]);
        // A null scalar has only its value term: the reference skips `:p:`.
        assert_eq!(terms("null"), vec![":n:"]);
    }

    #[test]
    fn nested_object_uses_dotted_paths_and_one_path_term_per_level() {
        assert_eq!(
            terms(r#"{"a":{"b":2}}"#),
            vec!["$.a.b:i:8000000000000002", "$.a.b:p:", "$.a:p:", "$:p:"]
        );
    }

    #[test]
    fn array_elements_carry_their_index_and_repeat_under_the_array_path() {
        assert_eq!(
            terms(r#"{"a":[1,"x"]}"#),
            vec![
                "$.a[0]:i:8000000000000001",
                "$.a[0]:p:",
                "$.a[1]:s:x",
                "$.a[1]:p:",
                // `json_contains(doc, value)` matches these; a numeric element
                // is also written under the other numeric tag, so `50` and
                // `50.0` both find it.
                "$.a:i:8000000000000001",
                "$.a:d:BFF0000000000000",
                "$.a:s:x",
                "$.a:p:",
                "$:p:",
            ]
        );
    }

    #[test]
    fn root_array_uses_dollar_as_its_parent_path() {
        // Elements of a root array are `[0]` (no `$`), while the array's own
        // path terms use `$`.
        assert_eq!(terms("[true]"), vec!["[0]:b:1", "[0]:p:", "$:b:1", "$:p:"]);
    }

    #[test]
    fn integer_terms_sort_numerically() {
        // The encoding's whole point: lexicographic order of the term suffixes
        // is numeric order, negatives first.
        let mut encoded: Vec<String> = [-5i64, -1, 0, 1, 10, 1000]
            .iter()
            .map(|value| encode_integer(*value))
            .collect();
        let sorted = {
            let mut copy = encoded.clone();
            copy.sort();
            copy
        };
        assert_eq!(encoded, sorted);
        encoded.dedup();
        assert_eq!(encode_integer(0), "8000000000000000");
        assert_eq!(encode_integer(-1), "7FFFFFFFFFFFFFFF");
    }

    #[test]
    fn double_terms_sort_numerically() {
        let values = [-1.5f64, -0.0, 0.0, 0.25, 1.5, 1e300];
        let mut encoded: Vec<String> = values.iter().map(|value| encode_double(*value)).collect();
        let sorted = {
            let mut copy = encoded.clone();
            copy.sort();
            copy
        };
        assert_eq!(encoded, sorted);
        // `-0.0` and `0.0` are one term, like the reference.
        assert_eq!(encode_double(-0.0), encode_double(0.0));
        encoded.dedup();
    }

    #[test]
    fn invalid_json_flattens_to_nothing() {
        assert!(terms("{not json").is_empty());
        assert!(terms("").is_empty());
    }

    /// The reference's long-term case nests ~130 levels, so the bound has to
    /// keep accepting it while `unbounded_depth` no longer protects the stack.
    #[test]
    fn documents_nested_past_the_reference_case_still_flatten() {
        let mut text = String::new();
        for _ in 0..130 {
            text.push_str(r#"{"k":"#);
        }
        text.push('1');
        for _ in 0..130 {
            text.push('}');
        }
        assert!(!exceeds_max_depth(&text, MAX_JSON_DEPTH));
        assert!(terms(&text)
            .iter()
            .any(|term| term.ends_with(":i:8000000000000001")));
    }

    /// A document past the bound is not "matches nothing": it is a row the
    /// index cannot decide, and it keeps exactly one term so a candidate set
    /// can find it.
    #[test]
    fn documents_beyond_the_bound_keep_their_candidate_term() {
        let mut text = "[".repeat(MAX_JSON_DEPTH + 1);
        text.push_str(&"]".repeat(MAX_JSON_DEPTH + 1));
        assert!(exceeds_max_depth(&text, MAX_JSON_DEPTH));
        assert_eq!(terms(&text), vec![OVERFLOW_TERM.to_string()]);
    }

    /// The sentinel has to sort before every real term, or a predicate's term
    /// range could start matching it and treat an undecided row as a hit.
    #[test]
    fn the_candidate_term_sorts_before_every_real_term() {
        let real = terms(r#"{"a":1,"b":[true,"x"],"c":null}"#);
        assert!(!real.is_empty());
        for term in &real {
            assert!(
                OVERFLOW_TERM.as_bytes() < term.as_bytes(),
                "{OVERFLOW_TERM:?} must sort before {term:?}"
            );
        }
    }

    /// Invalid JSON still yields nothing: unlike an over-depth document it is
    /// not a row the predicate could be true for.
    #[test]
    fn invalid_json_keeps_yielding_no_terms() {
        assert!(terms("{deep but broken").is_empty());
    }

    /// Brackets inside a string are text, and an escaped quote must not end the
    /// string: the check measures structure, not characters.
    #[test]
    fn brackets_inside_strings_do_not_count_as_depth() {
        let deep_text = format!("[\"{}\"]", "[".repeat(MAX_JSON_DEPTH + 10));
        assert!(!exceeds_max_depth(&deep_text, MAX_JSON_DEPTH));
        assert!(terms(&deep_text).iter().any(|term| term.contains(":s:[")));

        let escaped = format!("[\"quote \\\" then {}\"]", "{".repeat(MAX_JSON_DEPTH + 10));
        assert!(!exceeds_max_depth(&escaped, MAX_JSON_DEPTH));
    }

    #[test]
    fn object_members_are_visited_in_key_order() {
        let got = terms(r#"{"b":1,"a":2}"#);
        let a = got.iter().position(|term| term.starts_with("$.a")).unwrap();
        let b = got.iter().position(|term| term.starts_with("$.b")).unwrap();
        assert!(a < b, "members should be sorted by key: {got:?}");
    }
}
