//! `regex()` → full-text-index narrowing.
//!
//! C++ `RegexIndexRewrite` keeps a `regex(column, '<pattern>')` predicate but
//! conjoins a `filter_fulltext(column, '<run>', 'operator=and;similarity=boolean')`
//! for every literal run the pattern proves mandatory. The grams of those runs
//! then seed an index scan while the original regex stays as the residual
//! filter, so the rewrite cannot change the result.
//!
//! Only columns whose full-text index uses a sparse-gram analyzer participate;
//! a pattern without a usable run is left untouched.

use std::collections::HashMap;

use infinity_expression::regex_literal::{extract_regex_literal_runs, has_any_sparse_gram};
use infinity_expression::{Expr, Value};

/// The sparse-gram analyzer an indexed column was built with.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct SparseGramColumn {
    pub index_name: String,
    pub min_n: u32,
    pub max_n: u32,
    pub fold: bool,
}

/// The columns with a sparse-gram full-text index.
#[derive(Debug, Clone, Default)]
pub struct RegexIndexCatalog {
    pub columns: HashMap<usize, SparseGramColumn>,
}

impl RegexIndexCatalog {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn with_column(
        mut self,
        column: usize,
        index_name: impl Into<String>,
        min_n: u32,
        max_n: u32,
        fold: bool,
    ) -> Self {
        self.columns.insert(
            column,
            SparseGramColumn {
                index_name: index_name.into(),
                min_n,
                max_n,
                fold,
            },
        );
        self
    }
}

/// C++ `kMaxRunsPerPredicate`: the longest runs become the filters.
const MAX_RUNS_PER_PREDICATE: usize = 4;

/// A `filter_fulltext` conjunct over one mandatory literal run.
fn make_filter_fulltext(column: usize, column_info: &SparseGramColumn, run: &str) -> Expr {
    Expr::FullText(
        column,
        run.to_string(),
        format!(
            "operator=and;similarity=boolean;index={}",
            column_info.index_name
        ),
    )
}

/// Tries to rewrite one node; `None` when the node is not a rewritable regex.
fn try_rewrite(expr: &Expr, catalog: &RegexIndexCatalog) -> Option<Expr> {
    let Expr::Regex(column, pattern) = expr else {
        return None;
    };
    let column_info = catalog.columns.get(column)?;
    let Expr::Literal(Value::Str(pattern)) = &**pattern else {
        return None;
    };
    if pattern.is_empty() {
        return None;
    }
    let mut runs = extract_regex_literal_runs(pattern, false, column_info.fold);
    runs.retain(|run| has_any_sparse_gram(run, column_info.min_n, column_info.max_n));
    if runs.is_empty() {
        return None;
    }
    // Longest runs carry the most grams; keep the order stable on ties.
    runs.sort_by_key(|run| std::cmp::Reverse(run.len()));
    runs.truncate(MAX_RUNS_PER_PREDICATE);

    let mut conjuncts: Vec<Expr> = runs
        .iter()
        .map(|run| make_filter_fulltext(*column, column_info, run))
        .collect();
    // The pattern itself stays, verifying every candidate the grams admit.
    conjuncts.push(expr.clone());
    let mut iter = conjuncts.into_iter();
    let mut acc = iter.next().unwrap();
    for next in iter {
        acc = Expr::And(Box::new(acc), Box::new(next));
    }
    Some(acc)
}

fn rewrite_node(expr: &Expr, catalog: &RegexIndexCatalog) -> Expr {
    if let Some(replaced) = try_rewrite(expr, catalog) {
        return replaced;
    }
    match expr {
        Expr::And(a, b) => Expr::And(
            Box::new(rewrite_node(a, catalog)),
            Box::new(rewrite_node(b, catalog)),
        ),
        Expr::Or(a, b) => Expr::Or(
            Box::new(rewrite_node(a, catalog)),
            Box::new(rewrite_node(b, catalog)),
        ),
        other => other.clone(),
    }
}

/// C++ `RegexIndexRewrite`.
pub struct RegexIndexRewrite;

impl RegexIndexRewrite {
    /// C++ `RegexIndexRewrite::Rewrite`: returns the original expression
    /// unchanged when no column or pattern can be narrowed.
    pub fn rewrite(expression: &Expr, catalog: &RegexIndexCatalog) -> Expr {
        if catalog.columns.is_empty() {
            return expression.clone();
        }
        rewrite_node(expression, catalog)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn catalog() -> RegexIndexCatalog {
        RegexIndexCatalog::new().with_column(0, "idx_regex", 3, 12, false)
    }

    /// C++ `RegexIndexRewrite`: a mandatory run becomes a full-text conjunct
    /// and the regex is retained as the residual filter.
    #[test]
    fn rewrites_a_mandatory_run_into_a_fulltext_filter() {
        let expr = Expr::Regex(
            0,
            Box::new(Expr::Literal(Value::Str(
                "error (reading|writing) file".into(),
            ))),
        );
        let got = RegexIndexRewrite::rewrite(&expr, &catalog());
        // AND(AND(filter_fulltext("error "), filter_fulltext(" file")), regex)
        let mut conjuncts = Vec::new();
        fn flatten<'a>(e: &'a Expr, out: &mut Vec<&'a Expr>) {
            if let Expr::And(a, b) = e {
                flatten(a, out);
                flatten(b, out);
            } else {
                out.push(e);
            }
        }
        flatten(&got, &mut conjuncts);
        assert_eq!(
            conjuncts[0],
            &Expr::FullText(
                0,
                "error ".into(),
                "operator=and;similarity=boolean;index=idx_regex".into()
            )
        );
        assert!(conjuncts
            .iter()
            .any(|e| matches!(e, Expr::FullText(0, run, _) if run == " file")));
        assert_eq!(*conjuncts.last().unwrap(), &expr);
    }

    /// No sparse-gram index for the column: the predicate is unchanged.
    #[test]
    fn leaves_unindexed_columns_alone() {
        let expr = Expr::Regex(1, Box::new(Expr::Literal(Value::Str("nobel prize".into()))));
        assert_eq!(RegexIndexRewrite::rewrite(&expr, &catalog()), expr);
    }

    /// A pattern with no mandatory run (all alternatives) cannot be narrowed.
    #[test]
    fn leaves_alternation_only_patterns_alone() {
        let expr = Expr::Regex(
            0,
            Box::new(Expr::Literal(Value::Str("Nobel|nothing relevant".into()))),
        );
        assert_eq!(RegexIndexRewrite::rewrite(&expr, &catalog()), expr);
    }

    /// The rewrite descends through boolean conjunctions (C++ `RewriteNode`).
    #[test]
    fn rewrites_regex_inside_a_conjunction() {
        let regex = Expr::Regex(0, Box::new(Expr::Literal(Value::Str("error".into()))));
        let other = Expr::IsNull(Box::new(Expr::Column(1)), false);
        let got =
            RegexIndexRewrite::rewrite(&Expr::And(Box::new(regex), Box::new(other)), &catalog());
        match got {
            Expr::And(left, _) => assert!(matches!(*left, Expr::And(..))),
            other => panic!("expected conjunction, got {other:?}"),
        }
    }
}
