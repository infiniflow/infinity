//! `infinity-planner`: builds a logical plan from a parsed `SELECT`.
//! Operators are deliberately simple (M2); the Tokio executor consumes this tree.

#![forbid(unsafe_code)]

pub mod optimize;

use infinity_common::{Error, Result};
use infinity_expression::{Expr, Value};
use infinity_parser::{AggFunc, BoolOp, ExprAst, JoinClause, OrderKey, Predicate, SelectItem};

/// A simple logical plan.
#[derive(Debug, Clone, PartialEq)]
pub enum LogicalPlan {
    /// A constant relation (`SELECT 1, 'a'`).
    Values {
        columns: Vec<String>,
        rows: Vec<Vec<Value>>,
    },
    /// Full scan of a table with the given schema.
    Scan {
        table: String,
        columns: Vec<String>,
    },
    /// Cross product of two inputs, kept only where `left_key == right_key`.
    Join {
        left: Box<LogicalPlan>,
        right: Box<LogicalPlan>,
        left_key: usize,
        right_key: usize,
        /// number of columns produced by the left input (to offset right keys)
        left_width: usize,
    },
    Filter {
        input: Box<LogicalPlan>,
        predicate: Expr,
    },
    Projection {
        input: Box<LogicalPlan>,
        exprs: Vec<Expr>,
        names: Vec<String>,
    },
    Aggregate {
        input: Box<LogicalPlan>,
        group_key: Option<usize>,
        group_name: Option<String>,
        aggs: Vec<(AggFunc, Option<usize>, String)>,
    },
    Sort {
        input: Box<LogicalPlan>,
        key: usize,
        asc: bool,
    },
    Limit {
        input: Box<LogicalPlan>,
        n: u64,
    },
}

/// Concatenated schema of all tables in a FROM/JOIN.
pub fn combined_schema(tables: &[(String, Vec<String>)]) -> Vec<String> {
    tables
        .iter()
        .flat_map(|(_, cols)| cols.iter().cloned())
        .collect()
}

/// Resolves a column name in `schema`; ambiguous names are rejected.
pub fn resolve_column(schema: &[String], name: &str) -> Result<usize> {
    let mut hits = schema
        .iter()
        .enumerate()
        .filter(|(_, c)| c.as_str() == name);
    match (hits.next(), hits.next()) {
        (Some((i, _)), None) => Ok(i),
        (Some(_), Some(_)) => Err(Error::Invalid(format!("ambiguous column '{name}'"))),
        _ => Err(Error::Invalid(format!("unknown column '{name}'"))),
    }
}

/// Compiles a parsed scalar expression against a table schema.
pub fn compile_expr(e: &ExprAst, schema: &[String]) -> Result<Expr> {
    Ok(match e {
        ExprAst::Column(c) => Expr::Column(resolve_column(schema, c)?),
        ExprAst::Literal(l) => Expr::Literal(Value::from_literal(l)),
        ExprAst::Func { name, args } => {
            // SEARCH MATCH pseudo-columns are exposed as hidden scan columns.
            let hidden = match name.to_ascii_uppercase().as_str() {
                "ROW_ID" => Some("#_row_id"),
                "SIMILARITY" => Some("#_similarity"),
                "DISTANCE" => Some("#_distance"),
                "SCORE" => Some("#_score"),
                _ => None,
            };
            if let Some(h) = hidden {
                if let Some(i) = schema.iter().position(|c| c == h) {
                    return Ok(Expr::Column(i));
                }
            }
            Expr::Func {
                name: name.clone(),
                args: args
                    .iter()
                    .map(|a| compile_expr(a, schema))
                    .collect::<Result<Vec<_>>>()?,
            }
        }
    })
}

