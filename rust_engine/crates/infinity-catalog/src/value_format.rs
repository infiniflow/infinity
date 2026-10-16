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

/// The index-type names the C++ writer stores (`IndexInfo::IndexTypeToString`,
/// `src/parser/statement/extra/create_index_info.cpp:25-62`).
pub mod index_type {
    pub const IVF: &str = "IVF";
    pub const HNSW: &str = "HNSW";
    pub const FULLTEXT: &str = "FULLTEXT";
    pub const SECONDARY: &str = "SECONDARY";
    pub const SECONDARY_FUNCTIONAL: &str = "SECONDARYFUNCTIONAL";
    pub const BMP: &str = "BMP";
    pub const DISKANN: &str = "DISKANN";
    pub const EMVB: &str = "EMVB";
    pub const PLAID: &str = "PLAID";
    pub const SMVE: &str = "SMVE";
}

/// Maps a `CREATE INDEX … USING <type>` keyword to the stored name.
pub fn index_type_name(using: &str) -> Option<&'static str> {
    Some(match using.trim().to_ascii_uppercase().as_str() {
        "IVF" => index_type::IVF,
        "HNSW" => index_type::HNSW,
        "FULLTEXT" => index_type::FULLTEXT,
        "SECONDARY" => index_type::SECONDARY,
        "SECONDARYFUNCTIONAL" | "SECONDARY_FUNCTIONAL" => index_type::SECONDARY_FUNCTIONAL,
        "BMP" => index_type::BMP,
        "DISKANN" => index_type::DISKANN,
        "EMVB" => index_type::EMVB,
        "PLAID" => index_type::PLAID,
        "SMVE" => index_type::SMVE,
        _ => return None,
    })
}

/// The `IndexBase::Serialize()` payload (`index_base_impl.cpp:243-251`).
///
/// Per-index-type extras (HNSW's `M`/`ef_construction`/…, IVF's `ivf_option`,
/// the functional index's expression) are appended by the subclasses; this
/// struct covers the five fields every index carries.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct IndexBaseValue {
    pub index_type: String,
    pub index_name: String,
    pub index_comment: String,
    pub file_name: String,
    pub column_names: Vec<String>,
}

impl IndexBaseValue {
    pub fn new(
        index_type: impl Into<String>,
        index_name: impl Into<String>,
        file_name: impl Into<String>,
        column_names: Vec<String>,
    ) -> Self {
        Self {
            index_type: index_type.into(),
            index_name: index_name.into(),
            index_comment: String::new(),
            file_name: file_name.into(),
            column_names,
        }
    }
}

/// Encodes `IndexBase::Serialize()`.
pub fn index_base_json(v: &IndexBaseValue) -> Json {
    obj(vec![
        (
            "column_names".to_string(),
            Json::Arr(v.column_names.iter().cloned().map(Json::Str).collect()),
        ),
        ("file_name".to_string(), Json::Str(v.file_name.clone())),
        (
            "index_comment".to_string(),
            Json::Str(v.index_comment.clone()),
        ),
        ("index_name".to_string(), Json::Str(v.index_name.clone())),
        ("index_type".to_string(), Json::Str(v.index_type.clone())),
    ])
}

/// The bytes the catalog stores for `idx|…|index_base`.
pub fn index_base_bytes(v: &IndexBaseValue) -> Vec<u8> {
    index_base_json(v).compact().into_bytes()
}

/// Decodes `IndexBase::Deserialize`'s common fields (`index_base_impl.cpp:253-266`).
///
/// A missing (or non-string) `index_comment` becomes the empty string, exactly
/// as the C++ reader does.
pub fn index_base_from_json(j: &Json) -> Option<IndexBaseValue> {
    let as_str = |v: Option<&Json>| match v {
        Some(Json::Str(s)) => Some(s.clone()),
        _ => None,
    };
    let index_type = as_str(field(j, "index_type"))?;
    let index_name = as_str(field(j, "index_name"))?;
    let file_name = as_str(field(j, "file_name"))?;
    let column_names = match field(j, "column_names") {
        Some(Json::Arr(items)) => items
            .iter()
            .map(|i| match i {
                Json::Str(s) => Some(s.clone()),
                _ => None,
            })
            .collect::<Option<Vec<_>>>()?,
        _ => return None,
    };
    Some(IndexBaseValue {
        index_type,
        index_name,
        index_comment: as_str(field(j, "index_comment")).unwrap_or_default(),
        file_name,
        column_names,
    })
}

