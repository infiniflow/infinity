//! Index-scan filter push-down and its evaluators.
//!
//! This ports the `index_scan` cluster:
//!
//! * `FilterExpressionPushDownHelper` — constant folding, cast unwinding and
//!   JSON term encoding;
//! * `ExpressionInfoTree` + `ExpressionIndexScanInfo` — classifies a predicate
//!   into index-usable pieces;
//! * `IndexFilterEvaluator` and its AllTrue / AllFalse / Secondary / Fulltext /
//!   LogicalChildren variants — evaluate the index filter into a row bitmask;
//! * `FilterExpressionPushDown` — splits a predicate into index filter and
//!   leftover filter and builds the evaluator;
//! * `IndexScanBuilder` — turns `Filter(Scan)` into an index-scan candidate;
//! * `ApplyFastRoughFilter` — builds the block/segment skip evaluator.
//!
//! The C++ evaluators read a secondary/full-text index through the catalog.
//! The planner owns no catalog handle, so callers supply an [`IndexCatalog`]
//! (which columns are indexed) and an [`IndexEvalContext`] holding the actual
//! index readers at evaluation time.

use std::cmp::Ordering;
use std::collections::{BTreeMap, BTreeSet, HashMap};

use infinity_common::{Error, Result};
use infinity_expression::{Expr, Value};
use infinity_parser::CmpOp;

use crate::regex_rewrite::{RegexIndexCatalog, RegexIndexRewrite};
use crate::LogicalPlan;

// ---------------------------------------------------------------------------
// Catalog description
// ---------------------------------------------------------------------------

/// The logical type of an indexed column (`LogicalType` subset used here).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ColumnKind {
    Boolean,
    TinyInt,
    SmallInt,
    Integer,
    BigInt,
    Float,
    Double,
    Date,
    Time,
    DateTime,
    Timestamp,
    Varchar,
    Json,
}

impl ColumnKind {
    fn is_float(self) -> bool {
        matches!(self, ColumnKind::Float | ColumnKind::Double)
    }

    fn is_varchar(self) -> bool {
        self == ColumnKind::Varchar
    }

    fn bounds(self) -> (f64, f64) {
        match self {
            ColumnKind::TinyInt => (i8::MIN as f64, i8::MAX as f64),
            ColumnKind::SmallInt => (i16::MIN as f64, i16::MAX as f64),
            ColumnKind::Integer => (i32::MIN as f64, i32::MAX as f64),
            ColumnKind::BigInt => (i64::MIN as f64, i64::MAX as f64),
            _ => (f64::MIN, f64::MAX),
        }
    }
}

/// A column with a secondary index (`new_candidate_column_index_map_`).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct SecondaryIndexInfo {
    pub column_id: usize,
    pub kind: ColumnKind,
    /// A JSON secondary index (`IndexSecondary::IsJsonIndex`).
    pub json: bool,
}

/// The catalog facts push-down needs: column names/types and which columns or
/// functional expressions carry a secondary index.
#[derive(Debug, Clone, Default)]
pub struct IndexCatalog {
    pub column_names: Vec<String>,
    pub column_kinds: Vec<ColumnKind>,
    /// Keyed by column name (C++ looks the column id up by name so that a
    /// `DROP COLUMN` shift cannot mis-bind an index).
    pub secondary: HashMap<String, SecondaryIndexInfo>,
    /// `ExtractFunctionInfo()` spelling → indexed column name.
    pub functional: HashMap<String, String>,
}

impl IndexCatalog {
    pub fn new(columns: &[(&str, ColumnKind)]) -> Self {
        Self {
            column_names: columns.iter().map(|(n, _)| n.to_string()).collect(),
            column_kinds: columns.iter().map(|(_, k)| *k).collect(),
            ..Default::default()
        }
    }

    pub fn with_secondary(mut self, column: &str, json: bool) -> Self {
        if let Some(id) = self.column_id(column) {
            let kind = self.column_kinds[id];
            self.secondary.insert(
                column.to_string(),
                SecondaryIndexInfo {
                    column_id: id,
                    kind,
                    json,
                },
            );
        }
        self
    }

    pub fn with_functional(mut self, key: &str, column: &str) -> Self {
        self.functional.insert(key.to_string(), column.to_string());
        self
    }

    pub fn column_id(&self, name: &str) -> Option<usize> {
        self.column_names.iter().position(|n| n == name)
    }

    pub fn kind(&self, column_id: usize) -> Option<ColumnKind> {
        self.column_kinds.get(column_id).copied()
    }

    pub fn secondary_for(&self, column_id: usize) -> Option<&SecondaryIndexInfo> {
        self.secondary
            .values()
            .find(|info| info.column_id == column_id)
    }
}

/// The `ExtractFunctionInfo` spelling used as the functional-index key.
pub fn function_key(expr: &Expr) -> Option<String> {
    let Expr::Func { name, args } = expr else {
        return None;
    };
    Some(format!(
        "{}({})",
        name.to_ascii_lowercase(),
        args.iter()
            .map(|a| match a {
                Expr::Column(i) => format!("c{i}"),
                Expr::Literal(v) => v.to_slt_string(),
                other => format!("{other:?}"),
            })
            .collect::<Vec<_>>()
            .join(",")
    ))
}

// ---------------------------------------------------------------------------
// FilterExpressionPushDownHelper
// ---------------------------------------------------------------------------

/// C++ `FilterCompareType`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FilterCompareType {
    Equal,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    AlwaysFalse,
    AlwaysTrue,
    Invalid,
}

/// C++ `UnwindCast` result.
#[derive(Debug, Clone, PartialEq)]
pub struct UnwindCast {
    pub column_id: usize,
    pub value: Value,
    pub functional: Option<Expr>,
    pub compare_type: FilterCompareType,
}

fn expr_has_column(expr: &Expr) -> bool {
    match expr {
        Expr::Column(_) | Expr::FullText(..) => true,
        Expr::Literal(_) => false,
        Expr::Cmp(a, _, b) | Expr::And(a, b) | Expr::Or(a, b) | Expr::Binary(a, _, b) => {
            expr_has_column(a) || expr_has_column(b)
        }
        Expr::Not(a) | Expr::IsNull(a, _) => expr_has_column(a),
        Expr::Like(a, b, _, _) => expr_has_column(a) || expr_has_column(b),
        Expr::In(a, vals, _) => expr_has_column(a) || vals.iter().any(expr_has_column),
        Expr::Func { args, .. } => args.iter().any(expr_has_column),
        Expr::Regex(i, _) => {
            let _ = i;
            true
        }
    }
}

fn find_column(expr: &Expr) -> Option<usize> {
    match expr {
        Expr::Column(i) => Some(*i),
        Expr::Cmp(a, _, b) | Expr::Binary(a, _, b) | Expr::And(a, b) | Expr::Or(a, b) => {
            find_column(a).or_else(|| find_column(b))
        }
        Expr::Not(a) | Expr::IsNull(a, _) => find_column(a),
        Expr::Func { args, .. } => args.iter().find_map(find_column),
        _ => None,
    }
}

fn cast_target(expr: &Expr) -> Option<&str> {
    let Expr::Func { name, args } = expr else {
        return None;
    };
    if !name.eq_ignore_ascii_case("cast") || args.len() != 2 {
        return None;
    }
    match &args[1] {
        Expr::Literal(Value::Str(t)) => Some(t.as_str()),
        _ => None,
    }
}

fn kind_from_type_name(name: &str) -> Option<ColumnKind> {
    Some(match name.to_ascii_lowercase().as_str() {
        "boolean" | "bool" => ColumnKind::Boolean,
        "tinyint" | "int8" => ColumnKind::TinyInt,
        "smallint" | "int16" => ColumnKind::SmallInt,
        "integer" | "int" | "int32" => ColumnKind::Integer,
        "bigint" | "int64" => ColumnKind::BigInt,
        "float" | "float32" => ColumnKind::Float,
        "double" | "float64" => ColumnKind::Double,
        "date" => ColumnKind::Date,
        "time" => ColumnKind::Time,
        "datetime" => ColumnKind::DateTime,
        "timestamp" => ColumnKind::Timestamp,
        "varchar" | "text" | "string" => ColumnKind::Varchar,
        "json" => ColumnKind::Json,
        _ => return None,
    })
}

fn next_up(v: f64) -> f64 {
    v.next_up()
}

fn next_down(v: f64) -> f64 {
    v.next_down()
}

/// C++ `FilterExpressionPushDownHelper`.
pub struct FilterExpressionPushDownHelper;

impl FilterExpressionPushDownHelper {
    /// C++ `CalcValueResult`: evaluates a column-free expression.
    pub fn calc_value_result(expr: &Expr) -> Result<Value> {
        if expr_has_column(expr) {
            return Err(Error::Invalid(
                "filter push-down requires a constant comparison operand".into(),
            ));
        }
        Ok(expr.eval(&[]))
    }

    /// C++ `EncodeJsonInteger`.
    pub fn encode_json_integer(value: i64) -> String {
        let sortable = (value as u64) ^ (1u64 << 63);
        format!("{sortable:016X}")
    }

    /// C++ `EncodeJsonDouble`.
    pub fn encode_json_double(value: f64) -> String {
        let value = if value == 0.0 { 0.0 } else { value };
        let bits = value.to_bits();
        let sortable = if bits & (1u64 << 63) != 0 {
            !bits
        } else {
            bits ^ (1u64 << 63)
        };
        format!("{sortable:016X}")
    }

    /// C++ `SimplifyCompareTypeAndValue` (`<` → `<= prev`, `>` → `>= next`).
    pub fn simplify_compare_type_and_value(
        value: &mut Value,
        compare_type: &mut FilterCompareType,
    ) {
        match *compare_type {
            FilterCompareType::Less => rewrite_compare(value, compare_type, false),
            FilterCompareType::Greater => rewrite_compare(value, compare_type, true),
            _ => {}
        }
    }

    /// C++ `UnwindCast`.
    pub fn unwind_cast(
        expr: &Expr,
        right: Value,
        compare_type: FilterCompareType,
        catalog: &IndexCatalog,
    ) -> Result<UnwindCast> {
        let mut value = right;
        let mut compare_type = compare_type;
        Self::simplify_compare_type_and_value(&mut value, &mut compare_type);
        if matches!(
            compare_type,
            FilterCompareType::AlwaysFalse | FilterCompareType::AlwaysTrue
        ) {
            return Ok(UnwindCast {
                column_id: 0,
                value: Value::Null,
                functional: None,
                compare_type,
            });
        }
        match expr {
            Expr::Column(i) => Ok(UnwindCast {
                column_id: *i,
                value,
                functional: None,
                compare_type,
            }),
            Expr::Func { .. } if cast_target(expr).is_some() => {
                let Expr::Func { args, .. } = expr else {
                    unreachable!()
                };
                let source = &args[0];
                let target = kind_from_type_name(cast_target(expr).unwrap()).ok_or_else(|| {
                    Error::Invalid("unsupported cast target in filter push-down".into())
                })?;
                let source_kind = match source {
                    Expr::Column(i) => catalog.kind(*i),
                    _ => None,
                };
                if source_kind == Some(target) {
                    return Self::unwind_cast(source, value, compare_type, catalog);
                }
                match (source_kind, target) {
                    (Some(kind), ColumnKind::BigInt) => {
                        let raw = match value {
                            Value::Int(i) => i,
                            _ => {
                                return Ok(UnwindCast {
                                    column_id: 0,
                                    value: Value::Null,
                                    functional: None,
                                    compare_type: FilterCompareType::Invalid,
                                })
                            }
                        };
                        let (ok, next) = integral_continue_bigint(raw, kind, compare_type);
                        if !ok {
                            return Ok(UnwindCast {
                                column_id: 0,
                                value: Value::Null,
                                functional: None,
                                compare_type: next,
                            });
                        }
                        Self::unwind_cast(source, Value::Int(raw), next, catalog)
                    }
                    (Some(kind), ColumnKind::Double) => {
                        let raw = match value {
                            Value::Float(f) => f,
                            Value::Int(i) => i as f64,
                            _ => {
                                return Ok(UnwindCast {
                                    column_id: 0,
                                    value: Value::Null,
                                    functional: None,
                                    compare_type: FilterCompareType::Invalid,
                                })
                            }
                        };
                        if kind == ColumnKind::Float {
                            return Self::unwind_cast(
                                source,
                                Value::Float(raw as f32 as f64),
                                compare_type,
                                catalog,
                            );
                        }
                        match integral_continue_double(raw, kind, compare_type) {
                            DoubleNarrow::Value(next_value) => Self::unwind_cast(
                                source,
                                Value::Int(next_value),
                                compare_type,
                                catalog,
                            ),
                            DoubleNarrow::AlwaysTrue => Ok(UnwindCast {
                                column_id: 0,
                                value: Value::Null,
                                functional: None,
                                compare_type: FilterCompareType::AlwaysTrue,
                            }),
                            DoubleNarrow::AlwaysFalse => Ok(UnwindCast {
                                column_id: 0,
                                value: Value::Null,
                                functional: None,
                                compare_type: FilterCompareType::AlwaysFalse,
                            }),
                        }
                    }
                    _ => Ok(UnwindCast {
                        column_id: 0,
                        value: Value::Null,
                        functional: None,
                        compare_type: FilterCompareType::Invalid,
                    }),
                }
            }
            Expr::Func { .. } => {
                let column_id = find_column(expr).ok_or_else(|| {
                    Error::Invalid("functional index expression has no column argument".into())
                })?;
                Ok(UnwindCast {
                    column_id,
                    value,
                    functional: Some(expr.clone()),
                    compare_type,
                })
            }
            _ => Err(Error::Invalid(
                "UnwindCast(): unsupported expression type".into(),
            )),
        }
    }

