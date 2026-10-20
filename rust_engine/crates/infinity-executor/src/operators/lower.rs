//! Lowers a [`LogicalPlan`] into the physical operator chain.
//!
//! The reference builds this chain in `PhysicalPlanner` and the fragment
//! context wires one state per operator (`fragment_context_impl.cpp:494`). Only
//! the operators that have been ported are accepted here; anything else returns
//! `None` so `run_plan` keeps using the stage pipeline (the migration is
//! deliberately incremental: the fixture has to stay green at every step).

use infinity_common::Result;
use infinity_planner::LogicalPlan;

use super::filter::Filter;
use super::limit::Limit;
use super::project::Projection;
use super::top::Top;
use super::{PhysicalOperator, PhysicalOperatorType};

/// A lowered plan: the operator chain in execution order (scan first) and the
/// names of the columns it produces.
pub type Lowered = (Vec<Box<dyn PhysicalOperator>>, Vec<String>);

/// The operator chain for `plan`.
///
/// `None` means the plan uses a construct this slice has not ported yet.
pub fn lower(plan: &LogicalPlan) -> Result<Option<Lowered>> {
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
            LogicalPlan::Scan { columns, .. } | LogicalPlan::Values { columns, .. } => {
                if operators.is_empty() {
                    return Ok(None);
                }
                let names = names.unwrap_or_else(|| columns.clone());
                operators.reverse();
                return Ok(Some((operators, names)));
            }
            LogicalPlan::Join { .. } => {
                // The join columns are the two scans' column lists
                // concatenated (see `Engine::decompose`).
                let Some(columns) = leaf_columns(cursor) else {
                    return Ok(None);
                };
                if operators.is_empty() {
                    return Ok(None);
                }
                let names = names.unwrap_or(columns);
                operators.reverse();
                return Ok(Some((operators, names)));
            }
            // Not ported yet: `Aggregate`, and a `Sort` that is not fused into
            // a `Top` (slice 6 gives sort its own operator).
            _ => return Ok(None),
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

/// Whether the lowered chain covers exactly the stages `Engine::decompose`
/// finds, so `run_plan` knows the two views of the plan agree.
///
/// A `Top` covers the `Sort` it was fused with plus every `Limit`/`Offset`
/// above it; the other operators cover one stage each.
pub(crate) fn covers(operators: &[Box<dyn PhysicalOperator>], stages: &[crate::Stage]) -> bool {
    let mut operator_index = 0;
    let mut stage_index = 0;
    while operator_index < operators.len() {
        let Some(stage) = stages.get(stage_index) else {
            return false;
        };
        match operators[operator_index].operator_type() {
            PhysicalOperatorType::Projection if matches!(stage, crate::Stage::Projection(..)) => {}
            PhysicalOperatorType::Filter if matches!(stage, crate::Stage::Filter(..)) => {}
            PhysicalOperatorType::Limit
                if matches!(stage, crate::Stage::Limit(_) | crate::Stage::Offset(_)) => {}
            PhysicalOperatorType::Top if matches!(stage, crate::Stage::Sort { .. }) => {
                // The fused sort, plus the limit/offset budgets above it.
                stage_index += 1;
                while stage_index < stages.len()
                    && matches!(
                        stages[stage_index],
                        crate::Stage::Limit(_) | crate::Stage::Offset(_)
                    )
                {
                    stage_index += 1;
                }
                operator_index += 1;
                continue;
            }
            _ => return false,
        }
        operator_index += 1;
        stage_index += 1;
    }
    stage_index == stages.len()
}

/// The column list the relation under `plan` produces.
fn leaf_columns(plan: &LogicalPlan) -> Option<Vec<String>> {
    match plan {
        LogicalPlan::Scan { columns, .. } | LogicalPlan::Values { columns, .. } => {
            Some(columns.clone())
        }
        LogicalPlan::Join { left, right, .. } => {
            let mut columns = leaf_columns(left)?;
            columns.extend(leaf_columns(right)?);
            Some(columns)
        }
        LogicalPlan::Filter { input, .. }
        | LogicalPlan::Projection { input, .. }
        | LogicalPlan::Aggregate { input, .. }
        | LogicalPlan::Sort { input, .. }
        | LogicalPlan::Limit { input, .. }
        | LogicalPlan::Offset { input, .. } => leaf_columns(input),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_expression::{Expr, Value};

    fn scan() -> LogicalPlan {
        LogicalPlan::Scan {
            table: "t".into(),
            columns: vec!["c1".into(), "c2".into()],
        }
    }

    #[test]
    fn lowers_a_projection_over_a_scan() {
        let plan = LogicalPlan::Projection {
            input: Box::new(scan()),
            exprs: vec![Expr::Column(1)],
            names: vec!["c2".into()],
        };
        let (operators, names) = lower(&plan).unwrap().unwrap();
        assert_eq!(operators.len(), 1);
        assert_eq!(
            operators[0].operator_type(),
            super::super::PhysicalOperatorType::Projection
        );
        assert_eq!(names, vec!["c2".to_string()]);
    }

    #[test]
    fn a_scan_without_a_projection_is_not_lowered() {
        assert!(lower(&scan()).unwrap().is_none());
    }

    #[test]
    fn an_aggregate_falls_back_to_the_stage_pipeline() {
        let plan = LogicalPlan::Aggregate {
            input: Box::new(scan()),
            group_keys: vec![Expr::Column(0)],
            group_names: vec!["c1".into()],
            aggs: Vec::new(),
        };
        assert!(lower(&plan).unwrap().is_none());
    }

    #[test]
    fn lowers_a_filter() {
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Filter {
                input: Box::new(scan()),
                predicate: Expr::Cmp(
                    Box::new(Expr::Column(0)),
                    infinity_parser::CmpOp::Gt,
                    Box::new(Expr::Literal(Value::Int(1))),
                ),
            }),
            exprs: vec![Expr::Column(1)],
            names: vec!["c2".into()],
        };
        let (operators, names) = lower(&plan).unwrap().unwrap();
        assert_eq!(operators.len(), 2);
        // Execution order: the filter sits nearest the scan.
        assert_eq!(
            operators[0].operator_type(),
            super::super::PhysicalOperatorType::Filter
        );
        assert_eq!(
            operators[1].operator_type(),
            super::super::PhysicalOperatorType::Projection
        );
        assert_eq!(names, vec!["c2".to_string()]);
    }

    fn projection(input: LogicalPlan) -> LogicalPlan {
        LogicalPlan::Projection {
            input: Box::new(input),
            exprs: vec![Expr::Column(0)],
            names: vec!["c1".into()],
        }
    }

    #[test]
    fn lowers_a_limit_over_a_projection() {
        let plan = LogicalPlan::Limit {
            input: Box::new(projection(scan())),
            n: 5,
        };
        let (operators, _) = lower(&plan).unwrap().unwrap();
        assert_eq!(operators.len(), 2);
        assert_eq!(
            operators[0].operator_type(),
            super::super::PhysicalOperatorType::Projection
        );
        assert_eq!(
            operators[1].operator_type(),
            super::super::PhysicalOperatorType::Limit
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
        let (operators, _) = lower(&plan).unwrap().unwrap();
        // Two stages, two operators (offset first, then the limit).
        assert_eq!(operators.len(), 3);
        assert_eq!(
            operators[1].operator_type(),
            super::super::PhysicalOperatorType::Limit
        );
        assert_eq!(
            operators[2].operator_type(),
            super::super::PhysicalOperatorType::Limit
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
        let (operators, _) = lower(&plan).unwrap().unwrap();
        assert_eq!(operators.len(), 2);
        assert_eq!(
            operators[1].operator_type(),
            super::super::PhysicalOperatorType::Top
        );
    }

    #[test]
    fn a_sort_without_a_limit_still_falls_back() {
        let plan = LogicalPlan::Sort {
            input: Box::new(projection(scan())),
            keys: vec![(Expr::Column(0), true)],
        };
        assert!(lower(&plan).unwrap().is_none());
    }

    #[test]
    fn covers_accepts_the_chain_it_produced() {
        let plan = LogicalPlan::Limit {
            input: Box::new(LogicalPlan::Sort {
                input: Box::new(projection(scan())),
                keys: vec![(Expr::Column(0), true)],
            }),
            n: 10,
        };
        let (operators, _) = lower(&plan).unwrap().unwrap();
        let stages = vec![
            crate::Stage::Projection(vec![Expr::Column(0)], vec!["c1".into()]),
            crate::Stage::Sort {
                keys: vec![(Expr::Column(0), true)],
            },
            crate::Stage::Limit(10),
        ];
        assert!(covers(&operators, &stages));
        // A chain longer than the stage list is rejected...
        assert!(!covers(&operators, &stages[..1]));
        // ...and so is one whose operator does not match the stage below it.
        let mut with_aggregate = stages.clone();
        with_aggregate.insert(
            1,
            crate::Stage::Aggregate {
                group_keys: vec![Expr::Column(0)],
                group_names: vec!["c1".into()],
                aggs: Vec::new(),
            },
        );
        assert!(!covers(&operators, &with_aggregate));
    }

    #[test]
    fn a_projection_over_values_lowers() {
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Values {
                columns: vec!["a".into()],
                rows: vec![vec![Value::Int(1)]],
            }),
            exprs: vec![Expr::Column(0)],
            names: vec!["a".into()],
        };
        let (operators, names) = lower(&plan).unwrap().unwrap();
        assert_eq!(operators.len(), 1);
        assert_eq!(names, vec!["a".to_string()]);
    }
}
