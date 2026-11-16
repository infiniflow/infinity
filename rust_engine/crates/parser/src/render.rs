//! Rendering a statement tree back to SQL text.
//!
//! This is **diagnostic only**: nothing in the engine executes the string it
//! produces. It exists so `EXPLAIN`, `SHOW QUERIES` and the debug switches can
//! show an operator the statement a request describes, in the form they would
//! have typed.
//!
//! Execution goes through the tree (`Engine::execute_parsed`). A front end must
//! never hand the output of this module back to the parser: a value the caller
//! controls would then be re-read as statement text, which is exactly the
//! round trip that let a REST `filter` carry `;` and a second statement.

use crate::ast::{
    BoolOp, ExprAst, Literal, MatchClause, OrderKey, Predicate, SearchClause, SelectItem, Statement,
};

/// An expression as SQL.
pub fn expr(e: &ExprAst) -> String {
    match e {
        ExprAst::Column(c) => c.clone(),
        ExprAst::Literal(l) => literal(l),
        ExprAst::Func { name, args } => format!(
            "{}({})",
            name,
            args.iter().map(expr).collect::<Vec<_>>().join(", ")
        ),
        ExprAst::Binary(a, op, b) => {
            format!("({} {} {})", expr(a), bin_op(*op), expr(b))
        }
        ExprAst::Cmp(a, op, b) => format!("({} {} {})", expr(a), op.as_str(), expr(b)),
        ExprAst::Not(a) => format!("NOT {}", expr(a)),
        ExprAst::And(a, b) => format!("({} AND {})", expr(a), expr(b)),
        ExprAst::Or(a, b) => format!("({} OR {})", expr(a), expr(b)),
    }
}

/// A literal as SQL. Strings are quoted and escaped, so the text is valid SQL
/// for display; it is still never re-parsed by the engine.
pub fn literal(l: &Literal) -> String {
    match l {
        Literal::Int(i) => i.to_string(),
        Literal::Float(f) => f.to_string(),
        Literal::Str(s) => format!("'{}'", s.replace('\'', "''")),
        Literal::Null => "NULL".into(),
        Literal::Array(items) => format!("[{}]", items.join(", ")),
        Literal::Vector(items) => format!(
            "[{}]",
            items
                .iter()
                .map(|x| x.to_string())
                .collect::<Vec<_>>()
                .join(", ")
        ),
    }
}

/// A predicate as SQL.
pub fn predicate(p: &Predicate) -> String {
    match p {
        Predicate::Cmp(a, op, b) => format!("{} {} {}", expr(a), op.as_str(), expr(b)),
        Predicate::IsTrue(e) => expr(e),
        Predicate::IsNull(e, negated) => {
            format!("{} IS {}NULL", expr(e), if *negated { "NOT " } else { "" })
        }
        Predicate::Like(e, pattern, escape, negated) => {
            let mut out = format!(
                "{} {}LIKE {}",
                expr(e),
                if *negated { "NOT " } else { "" },
                expr(pattern)
            );
            if let Some(c) = escape {
                out.push_str(&format!(" ESCAPE '{c}'"));
            }
            out
        }
        Predicate::In(e, values, negated) => format!(
            "{} {}IN ({})",
            expr(e),
            if *negated { "NOT " } else { "" },
            values.iter().map(expr).collect::<Vec<_>>().join(", ")
        ),
        Predicate::Bool(a, op, b) => format!(
            "({} {} {})",
            predicate(a),
            match op {
                BoolOp::And => "AND",
                BoolOp::Or => "OR",
            },
            predicate(b)
        ),
        Predicate::Not(a) => format!("NOT ({})", predicate(a)),
    }
}

/// One projection.
pub fn select_item(item: &SelectItem) -> String {
    match item {
        SelectItem::Star => "*".into(),
        SelectItem::Literal(l) => literal(l),
        SelectItem::Column(c) => c.clone(),
        SelectItem::Aggregate(a) => {
            let args = if a.args.is_empty() {
                "*".to_string()
            } else {
                a.args.join(", ")
            };
            format!(
                "{}({}{})",
                a.func.as_str(),
                if a.distinct { "DISTINCT " } else { "" },
                args
            )
        }
        SelectItem::Func { name, args } => format!(
            "{}({})",
            name,
            args.iter().map(expr).collect::<Vec<_>>().join(", ")
        ),
        SelectItem::Expr(e) => expr(e),
    }
}