    /// C++ `BuildJsonTerm`.
    pub fn build_json_term(
        func_name: &str,
        path: &str,
        value: &Value,
        compare_type: &mut FilterCompareType,
    ) -> String {
        if func_name == "json_contains" {
            let term_value = value.to_slt_string();
            return path.to_string() + ":s:" + &term_value;
        }
        let type_tag = match func_name {
            "json_extract_int" | "json_extract_isnull" => ":i:",
            "json_extract_double" => ":d:",
            "json_extract_bool" => ":b:",
            "json_extract_string" => ":s:",
            "json_exists_path" => ":p:",
            _ => return String::new(),
        };
        match *compare_type {
            FilterCompareType::Equal => match func_name {
                "json_extract_int" => match value {
                    Value::Int(i) => format!("{path}{type_tag}{}", Self::encode_json_integer(*i)),
                    _ => String::new(),
                },
                "json_extract_double" => match value {
                    Value::Float(f) => format!("{path}{type_tag}{}", Self::encode_json_double(*f)),
                    _ => String::new(),
                },
                "json_extract_string" => format!("{path}{type_tag}{}", value.to_slt_string()),
                "json_extract_bool" => match value {
                    Value::Bool(b) => format!("{path}{type_tag}{}", u8::from(*b)),
                    _ => String::new(),
                },
                "json_exists_path" => format!("{path}{type_tag}"),
                "json_extract_isnull" => format!("{path}:n:"),
                _ => String::new(),
            },
            FilterCompareType::Greater => {
                *compare_type = FilterCompareType::GreaterEqual;
                match func_name {
                    "json_extract_int" => match value {
                        Value::Int(i) if *i == i64::MAX => {
                            *compare_type = FilterCompareType::AlwaysFalse;
                            String::new()
                        }
                        Value::Int(i) => {
                            format!("{path}{type_tag}{}", Self::encode_json_integer(i + 1))
                        }
                        _ => String::new(),
                    },
                    "json_extract_double" => match value {
                        Value::Float(f) => {
                            format!("{path}{type_tag}{}", Self::encode_json_double(next_up(*f)))
                        }
                        _ => String::new(),
                    },
                    "json_extract_string" => {
                        format!("{path}{type_tag}{}\u{1}", value.to_slt_string())
                    }
                    _ => String::new(),
                }
            }
            FilterCompareType::GreaterEqual => match func_name {
                "json_extract_int" => match value {
                    Value::Int(i) => format!("{path}{type_tag}{}", Self::encode_json_integer(*i)),
                    _ => String::new(),
                },
                "json_extract_double" => match value {
                    Value::Float(f) => format!("{path}{type_tag}{}", Self::encode_json_double(*f)),
                    _ => String::new(),
                },
                "json_extract_string" => format!("{path}{type_tag}{}", value.to_slt_string()),
                _ => String::new(),
            },
            FilterCompareType::Less => {
                *compare_type = FilterCompareType::LessEqual;
                match func_name {
                    "json_extract_int" => match value {
                        Value::Int(i) if *i == i64::MIN => {
                            *compare_type = FilterCompareType::AlwaysFalse;
                            String::new()
                        }
                        Value::Int(i) => {
                            format!("{path}{type_tag}{}", Self::encode_json_integer(i - 1))
                        }
                        _ => String::new(),
                    },
                    "json_extract_double" => match value {
                        Value::Float(f) => {
                            format!(
                                "{path}{type_tag}{}",
                                Self::encode_json_double(next_down(*f))
                            )
                        }
                        _ => String::new(),
                    },
                    "json_extract_string" => format!("{path}{type_tag}{}", value.to_slt_string()),
                    _ => String::new(),
                }
            }
            FilterCompareType::LessEqual => match func_name {
                "json_extract_int" => match value {
                    Value::Int(i) => format!("{path}{type_tag}{}", Self::encode_json_integer(*i)),
                    _ => String::new(),
                },
                "json_extract_double" => match value {
                    Value::Float(f) => format!("{path}{type_tag}{}", Self::encode_json_double(*f)),
                    _ => String::new(),
                },
                "json_extract_string" => format!("{path}{type_tag}{}", value.to_slt_string()),
                _ => String::new(),
            },
            _ => String::new(),
        }
    }
}

fn rewrite_compare(value: &mut Value, compare_type: &mut FilterCompareType, greater: bool) {
    match (&*value, greater) {
        (Value::Int(i), false) => {
            if *i == i64::MIN {
                *compare_type = FilterCompareType::AlwaysFalse;
            } else {
                *value = Value::Int(*i - 1);
                *compare_type = FilterCompareType::LessEqual;
            }
        }
        (Value::Int(i), true) => {
            if *i == i64::MAX {
                *compare_type = FilterCompareType::AlwaysFalse;
            } else {
                *value = Value::Int(*i + 1);
                *compare_type = FilterCompareType::GreaterEqual;
            }
        }
        (Value::Float(f), false) => {
            if *f <= f64::MIN {
                *compare_type = FilterCompareType::AlwaysFalse;
            } else {
                *value = Value::Float(next_down(*f));
                *compare_type = FilterCompareType::LessEqual;
            }
        }
        (Value::Float(f), true) => {
            if *f >= f64::MAX {
                *compare_type = FilterCompareType::AlwaysFalse;
            } else {
                *value = Value::Float(next_up(*f));
                *compare_type = FilterCompareType::GreaterEqual;
            }
        }
        (Value::Str(s), false) => {
            if s.is_empty() {
                *compare_type = FilterCompareType::AlwaysFalse;
            } else {
                let mut bytes = s.as_bytes().to_vec();
                let last = bytes.len() - 1;
                if bytes[last] == 0 {
                    bytes.pop();
                } else {
                    bytes[last] -= 1;
                    bytes.extend(std::iter::repeat_n(0x7f, 16));
                }
                *value = Value::Str(String::from_utf8_lossy(&bytes).into_owned());
                *compare_type = FilterCompareType::LessEqual;
            }
        }
        (Value::Str(s), true) => {
            *value = Value::Str(format!("{s}\0"));
            *compare_type = FilterCompareType::GreaterEqual;
        }
        _ => {
            *compare_type = FilterCompareType::Invalid;
        }
    }
}

fn integral_continue_bigint(
    right: i64,
    kind: ColumnKind,
    compare_type: FilterCompareType,
) -> (bool, FilterCompareType) {
    let (lo, hi) = match kind {
        ColumnKind::TinyInt => (i8::MIN as i64, i8::MAX as i64),
        ColumnKind::SmallInt => (i16::MIN as i64, i16::MAX as i64),
        ColumnKind::Integer => (i32::MIN as i64, i32::MAX as i64),
        _ => (i64::MIN, i64::MAX),
    };
    match compare_type {
        FilterCompareType::LessEqual => {
            if right < lo {
                (false, FilterCompareType::AlwaysFalse)
            } else if right >= hi {
                (false, FilterCompareType::AlwaysTrue)
            } else {
                (true, compare_type)
            }
        }
        FilterCompareType::GreaterEqual => {
            if right > hi {
                (false, FilterCompareType::AlwaysFalse)
            } else if right <= lo {
                (false, FilterCompareType::AlwaysTrue)
            } else {
                (true, compare_type)
            }
        }
        FilterCompareType::Equal => {
            if right < lo || right > hi {
                (false, FilterCompareType::AlwaysFalse)
            } else {
                (true, compare_type)
            }
        }
        _ => (false, FilterCompareType::Invalid),
    }
}

enum DoubleNarrow {
    Value(i64),
    AlwaysTrue,
    AlwaysFalse,
}

fn integral_continue_double(
    right: f64,
    kind: ColumnKind,
    compare_type: FilterCompareType,
) -> DoubleNarrow {
    let (lo, hi) = kind.bounds();
    match compare_type {
        FilterCompareType::LessEqual => {
            let floored = right.floor();
            if floored < lo {
                DoubleNarrow::AlwaysFalse
            } else if floored >= hi {
                DoubleNarrow::AlwaysTrue
            } else {
                DoubleNarrow::Value(floored as i64)
            }
        }
        FilterCompareType::GreaterEqual => {
            let ceiled = right.ceil();
            if ceiled > hi {
                DoubleNarrow::AlwaysFalse
            } else if ceiled <= lo {
                DoubleNarrow::AlwaysTrue
            } else {
                DoubleNarrow::Value(ceiled as i64)
            }
        }
        FilterCompareType::Equal => {
            if right >= lo && right <= hi {
                let int_val = right as i64;
                if int_val as f64 == right {
                    return DoubleNarrow::Value(int_val);
                }
            }
            DoubleNarrow::AlwaysFalse
        }
        _ => DoubleNarrow::AlwaysFalse,
    }
}

// ---------------------------------------------------------------------------
// ExpressionInfoTree
// ---------------------------------------------------------------------------

/// C++ `ExpressionInfoTree<T>`.
#[derive(Debug, Clone, PartialEq)]
pub struct ExpressionInfoTree<T> {
    pub expr: Expr,
    pub children: Vec<ExpressionInfoTree<T>>,
    pub info: T,
}

/// C++ `ExpressionIndexScanInfo::Enum`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IndexScanInfo {
    UnknownExpr,
    ValueExpr,
    SecondaryIndexColumnExpr,
    VarcharSecondaryIndexColumnExpr,
    SecondaryIndexValueCompareExpr,
    ValueSecondaryIndexCompareExpr,
    SecondaryIndexFunctionExpr,
    VarcharSecondaryIndexFunctionExpr,
    SecondaryFunctionalIndexValueCompareExpr,
    ValueSecondaryFunctionalIndexCompareExpr,
    FilterFulltextExpr,
    AndExpr,
    OrExpr,
    JsonSecondaryIndexFunctionExpr,
    JsonSecondaryIndexValueCompareExpr,
    ValueJsonSecondaryIndexCompareExpr,
}

fn is_column_like(info: IndexScanInfo) -> bool {
    matches!(
        info,
        IndexScanInfo::SecondaryIndexColumnExpr
            | IndexScanInfo::VarcharSecondaryIndexColumnExpr
            | IndexScanInfo::SecondaryIndexFunctionExpr
            | IndexScanInfo::VarcharSecondaryIndexFunctionExpr
            | IndexScanInfo::JsonSecondaryIndexFunctionExpr
            | IndexScanInfo::UnknownExpr
    )
}

