//! `infinity-expression`: scalar expressions and their evaluation over a row.
//! Seeded with literals, column references and comparisons (M2).

#![forbid(unsafe_code)]

pub mod json;
pub mod regex;

use std::cmp::Ordering;

use infinity_parser::{AggFunc, BinOp, CmpOp, Literal};

/// A runtime value.
#[derive(Debug, Clone, PartialEq)]
pub enum Value {
    Int(i64),
    Float(f64),
    Str(String),
    Bool(bool),
    Null,
}

impl Value {
    pub fn from_literal(l: &Literal) -> Value {
        match l {
            Literal::Int(i) => Value::Int(*i),
            Literal::Float(f) => Value::Float(*f),
            Literal::Str(s) => Value::Str(s.clone()),
            Literal::Null => Value::Null,
            // Vectors / arrays render as `[a,b,c]` with elements re-joined compactly.
            Literal::Array(items) => Value::Str(format!("[{}]", items.join(","))),
        }
    }

    /// `.slt`-style rendering (REAL fixed 6 decimals, per PoC H).
    pub fn to_slt_string(&self) -> String {
        match self {
            Value::Int(i) => i.to_string(),
            Value::Float(f) => format!("{f:.6}"),
            Value::Str(s) => s.clone(),
            Value::Bool(b) => b.to_string(),
            Value::Null => "null".to_string(),
        }
    }

    fn as_f64(&self) -> Option<f64> {
        match self {
            Value::Int(i) => Some(*i as f64),
            Value::Float(f) => Some(*f),
            _ => None,
        }
    }

    /// Total-ish comparison: numeric vs numeric, string vs string; else `None`.
    pub fn partial_cmp_value(&self, other: &Value) -> Option<Ordering> {
        match (self, other) {
            (Value::Null, _) | (_, Value::Null) => None,
            (Value::Bool(a), Value::Bool(b)) => Some(a.cmp(b)),
            _ => match (self.as_f64(), other.as_f64()) {
                (Some(a), Some(b)) => a.partial_cmp(&b),
                _ => match (self, other) {
                    (Value::Str(a), Value::Str(b)) => Some(a.cmp(b)),
                    // booleans compare against their textual form too
                    (Value::Bool(b), Value::Str(s2)) => Some(b.to_string().cmp(s2)),
                    (Value::Str(s2), Value::Bool(b)) => Some(s2.cmp(&b.to_string())),
                    // implicit numeric coercion: number vs numeric string
                    (Value::Str(a), other2) => match (a.trim().parse::<f64>(), other2.as_f64()) {
                        (Ok(f), Some(g)) => f.partial_cmp(&g),
                        _ => None,
                    },
                    (other2, Value::Str(b)) => match (other2.as_f64(), b.trim().parse::<f64>()) {
                        (Some(f), Ok(g)) => f.partial_cmp(&g),
                        _ => None,
                    },
                    _ => None,
                },
            },
        }
    }
}

/// A scalar expression.
#[derive(Debug, Clone, PartialEq)]
pub enum Expr {
    Column(usize),
    Literal(Value),
    Cmp(Box<Expr>, CmpOp, Box<Expr>),
    And(Box<Expr>, Box<Expr>),
    Or(Box<Expr>, Box<Expr>),
    Not(Box<Expr>),
    /// `expr IS [NOT] NULL`.
    IsNull(Box<Expr>, bool),
    /// `expr LIKE pattern`.
    Like(Box<Expr>, Box<Expr>),
    /// `expr [NOT] IN (…)`.
    In(Box<Expr>, Vec<Expr>, bool),
    /// `a <op> b` arithmetic.
    Binary(Box<Expr>, BinOp, Box<Expr>),
    /// A scalar function call; unknown functions evaluate to NULL.
    Func {
        name: String,
        args: Vec<Expr>,
    },
    /// `filter_text(col, '…')` / `filter_fulltext(col, '…')`: term matching over
    /// the given column's text.
    FullText(usize, String),
    /// `regex(col, '…')`: regular-expression search over a column's text.
    Regex(usize, String),
}

