//! `infinity-expression`: scalar expressions and their evaluation over a row.
//! Seeded with literals, column references and comparisons (M2).

#![forbid(unsafe_code)]

pub mod json;

use std::cmp::Ordering;

use infinity_parser::{AggFunc, CmpOp, Literal};

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
    /// A scalar function call; unknown functions evaluate to NULL.
    Func {
        name: String,
        args: Vec<Expr>,
    },
}

impl Expr {
    pub fn eval(&self, row: &[Value]) -> Value {
        match self {
            Expr::Column(i) => row.get(*i).cloned().unwrap_or(Value::Null),
            Expr::Literal(v) => v.clone(),
            Expr::Cmp(a, op, b) => {
                let x = a.eval(row);
                let y = b.eval(row);
                let r = match x.partial_cmp_value(&y) {
                    Some(ord) => match op {
                        CmpOp::Eq => ord == Ordering::Equal,
                        CmpOp::Ne => ord != Ordering::Equal,
                        CmpOp::Lt => ord == Ordering::Less,
                        CmpOp::Le => ord != Ordering::Greater,
                        CmpOp::Gt => ord == Ordering::Greater,
                        CmpOp::Ge => ord != Ordering::Less,
                    },
                    None => false, // NULL comparisons are false
                };
                Value::Bool(r)
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
                // Compare with implicit numeric coercion (`'90.5' IN (90.5)`).
                let found = vals
                    .iter()
                    .any(|v| x.partial_cmp_value(&v.eval(row)) == Some(std::cmp::Ordering::Equal));
                Value::Bool(found != *neg)
            }
            Expr::Func { name, args } => eval_func(name, args, row),
        }
    }

    /// Convenience: evaluate as a boolean predicate (non-bool/NULL => false).
    pub fn eval_predicate(&self, row: &[Value]) -> bool {
        matches!(self.eval(row), Value::Bool(true))
    }
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
            let path = match a1() {
                Value::Str(s) => s,
                _ => return Value::Null,
            };
            let target = args.get(2).map(|e| e.eval(row)).unwrap_or(Value::Null);
            json::contains(&doc, &path, &target)
        }
        _ => Value::Null,
    }
}

/// Computes an aggregate over `rows`. `col = None` means `COUNT(*)`.
/// NULLs are ignored for `SUM/AVG/MIN/MAX` and for `COUNT(col)`.
pub fn aggregate(func: AggFunc, col: Option<usize>, rows: &[Vec<Value>]) -> Value {
    if func == AggFunc::Count && col.is_none() {
        return Value::Int(rows.len() as i64);
    }
    let idx = match col {
        Some(i) => i,
        None => return Value::Int(0),
    };
    let vals: Vec<&Value> = rows
        .iter()
        .filter_map(|r| r.get(idx))
        .filter(|v| **v != Value::Null)
        .collect();

    match func {
        AggFunc::Count => Value::Int(vals.len() as i64),
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
    fn null_comparison_is_false() {
        let e = Expr::Cmp(
            Box::new(Expr::Literal(Value::Null)),
            CmpOp::Eq,
            Box::new(Expr::Literal(Value::Int(1))),
        );
        assert_eq!(e.eval(&[]), Value::Bool(false));
    }

    #[test]
    fn aggregates() {
        use AggFunc::*;
        let rows = vec![
            vec![Value::Int(1), Value::Str("a".into())],
            vec![Value::Int(2), Value::Str("b".into())],
            vec![Value::Int(3), Value::Null],
        ];
        assert_eq!(aggregate(Count, None, &rows), Value::Int(3));
        assert_eq!(aggregate(Count, Some(1), &rows), Value::Int(2));
        assert_eq!(aggregate(Sum, Some(0), &rows), Value::Int(6));
        assert_eq!(aggregate(Avg, Some(0), &rows), Value::Float(2.0));
        assert_eq!(aggregate(Min, Some(0), &rows), Value::Int(1));
        assert_eq!(aggregate(Max, Some(0), &rows), Value::Int(3));
        assert_eq!(aggregate(Max, Some(1), &rows), Value::Str("b".into()));
        assert_eq!(aggregate(First, Some(0), &rows), Value::Int(1));
        assert_eq!(aggregate(Last, Some(0), &rows), Value::Int(3));
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
