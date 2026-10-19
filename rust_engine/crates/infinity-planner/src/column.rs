//! Column pruning, binding remapping and lazy-load column collection.
//!
//! C++ operates on `ColumnBinding(table_idx, column_idx)` pairs and rewrites
//! every `ColumnExpression` into a `ReferenceExpression` over the operator's
//! output. This planner addresses columns by their position in the concatenated
//! input schema, so a `ColumnBinding` collapses to its position: pruning
//! produces an old-position → new-position map and remapping applies it to
//! every expression in the plan.

use std::collections::BTreeSet;

use infinity_common::{Error, Result};
use infinity_expression::Expr;

use crate::optimize::OptimizerRule;
use crate::{output_width, LogicalPlan};

/// Old output position → new output position (`None` when pruned).
pub type ColumnMap = Vec<Option<usize>>;

/// Every input column referenced by `expr`.
fn collect_expr_columns(expr: &Expr, out: &mut BTreeSet<usize>) {
    match expr {
        Expr::Column(i) => {
            out.insert(*i);
        }
        Expr::Literal(_) => {}
        Expr::Cmp(a, _, b) | Expr::And(a, b) | Expr::Or(a, b) | Expr::Binary(a, _, b) => {
            collect_expr_columns(a, out);
            collect_expr_columns(b, out);
        }
        Expr::Not(a) | Expr::IsNull(a, _) => collect_expr_columns(a, out),
        Expr::Like(a, b, _, _) => {
            collect_expr_columns(a, out);
            collect_expr_columns(b, out);
        }
        Expr::In(a, vals, _) => {
            collect_expr_columns(a, out);
            for v in vals {
                collect_expr_columns(v, out);
            }
        }
        Expr::Func { args, .. } => {
            for a in args {
                collect_expr_columns(a, out);
            }
        }
        Expr::FullText(i, _, _) => {
            out.insert(*i);
        }
        Expr::Regex(i, pat) => {
            out.insert(*i);
            collect_expr_columns(pat, out);
        }
    }
}

/// Replaces every column reference through `map` (C++ `BindingRemapper`).
pub fn remap_expr(expr: &Expr, map: &[Option<usize>]) -> Result<Expr> {
    Ok(match expr {
        Expr::Column(i) => {
            let mapped = map.get(*i).copied().flatten().ok_or_else(|| {
                Error::Unexpected(format!("column {i} is not present in the remapping table"))
            })?;
            Expr::Column(mapped)
        }
        Expr::Literal(v) => Expr::Literal(v.clone()),
        Expr::Cmp(a, op, b) => Expr::Cmp(
            Box::new(remap_expr(a, map)?),
            *op,
            Box::new(remap_expr(b, map)?),
        ),
        Expr::And(a, b) => Expr::And(Box::new(remap_expr(a, map)?), Box::new(remap_expr(b, map)?)),
        Expr::Or(a, b) => Expr::Or(Box::new(remap_expr(a, map)?), Box::new(remap_expr(b, map)?)),
        Expr::Not(a) => Expr::Not(Box::new(remap_expr(a, map)?)),
        Expr::IsNull(a, neg) => Expr::IsNull(Box::new(remap_expr(a, map)?), *neg),
        Expr::Like(a, b, esc, neg) => Expr::Like(
            Box::new(remap_expr(a, map)?),
            Box::new(remap_expr(b, map)?),
            *esc,
            *neg,
        ),
        Expr::In(a, vals, neg) => Expr::In(
            Box::new(remap_expr(a, map)?),
            vals.iter()
                .map(|v| remap_expr(v, map))
                .collect::<Result<Vec<_>>>()?,
            *neg,
        ),
        Expr::Binary(a, op, b) => Expr::Binary(
            Box::new(remap_expr(a, map)?),
            *op,
            Box::new(remap_expr(b, map)?),
        ),
        Expr::Func { name, args } => Expr::Func {
            name: name.clone(),
            args: args
                .iter()
                .map(|a| remap_expr(a, map))
                .collect::<Result<Vec<_>>>()?,
        },
        Expr::FullText(i, q, opts) => {
            let mapped = map.get(*i).copied().flatten().ok_or_else(|| {
                Error::Unexpected(format!("column {i} is not present in the remapping table"))
            })?;
            Expr::FullText(mapped, q.clone(), opts.clone())
        }
        Expr::Regex(i, pat) => {
            let mapped = map.get(*i).copied().flatten().ok_or_else(|| {
                Error::Unexpected(format!("column {i} is not present in the remapping table"))
            })?;
            Expr::Regex(mapped, Box::new(remap_expr(pat, map)?))
        }
    })
}

