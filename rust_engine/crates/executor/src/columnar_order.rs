//! Column-at-a-time `ORDER BY` keys.
//!
//! The row path builds every sort key by refilling a whole row buffer and
//! evaluating the key expression on it - for `ORDER BY c1, c2` that is
//! `2 x rows` full row materialisations, and for a varchar key one `String`
//! clone per row - and then compares `Value`s with `partial_cmp_value`, which
//! re-parses text as a number when it can.
//!
//! A key that is a plain column needs none of that: the decoded buffers already
//! hold comparable values, so [`sort_key`] only *borrows* them and
//! [`SortKey::compare`] compares in the right domain. No key vector, no `Value`,
//! no per-row allocation.
//!
//! The comparisons are the row path's, deliberately: numbers compare as `f64`
//! (so two `i64`s past 2^53 that round to the same float tie, and the stable
//! index tie-break decides - an "exact" `i64` compare would order rows
//! differently from every existing answer), a NULL ties with everything (`None`,
//! which the caller treats as "look at the next key"), and text orders by bytes.
//! `None` from [`sort_key`] means the key is not a plain column (an expression,
//! a literal) and the caller keeps the row path.

use std::cmp::Ordering;

use expression::Expr;

use crate::columnar::{Batch, Bitmap, ColumnData, Utf8Column};

/// One `ORDER BY` key, borrowing the column's buffers: no per-row allocation,
/// no per-row copy, so sorting 8 M rows does not also allocate 8 M keys.
pub(crate) enum SortKey<'a> {
    /// Every `i32` is exact in `f64`, so comparing them as `i32` is the row
    /// path's ordering without widening a copy.
    Int32 {
        values: &'a [i32],
        valid: &'a Bitmap,
    },
    /// `i64` compares as `f64`, which is what `partial_cmp_value` does.
    Int64 {
        values: &'a [i64],
        valid: &'a Bitmap,
    },
    Float64 {
        values: &'a [f64],
        valid: &'a Bitmap,
    },
    Bool {
        values: &'a [bool],
        valid: &'a Bitmap,
    },
    Utf8 {
        text: &'a Utf8Column,
        valid: &'a Bitmap,
    },
}

impl SortKey<'_> {
    /// `a` vs `b` at this key; `None` means "unknown" - a NULL on either side,
    /// or a NaN - which the caller treats as a tie and moves to the next key.
    pub(crate) fn compare(&self, a: usize, b: usize) -> Option<Ordering> {
        match self {
            SortKey::Int32 { values, valid } => {
                if !valid.get(a) || !valid.get(b) {
                    return None;
                }
                Some(values[a].cmp(&values[b]))
            }
            SortKey::Int64 { values, valid } => {
                if !valid.get(a) || !valid.get(b) {
                    return None;
                }
                (values[a] as f64).partial_cmp(&(values[b] as f64))
            }
            SortKey::Float64 { values, valid } => {
                if !valid.get(a) || !valid.get(b) {
                    return None;
                }
                values[a].partial_cmp(&values[b])
            }
            SortKey::Bool { values, valid } => {
                if !valid.get(a) || !valid.get(b) {
                    return None;
                }
                Some(values[a].cmp(&values[b]))
            }
            SortKey::Utf8 { text, valid } => {
                if !valid.get(a) || !valid.get(b) {
                    return None;
                }
                Some(text.get(a).cmp(text.get(b)))
            }
        }
    }
}

