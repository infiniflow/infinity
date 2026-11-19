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

//! Column-at-a-time evaluation of filter predicates.
//!
//! `Expr::eval` is row-at-a-time: the caller materialises every cell of a row
//! into `Vec<Value>` and walks the tree on it. For a 40 k-row batch that is 40 k
//! row buffers and, for a varchar column, one `String` per cell, before a single
//! row is decided.
//!
//! This module evaluates the same predicate over a whole [`Batch`] and returns
//! one mask. It covers the shapes a `WHERE` actually takes - `col <op> literal`,
//! `AND`/`OR`/`NOT`, `IS [NOT] NULL`, and a boolean column used as the predicate
//! itself - and returns `None` for everything else, which sends the caller back
//! to the row path. Coverage can therefore grow one shape at a time without a
//! call site changing.
//!
//! The mask is three-valued (`None` = unknown) so `NOT (c = 1)` keeps a NULL `c`
//! out, exactly as `Expr::eval` does; a plain `Vec<bool>` cannot express that.
//! [`matches`] is the projection onto "the row is kept".

use expression::{Expr, Value};
use parser::CmpOp;

use crate::columnar::Batch;

/// The mask `expr` produces over `batch`, or `None` when this module does not
/// evaluate that shape.
///
/// `column` maps a schema position to the index of the batch column carrying it.
pub(crate) fn predicate_mask(
    expr: &Expr,
    batch: &Batch,
    column: &dyn Fn(usize) -> Option<usize>,
) -> Option<Vec<Option<bool>>> {
    match expr {
        Expr::Cmp(a, op, b) => {
            let (pos, op, value) = column_const(a, *op, b)?;
            batch.data[column(pos)?].cmp_mask(op, value)
        }
        Expr::And(a, b) => {
            let left = predicate_mask(a, batch, column)?;
            let right = predicate_mask(b, batch, column)?;
            Some(and_mask(&left, &right))
        }
        Expr::Or(a, b) => {
            let left = predicate_mask(a, batch, column)?;
            let right = predicate_mask(b, batch, column)?;
            Some(or_mask(&left, &right))
        }
        Expr::Not(a) => Some(not_mask(&predicate_mask(a, batch, column)?)),
        Expr::IsNull(a, neg) => {
            let Expr::Column(pos) = **a else {
                return None;
            };
            let nulls = batch.data[column(pos)?].null_mask();
            Some(
                nulls
                    .into_iter()
                    .map(|is_null| Some(is_null != *neg))
                    .collect(),
            )
        }
        // A bare boolean column: `WHERE flag` (NULL is "not true", not false).
        Expr::Column(pos) => batch.data[column(*pos)?].bool_mask(),
        Expr::Literal(Value::Bool(value)) => Some(vec![Some(*value); batch.row_count]),
        _ => None,
    }
}

/// The rows kept by a three-valued mask: `Some(true)` only.
pub(crate) fn matches(mask: &[Option<bool>]) -> Vec<bool> {
    mask.iter().map(|k| *k == Some(true)).collect()
}

/// `col <op> literal` from `a <op> b`, flipping the operator when the literal is
/// on the left (`1 < c` is `c > 1`).
fn column_const<'a>(a: &'a Expr, op: CmpOp, b: &'a Expr) -> Option<(usize, CmpOp, &'a Value)> {
    match (a, b) {
        (Expr::Column(pos), Expr::Literal(value)) => Some((*pos, op, value)),
        (Expr::Literal(value), Expr::Column(pos)) => Some((*pos, flip(op), value)),
        _ => None,
    }
}

fn flip(op: CmpOp) -> CmpOp {
    match op {
        CmpOp::Eq => CmpOp::Eq,
        CmpOp::Ne => CmpOp::Ne,
        CmpOp::Lt => CmpOp::Gt,
        CmpOp::Le => CmpOp::Ge,
        CmpOp::Gt => CmpOp::Lt,
        CmpOp::Ge => CmpOp::Le,
    }
}