fn required_refs(exprs: impl IntoIterator<Item = Expr>) -> BTreeSet<usize> {
    let mut out = BTreeSet::new();
    for expr in exprs {
        collect_expr_columns(&expr, &mut out);
    }
    out
}

fn indices(required: &BTreeSet<usize>) -> Vec<usize> {
    required.iter().copied().collect()
}

/// Prunes the columns produced/consumed by `plan`.
///
/// `prune_projections` is on for `ColumnPruner` and off for `LazyLoad`, whose
/// `CleanScan` only drops base-table columns; both share the reference
/// collection and the remapping pass.
fn prune_node(
    plan: LogicalPlan,
    required: &BTreeSet<usize>,
    prune_projections: bool,
) -> Result<(LogicalPlan, ColumnMap)> {
    match plan {
        LogicalPlan::Values { columns, rows } => {
            let keep = indices(required);
            let kept_columns = keep.iter().map(|i| columns[*i].clone()).collect();
            let kept_rows = rows
                .into_iter()
                .map(|row| keep.iter().map(|i| row[*i].clone()).collect())
                .collect();
            let map = map_for(&keep, columns.len());
            Ok((
                LogicalPlan::Values {
                    columns: kept_columns,
                    rows: kept_rows,
                },
                map,
            ))
        }
        LogicalPlan::Scan { table, columns } => {
            let keep = indices(required);
            let kept_columns = keep.iter().map(|i| columns[*i].clone()).collect();
            let map = map_for(&keep, columns.len());
            Ok((
                LogicalPlan::Scan {
                    table,
                    columns: kept_columns,
                },
                map,
            ))
        }
        LogicalPlan::Projection {
            input,
            exprs,
            names,
        } => {
            let keep: Vec<usize> = if prune_projections {
                indices(required)
                    .into_iter()
                    .filter(|i| *i < exprs.len())
                    .collect()
            } else {
                (0..exprs.len()).collect()
            };
            let kept_exprs: Vec<Expr> = keep.iter().map(|i| exprs[*i].clone()).collect();
            let kept_names: Vec<String> = keep.iter().map(|i| names[*i].clone()).collect();
            let input_required = required_refs(kept_exprs.clone());
            let (new_input, input_map) = prune_node(*input, &input_required, prune_projections)?;
            let exprs = kept_exprs
                .iter()
                .map(|e| remap_expr(e, &input_map))
                .collect::<Result<Vec<_>>>()?;
            let map = map_for(&keep, names.len());
            Ok((
                LogicalPlan::Projection {
                    input: Box::new(new_input),
                    exprs,
                    names: kept_names,
                },
                map,
            ))
        }
        LogicalPlan::Aggregate {
            input,
            group_keys,
            group_names,
            aggs,
        } => {
            let group_len = group_keys.len();
            let old_agg_len = aggs.len();
            // Group keys define the groups and cannot be removed; aggregates
            // that nothing references are pruned (C++ `ClearUnusedExpressions`).
            let kept: Vec<(usize, _)> = aggs
                .iter()
                .enumerate()
                .filter(|(i, _)| required.contains(&(group_len + i)))
                .map(|(i, a)| (i, a.clone()))
                .collect();
            let kept_aggs: Vec<_> = kept.iter().map(|(_, a)| a.clone()).collect();
            let mut input_required = required_refs(group_keys.clone());
            for (_, args, _, _) in &kept_aggs {
                input_required.extend(args.iter().copied());
            }
            let (new_input, input_map) = prune_node(*input, &input_required, prune_projections)?;
            let group_keys = group_keys
                .iter()
                .map(|e| remap_expr(e, &input_map))
                .collect::<Result<Vec<_>>>()?;
            let aggs = kept_aggs
                .into_iter()
                .map(|(f, args, distinct, label)| {
                    Ok((
                        f,
                        args.into_iter()
                            .map(|i| {
                                input_map.get(i).copied().flatten().ok_or_else(|| {
                                    Error::Unexpected(format!(
                                        "aggregate argument {i} is not present in the remapping table"
                                    ))
                                })
                            })
                            .collect::<Result<Vec<_>>>()?,
                        distinct,
                        label,
                    ))
                })
                .collect::<Result<Vec<_>>>()?;
            let mut map = vec![None; group_len + old_agg_len];
            for (i, slot) in map.iter_mut().enumerate().take(group_len) {
                *slot = Some(i);
            }
            for (new_i, (old_i, _)) in kept.iter().enumerate() {
                map[group_len + old_i] = Some(group_len + new_i);
            }
            Ok((
                LogicalPlan::Aggregate {
                    input: Box::new(new_input),
                    group_keys,
                    group_names,
                    aggs,
                },
                map,
            ))
        }
        LogicalPlan::Filter { input, predicate } => {
            let mut input_required = required.clone();
            collect_expr_columns(&predicate, &mut input_required);
            let (new_input, input_map) = prune_node(*input, &input_required, prune_projections)?;
            Ok((
                LogicalPlan::Filter {
                    input: Box::new(new_input),
                    predicate: remap_expr(&predicate, &input_map)?,
                },
                input_map,
            ))
        }
        LogicalPlan::Sort { input, keys } => {
            let mut input_required = required.clone();
            for (key, _) in &keys {
                collect_expr_columns(key, &mut input_required);
            }
            let (new_input, input_map) = prune_node(*input, &input_required, prune_projections)?;
            let keys = keys
                .iter()
                .map(|(key, asc)| Ok((remap_expr(key, &input_map)?, *asc)))
                .collect::<Result<Vec<_>>>()?;
            Ok((
                LogicalPlan::Sort {
                    input: Box::new(new_input),
                    keys,
                },
                input_map,
            ))
        }
        LogicalPlan::Limit { input, n } => {
            let (new_input, input_map) = prune_node(*input, required, prune_projections)?;
            Ok((
                LogicalPlan::Limit {
                    input: Box::new(new_input),
                    n,
                },
                input_map,
            ))
        }
        LogicalPlan::Offset { input, n } => {
            let (new_input, input_map) = prune_node(*input, required, prune_projections)?;
            Ok((
                LogicalPlan::Offset {
                    input: Box::new(new_input),
                    n,
                },
                input_map,
            ))
        }
        LogicalPlan::Join {
            left,
            right,
            left_key,
            right_key,
            left_width,
        } => {
            let old_right_width = output_width(&right);
            let mut left_required: BTreeSet<usize> = required
                .iter()
                .copied()
                .filter(|i| *i < left_width)
                .collect();
            left_required.insert(left_key);
            let mut right_required: BTreeSet<usize> = required
                .iter()
                .copied()
                .filter(|i| *i >= left_width)
                .map(|i| i - left_width)
                .collect();
            right_required.insert(right_key);
            let (new_left, left_map) = prune_node(*left, &left_required, prune_projections)?;
            let (new_right, right_map) = prune_node(*right, &right_required, prune_projections)?;
            let new_left_width = output_width(&new_left);
            let new_left_key = left_map
                .get(left_key)
                .copied()
                .flatten()
                .ok_or_else(|| Error::Unexpected("join left key was pruned".into()))?;
            let new_right_key = right_map
                .get(right_key)
                .copied()
                .flatten()
                .ok_or_else(|| Error::Unexpected("join right key was pruned".into()))?;
            let mut map = vec![None; left_width + old_right_width];
            for (i, slot) in left_map.iter().enumerate() {
                if i < map.len() {
                    map[i] = *slot;
                }
            }
            for (i, slot) in right_map.iter().enumerate() {
                if let Some(new_i) = slot {
                    map[left_width + i] = Some(new_left_width + new_i);
                }
            }
            Ok((
                LogicalPlan::Join {
                    left: Box::new(new_left),
                    right: Box::new(new_right),
                    left_key: new_left_key,
                    right_key: new_right_key,
                    left_width: new_left_width,
                },
                map,
            ))
        }
    }
}

