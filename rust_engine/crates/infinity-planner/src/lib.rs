//! `infinity-planner`: builds a logical plan from a parsed `SELECT`.
//! Operators are deliberately simple (M2); the Tokio executor consumes this tree.

#![forbid(unsafe_code)]

pub mod binder;
pub mod column;
pub mod column_binding;
pub mod index_scan;
pub mod optimize;
pub mod regex_rewrite;
pub mod result_cache;
pub mod subquery;

pub use column_binding::ColumnBinding;

use infinity_common::{Error, Result};
use infinity_expression::{Expr, Value};
use infinity_parser::{
    AggFunc, BoolOp, ExprAst, JoinClause, Literal, OrderKey, Predicate, SelectItem,
};

/// The column names produced by `plan`.
pub fn output_schema(plan: &LogicalPlan) -> Vec<String> {
    match plan {
        LogicalPlan::Values { columns, .. } => columns.clone(),
        LogicalPlan::Scan { columns, .. } => columns.clone(),
        LogicalPlan::Join {
            left,
            right,
            left_width,
            ..
        } => {
            let mut names = output_schema(left);
            names.truncate(*left_width);
            names.extend(output_schema(right));
            names
        }
        LogicalPlan::Filter { input, .. }
        | LogicalPlan::Sort { input, .. }
        | LogicalPlan::Limit { input, .. }
        | LogicalPlan::Offset { input, .. } => output_schema(input),
        LogicalPlan::Projection { names, .. } => names.clone(),
        LogicalPlan::Aggregate {
            group_names, aggs, ..
        } => {
            let mut names = group_names.clone();
            names.extend(aggs.iter().map(|(_, _, _, label)| label.clone()));
            names
        }
    }
}