impl Expr {
    pub fn eval(&self, row: &[Value]) -> Value {
        match self {
            Expr::Column(i) => row.get(*i).cloned().unwrap_or(Value::Null),
            Expr::Literal(v) => v.clone(),
            Expr::Cmp(a, op, b) => {
                let x = a.eval(row);
                let y = b.eval(row);
                match x.partial_cmp_value(&y) {
                    Some(ord) => {
                        let r = match op {
                            CmpOp::Eq => ord == Ordering::Equal,
                            CmpOp::Ne => ord != Ordering::Equal,
                            CmpOp::Lt => ord == Ordering::Less,
                            CmpOp::Le => ord != Ordering::Greater,
                            CmpOp::Gt => ord == Ordering::Greater,
                            CmpOp::Ge => ord != Ordering::Less,
                        };
                        Value::Bool(r)
                    }
                    // A NULL operand makes the comparison unknown (falsy in WHERE).
                    None => Value::Null,
                }
            }
            Expr::And(a, b) => Value::Bool(a.eval_predicate(row) && b.eval_predicate(row)),
            Expr::Or(a, b) => Value::Bool(a.eval_predicate(row) || b.eval_predicate(row)),
            Expr::Not(a) => Value::Bool(!a.eval_predicate(row)),
            Expr::IsNull(a, neg) => {
                let is_null = matches!(a.eval(row), Value::Null);
                Value::Bool(is_null != *neg)
            }
            Expr::Like(a, pat) => {
                let text = match a.eval(row) {
                    Value::Str(s) => s,
                    other => other.to_slt_string(),
                };
                let pattern = match pat.eval(row) {
                    Value::Str(s) => s,
                    other => other.to_slt_string(),
                };
                Value::Bool(like_match(&text, &pattern))
            }
            Expr::In(a, vals, neg) => {
                let x = a.eval(row);
                if matches!(x, Value::Null) {
                    return Value::Null; // NULL IN (…) is unknown
                }
                let evaluated: Vec<Value> = vals.iter().map(|v| v.eval(row)).collect();
                if evaluated.iter().any(|v| matches!(v, Value::Null)) {
                    return Value::Null; // a NULL in the list makes the result unknown
                }
                // Compare with implicit numeric coercion (`'90.5' IN (90.5)`).
                let found = evaluated
                    .iter()
                    .any(|v| x.partial_cmp_value(v) == Some(std::cmp::Ordering::Equal));
                Value::Bool(found != *neg)
            }
            Expr::Binary(a, op, b) => {
                let x = a.eval(row);
                let y = b.eval(row);
                eval_binary(&x, *op, &y)
            }
            Expr::Func { name, args } => eval_func(name, args, row),
            Expr::FullText(i, q) => {
                let text = match row.get(*i) {
                    Some(Value::Str(s)) => s.clone(),
                    Some(other) => other.to_slt_string(),
                    None => String::new(),
                };
                Value::Bool(fulltext_match(&text, q))
            }
            Expr::Regex(i, pat) => {
                let text = match row.get(*i) {
                    Some(Value::Str(s)) => s.clone(),
                    Some(other) => other.to_slt_string(),
                    None => String::new(),
                };
                Value::Bool(regex::is_match(&text, pat))
            }
        }
    }

    /// Convenience: evaluate as a boolean predicate (non-bool/NULL => false).
    pub fn eval_predicate(&self, row: &[Value]) -> bool {
        matches!(self.eval(row), Value::Bool(true))
    }
}

/// Evaluates `a <op> b` with numeric promotion; NULL propagates.
fn eval_binary(a: &Value, op: BinOp, b: &Value) -> Value {
    let (x, y) = match (a, b) {
        (Value::Int(x), Value::Int(y)) if matches!(op, BinOp::Add | BinOp::Sub | BinOp::Mul) => {
            let v = match op {
                BinOp::Add => x.wrapping_add(*y),
                BinOp::Sub => x.wrapping_sub(*y),
                _ => x.wrapping_mul(*y),
            };
            return Value::Int(v);
        }
        (Value::Int(x), Value::Int(y)) if op == BinOp::Mod => {
            return if *y == 0 {
                Value::Null
            } else {
                Value::Int(x % y)
            }
        }
        _ => match (a.as_f64(), b.as_f64()) {
            (Some(x), Some(y)) => (x, y),
            _ => return Value::Null,
        },
    };
    Value::Float(match op {
        BinOp::Add => x + y,
        BinOp::Sub => x - y,
        BinOp::Mul => x * y,
        BinOp::Div => {
            if y == 0.0 {
                return Value::Null;
            }
            x / y
        }
        BinOp::Mod => {
            if y == 0.0 {
                return Value::Null;
            }
            x % y
        }
    })
}