impl ExpressionInfoTree<IndexScanInfo> {
    /// C++ `ExpressionIndexScanInfo::BuildTree`.
    pub fn build(expr: &Expr, catalog: &IndexCatalog) -> Self {
        let mut tree = Self {
            expr: expr.clone(),
            children: Vec::new(),
            info: IndexScanInfo::UnknownExpr,
        };
        match expr {
            Expr::Literal(_) => tree.info = IndexScanInfo::ValueExpr,
            Expr::Column(i) => {
                if let Some(name) = catalog.column_names.get(*i) {
                    if let Some(info) = catalog.secondary.get(name) {
                        tree.info = if info.kind.is_varchar() {
                            IndexScanInfo::VarcharSecondaryIndexColumnExpr
                        } else {
                            IndexScanInfo::SecondaryIndexColumnExpr
                        };
                    }
                }
            }
            Expr::FullText(..) => tree.info = IndexScanInfo::FilterFulltextExpr,
            Expr::Cmp(a, op, b) => {
                tree.children = vec![Self::build(a, catalog), Self::build(b, catalog)];
                tree.info = if *op == CmpOp::Ne {
                    IndexScanInfo::UnknownExpr
                } else {
                    classify_compare(
                        tree.children[0].info,
                        tree.children[1].info,
                        *op == CmpOp::Eq,
                    )
                };
            }
            Expr::And(a, b) | Expr::Or(a, b) => {
                tree.children = vec![Self::build(a, catalog), Self::build(b, catalog)];
                let all_column_like = tree.children.iter().all(|c| is_column_like(c.info));
                let all_value = tree
                    .children
                    .iter()
                    .all(|c| c.info == IndexScanInfo::ValueExpr);
                if all_value {
                    tree.info = IndexScanInfo::ValueExpr;
                } else if !all_column_like {
                    tree.info = if matches!(expr, Expr::And(..)) {
                        IndexScanInfo::AndExpr
                    } else {
                        IndexScanInfo::OrExpr
                    };
                }
            }
            Expr::Func { name, args } => {
                tree.children = args.iter().map(|a| Self::build(a, catalog)).collect();
                let all_value = tree
                    .children
                    .iter()
                    .all(|c| c.info == IndexScanInfo::ValueExpr);
                if all_value {
                    tree.info = IndexScanInfo::ValueExpr;
                    return tree;
                }
                let upper = name.to_ascii_uppercase();
                if upper == "AND" || upper == "OR" {
                    let all_column_like = tree.children.iter().all(|c| is_column_like(c.info));
                    if !all_column_like {
                        tree.info = if upper == "AND" {
                            IndexScanInfo::AndExpr
                        } else {
                            IndexScanInfo::OrExpr
                        };
                    }
                    return tree;
                }
                if let Some(op) = compare_op(name) {
                    if tree.children.len() == 2 {
                        tree.info = classify_compare(
                            tree.children[0].info,
                            tree.children[1].info,
                            op == CmpOp::Eq,
                        );
                    }
                    return tree;
                }
                if is_json_index_function(name) {
                    if let Some(Expr::Column(i)) = args.first() {
                        if let Some(col) = catalog.column_names.get(*i) {
                            if let Some(info) = catalog.secondary.get(col) {
                                if info.json {
                                    tree.info = IndexScanInfo::JsonSecondaryIndexFunctionExpr;
                                }
                            }
                        }
                    }
                    return tree;
                }
                if cast_target(expr).is_some() {
                    if let Some(child) = tree.children.first() {
                        let child_info = child.info;
                        if cast_valid(catalog, expr, child_info) {
                            tree.info = child_info;
                        }
                    }
                    return tree;
                }
                if !(matches!(upper.as_str(), "NOT" | "<>" | "!=")
                    || (name == "-" && args.len() == 1))
                {
                    if let Some(key) = function_key(expr) {
                        if catalog.functional.contains_key(&key) {
                            let kind = catalog
                                .column_names
                                .first()
                                .and_then(|_| find_column(expr))
                                .and_then(|i| catalog.kind(i));
                            tree.info = if kind == Some(ColumnKind::Varchar) {
                                IndexScanInfo::VarcharSecondaryIndexFunctionExpr
                            } else {
                                IndexScanInfo::SecondaryIndexFunctionExpr
                            };
                        }
                    }
                }
            }
            _ => {}
        }
        tree
    }
}

fn cast_valid(catalog: &IndexCatalog, cast: &Expr, child: IndexScanInfo) -> bool {
    if !is_column_like(child) {
        return false;
    }
    let Some(target) = cast_target(cast) else {
        return false;
    };
    let Some(target) = kind_from_type_name(target) else {
        return false;
    };
    let source = match cast {
        Expr::Func { args, .. } => args.first().and_then(|a| match a {
            Expr::Column(i) => catalog.kind(*i),
            _ => None,
        }),
        _ => None,
    };
    match (target, source) {
        (ColumnKind::BigInt, Some(k)) => matches!(
            k,
            ColumnKind::TinyInt | ColumnKind::SmallInt | ColumnKind::Integer
        ),
        (ColumnKind::Double, Some(k)) => matches!(
            k,
            ColumnKind::TinyInt
                | ColumnKind::SmallInt
                | ColumnKind::Integer
                | ColumnKind::BigInt
                | ColumnKind::Float
        ),
        _ => false,
    }
}

/// C++ `CompareFunctionNames` → operator.
fn compare_op(name: &str) -> Option<CmpOp> {
    Some(match name {
        "<" => CmpOp::Lt,
        ">" => CmpOp::Gt,
        "<=" => CmpOp::Le,
        ">=" => CmpOp::Ge,
        "=" => CmpOp::Eq,
        _ => return None,
    })
}

fn is_json_index_function(name: &str) -> bool {
    matches!(
        name.to_ascii_lowercase().as_str(),
        "json_extract_int"
            | "json_extract_double"
            | "json_extract_bool"
            | "json_extract_string"
            | "json_exists_path"
            | "json_contains"
    )
}

fn classify_compare(left: IndexScanInfo, right: IndexScanInfo, is_eq: bool) -> IndexScanInfo {
    let column_ok = |info: IndexScanInfo| match info {
        IndexScanInfo::SecondaryIndexColumnExpr => true,
        IndexScanInfo::VarcharSecondaryIndexColumnExpr => is_eq,
        _ => false,
    };
    let func_ok = |info: IndexScanInfo| match info {
        IndexScanInfo::SecondaryIndexFunctionExpr => true,
        IndexScanInfo::VarcharSecondaryIndexFunctionExpr => is_eq,
        _ => false,
    };
    if column_ok(left) && right == IndexScanInfo::ValueExpr {
        IndexScanInfo::SecondaryIndexValueCompareExpr
    } else if column_ok(right) && left == IndexScanInfo::ValueExpr {
        IndexScanInfo::ValueSecondaryIndexCompareExpr
    } else if func_ok(left) && right == IndexScanInfo::ValueExpr {
        IndexScanInfo::SecondaryFunctionalIndexValueCompareExpr
    } else if func_ok(right) && left == IndexScanInfo::ValueExpr {
        IndexScanInfo::ValueSecondaryFunctionalIndexCompareExpr
    } else if left == IndexScanInfo::JsonSecondaryIndexFunctionExpr {
        IndexScanInfo::JsonSecondaryIndexValueCompareExpr
    } else if right == IndexScanInfo::JsonSecondaryIndexFunctionExpr {
        IndexScanInfo::ValueJsonSecondaryIndexCompareExpr
    } else {
        IndexScanInfo::UnknownExpr
    }
}

// ---------------------------------------------------------------------------
// Bitmask
// ---------------------------------------------------------------------------

/// A row bitmask with the all-true fast path the C++ `Bitmask` carries.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Bitmask {
    count: u32,
    all_true: bool,
    values: BTreeSet<u32>,
}

impl Bitmask {
    /// C++ `Bitmask(count)` (all true by default).
    pub fn all_true(count: u32) -> Self {
        Self {
            count,
            all_true: true,
            values: BTreeSet::new(),
        }
    }

    pub fn all_false(count: u32) -> Self {
        Self {
            count,
            all_true: false,
            values: BTreeSet::new(),
        }
    }

    pub fn set_true(&mut self, row: u32) {
        if row < self.count && !self.all_true {
            self.values.insert(row);
            if self.values.len() as u32 == self.count {
                self.all_true = true;
                self.values.clear();
            }
        }
    }

    pub fn set_all_false(&mut self) {
        self.all_true = false;
        self.values.clear();
    }

    pub fn is_true(&self, row: u32) -> bool {
        row < self.count && (self.all_true || self.values.contains(&row))
    }

    pub fn count_true(&self) -> u32 {
        if self.all_true {
            self.count
        } else {
            self.values.len() as u32
        }
    }

    pub fn count_false(&self) -> u32 {
        self.count - self.count_true()
    }

    pub fn is_all_true(&self) -> bool {
        self.all_true
    }

    pub fn merge_and(&mut self, other: &Bitmask) {
        if self.all_true {
            *self = other.clone();
            return;
        }
        if other.all_true {
            return;
        }
        self.values = self.values.intersection(&other.values).copied().collect();
    }

    pub fn merge_or(&mut self, other: &Bitmask) {
        if other.all_true {
            self.all_true = true;
            self.values.clear();
            return;
        }
        if self.all_true {
            return;
        }
        self.values.extend(other.values.iter().copied());
        if self.values.len() as u32 == self.count {
            self.all_true = true;
            self.values.clear();
        }
    }
}

// ---------------------------------------------------------------------------
// Evaluators
// ---------------------------------------------------------------------------

/// C++ `IndexFilterEvaluator::Type`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum EvaluatorType {
    AllTrue,
    AllFalse,
    SecondaryIndex,
    FulltextIndex,
    And,
    Or,
    Invalid,
}

/// A secondary index queried by range. The provider owns the stored keys; the
/// evaluator only supplies the inclusive `[start, end]` pair.
pub trait SecondaryIndexProvider {
    fn range_rows(&self, column_id: usize, start: &Value, end: &Value) -> Vec<u32>;
}

/// A full-text index queried by term.
pub trait FulltextIndex {
    fn postings(&self, term: &str) -> Vec<u32>;
    /// BM25 score for `doc` over the query's terms; only consulted when a
    /// score threshold is set.
    fn score(&self, _doc: u32, _terms: &[String]) -> f32 {
        0.0
    }
}

/// The per-segment inputs an evaluator needs.
#[derive(Clone, Copy)]
pub struct IndexEvalContext<'a> {
    pub row_count: u32,
    pub secondary: Option<&'a dyn SecondaryIndexProvider>,
    pub fulltext: Option<&'a dyn FulltextIndex>,
}

impl<'a> IndexEvalContext<'a> {
    pub fn new(row_count: u32) -> Self {
        Self {
            row_count,
            secondary: None,
            fulltext: None,
        }
    }

    pub fn with_secondary(mut self, provider: &'a dyn SecondaryIndexProvider) -> Self {
        self.secondary = Some(provider);
        self
    }

    pub fn with_fulltext(mut self, index: &'a dyn FulltextIndex) -> Self {
        self.fulltext = Some(index);
        self
    }
}

/// C++ `FilterFulltextExpression::options_text_` subset.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FulltextSimilarity {
    Bm25,
    Boolean,
}

/// C++ `QueryNode` subset used by a filter_fulltext predicate.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum FulltextQuery {
    Term(String),
    And(Vec<FulltextQuery>),
    Or(Vec<FulltextQuery>),
}

impl FulltextQuery {
    pub fn leaf_count(&self) -> usize {
        match self {
            FulltextQuery::Term(_) => 1,
            FulltextQuery::And(c) | FulltextQuery::Or(c) => c.iter().map(|q| q.leaf_count()).sum(),
        }
    }

    pub fn terms(&self) -> Vec<String> {
        match self {
            FulltextQuery::Term(t) => vec![t.clone()],
            FulltextQuery::And(c) | FulltextQuery::Or(c) => {
                c.iter().flat_map(|q| q.terms()).collect()
            }
        }
    }

    fn optimize(self) -> FulltextQuery {
        match self {
            FulltextQuery::Term(t) => FulltextQuery::Term(t),
            FulltextQuery::And(children) => {
                let mut flat = Vec::new();
                for child in children {
                    match child.optimize() {
                        FulltextQuery::And(inner) => flat.extend(inner),
                        other => flat.push(other),
                    }
                }
                if flat.len() == 1 {
                    flat.pop().unwrap()
                } else {
                    FulltextQuery::And(flat)
                }
            }
            FulltextQuery::Or(children) => {
                let mut flat = Vec::new();
                for child in children {
                    match child.optimize() {
                        FulltextQuery::Or(inner) => flat.extend(inner),
                        other => flat.push(other),
                    }
                }
                if flat.len() == 1 {
                    flat.pop().unwrap()
                } else {
                    FulltextQuery::Or(flat)
                }
            }
        }
    }
}

/// C++ `IndexFilterEvaluatorFulltext`.
#[derive(Debug, Clone, PartialEq)]
pub struct IndexFilterEvaluatorFulltext {
    pub src_expr: Expr,
    pub query_tree: FulltextQuery,
    pub minimum_should_match: usize,
    pub score_threshold: f32,
    pub similarity: FulltextSimilarity,
    pub optimized: bool,
}

impl IndexFilterEvaluatorFulltext {
    pub fn new(src_expr: Expr, query_tree: FulltextQuery) -> Self {
        Self {
            src_expr,
            query_tree,
            minimum_should_match: 0,
            score_threshold: 0.0,
            similarity: FulltextSimilarity::Bm25,
            optimized: false,
        }
    }

    pub fn have_minimum_should_match_option(&self) -> bool {
        self.minimum_should_match > 0
    }

    /// C++ `OptimizeQueryTree`.
    pub fn optimize_query_tree(&mut self) {
        assert!(
            !self.optimized,
            "IndexFilterEvaluatorFulltext: already optimized"
        );
        self.query_tree = self.query_tree.clone().optimize();
        self.optimized = true;
    }

