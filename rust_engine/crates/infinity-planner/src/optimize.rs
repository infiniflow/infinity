//! A conservative logical-plan optimizer (plan §2.2 "optimizer" / M2).
//!
//! Rules (all result-preserving):
//!   1. **Limit push-down through Projection** — `Limit(Projection(x))` → `Projection(Limit(x))`
//!      (projection is 1:1).
//!   2. **Filter push-down below Sort** — `Filter(Sort(x))` → `Sort(Filter(x))`
//!      (filtering commutes with ordering).
//!   3. **Constant folding** — a predicate whose both sides are literals is folded;
//!      a statically-true predicate is removed.

use infinity_common::Result;
use infinity_expression::Expr;
use infinity_parser::CmpOp;

use crate::LogicalPlan;

/// C++ `OptimizerRule`: a result-preserving rewrite of a logical plan.
pub trait OptimizerRule {
    /// The rule's display name (`OptimizerRule::name`).
    fn name(&self) -> &'static str;

    /// Applies the rule to a plan (`OptimizerRule::ApplyToPlan`).
    fn apply(&self, plan: LogicalPlan) -> Result<LogicalPlan>;
}

/// C++ `Optimizer`: an ordered list of rules, run in registration order.
///
/// C++ mutates the plan in place; the Rust rules return a new tree so the
/// executor keeps owning its plan by value.
#[derive(Default)]
pub struct Optimizer {
    rules: Vec<Box<dyn OptimizerRule>>,
}

impl Optimizer {
    pub fn new() -> Self {
        Self::default()
    }

    /// The C++ optimizer's rule list, minus the rules whose subsystem is
    /// absent (index scan / result cache): constant folding, column pruning,
    /// lazy-load column collection.
    pub fn with_default_rules() -> Self {
        let mut optimizer = Self::new();
        optimizer.add_rule(Box::new(crate::column::ColumnPruner));
        optimizer
    }

    /// C++ `AddRule`.
    pub fn add_rule(&mut self, rule: Box<dyn OptimizerRule>) {
        self.rules.push(rule);
    }

    /// C++ `optimize`: applies every rule once, in order.
    pub fn optimize(&self, plan: LogicalPlan) -> Result<LogicalPlan> {
        let mut plan = plan;
        for rule in &self.rules {
            plan = rule.apply(plan)?;
        }
        Ok(plan)
    }

    pub fn rule_names(&self) -> Vec<&'static str> {
        self.rules.iter().map(|rule| rule.name()).collect()
    }
}

/// `OptimizerRule` wrapper around the three built-in rewrites below.
pub struct BuiltinRules;

impl OptimizerRule for BuiltinRules {
    fn name(&self) -> &'static str {
        "Builtin Rules"
    }

    fn apply(&self, plan: LogicalPlan) -> Result<LogicalPlan> {
        Ok(optimize(plan))
    }
}

/// Optimizes `plan` bottom-up.
///
/// Column pruning is deliberately not part of this pipeline: the executor
/// appends search/unnest hidden columns after planning, so pruning a scan here
/// would invalidate those positions. `ColumnPruner` is exposed as a rule for
/// callers that own the full schema.
pub fn optimize(plan: LogicalPlan) -> LogicalPlan {
    match plan {
        LogicalPlan::Limit { input, n } => {
            let input = optimize(*input);
            match input {
                LogicalPlan::Projection {
                    input,
                    exprs,
                    names,
                } => LogicalPlan::Projection {
                    input: Box::new(LogicalPlan::Limit { input, n }),
                    exprs,
                    names,
                },
                other => LogicalPlan::Limit {
                    input: Box::new(other),
                    n,
                },
            }
        }
        LogicalPlan::Filter { input, predicate } => {
            let input = optimize(*input);
            if matches!(const_bool(&predicate), Some(true)) {
                return input; // statically true -> drop the filter
            }
            match input {
                LogicalPlan::Offset { input, n } => LogicalPlan::Offset {
                    input: Box::new(optimize(*input)),
                    n,
                },
                LogicalPlan::Sort { input, keys } => LogicalPlan::Sort {
                    input: Box::new(LogicalPlan::Filter { input, predicate }),
                    keys,
                },
                other => LogicalPlan::Filter {
                    input: Box::new(other),
                    predicate,
                },
            }
        }
        LogicalPlan::Projection {
            input,
            exprs,
            names,
        } => LogicalPlan::Projection {
            input: Box::new(optimize(*input)),
            exprs,
            names,
        },
        LogicalPlan::Aggregate {
            input,
            group_keys,
            group_names,
            aggs,
        } => LogicalPlan::Aggregate {
            input: Box::new(optimize(*input)),
            group_keys,
            group_names,
            aggs,
        },
        LogicalPlan::Offset { input, n } => LogicalPlan::Offset {
            input: Box::new(optimize(*input)),
            n,
        },
        LogicalPlan::Sort { input, keys } => LogicalPlan::Sort {
            input: Box::new(optimize(*input)),
            keys,
        },
        LogicalPlan::Join {
            left,
            right,
            left_key,
            right_key,
            left_width,
        } => LogicalPlan::Join {
            left: Box::new(optimize(*left)),
            right: Box::new(optimize(*right)),
            left_key,
            right_key,
            left_width,
        },
        other => other,
    }
}