/// `CAST(value AS type)`; out-of-range / unparseable numeric casts yield NULL.
fn cast_value(v: &Value, ty: &str) -> Value {
    if matches!(v, Value::Null) {
        return Value::Null;
    }
    let as_int = |v: &Value| -> Option<i64> {
        match v {
            Value::Int(i) => Some(*i),
            Value::Float(f) => Some(*f as i64),
            Value::Bool(b) => Some(i64::from(*b)),
            Value::Str(s) => s.trim().parse::<i64>().ok(),
            Value::Null => None,
        }
    };
    let as_float = |v: &Value| -> Option<f64> {
        match v {
            Value::Int(i) => Some(*i as f64),
            Value::Float(f) => Some(*f),
            Value::Bool(b) => Some(f64::from(u8::from(*b))),
            Value::Str(s) => s.trim().parse::<f64>().ok(),
            Value::Null => None,
        }
    };
    match ty {
        "TINYINT" => as_int(v)
            .and_then(|i| i8::try_from(i).ok())
            .map(|i| Value::Int(i as i64))
            .unwrap_or(Value::Null),
        "SMALLINT" => as_int(v)
            .and_then(|i| i16::try_from(i).ok())
            .map(|i| Value::Int(i as i64))
            .unwrap_or(Value::Null),
        "INT" | "INTEGER" => as_int(v)
            .and_then(|i| i32::try_from(i).ok())
            .map(|i| Value::Int(i as i64))
            .unwrap_or(Value::Null),
        "BIGINT" => as_int(v).map(Value::Int).unwrap_or(Value::Null),
        "FLOAT" | "REAL" => match as_float(v) {
            Some(f) => Value::Float((f as f32) as f64),
            None => Value::Null,
        },
        "DOUBLE" => as_float(v).map(Value::Float).unwrap_or(Value::Null),
        "BOOL" | "BOOLEAN" => match v {
            Value::Bool(b) => Value::Bool(*b),
            Value::Str(s) => match s.trim().to_ascii_lowercase().as_str() {
                "true" | "1" => Value::Bool(true),
                "false" | "0" => Value::Bool(false),
                _ => Value::Null,
            },
            Value::Int(i) => Value::Bool(*i != 0),
            _ => Value::Null,
        },
        // JSON: re-serialise compactly with sorted keys.
        "JSON" => match v {
            Value::Str(s) => match crate::json::parse(s) {
                Some(j) => Value::Str(j.compact()),
                None => Value::Str(s.clone()),
            },
            other => other.clone(),
        },
        // VARCHAR / TEXT / STRING and anything else: render as text.
        _ => Value::Str(v.to_slt_string()),
    }
}

/// Parsed date/time components (`None` when absent from the text form).
#[derive(Default)]
struct DtFields {
    year: Option<i64>,
    month: Option<i64>,
    day: Option<i64>,
    hour: Option<i64>,
    minute: Option<i64>,
    second: Option<i64>,
}

fn dt_fields(s: &str) -> DtFields {
    let mut f = DtFields::default();
    let s = s.trim();
    let (date_part, time_part) = if let Some((a, b)) = s.split_once(' ') {
        (Some(a), Some(b))
    } else if s.contains(':') {
        (None, Some(s))
    } else {
        (Some(s), None)
    };
    if let Some(d) = date_part {
        // a leading '-' denotes a BC year
        let (neg, dd) = match d.strip_prefix('-') {
            Some(rest) => (true, rest),
            None => (false, d),
        };
        let mut it = dd.split('-');
        if let Some(y) = it.next().and_then(|x| x.parse::<i64>().ok()) {
            f.year = Some(if neg { -y } else { y });
        }
        f.month = it.next().and_then(|x| x.parse().ok());
        f.day = it.next().and_then(|x| x.parse().ok());
    }
    if let Some(t) = time_part {
        let mut it = t.split(':');
        f.hour = it.next().and_then(|x| x.parse().ok());
        f.minute = it.next().and_then(|x| x.parse().ok());
        f.second = it
            .next()
            .and_then(|x| x.split('.').next().unwrap_or(x).parse().ok());
    }
    f
}

