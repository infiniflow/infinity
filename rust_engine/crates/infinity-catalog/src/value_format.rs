//! C++-compatible **catalog value encoding** (RUST_REWRITE_PLAN.md §2.6.1
//! item 4, task #69).
//!
//! The C++ catalog stores JSON under the keys of `catalog_format` (§8.2); this
//! module reproduces the serializers that produce those values, frozen in
//! `rust_engine/docs/CPP_FORMAT_SPEC.md` §8.6:
//!
//!   * `ColumnDef::ToJson()`      (`src/parser/definition/column_def.cpp:228-248`)
//!   * `DataType::Serialize()`    (`src/parser/type/data_type.cpp:427-435`)
//!   * the per-`type_info` shapes (`data_type.cpp:437-476`)
//!
//! Byte-exactness is possible because both `nlohmann::json` (a `std::map`) and
//! this crate's [`Json::compact`] emit object keys in sorted order.

use infinity_expression::json::Json;

/// `LogicalType` ordinals (`src/parser/type/logical_type.h`).
pub mod logical {
    pub const BOOLEAN: i64 = 0;
    pub const TINYINT: i64 = 1;
    pub const SMALLINT: i64 = 2;
    pub const INTEGER: i64 = 3;
    pub const BIGINT: i64 = 4;
    pub const HUGEINT: i64 = 5;
    pub const DECIMAL: i64 = 6;
    pub const FLOAT: i64 = 7;
    pub const DOUBLE: i64 = 8;
    pub const FLOAT16: i64 = 9;
    pub const BFLOAT16: i64 = 10;
    pub const VARCHAR: i64 = 11;
    pub const EMBEDDING: i64 = 12;
    pub const TENSOR: i64 = 13;
    pub const TENSORARRAY: i64 = 14;
    pub const SPARSE: i64 = 15;
    pub const MULTIVECTOR: i64 = 16;
    pub const DATE: i64 = 17;
    pub const TIME: i64 = 18;
    pub const DATETIME: i64 = 19;
    pub const TIMESTAMP: i64 = 20;
    pub const INTERVAL: i64 = 21;
    pub const ARRAY: i64 = 22;
    pub const JSON: i64 = 23;
}

/// `EmbeddingDataType` ordinals, shared by the embedding/tensor `type_info`.
pub mod element {
    pub const BIT: i64 = 0;
    pub const UINT8: i64 = 1;
    pub const INT8: i64 = 2;
    pub const INT16: i64 = 3;
    pub const INT32: i64 = 4;
    pub const INT64: i64 = 5;
    pub const FLOAT32: i64 = 6;
    pub const FLOAT64: i64 = 7;
    pub const FLOAT16: i64 = 8;
    pub const BFLOAT16: i64 = 9;
}

fn obj(entries: Vec<(String, Json)>) -> Json {
    Json::Obj(entries)
}

/// The `type_info` of an embedding/tensor/… column:
/// `{"dimension": <int>, "embedding_type": <int>}` (`data_type.cpp:459-463`).
pub fn embedding_type_info(embedding_type: i64, dimension: i64) -> Json {
    obj(vec![
        ("dimension".to_string(), Json::Int(dimension)),
        ("embedding_type".to_string(), Json::Int(embedding_type)),
    ])
}

/// The `type_info` of a decimal column: `{"precision": <int>, "scale": <int>}`.
pub fn decimal_type_info(precision: i64, scale: i64) -> Json {
    obj(vec![
        ("precision".to_string(), Json::Int(precision)),
        ("scale".to_string(), Json::Int(scale)),
    ])
}

/// The `type_info` of an array column: the **element's** `DataType` object
/// (`data_type.cpp:439-443` recurses straight into `DataType::Deserialize`).
pub fn array_type_info(element: Json) -> Json {
    element
}

/// `DataType::Serialize()`: `{"data_type": <LogicalType>, "type_info"?: …}`.
pub fn data_type_json(logic_type: i64, type_info: Option<Json>) -> Json {
    let mut entries = vec![("data_type".to_string(), Json::Int(logic_type))];
    if let Some(ti) = type_info {
        entries.push(("type_info".to_string(), ti));
    }
    obj(entries)
}

/// A column's `ColumnDef::ToJson()` payload.
#[derive(Debug, Clone, PartialEq)]
pub struct ColumnDefValue {
    /// `DataType::Serialize()` (already a `Json::Obj`).
    pub column_type: Json,
    pub column_id: i64,
    pub column_name: String,
    /// `ConstraintType` ordinals; omitted from the JSON when empty.
    pub constraints: Vec<i64>,
    /// Omitted when empty.
    pub column_comment: Option<String>,
    /// `ConstantExpr::Serialize()`; omitted when the column has no default.
    pub default: Option<Json>,
}

impl ColumnDefValue {
    /// A column with just a type, id and name (the common case).
    pub fn new(column_type: Json, column_id: i64, column_name: impl Into<String>) -> Self {
        Self {
            column_type,
            column_id,
            column_name: column_name.into(),
            constraints: Vec::new(),
            column_comment: None,
            default: None,
        }
    }
}