    /// C++ `Evaluate`: the rows whose documents match the query tree.
    pub fn evaluate(&self, ctx: &IndexEvalContext<'_>) -> Result<Bitmask> {
        let index = ctx
            .fulltext
            .ok_or_else(|| Error::Unexpected("full-text evaluator needs an index".into()))?;
        let mut mask = Bitmask::all_false(ctx.row_count);
        let terms = self.query_tree.terms();
        let docs = match &self.query_tree {
            FulltextQuery::Term(t) => index.postings(t),
            FulltextQuery::And(children) => {
                let mut acc: Option<BTreeSet<u32>> = None;
                for child in children {
                    let set: BTreeSet<u32> = if let FulltextQuery::Term(t) = child {
                        index.postings(t).into_iter().collect()
                    } else {
                        // nested boolean node: evaluate through a child evaluator
                        let mut sub = self.clone();
                        sub.query_tree = child.clone();
                        sub.optimized = true;
                        let sub_mask = sub.evaluate(ctx)?;
                        (0..ctx.row_count)
                            .filter(|r| sub_mask.is_true(*r))
                            .collect()
                    };
                    acc = Some(match acc {
                        None => set,
                        Some(prev) => prev.intersection(&set).copied().collect(),
                    });
                }
                acc.unwrap_or_default().into_iter().collect()
            }
            FulltextQuery::Or(children) => {
                let mut sets: Vec<BTreeSet<u32>> = Vec::new();
                for child in children {
                    let set: BTreeSet<u32> = if let FulltextQuery::Term(t) = child {
                        index.postings(t).into_iter().collect()
                    } else {
                        let mut sub = self.clone();
                        sub.query_tree = child.clone();
                        sub.optimized = true;
                        let sub_mask = sub.evaluate(ctx)?;
                        (0..ctx.row_count)
                            .filter(|r| sub_mask.is_true(*r))
                            .collect()
                    };
                    sets.push(set);
                }
                let minimum = self.minimum_should_match.min(sets.len());
                let mut counts: BTreeMap<u32, usize> = BTreeMap::new();
                for set in &sets {
                    for doc in set {
                        *counts.entry(*doc).or_default() += 1;
                    }
                }
                counts
                    .into_iter()
                    .filter(|(_, n)| *n >= minimum.max(1))
                    .map(|(doc, _)| doc)
                    .collect()
            }
        };
        for doc in docs {
            if doc >= ctx.row_count {
                continue;
            }
            if self.score_threshold > 0.0 && index.score(doc, &terms) < self.score_threshold {
                continue;
            }
            mask.set_true(doc);
        }
        Ok(mask)
    }
}

/// C++ `IndexFilterEvaluatorSecondary` (the typed `...T` instantiation).
#[derive(Debug, Clone, PartialEq)]
pub struct IndexFilterEvaluatorSecondary {
    pub src_expr: Expr,
    pub column_id: usize,
    pub column_kind: ColumnKind,
    pub json: bool,
    /// Inclusive `[start, end]` pairs, sorted.
    pub ranges: Vec<(Value, Value)>,
}

impl IndexFilterEvaluatorSecondary {
    /// C++ `IndexFilterEvaluatorSecondaryT::Make`.
    pub fn make(
        src_expr: Expr,
        column_id: usize,
        column_kind: ColumnKind,
        json: bool,
        compare_type: FilterCompareType,
        value: Value,
    ) -> Result<Self> {
        let mut result = Self {
            src_expr,
            column_id,
            column_kind,
            json,
            ranges: Vec::new(),
        };
        match compare_type {
            FilterCompareType::Equal => result.ranges.push((value.clone(), value)),
            FilterCompareType::GreaterEqual => {
                let end = if json {
                    json_upper(value.to_slt_string())
                } else {
                    upper_bound(column_kind)
                };
                result.ranges.push((value, end));
            }
            FilterCompareType::LessEqual => {
                let begin = if json {
                    json_prefix(value.to_slt_string())
                } else {
                    lower_bound(column_kind)
                };
                result.ranges.push((begin, value));
            }
            FilterCompareType::AlwaysTrue | FilterCompareType::AlwaysFalse => {}
            _ => {
                return Err(Error::Invalid(
                    "wrong comparison type for index scan".into(),
                ))
            }
        }
        if column_kind.is_varchar() && compare_type != FilterCompareType::Equal {
            return Err(Error::Invalid(
                "Varchar only supports equality in a secondary index".into(),
            ));
        }
        Ok(result)
    }

    pub fn is_valid(&self) -> bool {
        !self.ranges.is_empty()
    }

    /// C++ `AddRange` (JSON-only in the reference).
    pub fn add_range(&mut self, range: (Value, Value)) {
        self.ranges.push(range);
    }

    /// C++ `Merge`: interval union (`Or`) or intersection (`And`).
    pub fn merge(&mut self, other: &Self, op: EvaluatorType) -> Result<()> {
        if self.column_id != other.column_id {
            return Err(Error::Unexpected("merge of different columns".into()));
        }
        let mut merged = Vec::new();
        let mut left = self.ranges.clone();
        let mut right = other.ranges.clone();
        left.sort_by(|a, b| cmp_value(&a.0, &b.0));
        right.sort_by(|a, b| cmp_value(&a.0, &b.0));
        match op {
            EvaluatorType::Or => {
                let mut all = left;
                all.extend(right);
                all.sort_by(|a, b| cmp_value(&a.0, &b.0));
                for range in all {
                    match merged.last_mut() {
                        Some((_, end)) if cmp_value(&range.0, end) != Ordering::Greater => {
                            if cmp_value(&range.1, end) == Ordering::Greater {
                                *end = range.1;
                            }
                        }
                        _ => merged.push(range),
                    }
                }
            }
            EvaluatorType::And => {
                let (mut i, mut j) = (0usize, 0usize);
                while i < left.len() && j < right.len() {
                    let (s1, e1) = &left[i];
                    let (s2, e2) = &right[j];
                    if cmp_value(e1, s2) == Ordering::Less {
                        i += 1;
                        continue;
                    }
                    if cmp_value(e2, s1) == Ordering::Less {
                        j += 1;
                        continue;
                    }
                    let start = if cmp_value(s1, s2) == Ordering::Less {
                        s2.clone()
                    } else {
                        s1.clone()
                    };
                    let end = if cmp_value(e1, e2) == Ordering::Less {
                        e1.clone()
                    } else {
                        e2.clone()
                    };
                    merged.push((start, end));
                    if cmp_value(e1, e2) != Ordering::Greater {
                        i += 1;
                    } else {
                        j += 1;
                    }
                }
            }
            _ => return Err(Error::Invalid("merge only supports AND/OR".into())),
        }
        self.ranges = merged;
        Ok(())
    }

    /// C++ `Evaluate`: selects the rows whose key falls in any range.
    pub fn evaluate(&self, ctx: &IndexEvalContext<'_>) -> Result<Bitmask> {
        let provider = ctx
            .secondary
            .ok_or_else(|| Error::Unexpected("secondary evaluator needs an index".into()))?;
        let mut mask = Bitmask::all_false(ctx.row_count);
        for (start, end) in &self.ranges {
            for row in provider.range_rows(self.column_id, start, end) {
                if row < ctx.row_count {
                    mask.set_true(row);
                }
            }
        }
        Ok(mask)
    }
}

fn cmp_value(a: &Value, b: &Value) -> Ordering {
    a.partial_cmp_value(b).unwrap_or(Ordering::Equal)
}

fn lower_bound(kind: ColumnKind) -> Value {
    match kind {
        ColumnKind::Boolean => Value::Bool(false),
        ColumnKind::Varchar | ColumnKind::Json => Value::Str(String::new()),
        ColumnKind::Float | ColumnKind::Double => Value::Float(f64::MIN),
        _ => Value::Int(i64::MIN),
    }
}

fn upper_bound(kind: ColumnKind) -> Value {
    match kind {
        ColumnKind::Boolean => Value::Bool(true),
        ColumnKind::Varchar | ColumnKind::Json => Value::Str("\u{10ffff}".repeat(16)),
        ColumnKind::Float | ColumnKind::Double => Value::Float(f64::MAX),
        _ => Value::Int(i64::MAX),
    }
}

fn json_upper(term: String) -> Value {
    // `$path:tag:value` → `$path:tag;`, the exclusive end of this path+type.
    Value::Str(match term.rfind(':') {
        Some(pos) if pos >= 2 => format!("{};", &term[..pos]),
        _ => "\u{10ffff}".repeat(16),
    })
}

fn json_prefix(term: String) -> Value {
    Value::Str(match term.rfind(':') {
        Some(pos) if pos >= 2 => term[..pos].to_string(),
        _ => String::new(),
    })
}

/// C++ `IndexFilterEvaluatorLogicalChildren`.
#[derive(Debug, Clone, PartialEq)]
pub struct IndexFilterEvaluatorLogicalChildren {
    pub secondary: Vec<IndexFilterEvaluatorSecondary>,
    pub fulltext: Option<Box<IndexFilterEvaluatorFulltext>>,
    pub other: Vec<IndexFilterEvaluator>,
}

/// C++ `IndexFilterEvaluator` base + the AllTrue/AllFalse/Secondary/Fulltext/
/// LogicalChildren subclasses.
#[derive(Debug, Clone, PartialEq)]
pub enum IndexFilterEvaluator {
    AllTrue,
    AllFalse,
    Secondary(IndexFilterEvaluatorSecondary),
    Fulltext(Box<IndexFilterEvaluatorFulltext>),
    And(IndexFilterEvaluatorLogicalChildren),
    Or(IndexFilterEvaluatorLogicalChildren),
}

impl IndexFilterEvaluator {
    pub fn kind(&self) -> EvaluatorType {
        match self {
            IndexFilterEvaluator::AllTrue => EvaluatorType::AllTrue,
            IndexFilterEvaluator::AllFalse => EvaluatorType::AllFalse,
            IndexFilterEvaluator::Secondary(_) => EvaluatorType::SecondaryIndex,
            IndexFilterEvaluator::Fulltext(_) => EvaluatorType::FulltextIndex,
            IndexFilterEvaluator::And(_) => EvaluatorType::And,
            IndexFilterEvaluator::Or(_) => EvaluatorType::Or,
        }
    }

    /// C++ `Evaluate`.
    pub fn evaluate(&self, ctx: &IndexEvalContext<'_>) -> Result<Bitmask> {
        match self {
            IndexFilterEvaluator::AllTrue => Ok(Bitmask::all_true(ctx.row_count)),
            IndexFilterEvaluator::AllFalse => Ok(Bitmask::all_false(ctx.row_count)),
            IndexFilterEvaluator::Secondary(s) => s.evaluate(ctx),
            IndexFilterEvaluator::Fulltext(f) => f.evaluate(ctx),
            IndexFilterEvaluator::And(children) => children.evaluate_and(ctx),
            IndexFilterEvaluator::Or(children) => children.evaluate_or(ctx),
        }
    }

    fn optimize_fulltext(&mut self) {
        match self {
            IndexFilterEvaluator::Fulltext(f) => {
                if !f.optimized {
                    f.optimize_query_tree();
                }
            }
            IndexFilterEvaluator::And(c) | IndexFilterEvaluator::Or(c) => {
                if let Some(f) = &mut c.fulltext {
                    if !f.optimized {
                        f.optimize_query_tree();
                    }
                }
                for child in &mut c.other {
                    child.optimize_fulltext();
                }
            }
            _ => {}
        }
    }
}

impl IndexFilterEvaluatorLogicalChildren {
    /// C++ `IndexFilterEvaluatorAND::Evaluate`.
    fn evaluate_and(&self, ctx: &IndexEvalContext<'_>) -> Result<Bitmask> {
        let mut result = Bitmask::all_true(ctx.row_count);
        for child in &self.secondary {
            result.merge_and(&child.evaluate(ctx)?);
            if result.count_true() == 0 {
                return Ok(result);
            }
        }
        if let Some(fulltext) = &self.fulltext {
            result.merge_and(&fulltext.evaluate(ctx)?);
        }
        for child in &self.other {
            if result.count_true() == 0 {
                return Ok(result);
            }
            result.merge_and(&child.evaluate(ctx)?);
        }
        Ok(result)
    }

    /// C++ `IndexFilterEvaluatorOR::Evaluate`.
    fn evaluate_or(&self, ctx: &IndexEvalContext<'_>) -> Result<Bitmask> {
        let mut result = Bitmask::all_false(ctx.row_count);
        for child in &self.secondary {
            result.merge_or(&child.evaluate(ctx)?);
            if result.count_false() == 0 {
                return Ok(result);
            }
        }
        if let Some(fulltext) = &self.fulltext {
            result.merge_or(&fulltext.evaluate(ctx)?);
        }
        for child in &self.other {
            if result.count_false() == 0 {
                return Ok(result);
            }
            result.merge_or(&child.evaluate(ctx)?);
        }
        Ok(result)
    }
}

fn merge_fulltext(
    target: &mut Option<Box<IndexFilterEvaluatorFulltext>>,
    other: &mut Vec<IndexFilterEvaluator>,
    input: Box<IndexFilterEvaluatorFulltext>,
    op: EvaluatorType,
) {
    let Some(current) = target else {
        *target = Some(input);
        return;
    };
    let mergeable = !current.have_minimum_should_match_option()
        && !input.have_minimum_should_match_option()
        && current.score_threshold <= 0.0
        && input.score_threshold <= 0.0
        && current.similarity == input.similarity;
    if !mergeable {
        other.push(IndexFilterEvaluator::Fulltext(input));
        return;
    }
    let left = std::mem::replace(&mut current.query_tree, FulltextQuery::Term(String::new()));
    let new_tree = match op {
        EvaluatorType::And => FulltextQuery::And(vec![left, input.query_tree.clone()]),
        EvaluatorType::Or => FulltextQuery::Or(vec![left, input.query_tree.clone()]),
        _ => return,
    };
    current.query_tree = new_tree;
    current.src_expr = Expr::And(
        Box::new(current.src_expr.clone()),
        Box::new(input.src_expr.clone()),
    );
    current.optimized = false;
}