/// Compiles a parsed boolean predicate against a table schema.
pub fn predicate_expr(pred: &Predicate, schema: &[String]) -> Result<Expr> {
    Ok(match pred {
        Predicate::Cmp(l, op, r) => Expr::Cmp(
            Box::new(compile_expr(l, schema)?),
            *op,
            Box::new(compile_expr(r, schema)?),
        ),
        Predicate::IsTrue(e) => compile_expr(e, schema)?,
        Predicate::IsNull(e, neg) => Expr::IsNull(Box::new(compile_expr(e, schema)?), *neg),
        Predicate::Like(a, pat) => Expr::Like(
            Box::new(compile_expr(a, schema)?),
            Box::new(compile_expr(pat, schema)?),
        ),
        Predicate::In(a, vals, neg) => Expr::In(
            Box::new(compile_expr(a, schema)?),
            vals.iter()
                .map(|v| compile_expr(v, schema))
                .collect::<Result<Vec<_>>>()?,
            *neg,
        ),
        Predicate::Bool(a, BoolOp::And, b) => Expr::And(
            Box::new(predicate_expr(a, schema)?),
            Box::new(predicate_expr(b, schema)?),
        ),
        Predicate::Bool(a, BoolOp::Or, b) => Expr::Or(
            Box::new(predicate_expr(a, schema)?),
            Box::new(predicate_expr(b, schema)?),
        ),
        Predicate::Not(p) => Expr::Not(Box::new(predicate_expr(p, schema)?)),
    })
}

fn agg_label(func: AggFunc, arg: &Option<String>) -> String {
    match arg {
        Some(c) => format!("{}({})", func.as_str(), c),
        None => format!("{}(*)", func.as_str()),
    }
}

fn build_source(
    tables: &[(String, Vec<String>)],
    join: Option<&JoinClause>,
) -> Result<LogicalPlan> {
    match (tables, join) {
        ([], _) => Err(Error::Invalid("no FROM".into())),
        ([(t, cols)], None) => Ok(LogicalPlan::Scan {
            table: t.clone(),
            columns: cols.clone(),
        }),
        ([(lt, lcols), (rt, rcols)], Some(j)) => {
            let left_key = resolve_column(lcols, &j.left)?;
            let right_key = resolve_column(rcols, &j.right)?;
            Ok(LogicalPlan::Join {
                left: Box::new(LogicalPlan::Scan {
                    table: lt.clone(),
                    columns: lcols.clone(),
                }),
                right: Box::new(LogicalPlan::Scan {
                    table: rt.clone(),
                    columns: rcols.clone(),
                }),
                left_key,
                right_key,
                left_width: lcols.len(),
            })
        }
        _ => Err(Error::Invalid("unsupported FROM/JOIN combination".into())),
    }
}