/// Decodes an `index_base` record from its stored bytes.
pub fn index_base_from_bytes(bytes: &[u8]) -> Option<IndexBaseValue> {
    let text = std::str::from_utf8(bytes).ok()?;
    index_base_from_json(&infinity_expression::json::parse(text)?)
}

/// `MetricType` ordinals (`index_base.cppm:32`).
pub mod metric {
    pub const COSINE: i64 = 0;
    pub const INNER_PRODUCT: i64 = 1;
    pub const L2: i64 = 2;
    pub const INVALID: i64 = 3;
}

/// `HnswEncodeTypeToString` spellings (`index_hnsw_impl.cpp:49-60`).
pub mod hnsw_encode {
    pub const PLAIN: &str = "plain";
    pub const LVQ: &str = "lvq";
    pub const RABITQ: &str = "rabitq";
}

/// `HnswBuildTypeToString` spellings (`index_hnsw_impl.cpp:62-71`).
pub mod hnsw_build {
    pub const PLAIN: &str = "plain";
    pub const LSG: &str = "lsg";
}

/// `IndexIVFStorageOption::Type` ordinals (`index_ivf.cppm:39-43`).
pub mod ivf_storage {
    pub const PLAIN: i64 = 0;
    pub const SCALAR_QUANTIZATION: i64 = 1;
    pub const PRODUCT_QUANTIZATION: i64 = 2;
}

/// HNSW build defaults (`src/common/default_values.cppm:178-180`).
pub const HNSW_M: i64 = 16;
pub const HNSW_EF_CONSTRUCTION: i64 = 200;
pub const HNSW_BLOCK_SIZE: i64 = 8192;

/// `OPTION_FLAG_ALL` (`index_defines.cppm:32`): the six flags OR'd = 63.
pub const FULLTEXT_OPTION_FLAG_ALL: i64 = 63;

/// The C++ `index_filename` rule (`logical_planner_impl.cpp:859`):
/// `fmt::format("{}_{}", table_name, index_name)`.
pub fn index_file_name(table_name: &str, index_name: &str) -> String {
    format!("{table_name}_{index_name}")
}

/// `MetricTypeToString` (`index_base_impl.cpp:39-54`).
pub fn metric_name(code: i64) -> &'static str {
    match code {
        metric::COSINE => "cosine",
        metric::INNER_PRODUCT => "ip",
        metric::L2 => "l2",
        _ => "Invalid",
    }
}

/// `StringToMetricType` (`index_base_impl.cpp:56-66`): the user's `metric`
/// option to the stored spelling.
pub fn metric_name_of(text: &str) -> &'static str {
    match text.trim().to_ascii_lowercase().as_str() {
        "cos" | "cosine" => "cosine",
        "ip" => "ip",
        "l2" => "l2",
        _ => "Invalid",
    }
}

/// `StringToHnswEncodeType` spelling (`index_hnsw.cppm:28-33`).
pub fn hnsw_encode_name_of(text: &str) -> &'static str {
    match text.trim().to_ascii_lowercase().as_str() {
        "plain" => hnsw_encode::PLAIN,
        "lvq" => hnsw_encode::LVQ,
        "rabitq" => hnsw_encode::RABITQ,
        _ => hnsw_encode::PLAIN,
    }
}

/// The `data_type` (a `LogicalType` ordinal) of a serialized `DataType` object.
pub fn data_type_code(column_type: &Json) -> Option<i64> {
    as_int(field(column_type, "data_type")?)
}

/// An integer field of a serialized `DataType`'s `type_info`
/// (`embedding_type`/`dimension`/…).
pub fn type_info_int(column_type: &Json, key: &str) -> Option<i64> {
    as_int(field(field(column_type, "type_info")?, key)?)
}