/// C++ `IndexFilterEvaluatorBuildFromAnd`.
pub fn index_filter_evaluator_build_from_and(
    candidates: Vec<IndexFilterEvaluator>,
) -> IndexFilterEvaluator {
    let combine = EvaluatorType::And;
    let mut secondary = Vec::new();
    let mut fulltext = None;
    let mut other = Vec::new();
    let mut candidates = candidates;
    if candidates
        .iter()
        .any(|c| c.kind() == EvaluatorType::AllFalse)
    {
        return IndexFilterEvaluator::AllFalse;
    }
    candidates.retain(|c| c.kind() != EvaluatorType::AllTrue);
    for child in candidates {
        match child {
            IndexFilterEvaluator::Secondary(s) => secondary.push(s),
            IndexFilterEvaluator::Fulltext(f) => {
                merge_fulltext(&mut fulltext, &mut other, f, combine)
            }
            IndexFilterEvaluator::And(c) => {
                secondary.extend(c.secondary);
                if let Some(f) = c.fulltext {
                    merge_fulltext(&mut fulltext, &mut other, f, combine);
                }
                other.extend(c.other);
            }
            other_child => other.push(other_child),
        }
    }
    if !simplify_secondary(&mut secondary, combine) {
        return IndexFilterEvaluator::AllFalse;
    }
    if secondary.is_empty() && other.is_empty() && fulltext.is_none() {
        return IndexFilterEvaluator::AllTrue;
    }
    if secondary.is_empty() && other.is_empty() {
        return IndexFilterEvaluator::Fulltext(fulltext.unwrap());
    }
    if fulltext.is_none() {
        if secondary.is_empty() && other.len() == 1 {
            return other.pop().unwrap();
        }
        if secondary.len() == 1 && other.is_empty() {
            return IndexFilterEvaluator::Secondary(secondary.pop().unwrap());
        }
    }
    IndexFilterEvaluator::And(IndexFilterEvaluatorLogicalChildren {
        secondary,
        fulltext,
        other,
    })
}

/// C++ `IndexFilterEvaluatorBuildFromOr`.
pub fn index_filter_evaluator_build_from_or(
    candidates: Vec<IndexFilterEvaluator>,
) -> IndexFilterEvaluator {
    let combine = EvaluatorType::Or;
    let mut secondary = Vec::new();
    let mut fulltext = None;
    let mut other = Vec::new();
    let mut candidates = candidates;
    if candidates
        .iter()
        .any(|c| c.kind() == EvaluatorType::AllTrue)
    {
        return IndexFilterEvaluator::AllTrue;
    }
    candidates.retain(|c| c.kind() != EvaluatorType::AllFalse);
    for child in candidates {
        match child {
            IndexFilterEvaluator::Secondary(s) => secondary.push(s),
            IndexFilterEvaluator::Fulltext(f) => {
                merge_fulltext(&mut fulltext, &mut other, f, combine)
            }
            IndexFilterEvaluator::Or(c) => {
                secondary.extend(c.secondary);
                if let Some(f) = c.fulltext {
                    merge_fulltext(&mut fulltext, &mut other, f, combine);
                }
                other.extend(c.other);
            }
            other_child => other.push(other_child),
        }
    }
    if !simplify_secondary(&mut secondary, combine) {
        return IndexFilterEvaluator::AllTrue;
    }
    if secondary.is_empty() && other.is_empty() && fulltext.is_none() {
        return IndexFilterEvaluator::AllFalse;
    }
    if secondary.is_empty() && other.is_empty() {
        return IndexFilterEvaluator::Fulltext(fulltext.unwrap());
    }
    if fulltext.is_none() {
        if secondary.is_empty() && other.len() == 1 {
            return other.pop().unwrap();
        }
        if secondary.len() == 1 && other.is_empty() {
            return IndexFilterEvaluator::Secondary(secondary.pop().unwrap());
        }
    }
    IndexFilterEvaluator::Or(IndexFilterEvaluatorLogicalChildren {
        secondary,
        fulltext,
        other,
    })
}

/// C++ `SimplifySecondaryIndexEvaluators`: merge intervals per column.
fn simplify_secondary(
    evaluators: &mut Vec<IndexFilterEvaluatorSecondary>,
    op: EvaluatorType,
) -> bool {
    if evaluators.is_empty() {
        return true;
    }
    let mut by_column: BTreeMap<usize, Vec<IndexFilterEvaluatorSecondary>> = BTreeMap::new();
    let mut json = Vec::new();
    for evaluator in evaluators.drain(..) {
        if evaluator.json {
            json.push(evaluator);
        } else {
            by_column
                .entry(evaluator.column_id)
                .or_default()
                .push(evaluator);
        }
    }
    for (_, mut same_column) in by_column {
        let mut result = same_column.remove(0);
        for other in same_column {
            if result.merge(&other, op).is_err() || !result.is_valid() {
                return false;
            }
        }
        evaluators.push(result);
    }
    evaluators.extend(json);
    true
}

// ---------------------------------------------------------------------------
// IndexScanFilterExpressionPushDownResult / FilterExpressionPushDown
// ---------------------------------------------------------------------------

/// C++ `IndexScanFilterExpressionPushDownResult`.
#[derive(Debug, Clone, PartialEq)]
pub struct IndexScanFilterExpressionPushDownResult {
    pub index_filter: Option<Expr>,
    pub leftover_filter: Option<Expr>,
    pub index_filter_evaluator: IndexFilterEvaluator,
}

fn and_exprs(mut exprs: Vec<Expr>) -> Option<Expr> {
    match exprs.len() {
        0 => None,
        1 => exprs.pop(),
        _ => {
            let mut iter = exprs.into_iter();
            let mut acc = iter.next().unwrap();
            for next in iter {
                acc = Expr::And(Box::new(acc), Box::new(next));
            }
            Some(acc)
        }
    }
}

fn split_tree(tree: &ExpressionInfoTree<IndexScanInfo>) -> (Option<Expr>, Option<Expr>) {
    match tree.info {
        IndexScanInfo::VarcharSecondaryIndexColumnExpr
        | IndexScanInfo::SecondaryIndexColumnExpr
        | IndexScanInfo::VarcharSecondaryIndexFunctionExpr
        | IndexScanInfo::SecondaryIndexFunctionExpr
        | IndexScanInfo::UnknownExpr => (None, Some(tree.expr.clone())),
        IndexScanInfo::ValueExpr
        | IndexScanInfo::ValueSecondaryIndexCompareExpr
        | IndexScanInfo::SecondaryIndexValueCompareExpr
        | IndexScanInfo::ValueSecondaryFunctionalIndexCompareExpr
        | IndexScanInfo::SecondaryFunctionalIndexValueCompareExpr
        | IndexScanInfo::JsonSecondaryIndexValueCompareExpr
        | IndexScanInfo::ValueJsonSecondaryIndexCompareExpr
        | IndexScanInfo::JsonSecondaryIndexFunctionExpr
        | IndexScanInfo::FilterFulltextExpr => (Some(tree.expr.clone()), None),
        IndexScanInfo::AndExpr => {
            let mut index_parts = Vec::new();
            let mut leftover_parts = Vec::new();
            for child in &tree.children {
                let (index, leftover) = split_tree(child);
                if let Some(e) = index {
                    index_parts.push(e);
                }
                if let Some(e) = leftover {
                    leftover_parts.push(e);
                }
            }
            (and_exprs(index_parts), and_exprs(leftover_parts))
        }
        IndexScanInfo::OrExpr => {
            let all_index = tree
                .children
                .iter()
                .all(|child| split_tree(child).1.is_none());
            if all_index {
                (Some(tree.expr.clone()), None)
            } else {
                (None, Some(tree.expr.clone()))
            }
        }
    }
}

fn filter_compare_of(op: CmpOp) -> Option<FilterCompareType> {
    Some(match op {
        CmpOp::Eq => FilterCompareType::Equal,
        CmpOp::Lt => FilterCompareType::Less,
        CmpOp::Le => FilterCompareType::LessEqual,
        CmpOp::Gt => FilterCompareType::Greater,
        CmpOp::Ge => FilterCompareType::GreaterEqual,
        CmpOp::Ne => return None,
    })
}

fn compare_parts(expr: &Expr) -> Option<(&Expr, CmpOp, &Expr)> {
    match expr {
        Expr::Cmp(a, op, b) => Some((a, *op, b)),
        Expr::Func { name, args } if args.len() == 2 => {
            Some((&args[0], compare_op(name)?, &args[1]))
        }
        _ => None,
    }
}

fn parse_fulltext_query(text: &str, options: &str) -> FulltextQuery {
    let terms: Vec<String> = text
        .split_whitespace()
        .map(|t| {
            t.trim_matches(|c| c == '"' || c == '\'' || c == '(' || c == ')')
                .to_ascii_lowercase()
        })
        .filter(|t| !t.is_empty())
        .collect();
    let or = options
        .split(';')
        .any(|p| p.trim().eq_ignore_ascii_case("operator=or"));
    let queries: Vec<FulltextQuery> = terms.into_iter().map(FulltextQuery::Term).collect();
    if queries.is_empty() {
        FulltextQuery::Term(String::new())
    } else if or {
        FulltextQuery::Or(queries)
    } else {
        FulltextQuery::And(queries)
    }
}

fn fulltext_evaluator(expr: &Expr) -> Option<IndexFilterEvaluatorFulltext> {
    let Expr::FullText(_, text, options) = expr else {
        return None;
    };
    let mut evaluator =
        IndexFilterEvaluatorFulltext::new(expr.clone(), parse_fulltext_query(text, options));
    for part in options.split(';') {
        let (key, value) = part.split_once('=').unwrap_or(("", ""));
        match key.trim().to_ascii_lowercase().as_str() {
            "minimum_should_match" => {
                evaluator.minimum_should_match = value
                    .trim()
                    .parse::<usize>()
                    .unwrap_or(evaluator.minimum_should_match)
            }
            "threshold" => evaluator.score_threshold = value.trim().parse::<f32>().unwrap_or(0.0),
            "similarity" if value.trim().eq_ignore_ascii_case("boolean") => {
                evaluator.similarity = FulltextSimilarity::Boolean
            }
            _ => {}
        }
    }
    evaluator.optimize_query_tree();
    Some(evaluator)
}

fn secondary_from_compare(
    left: &Expr,
    op: CmpOp,
    right: &Expr,
    catalog: &IndexCatalog,
) -> Result<IndexFilterEvaluator> {
    let compare = filter_compare_of(op)
        .ok_or_else(|| Error::Invalid("cannot push a '<>' comparison into an index".into()))?;
    let value = FilterExpressionPushDownHelper::calc_value_result(right)?;
    let unwound = FilterExpressionPushDownHelper::unwind_cast(left, value, compare, catalog)?;
    if matches!(
        unwound.compare_type,
        FilterCompareType::AlwaysTrue | FilterCompareType::AlwaysFalse
    ) {
        return Ok(if unwound.compare_type == FilterCompareType::AlwaysTrue {
            IndexFilterEvaluator::AllTrue
        } else {
            IndexFilterEvaluator::AllFalse
        });
    }
    if unwound.compare_type == FilterCompareType::Invalid {
        return Ok(IndexFilterEvaluator::AllTrue);
    }
    let info = catalog
        .secondary_for(unwound.column_id)
        .ok_or_else(|| Error::Invalid("no secondary index for the pushed-down column".into()))?;
    let evaluator = IndexFilterEvaluatorSecondary::make(
        left.clone(),
        unwound.column_id,
        info.kind,
        info.json,
        unwound.compare_type,
        unwound.value,
    )?;
    Ok(IndexFilterEvaluator::Secondary(evaluator))
}

fn json_compare_evaluator(
    tree: &ExpressionInfoTree<IndexScanInfo>,
    catalog: &IndexCatalog,
    reverse: bool,
) -> Result<IndexFilterEvaluator> {
    let Some((a, op, b)) = compare_parts(&tree.expr) else {
        return Ok(IndexFilterEvaluator::AllTrue);
    };
    let op = if reverse {
        match op {
            CmpOp::Lt => CmpOp::Gt,
            CmpOp::Le => CmpOp::Ge,
            CmpOp::Gt => CmpOp::Lt,
            CmpOp::Ge => CmpOp::Le,
            other => other,
        }
    } else {
        op
    };
    let (json_expr, value_expr) = if reverse { (b, a) } else { (a, b) };
    let Expr::Func { name, args } = json_expr else {
        return Ok(IndexFilterEvaluator::AllTrue);
    };
    let func_name = name.to_ascii_lowercase();
    let path = args
        .get(1)
        .and_then(|p| FilterExpressionPushDownHelper::calc_value_result(p).ok())
        .map(|v| v.to_slt_string())
        .unwrap_or_else(|| "$".to_string());
    let raw = FilterExpressionPushDownHelper::calc_value_result(value_expr)?;
    let mut compare = filter_compare_of(op)
        .ok_or_else(|| Error::Invalid("cannot push '<>' into a JSON index".into()))?;
    let term =
        FilterExpressionPushDownHelper::build_json_term(&func_name, &path, &raw, &mut compare);
    if compare == FilterCompareType::AlwaysFalse {
        return Ok(IndexFilterEvaluator::AllFalse);
    }
    let column = find_column(json_expr)
        .ok_or_else(|| Error::Invalid("JSON index expression has no column".into()))?;
    let info = catalog.secondary_for(column).ok_or_else(|| {
        Error::Invalid("no JSON secondary index for the pushed-down column".into())
    })?;
    let evaluator = IndexFilterEvaluatorSecondary::make(
        tree.expr.clone(),
        column,
        info.kind,
        true,
        compare,
        Value::Str(term),
    )?;
    Ok(IndexFilterEvaluator::Secondary(evaluator))
}

