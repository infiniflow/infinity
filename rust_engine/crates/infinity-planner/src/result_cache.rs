//! Plan-level result cache lookup.
//!
//! C++ `ResultCacheGetter` replaces a scan node with a `LogicalReadCache` that
//! yields the cached columns and rows. The Rust plan has no dedicated read-cache
//! operator, so a hit is substituted with a `Values` node holding exactly the
//! cached content. The key is the node's shape plus the transaction's begin
//! timestamp, mirroring C++'s per-node cache key.

use std::collections::BTreeMap;

use infinity_expression::Value;

use crate::LogicalPlan;

/// C++ `CacheKey`: a node shape and the transaction timestamp it was read at.
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub struct CacheKey {
    pub node: String,
    pub begin_ts: u64,
}

/// C++ `CacheContent`: the cached columns and their rows.
#[derive(Debug, Clone, PartialEq)]
pub struct CacheContent {
    pub columns: Vec<String>,
    pub rows: Vec<Vec<Value>>,
}

/// A cache the getter can consult (C++ `ResultCacheManager`).
pub trait ResultCache {
    fn get(&self, key: &CacheKey) -> Option<CacheContent>;
}

/// The process-local map behind [`ResultCache`].
#[derive(Debug, Default)]
pub struct CacheMap {
    entries: BTreeMap<CacheKey, CacheContent>,
}

impl CacheMap {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn insert(&mut self, key: CacheKey, content: CacheContent) {
        self.entries.insert(key, content);
    }
}

impl ResultCache for CacheMap {
    fn get(&self, key: &CacheKey) -> Option<CacheContent> {
        self.entries.get(key).cloned()
    }
}

fn node_key(plan: &LogicalPlan) -> String {
    match plan {
        LogicalPlan::Values { columns, .. } => format!("values[{}]", columns.join(",")),
        LogicalPlan::Scan { table, columns } => format!("scan:{table}[{}]", columns.join(",")),
        LogicalPlan::Join {
            left,
            right,
            left_key,
            right_key,
            ..
        } => format!(
            "join({},{};{left_key},{right_key})",
            node_key(left),
            node_key(right)
        ),
        LogicalPlan::Filter { input, predicate } => {
            format!("filter({};{})", node_key(input), predicate_key(predicate))
        }
        LogicalPlan::Projection {
            input,
            exprs,
            names,
            ..
        } => format!(
            "project({};{};{})",
            node_key(input),
            exprs.iter().map(expr_key).collect::<Vec<_>>().join(","),
            names.join(",")
        ),
        LogicalPlan::Aggregate {
            input,
            group_keys,
            aggs,
            ..
        } => format!(
            "aggregate({};{};{})",
            node_key(input),
            group_keys
                .iter()
                .map(expr_key)
                .collect::<Vec<_>>()
                .join(","),
            aggs.iter()
                .map(|(_, args, distinct, label)| {
                    format!(
                        "{label}[{}]{distinct}",
                        args.iter()
                            .map(|i| i.to_string())
                            .collect::<Vec<_>>()
                            .join(",")
                    )
                })
                .collect::<Vec<_>>()
                .join(",")
        ),
        LogicalPlan::Sort { input, keys } => format!(
            "sort({};{})",
            node_key(input),
            keys.iter()
                .map(|(e, asc)| format!("{}:{asc}", expr_key(e)))
                .collect::<Vec<_>>()
                .join(",")
        ),
        LogicalPlan::Limit { input, n } => format!("limit({};{n})", node_key(input)),
        LogicalPlan::Offset { input, n } => format!("offset({};{n})", node_key(input)),
    }
}

fn expr_key(expr: &infinity_expression::Expr) -> String {
    format!("{expr:?}")
}

fn predicate_key(expr: &infinity_expression::Expr) -> String {
    expr_key(expr)
}

/// C++ `ResultCacheGetter`: the cache key for `plan` at `begin_ts`.
pub fn cache_key(plan: &LogicalPlan, begin_ts: u64) -> CacheKey {
    CacheKey {
        node: node_key(plan),
        begin_ts,
    }
}

/// C++ `ResultCacheGetter`.
pub struct ResultCacheGetter;

impl ResultCacheGetter {
    /// Looks the node's cache entry up (C++ `ResultCacheManager::GetCache`).
    pub fn get(cache: &dyn ResultCache, begin_ts: u64, plan: &LogicalPlan) -> Option<CacheContent> {
        cache.get(&cache_key(plan, begin_ts))
    }