/// `a AND b`: `false` absorbs, `true AND true` is `true`, everything else unknown.
fn and_mask(a: &[Option<bool>], b: &[Option<bool>]) -> Vec<Option<bool>> {
    a.iter()
        .zip(b)
        .map(|(x, y)| match (x, y) {
            (Some(false), _) | (_, Some(false)) => Some(false),
            (Some(true), Some(true)) => Some(true),
            _ => None,
        })
        .collect()
}

/// `a OR b`: `true` absorbs, `false OR false` is `false`, everything else unknown.
fn or_mask(a: &[Option<bool>], b: &[Option<bool>]) -> Vec<Option<bool>> {
    a.iter()
        .zip(b)
        .map(|(x, y)| match (x, y) {
            (Some(true), _) | (_, Some(true)) => Some(true),
            (Some(false), Some(false)) => Some(false),
            _ => None,
        })
        .collect()
}

/// `NOT a`: unknown stays unknown.
fn not_mask(a: &[Option<bool>]) -> Vec<Option<bool>> {
    a.iter().map(|x| x.map(|v| !v)).collect()
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Every shape the module claims to answer, checked against the row path it
    /// replaces. The rows include a NULL in each interesting role: a compared
    /// cell, a boolean predicate cell, and a cell `IS NULL` looks at.
    fn rows() -> Vec<Vec<Value>> {
        vec![
            vec![
                Value::Int(1),
                Value::Str("a".into()),
                Value::Bool(true),
                Value::Null,
            ],
            vec![
                Value::Int(2),
                Value::Str("b".into()),
                Value::Bool(false),
                Value::Int(5),
            ],
            vec![
                Value::Int(3),
                Value::Str("c".into()),
                Value::Null,
                Value::Int(6),
            ],
        ]
    }

    fn col(expr: Expr) -> Box<Expr> {
        Box::new(expr)
    }

    fn cases() -> Vec<Expr> {
        vec![
            // Equality and every comparison, against a matching and a missing key.
            Expr::Cmp(
                col(Expr::Column(0)),
                CmpOp::Eq,
                col(Expr::Literal(Value::Int(2))),
            ),
            Expr::Cmp(
                col(Expr::Column(0)),
                CmpOp::Ne,
                col(Expr::Literal(Value::Int(2))),
            ),
            Expr::Cmp(
                col(Expr::Column(0)),
                CmpOp::Gt,
                col(Expr::Literal(Value::Int(1))),
            ),
            Expr::Cmp(
                col(Expr::Column(0)),
                CmpOp::Ge,
                col(Expr::Literal(Value::Int(3))),
            ),
            Expr::Cmp(
                col(Expr::Column(0)),
                CmpOp::Lt,
                col(Expr::Literal(Value::Int(3))),
            ),
            Expr::Cmp(
                col(Expr::Column(0)),
                CmpOp::Le,
                col(Expr::Literal(Value::Int(1))),
            ),
            // The literal on the left flips the operator.
            Expr::Cmp(
                col(Expr::Literal(Value::Int(2))),
                CmpOp::Lt,
                col(Expr::Column(0)),
            ),
            // Text, and the implicit numeric coercion `'2'` carries.
            Expr::Cmp(
                col(Expr::Column(1)),
                CmpOp::Eq,
                col(Expr::Literal(Value::Str("b".into()))),
            ),
            Expr::Cmp(
                col(Expr::Column(1)),
                CmpOp::Gt,
                col(Expr::Literal(Value::Str("a".into()))),
            ),
            Expr::Cmp(
                col(Expr::Column(0)),
                CmpOp::Eq,
                col(Expr::Literal(Value::Str("2".into()))),
            ),
            // A NULL cell makes the comparison unknown, not false.
            Expr::Cmp(
                col(Expr::Column(3)),
                CmpOp::Gt,
                col(Expr::Literal(Value::Int(5))),
            ),
            Expr::Cmp(
                col(Expr::Column(3)),
                CmpOp::Eq,
                col(Expr::Literal(Value::Null)),
            ),
            // Null tests, both polarities.
            Expr::IsNull(col(Expr::Column(3)), false),
            Expr::IsNull(col(Expr::Column(3)), true),
            // A boolean column used as the predicate, with a NULL cell.
            Expr::Column(2),
            Expr::Cmp(
                col(Expr::Column(2)),
                CmpOp::Eq,
                col(Expr::Literal(Value::Bool(true))),
            ),
            // Combinations, including NOT over an unknown.
            Expr::And(
                col(Expr::Cmp(
                    col(Expr::Column(0)),
                    CmpOp::Gt,
                    col(Expr::Literal(Value::Int(1))),
                )),
                col(Expr::IsNull(col(Expr::Column(3)), false)),
            ),
            Expr::Or(
                col(Expr::Cmp(
                    col(Expr::Column(0)),
                    CmpOp::Eq,
                    col(Expr::Literal(Value::Int(1))),
                )),
                col(Expr::Cmp(
                    col(Expr::Column(1)),
                    CmpOp::Eq,
                    col(Expr::Literal(Value::Str("c".into()))),
                )),
            ),
            Expr::Not(col(Expr::Cmp(
                col(Expr::Column(3)),
                CmpOp::Gt,
                col(Expr::Literal(Value::Int(4))),
            ))),
            Expr::Not(col(Expr::Column(2))),
            // Fully unknown: every row is excluded, and `NOT` of it stays unknown.
            Expr::And(
                col(Expr::Cmp(
                    col(Expr::Column(3)),
                    CmpOp::Eq,
                    col(Expr::Literal(Value::Null)),
                )),
                col(Expr::Literal(Value::Bool(true))),
            ),
        ]
    }

    /// The mask agrees with `Expr::eval_predicate` row for row, and `matches`
    /// keeps exactly the rows the row path keeps.
    #[test]
    fn every_mask_agrees_with_the_row_path() {
        let rows = rows();
        let batch = Batch::from_rows(rows.clone());
        let column = |pos: usize| (pos < batch.data.len()).then_some(pos);
        for expr in cases() {
            let Some(mask) = predicate_mask(&expr, &batch, &column) else {
                panic!("{expr:?} should be evaluated column-wise");
            };
            let kept = matches(&mask);
            for (r, row) in rows.iter().enumerate() {
                assert_eq!(
                    kept[r],
                    expr.eval_predicate(row),
                    "row {r} of {expr:?}: kept={} mask={:?} row={row:?}",
                    kept[r],
                    mask[r]
                );
            }
        }
    }

    /// The trap a `Vec<bool>` mask cannot express: `NOT (c > 4)` on a NULL `c`
    /// is unknown, so the row stays out - `!false` would let it in.
    #[test]
    fn not_over_an_unknown_cell_keeps_the_row_out() {
        let rows = rows();
        let batch = Batch::from_rows(rows.clone());
        let column = |pos: usize| (pos < batch.data.len()).then_some(pos);
        let expr = Expr::Not(col(Expr::Cmp(
            col(Expr::Column(3)),
            CmpOp::Gt,
            col(Expr::Literal(Value::Int(4))),
        )));
        let mask = predicate_mask(&expr, &batch, &column).expect("evaluated column-wise");
        assert_eq!(mask[0], None, "NULL c is unknown, not false");
        assert!(!matches(&mask)[0], "the row must stay out");
    }

    /// A shape the module does not evaluate hands the caller the row path.
    #[test]
    fn an_unhandled_shape_returns_none() {
        let batch = Batch::from_rows(rows());
        let column = |pos: usize| (pos < batch.data.len()).then_some(pos);
        let like = Expr::Like(
            col(Expr::Column(1)),
            col(Expr::Literal(Value::Str("a%".into()))),
            None,
            false,
        );
        assert!(predicate_mask(&like, &batch, &column).is_none());
        // A column the batch does not carry is not guessed at either.
        let cmp = Expr::Cmp(
            col(Expr::Column(9)),
            CmpOp::Eq,
            col(Expr::Literal(Value::Int(1))),
        );
        assert!(predicate_mask(&cmp, &batch, &column).is_none());
    }
}