fn json_function_evaluator(expr: &Expr, catalog: &IndexCatalog) -> Result<IndexFilterEvaluator> {
    let Expr::Func { name, args } = expr else {
        return Ok(IndexFilterEvaluator::AllTrue);
    };
    let func_name = name.to_ascii_lowercase();
    let column = find_column(expr)
        .ok_or_else(|| Error::Invalid("JSON index function has no column".into()))?;
    let info = catalog.secondary_for(column).ok_or_else(|| {
        Error::Invalid("no JSON secondary index for the pushed-down column".into())
    })?;
    let mut compare = FilterCompareType::Equal;
    let (path, value) = match func_name.as_str() {
        "json_contains" if args.len() >= 3 => (
            FilterExpressionPushDownHelper::calc_value_result(&args[1])?.to_slt_string(),
            FilterExpressionPushDownHelper::calc_value_result(&args[2])?,
        ),
        "json_contains" if args.len() >= 2 => (
            "$".to_string(),
            FilterExpressionPushDownHelper::calc_value_result(&args[1])?,
        ),
        "json_exists_path" => (
            args.get(1)
                .and_then(|p| FilterExpressionPushDownHelper::calc_value_result(p).ok())
                .map(|v| v.to_slt_string())
                .unwrap_or_default(),
            Value::Bool(true),
        ),
        _ => (
            args.get(1)
                .and_then(|p| FilterExpressionPushDownHelper::calc_value_result(p).ok())
                .map(|v| v.to_slt_string())
                .unwrap_or_default(),
            args.get(2)
                .and_then(|p| FilterExpressionPushDownHelper::calc_value_result(p).ok())
                .unwrap_or(Value::Bool(true)),
        ),
    };
    let term =
        FilterExpressionPushDownHelper::build_json_term(&func_name, &path, &value, &mut compare);
    let evaluator = IndexFilterEvaluatorSecondary::make(
        expr.clone(),
        column,
        info.kind,
        true,
        compare,
        Value::Str(term),
    )?;
    Ok(IndexFilterEvaluator::Secondary(evaluator))
}

fn build_evaluator(
    tree: &ExpressionInfoTree<IndexScanInfo>,
    catalog: &IndexCatalog,
) -> Result<IndexFilterEvaluator> {
    match tree.info {
        IndexScanInfo::ValueExpr => {
            let value = FilterExpressionPushDownHelper::calc_value_result(&tree.expr)?;
            Ok(match value {
                Value::Bool(true) => IndexFilterEvaluator::AllTrue,
                Value::Bool(false) => IndexFilterEvaluator::AllFalse,
                _ => {
                    return Err(Error::Invalid(
                        "index filter expression must be boolean".into(),
                    ))
                }
            })
        }
        IndexScanInfo::SecondaryIndexValueCompareExpr
        | IndexScanInfo::ValueSecondaryIndexCompareExpr
        | IndexScanInfo::SecondaryFunctionalIndexValueCompareExpr
        | IndexScanInfo::ValueSecondaryFunctionalIndexCompareExpr => {
            let Some((a, op, b)) = compare_parts(&tree.expr) else {
                return Ok(IndexFilterEvaluator::AllTrue);
            };
            let reverse = matches!(
                tree.info,
                IndexScanInfo::ValueSecondaryIndexCompareExpr
                    | IndexScanInfo::ValueSecondaryFunctionalIndexCompareExpr
            );
            if reverse {
                let reversed = match op {
                    CmpOp::Lt => CmpOp::Gt,
                    CmpOp::Le => CmpOp::Ge,
                    CmpOp::Gt => CmpOp::Lt,
                    CmpOp::Ge => CmpOp::Le,
                    other => other,
                };
                secondary_from_compare(b, reversed, a, catalog)
            } else {
                secondary_from_compare(a, op, b, catalog)
            }
        }
        IndexScanInfo::JsonSecondaryIndexValueCompareExpr => {
            json_compare_evaluator(tree, catalog, false)
        }
        IndexScanInfo::ValueJsonSecondaryIndexCompareExpr => {
            json_compare_evaluator(tree, catalog, true)
        }
        IndexScanInfo::JsonSecondaryIndexFunctionExpr => {
            json_function_evaluator(&tree.expr, catalog)
        }
        IndexScanInfo::FilterFulltextExpr => Ok(IndexFilterEvaluator::Fulltext(Box::new(
            fulltext_evaluator(&tree.expr)
                .ok_or_else(|| Error::Invalid("malformed filter_fulltext expression".into()))?,
        ))),
        IndexScanInfo::AndExpr => {
            let mut candidates = Vec::new();
            for child in &tree.children {
                let evaluator = build_evaluator(child, catalog)?;
                match evaluator {
                    IndexFilterEvaluator::AllFalse => return Ok(IndexFilterEvaluator::AllFalse),
                    IndexFilterEvaluator::AllTrue => {}
                    other => candidates.push(other),
                }
            }
            Ok(match candidates.len() {
                0 => IndexFilterEvaluator::AllTrue,
                1 => candidates.pop().unwrap(),
                _ => {
                    let mut acc = candidates.remove(0);
                    for next in candidates {
                        acc = index_filter_evaluator_build_from_and(vec![acc, next]);
                    }
                    acc
                }
            })
        }
        IndexScanInfo::OrExpr => {
            let mut candidates = Vec::new();
            for child in &tree.children {
                let evaluator = build_evaluator(child, catalog)?;
                match evaluator {
                    IndexFilterEvaluator::AllTrue => return Ok(IndexFilterEvaluator::AllTrue),
                    IndexFilterEvaluator::AllFalse => {}
                    other => candidates.push(other),
                }
            }
            Ok(match candidates.len() {
                0 => IndexFilterEvaluator::AllFalse,
                1 => candidates.pop().unwrap(),
                _ => {
                    let mut acc = candidates.remove(0);
                    for next in candidates {
                        acc = index_filter_evaluator_build_from_or(vec![acc, next]);
                    }
                    acc
                }
            })
        }
        _ => Ok(IndexFilterEvaluator::AllTrue),
    }
}

/// C++ `FilterExpressionPushDown`.
pub struct FilterExpressionPushDown;

impl FilterExpressionPushDown {
    /// C++ `PushDownToIndexScan`.
    pub fn push_down_to_index_scan(
        expression: &Expr,
        catalog: &IndexCatalog,
    ) -> Result<IndexScanFilterExpressionPushDownResult> {
        let tree = ExpressionInfoTree::build(expression, catalog);
        let (index_filter, leftover_filter) = split_tree(&tree);
        let mut index_filter_evaluator = if let Some(filter) = &index_filter {
            let filter_tree = ExpressionInfoTree::build(filter, catalog);
            build_evaluator(&filter_tree, catalog)?
        } else {
            IndexFilterEvaluator::AllTrue
        };
        index_filter_evaluator.optimize_fulltext();
        Ok(IndexScanFilterExpressionPushDownResult {
            index_filter,
            leftover_filter,
            index_filter_evaluator,
        })
    }

    /// C++ `PushDownToFastRoughFilter`: the block/segment skip evaluator.
    pub fn push_down_to_fast_rough_filter(
        expression: &Expr,
        catalog: &IndexCatalog,
    ) -> FastRoughFilterEvaluator {
        build_rough(expression, catalog)
    }

    /// C++ `BuildFilterFulltextExpression`: evaluators for every
    /// `filter_fulltext` predicate left in a projection/leftover expression.
    pub fn build_filter_fulltext_expression(
        expressions: &[Expr],
        catalog: &IndexCatalog,
    ) -> Result<Vec<IndexFilterEvaluator>> {
        let mut out = Vec::new();
        for expression in expressions {
            let tree = ExpressionInfoTree::build(expression, catalog);
            collect_fulltext(&tree, &mut out)?;
        }
        Ok(out)
    }
}

fn collect_fulltext(
    tree: &ExpressionInfoTree<IndexScanInfo>,
    out: &mut Vec<IndexFilterEvaluator>,
) -> Result<()> {
    if tree.info == IndexScanInfo::FilterFulltextExpr {
        out.push(IndexFilterEvaluator::Fulltext(Box::new(
            fulltext_evaluator(&tree.expr)
                .ok_or_else(|| Error::Invalid("malformed filter_fulltext expression".into()))?,
        )));
        return Ok(());
    }
    for child in &tree.children {
        collect_fulltext(child, out)?;
    }
    Ok(())
}

// ---------------------------------------------------------------------------
// IndexScanBuilder
// ---------------------------------------------------------------------------

/// One `Filter(Scan)` that can run through an index.
#[derive(Debug, Clone, PartialEq)]
pub struct IndexScanCandidate {
    pub table: String,
    pub index_filter: Expr,
    pub leftover_filter: Option<Expr>,
    pub evaluator: IndexFilterEvaluator,
}

/// The plan plus the index-scan candidates found in it.
#[derive(Debug, Clone, PartialEq)]
pub struct IndexScanPlan {
    pub plan: LogicalPlan,
    pub candidates: Vec<IndexScanCandidate>,
}

/// C++ `IndexScanBuilder`.
///
/// C++ replaces the `LogicalTableScan` with a `LogicalIndexScan` node holding
/// the evaluator. The Rust plan has no index-scan operator; the candidate is
/// reported alongside the plan, whose filter becomes the leftover predicate.
/// A caller that executes the plan must apply `evaluator` before the leftover
/// filter, exactly as `LogicalIndexScan` does.
pub struct IndexScanBuilder;

impl IndexScanBuilder {
    pub fn build(plan: LogicalPlan, catalog: &IndexCatalog) -> Result<IndexScanPlan> {
        Self::build_with_regex(plan, catalog, None)
    }

    /// C++ `IndexScanBuilder::ApplyToPlan`: `RegexIndexRewrite` first
    /// (`index_scan_builder_impl.cpp:74`), then the filter push-down. The
    /// rewrite only ever conjoins predicates that a matching row must satisfy,
    /// and the original regex stays in the residual filter.
    pub fn build_with_regex(
        plan: LogicalPlan,
        catalog: &IndexCatalog,
        regex: Option<&RegexIndexCatalog>,
    ) -> Result<IndexScanPlan> {
        let mut candidates = Vec::new();
        let plan = rewrite(plan, catalog, regex, &mut candidates)?;
        Ok(IndexScanPlan { plan, candidates })
    }
}

fn rewrite(
    plan: LogicalPlan,
    catalog: &IndexCatalog,
    regex: Option<&RegexIndexCatalog>,
    candidates: &mut Vec<IndexScanCandidate>,
) -> Result<LogicalPlan> {
    match plan {
        LogicalPlan::Filter { input, predicate } => {
            let input = rewrite(*input, catalog, regex, candidates)?;
            // C++ rewrites `regex()` into full-text narrowing before the
            // push-down (`index_scan_builder_impl.cpp:74`).
            let predicate = match regex {
                Some(regex) if !regex.columns.is_empty() => {
                    RegexIndexRewrite::rewrite(&predicate, regex)
                }
                _ => predicate,
            };
            if let LogicalPlan::Scan { table, columns } = &input {
                let pushed =
                    FilterExpressionPushDown::push_down_to_index_scan(&predicate, catalog)?;
                // An evaluator that admits every row cannot replace the
                // predicate: the column has no index (C++ only visits tables
                // that have candidate index columns).
                let answered = pushed.index_filter_evaluator.kind() != EvaluatorType::AllTrue;
                if let Some(index_filter) = pushed.index_filter.as_ref().filter(|_| answered) {
                    candidates.push(IndexScanCandidate {
                        table: table.clone(),
                        index_filter: index_filter.clone(),
                        leftover_filter: pushed.leftover_filter.clone(),
                        evaluator: pushed.index_filter_evaluator,
                    });
                    return Ok(match pushed.leftover_filter {
                        Some(leftover) => LogicalPlan::Filter {
                            input: Box::new(LogicalPlan::Scan {
                                table: table.clone(),
                                columns: columns.clone(),
                            }),
                            predicate: leftover,
                        },
                        None => input,
                    });
                }
            }
            Ok(LogicalPlan::Filter {
                input: Box::new(input),
                predicate,
            })
        }
        LogicalPlan::Projection {
            input,
            exprs,
            names,
        } => Ok(LogicalPlan::Projection {
            input: Box::new(rewrite(*input, catalog, regex, candidates)?),
            exprs,
            names,
        }),
        LogicalPlan::Aggregate {
            input,
            group_keys,
            group_names,
            aggs,
        } => Ok(LogicalPlan::Aggregate {
            input: Box::new(rewrite(*input, catalog, regex, candidates)?),
            group_keys,
            group_names,
            aggs,
        }),
        LogicalPlan::Sort { input, keys } => Ok(LogicalPlan::Sort {
            input: Box::new(rewrite(*input, catalog, regex, candidates)?),
            keys,
        }),
        LogicalPlan::Limit { input, n } => Ok(LogicalPlan::Limit {
            input: Box::new(rewrite(*input, catalog, regex, candidates)?),
            n,
        }),
        LogicalPlan::Offset { input, n } => Ok(LogicalPlan::Offset {
            input: Box::new(rewrite(*input, catalog, regex, candidates)?),
            n,
        }),
        LogicalPlan::Join {
            left,
            right,
            left_key,
            right_key,
            left_width,
        } => Ok(LogicalPlan::Join {
            left: Box::new(rewrite(*left, catalog, regex, candidates)?),
            right: Box::new(rewrite(*right, catalog, regex, candidates)?),
            left_key,
            right_key,
            left_width,
        }),
        other => Ok(other),
    }
}