/// Plans a `SELECT`. `tables` is the list of `(table, columns)` in FROM/JOIN
/// order; pass an empty slice for a constant `SELECT` with no FROM.
pub fn plan_select(
    items: &[SelectItem],
    tables: &[(String, Vec<String>)],
    join: Option<&JoinClause>,
    where_pred: Option<&Predicate>,
    group_by: Option<&str>,
    order_by: Option<&OrderKey>,
    limit: Option<u64>,
) -> Result<LogicalPlan> {
    if tables.is_empty() {
        let mut row = Vec::new();
        for it in items {
            match it {
                SelectItem::Literal(l) => row.push(Value::from_literal(l)),
                _ => {
                    return Err(Error::Invalid(
                        "SELECT without FROM may only contain literals".into(),
                    ))
                }
            }
        }
        let columns: Vec<String> = (0..row.len()).map(|i| format!("c{i}")).collect();
        return Ok(LogicalPlan::Values {
            columns,
            rows: vec![row],
        });
    }

    let schema = combined_schema(tables);
    let mut plan = build_source(tables, join)?;
    let mut out_names = schema.clone();

    if let Some(pred) = where_pred {
        plan = LogicalPlan::Filter {
            input: Box::new(plan),
            predicate: predicate_expr(pred, &schema)?,
        };
    }

    let has_agg = group_by.is_some() || items.iter().any(|i| matches!(i, SelectItem::Aggregate(_)));
    let is_star = matches!(items, [SelectItem::Star]);

    if has_agg {
        let group_key = match group_by {
            Some(name) => Some(resolve_column(&schema, name)?),
            None => None,
        };
        let mut aggs: Vec<(AggFunc, Option<usize>, String)> = Vec::new();
        let mut names: Vec<String> = Vec::new();
        if let Some(g) = group_by {
            names.push(g.to_string());
        }
        for it in items {
            match it {
                SelectItem::Aggregate(a) => {
                    let arg_idx = match &a.arg {
                        Some(c) => Some(resolve_column(&schema, c)?),
                        None => None,
                    };
                    names.push(agg_label(a.func, &a.arg));
                    aggs.push((a.func, arg_idx, agg_label(a.func, &a.arg)));
                }
                SelectItem::Column(c) => {
                    if group_by != Some(c.as_str()) {
                        return Err(Error::Invalid(format!(
                            "column '{c}' must appear in GROUP BY or inside an aggregate"
                        )));
                    }
                }
                // scalar functions inside an aggregate query are tolerated but not
                // part of the aggregate output yet; skip them.
                SelectItem::Func { .. } => {}
                SelectItem::Star | SelectItem::Literal(_) => {
                    return Err(Error::Invalid(
                        "star/literal not allowed with aggregation".into(),
                    ));
                }
            }
        }
        plan = LogicalPlan::Aggregate {
            input: Box::new(plan),
            group_key,
            group_name: group_by.map(|s| s.to_string()),
            aggs,
        };
        out_names = names;
    } else if !is_star {
        let mut exprs = Vec::new();
        let mut names = Vec::new();
        for it in items {
            match it {
                SelectItem::Column(name) => {
                    exprs.push(Expr::Column(resolve_column(&schema, name)?));
                    names.push(name.clone());
                }
                SelectItem::Func { name, args } => {
                    let e = ExprAst::Func {
                        name: name.clone(),
                        args: args.clone(),
                    };
                    names.push(name.to_ascii_lowercase());
                    exprs.push(compile_expr(&e, &schema)?);
                }
                SelectItem::Literal(l) => {
                    names.push(format!("c{}", exprs.len()));
                    exprs.push(Expr::Literal(Value::from_literal(l)));
                }
                _ => return Err(Error::Invalid("projection must reference columns".into())),
            }
        }
        plan = LogicalPlan::Projection {
            input: Box::new(plan),
            exprs,
            names: names.clone(),
        };
        out_names = names;
    }

    if let Some(ok) = order_by {
        let key = resolve_column(&out_names, &ok.column)?;
        plan = LogicalPlan::Sort {
            input: Box::new(plan),
            key,
            asc: ok.asc,
        };
    }

    if let Some(n) = limit {
        plan = LogicalPlan::Limit {
            input: Box::new(plan),
            n,
        };
    }
    Ok(plan)
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_parser::SelectAgg;

    fn tables() -> Vec<(String, Vec<String>)> {
        vec![("t".into(), vec!["id".into(), "name".into()])]
    }

    #[test]
    fn plan_scan_only() {
        let p = plan_select(&[SelectItem::Star], &tables(), None, None, None, None, None).unwrap();
        assert_eq!(
            p,
            LogicalPlan::Scan {
                table: "t".into(),
                columns: vec!["id".into(), "name".into()]
            }
        );
    }

    #[test]
    fn plan_global_aggregate() {
        let items = vec![
            SelectItem::Aggregate(SelectAgg {
                func: AggFunc::Count,
                arg: None,
            }),
            SelectItem::Aggregate(SelectAgg {
                func: AggFunc::Sum,
                arg: Some("id".into()),
            }),
        ];
        let p = plan_select(&items, &tables(), None, None, None, None, None).unwrap();
        match p {
            LogicalPlan::Aggregate {
                group_key, aggs, ..
            } => {
                assert!(group_key.is_none());
                assert_eq!(aggs.len(), 2);
                assert_eq!(aggs[1].2, "sum(id)");
            }
            other => panic!("expected aggregate, got {other:?}"),
        }
    }

    #[test]
    fn plan_join() {
        let t = vec![
            ("a".to_string(), vec!["aid".to_string(), "an".to_string()]),
            ("b".to_string(), vec!["bid".to_string(), "bn".to_string()]),
        ];
        let j = JoinClause {
            table: "b".into(),
            left: "aid".into(),
            right: "bid".into(),
        };
        let p = plan_select(&[SelectItem::Star], &t, Some(&j), None, None, None, None).unwrap();
        match p {
            LogicalPlan::Join {
                left_key,
                right_key,
                left_width,
                ..
            } => {
                assert_eq!((left_key, right_key, left_width), (0, 0, 2));
            }
            other => panic!("expected join, got {other:?}"),
        }
    }

    #[test]
    fn unknown_and_ambiguous_columns() {
        assert!(plan_select(
            &[SelectItem::Column("nope".into())],
            &tables(),
            None,
            None,
            None,
            None,
            None
        )
        .is_err());
        let dup = vec![
            ("a".to_string(), vec!["k".to_string()]),
            ("b".to_string(), vec!["k".to_string()]),
        ];
        assert!(plan_select(
            &[SelectItem::Column("k".into())],
            &dup,
            None,
            None,
            None,
            None,
            None
        )
        .is_err());
    }
}