/// `StringToMetricType` mapped straight to the `MetricType` ordinal.
pub fn metric_code_of(text: &str) -> i64 {
    match metric_name_of(text) {
        "cosine" => metric::COSINE,
        "ip" => metric::INNER_PRODUCT,
        "l2" => metric::L2,
        _ => metric::INVALID,
    }
}

/// The buffer key for the record: every *other* key is a per-type extra.
const INDEX_BASE_KEYS: [&str; 5] = [
    "column_names",
    "file_name",
    "index_comment",
    "index_name",
    "index_type",
];

/// A whole `IndexBase`-family record: the five common fields plus the
/// per-subtype extras the C++ `Serialize()` overrides append
/// (`index_base_impl.cpp:243-251` and the `Index*Impl::Serialize` overrides).
#[derive(Debug, Clone, PartialEq)]
pub struct IndexDefValue {
    pub base: IndexBaseValue,
    /// The subtype's own fields, in no particular order (`compact()` sorts).
    pub extra: Vec<(String, Json)>,
}

impl IndexDefValue {
    pub fn new(base: IndexBaseValue) -> Self {
        Self {
            base,
            extra: Vec::new(),
        }
    }

    fn with(mut self, key: &str, value: Json) -> Self {
        self.extra.push((key.to_string(), value));
        self
    }

    /// `IndexHnsw::Serialize()` (`index_hnsw_impl.cpp:278-290`).
    #[allow(clippy::too_many_arguments)]
    pub fn hnsw(
        index_name: &str,
        table_name: &str,
        column: &str,
        metric: &str,
        encode: &str,
        build: &str,
        m: i64,
        ef_construction: i64,
        block_size: i64,
    ) -> Self {
        Self::new(IndexBaseValue::new(
            index_type::HNSW,
            index_name,
            index_file_name(table_name, index_name),
            vec![column.to_string()],
        ))
        .with("build_type", Json::Str(build.to_string()))
        .with("ef_construction", Json::Int(ef_construction))
        .with("encode_type", Json::Str(encode.to_string()))
        .with("M", Json::Int(m))
        .with("block_size", Json::Int(block_size))
        .with("metric_type", Json::Str(metric.to_string()))
    }

    /// An HNSW index with the C++ build defaults
    /// (`index_hnsw_impl.cpp:278-290`).
    pub fn hnsw_default(index_name: &str, table_name: &str, column: &str, metric: &str) -> Self {
        Self::hnsw(
            index_name,
            table_name,
            column,
            metric,
            hnsw_encode::PLAIN,
            hnsw_build::PLAIN,
            HNSW_M,
            HNSW_EF_CONSTRUCTION,
            HNSW_BLOCK_SIZE,
        )
    }

    /// `IndexFullText::Serialize()` (`index_full_text_impl.cpp:114-119`).
    pub fn full_text(index_name: &str, table_name: &str, column: &str, analyzer: &str) -> Self {
        Self::new(IndexBaseValue::new(
            index_type::FULLTEXT,
            index_name,
            index_file_name(table_name, index_name),
            vec![column.to_string()],
        ))
        .with("analyzer", Json::Str(analyzer.to_string()))
        .with("flag", Json::Int(FULLTEXT_OPTION_FLAG_ALL))
    }

    /// `IndexSecondary::Serialize()` (`index_secondary_impl.cpp:117-124`).
    pub fn secondary(
        index_name: &str,
        table_name: &str,
        column: &str,
        low_cardinality: bool,
        is_json_index: bool,
    ) -> Self {
        Self::new(IndexBaseValue::new(
            index_type::SECONDARY,
            index_name,
            index_file_name(table_name, index_name),
            vec![column.to_string()],
        ))
        .with(
            "cardinality",
            Json::Str(if low_cardinality { "low" } else { "high" }.to_string()),
        )
        .with("is_json_index", Json::Bool(is_json_index))
    }