fn map_for(keep: &[usize], width: usize) -> ColumnMap {
    let mut map = vec![None; width];
    for (new_i, old_i) in keep.iter().enumerate() {
        if *old_i < width {
            map[*old_i] = Some(new_i);
        }
    }
    map
}

/// C++ `ColumnPruner`: removes columns and expressions nothing references.
///
/// The root operator's output is fully referenced (C++ constructs the visitor
/// with `is_root=true`), so `SELECT *` keeps every scan column.
pub struct ColumnPruner;

impl ColumnPruner {
    pub fn prune(plan: LogicalPlan) -> Result<LogicalPlan> {
        let width = output_width(&plan);
        let required: BTreeSet<usize> = (0..width).collect();
        Ok(prune_node(plan, &required, true)?.0)
    }
}

impl OptimizerRule for ColumnPruner {
    fn name(&self) -> &'static str {
        "Column Pruner"
    }

    fn apply(&self, plan: LogicalPlan) -> Result<LogicalPlan> {
        Self::prune(plan)
    }
}

/// C++ `ColumnRemapper` / `BindingRemapper`: rewrites expression bindings
/// through the position map a scanner/planner produced.
pub struct ColumnRemapper {
    map: ColumnMap,
}

impl ColumnRemapper {
    pub fn new(map: ColumnMap) -> Self {
        Self { map }
    }