/// Encodes `ColumnDef::ToJson()` (`column_def.cpp:228-248`).
pub fn column_def_json(v: &ColumnDefValue) -> Json {
    let mut entries = vec![
        ("column_type".to_string(), v.column_type.clone()),
        ("column_id".to_string(), Json::Int(v.column_id)),
        ("column_name".to_string(), Json::Str(v.column_name.clone())),
    ];
    if !v.constraints.is_empty() {
        entries.push((
            "constraints".to_string(),
            Json::Arr(v.constraints.iter().map(|c| Json::Int(*c)).collect()),
        ));
    }
    if let Some(comment) = &v.column_comment {
        if !comment.is_empty() {
            entries.push(("column_comment".to_string(), Json::Str(comment.clone())));
        }
    }
    if let Some(default) = &v.default {
        entries.push(("default".to_string(), default.clone()));
    }
    obj(entries)
}

/// The bytes the catalog stores for a column record (`ToJson().dump()`).
pub fn column_def_bytes(v: &ColumnDefValue) -> Vec<u8> {
    column_def_json(v).compact().into_bytes()
}

fn as_int(j: &Json) -> Option<i64> {
    match j {
        Json::Int(i) => Some(*i),
        Json::Float(f) if f.fract() == 0.0 => Some(*f as i64),
        _ => None,
    }
}

fn field<'a>(j: &'a Json, name: &str) -> Option<&'a Json> {
    match j {
        Json::Obj(entries) => entries.iter().find(|(k, _)| k == name).map(|(_, v)| v),
        _ => None,
    }
}

/// Decodes a column record, mirroring `ColumnDef::FromJson`
/// (`column_def.cpp:250-280`). Returns `None` when a required field is missing.
pub fn column_def_from_json(j: &Json) -> Option<ColumnDefValue> {
    let column_type = field(j, "column_type")?.clone();
    let column_id = as_int(field(j, "column_id")?)?;
    let column_name = match field(j, "column_name")? {
        Json::Str(s) => s.clone(),
        _ => return None,
    };
    let constraints = match field(j, "constraints") {
        Some(Json::Arr(items)) => items.iter().filter_map(as_int).collect(),
        _ => Vec::new(),
    };
    let column_comment = match field(j, "column_comment") {
        Some(Json::Str(s)) if !s.is_empty() => Some(s.clone()),
        _ => None,
    };
    Some(ColumnDefValue {
        column_type,
        column_id,
        column_name,
        constraints,
        column_comment,
        default: field(j, "default").cloned(),
    })
}

/// Decodes a `ColumnDef` record from its stored bytes.
pub fn column_def_from_bytes(bytes: &[u8]) -> Option<ColumnDefValue> {
    let text = std::str::from_utf8(bytes).ok()?;
    column_def_from_json(&infinity_expression::json::parse(text)?)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn plain_integer_column_matches_the_cpp_json() {
        let v = ColumnDefValue::new(data_type_json(logical::INTEGER, None), 0, "i");
        // Keys sorted, no spaces - exactly what nlohmann's dump() writes.
        assert_eq!(
            column_def_json(&v).compact(),
            r#"{"column_id":0,"column_name":"i","column_type":{"data_type":3}}"#
        );
    }

    #[test]
    fn embedding_column_carries_its_type_info() {
        let ty = data_type_json(
            logical::EMBEDDING,
            Some(embedding_type_info(element::FLOAT32, 4)),
        );
        let v = ColumnDefValue::new(ty, 2, "vec");
        assert_eq!(
            column_def_json(&v).compact(),
            r#"{"column_id":2,"column_name":"vec","column_type":{"data_type":12,"type_info":{"dimension":4,"embedding_type":6}}}"#
        );
    }

    #[test]
    fn array_column_nests_the_element_type() {
        // `type_info` of an ARRAY column is the element's whole DataType object.
        let ty = data_type_json(
            logical::ARRAY,
            Some(array_type_info(data_type_json(logical::INTEGER, None))),
        );
        assert_eq!(
            ty.compact(),
            r#"{"data_type":22,"type_info":{"data_type":3}}"#
        );
    }

    #[test]
    fn optional_fields_appear_only_when_present() {
        let mut v = ColumnDefValue::new(data_type_json(logical::VARCHAR, None), 1, "s");
        v.constraints = vec![1, 2];
        v.column_comment = Some("note".to_string());
        v.default = Some(Json::Str("x".to_string()));
        let text = column_def_json(&v).compact();
        assert_eq!(
            text,
            r#"{"column_comment":"note","column_id":1,"column_name":"s","column_type":{"data_type":11},"constraints":[1,2],"default":"x"}"#
        );
        // An empty comment is omitted, like the C++ writer does.
        v.column_comment = Some(String::new());
        assert!(!column_def_json(&v).compact().contains("column_comment"));
    }

    #[test]
    fn column_def_roundtrip() {
        let mut v = ColumnDefValue::new(
            data_type_json(logical::DECIMAL, Some(decimal_type_info(18, 2))),
            7,
            "amount",
        );
        v.constraints = vec![0];
        v.column_comment = Some("money".to_string());
        let bytes = column_def_bytes(&v);
        let got = column_def_from_bytes(&bytes).unwrap();
        assert_eq!(got, v);
        // And the JSON parses back field-by-field.
        let back = column_def_from_json(&column_def_json(&v)).unwrap();
        assert_eq!(back, v);
    }

    #[test]
    fn column_def_rejects_missing_required_fields() {
        let bad = infinity_expression::json::parse(r#"{"column_name":"x"}"#).unwrap();
        assert!(column_def_from_json(&bad).is_none());
        assert!(column_def_from_bytes(b"not json").is_none());
    }
}