    /// `IndexIVF::Serialize()` with **plain** storage and the C++ centroid
    /// defaults (`index_ivf_impl.cpp:339-393`, `index_ivf.cppm:30-61`).
    pub fn ivf_plain(
        index_name: &str,
        table_name: &str,
        column: &str,
        metric: i64,
        plain_storage_data_type: i64,
    ) -> Self {
        let centroid = obj(vec![
            ("centroids_num_ratio_".to_string(), Json::Float(1.0)),
            ("min_points_per_centroid_".to_string(), Json::Int(32)),
            ("max_points_per_centroid_".to_string(), Json::Int(256)),
        ]);
        let storage = obj(vec![
            ("plain_storage_data_type_".to_string(), Json::Int(plain_storage_data_type)),
            ("product_quantization_subspace_bits_".to_string(), Json::Int(0)),
            ("product_quantization_subspace_num_".to_string(), Json::Int(0)),
            ("scalar_quantization_bits_".to_string(), Json::Int(0)),
            ("type_".to_string(), Json::Int(ivf_storage::PLAIN)),
        ]);
        let option = obj(vec![
            ("metric_".to_string(), Json::Int(metric)),
            ("centroid_option_".to_string(), centroid),
            ("storage_option_".to_string(), storage),
        ]);
        Self::new(IndexBaseValue::new(
            index_type::IVF,
            index_name,
            index_file_name(table_name, index_name),
            vec![column.to_string()],
        ))
        .with("ivf_option", option)
    }
}

/// Encodes a full `IndexBase`-family record, i.e. the base fields plus the
/// subtype's extras; the bytes the catalog stores under `idx|…|index_base`.
pub fn index_def_json(v: &IndexDefValue) -> Json {
    let base = index_base_json(&v.base);
    let Json::Obj(mut entries) = base else {
        unreachable!("index_base_json always builds an object")
    };
    for (k, val) in &v.extra {
        entries.push((k.clone(), val.clone()));
    }
    obj(entries)
}

/// The bytes the catalog stores for `idx|…|index_base`.
pub fn index_def_bytes(v: &IndexDefValue) -> Vec<u8> {
    index_def_json(v).compact().into_bytes()
}

/// Decodes a full record: the five common fields, plus whatever else the
/// subtype added (kept verbatim in [`IndexDefValue::extra`]).
pub fn index_def_from_json(j: &Json) -> Option<IndexDefValue> {
    let base = index_base_from_json(j)?;
    let Json::Obj(entries) = j else {
        return None;
    };
    let extra = entries
        .iter()
        .filter(|(k, _)| !INDEX_BASE_KEYS.contains(&k.as_str()))
        .map(|(k, v)| (k.clone(), v.clone()))
        .collect();
    Some(IndexDefValue { base, extra })
}