    pub fn remap(&self, plan: LogicalPlan) -> Result<LogicalPlan> {
        remap_plan(plan, &self.map)
    }
}

impl OptimizerRule for ColumnRemapper {
    fn name(&self) -> &'static str {
        "Column Remapper"
    }

    fn apply(&self, plan: LogicalPlan) -> Result<LogicalPlan> {
        self.remap(plan)
    }
}

fn remap_plan(plan: LogicalPlan, map: &[Option<usize>]) -> Result<LogicalPlan> {
    Ok(match plan {
        LogicalPlan::Values { columns, rows } => LogicalPlan::Values { columns, rows },
        LogicalPlan::Scan { table, columns } => LogicalPlan::Scan { table, columns },
        LogicalPlan::Projection {
            input,
            exprs,
            names,
        } => LogicalPlan::Projection {
            input: Box::new(remap_plan(*input, map)?),
            exprs: exprs
                .iter()
                .map(|e| remap_expr(e, map))
                .collect::<Result<Vec<_>>>()?,
            names,
        },
        LogicalPlan::Aggregate {
            input,
            group_keys,
            group_names,
            aggs,
        } => LogicalPlan::Aggregate {
            input: Box::new(remap_plan(*input, map)?),
            group_keys: group_keys
                .iter()
                .map(|e| remap_expr(e, map))
                .collect::<Result<Vec<_>>>()?,
            group_names,
            aggs: aggs
                .into_iter()
                .map(|(f, args, distinct, label)| {
                    Ok((
                        f,
                        args.into_iter()
                            .map(|i| {
                                map.get(i).copied().flatten().ok_or_else(|| {
                                    Error::Unexpected(format!(
                                        "aggregate argument {i} is not present in the remapping table"
                                    ))
                                })
                            })
                            .collect::<Result<Vec<_>>>()?,
                        distinct,
                        label,
                    ))
                })
                .collect::<Result<Vec<_>>>()?,
        },
        LogicalPlan::Filter { input, predicate } => LogicalPlan::Filter {
            input: Box::new(remap_plan(*input, map)?),
            predicate: remap_expr(&predicate, map)?,
        },
        LogicalPlan::Sort { input, keys } => LogicalPlan::Sort {
            input: Box::new(remap_plan(*input, map)?),
            keys: keys
                .iter()
                .map(|(e, asc)| Ok((remap_expr(e, map)?, *asc)))
                .collect::<Result<Vec<_>>>()?,
        },
        LogicalPlan::Limit { input, n } => LogicalPlan::Limit {
            input: Box::new(remap_plan(*input, map)?),
            n,
        },
        LogicalPlan::Offset { input, n } => LogicalPlan::Offset {
            input: Box::new(remap_plan(*input, map)?),
            n,
        },
        LogicalPlan::Join {
            left,
            right,
            left_key,
            right_key,
            left_width,
        } => {
            let new_left_key = map
                .get(left_key)
                .copied()
                .flatten()
                .ok_or_else(|| Error::Unexpected("join left key was pruned".into()))?;
            let new_right_key = map
                .get(left_width + right_key)
                .copied()
                .flatten()
                .ok_or_else(|| Error::Unexpected("join right key was pruned".into()))?;
            let left_map: ColumnMap = (0..left_width)
                .map(|i| map.get(i).copied().flatten())
                .collect();
            let new_left = remap_plan(*left, &left_map)?;
            let new_left_width = output_width(&new_left);
            let right_map: ColumnMap = map
                .iter()
                .skip(left_width)
                .map(|slot| slot.map(|v| v.saturating_sub(new_left_width)))
                .collect();
            LogicalPlan::Join {
                left: Box::new(new_left),
                right: Box::new(remap_plan(*right, &right_map)?),
                left_key: new_left_key,
                right_key: new_right_key.saturating_sub(new_left_width),
                left_width: new_left_width,
            }
        }
    })
}

