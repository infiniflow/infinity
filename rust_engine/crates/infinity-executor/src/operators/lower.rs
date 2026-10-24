//! Lowers a [`LogicalPlan`] into the physical operator chain and the source it
//! reads from.
//!
//! The reference builds this chain in `PhysicalPlanner` and the fragment context
//! wires one state per operator (`fragment_context_impl.cpp:494`). `lower` is
//! total: every `LogicalPlan` node maps onto an operator or onto the fragment's
//! source.

use infinity_common::Result;
use infinity_expression::Value;
use infinity_planner::LogicalPlan;

use super::aggregate::Aggregate;
use super::filter::Filter;
use super::limit::Limit;
use super::project::Projection;
use super::sort::Sort;
use super::top::Top;
use super::PhysicalOperator;

/// The relation under the operator chain, in the shape the source operator
/// needs. The plan-time scan parameters (index candidate, override rows, …)
/// stay with the caller, which owns them.
#[derive(Debug, Clone)]
pub enum SourceKind {
    Values {
        columns: Vec<String>,
        rows: Vec<Vec<Value>>,
    },
    Table {
        table: String,
        columns: Vec<String>,
    },
    Join {
        columns: Vec<String>,
        left: String,
        right: String,
        /// Columns each side's scan produces after pruning, used to line the
        /// fetched rows up with the pruned scan (C++ `BindingRemapper`).
        left_columns: Vec<String>,
        right_columns: Vec<String>,
        left_key: usize,
        right_key: usize,
        left_width: usize,
    },
}

impl SourceKind {
    /// The columns the source emits.
    pub fn columns(&self) -> &[String] {
        match self {
            SourceKind::Values { columns, .. }
            | SourceKind::Table { columns, .. }
            | SourceKind::Join { columns, .. } => columns,
        }
    }
}

/// A lowered plan: the operator chain in execution order (scan first), the
/// names of the columns it produces, and the source it reads from.
pub struct Lowered {
    pub operators: Vec<Box<dyn PhysicalOperator>>,
    pub names: Vec<String>,
    pub source: SourceKind,
}

/// The operator chain for `plan`.
pub fn lower(plan: &LogicalPlan) -> Result<Lowered> {
    // Built root-first while walking the plan downwards, then reversed.
    let mut operators: Vec<Box<dyn PhysicalOperator>> = Vec::new();
    // The output names are the ones of the projection nearest the root.
    let mut names: Option<Vec<String>> = None;
    let mut cursor = plan;
    loop {
        match cursor {
            LogicalPlan::Projection {
                input,
                exprs,
                names: projection_names,
            } => {
                if names.is_none() {
                    names = Some(projection_names.clone());
                }
                operators.push(Box::new(Projection::new(
                    exprs.clone(),
                    projection_names.clone(),
                )));
                cursor = input;
            }
            LogicalPlan::Filter { input, predicate } => {
                operators.push(Box::new(Filter::new(predicate.clone())));
                cursor = input;
            }
            LogicalPlan::Aggregate {
                input,
                group_keys,
                group_names,
                aggs,
            } => {
                let aggregate =
                    Aggregate::new(group_keys.clone(), group_names.clone(), aggs.clone());
                if names.is_none() {
                    names = Some(aggregate.output_names());
                }
                operators.push(Box::new(aggregate));
                cursor = input;
            }
            LogicalPlan::Limit { .. } | LogicalPlan::Offset { .. } => {
                if let Some((keys, limit, offset, rest)) = fuse_top(cursor) {
                    operators.push(Box::new(Top::new(keys, limit, offset)));
                    cursor = rest;
                } else {
                    // A `Limit`/`Offset` on its own: the offset budget skips
                    // rows, the limit budget keeps them.
                    match cursor {
                        LogicalPlan::Limit { input, n } => {
                            operators.push(Box::new(Limit::new(0, *n)));
                            cursor = input;
                        }
                        LogicalPlan::Offset { input, n } => {
                            operators.push(Box::new(Limit::new(*n, u64::MAX)));
                            cursor = input;
                        }
                        _ => unreachable!("matched Limit/Offset above"),
                    }
                }
            }
            LogicalPlan::Sort { input, keys } => {
                // A sort that is not fused into a `Top`.
                operators.push(Box::new(Sort::new(keys.clone())));
                cursor = input;
            }
            LogicalPlan::Scan { table, columns }
            | LogicalPlan::IndexScan { table, columns, .. } => {
                operators.reverse();
                let names = names.unwrap_or_else(|| columns.clone());
                return Ok(Lowered {
                    operators,
                    names,
                    source: SourceKind::Table {
                        table: table.clone(),
                        columns: columns.clone(),
                    },
                });
            }
            LogicalPlan::Values { columns, rows } => {
                operators.reverse();
                let names = names.unwrap_or_else(|| columns.clone());
                return Ok(Lowered {
                    operators,
                    names,
                    source: SourceKind::Values {
                        columns: columns.clone(),
                        rows: rows.clone(),
                    },
                });
            }
            LogicalPlan::Join {
                left,
                right,
                left_key,
                right_key,
                left_width,
            } => {
                let (left_table, left_columns) = leaf_scan(left);
                let (right_table, right_columns) = leaf_scan(right);
                let mut columns = left_columns.clone();
                columns.extend(right_columns.clone());
                operators.reverse();
                let names = names.unwrap_or_else(|| columns.clone());
                return Ok(Lowered {
                    operators,
                    names,
                    source: SourceKind::Join {
                        columns,
                        left: left_table,
                        right: right_table,
                        left_columns,
                        right_columns,
                        left_key: *left_key,
                        right_key: *right_key,
                        left_width: *left_width,
                    },
                });
            }
        }
    }
}