/// Days since 1970-01-01 (Howard Hinnant's civil algorithm).
fn days_from_civil(y: i64, m: i64, d: i64) -> i64 {
    let y = if m <= 2 { y - 1 } else { y };
    let era = if y >= 0 { y } else { y - 399 } / 400;
    let yoe = y - era * 400;
    let mp = (m + 9) % 12;
    let doy = (153 * mp + 2) / 5 + d - 1;
    let doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    era * 146097 + doe - 719468
}

/// Evaluates a date/time component function.
fn dt_component(s: &str, field: &str) -> Value {
    let f = dt_fields(s);
    let int = |o: Option<i64>| o.map(Value::Int).unwrap_or(Value::Null);
    match field {
        "YEAR" => int(f.year),
        "MONTH" | "MON" => int(f.month),
        "DAY" | "DAYOFMONTH" => int(f.day),
        "HOUR" => int(f.hour.or(Some(0))),
        "MINUTE" => int(f.minute.or(Some(0))),
        "SECOND" => int(f.second.or(Some(0))),
        "QUARTER" => int(f.month.map(|m| (m - 1) / 3 + 1)),
        "ERA" => int(f.year.map(|y| i64::from(y > 0))),
        "CENTURY" => int(f.year.map(|y| {
            if y > 0 {
                (y - 1) / 100 + 1
            } else {
                -((-y) / 100 + 1)
            }
        })),
        "EPOCH" => match (f.year, f.month, f.day) {
            (Some(y), Some(m), Some(d)) => {
                let days = days_from_civil(y, m, d);
                if f.hour.is_some() {
                    Value::Int(
                        days * 86400
                            + f.hour.unwrap_or(0) * 3600
                            + f.minute.unwrap_or(0) * 60
                            + f.second.unwrap_or(0),
                    )
                } else {
                    Value::Int(days)
                }
            }
            _ => Value::Null,
        },
        "WEEKOFYEAR" | "WEEK" => match (f.year, f.month, f.day) {
            (Some(y), Some(m), Some(d)) => Value::Int(iso_week(y, m, d)),
            _ => Value::Null,
        },
        "DAYOFYEAR" => match (f.year, f.month, f.day) {
            (Some(y), Some(m), Some(d)) => {
                Value::Int(days_from_civil(y, m, d) - days_from_civil(y, 1, 1) + 1)
            }
            _ => Value::Null,
        },
        // weekday: 0 = Sunday … 6 = Saturday
        "WEEKDAY" => match (f.year, f.month, f.day) {
            (Some(y), Some(m), Some(d)) => Value::Int((days_from_civil(y, m, d) + 4).rem_euclid(7)),
            _ => Value::Null,
        },
        // day of week: ISO-8601, 1 = Monday … 7 = Sunday
        "DAYOFWEEK" => match (f.year, f.month, f.day) {
            (Some(y), Some(m), Some(d)) => {
                Value::Int((days_from_civil(y, m, d) + 3).rem_euclid(7) + 1)
            }
            _ => Value::Null,
        },
        _ => Value::Null,
    }
}

/// ISO-8601 week number.
fn iso_week(y: i64, m: i64, d: i64) -> i64 {
    let days = days_from_civil(y, m, d);
    // ISO weekdays: Monday = 1 … Sunday = 7 (1970-01-01 was a Thursday = 4).
    let dow = (days.rem_euclid(7)) + 4;
    let dow = if dow > 7 { dow - 7 } else { dow };
    let thursday = days - (dow - 4);
    let (ty, _, _) = civil_from_days(thursday);
    let jan1 = days_from_civil(ty, 1, 1);
    (thursday - jan1) / 7 + 1
}

fn civil_from_days(z: i64) -> (i64, i64, i64) {
    let z = z + 719468;
    let era = if z >= 0 { z } else { z - 146096 } / 146097;
    let doe = z - era * 146097;
    let yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    let y = yoe + era * 400;
    let doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    let mp = (5 * doy + 2) / 153;
    let d = doy - (153 * mp + 2) / 5 + 1;
    let m = if mp < 10 { mp + 3 } else { mp - 9 };
    (if m <= 2 { y + 1 } else { y }, m, d)
}