/// One `ORDER BY` key.
pub fn order_key(k: &OrderKey) -> String {
    let key = match &k.expr {
        Some(e) => expr(e),
        None => k.column.clone(),
    };
    format!("{key} {}", if k.asc { "ASC" } else { "DESC" })
}

/// A `SEARCH` clause: its match terms, then the fusion tail.
pub fn search_clause(s: &SearchClause) -> String {
    let mut parts: Vec<String> = s.matches.iter().map(match_clause).collect();
    for (name, params) in &s.fusions {
        parts.push(if params.is_empty() {
            format!("FUSION('{name}')")
        } else {
            format!("FUSION('{name}', '{params}')")
        });
    }
    let mut out = parts.join(", ");
    if let Some(with) = &s.with_opts {
        out.push_str(&format!(" WITH ({with})"));
    }
    if let Some(index) = &s.using_index {
        out.push_str(&format!(" USING INDEX ({index})"));
    }
    out
}

/// One `MATCH <KIND> (…)` term.
pub fn match_clause(m: &MatchClause) -> String {
    let args = m.args.iter().map(expr).collect::<Vec<_>>().join(", ");
    let filter = match &m.filter {
        Some(p) => format!(", WHERE {}", predicate(p)),
        None => String::new(),
    };
    format!("MATCH {} ({}, {args}{filter})", m.kind, m.column)
}

/// A full statement as SQL, or `None` for one this module does not render.
pub fn statement(s: &Statement) -> Option<String> {
    let text = match s {
        Statement::Select {
            items,
            from,
            where_pred,
            group_by,
            having,
            order_by,
            limit,
            offset,
            search,
            distinct,
            highlight,
            ..
        } => {
            let cols = items.iter().map(select_item).collect::<Vec<_>>().join(", ");
            let mut out = format!("SELECT {}{cols}", if *distinct { "DISTINCT " } else { "" });
            if !highlight.is_empty() {
                out.push_str(&format!(" HIGHLIGHT {}", highlight.join(", ")));
            }
            out.push_str(&format!(" FROM {}", from.as_deref().unwrap_or_default()));
            if let Some(srch) = search {
                out.push_str(&format!(" SEARCH {}", search_clause(srch)));
            }
            if let Some(w) = where_pred {
                out.push_str(&format!(" WHERE {}", predicate(w)));
            }
            if !group_by.is_empty() {
                out.push_str(&format!(
                    " GROUP BY {}",
                    group_by.iter().map(expr).collect::<Vec<_>>().join(", ")
                ));
            }
            if let Some(h) = having {
                out.push_str(&format!(" HAVING {}", predicate(h)));
            }
            if !order_by.is_empty() {
                out.push_str(&format!(
                    " ORDER BY {}",
                    order_by
                        .iter()
                        .map(order_key)
                        .collect::<Vec<_>>()
                        .join(", ")
                ));
            }
            if let Some(n) = limit {
                out.push_str(&format!(" LIMIT {n}"));
            } else if offset.is_some() {
                // `OFFSET` without a limit means "every row after n"; the
                // grammar wants a LIMIT first.
                out.push_str(" LIMIT 9223372036854775807");
            }
            if let Some(n) = offset {
                out.push_str(&format!(" OFFSET {n}"));
            }
            out
        }
        Statement::Delete {
            table,
            where_pred: Some(p),
        } => format!("DELETE FROM {table} WHERE {}", predicate(p)),
        Statement::Delete {
            table,
            where_pred: None,
        } => format!("DELETE FROM {table}"),
        Statement::Update {
            table,
            assignments,
            where_pred,
        } => {
            let sets = assignments
                .iter()
                .map(|(c, e)| format!("{c} = {}", expr(e)))
                .collect::<Vec<_>>()
                .join(", ");
            match where_pred {
                Some(p) => format!("UPDATE {table} SET {sets} WHERE {}", predicate(p)),
                None => format!("UPDATE {table} SET {sets}"),
            }
        }
        _ => return None,
    };
    Some(format!("{text};"))
}

fn bin_op(op: crate::ast::BinOp) -> &'static str {
    use crate::ast::BinOp::*;
    match op {
        Add => "+",
        Sub => "-",
        Mul => "*",
        Div => "/",
        Mod => "%",
    }
}