/// The number of columns produced by `plan`.
pub fn output_width(plan: &LogicalPlan) -> usize {
    match plan {
        LogicalPlan::Values { columns, .. } => columns.len(),
        LogicalPlan::Scan { columns, .. } => columns.len(),
        LogicalPlan::Join {
            left_width, right, ..
        } => left_width + output_width(right),
        LogicalPlan::Filter { input, .. }
        | LogicalPlan::Sort { input, .. }
        | LogicalPlan::Limit { input, .. }
        | LogicalPlan::Offset { input, .. } => output_width(input),
        LogicalPlan::Projection { exprs, .. } => exprs.len(),
        LogicalPlan::Aggregate {
            group_keys, aggs, ..
        } => group_keys.len() + aggs.len(),
    }
}

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
        /// group-key expressions, evaluated per input row
        group_keys: Vec<Expr>,
        group_names: Vec<String>,
        aggs: Vec<(AggFunc, Vec<usize>, bool, String)>,
    },
    Sort {
        input: Box<LogicalPlan>,
        /// sort keys in priority order, each `(key expression, ascending)`
        keys: Vec<(Expr, bool)>,
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

/// Resolves a select-list column, mapping the `_row_id` system column onto the
/// hidden `#_row_id` scan column the executor appends for `ROW_ID()`.
fn resolve_output_column(schema: &[String], name: &str) -> Result<usize> {
    if name.eq_ignore_ascii_case("_row_id") {
        if let Some(i) = schema.iter().position(|c| c == "#_row_id") {
            return Ok(i);
        }
    }
    resolve_column(schema, name)
}

/// Compiles a parsed scalar expression against a table schema.
pub fn compile_expr(e: &ExprAst, schema: &[String]) -> Result<Expr> {
    Ok(match e {
        ExprAst::Column(c) => Expr::Column(resolve_output_column(schema, c)?),
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
            // a CAST between a scalar and a vector type is invalid
            if up == "CAST" {
                if let Some(ExprAst::Literal(Literal::Str(ty))) = args.get(1) {
                    let ty_up = ty.to_ascii_uppercase();
                    let vector_target = [
                        "VECTOR",
                        "TENSOR",
                        "TENSORARRAY",
                        "MULTIVECTOR",
                        "SPARSE",
                        "EMBEDDING",
                    ]
                    .iter()
                    .any(|k| ty_up.starts_with(k));
                    match args.first() {
                        Some(ExprAst::Literal(Literal::Array(_))) if !vector_target => {
                            return Err(Error::Invalid(format!("cannot cast an array to '{ty}'")))
                        }
                        Some(ExprAst::Literal(_)) if vector_target => {
                            return Err(Error::Invalid(format!("cannot cast a scalar to '{ty}'")))
                        }
                        _ => {}
                    }
                }
            }
            if up == "FILTER_TEXT" || up == "FILTER_FULLTEXT" {
                if let (Some(ExprAst::Literal(Literal::Str(col))), Some(q)) =
                    (args.first(), args.get(1))
                {
                    let opts = match args.get(2) {
                        Some(ExprAst::Literal(Literal::Str(o))) => o.clone(),
                        _ => String::new(),
                    };
                    // `col@index` names the column plus an optional index hint.
                    let mut col_name = col.split('@').next().unwrap_or(col).to_string();
                    // An empty field falls back to `default_field=<col>`.
                    if col_name.is_empty() {
                        if let Some(f) = opts.split(';').find_map(|p| {
                            let (k, v) = p.split_once('=')?;
                            k.trim()
                                .eq_ignore_ascii_case("default_field")
                                .then(|| v.trim().to_string())
                        }) {
                            col_name = f;
                        }
                    }
                    if let (Some(i), ExprAst::Literal(Literal::Str(query))) =
                        (schema.iter().position(|c| c == &col_name), q)
                    {
                        return Ok(Expr::FullText(i, query.clone(), opts));
                    }
                }
            }
            // `regex(col, '…')` resolves the column at compile time.
            if up == "REGEX" {
                if let (Some(ExprAst::Column(col)), Some(pat)) = (args.first(), args.get(1)) {
                    if let Some(i) = schema.iter().position(|c| c == col) {
                        return Ok(Expr::Regex(i, Box::new(compile_expr(pat, schema)?)));
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
    // The SDK reads `count(*)` by the reference's `count(star)` key, and AVG is
    // reported as the `(sum(x) / count(x))` expression the reference plans.
    if args.is_empty() || args.iter().all(|a| a == "*") {
        return format!("{}(star)", func.as_str());
    }
    if func == AggFunc::Avg && args.len() == 1 {
        return format!("(sum({0}) / count({0}))", args[0]);
    }
    format!("{}({})", func.as_str(), args.join(","))
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
    // A select expression that matches a GROUP BY key resolves to that key's
    // output column.
    let text = key_text(e);
    if let Some(i) = group_by.iter().position(|g| *g == text) {
        return Ok(Expr::Column(i));
    }
    Ok(match e {
        ExprAst::Column(c) => {
            return Err(Error::Invalid(format!(
                "column '{c}' must appear in GROUP BY or inside an aggregate"
            )));
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

/// A canonical text for an expression, used to match select items against
/// GROUP BY keys (both are parsed from the same query text).
/// The C++ spelling of a cast target (`varchar` -> `Varchar`).
fn cpp_type_name(t: &str) -> String {
    match t.to_ascii_lowercase().as_str() {
        "varchar" => "Varchar".into(),
        "integer" | "int" | "int32" => "Integer".into(),
        "bigint" | "int64" => "BigInt".into(),
        "tinyint" | "int8" => "TinyInt".into(),
        "smallint" | "int16" => "SmallInt".into(),
        "float" | "float32" => "Float".into(),
        "double" | "float64" => "Double".into(),
        "boolean" | "bool" => "Boolean".into(),
        "date" => "Date".into(),
        "datetime" => "DateTime".into(),
        "time" => "Time".into(),
        other => {
            let mut c = other.chars();
            match c.next() {
                Some(f) => f.to_ascii_uppercase().to_string() + c.as_str(),
                None => String::new(),
            }
        }
    }
}

/// A literal as the reference prints it in an expression name (`0.1`, not
/// `0.100000`).
fn name_literal(l: &infinity_parser::Literal) -> String {
    match l {
        infinity_parser::Literal::Float(f) => {
            let s = format!("{f}");
            if s.contains('.') || s.contains('e') {
                s
            } else {
                format!("{s}.0")
            }
        }
        other => other.to_slt_string(),
    }
}

fn key_text(e: &ExprAst) -> String {
    match e {
        ExprAst::Column(c) => c.clone(),
        ExprAst::Literal(l) => name_literal(l),
        // `CAST(x AS type)` prints as the reference's `Cast(x AS Type)`.
        ExprAst::Func { name, args } if name.eq_ignore_ascii_case("cast") && args.len() == 2 => {
            // `CAST(x AS T)` and the REST spelling `cast(x, T)` both print as
            // the reference's `Cast(x AS T)`.
            let ty = match &args[1] {
                ExprAst::Literal(infinity_parser::Literal::Str(s)) => cpp_type_name(s),
                ExprAst::Column(c) => cpp_type_name(c),
                _ => String::new(),
            };
            format!("Cast({} AS {ty})", key_text(&args[0]))
        }
        ExprAst::Func { name, args } => {
            // The search pseudo-columns are named by their bare upper-case name
            // (`SIMILARITY`, `DISTANCE`, `SCORE`, `ROW_ID`), as the SDK reads them.
            let pseudo = name.to_ascii_uppercase();
            if matches!(
                pseudo.as_str(),
                "SIMILARITY" | "DISTANCE" | "SCORE" | "ROW_ID"
            ) {
                return pseudo;
            }
            // The full-text filter functions print as `FILTER_FULLTEXT('col', 'q')`
            // in the reference; other scalar functions keep the lower-case name.
            let up = name.to_ascii_uppercase();
            if up == "FILTER_TEXT" || up == "FILTER_FULLTEXT" {
                let rendered = args
                    .iter()
                    .map(|a| match a {
                        ExprAst::Literal(infinity_parser::Literal::Str(s)) => format!("'{s}'"),
                        _ => key_text(a),
                    })
                    .collect::<Vec<_>>()
                    .join(", ");
                return format!("FILTER_FULLTEXT({rendered})");
            }
            format!(
                "{}({})",
                name.to_ascii_lowercase(),
                args.iter().map(key_text).collect::<Vec<_>>().join(", ")
            )
        }
        // Unary minus is parsed as `0 - x`; print it as `-x` (a function
        // operand keeps its parentheses and upper-case name, as the reference
        // prints `-(ABS(num))`).
        ExprAst::Binary(a, infinity_parser::BinOp::Sub, b)
            if matches!(**a, ExprAst::Literal(infinity_parser::Literal::Int(0))) =>
        {
            match b.as_ref() {
                ExprAst::Func { name, args } => format!(
                    "-({}({}))",
                    name.to_ascii_uppercase(),
                    args.iter().map(key_text).collect::<Vec<_>>().join(", ")
                ),
                other => format!("-{}", key_text(other)),
            }
        }
        ExprAst::Binary(a, op, b) => {
            let sym = match op {
                infinity_parser::BinOp::Add => "+",
                infinity_parser::BinOp::Sub => "-",
                infinity_parser::BinOp::Mul => "*",
                infinity_parser::BinOp::Div => "/",
                infinity_parser::BinOp::Mod => "%",
            };
            format!("({} {} {})", key_text(a), sym, key_text(b))
        }
        ExprAst::Cmp(a, op, b) => {
            format!("{} {} {}", key_text(a), op.as_str(), key_text(b))
        }
        ExprAst::Not(a) => format!("NOT ({})", key_text(a)),
        ExprAst::And(a, b) => format!("({} AND {})", key_operand(a), key_operand(b)),
        ExprAst::Or(a, b) => format!("({} OR {})", key_operand(a), key_operand(b)),
    }
}

/// A logical operator's operands are parenthesised when they are comparisons.
fn key_operand(e: &ExprAst) -> String {
    match e {
        ExprAst::Cmp(..) | ExprAst::And(..) | ExprAst::Or(..) => format!("({})", key_text(e)),
        _ => key_text(e),
    }
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
pub fn item_label(it: &SelectItem) -> String {
    match it {
        SelectItem::Aggregate(a) => agg_label(a.func, &a.args),
        SelectItem::Column(c) => c.clone(),
        // The C++ engine names an un-aliased function column after the
        // expression text (`year(c1)`, `Cast(c3 AS Varchar)`), not the bare name.
        SelectItem::Func { name, args } => key_text(&ExprAst::Func {
            name: name.clone(),
            args: args.clone(),
        }),
        SelectItem::Literal(_) => "c0".to_string(),
        SelectItem::Expr(e) => key_text(e),
        SelectItem::Star => "*".to_string(),
    }
}

pub(crate) fn build_source(
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
    group_by: &[ExprAst],
    having: Option<&Predicate>,
    order_by: &[OrderKey],
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
    // (visible width, visible names) when ORDER BY needed a hidden column
    let mut hidden_order: Option<(usize, Vec<String>)> = None;

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
        // Aggregate arguments that are computed expressions (e.g.
        // `COUNT(CAST(c3 AS INTEGER))`) are materialised as extra input columns.
        let mut pre_exprs: Vec<ExprAst> = Vec::new();
        let mut pre_pos: std::collections::HashMap<String, usize> = Default::default();
        for it in items {
            if let SelectItem::Aggregate(a) = it {
                for oe in a.arg_exprs.iter().flatten() {
                    pre_pos.entry(key_text(oe)).or_insert_with(|| {
                        let i = pre_exprs.len();
                        pre_exprs.push(oe.clone());
                        i
                    });
                }
            }
        }
        let base_cols = schema.len();
        let mut agg_schema = schema.clone();
        agg_schema.extend(pre_exprs.iter().map(key_text));
        if !pre_exprs.is_empty() {
            let mut exprs: Vec<Expr> = (0..schema.len()).map(Expr::Column).collect();
            let mut names: Vec<String> = schema.clone();
            for e in &pre_exprs {
                exprs.push(compile_expr(e, &schema)?);
                names.push(key_text(e));
            }
            plan = LogicalPlan::Projection {
                input: Box::new(plan),
                exprs,
                names,
            };
        }
        // Group keys may be plain columns or arbitrary expressions; keep both a
        // canonical key text (for matching select items) and the compiled form.
        let group_texts: Vec<String> = group_by.iter().map(key_text).collect();
        let group_keys: Vec<Expr> = group_by
            .iter()
            .map(|e| compile_expr(e, &agg_schema))
            .collect::<Result<Vec<_>>>()?;
        let base = group_by.len();
        let mut aggs: Vec<(AggFunc, Vec<usize>, bool, String)> = Vec::new();
        let mut agg_index: std::collections::HashMap<String, usize> = Default::default();
        for it in items {
            match it {
                SelectItem::Aggregate(a) => {
                    let label = agg_label(a.func, &a.args);
                    if !agg_index.contains_key(&label) {
                        let idxs: Vec<usize> = a
                            .args
                            .iter()
                            .enumerate()
                            .map(|(j, c)| match a.arg_exprs.get(j).and_then(|o| o.as_ref()) {
                                Some(e) => Ok(base_cols + pre_pos[&key_text(e)]),
                                None => resolve_column(&agg_schema, c),
                            })
                            .collect::<Result<Vec<_>>>()?;
                        agg_index.insert(label.clone(), base + aggs.len());
                        aggs.push((a.func, idxs, a.distinct, label));
                    }
                }
                SelectItem::Expr(e) => {
                    gather_agg(e, &schema, &mut aggs, &mut agg_index, base)?;
                }
                SelectItem::Column(c) => {
                    if !group_texts.iter().any(|g| g == c) {
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
        let mut agg_names: Vec<String> = group_texts.clone();
        agg_names.extend(aggs.iter().map(|(_, _, _, l)| l.clone()));
        plan = LogicalPlan::Aggregate {
            input: Box::new(plan),
            group_keys,
            group_names: group_texts.clone(),
            aggs,
        };
        out_names = agg_names.clone();
        // HAVING filters the aggregate output (group columns + aggregate results).
        if let Some(h) = having {
            let pred = compile_agg_predicate(h, &agg_index, &group_texts)?;
            plan = LogicalPlan::Filter {
                input: Box::new(plan),
                predicate: pred,
            };
        }
        // Always project the select list so duplicate aggregates and column
        // order are preserved one output column per select item.
        if !items.is_empty() {
            let mut exprs = Vec::new();
            let mut names = Vec::new();
            for it in items {
                exprs.push(item_agg_expr(it, &agg_index, &group_texts, &agg_names)?);
                names.push(item_label(it));
            }
            plan = LogicalPlan::Projection {
                input: Box::new(plan),
                exprs,
                names: names.clone(),
            };
            out_names = names;
        }
    } else if !is_star || schema.iter().any(|c| c.starts_with("#_")) {
        let mut exprs = Vec::new();
        let mut names = Vec::new();
        for it in items {
            match it {
                SelectItem::Column(name) => {
                    exprs.push(Expr::Column(resolve_output_column(&schema, name)?));
                    names.push(name.clone());
                }
                SelectItem::Func { name, args } => {
                    let e = ExprAst::Func {
                        name: name.clone(),
                        args: args.clone(),
                    };
                    // Name the column after the expression, as the C++ engine
                    // does (`year(c1)`, `datepart(c4, c1)`) — the SDK reads
                    // results by that name.
                    names.push(item_label(it));
                    exprs.push(compile_expr(&e, &schema)?);
                }
                SelectItem::Literal(l) => {
                    names.push(format!("c{}", exprs.len()));
                    exprs.push(Expr::Literal(Value::from_literal(l)));
                }
                SelectItem::Expr(e) => {
                    names.push(key_text(e));
                    exprs.push(compile_expr(e, &schema)?);
                }
                SelectItem::Star => {
                    for (i, c) in schema.iter().enumerate() {
                        if c.starts_with("#_") {
                            continue;
                        }
                        exprs.push(Expr::Column(i));
                        names.push(c.clone());
                    }
                }
                _ => return Err(Error::Invalid("projection must reference columns".into())),
            }
        }
        // `ORDER BY` may name a column that is not projected. Carry it as a
        // hidden trailing column and drop it again after the sort.
        let visible = names.len();
        for ok in order_by {
            let col = match &ok.expr {
                Some(ExprAst::Column(c)) => c.clone(),
                Some(_) => continue,
                None => ok.column.clone(),
            };
            if names.iter().any(|n| n == &col) {
                continue;
            }
            let Ok(idx) = resolve_column(&schema, &col) else {
                continue;
            };
            exprs.push(Expr::Column(idx));
            names.push(col);
        }
        if names.len() > visible {
            hidden_order = Some((visible, names[..visible].to_vec()));
        }
        plan = LogicalPlan::Projection {
            input: Box::new(plan),
            exprs,
            names: names.clone(),
        };
        out_names = names;
    }

    if !order_by.is_empty() {
        let keys: Vec<(Expr, bool)> = order_by
            .iter()
            .map(|ok| {
                let e = match &ok.expr {
                    Some(e) if !matches!(e, ExprAst::Column(_)) => compile_expr(e, &out_names)?,
                    _ => Expr::Column(resolve_column(&out_names, &ok.column)?),
                };
                Ok((e, ok.asc))
            })
            .collect::<Result<Vec<_>>>()?;
        plan = LogicalPlan::Sort {
            input: Box::new(plan),
            keys,
        };
    }

    if let Some((visible, names)) = hidden_order {
        plan = LogicalPlan::Projection {
            input: Box::new(plan),
            exprs: (0..visible).map(Expr::Column).collect(),
            names,
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
            &[],
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
                arg_exprs: vec![],
                distinct: false,
            }),
            SelectItem::Aggregate(SelectAgg {
                func: AggFunc::Sum,
                args: vec!["id".into()],
                arg_exprs: vec![],
                distinct: false,
            }),
        ];
        let p = plan_select(&items, &tables(), None, None, &[], None, &[], None, None).unwrap();
        // the select list is always projected over the aggregate output
        let inner = match p {
            LogicalPlan::Projection { input, .. } => *input,
            other => panic!("expected projection, got {other:?}"),
        };
        match inner {
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
            kind: infinity_parser::JoinKind::Inner,
        };
        let p = plan_select(
            &[SelectItem::Star],
            &t,
            Some(&j),
            None,
            &[],
            None,
            &[],
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
            &[],
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
            &[],
            None,
            None
        )
        .is_err());
    }
}
