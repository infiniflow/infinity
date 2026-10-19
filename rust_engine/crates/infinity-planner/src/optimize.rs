//! The logical-plan optimizer (plan §2.2 "optimizer" / M2).
//!
//! Two layers live here:
//!
//! * [`optimize`] — the built-in, catalog-free rewrites used since M2:
//!   1. **Limit push-down through Projection** — `Limit(Projection(x))` → `Projection(Limit(x))`
//!      (projection is 1:1).
//!   2. **Filter push-down below Sort** — `Filter(Sort(x))` → `Sort(Filter(x))`
//!      (filtering commutes with ordering).
//!   3. **Constant folding** — a statically-true predicate is removed.
//! * [`Optimizer`] — the C++ `Optimizer` rule list, run in the reference's
//!   registration order (`src/planner/optimizer_impl.cpp:40-46`):
//!   `ApplyFastRoughFilter`, `IndexScanBuilder`, `ColumnPruner`, `LazyLoad`,
//!   `ColumnRemapper`, `ResultCacheGetter`.

use std::sync::Arc;

use infinity_common::Result;
use infinity_expression::Expr;
use infinity_parser::CmpOp;

use crate::column::{ColumnPruner, ColumnRemapper, LazyLoad};
use crate::index_scan::{ApplyFastRoughFilter, IndexCatalog};
use crate::regex_rewrite::RegexIndexCatalog;
use crate::result_cache::{ResultCache, ResultCacheGetter};
use crate::{output_schema, LogicalPlan};

/// C++ `OptimizerRule`: a result-preserving rewrite of a logical plan.
pub trait OptimizerRule {
    /// The rule's display name (`OptimizerRule::name`).
    fn name(&self) -> &'static str;

    /// Applies the rule to a plan (`OptimizerRule::ApplyToPlan`).
    fn apply(&self, plan: LogicalPlan) -> Result<LogicalPlan>;
}

/// C++ `ApplyFastRoughFilter` as an optimizer rule.
///
/// The reference attaches a block-level evaluator to every `Filter(Scan)`. The
/// Rust plan has no block layer here, so the one result the rule can act on is
/// a filter that can never be true: such a scan produces no rows, and the
/// sub-plan is folded to an empty relation with the same schema.
#[derive(Default)]
pub struct FastRoughFilterRule {
    catalog: IndexCatalog,
}

impl FastRoughFilterRule {
    pub fn new(catalog: IndexCatalog) -> Self {
        Self { catalog }
    }
}

impl OptimizerRule for FastRoughFilterRule {
    fn name(&self) -> &'static str {
        "Apply Fast Rough Filter"
    }

    fn apply(&self, plan: LogicalPlan) -> Result<LogicalPlan> {
        Ok(fold_unmatchable(plan, &self.catalog))
    }
}

/// Replaces `Filter(Scan)` with an empty relation when the rough filter proves
/// no block can match (C++ marks the scan with `AlwaysFalse`).
fn fold_unmatchable(plan: LogicalPlan, catalog: &IndexCatalog) -> LogicalPlan {
    match plan {
        LogicalPlan::Filter { input, predicate } => {
            let input = fold_unmatchable(*input, catalog);
            let unmatchable = ApplyFastRoughFilter::build(
                &LogicalPlan::Filter {
                    input: Box::new(input.clone()),
                    predicate: predicate.clone(),
                },
                catalog,
            )
            .iter()
            .any(|binding| {
                binding.evaluator.tag() == crate::index_scan::FastRoughFilterTag::AlwaysFalse
            });
            if unmatchable {
                return LogicalPlan::Values {
                    columns: output_schema(&input),
                    rows: Vec::new(),
                };
            }
            LogicalPlan::Filter {
                input: Box::new(input),
                predicate,
            }
        }
        LogicalPlan::Projection {
            input,
            exprs,
            names,
        } => LogicalPlan::Projection {
            input: Box::new(fold_unmatchable(*input, catalog)),
            exprs,
            names,
        },
        LogicalPlan::Aggregate {
            input,
            group_keys,
            group_names,
            aggs,
        } => LogicalPlan::Aggregate {
            input: Box::new(fold_unmatchable(*input, catalog)),
            group_keys,
            group_names,
            aggs,
        },
        LogicalPlan::Sort { input, keys } => LogicalPlan::Sort {
            input: Box::new(fold_unmatchable(*input, catalog)),
            keys,
        },
        LogicalPlan::Limit { input, n } => LogicalPlan::Limit {
            input: Box::new(fold_unmatchable(*input, catalog)),
            n,
        },
        LogicalPlan::Offset { input, n } => LogicalPlan::Offset {
            input: Box::new(fold_unmatchable(*input, catalog)),
            n,
        },
        LogicalPlan::Join {
            left,
            right,
            left_key,
            right_key,
            left_width,
        } => LogicalPlan::Join {
            left: Box::new(fold_unmatchable(*left, catalog)),
            right: Box::new(fold_unmatchable(*right, catalog)),
            left_key,
            right_key,
            left_width,
        },
        other => other,
    }
}