/// Tokenises text into lowercase word tokens.
fn tokens(text: &str) -> Vec<String> {
    text.split(|c: char| !c.is_alphanumeric() && c != '_')
        .filter(|t| !t.is_empty())
        .map(|t| t.to_lowercase())
        .collect()
}

/// `filter_text` semantics: whitespace-separated terms are AND-ed; `OR`
/// separates alternatives; every term of a group must appear.
fn fulltext_match(text: &str, query: &str) -> bool {
    let doc = tokens(text);
    let mut groups: Vec<Vec<String>> = vec![Vec::new()];
    for w in query.split_whitespace() {
        if w.eq_ignore_ascii_case("or") {
            groups.push(Vec::new());
        } else {
            groups.last_mut().unwrap().extend(tokens(w));
        }
    }
    groups
        .iter()
        .filter(|g| !g.is_empty())
        .any(|g| g.iter().all(|t| doc.iter().any(|d| d == t)))
}

/// SQL `LIKE`: `%` matches any run of characters, `_` a single character.
fn like_match(text: &str, pattern: &str) -> bool {
    let t: Vec<char> = text.chars().collect();
    let p: Vec<char> = pattern.chars().collect();
    // dp[i][j] = p[..j] matches t[..i]
    let mut dp = vec![vec![false; p.len() + 1]; t.len() + 1];
    dp[0][0] = true;
    for j in 1..=p.len() {
        if p[j - 1] == '%' {
            dp[0][j] = dp[0][j - 1];
        }
    }
    for i in 1..=t.len() {
        for j in 1..=p.len() {
            dp[i][j] = match p[j - 1] {
                '%' => dp[i - 1][j] || dp[i][j - 1],
                '_' => dp[i - 1][j - 1],
                c => dp[i - 1][j - 1] && t[i - 1] == c,
            };
        }
    }
    dp[t.len()][p.len()]
}