    /// Replaces every cached node with its rows (C++ `LogicalReadCache`).
    pub fn rewrite(plan: LogicalPlan, cache: &dyn ResultCache, begin_ts: u64) -> LogicalPlan {
        if let Some(content) = Self::get(cache, begin_ts, &plan) {
            return LogicalPlan::Values {
                columns: content.columns,
                rows: content.rows,
            };
        }
        rewrite_children(plan, cache, begin_ts)
    }
}

fn rewrite_children(plan: LogicalPlan, cache: &dyn ResultCache, begin_ts: u64) -> LogicalPlan {
    match plan {
        LogicalPlan::Scan { .. } | LogicalPlan::Values { .. } => plan,
        LogicalPlan::Filter { input, predicate } => LogicalPlan::Filter {
            input: Box::new(ResultCacheGetter::rewrite(*input, cache, begin_ts)),
            predicate,
        },
        LogicalPlan::Projection {
            input,
            exprs,
            names,
        } => LogicalPlan::Projection {
            input: Box::new(ResultCacheGetter::rewrite(*input, cache, begin_ts)),
            exprs,
            names,
        },
        LogicalPlan::Aggregate {
            input,
            group_keys,
            group_names,
            aggs,
        } => LogicalPlan::Aggregate {
            input: Box::new(ResultCacheGetter::rewrite(*input, cache, begin_ts)),
            group_keys,
            group_names,
            aggs,
        },
        LogicalPlan::Sort { input, keys } => LogicalPlan::Sort {
            input: Box::new(ResultCacheGetter::rewrite(*input, cache, begin_ts)),
            keys,
        },
        LogicalPlan::Limit { input, n } => LogicalPlan::Limit {
            input: Box::new(ResultCacheGetter::rewrite(*input, cache, begin_ts)),
            n,
        },
        LogicalPlan::Offset { input, n } => LogicalPlan::Offset {
            input: Box::new(ResultCacheGetter::rewrite(*input, cache, begin_ts)),
            n,
        },
        LogicalPlan::Join {
            left,
            right,
            left_key,
            right_key,
            left_width,
        } => LogicalPlan::Join {
            left: Box::new(ResultCacheGetter::rewrite(*left, cache, begin_ts)),
            right: Box::new(ResultCacheGetter::rewrite(*right, cache, begin_ts)),
            left_key,
            right_key,
            left_width,
        },
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::output_schema;
    use infinity_expression::Value;

    fn scan() -> LogicalPlan {
        LogicalPlan::Scan {
            table: "t".into(),
            columns: vec!["a".into()],
        }
    }

    /// A cached scan is replaced by a `Values` node with the cached rows.
    #[test]
    fn result_cache_getter_replaces_a_hit() {
        let plan = scan();
        let key = cache_key(&plan, 42);
        let mut cache = CacheMap::new();
        cache.insert(
            key,
            CacheContent {
                columns: vec!["a".into()],
                rows: vec![vec![Value::Int(1)]],
            },
        );
        let got = ResultCacheGetter::rewrite(plan, &cache, 42);
        assert_eq!(
            got,
            LogicalPlan::Values {
                columns: vec!["a".into()],
                rows: vec![vec![Value::Int(1)]],
            }
        );
    }

    /// A miss (including a different begin timestamp) leaves the plan alone.
    #[test]
    fn result_cache_getter_keeps_a_miss() {
        let plan = scan();
        let mut cache = CacheMap::new();
        cache.insert(
            cache_key(&plan, 1),
            CacheContent {
                columns: vec!["a".into()],
                rows: vec![vec![Value::Int(1)]],
            },
        );
        assert_eq!(ResultCacheGetter::rewrite(plan.clone(), &cache, 2), plan);
    }

    /// A hit on a child rewrites only that child (C++ visits children).
    #[test]
    fn result_cache_getter_rewrites_nested_nodes() {
        let plan = LogicalPlan::Limit {
            input: Box::new(scan()),
            n: 3,
        };
        let mut cache = CacheMap::new();
        cache.insert(
            cache_key(&scan(), 7),
            CacheContent {
                columns: vec!["a".into()],
                rows: vec![vec![Value::Int(5)]],
            },
        );
        let got = ResultCacheGetter::rewrite(plan, &cache, 7);
        match got {
            LogicalPlan::Limit { input, n } => {
                assert_eq!(n, 3);
                assert!(matches!(*input, LogicalPlan::Values { .. }));
            }
            other => panic!("expected limit, got {other:?}"),
        }
        assert_eq!(output_schema(&scan()), vec!["a".to_string()]);
    }
}