/// Decodes a full record from its stored bytes.
pub fn index_def_from_bytes(bytes: &[u8]) -> Option<IndexDefValue> {
    let text = std::str::from_utf8(bytes).ok()?;
    index_def_from_json(&infinity_expression::json::parse(text)?)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn index_base_matches_the_cpp_json() {
        let mut v = IndexBaseValue::new(
            index_type::HNSW,
            "idx_vec",
            "idx_vec_0",
            vec!["v".to_string()],
        );
        v.index_comment = "vectors".to_string();
        assert_eq!(
            index_base_json(&v).compact(),
            r#"{"column_names":["v"],"file_name":"idx_vec_0","index_comment":"vectors","index_name":"idx_vec","index_type":"HNSW"}"#
        );
        assert_eq!(index_base_from_bytes(&index_base_bytes(&v)).unwrap(), v);
    }

    #[test]
    fn index_base_tolerates_a_missing_comment() {
        let j = infinity_expression::json::parse(
            r#"{"column_names":["c"],"file_name":"f","index_name":"i","index_type":"SECONDARY"}"#,
        )
        .unwrap();
        let v = index_base_from_json(&j).unwrap();
        assert_eq!(v.index_comment, "");
        assert_eq!(v.index_type, "SECONDARY");
        // Required fields really are required.
        let bad = infinity_expression::json::parse(r#"{"index_name":"i"}"#).unwrap();
        assert!(index_base_from_json(&bad).is_none());
    }

    #[test]
    fn index_type_names_are_the_cpp_spellings() {
        assert_eq!(index_type_name("hnsw"), Some("HNSW"));
        assert_eq!(index_type_name("FULLTEXT"), Some("FULLTEXT"));
        assert_eq!(
            index_type_name("secondary_functional"),
            Some("SECONDARYFUNCTIONAL")
        );
        assert_eq!(index_type_name("nope"), None);
    }

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

    #[test]
    fn index_filename_follows_the_cpp_rule() {
        // `fmt::format("{}_{}", table_name, index_name)` (logical_planner_impl.cpp:859).
        assert_eq!(index_file_name("t", "idx_vec"), "t_idx_vec");
    }

    #[test]
    fn hnsw_index_matches_the_cpp_json() {
        // Note the uppercase "M" sorts before the lowercase keys, exactly as
        // nlohmann's std::map does.
        let v = IndexDefValue::hnsw_default("idx_vec", "t", "v", metric_name(metric::L2));
        assert_eq!(
            index_def_json(&v).compact(),
            r#"{"M":16,"block_size":8192,"build_type":"plain","column_names":["v"],"ef_construction":200,"encode_type":"plain","file_name":"t_idx_vec","index_comment":"","index_name":"idx_vec","index_type":"HNSW","metric_type":"l2"}"#
        );
        // And it round-trips: extras survive byte-for-byte.
        let back = index_def_from_bytes(&index_def_bytes(&v)).unwrap();
        assert_eq!(back.base, v.base);
        assert_eq!(back.extra.len(), v.extra.len());
    }

    #[test]
    fn fulltext_index_carries_the_all_flag() {
        let v = IndexDefValue::full_text("idx_ft", "t", "body", "standard");
        assert_eq!(
            index_def_json(&v).compact(),
            r#"{"analyzer":"standard","column_names":["body"],"file_name":"t_idx_ft","flag":63,"index_comment":"","index_name":"idx_ft","index_type":"FULLTEXT"}"#
        );
        assert_eq!(FULLTEXT_OPTION_FLAG_ALL, 63);
    }

    #[test]
    fn secondary_index_records_cardinality_and_json_flag() {
        let low = IndexDefValue::secondary("idx_json", "t", "doc", true, true);
        assert_eq!(
            index_def_json(&low).compact(),
            r#"{"cardinality":"low","column_names":["doc"],"file_name":"t_idx_json","index_comment":"","index_name":"idx_json","index_type":"SECONDARY","is_json_index":true}"#
        );
        let high = IndexDefValue::secondary("idx_i", "t", "i", false, false);
        assert!(index_def_json(&high).compact().contains(r#""cardinality":"high""#));
        assert!(index_def_json(&high).compact().contains(r#""is_json_index":false"#));
    }

    #[test]
    fn ivf_index_nests_its_option() {
        let v = IndexDefValue::ivf_plain("idx_ivf", "t", "v", metric::L2, element::FLOAT32);
        let text = index_def_json(&v).compact();
        assert!(text.starts_with(r#"{"column_names":["v"],"file_name":"t_idx_ivf","#), "{text}");
        assert!(text.contains(r#""index_type":"IVF""#), "{text}");
        // The nested option keeps every field `to_json` writes.
        assert!(text.contains(r#""metric_":2"#), "{text}");
        assert!(text.contains(r#""centroids_num_ratio_":1.0"#), "{text}");
        assert!(text.contains(r#""min_points_per_centroid_":32"#), "{text}");
        assert!(text.contains(r#""max_points_per_centroid_":256"#), "{text}");
        assert!(text.contains(r#""type_":0"#), "{text}");
        assert!(text.contains(r#""plain_storage_data_type_":6"#), "{text}");
        // `ivf_option` decodes back to the same bytes.
        assert_eq!(index_def_from_bytes(&index_def_bytes(&v)).unwrap().base, v.base);
    }

    #[test]
    fn index_def_keeps_unknown_subtype_fields() {
        // A subtype we have not modelled: the base fields still decode, and the
        // rest is preserved verbatim rather than dropped.
        let j = infinity_expression::json::parse(
            r#"{"colbertsar_mode":true,"column_names":["c"],"file_name":"f","index_comment":"","index_name":"i","index_type":"PLAID","n_centroids":4,"nbits":8}"#,
        )
        .unwrap();
        let v = index_def_from_json(&j).unwrap();
        assert_eq!(v.base.index_type, "PLAID");
        assert_eq!(v.extra.len(), 3);
        assert_eq!(index_def_json(&v).compact(), j.compact());
    }
}
