//! The scalar-function whitelist.
//!
//! C++ binds a call against the catalog's function registry: `FunctionSet::GetFunctionSet`
//! (`src/function/function_set_impl.cpp:27`) looks the name up through
//! `NewCatalog::GetFunctionSetByName` (`src/storage/catalog/new_catalog_static_impl.cpp:1553`),
//! which lower-cases the name and raises `Status::FunctionNotFound` on a miss
//! (`src/planner/expression_binder_impl.cpp:453`). A typo is therefore a bind-time
//! error, never a NULL value.
//!
//! This module is the Rust counterpart: the names below are the ones registered by
//! `BuiltinFunctions` from `src/function/scalar/**` (`NewCatalog::AddFunctionSet`) plus the
//! special functions of `RegisterSpecialFunction`
//! (`src/function/builtin_functions_impl.cpp:199`). [`check_function`] rejects anything
//! else, and [`func`] is the validating constructor for [`Expr::Func`].

use crate::Expr;

/// Scalar functions that [`crate::Expr::eval`] implements, spelled as the C++
/// `FunctionSet` names. Aliases that C++ registers as separate sets (or that the
/// reference parser accepts) are listed alongside their primary spelling.
///
/// `REGEX` is registered because C++ registers it, but the planner turns it into
/// [`crate::Expr::Regex`] so its pattern is matched against the column's text.
pub const SCALAR_FUNCTIONS: &[&str] = &[
    // arithmetic / numeric
    "ABS",
    "CEIL",
    "CEILING",
    "EXP",
    "FLOOR",
    "LN",
    "LOG10",
    "LOG2",
    "POW",
    "POWER",
    "ROUND",
    "SQRT",
    "TRUNC",
    "TRUNCATE",
    // string
    "CHAR_LENGTH",
    "CHAR_POSITION",
    "INSTR",
    "LEN",
    "LENGTH",
    "LOWER",
    "LTRIM",
    "MD5",
    "REGEX",
    "REVERSE",
    "RTRIM",
    "STRPOS",
    "SUBSTR",
    "SUBSTRING",
    "TRIM",
    "UPPER",
    // predicate helpers
    "ISFINITE",
    "ISINF",
    "ISNAN",
    "IS_NOT_NULL",
    "IS_NULL",
    // date / time
    "CENTURY",
    "DATEPART",
    "DATE_PART",
    "DAY",
    "DAYOFMONTH",
    "DAYOFWEEK",
    "DAYOFYEAR",
    "EPOCH",
    "ERA",
    "EXTRACT",
    "HOUR",
    "INTERVAL",
    "MINUTE",
    "MONTH",
    "QUARTER",
    "SECOND",
    "WEEK",
    "WEEKDAY",
    "WEEKOFYEAR",
    "YEAR",
    // json
    "JSON_CONTAINS",
    "JSON_EXISTS_PATH",
    "JSON_EXTRACT",
    "JSON_EXTRACT_BOOL",
    "JSON_EXTRACT_DOUBLE",
    "JSON_EXTRACT_FLOAT",
    "JSON_EXTRACT_INT",
    "JSON_EXTRACT_ISNULL",
    "JSON_EXTRACT_STRING",
    "PARSE_JSON",
    // cast
    "CAST",
    // machine learning: `RegisterFDEFunction`, implemented by `crate::fde`
    "FDE",
];

/// The fields `EXTRACT(<field> FROM x)` may name. The reference parser rewrites the
/// form to `extract_<field>(x)` and only emits `extract_year`, `extract_month`,
/// `extract_day`, `extract_hour`, `extract_minute` and `extract_second`
/// (`src/parser/expression_parser.cpp:2537`); the remaining entries are the fields
/// [`crate::Expr::eval`] understands as date parts.
pub const EXTRACT_FIELDS: &[&str] = &[
    "CENTURY",
    "DAY",
    "DAYOFMONTH",
    "DAYOFWEEK",
    "DAYOFYEAR",
    "EPOCH",
    "ERA",
    "HOUR",
    "MINUTE",
    "MONTH",
    "QUARTER",
    "SECOND",
    "WEEK",
    "WEEKDAY",
    "WEEKOFYEAR",
    "YEAR",
];

/// Names that are not scalar calls: the executor resolves `SUBQUERY`/`EXISTS`
/// before an expression is compiled, and the planner turns the search
/// pseudo-columns and the full-text filters into hidden columns or dedicated
/// [`crate::Expr`] nodes. C++ knows them through `AddSpecialFunction`, so they
/// must not be reported as unknown. The remaining C++ specials
/// (`CREATE_TIMESTAMP`, `DELETE_TIMESTAMP`, the `*_FACTORS` columns) are listed
/// for the same reason even though this planner does not resolve them yet.
pub const SPECIAL_FUNCTIONS: &[&str] = &[
    "CREATE_TIMESTAMP",
    "DELETE_TIMESTAMP",
    "DISTANCE",
    "DISTANCE_FACTORS",
    "EXISTS",
    "FILTER_FULLTEXT",
    "FILTER_TEXT",
    "ROW_ID",
    "SCORE",
    "SCORE_FACTORS",
    "SIMILARITY",
    "SIMILARITY_FACTORS",
    "SUBQUERY",
    // The executor expands `UNNEST(col)` itself: `rewrite_unnest_expr`
    // (infinity-executor/src/lib.rs:1395) maps it to the synthetic column that
    // the expansion appends before the expression is compiled.
    "UNNEST",
];

/// Prefix the reference parser gives the rewritten `EXTRACT` call.
const EXTRACT_PREFIX: &str = "EXTRACT_";