// ---------------------------------------------------------------------------
// ApplyFastRoughFilter
// ---------------------------------------------------------------------------

/// C++ `FastRoughFilterEvaluatorTag`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FastRoughFilterTag {
    AlwaysTrue,
    AlwaysFalse,
    ProbabilisticDataFilter,
    MinMaxFilter,
    CombineAnd,
    CombineOr,
}

/// A block/segment skip predicate (C++ `FastRoughFilterEvaluator`).
#[derive(Debug, Clone, PartialEq)]
pub enum FastRoughFilterEvaluator {
    AlwaysTrue,
    AlwaysFalse,
    ProbabilisticDataFilter {
        column_id: usize,
        value: Value,
    },
    MinMaxFilter {
        column_id: usize,
        value: Value,
        compare_type: FilterCompareType,
    },
    CombineAnd(Box<FastRoughFilterEvaluator>, Box<FastRoughFilterEvaluator>),
    CombineOr(Box<FastRoughFilterEvaluator>, Box<FastRoughFilterEvaluator>),
}

impl FastRoughFilterEvaluator {
    pub fn tag(&self) -> FastRoughFilterTag {
        match self {
            FastRoughFilterEvaluator::AlwaysTrue => FastRoughFilterTag::AlwaysTrue,
            FastRoughFilterEvaluator::AlwaysFalse => FastRoughFilterTag::AlwaysFalse,
            FastRoughFilterEvaluator::ProbabilisticDataFilter { .. } => {
                FastRoughFilterTag::ProbabilisticDataFilter
            }
            FastRoughFilterEvaluator::MinMaxFilter { .. } => FastRoughFilterTag::MinMaxFilter,
            FastRoughFilterEvaluator::CombineAnd(..) => FastRoughFilterTag::CombineAnd,
            FastRoughFilterEvaluator::CombineOr(..) => FastRoughFilterTag::CombineOr,
        }
    }

    /// C++ `EvaluateInner`: false means the block/segment can be skipped.
    pub fn evaluate(&self, filter: &dyn FastRoughFilter) -> bool {
        match self {
            FastRoughFilterEvaluator::AlwaysTrue => true,
            FastRoughFilterEvaluator::AlwaysFalse => false,
            FastRoughFilterEvaluator::ProbabilisticDataFilter { column_id, value } => {
                filter.may_contain(*column_id, value)
            }
            FastRoughFilterEvaluator::MinMaxFilter {
                column_id,
                value,
                compare_type,
            } => filter.may_in_range(*column_id, value, *compare_type),
            FastRoughFilterEvaluator::CombineAnd(a, b) => a.evaluate(filter) && b.evaluate(filter),
            FastRoughFilterEvaluator::CombineOr(a, b) => a.evaluate(filter) || b.evaluate(filter),
        }
    }
}

/// The block/segment statistics the rough filter consults.
pub trait FastRoughFilter {
    fn may_contain(&self, column_id: usize, value: &Value) -> bool;
    fn may_in_range(
        &self,
        column_id: usize,
        value: &Value,
        compare_type: FilterCompareType,
    ) -> bool;
}

/// One scan and the evaluator attached to it.
#[derive(Debug, Clone, PartialEq)]
pub struct RoughFilterBinding {
    pub table: String,
    pub evaluator: FastRoughFilterEvaluator,
}

/// C++ `ApplyFastRoughFilter`.
pub struct ApplyFastRoughFilter;

impl ApplyFastRoughFilter {
    /// Attaches a rough-filter evaluator to every `Filter(Scan)` in the plan.
    pub fn build(plan: &LogicalPlan, catalog: &IndexCatalog) -> Vec<RoughFilterBinding> {
        let mut out = Vec::new();
        collect_rough(plan, catalog, &mut out);
        out
    }
}

fn collect_rough(plan: &LogicalPlan, catalog: &IndexCatalog, out: &mut Vec<RoughFilterBinding>) {
    match plan {
        LogicalPlan::Filter { input, predicate } => {
            if let LogicalPlan::Scan { table, .. } = &**input {
                out.push(RoughFilterBinding {
                    table: table.clone(),
                    evaluator: build_rough(predicate, catalog),
                });
            }
            collect_rough(input, catalog, out);
        }
        LogicalPlan::Projection { input, .. }
        | LogicalPlan::Aggregate { input, .. }
        | LogicalPlan::Sort { input, .. }
        | LogicalPlan::Limit { input, .. }
        | LogicalPlan::Offset { input, .. } => collect_rough(input, catalog, out),
        LogicalPlan::Join { left, right, .. } => {
            collect_rough(left, catalog, out);
            collect_rough(right, catalog, out);
        }
        _ => {}
    }
}

fn build_rough(expr: &Expr, catalog: &IndexCatalog) -> FastRoughFilterEvaluator {
    match expr {
        Expr::And(a, b) => {
            let left = build_rough(a, catalog);
            if left.tag() == FastRoughFilterTag::AlwaysFalse {
                return left;
            }
            let right = build_rough(b, catalog);
            if right.tag() == FastRoughFilterTag::AlwaysFalse {
                return right;
            }
            if left.tag() == FastRoughFilterTag::AlwaysTrue {
                return right;
            }
            if right.tag() == FastRoughFilterTag::AlwaysTrue {
                return left;
            }
            FastRoughFilterEvaluator::CombineAnd(Box::new(left), Box::new(right))
        }
        Expr::Or(a, b) => {
            let left = build_rough(a, catalog);
            if left.tag() == FastRoughFilterTag::AlwaysTrue {
                return left;
            }
            let right = build_rough(b, catalog);
            if right.tag() == FastRoughFilterTag::AlwaysTrue {
                return right;
            }
            if left.tag() == FastRoughFilterTag::AlwaysFalse {
                return right;
            }
            if right.tag() == FastRoughFilterTag::AlwaysFalse {
                return left;
            }
            FastRoughFilterEvaluator::CombineOr(Box::new(left), Box::new(right))
        }
        Expr::Literal(Value::Bool(false)) => FastRoughFilterEvaluator::AlwaysFalse,
        Expr::Literal(Value::Bool(true)) => FastRoughFilterEvaluator::AlwaysTrue,
        Expr::Cmp(..) | Expr::Func { .. } => {
            let Some((a, op, b)) = compare_parts(expr) else {
                return FastRoughFilterEvaluator::AlwaysTrue;
            };
            let (column, value_expr, op) = match (a, b) {
                (col @ Expr::Column(_), val) => (col, val, op),
                (val, col @ Expr::Column(_)) => (
                    col,
                    val,
                    match op {
                        CmpOp::Lt => CmpOp::Gt,
                        CmpOp::Le => CmpOp::Ge,
                        CmpOp::Gt => CmpOp::Lt,
                        CmpOp::Ge => CmpOp::Le,
                        other => other,
                    },
                ),
                _ => return FastRoughFilterEvaluator::AlwaysTrue,
            };
            let Ok(value) = FilterExpressionPushDownHelper::calc_value_result(value_expr) else {
                return FastRoughFilterEvaluator::AlwaysTrue;
            };
            let Some(compare) = filter_compare_of(op) else {
                return FastRoughFilterEvaluator::AlwaysTrue;
            };
            let Ok(unwound) =
                FilterExpressionPushDownHelper::unwind_cast(column, value, compare, catalog)
            else {
                return FastRoughFilterEvaluator::AlwaysTrue;
            };
            rough_for_unwound(&unwound, catalog)
        }
        _ => FastRoughFilterEvaluator::AlwaysTrue,
    }
}

