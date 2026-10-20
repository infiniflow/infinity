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
use super::project::Projection;
use super::PhysicalOperator;

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
            // Not ported yet: `Aggregate` and a bare `Sort` (no operator yet)
            // keep the stage pipeline.
            _ => return Ok(None),
        }
    }
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
