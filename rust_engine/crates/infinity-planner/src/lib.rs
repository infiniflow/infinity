//! `infinity-planner`: builds a logical plan from a parsed `SELECT`.
//! Operators are deliberately simple (M2); the Tokio executor consumes this tree.

#![forbid(unsafe_code)]

pub mod optimize;

use infinity_common::{Error, Result};
use infinity_expression::{Expr, Value};
use infinity_parser::{
    AggFunc, BoolOp, ExprAst, JoinClause, Literal, OrderKey, Predicate, SelectItem,
};

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
        group_keys: Vec<usize>,
        group_names: Vec<String>,
        aggs: Vec<(AggFunc, Vec<usize>, bool, String)>,
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
    Offset {
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
        ExprAst::Binary(a, op, b) => Expr::Binary(
            Box::new(compile_expr(a, schema)?),
            *op,
            Box::new(compile_expr(b, schema)?),
        ),
        ExprAst::Cmp(a, op, b) => Expr::Cmp(
            Box::new(compile_expr(a, schema)?),
            *op,
            Box::new(compile_expr(b, schema)?),
        ),
        ExprAst::Not(a) => Expr::Not(Box::new(compile_expr(a, schema)?)),
        ExprAst::And(a, b) => Expr::And(
            Box::new(compile_expr(a, schema)?),
            Box::new(compile_expr(b, schema)?),
        ),
        ExprAst::Or(a, b) => Expr::Or(
            Box::new(compile_expr(a, schema)?),
            Box::new(compile_expr(b, schema)?),
        ),
        ExprAst::Func { name, args } => {
            // `filter_text(col, '…')` / `filter_fulltext(col, '…')` resolve the
            // column name at compile time.
            let up = name.to_ascii_uppercase();
            if up == "FILTER_TEXT" || up == "FILTER_FULLTEXT" {
                if let (Some(ExprAst::Literal(Literal::Str(col))), Some(q)) =
                    (args.first(), args.get(1))
                {
                    if let (Some(i), ExprAst::Literal(Literal::Str(query))) =
                        (schema.iter().position(|c| c == col), q)
                    {
                        return Ok(Expr::FullText(i, query.clone()));
                    }
                }
            }
            // `regex(col, '…')` resolves the column at compile time.
            if up == "REGEX" {
                if let (Some(ExprAst::Column(col)), Some(ExprAst::Literal(Literal::Str(pat)))) =
                    (args.first(), args.get(1))
                {
                    if let Some(i) = schema.iter().position(|c| c == col) {
                        return Ok(Expr::Regex(i, pat.clone()));
                    }
                }
            }
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
        Predicate::Like(a, pat, esc, neg) => Expr::Like(
            Box::new(compile_expr(a, schema)?),
            Box::new(compile_expr(pat, schema)?),
            *esc,
            *neg,
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

fn agg_label(func: AggFunc, args: &[String]) -> String {
    if args.is_empty() {
        format!("{}(*)", func.as_str())
    } else {
        format!("{}({})", func.as_str(), args.join(","))
    }
}

/// True if `e` contains an aggregate call.
fn expr_has_agg(e: &ExprAst) -> bool {
    match e {
        ExprAst::Func { name, args } => {
            AggFunc::from_name(name).is_some() || args.iter().any(expr_has_agg)
        }
        ExprAst::Binary(a, _, b) => expr_has_agg(a) || expr_has_agg(b),
        ExprAst::Cmp(a, _, b) => expr_has_agg(a) || expr_has_agg(b),
        ExprAst::Not(a) => expr_has_agg(a),
        ExprAst::And(a, b) | ExprAst::Or(a, b) => expr_has_agg(a) || expr_has_agg(b),
        _ => false,
    }
}

/// Registers every aggregate found in `e` (recursively) into `aggs`/`index`.
fn gather_agg(
    e: &ExprAst,
    schema: &[String],
    aggs: &mut Vec<(AggFunc, Vec<usize>, bool, String)>,
    index: &mut std::collections::HashMap<String, usize>,
    base: usize,
) -> Result<()> {
    match e {
        ExprAst::Func { name, args } => {
            if let Some(f) = AggFunc::from_name(name) {
                let arg_vec: Vec<String> = args
                    .iter()
                    .filter_map(|a| match a {
                        ExprAst::Column(c) => Some(c.clone()),
                        _ => None,
                    })
                    .collect();
                let label = agg_label(f, &arg_vec);
                if !index.contains_key(&label) {
                    let idxs: Vec<usize> = arg_vec
                        .iter()
                        .map(|c| resolve_column(schema, c))
                        .collect::<Result<Vec<_>>>()?;
                    index.insert(label.clone(), base + aggs.len());
                    aggs.push((f, idxs, false, label));
                }
            } else {
                for a in args {
                    gather_agg(a, schema, aggs, index, base)?;
                }
            }
        }
        ExprAst::Binary(a, _, b) => {
            gather_agg(a, schema, aggs, index, base)?;
            gather_agg(b, schema, aggs, index, base)?;
        }
        ExprAst::Cmp(a, _, b) => {
            gather_agg(a, schema, aggs, index, base)?;
            gather_agg(b, schema, aggs, index, base)?;
        }
        ExprAst::Not(a) => gather_agg(a, schema, aggs, index, base)?,
        ExprAst::And(a, b) | ExprAst::Or(a, b) => {
            gather_agg(a, schema, aggs, index, base)?;
            gather_agg(b, schema, aggs, index, base)?;
        }
        _ => {}
    }
    Ok(())
}

/// Compiles a HAVING predicate over the aggregate output schema.
fn compile_agg_predicate(
    pred: &Predicate,
    agg_index: &std::collections::HashMap<String, usize>,
    group_by: &[String],
) -> Result<Expr> {
    Ok(match pred {
        Predicate::Cmp(l, op, r) => Expr::Cmp(
            Box::new(compile_agg_expr(l, agg_index, group_by)?),
            *op,
            Box::new(compile_agg_expr(r, agg_index, group_by)?),
        ),
        Predicate::IsTrue(e) => compile_agg_expr(e, agg_index, group_by)?,
        Predicate::IsNull(e, neg) => {
            Expr::IsNull(Box::new(compile_agg_expr(e, agg_index, group_by)?), *neg)
        }
        Predicate::Like(a, b, esc, neg) => Expr::Like(
            Box::new(compile_agg_expr(a, agg_index, group_by)?),
            Box::new(compile_agg_expr(b, agg_index, group_by)?),
            *esc,
            *neg,
        ),
        Predicate::In(a, vals, neg) => Expr::In(
            Box::new(compile_agg_expr(a, agg_index, group_by)?),
            vals.iter()
                .map(|v| compile_agg_expr(v, agg_index, group_by))
                .collect::<Result<Vec<_>>>()?,
            *neg,
        ),
        Predicate::Bool(a, BoolOp::And, b) => Expr::And(
            Box::new(compile_agg_predicate(a, agg_index, group_by)?),
            Box::new(compile_agg_predicate(b, agg_index, group_by)?),
        ),
        Predicate::Bool(a, BoolOp::Or, b) => Expr::Or(
            Box::new(compile_agg_predicate(a, agg_index, group_by)?),
            Box::new(compile_agg_predicate(b, agg_index, group_by)?),
        ),
        Predicate::Not(a) => Expr::Not(Box::new(compile_agg_predicate(a, agg_index, group_by)?)),
    })
}

/// Compiles an expression over the *aggregate output* schema (group column then
/// aggregate results). Aggregate calls become column references.
fn compile_agg_expr(
    e: &ExprAst,
    agg_index: &std::collections::HashMap<String, usize>,
    group_by: &[String],
) -> Result<Expr> {
    Ok(match e {
        ExprAst::Column(c) => {
            if group_by.iter().any(|g| g == c) {
                Expr::Column(0)
            } else {
                return Err(Error::Invalid(format!(
                    "column '{c}' must appear in GROUP BY or inside an aggregate"
                )));
            }
        }
        ExprAst::Literal(l) => Expr::Literal(Value::from_literal(l)),
        ExprAst::Binary(a, op, b) => Expr::Binary(
            Box::new(compile_agg_expr(a, agg_index, group_by)?),
            *op,
            Box::new(compile_agg_expr(b, agg_index, group_by)?),
        ),
        ExprAst::Cmp(a, op, b) => Expr::Cmp(
            Box::new(compile_agg_expr(a, agg_index, group_by)?),
            *op,
            Box::new(compile_agg_expr(b, agg_index, group_by)?),
        ),
        ExprAst::Not(a) => Expr::Not(Box::new(compile_agg_expr(a, agg_index, group_by)?)),
        ExprAst::And(a, b) => Expr::And(
            Box::new(compile_agg_expr(a, agg_index, group_by)?),
            Box::new(compile_agg_expr(b, agg_index, group_by)?),
        ),
        ExprAst::Or(a, b) => Expr::Or(
            Box::new(compile_agg_expr(a, agg_index, group_by)?),
            Box::new(compile_agg_expr(b, agg_index, group_by)?),
        ),
        ExprAst::Func { name, args } => {
            if let Some(f) = AggFunc::from_name(name) {
                let arg_vec: Vec<String> = args
                    .iter()
                    .filter_map(|a| match a {
                        ExprAst::Column(c) => Some(c.clone()),
                        _ => None,
                    })
                    .collect();
                let label = agg_label(f, &arg_vec);
                let idx = *agg_index
                    .get(&label)
                    .ok_or_else(|| Error::Invalid(format!("aggregate '{label}' not collected")))?;
                Expr::Column(idx)
            } else {
                Expr::Func {
                    name: name.clone(),
                    args: args
                        .iter()
                        .map(|a| compile_agg_expr(a, agg_index, group_by))
                        .collect::<Result<Vec<_>>>()?,
                }
            }
        }
    })
}

/// Builds the post-aggregate projection expression for one select item.
fn item_agg_expr(
    it: &SelectItem,
    agg_index: &std::collections::HashMap<String, usize>,
    group_by: &[String],
    _agg_names: &[String],
) -> Result<Expr> {
    match it {
        SelectItem::Aggregate(a) => {
            let label = agg_label(a.func, &a.args);
            let idx = *agg_index
                .get(&label)
                .ok_or_else(|| Error::Invalid(format!("aggregate '{label}' not collected")))?;
            Ok(Expr::Column(idx))
        }
        SelectItem::Column(c) => match group_by.iter().position(|g| g == c) {
            Some(i) => Ok(Expr::Column(i)),
            None => Err(Error::Invalid(format!(
                "column '{c}' must appear in GROUP BY or inside an aggregate"
            ))),
        },
        SelectItem::Literal(l) => Ok(Expr::Literal(Value::from_literal(l))),
        SelectItem::Func { name, args } => {
            let e = ExprAst::Func {
                name: name.clone(),
                args: args.clone(),
            };
            compile_agg_expr(&e, agg_index, group_by)
        }
        SelectItem::Expr(e) => compile_agg_expr(e, agg_index, group_by),
        SelectItem::Star => Err(Error::Invalid("star not allowed with aggregation".into())),
    }
}

/// A display name for a select item (used for post-aggregate projection).
fn item_label(it: &SelectItem) -> String {
    match it {
        SelectItem::Aggregate(a) => agg_label(a.func, &a.args),
        SelectItem::Column(c) => c.clone(),
        SelectItem::Func { name, .. } => name.to_ascii_lowercase(),
        SelectItem::Literal(_) => "c0".to_string(),
        SelectItem::Expr(_) => "expr".to_string(),
        SelectItem::Star => "*".to_string(),
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
#[allow(clippy::too_many_arguments)]
pub fn plan_select(
    items: &[SelectItem],
    tables: &[(String, Vec<String>)],
    join: Option<&JoinClause>,
    where_pred: Option<&Predicate>,
    group_by: &[String],
    having: Option<&Predicate>,
    order_by: Option<&OrderKey>,
    limit: Option<u64>,
    offset: Option<u64>,
) -> Result<LogicalPlan> {
    if tables.is_empty() {
        let mut row = Vec::new();
        for it in items {
            let e = match it {
                SelectItem::Literal(l) => Expr::Literal(Value::from_literal(l)),
                SelectItem::Func { name, args } => compile_expr(
                    &ExprAst::Func {
                        name: name.clone(),
                        args: args.clone(),
                    },
                    &[],
                )?,
                SelectItem::Expr(e) => compile_expr(e, &[])?,
                _ => {
                    return Err(Error::Invalid(
                        "SELECT without FROM may only contain constant expressions".into(),
                    ))
                }
            };
            row.push(e.eval(&[]));
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

    let has_agg = !group_by.is_empty()
        || items.iter().any(|i| match i {
            SelectItem::Aggregate(_) => true,
            SelectItem::Expr(e) => expr_has_agg(e),
            _ => false,
        });
    let is_star = matches!(items, [SelectItem::Star]);

    if has_agg {
        let group_keys: Vec<usize> = group_by
            .iter()
            .map(|name| resolve_column(&schema, name))
            .collect::<Result<Vec<_>>>()?;
        let base = group_by.len();
        let mut aggs: Vec<(AggFunc, Vec<usize>, bool, String)> = Vec::new();
        let mut agg_index: std::collections::HashMap<String, usize> = Default::default();
        let mut needs_projection = false;
        for it in items {
            match it {
                SelectItem::Aggregate(a) => {
                    let label = agg_label(a.func, &a.args);
                    if !agg_index.contains_key(&label) {
                        let idxs: Vec<usize> = a
                            .args
                            .iter()
                            .map(|c| resolve_column(&schema, c))
                            .collect::<Result<Vec<_>>>()?;
                        agg_index.insert(label.clone(), base + aggs.len());
                        aggs.push((a.func, idxs, a.distinct, label));
                    }
                }
                SelectItem::Expr(e) => {
                    needs_projection = true;
                    gather_agg(e, &schema, &mut aggs, &mut agg_index, base)?;
                }
                SelectItem::Column(c) => {
                    if !group_by.iter().any(|g| g == c) {
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
        let mut agg_names: Vec<String> = group_by.to_vec();
        agg_names.extend(aggs.iter().map(|(_, _, _, l)| l.clone()));
        plan = LogicalPlan::Aggregate {
            input: Box::new(plan),
            group_keys,
            group_names: group_by.to_vec(),
            aggs,
        };
        out_names = agg_names.clone();
        // HAVING filters the aggregate output (group columns + aggregate results).
        if let Some(h) = having {
            let pred = compile_agg_predicate(h, &agg_index, group_by)?;
            plan = LogicalPlan::Filter {
                input: Box::new(plan),
                predicate: pred,
            };
        }
        if needs_projection {
            let mut exprs = Vec::new();
            let mut names = Vec::new();
            for it in items {
                exprs.push(item_agg_expr(it, &agg_index, group_by, &agg_names)?);
                names.push(item_label(it));
            }
            plan = LogicalPlan::Projection {
                input: Box::new(plan),
                exprs,
                names: names.clone(),
            };
            out_names = names;
        }
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
                SelectItem::Expr(e) => {
                    names.push("expr".to_string());
                    exprs.push(compile_expr(e, &schema)?);
                }
                SelectItem::Star => {
                    for (i, c) in schema.iter().enumerate() {
                        exprs.push(Expr::Column(i));
                        names.push(c.clone());
                    }
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

    if let Some(n) = offset {
        plan = LogicalPlan::Offset {
            input: Box::new(plan),
            n,
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
        let p = plan_select(
            &[SelectItem::Star],
            &tables(),
            None,
            None,
            &[],
            None,
            None,
            None,
            None,
        )
        .unwrap();
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
                args: vec![],
                distinct: false,
            }),
            SelectItem::Aggregate(SelectAgg {
                func: AggFunc::Sum,
                args: vec!["id".into()],
                distinct: false,
            }),
        ];
        let p = plan_select(&items, &tables(), None, None, &[], None, None, None, None).unwrap();
        match p {
            LogicalPlan::Aggregate {
                group_keys, aggs, ..
            } => {
                assert!(group_keys.is_empty());
                assert_eq!(aggs.len(), 2);
                assert_eq!(aggs[1].3, "sum(id)");
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
        let p = plan_select(
            &[SelectItem::Star],
            &t,
            Some(&j),
            None,
            &[],
            None,
            None,
            None,
            None,
        )
        .unwrap();
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
            &[],
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
            &[],
            None,
            None,
            None,
            None
        )
        .is_err());
    }
}