/// The columns one scan has to read (C++ `LoadMeta` / `ScanLoad`).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ScanLoad {
    pub table: String,
    pub columns: Vec<String>,
    pub column_indices: Vec<usize>,
}

/// C++ `LazyLoad`: collects the columns each scan has to load and cleans the
/// scan column list down to them.
///
/// The Rust executor materialises a whole row, so there is no separate
/// deferred-load step; `collect` produces the same per-scan load set and
/// `clean_scan` is the `CleanScan` visitor that applies it.
pub struct LazyLoad;

impl LazyLoad {
    /// C++ `RefencecColumnCollection`: the referenced columns per base scan.
    pub fn collect(plan: &LogicalPlan) -> Vec<ScanLoad> {
        let width = output_width(plan);
        let required: BTreeSet<usize> = (0..width).collect();
        let mut out = Vec::new();
        collect_loads(plan, &required, &mut out);
        out
    }

    /// C++ `CleanScan`: removes unreferenced base-table columns.
    pub fn clean_scan(plan: LogicalPlan) -> Result<LogicalPlan> {
        let width = output_width(&plan);
        let required: BTreeSet<usize> = (0..width).collect();
        Ok(prune_node(plan, &required, false)?.0)
    }
}

impl OptimizerRule for LazyLoad {
    fn name(&self) -> &'static str {
        "Lazy Load"
    }

    fn apply(&self, plan: LogicalPlan) -> Result<LogicalPlan> {
        Self::clean_scan(plan)
    }
}