/// Evaluates a scalar function. Only a small, deterministic set is implemented;
/// anything unknown yields NULL (so parsing breadth never breaks execution).
fn eval_func(name: &str, args: &[Expr], row: &[Value]) -> Value {
    let a0 = || args.first().map(|e| e.eval(row)).unwrap_or(Value::Null);
    let a1 = || args.get(1).map(|e| e.eval(row)).unwrap_or(Value::Null);
    match name.to_ascii_uppercase().as_str() {
        "SQRT" => a0()
            .as_f64()
            .map(|v| Value::Float(v.sqrt()))
            .unwrap_or(Value::Null),
        "ABS" => match a0() {
            Value::Int(i) => Value::Int(i.abs()),
            Value::Float(f) => Value::Float(f.abs()),
            _ => Value::Null,
        },
        "CEIL" | "CEILING" => a0()
            .as_f64()
            .map(|v| Value::Float(v.ceil()))
            .unwrap_or(Value::Null),
        "FLOOR" => a0()
            .as_f64()
            .map(|v| Value::Float(v.floor()))
            .unwrap_or(Value::Null),
        "ROUND" => {
            let x = a0();
            let n = match a1() {
                Value::Int(i) => i,
                Value::Float(f) => f as i64,
                _ => 0,
            };
            match x {
                Value::Float(f) => {
                    let m = 10f64.powi(n as i32);
                    Value::Float((f * m).round() / m)
                }
                Value::Int(i) => Value::Int(i),
                _ => Value::Null,
            }
        }
        "UPPER" => match a0() {
            Value::Str(s) => Value::Str(s.to_uppercase()),
            _ => Value::Null,
        },
        "LOWER" => match a0() {
            Value::Str(s) => Value::Str(s.to_lowercase()),
            _ => Value::Null,
        },
        "LENGTH" | "CHAR_LENGTH" | "LEN" => match a0() {
            Value::Str(s) => Value::Int(s.chars().count() as i64),
            _ => Value::Null,
        },
        "REVERSE" => match a0() {
            Value::Str(s) => Value::Str(s.chars().rev().collect()),
            _ => Value::Null,
        },
        "TRIM" => match a0() {
            Value::Str(s) => Value::Str(s.trim().to_string()),
            _ => Value::Null,
        },
        "LTRIM" => match a0() {
            Value::Str(s) => Value::Str(s.trim_start().to_string()),
            _ => Value::Null,
        },
        "RTRIM" => match a0() {
            Value::Str(s) => Value::Str(s.trim_end().to_string()),
            _ => Value::Null,
        },
        "SUBSTRING" | "SUBSTR" => match a0() {
            Value::Str(s) => {
                let chars: Vec<char> = s.chars().collect();
                let start = match a1() {
                    Value::Int(i) if i >= 0 => i as usize,
                    Value::Int(i) => chars.len().saturating_sub((-i) as usize),
                    _ => 0,
                };
                let len = match args.get(2).map(|e| e.eval(row)) {
                    Some(Value::Int(i)) if i >= 0 => Some(i as usize),
                    _ => None,
                };
                let end = len
                    .map(|l| (start + l).min(chars.len()))
                    .unwrap_or(chars.len());
                let start = start.min(chars.len());
                Value::Str(chars[start..end].iter().collect())
            }
            _ => Value::Null,
        },
        "JSON_EXTRACT"
        | "JSON_EXTRACT_STRING"
        | "JSON_EXTRACT_INT"
        | "JSON_EXTRACT_DOUBLE"
        | "JSON_EXTRACT_FLOAT"
        | "JSON_EXTRACT_BOOL"
        | "JSON_EXTRACT_ISNULL"
        | "JSON_EXISTS_PATH" => {
            let doc = match a0() {
                Value::Str(s) => s,
                _ => return Value::Null,
            };
            let path = match a1() {
                Value::Str(s) => s,
                _ => return Value::Null,
            };
            json::extract_typed(&doc, &path, &name.to_ascii_lowercase())
        }
        "JSON_CONTAINS" => {
            let doc = match a0() {
                Value::Str(s) => s,
                _ => return Value::Null,
            };
            if args.len() == 2 {
                return json::contains_value(&doc, &a1());
            }
            let path = match a1() {
                Value::Str(s) => s,
                _ => return Value::Null,
            };
            let target = args.get(2).map(|e| e.eval(row)).unwrap_or(Value::Null);
            json::contains(&doc, &path, &target)
        }
        "IS_NULL" => Value::Bool(matches!(a0(), Value::Null)),
        "IS_NOT_NULL" => Value::Bool(!matches!(a0(), Value::Null)),
        "YEAR" | "MONTH" | "DAY" | "HOUR" | "MINUTE" | "SECOND" | "QUARTER" | "ERA" | "CENTURY"
        | "EPOCH" | "WEEKOFYEAR" | "WEEK" | "DAYOFMONTH" | "DAYOFYEAR" | "WEEKDAY"
        | "DAYOFWEEK" => {
            let s = match a0() {
                Value::Str(s) => s,
                _ => return Value::Null,
            };
            dt_component(&s, &name.to_ascii_uppercase())
        }
        "DATEPART" | "DATE_PART" => {
            let field = match a0() {
                Value::Str(s) => s,
                _ => return Value::Null,
            };
            let val = match a1() {
                Value::Str(s) => s,
                _ => return Value::Null,
            };
            dt_component(&val, &field.to_ascii_uppercase())
        }
        "PARSE_JSON" => match a0() {
            Value::Str(s) => match json::parse(&s) {
                Some(v) => Value::Str(v.compact()),
                None => Value::Null,
            },
            _ => Value::Null,
        },
        "CAST" => {
            let ty = match a1() {
                Value::Str(s) => s.to_ascii_uppercase(),
                _ => return Value::Null,
            };
            cast_value(&a0(), &ty)
        }
        _ => Value::Null,
    }
}

