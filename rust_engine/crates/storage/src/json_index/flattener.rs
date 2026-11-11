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

/// Flattens the JSON text stored in a `json` cell.
///
/// Invalid JSON yields no terms rather than an error, matching the reference's
/// `if (json_obj)` guard around the recursive walk.
pub fn flatten_json(text: &str) -> Vec<FlattenedTerm> {
    // `serde_json` stops at 128 levels of nesting by default, while the
    // reference parses with `nlohmann::json`, which has no limit: a document
    // nested deeper (`test/sql/ddl/index/test_json_index.slt`'s long-term case
    // nests ~130 levels) is valid JSON and must index, not silently yield no
    // terms at all.
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
    if let Some(integer) = number.as_i64().or_else(|| number.as_u64().map(|u| u as i64)) {
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
                // `json_contains(doc, value)` matches these.
                "$.a:i:8000000000000001",
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

    #[test]
    fn object_members_are_visited_in_key_order() {
        let got = terms(r#"{"b":1,"a":2}"#);
        let a = got.iter().position(|term| term.starts_with("$.a")).unwrap();
        let b = got.iter().position(|term| term.starts_with("$.b")).unwrap();
        assert!(a < b, "members should be sorted by key: {got:?}");
    }
}