/// Reads `expr` as a sort key over `batch`, or `None` when it is not a plain
/// column this module knows how to read.
pub(crate) fn sort_key<'a>(
    expr: &Expr,
    batch: &'a Batch,
    column: &dyn Fn(usize) -> Option<usize>,
) -> Option<SortKey<'a>> {
    let Expr::Column(pos) = expr else {
        return None;
    };
    match &batch.data[column(*pos)?] {
        ColumnData::Int32 { values, valid } => Some(SortKey::Int32 { values, valid }),
        ColumnData::Int64 { values, valid } => Some(SortKey::Int64 { values, valid }),
        ColumnData::Float64 { values, valid } => Some(SortKey::Float64 { values, valid }),
        ColumnData::Bool { values, valid } => Some(SortKey::Bool { values, valid }),
        ColumnData::Utf8 { text, valid } => Some(SortKey::Utf8 { text, valid }),
        // No type yet, so no key: the caller's row path decides (it will answer
        // "everything is NULL").
        ColumnData::Empty { .. } => None,
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::columnar::Batch;
    use expression::Value;

    /// The row path this module replaces, verbatim: `partial_cmp_value` on the
    /// cells, `None` = tie, ties broken by the original row index.
    fn row_path_order(rows: &[Vec<Value>], keys: &[(usize, bool)]) -> Vec<usize> {
        let mut order: Vec<usize> = (0..rows.len()).collect();
        order.sort_by(|a, b| {
            for (pos, asc) in keys {
                let Some(ord) = rows[*a][*pos].partial_cmp_value(&rows[*b][*pos]) else {
                    continue;
                };
                if ord != Ordering::Equal {
                    return if *asc { ord } else { ord.reverse() };
                }
            }
            a.cmp(b)
        });
        order
    }

    fn columnar_order(rows: &[Vec<Value>], keys: &[(usize, bool)]) -> Vec<usize> {
        let batch = Batch::from_rows(rows.to_vec());
        let column = |pos: usize| (pos < batch.data.len()).then_some(pos);
        let sort_keys: Vec<SortKey> = keys
            .iter()
            .map(|(pos, _)| {
                sort_key(&Expr::Column(*pos), &batch, &column).expect("a plain column key")
            })
            .collect();
        let mut order: Vec<usize> = (0..rows.len()).collect();
        order.sort_by(|a, b| {
            for (k, (_, asc)) in keys.iter().enumerate() {
                let Some(ord) = sort_keys[k].compare(*a, *b) else {
                    continue;
                };
                if ord != Ordering::Equal {
                    return if *asc { ord } else { ord.reverse() };
                }
            }
            a.cmp(b)
        });
        order
    }

    fn rows() -> Vec<Vec<Value>> {
        vec![
            // id, name, score, flag
            vec![
                Value::Int(3),
                Value::Str("b".into()),
                Value::Float(1.5),
                Value::Bool(true),
            ],
            vec![
                Value::Int(1),
                Value::Null,
                Value::Float(f64::NAN),
                Value::Null,
            ],
            vec![
                Value::Int(2),
                Value::Str("a".into()),
                Value::Null,
                Value::Bool(false),
            ],
            // Past 2^53: the row path compares as `f64`, so 2^53 + 1 and 2^53
            // tie there. An "exact" compare here would reorder these two and
            // this table is what keeps the two paths honest.
            vec![
                Value::Int(9_007_199_254_740_993),
                Value::Str("c".into()),
                Value::Float(2.5),
                Value::Bool(true),
            ],
            vec![
                Value::Int(9_007_199_254_740_992),
                Value::Str("a".into()),
                Value::Float(2.5),
                Value::Bool(false),
            ],
        ]
    }

    /// Every key shape, ascending and descending, single and composite, is the
    /// same permutation as the row path - including the NULL ties and the
    /// `f64`-coerced integers past 2^53.
    #[test]
    fn the_columnar_order_matches_the_row_path() {
        let rows = rows();
        for keys in [
            vec![(0usize, true)],
            vec![(0, false)],
            vec![(1, true)],
            vec![(1, false)],
            vec![(2, true)],
            vec![(3, false)],
            vec![(1, true), (0, false)],
            vec![(0, true), (2, false)],
            vec![(3, true), (1, true), (0, false)],
        ] {
            assert_eq!(
                columnar_order(&rows, &keys),
                row_path_order(&rows, &keys),
                "keys {keys:?}"
            );
        }
    }

    /// The trap the `f64` coercion creates, pinned: `2^53 + 1` and `2^53` tie in
    /// the row path, so the stable index order decides. If someone "fixes" the
    /// integer compare, this test says the answers would change.
    #[test]
    fn integers_past_2_53_keep_the_row_paths_tie() {
        let rows = rows();
        let order = columnar_order(&rows, &[(0, true)]);
        // 1, 2, 3, then 2^53+1 and 2^53 tie and keep their row order (3, 4).
        assert_eq!(order, vec![1, 2, 0, 3, 4]);
        let equal = rows[3][0].partial_cmp_value(&rows[4][0]);
        assert_eq!(equal, Some(Ordering::Equal), "the row path ties them");
    }

    /// An expression key is not read here: the caller keeps the row path.
    #[test]
    fn an_expression_key_is_not_read_here() {
        let rows = rows();
        let batch = Batch::from_rows(rows);
        let column = |pos: usize| (pos < batch.data.len()).then_some(pos);
        let expr = Expr::Binary(
            Box::new(Expr::Column(0)),
            parser::BinOp::Add,
            Box::new(Expr::Literal(Value::Int(1))),
        );
        assert!(sort_key(&expr, &batch, &column).is_none());
    }
}