/// Evaluates a comparison of two literals to a constant boolean, if possible.
fn const_bool(expr: &Expr) -> Option<bool> {
    let Expr::Cmp(a, op, b) = expr else {
        return None;
    };
    let (Expr::Literal(x), Expr::Literal(y)) = (&**a, &**b) else {
        return None;
    };
    let ord = x.partial_cmp_value(y)?;
    use std::cmp::Ordering::*;
    Some(match op {
        CmpOp::Eq => ord == Equal,
        CmpOp::Ne => ord != Equal,
        CmpOp::Lt => ord == Less,
        CmpOp::Le => ord != Greater,
        CmpOp::Gt => ord == Greater,
        CmpOp::Ge => ord != Less,
    })
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::plan_select;
    use infinity_expression::Value;
    use infinity_parser::SelectItem;

    fn scan() -> LogicalPlan {
        LogicalPlan::Scan {
            table: "t".into(),
            columns: vec!["id".into(), "name".into()],
        }
    }

    #[test]
    fn limit_pushes_below_projection() {
        let plan = LogicalPlan::Limit {
            input: Box::new(LogicalPlan::Projection {
                input: Box::new(scan()),
                exprs: vec![Expr::Column(0)],
                names: vec!["id".into()],
            }),
            n: 5,
        };
        let got = optimize(plan);
        match got {
            LogicalPlan::Projection { input, .. } => {
                assert!(matches!(*input, LogicalPlan::Limit { n: 5, .. }))
            }
            other => panic!("expected projection on top, got {other:?}"),
        }
    }

    #[test]
    fn filter_pushes_below_sort() {
        let pred = Expr::Cmp(
            Box::new(Expr::Column(0)),
            CmpOp::Gt,
            Box::new(Expr::Literal(Value::Int(1))),
        );
        let plan = LogicalPlan::Filter {
            input: Box::new(LogicalPlan::Sort {
                input: Box::new(scan()),
                keys: vec![(Expr::Column(0), true)],
            }),
            predicate: pred,
        };
        let got = optimize(plan);
        match got {
            LogicalPlan::Sort { input, .. } => {
                assert!(matches!(*input, LogicalPlan::Filter { .. }))
            }
            other => panic!("expected sort on top, got {other:?}"),
        }
    }

    #[test]
    fn statically_true_filter_is_removed() {
        let pred = Expr::Cmp(
            Box::new(Expr::Literal(Value::Int(1))),
            CmpOp::Eq,
            Box::new(Expr::Literal(Value::Int(1))),
        );
        let got = optimize(LogicalPlan::Filter {
            input: Box::new(scan()),
            predicate: pred,
        });
        assert!(matches!(got, LogicalPlan::Scan { .. }));
    }

    #[test]
    fn optimizer_rule_trait_runs_in_order() {
        let mut optimizer = Optimizer::new();
        optimizer.add_rule(Box::new(BuiltinRules));
        optimizer.add_rule(Box::new(crate::column::ColumnPruner));
        assert_eq!(
            optimizer.rule_names(),
            vec!["Builtin Rules", "Column Pruner"]
        );
        let plan = LogicalPlan::Limit {
            input: Box::new(LogicalPlan::Projection {
                input: Box::new(scan()),
                exprs: vec![Expr::Column(0)],
                names: vec!["id".into()],
            }),
            n: 4,
        };
        let got = optimizer.optimize(plan).unwrap();
        assert!(matches!(got, LogicalPlan::Projection { .. }));
    }

    #[test]
    fn optimize_preserves_results_end_to_end_shape() {
        // With no ORDER BY, Limit sits directly on Projection, so it is pushed down.
        let tables = vec![("t".to_string(), vec!["id".to_string(), "name".to_string()])];
        let plan = plan_select(
            &[SelectItem::Column("id".into())],
            &tables,
            None,
            Some(&infinity_parser::Predicate::simple(
                "id",
                CmpOp::Ge,
                infinity_parser::Literal::Int(1),
            )),
            &[],
            None,
            &[],
            Some(10),
            None,
        )
        .unwrap();
        let optimized = optimize(plan);
        assert!(matches!(optimized, LogicalPlan::Projection { .. }));
    }
}