/// True when `name` is a function this crate can compile: a scalar from
/// [`SCALAR_FUNCTIONS`], an `extract_<field>` call, or a name the planner and
/// executor resolve themselves ([`SPECIAL_FUNCTIONS`]). The lookup is
/// case-insensitive, matching `NewCatalog::GetFunctionSetByName`.
pub fn is_known_function(name: &str) -> bool {
    let up = name.to_ascii_uppercase();
    if let Some(field) = up.strip_prefix(EXTRACT_PREFIX) {
        return EXTRACT_FIELDS.contains(&field);
    }
    SCALAR_FUNCTIONS.contains(&up.as_str()) || SPECIAL_FUNCTIONS.contains(&up.as_str())
}

/// Checks that `name` names a function this crate implements, returning the
/// bind-time error the reference reports for an unknown name.
pub fn check_function(name: &str) -> Result<(), String> {
    if is_known_function(name) {
        Ok(())
    } else {
        Err(format!("unknown function '{name}'"))
    }
}

/// Builds a scalar call, rejecting an unknown function name. This is the only
/// constructor callers should use for [`Expr::Func`].
pub fn func(name: &str, args: Vec<Expr>) -> Result<Expr, String> {
    check_function(name)?;
    Ok(Expr::Func {
        name: name.to_string(),
        args,
    })
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::Value;

    fn call(name: &str, args: Vec<Expr>) -> Expr {
        func(name, args).unwrap_or_else(|e| panic!("{name}: {e}"))
    }

    /// A typo is a compile error naming the offending function, not a NULL row.
    #[test]
    fn unknown_function_is_rejected_by_name() {
        let err = func("foobar", vec![Expr::Column(0)]).unwrap_err();
        assert_eq!(err, "unknown function 'foobar'");
        assert_eq!(
            check_function("Foobar").unwrap_err(),
            "unknown function 'Foobar'"
        );
        assert!(matches!(func("extract_weekday_of_month", vec![]), Err(e) if e.contains("extract_weekday_of_month")));
    }

    /// Every scalar the crate implements still evaluates; one representative per family.
    #[test]
    fn implemented_families_still_evaluate() {
        let row = vec![Value::Str("2024-03-05 12:34:56".into())];
        // arithmetic
        assert_eq!(
            call(
                "POW",
                vec![
                    Expr::Literal(Value::Int(2)),
                    Expr::Literal(Value::Int(10))
                ]
            )
            .eval(&row),
            Value::Float(1024.0)
        );
        // string
        assert_eq!(
            call("UPPER", vec![Expr::Literal(Value::Str("abc".into()))]).eval(&row),
            Value::Str("ABC".into())
        );
        // date
        assert_eq!(
            call("YEAR", vec![Expr::Column(0)]).eval(&row),
            Value::Int(2024)
        );
        // json
        assert_eq!(
            call(
                "JSON_EXTRACT",
                vec![
                    Expr::Literal(Value::Str(r#"{"a":{"b":7}}"#.into())),
                    Expr::Literal(Value::Str("$.a.b".into()))
                ]
            )
            .eval(&row),
            Value::Int(7)
        );
        // machine learning: the FDE family is the `fde` module's encoder.
        assert!(is_known_function("fde"));
        let cfg = crate::fde::FixedDimensionalEncodingConfig {
            dimension: 2,
            ..Default::default()
        };
        assert_eq!(
            crate::fde::generate_fixed_dimensional_encoding(&[1.0, 2.0, 3.0, 4.0], &cfg),
            Some(vec![4.0, 6.0])
        );
    }

    /// Lookup is case-insensitive and the aliases C++ registers all resolve.
    #[test]
    fn case_insensitive_and_aliases_resolve() {
        for name in [
            "abs", "Abs", "ABS", // upper case wins in C++ but lookup lower-cases
            "length", "char_length", "CHAR_LENGTH", // one FunctionSet in C++
            "char_position", "strpos", "instr",
            "pow", "power",
            "trunc", "truncate",
            "substring", "substr",
            "datepart", "date_part",
            "weekofyear", "dayofmonth", "dayofweek", "dayofyear", "is_null",
            "json_extract_string", "json_exists_path", "md5", "regex", "fde",
        ] {
            assert!(is_known_function(name), "{name} should resolve");
        }
        // the parser-rewritten EXTRACT form
        assert!(is_known_function("extract_hour"));
        assert!(is_known_function("EXTRACT_SECOND"));
        // special functions resolved by the planner / executor
        assert!(is_known_function("subquery"));
        assert!(is_known_function("Exists"));
        // the remaining names from C++ `RegisterSpecialFunction`
        assert!(is_known_function("create_timestamp"));
        assert!(is_known_function("DELETE_TIMESTAMP"));
        assert!(is_known_function("similarity_factors"));
        // `UNNEST` is expanded by the executor before the call is compiled
        assert!(is_known_function("Unnest"));
    }

    /// The whitelist has no duplicates and every entry resolves case-insensitively.
    #[test]
    fn registry_entries_are_unique_and_known() {
        let mut seen = std::collections::HashSet::new();
        for name in SCALAR_FUNCTIONS.iter().chain(SPECIAL_FUNCTIONS) {
            assert!(seen.insert(name.to_ascii_uppercase()), "duplicate {name}");
            assert!(is_known_function(name));
            assert!(!name.trim().is_empty());
        }
        assert!(SCALAR_FUNCTIONS.len() >= 50, "registry shrank unexpectedly");
    }
}