fn collect_loads(plan: &LogicalPlan, required: &BTreeSet<usize>, out: &mut Vec<ScanLoad>) {
    match plan {
        LogicalPlan::Scan { table, columns } => {
            let column_indices: Vec<usize> = required
                .iter()
                .copied()
                .filter(|i| *i < columns.len())
                .collect();
            out.push(ScanLoad {
                table: table.clone(),
                columns: column_indices.iter().map(|i| columns[*i].clone()).collect(),
                column_indices,
            });
        }
        LogicalPlan::Projection { input, exprs, .. } => {
            let mut req = BTreeSet::new();
            for i in required {
                if let Some(e) = exprs.get(*i) {
                    collect_expr_columns(e, &mut req);
                }
            }
            collect_loads(input, &req, out);
        }
        LogicalPlan::Aggregate {
            input,
            group_keys,
            aggs,
            ..
        } => {
            let mut req = BTreeSet::new();
            for key in group_keys {
                collect_expr_columns(key, &mut req);
            }
            let group_len = group_keys.len();
            for (i, (_, args, _, _)) in aggs.iter().enumerate() {
                if required.contains(&(group_len + i)) {
                    req.extend(args.iter().copied());
                }
            }
            collect_loads(input, &req, out);
        }
        LogicalPlan::Filter { input, predicate } => {
            let mut req = required.clone();
            collect_expr_columns(predicate, &mut req);
            collect_loads(input, &req, out);
        }
        LogicalPlan::Sort { input, keys } => {
            let mut req = required.clone();
            for (key, _) in keys {
                collect_expr_columns(key, &mut req);
            }
            collect_loads(input, &req, out);
        }
        LogicalPlan::Limit { input, .. } | LogicalPlan::Offset { input, .. } => {
            collect_loads(input, required, out)
        }
        LogicalPlan::Join {
            left,
            right,
            left_key,
            right_key,
            left_width,
        } => {
            let mut left_req: BTreeSet<usize> = required
                .iter()
                .copied()
                .filter(|i| *i < *left_width)
                .collect();
            left_req.insert(*left_key);
            let mut right_req: BTreeSet<usize> = required
                .iter()
                .copied()
                .filter(|i| *i >= *left_width)
                .map(|i| i - *left_width)
                .collect();
            right_req.insert(*right_key);
            collect_loads(left, &left_req, out);
            collect_loads(right, &right_req, out);
        }
        LogicalPlan::Values { .. } => {}
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{output_schema, plan_select};
    use infinity_expression::Value;
    use infinity_parser::SelectItem;

    fn scan() -> LogicalPlan {
        LogicalPlan::Scan {
            table: "t".into(),
            columns: vec!["a".into(), "b".into(), "c".into()],
        }
    }

    fn project(plan: LogicalPlan, cols: &[usize]) -> LogicalPlan {
        LogicalPlan::Projection {
            input: Box::new(plan),
            exprs: cols.iter().map(|i| Expr::Column(*i)).collect(),
            names: cols.iter().map(|i| format!("c{i}")).collect(),
        }
    }

    /// `ColumnPruner` / `ColumnRemapper`: a projection on `c` prunes `a`/`b`
    /// from the scan and remaps the surviving reference to position 0.
    #[test]
    fn column_pruner_keeps_only_referenced_columns() {
        let plan = project(scan(), &[2]);
        let pruned = ColumnPruner::prune(plan).unwrap();
        match pruned {
            LogicalPlan::Projection { input, exprs, .. } => {
                assert_eq!(exprs, vec![Expr::Column(0)]);
                match *input {
                    LogicalPlan::Scan { columns, .. } => assert_eq!(columns, vec!["c".to_string()]),
                    other => panic!("expected scan, got {other:?}"),
                }
            }
            other => panic!("expected projection, got {other:?}"),
        }
    }

    /// C++ `RemoveUnusedColumns`: the root `SELECT *` output is fully
    /// referenced, so no scan column is removed.
    #[test]
    fn column_pruner_keeps_everything_for_select_star() {
        let tables = vec![(
            "t".to_string(),
            vec!["a".to_string(), "b".to_string(), "c".to_string()],
        )];
        let plan = plan_select(
            &[SelectItem::Star],
            &tables,
            None,
            None,
            &[],
            None,
            &[],
            None,
            None,
        )
        .unwrap();
        let pruned = ColumnPruner::prune(plan).unwrap();
        assert_eq!(output_schema(&pruned).len(), 3);
        assert!(matches!(pruned, LogicalPlan::Scan { columns, .. } if columns.len() == 3));
    }

    /// A filter reference and a projection reference are unioned; the scan
    /// keeps both and every expression is remapped consistently.
    #[test]
    fn column_pruner_unions_filter_and_projection_references() {
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Filter {
                input: Box::new(scan()),
                predicate: Expr::Cmp(
                    Box::new(Expr::Column(1)),
                    infinity_parser::CmpOp::Gt,
                    Box::new(Expr::Literal(Value::Int(0))),
                ),
            }),
            exprs: vec![Expr::Column(2)],
            names: vec!["c".into()],
        };
        let pruned = ColumnPruner::prune(plan).unwrap();
        match pruned {
            LogicalPlan::Projection { input, exprs, .. } => {
                assert_eq!(exprs, vec![Expr::Column(1)]);
                match *input {
                    LogicalPlan::Filter { input, predicate } => {
                        assert_eq!(
                            predicate,
                            Expr::Cmp(
                                Box::new(Expr::Column(0)),
                                infinity_parser::CmpOp::Gt,
                                Box::new(Expr::Literal(Value::Int(0)))
                            )
                        );
                        assert!(
                            matches!(*input, LogicalPlan::Scan { columns, .. } if columns == ["b", "c"])
                        );
                    }
                    other => panic!("expected filter, got {other:?}"),
                }
            }
            other => panic!("expected projection, got {other:?}"),
        }
    }

    /// `ColumnRemapper::remap` applies an externally supplied binding map to
    /// every expression in the plan (C++ `BindingRemapper`).
    #[test]
    fn column_remapper_rewrites_all_expressions() {
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Filter {
                input: Box::new(scan()),
                predicate: Expr::Cmp(
                    Box::new(Expr::Column(2)),
                    infinity_parser::CmpOp::Eq,
                    Box::new(Expr::Literal(Value::Int(7))),
                ),
            }),
            exprs: vec![Expr::Column(1)],
            names: vec!["b".into()],
        };
        let map = vec![None, Some(0), Some(1)];
        let got = ColumnRemapper::new(map).remap(plan).unwrap();
        match got {
            LogicalPlan::Projection { input, exprs, .. } => {
                assert_eq!(exprs, vec![Expr::Column(0)]);
                match *input {
                    LogicalPlan::Filter { predicate, .. } => assert_eq!(
                        predicate,
                        Expr::Cmp(
                            Box::new(Expr::Column(1)),
                            infinity_parser::CmpOp::Eq,
                            Box::new(Expr::Literal(Value::Int(7)))
                        )
                    ),
                    other => panic!("expected filter, got {other:?}"),
                }
            }
            other => panic!("expected projection, got {other:?}"),
        }
    }

    /// `LazyLoad::collect` reports the same per-scan set `CleanScan` keeps.
    #[test]
    fn lazy_load_collects_and_cleans_scan_columns() {
        let plan = LogicalPlan::Projection {
            input: Box::new(scan()),
            exprs: vec![Expr::Column(0), Expr::Column(2)],
            names: vec!["a".into(), "c".into()],
        };
        let loads = LazyLoad::collect(&plan);
        assert_eq!(loads.len(), 1);
        assert_eq!(loads[0].column_indices, vec![0, 2]);
        assert_eq!(loads[0].columns, vec!["a".to_string(), "c".to_string()]);
        let cleaned = LazyLoad::clean_scan(plan).unwrap();
        match cleaned {
            LogicalPlan::Projection { input, .. } => match *input {
                LogicalPlan::Scan { columns, .. } => {
                    assert_eq!(columns, vec!["a".to_string(), "c".to_string()])
                }
                other => panic!("expected scan, got {other:?}"),
            },
            other => panic!("expected projection, got {other:?}"),
        }
    }

    /// Both sides of a join are pruned independently and the join keys are
    /// remapped to the compacted positions.
    #[test]
    fn column_pruner_handles_join_offsets() {
        let left = LogicalPlan::Scan {
            table: "l".into(),
            columns: vec!["l0".into(), "l1".into()],
        };
        let right = LogicalPlan::Scan {
            table: "r".into(),
            columns: vec!["r0".into(), "r1".into(), "r2".into()],
        };
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Join {
                left: Box::new(left),
                right: Box::new(right),
                left_key: 1,
                right_key: 2,
                left_width: 2,
            }),
            exprs: vec![Expr::Column(4)],
            names: vec!["r2".into()],
        };
        let pruned = ColumnPruner::prune(plan).unwrap();
        match pruned {
            LogicalPlan::Projection { input, exprs, .. } => {
                assert_eq!(exprs, vec![Expr::Column(1)]);
                match *input {
                    LogicalPlan::Join {
                        left,
                        right,
                        left_key,
                        right_key,
                        left_width,
                    } => {
                        assert_eq!((left_key, right_key, left_width), (0, 0, 1));
                        assert!(
                            matches!(*left, LogicalPlan::Scan { columns, .. } if columns == ["l1"])
                        );
                        assert!(
                            matches!(*right, LogicalPlan::Scan { columns, .. } if columns == ["r2"])
                        );
                    }
                    other => panic!("expected join, got {other:?}"),
                }
            }
            other => panic!("expected projection, got {other:?}"),
        }
    }
}