/// C++ `ColumnRemapper` as an optimizer rule.
///
/// In the reference the pruner removes columns and this pass rewrites the
/// bindings; here the pruner applies the [`ColumnRemapper`] as it prunes, so by
/// the time this rule runs the plan already carries compacted positions and the
/// identity map is the correct binding. It stays in the pipeline so the pass
/// order matches the reference and a caller can feed a different map.
pub struct ColumnRemapRule;

impl OptimizerRule for ColumnRemapRule {
    fn name(&self) -> &'static str {
        "Column Remapper"
    }

    fn apply(&self, plan: LogicalPlan) -> Result<LogicalPlan> {
        ColumnRemapper::identity_for(&plan).remap(plan)
    }
}

/// C++ `ResultCacheGetter` as an optimizer rule (registered only when a result
/// cache exists, and after pruning/remapping as the reference comments).
pub struct ResultCacheGetterRule {
    cache: Arc<dyn ResultCache + Send + Sync>,
    begin_ts: u64,
}

impl ResultCacheGetterRule {
    pub fn new(cache: Arc<dyn ResultCache + Send + Sync>, begin_ts: u64) -> Self {
        Self { cache, begin_ts }
    }
}

impl OptimizerRule for ResultCacheGetterRule {
    fn name(&self) -> &'static str {
        "Result Cache Getter"
    }

    fn apply(&self, plan: LogicalPlan) -> Result<LogicalPlan> {
        Ok(ResultCacheGetter::rewrite(
            plan,
            self.cache.as_ref(),
            self.begin_ts,
        ))
    }
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

    /// The C++ rule list (`optimizer_impl.cpp:40-46`) minus the two rules that
    /// cannot be expressed as a plan-only rewrite:
    ///
    /// * `IndexScanBuilder` returns the candidates a runtime has to execute, so
    ///   a rule that dropped the pushed predicate would lose rows; callers use
    ///   [`crate::index_scan::IndexScanBuilder::build_with_regex`] directly
    ///   (the executor does, as optimizer step 2).
    /// * `ResultCacheGetter` needs a cache and timestamp, so it is registered by
    ///   [`Self::with_result_cache`].
    ///
    /// The default list is therefore fast-rough-filter, column pruning,
    /// lazy-load and the binder remap.
    pub fn with_default_rules() -> Self {
        let mut optimizer = Self::new();
        optimizer.add_rule(Box::new(FastRoughFilterRule::default()));
        optimizer.add_rule(Box::new(ColumnPruner));
        optimizer.add_rule(Box::new(LazyLoad));
        optimizer.add_rule(Box::new(ColumnRemapRule));
        optimizer
    }

    /// The same list with the fast-rough-filter bound to a real catalog.
    pub fn with_catalog(catalog: IndexCatalog, _regex: Option<RegexIndexCatalog>) -> Self {
        let mut optimizer = Self::new();
        optimizer.add_rule(Box::new(FastRoughFilterRule::new(catalog)));
        optimizer.add_rule(Box::new(ColumnPruner));
        optimizer.add_rule(Box::new(LazyLoad));
        optimizer.add_rule(Box::new(ColumnRemapRule));
        optimizer
    }

    /// Appends the `ResultCacheGetter` rule (C++ registers it last).
    pub fn with_result_cache(
        mut self,
        cache: Arc<dyn ResultCache + Send + Sync>,
        begin_ts: u64,
    ) -> Self {
        self.add_rule(Box::new(ResultCacheGetterRule::new(cache, begin_ts)));
        self
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

/// Optimizes `plan` bottom-up (the catalog-free half of the optimizer).
///
/// Column pruning is part of [`Optimizer::with_default_rules`]; this function
/// keeps the original three rewrites for callers that have not bound a
/// catalog. [`crate::plan_select`] already returns a pruned plan.
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

    /// The default rule list is the reference's order (`optimizer_impl.cpp`).
    #[test]
    fn default_rule_list_matches_the_reference_order() {
        let optimizer = Optimizer::with_default_rules();
        assert_eq!(
            optimizer.rule_names(),
            vec![
                "Apply Fast Rough Filter",
                "Column Pruner",
                "Lazy Load",
                "Column Remapper",
            ]
        );
    }

    /// `ApplyFastRoughFilter`: a statically-false predicate folds to an empty
    /// relation with the scan's schema.
    #[test]
    fn rough_filter_folds_an_unmatchable_scan_to_no_rows() {
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Filter {
                input: Box::new(scan()),
                predicate: Expr::Literal(Value::Bool(false)),
            }),
            exprs: vec![Expr::Column(0)],
            names: vec!["id".into()],
        };
        let got = Optimizer::with_default_rules().optimize(plan).unwrap();
        match got {
            LogicalPlan::Projection { input, .. } => match *input {
                LogicalPlan::Values { columns, rows } => {
                    // pruning already ran, so only `id` survives
                    assert_eq!(columns, vec!["id".to_string()]);
                    assert!(rows.is_empty());
                }
                other => panic!("expected an empty relation, got {other:?}"),
            },
            other => panic!("expected projection, got {other:?}"),
        }
    }

    /// `IndexScanBuilder` splits the predicate: the sargable part becomes the
    /// candidate's index filter, the rest stays as the plan's residual filter,
    /// and `ColumnPruner` then keeps only the columns that residual reads.
    #[test]
    fn index_scan_builder_keeps_only_the_residual_filter() {
        use crate::column::ColumnPruner;
        use crate::index_scan::{ColumnKind, IndexCatalog, IndexScanBuilder};
        let catalog = IndexCatalog::new(&[("a", ColumnKind::Integer), ("b", ColumnKind::Integer)])
            .with_secondary("a", false);
        let pred = Expr::And(
            Box::new(Expr::Cmp(
                Box::new(Expr::Column(0)),
                CmpOp::Eq,
                Box::new(Expr::Literal(Value::Int(3))),
            )),
            Box::new(Expr::Cmp(
                Box::new(Expr::Column(1)),
                CmpOp::Gt,
                Box::new(Expr::Literal(Value::Int(1))),
            )),
        );
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Filter {
                input: Box::new(LogicalPlan::Scan {
                    table: "t".into(),
                    columns: vec!["a".into(), "b".into()],
                }),
                predicate: pred,
            }),
            exprs: vec![Expr::Column(1)],
            names: vec!["b".into()],
        };
        let indexed = IndexScanBuilder::build(plan, &catalog).unwrap();
        assert_eq!(indexed.candidates.len(), 1);
        assert_eq!(
            indexed.candidates[0].index_filter,
            Expr::Cmp(
                Box::new(Expr::Column(0)),
                CmpOp::Eq,
                Box::new(Expr::Literal(Value::Int(3)))
            )
        );
        let pruned = ColumnPruner::prune(indexed.plan).unwrap();
        match pruned {
            LogicalPlan::Projection { exprs, input, .. } => {
                assert_eq!(exprs, vec![Expr::Column(0)]);
                match *input {
                    LogicalPlan::Filter { input, predicate } => {
                        assert_eq!(
                            predicate,
                            Expr::Cmp(
                                Box::new(Expr::Column(0)),
                                CmpOp::Gt,
                                Box::new(Expr::Literal(Value::Int(1)))
                            )
                        );
                        assert!(
                            matches!(*input, LogicalPlan::Scan { columns, .. } if columns == ["b"])
                        );
                    }
                    other => panic!("expected the residual filter, got {other:?}"),
                }
            }
            other => panic!("expected projection, got {other:?}"),
        }
    }

    /// An un-indexed column cannot answer a predicate; the builder must leave
    /// the filter in place rather than drop it for an `AllTrue` evaluator.
    #[test]
    fn index_scan_builder_keeps_filters_it_cannot_answer() {
        use crate::index_scan::{ColumnKind, IndexCatalog, IndexScanBuilder};
        let catalog = IndexCatalog::new(&[("a", ColumnKind::Integer), ("b", ColumnKind::Integer)]);
        let plan = LogicalPlan::Filter {
            input: Box::new(LogicalPlan::Scan {
                table: "t".into(),
                columns: vec!["a".into(), "b".into()],
            }),
            predicate: Expr::Cmp(
                Box::new(Expr::Column(0)),
                CmpOp::Eq,
                Box::new(Expr::Literal(Value::Int(3))),
            ),
        };
        let indexed = IndexScanBuilder::build(plan.clone(), &catalog).unwrap();
        assert!(indexed.candidates.is_empty());
        assert_eq!(indexed.plan, plan);
    }

    /// `ResultCacheGetter` is the last rule and replaces a cached node.
    #[test]
    fn result_cache_rule_replaces_a_cache_hit() {
        use crate::result_cache::{cache_key, CacheContent, CacheMap};
        let plan = LogicalPlan::Scan {
            table: "t".into(),
            columns: vec!["id".into()],
        };
        let mut cache = CacheMap::new();
        cache.insert(
            cache_key(&plan, 9),
            CacheContent {
                columns: vec!["id".into()],
                rows: vec![vec![Value::Int(7)]],
            },
        );
        let optimizer = Optimizer::new().with_result_cache(Arc::new(cache), 9);
        assert_eq!(optimizer.rule_names(), vec!["Result Cache Getter"]);
        let got = optimizer.optimize(plan).unwrap();
        assert_eq!(
            got,
            LogicalPlan::Values {
                columns: vec!["id".into()],
                rows: vec![vec![Value::Int(7)]],
            }
        );
    }
}