fn rough_for_unwound(unwound: &UnwindCast, catalog: &IndexCatalog) -> FastRoughFilterEvaluator {
    use FastRoughFilterEvaluator::*;
    match unwound.compare_type {
        FilterCompareType::AlwaysTrue | FilterCompareType::Invalid => AlwaysTrue,
        FilterCompareType::AlwaysFalse => AlwaysFalse,
        FilterCompareType::Equal => {
            let kind = catalog
                .kind(unwound.column_id)
                .unwrap_or(ColumnKind::BigInt);
            match kind {
                ColumnKind::Boolean => ProbabilisticDataFilter {
                    column_id: unwound.column_id,
                    value: unwound.value.clone(),
                },
                k if k.is_float() => CombineAnd(
                    Box::new(MinMaxFilter {
                        column_id: unwound.column_id,
                        value: unwound.value.clone(),
                        compare_type: FilterCompareType::LessEqual,
                    }),
                    Box::new(MinMaxFilter {
                        column_id: unwound.column_id,
                        value: unwound.value.clone(),
                        compare_type: FilterCompareType::GreaterEqual,
                    }),
                ),
                _ => CombineAnd(
                    Box::new(ProbabilisticDataFilter {
                        column_id: unwound.column_id,
                        value: unwound.value.clone(),
                    }),
                    Box::new(CombineAnd(
                        Box::new(MinMaxFilter {
                            column_id: unwound.column_id,
                            value: unwound.value.clone(),
                            compare_type: FilterCompareType::LessEqual,
                        }),
                        Box::new(MinMaxFilter {
                            column_id: unwound.column_id,
                            value: unwound.value.clone(),
                            compare_type: FilterCompareType::GreaterEqual,
                        }),
                    )),
                ),
            }
        }
        FilterCompareType::LessEqual | FilterCompareType::GreaterEqual => MinMaxFilter {
            column_id: unwound.column_id,
            value: unwound.value.clone(),
            compare_type: unwound.compare_type,
        },
        FilterCompareType::Less | FilterCompareType::Greater => AlwaysTrue,
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_expression::Value;
    use infinity_parser::CmpOp;

    fn catalog() -> IndexCatalog {
        IndexCatalog::new(&[
            ("id", ColumnKind::BigInt),
            ("name", ColumnKind::Varchar),
            ("score", ColumnKind::Double),
            ("ok", ColumnKind::Boolean),
            ("small", ColumnKind::Integer),
        ])
        .with_secondary("id", false)
        .with_secondary("name", false)
        .with_secondary("ok", false)
    }

    fn col(i: usize) -> Expr {
        Expr::Column(i)
    }

    fn cmp(a: Expr, op: CmpOp, b: Expr) -> Expr {
        Expr::Cmp(Box::new(a), op, Box::new(b))
    }

    /// C++ `FilterExpressionPushDownHelper::CalcValueResult` and
    /// `SimplifyCompareTypeAndValue`: a strict comparison becomes inclusive
    /// with the adjacent value.
    #[test]
    fn helper_folds_constants_and_rewrites_strict_bounds() {
        let e = Expr::Binary(
            Box::new(Expr::Literal(Value::Int(2))),
            infinity_parser::BinOp::Add,
            Box::new(Expr::Literal(Value::Int(3))),
        );
        assert_eq!(
            FilterExpressionPushDownHelper::calc_value_result(&e).unwrap(),
            Value::Int(5)
        );
        let mut value = Value::Int(10);
        let mut cmp = FilterCompareType::Greater;
        FilterExpressionPushDownHelper::simplify_compare_type_and_value(&mut value, &mut cmp);
        assert_eq!(
            (value, cmp),
            (Value::Int(11), FilterCompareType::GreaterEqual)
        );
        let mut value = Value::Str("abc".into());
        let mut cmp = FilterCompareType::Greater;
        FilterExpressionPushDownHelper::simplify_compare_type_and_value(&mut value, &mut cmp);
        assert_eq!(cmp, FilterCompareType::GreaterEqual);
        assert_eq!(value, Value::Str("abc\0".into()));
    }

    /// C++ `UnwindCast`: `CAST(int_col AS BIGINT) >= 5` narrows back to the
    /// underlying integer column.
    #[test]
    fn helper_unwinds_a_cast_to_the_source_column() {
        let cast = Expr::Func {
            name: "cast".into(),
            args: vec![col(4), Expr::Literal(Value::Str("BigInt".into()))],
        };
        let got = FilterExpressionPushDownHelper::unwind_cast(
            &cast,
            Value::Int(5),
            FilterCompareType::GreaterEqual,
            &catalog(),
        )
        .unwrap();
        assert_eq!(got.column_id, 4);
        assert_eq!(got.value, Value::Int(5));
        assert_eq!(got.functional, None);
        // A value outside the source range is always false.
        let got = FilterExpressionPushDownHelper::unwind_cast(
            &cast,
            Value::Int(i64::MIN),
            FilterCompareType::LessEqual,
            &catalog(),
        )
        .unwrap();
        assert_eq!(got.compare_type, FilterCompareType::AlwaysFalse);
    }

    /// C++ `EncodeJsonInteger` / `EncodeJsonDouble` / `BuildJsonTerm`.
    #[test]
    fn helper_encodes_json_terms_in_sortable_order() {
        let neg = FilterExpressionPushDownHelper::encode_json_integer(-1);
        let zero = FilterExpressionPushDownHelper::encode_json_integer(0);
        assert!(neg < zero);
        assert_eq!(zero, "8000000000000000");
        let neg = FilterExpressionPushDownHelper::encode_json_double(-1.0);
        let pos = FilterExpressionPushDownHelper::encode_json_double(1.0);
        assert!(neg < pos);
        let mut cmp = FilterCompareType::Equal;
        assert_eq!(
            FilterExpressionPushDownHelper::build_json_term(
                "json_extract_int",
                "$.a",
                &Value::Int(1),
                &mut cmp
            ),
            "$.a:i:8000000000000001"
        );
        let mut cmp = FilterCompareType::Greater;
        let term = FilterExpressionPushDownHelper::build_json_term(
            "json_extract_string",
            "$.a",
            &Value::Str("x".into()),
            &mut cmp,
        );
        assert_eq!(cmp, FilterCompareType::GreaterEqual);
        assert_eq!(term, "$.a:s:x\u{1}");
    }

    /// C++ `ExpressionIndexScanInfo::BuildTree`: a column/value comparison on
    /// an indexed column is `kSecondaryIndexValueCompareExpr`; an unindexed
    /// column stays unknown.
    #[test]
    fn expression_info_tree_classifies_secondary_comparisons() {
        let e = cmp(col(0), CmpOp::Gt, Expr::Literal(Value::Int(3)));
        let tree = ExpressionInfoTree::build(&e, &catalog());
        assert_eq!(tree.info, IndexScanInfo::SecondaryIndexValueCompareExpr);
        let e = cmp(Expr::Literal(Value::Int(3)), CmpOp::Lt, col(0));
        let tree = ExpressionInfoTree::build(&e, &catalog());
        assert_eq!(tree.info, IndexScanInfo::ValueSecondaryIndexCompareExpr);
        // Varchar only supports equality.
        let e = cmp(col(1), CmpOp::Gt, Expr::Literal(Value::Str("m".into())));
        let tree = ExpressionInfoTree::build(&e, &catalog());
        assert_eq!(tree.info, IndexScanInfo::UnknownExpr);
        let e = cmp(col(1), CmpOp::Eq, Expr::Literal(Value::Str("m".into())));
        let tree = ExpressionInfoTree::build(&e, &catalog());
        assert_eq!(tree.info, IndexScanInfo::SecondaryIndexValueCompareExpr);
    }

    /// `FilterExpressionPushDown::PushDownToIndexScan`: AND splits into an
    /// index filter and a leftover; the evaluator covers the index part.
    #[test]
    fn pushdown_splits_index_and_leftover_filters() {
        let indexed = cmp(col(0), CmpOp::Gt, Expr::Literal(Value::Int(3)));
        let leftover = Expr::IsNull(Box::new(col(3)), false);
        let expression = Expr::And(Box::new(indexed.clone()), Box::new(leftover.clone()));
        let result =
            FilterExpressionPushDown::push_down_to_index_scan(&expression, &catalog()).unwrap();
        assert_eq!(result.index_filter, Some(indexed));
        assert_eq!(result.leftover_filter, Some(leftover));
        assert_eq!(
            result.index_filter_evaluator.kind(),
            EvaluatorType::SecondaryIndex
        );
    }

    /// C++ `IndexScanBuilder`: `Filter(Scan)` yields a candidate whose evaluator
    /// is `AllTrue` when nothing is indexable.
    #[test]
    fn index_scan_builder_reports_candidates_and_leftover() {
        let scan = LogicalPlan::Scan {
            table: "t".into(),
            columns: vec!["id".into(), "ok".into()],
        };
        let predicate = cmp(col(0), CmpOp::Ge, Expr::Literal(Value::Int(1)));
        let plan = LogicalPlan::Filter {
            input: Box::new(scan.clone()),
            predicate: predicate.clone(),
        };
        let built = IndexScanBuilder::build(plan, &catalog()).unwrap();
        assert_eq!(built.candidates.len(), 1);
        assert_eq!(built.candidates[0].table, "t");
        assert_eq!(built.candidates[0].index_filter, predicate);
        assert_eq!(built.candidates[0].leftover_filter, None);
        assert!(matches!(built.plan, LogicalPlan::Scan { .. }));

        // A non-indexed predicate keeps the filter and reports no candidate.
        let plan = LogicalPlan::Filter {
            input: Box::new(scan),
            predicate: Expr::IsNull(Box::new(col(1)), false),
        };
        let built = IndexScanBuilder::build(plan, &catalog()).unwrap();
        assert!(built.candidates.is_empty());
        assert!(matches!(built.plan, LogicalPlan::Filter { .. }));
    }

    struct RangeIndex {
        rows: Vec<(usize, Value, u32)>,
    }

    impl SecondaryIndexProvider for RangeIndex {
        fn range_rows(&self, column_id: usize, start: &Value, end: &Value) -> Vec<u32> {
            self.rows
                .iter()
                .filter(|(c, v, _)| {
                    *c == column_id
                        && cmp_value(v, start) != Ordering::Less
                        && cmp_value(v, end) != Ordering::Greater
                })
                .map(|(_, _, r)| *r)
                .collect()
        }
    }

    /// C++ `IndexFilterEvaluatorSecondary::Evaluate`: only rows inside the
    /// range survive. `Merge` merges intervals per column.
    #[test]
    fn secondary_evaluator_merges_ranges_and_filters_rows() {
        let mut a = IndexFilterEvaluatorSecondary::make(
            col(0),
            0,
            ColumnKind::BigInt,
            false,
            FilterCompareType::GreaterEqual,
            Value::Int(2),
        )
        .unwrap();
        let b = IndexFilterEvaluatorSecondary::make(
            col(0),
            0,
            ColumnKind::BigInt,
            false,
            FilterCompareType::LessEqual,
            Value::Int(3),
        )
        .unwrap();
        a.merge(&b, EvaluatorType::And).unwrap();
        assert_eq!(a.ranges, vec![(Value::Int(2), Value::Int(3))]);
        let index = RangeIndex {
            rows: vec![
                (0, Value::Int(1), 0),
                (0, Value::Int(2), 1),
                (0, Value::Int(3), 2),
                (0, Value::Int(4), 3),
            ],
        };
        let ctx = IndexEvalContext::new(4).with_secondary(&index);
        let mask = a.evaluate(&ctx).unwrap();
        assert_eq!(mask.count_true(), 2);
        assert!(mask.is_true(1) && mask.is_true(2));
    }

    struct TestFulltext {
        docs: Vec<(&'static str, Vec<u32>)>,
    }

    impl FulltextIndex for TestFulltext {
        fn postings(&self, term: &str) -> Vec<u32> {
            self.docs
                .iter()
                .find(|(t, _)| *t == term)
                .map(|(_, d)| d.clone())
                .unwrap_or_default()
        }
    }

    /// C++ `IndexFilterEvaluatorFulltext::Evaluate`: AND/OR query trees over
    /// the term postings, plus `minimum_should_match` for OR.
    #[test]
    fn fulltext_evaluator_combines_term_postings() {
        let index = TestFulltext {
            docs: vec![("rust", vec![0, 1]), ("db", vec![1, 2])],
        };
        let mut evaluator = IndexFilterEvaluatorFulltext::new(
            Expr::FullText(0, "rust db".into(), String::new()),
            FulltextQuery::And(vec![
                FulltextQuery::Term("rust".into()),
                FulltextQuery::Term("db".into()),
            ]),
        );
        evaluator.optimize_query_tree();
        let ctx = IndexEvalContext::new(3).with_fulltext(&index);
        let mask = evaluator.evaluate(&ctx).unwrap();
        assert_eq!(mask.count_true(), 1);
        assert!(mask.is_true(1));

        let mut or_evaluator = evaluator.clone();
        or_evaluator.query_tree = FulltextQuery::Or(vec![
            FulltextQuery::Term("rust".into()),
            FulltextQuery::Term("db".into()),
        ]);
        or_evaluator.minimum_should_match = 2;
        let mask = or_evaluator.evaluate(&ctx).unwrap();
        assert_eq!(mask.count_true(), 1);
        assert!(mask.is_true(1));
    }

    /// `IndexFilterEvaluatorBuildFromAnd`/`Or`: all-true/all-false children are
    /// eliminated, and same-column ranges merge.
    #[test]
    fn logical_evaluator_builders_simplify_children() {
        let a = IndexFilterEvaluatorSecondary::make(
            col(0),
            0,
            ColumnKind::BigInt,
            false,
            FilterCompareType::GreaterEqual,
            Value::Int(1),
        )
        .unwrap();
        let b = IndexFilterEvaluatorSecondary::make(
            col(0),
            0,
            ColumnKind::BigInt,
            false,
            FilterCompareType::LessEqual,
            Value::Int(5),
        )
        .unwrap();
        let combined = index_filter_evaluator_build_from_and(vec![
            IndexFilterEvaluator::AllTrue,
            IndexFilterEvaluator::Secondary(a),
            IndexFilterEvaluator::Secondary(b),
        ]);
        match combined {
            IndexFilterEvaluator::Secondary(s) => {
                assert_eq!(s.ranges, vec![(Value::Int(1), Value::Int(5))])
            }
            other => panic!("expected one merged secondary evaluator, got {other:?}"),
        }
        assert_eq!(
            index_filter_evaluator_build_from_or(vec![
                IndexFilterEvaluator::AllTrue,
                IndexFilterEvaluator::AllFalse
            ])
            .kind(),
            EvaluatorType::AllTrue
        );
    }

    /// `IndexScanBuilder::build_with_regex`: a `regex()` predicate on a
    /// sparse-gram column is rewritten into full-text narrowing, and the regex
    /// itself stays as the residual filter.
    #[test]
    fn builder_rewrites_regex_into_an_index_filter() {
        use crate::regex_rewrite::RegexIndexCatalog;
        let catalog = IndexCatalog::new(&[("doc", ColumnKind::Varchar)]);
        let regex = RegexIndexCatalog::new().with_column(0, "idx_regex", 3, 12, false);
        let plan = LogicalPlan::Filter {
            input: Box::new(LogicalPlan::Scan {
                table: "t".into(),
                columns: vec!["doc".into()],
            }),
            predicate: Expr::Regex(
                0,
                Box::new(Expr::Literal(Value::Str(
                    "error (reading|writing) file".into(),
                ))),
            ),
        };
        let indexed =
            IndexScanBuilder::build_with_regex(plan.clone(), &catalog, Some(&regex)).unwrap();
        // Without the regex catalog the predicate cannot be narrowed.
        let plain = IndexScanBuilder::build(plan, &catalog).unwrap();
        assert!(plain.candidates.is_empty());
        assert_eq!(indexed.candidates.len(), 1);
        assert!(matches!(indexed.candidates[0].index_filter, Expr::And(..)));
        // The original regex survives as the residual filter.
        assert!(matches!(
            indexed.plan,
            LogicalPlan::Filter {
                predicate: Expr::Regex(..),
                ..
            }
        ));
    }

    /// C++ `ApplyFastRoughFilter`: an equality becomes bloom + min/max, an
    /// ordered comparison becomes a min/max filter.
    #[test]
    fn apply_fast_rough_filter_builds_minmax_evaluators() {
        let scan = LogicalPlan::Scan {
            table: "t".into(),
            columns: vec!["id".into()],
        };
        let eq = LogicalPlan::Filter {
            input: Box::new(scan.clone()),
            predicate: cmp(col(0), CmpOp::Eq, Expr::Literal(Value::Int(2))),
        };
        let bindings = ApplyFastRoughFilter::build(&eq, &catalog());
        assert_eq!(bindings.len(), 1);
        assert_eq!(bindings[0].table, "t");
        assert_eq!(bindings[0].evaluator.tag(), FastRoughFilterTag::CombineAnd);

        let range = LogicalPlan::Filter {
            input: Box::new(scan),
            predicate: cmp(col(0), CmpOp::Ge, Expr::Literal(Value::Int(2))),
        };
        let bindings = ApplyFastRoughFilter::build(&range, &catalog());
        assert_eq!(
            bindings[0].evaluator.tag(),
            FastRoughFilterTag::MinMaxFilter
        );
    }
}