/// A `Top` recognised in the plan: the sort keys, the limit, the offset and
/// the input below the sort.
type FusedTop<'a> = (
    Vec<(infinity_expression::Expr, bool)>,
    u64,
    u64,
    &'a LogicalPlan,
);

/// Recognises `[Limit] [Offset] Sort` and folds it into one `Top`.
fn fuse_top(plan: &LogicalPlan) -> Option<FusedTop<'_>> {
    let (limit, offset, rest) = match plan {
        LogicalPlan::Limit { input, n } => match input.as_ref() {
            LogicalPlan::Offset { input, n: offset } => (*n, *offset, input.as_ref()),
            _ => (*n, 0, input.as_ref()),
        },
        LogicalPlan::Offset { input, n } => match input.as_ref() {
            LogicalPlan::Limit { input, n: limit } => (*limit, *n, input.as_ref()),
            _ => (u64::MAX, *n, input.as_ref()),
        },
        _ => return None,
    };
    match rest {
        LogicalPlan::Sort { input, keys } => Some((keys.clone(), limit, offset, input.as_ref())),
        _ => None,
    }
}

/// A join side's table and columns. A side that is not a bare scan has no
/// columns here, which is how the reference treats it too.
fn leaf_scan(plan: &LogicalPlan) -> (String, Vec<String>) {
    match plan {
        LogicalPlan::Scan { table, columns } | LogicalPlan::IndexScan { table, columns, .. } => {
            (table.clone(), columns.clone())
        }
        _ => (String::new(), Vec::new()),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_expression::Expr;
    use infinity_parser::AggFunc;
    use infinity_parser::CmpOp;

    fn scan() -> LogicalPlan {
        LogicalPlan::Scan {
            table: "t".into(),
            columns: vec!["c1".into(), "c2".into()],
        }
    }

    fn projection(input: LogicalPlan) -> LogicalPlan {
        LogicalPlan::Projection {
            input: Box::new(input),
            exprs: vec![Expr::Column(0)],
            names: vec!["c1".into()],
        }
    }

    fn kinds(lowered: &Lowered) -> Vec<super::super::PhysicalOperatorType> {
        lowered
            .operators
            .iter()
            .map(|operator| operator.operator_type())
            .collect()
    }

    #[test]
    fn lowers_a_projection_over_a_scan() {
        let plan = LogicalPlan::Projection {
            input: Box::new(scan()),
            exprs: vec![Expr::Column(1)],
            names: vec!["c2".into()],
        };
        let lowered = lower(&plan).unwrap();
        assert_eq!(
            kinds(&lowered),
            vec![super::super::PhysicalOperatorType::Projection]
        );
        assert_eq!(lowered.names, vec!["c2".to_string()]);
        match &lowered.source {
            SourceKind::Table { table, columns } => {
                assert_eq!(table, "t");
                assert_eq!(columns, &["c1".to_string(), "c2".to_string()]);
            }
            other => panic!("expected a table scan, got {other:?}"),
        }
    }

    #[test]
    fn a_scan_without_a_projection_lowers_to_an_empty_chain() {
        let lowered = lower(&scan()).unwrap();
        assert!(lowered.operators.is_empty());
        assert_eq!(lowered.names, vec!["c1".to_string(), "c2".to_string()]);
    }

    #[test]
    fn lowers_a_filter() {
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Filter {
                input: Box::new(scan()),
                predicate: Expr::Cmp(
                    Box::new(Expr::Column(0)),
                    CmpOp::Gt,
                    Box::new(Expr::Literal(Value::Int(1))),
                ),
            }),
            exprs: vec![Expr::Column(1)],
            names: vec!["c2".into()],
        };
        let lowered = lower(&plan).unwrap();
        // Execution order: the filter sits nearest the scan.
        assert_eq!(
            kinds(&lowered),
            vec![
                super::super::PhysicalOperatorType::Filter,
                super::super::PhysicalOperatorType::Projection
            ]
        );
        assert_eq!(lowered.names, vec!["c2".to_string()]);
    }

    #[test]
    fn lowers_an_aggregate() {
        let plan = LogicalPlan::Aggregate {
            input: Box::new(scan()),
            group_keys: vec![Expr::Column(0)],
            group_names: vec!["c1".into()],
            aggs: vec![(AggFunc::Count, Vec::new(), false, "count".into())],
        };
        let lowered = lower(&plan).unwrap();
        assert_eq!(
            kinds(&lowered),
            vec![super::super::PhysicalOperatorType::Aggregate]
        );
        // Without a projection above it, the aggregate names the output.
        assert_eq!(lowered.names, vec!["c1".to_string(), "count".to_string()]);
    }

    #[test]
    fn lowers_a_limit_over_a_projection() {
        let plan = LogicalPlan::Limit {
            input: Box::new(projection(scan())),
            n: 5,
        };
        let lowered = lower(&plan).unwrap();
        assert_eq!(
            kinds(&lowered),
            vec![
                super::super::PhysicalOperatorType::Projection,
                super::super::PhysicalOperatorType::Limit
            ]
        );
    }

    #[test]
    fn an_offset_becomes_a_limit_with_an_offset_budget() {
        let plan = LogicalPlan::Limit {
            input: Box::new(LogicalPlan::Offset {
                input: Box::new(projection(scan())),
                n: 3,
            }),
            n: 5,
        };
        let lowered = lower(&plan).unwrap();
        assert_eq!(
            kinds(&lowered),
            vec![
                super::super::PhysicalOperatorType::Projection,
                super::super::PhysicalOperatorType::Limit,
                super::super::PhysicalOperatorType::Limit
            ]
        );
    }

    #[test]
    fn fuses_a_sort_and_its_limit_into_one_top() {
        let plan = LogicalPlan::Limit {
            input: Box::new(LogicalPlan::Sort {
                input: Box::new(projection(scan())),
                keys: vec![(Expr::Column(0), true)],
            }),
            n: 10,
        };
        let lowered = lower(&plan).unwrap();
        assert_eq!(
            kinds(&lowered),
            vec![
                super::super::PhysicalOperatorType::Projection,
                super::super::PhysicalOperatorType::Top
            ]
        );
    }

    #[test]
    fn a_sort_without_a_limit_lowers_to_a_sort() {
        let plan = LogicalPlan::Sort {
            input: Box::new(projection(scan())),
            keys: vec![(Expr::Column(0), true)],
        };
        let lowered = lower(&plan).unwrap();
        assert_eq!(
            kinds(&lowered),
            vec![
                super::super::PhysicalOperatorType::Projection,
                super::super::PhysicalOperatorType::Sort
            ]
        );
        assert_eq!(lowered.names, vec!["c1".to_string()]);
    }

    #[test]
    fn a_projection_over_values_lowers_with_a_values_source() {
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Values {
                columns: vec!["a".into()],
                rows: vec![vec![Value::Int(1)]],
            }),
            exprs: vec![Expr::Column(0)],
            names: vec!["a".into()],
        };
        let lowered = lower(&plan).unwrap();
        assert_eq!(lowered.operators.len(), 1);
        assert_eq!(lowered.names, vec!["a".to_string()]);
        match &lowered.source {
            SourceKind::Values { columns, rows } => {
                assert_eq!(columns, &["a".to_string()]);
                assert_eq!(rows, &vec![vec![Value::Int(1)]]);
            }
            other => panic!("expected a values source, got {other:?}"),
        }
    }

    #[test]
    fn a_join_becomes_a_join_source() {
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Join {
                left: Box::new(LogicalPlan::Scan {
                    table: "l".into(),
                    columns: vec!["a".into()],
                }),
                right: Box::new(LogicalPlan::Scan {
                    table: "r".into(),
                    columns: vec!["b".into()],
                }),
                left_key: 0,
                right_key: 0,
                left_width: 1,
            }),
            exprs: vec![Expr::Column(0), Expr::Column(1)],
            names: vec!["a".into(), "b".into()],
        };
        let lowered = lower(&plan).unwrap();
        match &lowered.source {
            SourceKind::Join {
                columns,
                left,
                right,
                ..
            } => {
                assert_eq!(left, "l");
                assert_eq!(right, "r");
                assert_eq!(columns, &["a".to_string(), "b".to_string()]);
            }
            other => panic!("expected a join source, got {other:?}"),
        }
    }
}