/// Computes an aggregate over `rows`. Empty `cols` means `COUNT(*)`.
/// `distinct` deduplicates the gathered value tuples first.
/// NULLs are ignored for `SUM/AVG/MIN/MAX` and for `COUNT(col)`.
pub fn aggregate(func: AggFunc, cols: &[usize], distinct: bool, rows: &[Vec<Value>]) -> Value {
    if func == AggFunc::Count && cols.is_empty() {
        return Value::Int(rows.len() as i64);
    }
    if cols.is_empty() {
        return Value::Int(0);
    }
    let mut tuples: Vec<Vec<Value>> = Vec::new();
    for r in rows {
        let mut t = Vec::with_capacity(cols.len());
        let mut ok = true;
        for &c in cols {
            match r.get(c) {
                Some(v) => t.push(v.clone()),
                None => {
                    ok = false;
                    break;
                }
            }
        }
        if ok {
            tuples.push(t);
        }
    }
    if distinct {
        // Values may hold floats (not `Hash`), so dedupe on their text form.
        let mut seen = std::collections::HashSet::new();
        tuples.retain(|t| {
            let key: Vec<String> = t.iter().map(|v| v.to_slt_string()).collect();
            seen.insert(key)
        });
    }
    let vals: Vec<&Value> = if cols.len() == 1 {
        tuples
            .iter()
            .map(|t| &t[0])
            .filter(|v| **v != Value::Null)
            .collect()
    } else {
        tuples.iter().map(|t| &t[0]).collect()
    };

    match func {
        AggFunc::Count => {
            // COUNT(col) ignores NULLs; COUNT(DISTINCT a, b) counts tuples.
            let n = if cols.len() == 1 {
                vals.len()
            } else {
                tuples.len()
            };
            Value::Int(n as i64)
        }
        AggFunc::Sum | AggFunc::Avg => {
            let mut sum = 0f64;
            let mut all_int = true;
            for v in &vals {
                match v {
                    Value::Int(i) => sum += *i as f64,
                    Value::Float(f) => {
                        sum += *f;
                        all_int = false;
                    }
                    _ => {}
                }
            }
            if vals.is_empty() {
                return if func == AggFunc::Avg {
                    Value::Null
                } else {
                    Value::Int(0)
                };
            }
            match func {
                AggFunc::Sum if all_int => Value::Int(sum as i64),
                _ => Value::Float(
                    sum / if func == AggFunc::Avg {
                        vals.len() as f64
                    } else {
                        1.0
                    },
                ),
            }
        }
        AggFunc::Min | AggFunc::Max | AggFunc::First | AggFunc::Last => {
            if matches!(func, AggFunc::First) {
                return vals.first().map(|v| (*v).clone()).unwrap_or(Value::Null);
            }
            if matches!(func, AggFunc::Last) {
                return vals.last().map(|v| (*v).clone()).unwrap_or(Value::Null);
            }
            let mut best: Option<&Value> = None;
            for v in vals {
                best = Some(match best {
                    None => v,
                    Some(b) => match v.partial_cmp_value(b) {
                        Some(std::cmp::Ordering::Less) if func == AggFunc::Min => v,
                        Some(std::cmp::Ordering::Greater) if func == AggFunc::Max => v,
                        _ => b,
                    },
                });
            }
            best.cloned().unwrap_or(Value::Null)
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn row() -> Vec<Value> {
        vec![Value::Int(1), Value::Str("alice".into()), Value::Float(2.5)]
    }

    #[test]
    fn column_and_literal() {
        assert_eq!(Expr::Column(1).eval(&row()), Value::Str("alice".into()));
        assert_eq!(Expr::Literal(Value::Int(9)).eval(&row()), Value::Int(9));
    }

    #[test]
    fn comparisons() {
        let r = row();
        let gt = Expr::Cmp(
            Box::new(Expr::Column(0)),
            CmpOp::Gt,
            Box::new(Expr::Literal(Value::Int(0))),
        );
        assert_eq!(gt.eval(&r), Value::Bool(true));
        let le = Expr::Cmp(
            Box::new(Expr::Column(2)),
            CmpOp::Le,
            Box::new(Expr::Literal(Value::Float(2.5))),
        );
        assert_eq!(le.eval(&r), Value::Bool(true));
        // mixed int/float compare
        let eq = Expr::Cmp(
            Box::new(Expr::Column(0)),
            CmpOp::Eq,
            Box::new(Expr::Literal(Value::Float(1.0))),
        );
        assert_eq!(eq.eval(&r), Value::Bool(true));
        // string
        let se = Expr::Cmp(
            Box::new(Expr::Column(1)),
            CmpOp::Eq,
            Box::new(Expr::Literal(Value::Str("alice".into()))),
        );
        assert!(se.eval_predicate(&r));
    }

    #[test]
    fn null_comparison_is_unknown() {
        let e = Expr::Cmp(
            Box::new(Expr::Literal(Value::Null)),
            CmpOp::Eq,
            Box::new(Expr::Literal(Value::Int(1))),
        );
        // three-valued logic: the comparison is unknown, hence falsy in a predicate
        assert_eq!(e.eval(&[]), Value::Null);
        assert!(!e.eval_predicate(&[]));
    }

    #[test]
    fn aggregates() {
        use AggFunc::*;
        let rows = vec![
            vec![Value::Int(1), Value::Str("a".into())],
            vec![Value::Int(2), Value::Str("b".into())],
            vec![Value::Int(3), Value::Null],
        ];
        assert_eq!(aggregate(Count, &[], false, &rows), Value::Int(3));
        assert_eq!(aggregate(Count, &[1], false, &rows), Value::Int(2));
        assert_eq!(aggregate(Sum, &[0], false, &rows), Value::Int(6));
        assert_eq!(aggregate(Avg, &[0], false, &rows), Value::Float(2.0));
        assert_eq!(aggregate(Min, &[0], false, &rows), Value::Int(1));
        assert_eq!(aggregate(Max, &[0], false, &rows), Value::Int(3));
        assert_eq!(aggregate(Max, &[1], false, &rows), Value::Str("b".into()));
        assert_eq!(aggregate(First, &[0], false, &rows), Value::Int(1));
        assert_eq!(aggregate(Last, &[0], false, &rows), Value::Int(3));
        // DISTINCT over a multi-column tuple
        let dup = vec![
            vec![Value::Int(1), Value::Int(9)],
            vec![Value::Int(1), Value::Int(9)],
            vec![Value::Int(2), Value::Int(9)],
        ];
        assert_eq!(aggregate(Count, &[0, 1], true, &dup), Value::Int(2));
        assert_eq!(aggregate(Count, &[0, 1], false, &dup), Value::Int(3));
    }

    #[test]
    fn scalar_functions() {
        let row = vec![Value::Float(2.25), Value::Str("Ab".into())];
        let f = |name: &str, args: Vec<Expr>| Expr::Func {
            name: name.into(),
            args,
        };
        assert_eq!(
            f("ROUND", vec![Expr::Column(0), Expr::Literal(Value::Int(1))]).eval(&row),
            Value::Float(2.3)
        );
        assert_eq!(
            f("SQRT", vec![Expr::Literal(Value::Float(9.0))]).eval(&row),
            Value::Float(3.0)
        );
        assert_eq!(
            f("UPPER", vec![Expr::Column(1)]).eval(&row),
            Value::Str("AB".into())
        );
        assert_eq!(f("LENGTH", vec![Expr::Column(1)]).eval(&row), Value::Int(2));
        assert_eq!(f("NOPE", vec![Expr::Column(0)]).eval(&row), Value::Null);
    }

    #[test]
    fn json_extract_functions() {
        use crate::json::extract_typed;
        let doc = r#"{"year":2020,"score":1,"f":1.5,"name":"a","tags":["x","y"],"n":null}"#;
        assert_eq!(
            extract_typed(doc, "$.year", "json_extract_int"),
            Value::Int(2020)
        );
        assert_eq!(
            extract_typed(doc, "$.score", "json_extract_double"),
            Value::Float(1.0)
        );
        // a JSON float is not an int
        assert_eq!(extract_typed(doc, "$.f", "json_extract_int"), Value::Null);
        assert_eq!(
            extract_typed(doc, "$.f", "json_extract_double"),
            Value::Float(1.5)
        );
        assert_eq!(
            extract_typed(doc, "$.name", "json_extract_string"),
            Value::Str("a".into())
        );
        assert_eq!(
            extract_typed(doc, "$.tags[1]", "json_extract"),
            Value::Str("\"y\"".into())
        );
        assert_eq!(
            extract_typed(doc, "$.n", "json_extract_isnull"),
            Value::Bool(true)
        );
        assert_eq!(
            extract_typed(doc, "$.missing", "json_exists_path"),
            Value::Bool(false)
        );
        assert_eq!(
            extract_typed(doc, "$.tags", "json_extract"),
            Value::Str("[\"x\",\"y\"]".into())
        );
    }
}
