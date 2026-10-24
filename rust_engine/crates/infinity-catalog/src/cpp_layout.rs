//! Writing the catalog in the **C++ layout**: `catalog_format` keys (§8.2) with
//! `value_format` column records (§8.6), so a directory written here is what the
//! C++ engine's catalog expects to read (RUST_REWRITE_PLAN.md §2.6.1 item 4).
//!
//! This is additive: the engine's own catalog storage is untouched, and
//! [`Catalog::write_cpp_layout`] / [`read_cpp_layout`] let us prove the layout
//! round-trips on a real catalog. The C++ id counters are written too: the
//! database counter as the `next_database_id` system tag, the table counter as
//! the **per-database** `db|{db_id}|next_table_id` tag (the reference has no
//! top-level `next_table_id`).

use infinity_common::{Error, Result, TxnTimeStamp};
use infinity_expression::json::Json;
use infinity_storage::{catalog_format as key, MetaStore};

use crate::value_format::{
    self, array_type_info, data_type_json, embedding_type_info, logical, ColumnDefValue,
    IndexDefValue,
};
use crate::{split_sparse_store_suffix, Catalog, IndexMeta, SparseStore, TableMeta};

/// One table as read back from a C++-layout catalog.
#[derive(Debug, Clone, PartialEq)]
pub struct CppTable {
    pub db_name: String,
    pub db_id: u64,
    pub table_name: String,
    pub table_id: u64,
    pub columns: Vec<ColumnDefValue>,
    /// The `next_column_id` tag the C++ writer keeps per table.
    pub next_column_id: u64,
}

/// One block of a flushed segment, as the reference records it under
/// `catalog|blk|{db_id}|{table_id}|{segment_id}|{block_id}`.
///
/// `commit_ts` is the value of that record — the ts of the append that opened
/// the block (`SegmentMeta::AddBlockWithID`); the reference keeps a block when
/// it is `<= begin_ts` (`GetTableSegmentBlocks`,
/// `src/storage/catalog/kv_utility_impl.cpp:100`).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CppBlock {
    pub block_id: u16,
    pub commit_ts: u64,
}

/// One segment of a flushed mirror: `catalog|seg|…` plus its blocks.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CppSegment {
    pub segment_id: u32,
    /// Value of the `catalog|seg|…` record (the reference's create timestamp).
    pub create_ts: u64,
    /// Commit ts of the first delete that touched this segment
    /// (`SegmentMeta::CommitFirstDeleteTS`), or [`UNCOMMIT_TS`] when none did.
    /// A reader whose `begin_ts` is below it may skip the block's delete
    /// flags, so an unset value would resurrect deleted rows.
    pub first_delete_ts: u64,
    pub blocks: Vec<CppBlock>,
}

/// A table's segment layer, keyed by the **decimal** ids the C++ keys use.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CppSegmentSet {
    pub db_id: String,
    pub table_id: String,
    pub segments: Vec<CppSegment>,
}

/// One sealed index chunk, as `ChunkIndexMetaInfo` records it
/// (`src/storage/catalog/meta/chunk_index_meta_impl.cpp:47`).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CppIndexChunk {
    pub chunk_id: u32,
    /// Full-text chunks name their files after it; the other workers derive
    /// `chunk_{id}.idx` from the chunk id itself.
    pub base_name: String,
    pub base_row_id: u64,
    pub row_count: u64,
    pub term_count: u64,
    /// Bytes the index occupies on disk (the HNSW/BMP/IVF workers size their
    /// buffer with it).
    pub index_size: u64,
    /// Commit ts of the transaction that sealed the chunk, written as the
    /// `idx_chunk|…|{chunk_id}` value (`SegmentIndexMeta::AddChunkIndexID1`,
    /// `segment_index_meta_impl.cpp:129`). `0` means "no transaction to name" —
    /// the reader then sees the chunk from any `begin_ts`.
    pub commit_ts: u64,
}

/// The sealed chunks of one index over one segment.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CppIndexSegment {
    pub index_id: u64,
    pub segment_id: u32,
    pub chunks: Vec<CppIndexChunk>,
}

/// The timestamp a mirror record falls back to when it has no transaction of
/// its own to name.
///
/// Every ts the port can attribute — a block's append, a segment's creation,
/// an index's pairing with a segment, a catalog record — is now written with
/// the transaction's real commit ts. What is left is `idx_chunk|…`'s value, the
/// commit ts of the reference's per-index memory dump: the reference schedules
/// those dumps on background workers, so the value is the schedule, not the
/// data. 0 is visible to every reader (`GetTableSegments` /
/// `GetTableSegmentBlocks` hide a ts greater than `begin_ts`, and even a reader
/// that starts from an empty WAL begins at ts 1).
pub const CPP_MIRROR_TS: TxnTimeStamp = 0;

/// C++ `UNCOMMIT_TS` (`src/common/default_values.cppm:83`): the value a segment
/// carries until something is deleted from it.
pub const UNCOMMIT_TS: TxnTimeStamp = u64::MAX;

/// Maps a declared type text to the C++ `DataType::Serialize()` object.
///
/// A trailing `WITH (SORTED)` on a `SPARSE` type text selects the C++
/// `SparseStoreType::kSorted`; without it the column keeps `kSort`.
/// Returns `None` for shapes this mapping does not cover yet.
pub fn data_type_of_text(type_text: &str) -> Option<Json> {
    let (type_text, store) = split_sparse_store_suffix(type_text);
    data_type_of_text_with_store(type_text, store)
}

/// [`data_type_of_text`] with an explicit sparse storage hint.
pub fn data_type_of_text_with_store(type_text: &str, store: Option<SparseStore>) -> Option<Json> {
    let up = type_text.trim().to_ascii_uppercase();
    let up = expand_declaration(&up);
    let up = up.as_str();
    let base = up.split('(').next().unwrap_or("").trim();
    let plain = |lt: i64| Some(data_type_json(lt, None));
    match base {
        "BOOL" | "BOOLEAN" => plain(logical::BOOLEAN),
        "TINYINT" | "INT8" => plain(logical::TINYINT),
        "SMALLINT" | "INT16" => plain(logical::SMALLINT),
        "INT" | "INTEGER" | "INT32" => plain(logical::INTEGER),
        "BIGINT" | "INT64" => plain(logical::BIGINT),
        "HUGEINT" | "INT128" => plain(logical::HUGEINT),
        "FLOAT" | "REAL" | "FLOAT32" => plain(logical::FLOAT),
        "DOUBLE" | "FLOAT64" => plain(logical::DOUBLE),
        "FLOAT16" => plain(logical::FLOAT16),
        "BFLOAT16" => plain(logical::BFLOAT16),
        "VARCHAR" | "TEXT" | "STRING" => plain(logical::VARCHAR),
        "JSON" => plain(logical::JSON),
        "DATE" => plain(logical::DATE),
        "TIME" => plain(logical::TIME),
        "DATETIME" => plain(logical::DATETIME),
        "TIMESTAMP" => plain(logical::TIMESTAMP),
        "UUID" => plain(logical::UUID),
        "DECIMAL" => {
            // `DECIMAL(p,s)`; the engine's type text carries the parameters.
            let inner = up.split_once('(')?.1.trim_end_matches(')');
            let mut parts = inner.split(',');
            let p: i64 = parts.next()?.trim().parse().ok()?;
            let s: i64 = parts.next()?.trim().parse().ok()?;
            Some(data_type_json(
                logical::DECIMAL,
                Some(value_format::decimal_type_info(p, s)),
            ))
        }
        "EMBEDDING" | "VECTOR" | "TENSOR" | "TENSORARRAY" | "MULTIVECTOR" => {
            let lt = match base {
                "EMBEDDING" | "VECTOR" => logical::EMBEDDING,
                "TENSOR" => logical::TENSOR,
                "TENSORARRAY" => logical::TENSORARRAY,
                _ => logical::MULTIVECTOR,
            };
            let inner = up.split_once('(')?.1.trim_end_matches(')');
            let mut parts = inner.split(',');
            let elem = parts.next()?.trim().to_string();
            let dim: i64 = parts.next()?.trim().parse().ok()?;
            Some(data_type_json(
                lt,
                Some(embedding_type_info(element_code(&elem)?, dim)),
            ))
        }
        "ARRAY" => {
            let inner = up
                .split_once('(')?
                .1
                .trim_end_matches(')')
                .trim()
                .to_string();
            let element = data_type_of_text(&inner)?;
            Some(data_type_json(
                logical::ARRAY,
                Some(array_type_info(element)),
            ))
        }
        "SPARSE" => {
            // `SPARSE(<elem>,<bound>)` or `SPARSE(<elem>,<index>,<bound>)` ->
            // `SparseInfo::Serialize()` (`sparse_info.cpp:103-110`,
            // `GetIndexType` `:44-58`). The three-component form carries the
            // index width the column was declared with; without it the width is
            // derived from the bound.
            let inner = up.split_once('(')?.1.trim_end_matches(')');
            let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
            let elem = (*parts.first()?).to_string();
            let (explicit_index, bound) = if parts.len() >= 3 {
                (Some(parts[1]), parts[2])
            } else {
                (None, *parts.get(1)?)
            };
            let dim: i64 = bound.parse().ok()?;
            let index_type = match explicit_index.and_then(index_element_code) {
                Some(code) => code,
                None => value_format::sparse_index_type(dim),
            };
            Some(data_type_json(
                logical::SPARSE,
                Some(value_format::sparse_type_info(
                    element_code(&elem)?,
                    index_type,
                    dim,
                    store.unwrap_or(SparseStore::Sort).as_i64(),
                )),
            ))
        }
        _ => None,
    }
}

/// An `int8`/`int16`/`int32`/`int64` index name -> its `EmbeddingDataType`
/// ordinal, for the three-component `SPARSE(elem, index, bound)` spelling.
fn index_element_code(name: &str) -> Option<i64> {
    Some(match name {
        "INT8" | "TINYINT" => value_format::element::INT8,
        "INT16" | "SMALLINT" => value_format::element::INT16,
        "INT" | "INTEGER" | "INT32" => value_format::element::INT32,
        "INT64" | "BIGINT" => value_format::element::INT64,
        _ => return None,
    })
}

/// Rewrites the SDK's comma declaration into the parenthesised form the
/// branches below parse: `VECTOR,4,FLOAT` → `VECTOR(FLOAT,4)`,
/// `SPARSE,8,FLOAT,INT` → `SPARSE(FLOAT,INT,8)`.
fn expand_declaration(up: &str) -> String {
    if up.contains('(') || !up.contains(',') {
        return up.to_string();
    }
    let parts: Vec<&str> = up.split(',').map(str::trim).collect();
    match (parts[0], parts.len()) {
        ("VECTOR" | "EMBEDDING" | "TENSOR" | "TENSORARRAY" | "MULTIVECTOR", 3..) => {
            // `base,<dim>,<elem>`
            format!("{}({},{})", parts[0], parts[2], parts[1])
        }
        ("SPARSE", 4..) => format!("SPARSE({},{},{})", parts[2], parts[3], parts[1]),
        ("DECIMAL", 3..) => format!("DECIMAL({},{})", parts[1], parts[2]),
        ("ARRAY", 2..) => format!("ARRAY({})", parts[1]),
        _ => up.to_string(),
    }
}

/// Element type name -> `EmbeddingDataType` ordinal.
fn element_code(elem: &str) -> Option<i64> {
    Some(
        if elem.contains("UINT8")
            || (elem.contains("UNSIGNED") && (elem.contains("TINYINT") || elem.contains("INT8")))
        {
            value_format::element::UINT8
        } else if elem.contains("INT8") || elem.contains("TINYINT") {
            value_format::element::INT8
        } else if elem.contains("INT16") {
            value_format::element::INT16
        } else if elem.contains("INT32") || elem == "INT" {
            value_format::element::INT32
        } else if elem.contains("INT64") {
            value_format::element::INT64
        } else if elem.contains("FLOAT64") || elem.contains("DOUBLE") {
            value_format::element::FLOAT64
        } else if elem.contains("BFLOAT16") {
            value_format::element::BFLOAT16
        } else if elem.contains("FLOAT16") {
            value_format::element::FLOAT16
        } else if elem.contains("BIT") {
            value_format::element::BIT
        } else {
            value_format::element::FLOAT32
        },
    )
}

/// `EmbeddingDataType` ordinal -> the element name `data_type_of_text` accepts
/// (the inverse of [`element_code`]).
///
/// The spellings are the **engine's** (lower-case `float`/`double`/`int32`/…),
/// which is what `SHOW TABLE … COLUMNS` prints and the `.slt` fixtures expect —
/// not the C++ `FLOAT32`/`FLOAT64` names, which would leak into the engine's
/// type text.
fn element_name(code: i64) -> Option<&'static str> {
    Some(match code {
        value_format::element::BIT => "bit",
        value_format::element::UINT8 => "uint8",
        value_format::element::INT8 => "int8",
        value_format::element::INT16 => "int16",
        value_format::element::INT32 => "int32",
        value_format::element::INT64 => "int64",
        value_format::element::FLOAT32 => "float",
        value_format::element::FLOAT64 => "double",
        value_format::element::FLOAT16 => "float16",
        value_format::element::BFLOAT16 => "bfloat16",
        _ => return None,
    })
}

/// The `LogicalType` ordinal -> the type name `data_type_of_text` accepts
/// (the inverse of the plain-type half of [`data_type_of_text`]).
fn logical_name(code: i64) -> Option<&'static str> {
    Some(match code {
        value_format::logical::BOOLEAN => "BOOLEAN",
        value_format::logical::TINYINT => "TINYINT",
        value_format::logical::SMALLINT => "SMALLINT",
        value_format::logical::INTEGER => "INTEGER",
        value_format::logical::BIGINT => "BIGINT",
        value_format::logical::HUGEINT => "HUGEINT",
        value_format::logical::FLOAT => "FLOAT",
        value_format::logical::DOUBLE => "DOUBLE",
        value_format::logical::FLOAT16 => "FLOAT16",
        value_format::logical::BFLOAT16 => "BFLOAT16",
        value_format::logical::TIMESTAMP => "TIMESTAMP",
        value_format::logical::INTERVAL => "INTERVAL",
        value_format::logical::VARCHAR => "VARCHAR",
        value_format::logical::JSON => "JSON",
        value_format::logical::DATE => "DATE",
        value_format::logical::TIME => "TIME",
        value_format::logical::DATETIME => "DATETIME",
        value_format::logical::UUID => "UUID",
        value_format::logical::TUPLE => "TUPLE",
        _ => return None,
    })
}

/// The engine's declared-type text for a serialized C++ `DataType` — the
/// inverse of [`data_type_of_text`] for the shapes it covers.
pub fn type_text_of(column_type: &infinity_expression::json::Json) -> Option<String> {
    let code = value_format::data_type_code(column_type)?;
    if let Some(name) = logical_name(code) {
        return Some(name.to_string());
    }
    match code {
        value_format::logical::DECIMAL => {
            let p = value_format::type_info_int(column_type, "precision")?;
            let s = value_format::type_info_int(column_type, "scale")?;
            Some(format!("DECIMAL({p},{s})"))
        }
        value_format::logical::ARRAY => {
            // The ARRAY `type_info` is the element's whole DataType object.
            let inner = match column_type {
                infinity_expression::json::Json::Obj(entries) => entries
                    .iter()
                    .find(|(k, _)| k == "type_info")
                    .map(|(_, v)| v)?,
                _ => return None,
            };
            Some(format!("ARRAY({})", type_text_of(inner)?))
        }
        value_format::logical::EMBEDDING
        | value_format::logical::TENSOR
        | value_format::logical::TENSORARRAY
        | value_format::logical::MULTIVECTOR
        | value_format::logical::SPARSE => {
            let (base, elem_key) = match code {
                value_format::logical::EMBEDDING => ("EMBEDDING", "embedding_type"),
                value_format::logical::TENSOR => ("TENSOR", "embedding_type"),
                value_format::logical::TENSORARRAY => ("TENSORARRAY", "embedding_type"),
                value_format::logical::MULTIVECTOR => ("MULTIVECTOR", "embedding_type"),
                // A SPARSE `type_info` *is* the SparseInfo object, whose
                // `data_type` field is the element.
                _ => ("SPARSE", "data_type"),
            };
            let elem = value_format::type_info_int(column_type, elem_key)?;
            let dim = value_format::type_info_int(column_type, "dimension")?;
            // A SPARSE record also stores the index width, which is *not* a
            // function of the bound (`sparse,8,float,int` is an int32 index
            // over a bound of 8), so keep it in the text.
            let text = if code == value_format::logical::SPARSE {
                let index = value_format::type_info_int(column_type, "index_type")?;
                format!(
                    "{base}({},{},{})",
                    element_name(elem)?,
                    element_name(index)?,
                    dim
                )
            } else {
                format!("{base}({},{})", element_name(elem)?, dim)
            };
            // Keep the sparse storage hint in the text: the C++ record carries
            // it as `"sort": 1`, and `create_table_typed_with_sparse_stores`
            // splits it back out.
            if code == value_format::logical::SPARSE
                && value_format::type_info_int(column_type, "sort")
                    == Some(SparseStore::Sorted.as_i64())
            {
                return Some(format!("{text} WITH (SORTED)"));
            }
            Some(text)
        }
        _ => None,
    }
}

/// The `ConstantExpr::Serialize()` object for an engine default literal
/// (a SQL literal text, e.g. `'x'`, `42`, `true`); the scalar cases only.
pub fn default_constant_expr(text: &str) -> Option<infinity_expression::json::Json> {
    let t = text.trim();
    if t.eq_ignore_ascii_case("true") {
        return Some(value_format::constant_expr_bool(true));
    }
    if t.eq_ignore_ascii_case("false") {
        return Some(value_format::constant_expr_bool(false));
    }
    if let Some(inner) = t.strip_prefix('\'').and_then(|s| s.strip_suffix('\'')) {
        return Some(value_format::constant_expr_string(
            &inner.replace("''", "'"),
        ));
    }
    if let Ok(i) = t.parse::<i64>() {
        return Some(value_format::constant_expr_int(i));
    }
    if let Ok(f) = t.parse::<f64>() {
        return Some(value_format::constant_expr_double(f));
    }
    // The engine stores a string default in its **unquoted** `.slt` form
    // (`defaults_and_alter_add_column`), so anything left is a string constant —
    // including an explicitly empty `DEFAULT ''` (callers only pass `Some`).

    Some(value_format::constant_expr_string(t))
}

/// Whether a C++ `ColumnDef`'s `constraints` mark the column NOT NULL
/// (`ConstraintType::kNotNull == 1`, `src/parser/definition/column_def.h:37-43`).
pub fn is_not_null(col: &ColumnDefValue) -> bool {
    col.constraints.contains(&1)
}

/// The engine's default-literal text for a `ConstantExpr::Serialize()` value,
/// for the scalar cases; `None` for shapes not modelled.
pub fn default_text_of(default: &infinity_expression::json::Json) -> Option<String> {
    use infinity_expression::json::Json;
    let Json::Obj(entries) = default else {
        return None;
    };
    let get = |k: &str| entries.iter().find(|(n, _)| n == k).map(|(_, v)| v);
    let ty = match get("type")? {
        Json::Int(i) => *i,
        _ => return None,
    };
    let value = get("value")?;
    Some(match ty {
        0 => match value {
            Json::Bool(b) => b.to_string(),
            _ => return None,
        },
        1 => match value {
            Json::Float(f) => f.to_string(),
            Json::Int(i) => i.to_string(),
            _ => return None,
        },
        2 => match value {
            // The engine keeps a string default in its unquoted `.slt` form, and
            // `default_literal` reads it back as a `Str` — so do not re-quote.
            Json::Str(s) => s.clone(),
            _ => return None,
        },
        3 => match value {
            Json::Int(i) => i.to_string(),
            _ => return None,
        },
        _ => return None,
    })
}

/// The database a qualified table name belongs to (`default_db` when bare).
fn db_of(table: &str) -> &str {
    if table.contains('.') {
        table.split('.').next().unwrap_or("default_db")
    } else {
        "default_db"
    }
}

/// The unqualified part of a table name.
fn table_of(table: &str) -> &str {
    table.rsplit('.').next().unwrap_or(table)
}

/// Builds the C++ `IndexBase`-family record for one engine index.
///
/// Returns `None` for a `USING` kind the C++ has no index class for (or for a
/// column that vanished); the caller then leaves no record rather than write a
/// half-formed one.
/// One field of a JSON object, for the nested `ivf_option` blob.
fn json_field<'a>(parent: Option<&'a Json>, key: &str) -> Option<&'a Json> {
    match parent? {
        Json::Obj(fields) => fields.iter().find(|(k, _)| k == key).map(|(_, v)| v),
        _ => None,
    }
}

/// The name [`ivf_element_code`] reads back for a `plain_storage_data_type_`
/// ordinal (`embedding_type.h:33`).
fn ivf_element_name(code: i64) -> &'static str {
    match code {
        value_format::element::BIT => "bit",
        value_format::element::INT8 => "int8",
        value_format::element::INT16 => "int16",
        value_format::element::INT32 => "int32",
        value_format::element::INT64 => "int64",
        value_format::element::FLOAT64 => "double",
        value_format::element::UINT8 => "uint8",
        value_format::element::FLOAT16 => "float16",
        value_format::element::BFLOAT16 => "bfloat16",
        _ => "float32",
    }
}

/// `EmbeddingT::String2EmbeddingDataType` for the `plain_storage_data_type`
/// option (`embedding_type.h:33`).
fn ivf_element_code(text: &str) -> i64 {
    match text.trim().to_ascii_lowercase().as_str() {
        "bit" => value_format::element::BIT,
        "int8" | "i8" => value_format::element::INT8,
        "int16" | "i16" => value_format::element::INT16,
        "int32" | "i32" => value_format::element::INT32,
        "int64" | "i64" => value_format::element::INT64,
        "float16" | "f16" => value_format::element::FLOAT16,
        "bfloat16" | "bf16" => value_format::element::BFLOAT16,
        "double" | "float64" | "f64" => value_format::element::FLOAT64,
        "uint8" | "u8" => value_format::element::UINT8,
        _ => value_format::element::FLOAT32,
    }
}

fn index_def_for(im: &IndexMeta, table: &CppTable) -> Option<IndexDefValue> {
    // A functional index (`CREATE INDEX … (sqrt(c1))`) names an *expression*, so
    // there is no such column; the `SECONDARY` arm below turns it into a
    // `SECONDARYFUNCTIONAL` record instead of dropping it.
    let col = table.columns.iter().find(|c| c.column_name == im.column);
    let opt = |k: &str| -> Option<String> {
        im.options
            .iter()
            .find(|(n, _)| n.eq_ignore_ascii_case(k))
            .map(|(_, v)| v.clone())
    };
    let opt_word = |k: &str, default: &str| -> String {
        opt(k)
            .map(|v| v.trim().trim_matches(|c| c == '\'' || c == '"').to_string())
            .unwrap_or_else(|| default.to_string())
    };
    let opt_num = |k: &str, default: i64| -> i64 {
        opt(k)
            .and_then(|v| v.trim().parse().ok())
            .unwrap_or(default)
    };
    match im.index_type.as_str() {
        "HNSW" => Some(IndexDefValue::hnsw(
            &im.name,
            &table.table_name,
            &im.column,
            value_format::metric_name_of(&opt_word("METRIC", "l2")),
            value_format::hnsw_encode_name_of(&opt_word("ENCODE", "plain")),
            value_format::hnsw_build_name_of(&opt_word("BUILD_TYPE", "plain")),
            opt_num("M", value_format::HNSW_M),
            opt_num("EF_CONSTRUCTION", value_format::HNSW_EF_CONSTRUCTION),
            opt_num("BLOCK_SIZE", value_format::HNSW_BLOCK_SIZE),
            opt("LSG_CONFIG"),
        )),
        "FULLTEXT" => Some(IndexDefValue::full_text(
            &im.name,
            &table.table_name,
            &im.column,
            &opt_word("ANALYZER", "standard"),
        )),
        // An index with no `USING` kind is the C++ default, a secondary index.
        "SECONDARY" | "" => match col {
            Some(col) => {
                let is_json = value_format::data_type_code(&col.column_type)
                    == Some(value_format::logical::JSON);
                let low = opt("CARDINALITY")
                    .map(|v| v.eq_ignore_ascii_case("low"))
                    .unwrap_or(false);
                Some(IndexDefValue::secondary(
                    &im.name,
                    &table.table_name,
                    &im.column,
                    low,
                    is_json,
                ))
            }
            // No column of that name: a functional secondary index.
            None => functional_index_def(im, table),
        },
        "IVF" | "FLAT" => {
            let col = col?;
            let elem = value_format::type_info_int(&col.column_type, "embedding_type")
                .unwrap_or(value_format::element::FLOAT32);
            // `IndexIVF::Make` + `CheckIndexIVFStorageOption`: only the fields
            // the storage type uses are filled, so a quantized index keeps the
            // plain element type at `kElemInvalid` and an unset plain type
            // follows the column (i8/u8 keep theirs, any float becomes f32).
            let storage = opt_word("STORAGE_TYPE", "plain");
            let (storage_type, plain, sq, pqn, pqb) =
                if storage.eq_ignore_ascii_case("scalar_quantization") {
                    (
                        value_format::ivf_storage::SCALAR_QUANTIZATION,
                        value_format::element::INVALID,
                        opt_num("SCALAR_QUANTIZATION_BITS", 0),
                        0,
                        0,
                    )
                } else if storage.eq_ignore_ascii_case("product_quantization") {
                    (
                        value_format::ivf_storage::PRODUCT_QUANTIZATION,
                        value_format::element::INVALID,
                        0,
                        opt_num("PRODUCT_QUANTIZATION_SUBSPACE_NUM", 0),
                        opt_num("PRODUCT_QUANTIZATION_SUBSPACE_BITS", 0),
                    )
                } else {
                    let plain = opt("PLAIN_STORAGE_DATA_TYPE")
                        .map(|v| ivf_element_code(&v))
                        .unwrap_or(match elem {
                            value_format::element::INT8 | value_format::element::UINT8 => elem,
                            value_format::element::FLOAT32
                            | value_format::element::FLOAT64
                            | value_format::element::FLOAT16
                            | value_format::element::BFLOAT16 => value_format::element::FLOAT32,
                            other => other,
                        });
                    (value_format::ivf_storage::PLAIN, plain, 0, 0, 0)
                };
            Some(IndexDefValue::ivf(
                &im.name,
                &table.table_name,
                &im.column,
                value_format::metric_code_of(&opt_word("METRIC", "l2")),
                value_format::IvfCentroid {
                    num_ratio: opt("CENTROIDS_NUM_RATIO")
                        .and_then(|v| v.trim().parse::<f64>().ok())
                        .unwrap_or(1.0),
                    min_points: opt_num("MIN_POINTS_PER_CENTROID", 32),
                    max_points: opt_num("MAX_POINTS_PER_CENTROID", 256),
                },
                storage_type,
                plain,
                sq,
                pqn,
                pqb,
            ))
        }
        "BMP" => Some(IndexDefValue::bmp(
            &im.name,
            &table.table_name,
            &im.column,
            // `IndexBMP::block_size_` is the block size the index was built
            // with; the reference reads it back when it opens a chunk
            // (`BMPHandler::Load`, which sizes its result vector by it), so a 0
            // here makes every scan read an empty buffer.
            opt_num("BLOCK_SIZE", 16),
            if opt_word("COMPRESS_TYPE", "compressed").eq_ignore_ascii_case("raw") {
                value_format::bmp_compress::RAW
            } else {
                value_format::bmp_compress::COMPRESSED
            },
        )),
        "SMVE" => Some(IndexDefValue::smve(
            &im.name,
            &table.table_name,
            &im.column,
            opt_num("WIDTH", 2048),
            opt_num("TOPK", 8),
        )),
        "PLAID" => Some(IndexDefValue::plaid(
            &im.name,
            &table.table_name,
            &im.column,
            opt_num("NBITS", 4),
            opt_num("N_CENTROIDS", 0),
            opt("COLBERTSAR_MODE")
                .map(|v| v.eq_ignore_ascii_case("true"))
                .unwrap_or(false),
        )),
        "EMVB" => Some(IndexDefValue::emvb(
            &im.name,
            &table.table_name,
            &im.column,
            opt_num("PQ_SUBSPACE_NUM", 0),
            opt_num("PQ_SUBSPACE_BITS", 0),
        )),
        "DISKANN" => Some(IndexDefValue::diskann(
            &im.name,
            &table.table_name,
            &im.column,
            value_format::metric_name_of(&opt_word("METRIC", "l2")),
            value_format::diskann_encode_name_of(&opt_word("ENCODE", "plain")),
            opt_num("R", 0),
            opt_num("L", 0),
            opt_num("NUM_PQ_CHUNKS", 0),
            opt_num("NUM_PARTS", 0),
        )),
        _ => None,
    }
}

/// Builds the C++ `SECONDARYFUNCTIONAL` record for a functional index such as
/// `CREATE INDEX i ON t (sqrt(c1))`, where the engine keeps the *expression* in
/// `IndexMeta::column` rather than a column name.
///
/// C++ splits it into the base column(s) (`column_names`), the function's info
/// string (`func_col_params`) and the expression as JSON (`function_expression`,
/// which the C++ re-parses) plus the return type
/// (`index_secondary_functional_impl.cpp:67-77`;
/// `logical_planner_impl.cpp:970-979`). `func_col_params` carries the engine's
/// expression text verbatim, so reading the record back restores
/// `IndexMeta::column`. `func_return_type` is approximated from the base
/// column's type — this engine does not type-analyse the expression here.
fn functional_index_def(im: &IndexMeta, table: &CppTable) -> Option<IndexDefValue> {
    let expr = im.column.trim();
    // The base column the expression references: the longest matching name first,
    // so `c1` cannot match inside `c11`.
    let mut names: Vec<&str> = table
        .columns
        .iter()
        .map(|c| c.column_name.as_str())
        .collect();
    names.sort_by_key(|n| std::cmp::Reverse(n.len()));
    let base = *names.iter().find(|n| expr.contains(**n))?;
    let col = table.columns.iter().find(|c| c.column_name == base)?;
    let func = expr.split('(').next().unwrap_or(expr).trim();
    let low = im
        .options
        .iter()
        .find(|(k, _)| k.eq_ignore_ascii_case("CARDINALITY"))
        .map(|(_, v)| v.eq_ignore_ascii_case("low"))
        .unwrap_or(false);
    Some(IndexDefValue::secondary_functional(
        &im.name,
        &table.table_name,
        base,
        expr,
        col.column_type.clone(),
        infinity_expression::json::Json::Obj(vec![(
            "name".to_string(),
            infinity_expression::json::Json::Str(func.to_string()),
        )]),
        low,
    ))
}

impl Catalog {
    /// Writes every database, table and column into `store` using the C++
    /// key space and value encoding, returning how many records were written.
    ///
    /// The commit timestamp is a per-database monotonic counter, which is all a
    /// C++ reader needs (it scans a name's prefix and takes the newest version).
    pub fn write_cpp_layout(&self, store: &dyn MetaStore) -> Result<usize> {
        let dbs = self.list_databases()?;
        let tables = self.list_tables()?;
        let indexes = self.list_indexes()?;
        // The segment layer belongs to the block mirror, not to the native
        // catalog: keep whatever is already published instead of dropping it.
        let segments = read_cpp_segments(store)?;
        self.write_cpp_layout_from(store, &dbs, &tables, &indexes, &segments)
    }

    /// Like [`Self::write_cpp_layout`] but from an explicit snapshot. The
    /// C++-only catalog mode needs this: it mutates the desired lists and then
    /// rewrites the whole layout, and the just-mutated state is not yet readable
    /// back from the store.
    ///
    /// `segments` carries the tables' segment/block records
    /// ([`read_cpp_segments`] for a preserve-and-rewrite, or the mirror a
    /// checkpoint just wrote): the layout writer owns the `catalog|` key space
    /// and its sweep, so the segment layer has to be re-emitted here or a DDL
    /// would leave the C++ reader with a table that has no segments.
    pub fn write_cpp_layout_from(
        &self,
        store: &dyn MetaStore,
        dbs: &[String],
        tables: &[TableMeta],
        indexes: &[IndexMeta],
        segments: &[CppSegmentSet],
    ) -> Result<usize> {
        self.write_cpp_layout_with_ts(store, store, dbs, tables, indexes, segments)
    }

    /// Like [`Self::write_cpp_layout_from`], but the timestamps a record keeps
    /// are read from `ts_source` while the layout is written to `store`. The
    /// batch writer records into an in-memory sink that holds no catalog, so it
    /// has to name the real store the existing records live in.
    #[allow(clippy::too_many_arguments)]
    pub fn write_cpp_layout_with_ts(
        &self,
        store: &dyn MetaStore,
        ts_source: &dyn MetaStore,
        dbs: &[String],
        tables: &[TableMeta],
        indexes: &[IndexMeta],
        segments: &[CppSegmentSet],
    ) -> Result<usize> {
        let mut written = 0usize;
        // Every catalog record is keyed by the commit timestamp of the
        // transaction that created it (`KeyEncode::CatalogDbKey`). A record
        // that already exists keeps its timestamp; a record this DDL creates
        // gets the placeholder the caller replaces with the transaction's
        // commit timestamp.
        let ts_of = |prefix: &str| -> Result<Option<u64>> {
            let mut newest = None;
            for (k, _) in ts_source.scan_prefix(prefix.as_bytes())? {
                let text = String::from_utf8_lossy(&k).to_string();
                if let Some(ts) = text.rsplit('|').next().and_then(|t| t.parse::<u64>().ok()) {
                    newest = Some(newest.map_or(ts, |n: u64| n.max(ts)));
                }
            }
            Ok(newest)
        };

        let next_table_id = tables.iter().map(|t| t.id).max().unwrap_or(0) + 1;
        // The reference hands out **1-based** database ids (`CreateDatabase`
        // applies `next_database_id_ + 1`, and the block mirror names its
        // directory `db_{db_id}`), so the first database is 1, not 0.
        let db_id_of =
            |name: &str| -> u64 { dbs.iter().position(|d| d == name).unwrap_or(0) as u64 + 1 };

        for (i, db) in dbs.iter().enumerate() {
            let ts = ts_of(&format!("catalog|db|{db}|"))?.unwrap_or(crate::DDL_TS_SENTINEL);
            let db_str = (i as u64 + 1).to_string();
            store.put(key::catalog_db_key(db, ts).as_bytes(), db_str.as_bytes())?;
            store.put(
                key::catalog_db_tag_key(&db_str, "next_table_id").as_bytes(),
                next_table_id.to_string().as_bytes(),
            )?;
            written += 2;
        }

        // Resolved table layout, kept so the index pass below can reach each
        // table's db id, table id and column types.
        let mut resolved: Vec<(String, String, u64, CppTable)> = Vec::new();

        for t in tables {
            let db_name = db_of(&t.name);
            if !dbs.iter().any(|d| d == db_name) {
                continue; // unknown database: nothing to hang the table under
            }
            let db_id = db_id_of(db_name);
            let db_str = db_id.to_string();
            let tbl_str = t.id.to_string();
            let name = table_of(&t.name);
            let ts =
                ts_of(&format!("catalog|tbl|{db_str}|{name}|"))?.unwrap_or(crate::DDL_TS_SENTINEL);

            store.put(
                key::catalog_table_key(&db_str, name, ts).as_bytes(),
                tbl_str.as_bytes(),
            )?;
            for (tag, value) in [
                // The next id to hand out; dropping a column leaves a hole.
                (
                    "next_column_id",
                    t.ids.iter().copied().max().map_or(0, |v| v + 1),
                ),
                ("next_segment_id", 0),
                ("unsealed_segment_id", 0),
                // `next_index_id` starts at 0 (`table_meta_impl.cpp:281-282`);
                // the C++ reader takes it as the next id to hand out.
                ("next_index_id", 0),
            ] {
                store.put(
                    key::catalog_table_tag_key(&db_str, &tbl_str, tag).as_bytes(),
                    value.to_string().as_bytes(),
                )?;
                written += 1;
            }
            written += 1;

            let mut columns = Vec::new();
            for (i, col) in t.columns.iter().enumerate() {
                let type_text = t.types.get(i).cloned().unwrap_or_default();
                let sparse_store = t.sparse_stores.get(i).copied().flatten();
                let Some(data_type) = data_type_of_text_with_store(&type_text, sparse_store) else {
                    return Err(Error::Catalog(format!(
                        "column '{col}' of '{}' has a type the C++ catalog encoding \
                         does not cover yet: '{type_text}'",
                        t.name
                    )));
                };
                let column_id = t.ids.get(i).copied().unwrap_or(i as u64) as i64;
                let mut value = ColumnDefValue::new(data_type, column_id, col.clone());
                value.column_comment = t.comments.get(i).cloned().flatten();
                // The C++ records NOT NULL as the `kNotNull` constraint and a
                // default as a `ConstantExpr::Serialize()` object, so carry
                // both across. A column *without* a default still records
                // one: the reference's `ColumnDef` replaces a missing default
                // with a null constant
                // (`column_def.cpp:122-124`), so the record always has the
                // `{"type": 4}` (`LiteralType::kNull`) object.
                if t.not_nulls.get(i).copied().unwrap_or(false) {
                    value.constraints = vec![1]; // ConstraintType::kNotNull
                }
                value.default = Some(
                    t.defaults
                        .get(i)
                        .cloned()
                        .flatten()
                        .as_deref()
                        .and_then(default_constant_expr)
                        .unwrap_or_else(|| {
                            value_format::constant_expr_json(value_format::literal::NULL, None)
                        }),
                );
                store.put(
                    key::table_column_key(&db_str, &tbl_str, col, ts).as_bytes(),
                    &value_format::column_def_bytes(&value),
                )?;
                written += 1;
                columns.push(value);
            }
            resolved.push((
                db_str,
                tbl_str,
                ts,
                CppTable {
                    db_name: db_name.to_string(),
                    db_id,
                    table_name: name.to_string(),
                    table_id: t.id,
                    columns,
                    next_column_id: t.ids.iter().copied().max().map_or(0, |v| v + 1),
                },
            ));
            // The segment/block records of this table's block mirror. Written
            // after the table tags so the real `next_segment_id` /
            // `unsealed_segment_id` replace the 0 defaults above.
            //
            // Match on the table id alone and re-key the set to this layout's
            // database id: the publisher derives its db id from the engine's
            // 1-based numbering while the layout numbers databases from 0, and
            // a mismatch here meant no set was ever found - the sweep below
            // deletes every `catalog|` key, so the segment records were dropped
            // on every checkpoint and the reference saw a segmentless table.
            if let Some(set) = segments.iter().find(|s| s.table_id == t.id.to_string()) {
                let mut set = set.clone();
                set.db_id = db_id.to_string();
                written += write_cpp_segments(store, &set)?;
            }
        }

        // Index records: `idx|{db}|{tbl}|{name}|{ts}` -> index_id (a per-table
        // counter starting at 1, as the C++ engine hands out), plus an
        // `index_base` tag holding the C++
        // `IndexBase::Serialize()` JSON (`table_index_meta_impl.cpp:90-97`).
        for im in indexes {
            let im_table = table_of(&im.table);
            let im_db = db_of(&im.table);
            let Some((db_str, tbl_str, _table_ts, cpp)) = resolved
                .iter()
                .find(|(_, _, _, c)| c.table_name == im_table && c.db_name == im_db)
            else {
                continue; // the index's table is not in the written layout
            };
            let Some(def) = index_def_for(im, cpp) else {
                continue;
            };
            // Ids are assigned at CREATE INDEX and never reused; a record read
            // back without an id falls back to its 1-based position.
            let index_id = if im.id > 0 {
                im.id
            } else {
                indexes
                    .iter()
                    .filter(|o| table_of(&o.table) == im_table && db_of(&o.table) == im_db)
                    .position(|o| o.name == im.name)
                    .map_or(1, |p| p + 1) as u64
            };
            store.put(
                key::catalog_index_key(
                    db_str,
                    tbl_str,
                    &im.name,
                    ts_of(&format!("catalog|idx|{db_str}|{tbl_str}|{}|", im.name))?
                        .unwrap_or(crate::DDL_TS_SENTINEL),
                )
                .as_bytes(),
                index_id.to_string().as_bytes(),
            )?;
            store.put(
                key::catalog_index_tag_key(db_str, tbl_str, &index_id.to_string(), "index_base")
                    .as_bytes(),
                &value_format::index_def_bytes(&def),
            )?;
            written += 2;
        }
        // Record how many ids each table handed out.
        for (db_str, tbl_str, _, cpp) in &resolved {
            let count = indexes
                .iter()
                .filter(|o| table_of(&o.table) == cpp.table_name && db_of(&o.table) == cpp.db_name)
                .count();
            if count > 0 {
                // The C++ tag holds the last id handed out, and ids are 1-based.
                store.put(
                    key::catalog_table_tag_key(db_str, tbl_str, "next_index_id").as_bytes(),
                    count.to_string().as_bytes(),
                )?;
            }
        }
        // An index whose target is an expression this engine does not
        // materialise (`CREATE INDEX … (c1 + 5)`) has an **empty** `column`, so
        // there is no C++ `IndexBase` to write. Record just its name under a
        // table tag, so it still exists in the C++ key space (and can be
        // dropped); the C++ engine ignores an unknown tag.
        for (db_str, tbl_str, _, cpp) in &resolved {
            let names: Vec<&str> = indexes
                .iter()
                .filter(|im| {
                    im.column.is_empty()
                        && table_of(&im.table) == cpp.table_name
                        && db_of(&im.table) == cpp.db_name
                })
                .map(|im| im.name.as_str())
                .collect();
            if !names.is_empty() {
                store.put(
                    key::catalog_table_tag_key(db_str, tbl_str, "rust_name_only_indexes")
                        .as_bytes(),
                    names.join(",").as_bytes(),
                )?;
                written += 1;
            }
        }

        // The system tag the reference reads on startup
        // (`MetaTree::RestoreSystemCache`, `meta_tree_impl.cpp:577`). The
        // *table* id counter is a per-database tag (`db|{db_id}|next_table_id`,
        // written above), not a system tag - the reference has no top-level
        // `next_table_id`.
        store.put(
            key::NEXT_DATABASE_ID.as_bytes(),
            dbs.len().to_string().as_bytes(),
        )?;
        // `meta|version` is what stops the reference from treating the catalog
        // as pre-0.7.0 and running its JSON-index conversion on startup
        // (`Storage::AdminToWriter`, `storage_impl.cpp:381`). The layout sweep
        // deletes it with the rest of the key space, so it is written back here
        // with every rewrite.
        store.put(
            key::META_VERSION.as_bytes(),
            key::CATALOG_VERSION.as_bytes(),
        )?;
        written += 2;
        store.flush()?;
        Ok(written)
    }

    /// The `(key, value)` pairs [`Catalog::write_cpp_layout_from`] would write,
    /// without writing them.
    ///
    /// A DDL publishes the whole C++ layout at once (`rewrite_cpp`), and it must
    /// reach the store as a single `MetaStore::atomic_batch`: a crash between
    /// the prefix deletes and the rewrite is exactly the half-written catalog
    /// the C++ engine would read. Going through the same writer keeps the bytes
    /// identical to the direct path.
    pub fn write_cpp_layout_batch(
        &self,
        dbs: &[String],
        tables: &[TableMeta],
        indexes: &[IndexMeta],
        segments: &[CppSegmentSet],
    ) -> Result<Vec<crate::BatchItem>> {
        // The recorder is a sink: the timestamps the records keep have to be
        // read from the catalog this layout is being derived from.
        self.write_cpp_layout_batch_from(self.store.as_ref(), dbs, tables, indexes, segments)
    }

    /// Like [`Self::write_cpp_layout_batch`], but reading the timestamps the
    /// records keep from `ts_source`.
    ///
    /// A DDL that is still in flight writes into a capture overlay instead of
    /// the store, so the *existing* timestamps have to be read through the same
    /// overlay: a record created by an earlier step of the same statement is
    /// part of the layout this rewrite is derived from.
    pub fn write_cpp_layout_batch_from(
        &self,
        ts_source: &dyn MetaStore,
        dbs: &[String],
        tables: &[TableMeta],
        indexes: &[IndexMeta],
        segments: &[CppSegmentSet],
    ) -> Result<Vec<crate::BatchItem>> {
        let recorder = LayoutBatch::new();
        self.write_cpp_layout_with_ts(&recorder, ts_source, dbs, tables, indexes, segments)?;
        Ok(recorder.into_items())
    }

    /// Materializes a **C++-layout** catalog (as written by
    /// [`Catalog::write_cpp_layout`], or by the C++ engine itself) into this
    /// native catalog, so the engine can open a directory the C++ engine wrote
    /// (§2.6.1 item 4, the read direction).
    ///
    /// Databases, tables, columns (with declared type text, comments, defaults
    /// for the scalar cases and NOT NULL) and index records are imported;
    /// returns how many tables were created. A table that already exists under
    /// the same name is skipped.
    pub fn import_cpp_layout(&self, store: &dyn MetaStore) -> Result<usize> {
        let cpp_tables = read_cpp_layout(store)?;
        let mut imported = 0usize;
        for t in &cpp_tables {
            let qualified = format!("{}.{}", t.db_name, t.table_name);
            if self.get_table_by_name(&qualified)?.is_some() {
                continue;
            }
            self.create_database(&t.db_name, true)?;
            let columns: Vec<String> = t.columns.iter().map(|c| c.column_name.clone()).collect();
            let types: Vec<String> = t
                .columns
                .iter()
                .map(|c| type_text_of(&c.column_type).unwrap_or_default())
                .collect();
            let comments: Vec<Option<String>> =
                t.columns.iter().map(|c| c.column_comment.clone()).collect();
            let defaults: Vec<Option<String>> = t
                .columns
                .iter()
                .map(|c| c.default.as_ref().and_then(default_text_of))
                .collect();
            let not_nulls: Vec<bool> = t.columns.iter().map(is_not_null).collect();
            self.create_table_typed(
                &qualified, &columns, &defaults, &not_nulls, &types, &comments,
            )?;
            imported += 1;
        }
        // Index records, resolved against the tables just imported.
        for def in read_cpp_indexes(store)? {
            let Some(meta) = index_meta_of(&def, &cpp_tables) else {
                continue;
            };
            // Keep the reference's id: it names the index's directory
            // (`idx_{id}/seg_0/`) and its records, so a renumbered index
            // would have its chunk mirror read as another index's.
            self.create_index_typed_with_id(
                &meta.name,
                &meta.table,
                &meta.column,
                &meta.index_type,
                &meta.options,
                meta.id,
            )?;
        }
        Ok(imported)
    }
}

/// Reads a C++-layout catalog back, one entry per table.
pub fn read_cpp_layout(store: &dyn MetaStore) -> Result<Vec<CppTable>> {
    let mut dbs: Vec<(u64, String)> = Vec::new();
    // `catalog|db|` - note `catalog_db_prefix("")` would emit a doubled pipe.
    for (k, v) in store.scan_prefix(b"catalog|db|")? {
        let key = String::from_utf8_lossy(&k).to_string();
        // `catalog|db|{name}|{ts}` -> the name may itself contain '|', so take
        // the last two fields off the right.
        let mut it = key.rsplitn(3, '|');
        let _ts = it.next();
        let name = it.next().unwrap_or_default().to_string();
        let id = String::from_utf8_lossy(&v).parse().unwrap_or(0);
        dbs.push((id, name));
    }
    dbs.sort();

    let mut out = Vec::new();
    for (db_id, db_name) in &dbs {
        let prefix = key::catalog_db_table_prefix(&db_id.to_string());
        for (k, v) in store.scan_prefix(prefix.as_bytes())? {
            let key = String::from_utf8_lossy(&k).to_string();
            let mut it = key.rsplitn(2, '|');
            let _ts = it.next();
            let rest = it.next().unwrap_or_default().to_string();
            // `catalog|tbl|{db_id}|{table_name}` - the name is the 4th field.
            let table_name = rest.splitn(4, '|').nth(3).unwrap_or_default().to_string();
            let table_id: u64 = String::from_utf8_lossy(&v).parse().unwrap_or(0);

            let mut columns = Vec::new();
            let col_prefix = key::table_column_prefix(&db_id.to_string(), &table_id.to_string());
            for (_, cv) in store.scan_prefix(col_prefix.as_bytes())? {
                if let Some(col) = value_format::column_def_from_bytes(&cv) {
                    columns.push(col);
                }
            }
            columns.sort_by_key(|c| c.column_id);
            let next_column_id = store
                .get(
                    key::catalog_table_tag_key(
                        &db_id.to_string(),
                        &table_id.to_string(),
                        "next_column_id",
                    )
                    .as_bytes(),
                )?
                .and_then(|v| String::from_utf8_lossy(&v).parse().ok())
                .unwrap_or(columns.len() as u64);
            out.push(CppTable {
                db_name: db_name.clone(),
                db_id: *db_id,
                table_name,
                table_id,
                columns,
                next_column_id,
            });
        }
    }
    Ok(out)
}

/// Reads back the index records written by [`Catalog::write_cpp_layout`]:
/// every `idx|…|index_base` tag, decoded.
pub fn read_cpp_indexes(store: &dyn MetaStore) -> Result<Vec<IndexDefValue>> {
    let mut out = Vec::new();
    for (k, v) in store.scan_prefix(b"catalog|idx|")? {
        let key = String::from_utf8_lossy(&k).to_string();
        // `catalog|idx|{db}|{tbl}|{name}|{ts}` - six fields. The value is the
        // numeric index id that locates the `index_base` tag.
        let fields: Vec<&str> = key.split('|').collect();
        if fields.len() != 6 {
            continue;
        }
        let index_id = String::from_utf8_lossy(&v).to_string();
        let tag = key::catalog_index_tag_key(fields[2], fields[3], &index_id, "index_base");
        if let Some(bytes) = store.get(tag.as_bytes())? {
            if let Some(mut def) = value_format::index_def_from_bytes(&bytes) {
                // The record's key names the table the index belongs to; the
                // index base does not.
                def.owner = Some((fields[2].to_string(), fields[3].to_string()));
                // The record's value is the id the reference handed out; it
                // names the index's directory and records, so it has to
                // survive the round trip.
                def.index_id = index_id.parse().ok();
                out.push(def);
            }
        }
    }
    out.sort_by(|a, b| a.base.index_name.cmp(&b.base.index_name));
    Ok(out)
}

/// Reads a table's segment layer back: every `catalog|seg|…` record plus the
/// `catalog|blk|…` records beneath it.
///
/// The reference enumerates segments and blocks by prefix scan
/// (`GetTableSegments` / `GetTableSegmentBlocks`,
/// `src/storage/catalog/kv_utility_impl.cpp:37,100`), so these two families are
/// exactly what makes a written mirror visible: without them a C++ engine opens
/// the table but believes it has no data.
pub fn read_cpp_segments(store: &dyn MetaStore) -> Result<Vec<CppSegmentSet>> {
    let mut out: Vec<CppSegmentSet> = Vec::new();
    for (k, v) in store.scan_prefix(b"catalog|seg|")? {
        let key = String::from_utf8_lossy(&k).to_string();
        // `catalog|seg|{db_id}|{table_id}|{segment_id}`
        let fields: Vec<&str> = key.split('|').collect();
        if fields.len() != 5 {
            continue;
        }
        let (db_id, table_id) = (fields[2].to_string(), fields[3].to_string());
        let Ok(segment_id) = fields[4].parse::<u32>() else {
            continue;
        };
        let create_ts = String::from_utf8_lossy(&v).parse().unwrap_or(0);
        let mut blocks = Vec::new();
        for (bk, _) in store
            .scan_prefix(key::catalog_block_prefix(&db_id, &table_id, segment_id).as_bytes())?
        {
            let bkey = String::from_utf8_lossy(&bk).to_string();
            // `catalog|blk|{db_id}|{table_id}|{segment_id}|{block_id}`
            let bfields: Vec<&str> = bkey.split('|').collect();
            let Some(block_id) = bfields.get(5).and_then(|f| f.parse::<u16>().ok()) else {
                continue;
            };
            let commit_ts = store
                .get(bk.as_slice())?
                .and_then(|v| String::from_utf8_lossy(&v).parse().ok())
                .unwrap_or(CPP_MIRROR_TS);
            blocks.push(CppBlock {
                block_id,
                commit_ts,
            });
        }
        blocks.sort_by_key(|b| b.block_id);
        let set = match out
            .iter_mut()
            .find(|s| s.db_id == db_id && s.table_id == table_id)
        {
            Some(set) => set,
            None => {
                out.push(CppSegmentSet {
                    db_id: db_id.clone(),
                    table_id: table_id.clone(),
                    segments: Vec::new(),
                });
                out.last_mut().unwrap()
            }
        };
        set.segments.push(CppSegment {
            segment_id,
            create_ts,
            first_delete_ts: UNCOMMIT_TS,
            blocks,
        });
    }
    for set in out.iter_mut() {
        set.segments.sort_by_key(|s| s.segment_id);
    }
    out.sort_by(|a, b| (&a.db_id, &a.table_id).cmp(&(&b.db_id, &b.table_id)));
    Ok(out)
}

/// Writes a table's segment layer: `catalog|seg|…`, `catalog|blk|…`, the
/// segment's `first_delete_ts` tag, and the table's `next_segment_id` /
/// `unsealed_segment_id` tags.
///
/// Nothing else in the catalog is touched, so a checkpoint can publish the
/// blocks it just wrote without rewriting (and re-sweeping) the whole layout.
/// The C++ reader derives a block's column files from the column ids and its
/// row count from the block's `version` file (`ColumnMeta::FilePaths`,
/// `GetBlockRowCount`), so there are no per-block column records to write.
/// `drop|seg|{db}/{table}/{segment}` — the commit ts at which `segment` was
/// deprecated by a compaction (`NewTxn::Compact` -> the WAL's deprecated
/// segment list). A reader whose `begin_ts` is at or above it ignores the
/// segment; the files and the catalog records stay until `Cleanup`.
pub fn cpp_dropped_segment_key(db_id: &str, table_id: &str, segment_id: u32) -> String {
    format!("drop|seg|{db_id}/{table_id}/{segment_id}")
}

/// `drop|idx_seg|{db}/{table}/{index}/{segment}` — the same for one index's
/// copy of a deprecated segment (`drop|idx_seg|1/0/1/0 = 32` in a probe of a
/// four-segment table compacted at ts 32).
pub fn cpp_dropped_index_segment_key(
    db_id: &str,
    table_id: &str,
    index_id: u64,
    segment_id: u32,
) -> String {
    format!("drop|idx_seg|{db_id}/{table_id}/{index_id}/{segment_id}")
}

/// Records `segments` as deprecated at `ts`, for the table and for each of its
/// indexes. The reference writes this inside the compaction transaction and
/// keeps the source segments' files and catalog records until `Cleanup`.
pub fn write_cpp_drops(
    store: &dyn MetaStore,
    set: &CppSegmentSet,
    index_ids: &[u64],
    segments: &[u32],
    ts: u64,
) -> Result<usize> {
    let value = ts.to_string();
    let mut written = 0usize;
    for segment in segments {
        store.put(
            cpp_dropped_segment_key(&set.db_id, &set.table_id, *segment).as_bytes(),
            value.as_bytes(),
        )?;
        written += 1;
        for index_id in index_ids {
            store.put(
                cpp_dropped_index_segment_key(&set.db_id, &set.table_id, *index_id, *segment)
                    .as_bytes(),
                value.as_bytes(),
            )?;
            written += 1;
            // The reference drops the `(index, segment)` pair itself
            // (`TableIndexMeta::RemoveSegmentIndexID1`), which is what
            // `GetTableIndexSegments` enumerates - it never consults
            // `drop|idx_seg|`. The chunk records stay: only the pair is gone.
            let pair = key::catalog_index_segment_key(
                &set.db_id,
                &set.table_id,
                &index_id.to_string(),
                *segment,
            );
            for (k, _) in store.scan_prefix(format!("{pair}|").as_bytes())? {
                store.delete(&k)?;
            }
            store.delete(pair.as_bytes())?;
        }
    }
    Ok(written)
}

/// The segments of `table` that a compaction deprecated (and `Cleanup` has not
/// reclaimed yet).
pub fn cpp_dropped_segments(
    store: &dyn MetaStore,
    db_id: &str,
    table_id: &str,
) -> Result<Vec<u32>> {
    let prefix = format!("drop|seg|{db_id}/{table_id}/");
    let mut out = Vec::new();
    for (k, _) in store.scan_prefix(prefix.as_bytes())? {
        let key = String::from_utf8_lossy(&k).to_string();
        if let Some(id) = key.rsplit('/').next().and_then(|s| s.parse::<u32>().ok()) {
            out.push(id);
        }
    }
    out.sort_unstable();
    Ok(out)
}

pub fn write_cpp_segments(store: &dyn MetaStore, set: &CppSegmentSet) -> Result<usize> {
    let mut written = 0usize;
    // A mirror rewrite can leave fewer blocks (or segments) than the previous
    // one, so start from an empty segment layer for the table - except for the
    // segments a compaction deprecated, whose records the reference keeps until
    // `Cleanup` (the `drop|seg|` record is what makes a reader ignore them).
    let dropped = cpp_dropped_segments(store, &set.db_id, &set.table_id)?;
    for prefix in [
        key::catalog_segment_prefix(&set.db_id, &set.table_id),
        format!("catalog|blk|{}|{}|", set.db_id, set.table_id),
    ] {
        for (k, _) in store.scan_prefix(prefix.as_bytes())? {
            let key = String::from_utf8_lossy(&k).to_string();
            let mut parts = key.split('|').skip(2);
            let segment = parts.nth(2).and_then(|s| s.parse::<u32>().ok());
            if segment.is_some_and(|s| dropped.contains(&s)) {
                continue;
            }
            store.delete(&k)?;
        }
    }
    for seg in &set.segments {
        store.put(
            key::catalog_segment_key(&set.db_id, &set.table_id, seg.segment_id).as_bytes(),
            seg.create_ts.to_string().as_bytes(),
        )?;
        store.put(
            key::catalog_segment_tag_key(
                &set.db_id,
                &set.table_id,
                seg.segment_id,
                "first_delete_ts",
            )
            .as_bytes(),
            seg.first_delete_ts.to_string().as_bytes(),
        )?;
        written += 2;
        for block in &seg.blocks {
            store.put(
                key::catalog_block_key(&set.db_id, &set.table_id, seg.segment_id, block.block_id)
                    .as_bytes(),
                block.commit_ts.to_string().as_bytes(),
            )?;
            written += 1;
        }
    }
    // The reference writes `next_segment_id` as "0" when the table is created
    // and **never advances it** (`TableMeta::InitSet`,
    // `table_meta_impl.cpp:274`): the id it hands out is derived from the
    // segment map (`MetaTableObject::GetNextSegmentID`,
    // `meta_tree_impl.cpp:657`). Writing the derived value here instead would
    // leave a tag the reference itself never produces.
    store.put(
        key::catalog_table_tag_key(&set.db_id, &set.table_id, "next_segment_id").as_bytes(),
        b"0",
    )?;
    // `unsealed_segment_id` *is* maintained: it names the segment the engine is
    // still appending to (`NewTxn::CommitBottomAppend`,
    // `new_txn_data_impl.cpp:1854`).
    let unsealed = set.segments.iter().map(|s| s.segment_id).max();
    if let Some(unsealed) = unsealed {
        store.put(
            key::catalog_table_tag_key(&set.db_id, &set.table_id, "unsealed_segment_id").as_bytes(),
            unsealed.to_string().as_bytes(),
        )?;
    }
    written += 2;
    Ok(written)
}

/// Writes the sealed-chunk layer of one index: `idx_seg|…` (the segment index),
/// its `next_chunk_id` tag, and one `idx_chunk|…|chunk_info` record per chunk.
///
/// The reference needs these to know that the segment's rows are already
/// indexed: `NewTxn::RecoverMemIndex` walks the appended rows, and with no chunk
/// records every row is a "gap" it tries to rebuild into the memory index
/// (`CountMemIndexGapInSegment`, `new_txn_index_impl.cpp:2768`) — which dumps a
/// fresh chunk and, on a Rust-written mirror, aborts the startup.
pub fn write_cpp_index_segments(
    store: &dyn MetaStore,
    set: &CppSegmentSet,
    indexes: &[CppIndexSegment],
) -> Result<usize> {
    let mut written = 0usize;
    for index in indexes {
        let idx_str = index.index_id.to_string();
        let seg_prefix = format!(
            "idx_chunk|{}|{}|{}|{}|",
            set.db_id, set.table_id, idx_str, index.segment_id
        );
        for (k, _) in store.scan_prefix(seg_prefix.as_bytes())? {
            store.delete(&k)?;
        }
        let seg_key =
            key::catalog_index_segment_key(&set.db_id, &set.table_id, &idx_str, index.segment_id);
        for (k, _) in store.scan_prefix(format!("{seg_key}|").as_bytes())? {
            store.delete(&k)?;
        }
        if index.chunks.is_empty() {
            store.delete(seg_key.as_bytes())?;
            continue;
        }
        // `idx_seg|…` holds the commit ts of the transaction that first paired
        // this index with this segment (`TableIndexMeta::AddSegmentIndexID1`,
        // `src/storage/catalog/meta/table_index_meta_impl.cpp:138`). That is
        // either the append that opened the segment — which adds a segment
        // entry for every index the table already has — or the `CREATE INDEX`
        // when the segment already existed; whichever came later.
        let index_ts = index_create_ts(store, &set.db_id, &set.table_id, index.index_id)?;
        let segment_ts = segment_create_ts(store, &set.db_id, &set.table_id, index.segment_id)?;
        let paired_ts = match (index_ts, segment_ts) {
            (Some(a), Some(b)) => a.max(b),
            (Some(a), None) | (None, Some(a)) => a,
            (None, None) => CPP_MIRROR_TS,
        };
        store.put(seg_key.as_bytes(), paired_ts.to_string().as_bytes())?;
        written += 1;
        let next_chunk = index
            .chunks
            .iter()
            .map(|c| c.chunk_id + 1)
            .max()
            .unwrap_or(0);
        store.put(
            key::catalog_index_segment_tag_key(
                &set.db_id,
                &set.table_id,
                &idx_str,
                index.segment_id,
                "next_chunk_id",
            )
            .as_bytes(),
            next_chunk.to_string().as_bytes(),
        )?;
        written += 1;
        for chunk in &index.chunks {
            // The chunk **id** key (`idx_chunk|…|{chunk_id}`, value = the
            // commit ts) is what the reference enumerates to learn a
            // segment's chunks (`SegmentIndexMeta::LoadChunkIDs1`,
            // `src/storage/catalog/meta/segment_index_meta_impl.cpp:236`): it
            // takes the numeric suffix of every key under the prefix and skips
            // the tagged ones. Without it the reference sees no chunk at all,
            // treats every row of the segment as an index gap and rebuilds the
            // index from the table's rows.
            //
            // The key may only be published for a chunk whose files the
            // reference can read - with the key present it reads the files
            // instead of rebuilding, so a chunk it cannot decode would make
            // it answer with nothing. Every chunk the port mirrors is in the
            // reference's own byte layout, so all of them qualify.
            //
            // The value is the commit ts of the transaction whose memory
            // dump produced the chunk (`SegmentIndexMeta::AddChunkIndexID1`,
            // `segment_index_meta_impl.cpp:129`). `chunk.commit_ts` carries it:
            // the executor records the ts of the `DumpIndex` that sealed the
            // index and hands it to the mirror. The mirror writes the record at
            // every checkpoint, and a dump that happens afterwards stamps the
            // records it already published (the value is preserved here). A
            // chunk with no dump behind it keeps 0 — a reader sees it from any
            // `begin_ts`.
            let id_key = key::catalog_index_chunk_key(
                &set.db_id,
                &set.table_id,
                &idx_str,
                index.segment_id,
                chunk.chunk_id as u64,
            );
            store.put(id_key.as_bytes(), chunk.commit_ts.to_string().as_bytes())?;
            written += 1;
            let key = key::catalog_index_chunk_tag_key(
                &set.db_id,
                &set.table_id,
                &idx_str,
                index.segment_id,
                chunk.chunk_id as u64,
                "chunk_info",
            );
            // The reference's `nlohmann::json::dump()` writes an object's
            // keys in **sorted** order, so the record's bytes depend on the
            // order: `base_name`, `base_row_id`, `index_size`, `row_count`,
            // `term_count`.
            let value = format!(
                "{{\"base_name\":{},\"base_row_id\":{},\"index_size\":{},\"row_count\":{},\"term_count\":{}}}",
                json_string(&chunk.base_name),
                chunk.base_row_id,
                chunk.index_size,
                chunk.row_count,
                chunk.term_count
            );
            store.put(key.as_bytes(), value.as_bytes())?;
            written += 1;
        }
    }
    Ok(written)
}

/// The commit ts of the transaction that created `index_id`
/// (`catalog|idx|{db}|{table}|{name}|{ts}` → the index id).
fn index_create_ts(
    store: &dyn MetaStore,
    db_id: &str,
    table_id: &str,
    index_id: u64,
) -> Result<Option<u64>> {
    let prefix = key::catalog_table_index_prefix(db_id, table_id);
    let want = index_id.to_string();
    for (k, v) in store.scan_prefix(prefix.as_bytes())? {
        if v != want.as_bytes() {
            continue;
        }
        let k = String::from_utf8_lossy(&k).to_string();
        let ts = k.rsplit('|').next().and_then(|t| t.parse::<u64>().ok());
        return Ok(ts);
    }
    Ok(None)
}

/// The value of `catalog|seg|{db}|{table}|{segment}` — the ts of the append
/// that opened the segment.
fn segment_create_ts(
    store: &dyn MetaStore,
    db_id: &str,
    table_id: &str,
    segment_id: u32,
) -> Result<Option<u64>> {
    let k = key::catalog_segment_key(db_id, table_id, segment_id);
    Ok(store
        .get(k.as_bytes())?
        .and_then(|v| String::from_utf8_lossy(&v).parse::<u64>().ok()))
}

/// A JSON string literal (the chunk info is read with `simdjson`, so the base
/// name has to be quoted and escaped).
fn json_string(text: &str) -> String {
    let mut out = String::with_capacity(text.len() + 2);
    out.push('"');
    for c in text.chars() {
        match c {
            '"' => out.push_str("\\\""),
            '\\' => out.push_str("\\\\"),
            c if (c as u32) < 0x20 => out.push_str(&format!("\\u{:04x}", c as u32)),
            c => out.push(c),
        }
    }
    out.push('"');
    out
}

/// Maps a C++ index record onto the engine's `IndexMeta`, resolving the owning
/// table from the C++ tables (which carry db/table names and columns).
pub fn index_meta_of(def: &IndexDefValue, cpp_tables: &[CppTable]) -> Option<IndexMeta> {
    // The record's key says which table the index belongs to; only a record
    // without one (a hand-written one) falls back to guessing from the
    // indexed column's name - and two tables with a column of the same name
    // would then give the index to the wrong table.
    let table = match &def.owner {
        Some((db_id, table_id)) => cpp_tables
            .iter()
            .find(|t| t.db_id.to_string() == *db_id && t.table_id.to_string() == *table_id)
            .map(|t| format!("{}.{}", t.db_name, t.table_name)),
        None => def.base.column_names.first().and_then(|c| {
            cpp_tables
                .iter()
                .find(|t| t.columns.iter().any(|col| &col.column_name == c))
                .map(|t| format!("{}.{}", t.db_name, t.table_name))
        }),
    }?;
    let column = def.base.column_names.first().cloned().unwrap_or_default();
    // Carry the options the engine tracks on `IndexMeta`.
    let opt = |k: &str| {
        def.extra
            .iter()
            .find(|(n, _)| n == k)
            .map(|(_, v)| match v {
                infinity_expression::json::Json::Str(s) => s.clone(),
                infinity_expression::json::Json::Bool(b) => b.to_string(),
                infinity_expression::json::Json::Int(i) => i.to_string(),
                other => other.compact(),
            })
    };
    // A functional index keeps the whole expression in `func_col_params`
    // (`IndexSecondaryFunctional::Serialize`), and that is what the engine's
    // planner matches a predicate against — so it is the `IndexMeta::column`.
    let functional = def
        .base
        .index_type
        .eq_ignore_ascii_case("SECONDARYFUNCTIONAL");
    let column = if functional {
        opt("func_col_params").unwrap_or(column)
    } else {
        column
    };
    let mut options: Vec<(String, String)> = Vec::new();
    if let Some(a) = opt("analyzer") {
        options.push(("ANALYZER".to_string(), a));
    }
    if let Some(m) = opt("metric_type") {
        options.push(("METRIC".to_string(), m));
    }
    if let Some(c) = opt("cardinality") {
        options.push(("CARDINALITY".to_string(), c));
    }
    // The HNSW parameters (`IndexHnsw::Serialize`,
    // `index_hnsw_impl.cpp:278-290`). Without them the record's `M` /
    // `ef_construction` / `encode_type` / `build_type` are lost, and the next
    // write falls back to the defaults - so an index created with
    // `ef_construction = 20` would be rewritten with 200, and the HNSW chunk
    // would carry a different header than the reference's.
    //
    // The names are the ones `index_def_for` reads back, so the round trip is
    // stable: a record read and written again is byte for byte the same.
    if let Some(m) = opt("M") {
        options.push(("M".to_string(), m));
    }
    if let Some(e) = opt("ef_construction") {
        options.push(("EF_CONSTRUCTION".to_string(), e));
    }
    if let Some(e) = opt("encode_type") {
        options.push(("ENCODE".to_string(), e));
    }
    if let Some(b) = opt("build_type") {
        options.push(("BUILD_TYPE".to_string(), b));
    }
    if let Some(b) = opt("block_size") {
        options.push(("BLOCK_SIZE".to_string(), b));
    }
    if let Some(l) = opt("lsg_config") {
        options.push(("LSG_CONFIG".to_string(), l));
    }
    // `IndexIVF::Serialize` writes one nested `ivf_option` object, and
    // `index_def_for` reads it back as individual options. Expanding it here
    // keeps a reloaded record byte-identical - and keeps the storage type:
    // without it `ivf_storage_option` sees "plain", so reopening a directory
    // rebuilt a quantized index with the plain chunk layout.
    if let Some(option) = def
        .extra
        .iter()
        .find(|(name, _)| name == "ivf_option")
        .map(|(_, value)| value)
    {
        let field = json_field;
        let number = |v: Option<&Json>| -> Option<String> {
            v.map(|v| match v {
                Json::Int(i) => i.to_string(),
                Json::Float(f) => f.to_string(),
                other => other.compact(),
            })
        };
        let centroid = field(Some(option), "centroid_option_");
        let storage = field(Some(option), "storage_option_");
        if let Some(metric) = field(Some(option), "metric_").and_then(|v| match v {
            Json::Int(i) => Some(*i),
            _ => None,
        }) {
            options.push((
                "METRIC".to_string(),
                value_format::metric_name(metric).to_string(),
            ));
        }
        if let Some(ratio) = number(field(centroid, "centroids_num_ratio_")) {
            options.push(("CENTROIDS_NUM_RATIO".to_string(), ratio));
        }
        if let Some(v) = number(field(centroid, "min_points_per_centroid_")) {
            options.push(("MIN_POINTS_PER_CENTROID".to_string(), v));
        }
        if let Some(v) = number(field(centroid, "max_points_per_centroid_")) {
            options.push(("MAX_POINTS_PER_CENTROID".to_string(), v));
        }
        let storage_type = field(storage, "type_").and_then(|v| match v {
            Json::Int(i) => Some(*i),
            _ => None,
        });
        match storage_type {
            Some(value_format::ivf_storage::SCALAR_QUANTIZATION) => {
                options.push((
                    "STORAGE_TYPE".to_string(),
                    "scalar_quantization".to_string(),
                ));
                if let Some(v) = number(field(storage, "scalar_quantization_bits_")) {
                    options.push(("SCALAR_QUANTIZATION_BITS".to_string(), v));
                }
            }
            Some(value_format::ivf_storage::PRODUCT_QUANTIZATION) => {
                options.push((
                    "STORAGE_TYPE".to_string(),
                    "product_quantization".to_string(),
                ));
                if let Some(v) = number(field(storage, "product_quantization_subspace_num_")) {
                    options.push(("PRODUCT_QUANTIZATION_SUBSPACE_NUM".to_string(), v));
                }
                if let Some(v) = number(field(storage, "product_quantization_subspace_bits_")) {
                    options.push(("PRODUCT_QUANTIZATION_SUBSPACE_BITS".to_string(), v));
                }
            }
            _ => {
                options.push(("STORAGE_TYPE".to_string(), "plain".to_string()));
                if let Some(code) = field(storage, "plain_storage_data_type_").and_then(|v| match v
                {
                    Json::Int(i) => Some(*i),
                    _ => None,
                }) {
                    options.push((
                        "PLAIN_STORAGE_DATA_TYPE".to_string(),
                        ivf_element_name(code).to_string(),
                    ));
                }
            }
        }
    }
    Some(IndexMeta {
        name: def.base.index_name.clone(),
        table,
        column,
        // The engine's `USING` kind: a functional index is a plain secondary one,
        // and `index_def_for` must recognise it to write it back.
        index_type: if functional {
            "SECONDARY".to_string()
        } else {
            def.base.index_type.clone()
        },
        options,
        // The id the reference's record carries; `Catalog::cpp_indexes` fills
        // in a fallback for a record that has none.
        id: def.index_id.unwrap_or(0),
    })
}

/// Rebuilds `TableMeta`s from a C++-layout catalog (for comparison with the
/// engine's own view).
pub fn read_cpp_tables_as_meta(store: &dyn MetaStore) -> Result<Vec<TableMeta>> {
    let mut out = Vec::new();
    for t in read_cpp_layout(store)? {
        // `type_text_of` carries `WITH (SORTED)` for a kSorted sparse column;
        // split it back into the bare type text plus the hint field.
        let declared: Vec<(String, Option<SparseStore>)> = t
            .columns
            .iter()
            .map(|c| {
                let text = type_text_of(&c.column_type).unwrap_or_default();
                let (bare, store) = split_sparse_store_suffix(&text);
                (bare.to_string(), store)
            })
            .collect();
        out.push(TableMeta {
            id: t.table_id,
            name: format!("{}.{}", t.db_name, t.table_name),
            columns: t.columns.iter().map(|c| c.column_name.clone()).collect(),
            // Carry everything the record holds, so a reader comparing views
            // sees the same schema (types, NOT NULL, defaults, comments).
            defaults: t
                .columns
                .iter()
                .map(|c| c.default.as_ref().and_then(default_text_of))
                .collect(),
            not_nulls: t.columns.iter().map(is_not_null).collect(),
            types: declared.iter().map(|(text, _)| text.clone()).collect(),
            comments: t.columns.iter().map(|c| c.column_comment.clone()).collect(),
            ids: t
                .columns
                .iter()
                .map(|c| c.column_id.max(0) as u64)
                .collect(),
            sparse_stores: declared.into_iter().map(|(_, store)| store).collect(),
        });
    }
    Ok(out)
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_storage::FjallMetaStore;
    use std::sync::Arc;

    /// An IVF record has to survive a reload: `index_meta_of` expands the
    /// nested `ivf_option` back into the options `index_def_for` reads, so a
    /// reopened directory rewrites the same record - and, more to the point,
    /// keeps `storage_type`, without which a quantized index was rebuilt with
    /// the plain chunk layout.
    #[test]
    fn ivf_options_survive_a_reload() {
        let table = CppTable {
            db_name: "default_db".into(),
            db_id: 1,
            table_name: "t".into(),
            table_id: 0,
            columns: vec![ColumnDefValue::new(
                data_type_of_text("EMBEDDING(FLOAT,4)").unwrap(),
                0,
                "v",
            )],
            next_column_id: 1,
        };
        for (storage_type, sq, pqn, pqb) in [
            (value_format::ivf_storage::PLAIN, 0, 0, 0),
            (value_format::ivf_storage::SCALAR_QUANTIZATION, 8, 0, 0),
            (value_format::ivf_storage::PRODUCT_QUANTIZATION, 0, 2, 8),
        ] {
            let plain = if storage_type == value_format::ivf_storage::PLAIN {
                value_format::element::FLOAT32
            } else {
                value_format::element::INVALID
            };
            let mut def = IndexDefValue::ivf(
                "ivf",
                "t",
                "v",
                value_format::metric::L2,
                value_format::IvfCentroid::default(),
                storage_type,
                plain,
                sq,
                pqn,
                pqb,
            );
            def.owner = Some(("1".into(), "0".into()));
            let meta = index_meta_of(&def, std::slice::from_ref(&table)).expect("index meta");
            let back = index_def_for(&meta, &table).expect("index def");
            assert_eq!(
                value_format::index_def_bytes(&back),
                value_format::index_def_bytes(&def),
                "storage_type {storage_type}"
            );
        }
    }

    #[test]
    fn data_type_mapping_covers_the_common_shapes() {
        assert_eq!(
            data_type_of_text("INTEGER").unwrap().compact(),
            r#"{"data_type":3}"#
        );
        assert_eq!(
            data_type_of_text("EMBEDDING(FLOAT,4)").unwrap().compact(),
            r#"{"data_type":23,"type_info":{"dimension":4,"embedding_type":5}}"#
        );
        assert_eq!(
            data_type_of_text("ARRAY(INTEGER)").unwrap().compact(),
            r#"{"data_type":15,"type_info":{"data_type":3}}"#
        );
        assert_eq!(
            data_type_of_text("DECIMAL(18,2)").unwrap().compact(),
            r#"{"data_type":6,"type_info":{"precision":18,"scale":2}}"#
        );
        // Unknown shapes are reported instead of guessed.
        assert!(data_type_of_text("MYSTERY").is_none());
        assert!(data_type_of_text("EMBEDDING(BIT,8)").is_some());
        // SPARSE carries the `SparseInfo` type_info (index type from the bound).
        assert_eq!(
            data_type_of_text("SPARSE(FLOAT,100)").unwrap().compact(),
            r#"{"data_type":30,"type_info":{"data_type":5,"dimension":100,"index_type":1,"sort":0}}"#
        );
        // `WITH (SORTED)` selects the C++ `kSorted` flag; `WITH (sort)` keeps
        // the default.
        assert_eq!(
            data_type_of_text("SPARSE(FLOAT,100) WITH (SORTED)")
                .unwrap()
                .compact(),
            r#"{"data_type":30,"type_info":{"data_type":5,"dimension":100,"index_type":1,"sort":1}}"#
        );
        assert_eq!(
            data_type_of_text("SPARSE(FLOAT,100) WITH (sort)")
                .unwrap()
                .compact(),
            r#"{"data_type":30,"type_info":{"data_type":5,"dimension":100,"index_type":1,"sort":0}}"#
        );
    }

    #[test]
    fn sparse_sorted_hint_round_trips_through_the_cpp_layout() {
        let tmp = tempfile::tempdir().unwrap();
        let cat = Catalog::open(tmp.path()).unwrap();
        cat.create_table_typed_with_sparse_stores(
            "default_db.sp",
            &["id".to_string(), "v".to_string()],
            &[None, None],
            &[false, false],
            &["INTEGER".to_string(), "SPARSE(FLOAT,30000)".to_string()],
            &[None, None],
            &[None, Some(SparseStore::Sorted)],
        )
        .unwrap();

        let out = tempfile::tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(out.path(), "cpp").unwrap());
        cat.write_cpp_layout(store.as_ref()).unwrap();

        let tables = read_cpp_layout(store.as_ref()).unwrap();
        assert_eq!(tables.len(), 1);
        assert_eq!(
            tables[0].columns[1].column_type.compact(),
            r#"{"data_type":30,"type_info":{"data_type":5,"dimension":30000,"index_type":2,"sort":1}}"#
        );
        assert_eq!(
            type_text_of(&tables[0].columns[1].column_type).unwrap(),
            "SPARSE(float,int16,30000) WITH (SORTED)"
        );

        // The read direction restores the bare type text plus the hint field.
        let metas = read_cpp_tables_as_meta(store.as_ref()).unwrap();
        assert_eq!(metas[0].types[1], "SPARSE(float,int16,30000)");
        assert_eq!(metas[0].sparse_stores[1], Some(crate::SparseStore::Sorted));

        // Importing the layout into a fresh native catalog keeps the hint.
        let dst = tempfile::tempdir().unwrap();
        let imported = Catalog::open(dst.path()).unwrap();
        imported.import_cpp_layout(store.as_ref()).unwrap();
        let meta = imported
            .get_table_by_name("default_db.sp")
            .unwrap()
            .unwrap();
        assert_eq!(meta.types[1], "SPARSE(float,int16,30000)");
        assert_eq!(meta.sparse_stores[1], Some(crate::SparseStore::Sorted));
    }

    #[test]
    fn functional_indexes_round_trip() {
        let tmp = tempfile::tempdir().unwrap();
        let cat = Catalog::open(tmp.path()).unwrap();
        let cols = ["c1".to_string(), "c2".to_string()];
        let types = ["INTEGER".to_string(), "VARCHAR".to_string()];
        cat.create_table_typed(
            "default_db.t",
            &cols,
            &[None, None],
            &[false, false],
            &types,
            &[None, None],
        )
        .unwrap();
        // A functional index keeps the expression in `IndexMeta::column`.
        cat.create_index_typed("idx1", "default_db.t", "sqrt(c1)", "", &[])
            .unwrap();

        let out = tempfile::tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(out.path(), "cpp").unwrap());
        cat.write_cpp_layout(store.as_ref()).unwrap();

        let defs = read_cpp_indexes(store.as_ref()).unwrap();
        assert_eq!(defs.len(), 1, "the functional index must not be dropped");
        assert_eq!(defs[0].base.index_type, "SECONDARYFUNCTIONAL");
        assert_eq!(defs[0].base.column_names, vec!["c1".to_string()]);

        // Reading it back restores the expression the planner matches on.
        let tables = read_cpp_layout(store.as_ref()).unwrap();
        let meta = index_meta_of(&defs[0], &tables).unwrap();
        assert_eq!(meta.name, "idx1");
        assert_eq!(meta.table, "default_db.t");
        assert_eq!(meta.column, "sqrt(c1)");
        assert_eq!(meta.index_type, "SECONDARY");
    }

    #[test]
    fn cpp_layout_roundtrips_a_catalog() {
        let tmp = tempfile::tempdir().unwrap();
        let cat = Catalog::open(tmp.path()).unwrap();
        let columns = ["id", "name", "v"].map(String::from).to_vec();
        let types = ["INTEGER", "VARCHAR", "EMBEDDING(FLOAT,3)"]
            .map(String::from)
            .to_vec();
        let t = cat
            .create_table_typed(
                "default_db.t",
                &columns,
                &[None, None, None],
                &[false, false, false],
                &types,
                &[None, None, None],
            )
            .unwrap();

        // Write into a *separate* store so the engine's own catalog is untouched.
        let out = tempfile::tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(out.path(), "cpp").unwrap());
        let written = cat.write_cpp_layout(store.as_ref()).unwrap();
        assert!(written >= 4, "wrote {written} records");

        // The C++ keys are there. The next-table-id counter is a *per
        // database* tag (`db|{db_id}|next_table_id`,
        // `db_meta_impl.cpp:237`), not a system tag - the reference has no
        // top-level `next_table_id`.
        assert_eq!(
            store
                .get(key::catalog_db_tag_key("1", key::NEXT_TABLE_ID).as_bytes())
                .unwrap()
                .map(|v| String::from_utf8_lossy(&v).to_string()),
            Some((t.id + 1).to_string())
        );
        assert!(store.get(key::NEXT_TABLE_ID.as_bytes()).unwrap().is_none());
        // `default_db` is the first database, so its reference id is 1.
        assert!(!store
            .scan_prefix(key::catalog_db_table_prefix("1").as_bytes())
            .unwrap()
            .is_empty());

        let back = read_cpp_layout(store.as_ref()).unwrap();
        assert_eq!(back.len(), 1);
        let got = &back[0];
        assert_eq!(got.db_name, "default_db");
        assert_eq!(got.table_name, "t");
        assert_eq!(got.table_id, t.id);
        assert_eq!(
            got.columns
                .iter()
                .map(|c| c.column_name.clone())
                .collect::<Vec<_>>(),
            vec!["id", "name", "v"]
        );
        // The embedding column kept its type_info, byte for byte.
        assert_eq!(
            got.columns[2].column_type.compact(),
            r#"{"data_type":23,"type_info":{"dimension":3,"embedding_type":5}}"#
        );
        assert_eq!(got.next_column_id, 3);
    }

    #[test]
    fn type_text_of_inverts_data_type_of_text() {
        // `type_text_of` emits the **engine's** spellings, so a C++-style source
        // text canonicalises to it (and the canonical form is a fixed point).
        for (text, canonical) in [
            ("INTEGER", "INTEGER"),
            ("VARCHAR", "VARCHAR"),
            ("EMBEDDING(FLOAT32,4)", "EMBEDDING(float,4)"),
            ("TENSOR(FLOAT32,3)", "TENSOR(float,3)"),
            ("ARRAY(INTEGER)", "ARRAY(INTEGER)"),
            ("DECIMAL(18,2)", "DECIMAL(18,2)"),
            ("SPARSE(FLOAT32,100)", "SPARSE(float,int8,100)"),
            ("SPARSE(DOUBLE,30000)", "SPARSE(double,int16,30000)"),
        ] {
            let j = data_type_of_text(text).unwrap();
            assert_eq!(type_text_of(&j).unwrap(), canonical, "{text}");
            let j2 = data_type_of_text(canonical).unwrap();
            assert_eq!(type_text_of(&j2).unwrap(), canonical, "{canonical}");
        }
    }

    #[test]
    fn import_cpp_layout_rebuilds_a_native_catalog() {
        // Write a catalog into the C++ key space (as the C++ engine would),
        // then import it into a fresh native catalog and compare.
        let src = tempfile::tempdir().unwrap();
        let cat = Catalog::open(src.path()).unwrap();
        let columns = ["id", "name", "v"].map(String::from).to_vec();
        let types = ["INTEGER", "VARCHAR", "EMBEDDING(FLOAT,3)"]
            .map(String::from)
            .to_vec();
        cat.create_table_typed(
            "default_db.t",
            &columns,
            &[None, Some("'x'".to_string()), None],
            &[true, false, false],
            &types,
            &[None, Some("note".to_string()), None],
        )
        .unwrap();
        cat.create_index_typed(
            "idx_ft",
            "default_db.t",
            "name",
            "FULLTEXT",
            &[("ANALYZER".to_string(), "standard".to_string())],
        )
        .unwrap();

        let cpp = tempfile::tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(cpp.path(), "cpp").unwrap());
        cat.write_cpp_layout(store.as_ref()).unwrap();

        // A fresh, empty native catalog imports it.
        let dst = tempfile::tempdir().unwrap();
        let imported = Catalog::open(dst.path()).unwrap();
        let n = imported.import_cpp_layout(store.as_ref()).unwrap();
        assert_eq!(n, 1);

        assert!(imported
            .list_databases()
            .unwrap()
            .contains(&"default_db".to_string()));
        let t = imported.get_table_by_name("default_db.t").unwrap().unwrap();
        assert_eq!(t.columns, columns);
        // Types round-trip through the reverse mapping.
        assert_eq!(t.types[0], "INTEGER");
        assert_eq!(t.types[1], "VARCHAR");
        assert_eq!(t.types[2], "EMBEDDING(float,3)");
        // NOT NULL, comments and defaults come across too.
        assert_eq!(t.not_nulls, vec![true, false, false]);
        assert_eq!(t.comments[1], Some("note".to_string()));
        // The default comes back in the engine's unquoted `.slt` form, which is
        // what `default_literal` reads as a string.
        assert_eq!(t.defaults[1], Some("x".to_string()));
        // The index record is imported with its options.
        let idx = imported.list_indexes().unwrap();
        assert_eq!(idx.len(), 1);
        assert_eq!(idx[0].name, "idx_ft");
        assert_eq!(idx[0].index_type, "FULLTEXT");
        assert_eq!(idx[0].table, "default_db.t");
        assert!(idx[0]
            .options
            .iter()
            .any(|(k, v)| k == "ANALYZER" && v == "standard"));

        // Importing again is a no-op (the tables already exist).
        assert_eq!(imported.import_cpp_layout(store.as_ref()).unwrap(), 0);

        // The direct reader agrees with the import: types, NOT NULL, defaults
        // and comments all survive the C++ round trip.
        let meta = read_cpp_tables_as_meta(store.as_ref()).unwrap();
        assert_eq!(meta.len(), 1);
        assert_eq!(meta[0].name, "default_db.t");
        assert_eq!(
            meta[0].types,
            vec!["INTEGER", "VARCHAR", "EMBEDDING(float,3)"]
        );
        assert_eq!(meta[0].not_nulls, vec![true, false, false]);
        assert_eq!(meta[0].defaults[1], Some("x".to_string()));
        assert_eq!(meta[0].comments[1], Some("note".to_string()));
    }

    #[test]
    fn unsupported_column_type_is_reported() {
        let tmp = tempfile::tempdir().unwrap();
        let cat = Catalog::open(tmp.path()).unwrap();
        cat.create_table_typed(
            "default_db.t",
            &["x".to_string()],
            &[None],
            &[false],
            &["MYSTERY".to_string()],
            &[None],
        )
        .unwrap();
        let out = tempfile::tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(out.path(), "cpp").unwrap());
        let err = cat.write_cpp_layout(store.as_ref()).unwrap_err();
        assert!(err.to_string().contains("does not cover"), "{err}");
    }

    #[test]
    fn index_records_are_written_in_the_cpp_layout() {
        let tmp = tempfile::tempdir().unwrap();
        let cat = Catalog::open(tmp.path()).unwrap();
        let columns = ["id", "v", "body"].map(String::from).to_vec();
        let types = ["INTEGER", "EMBEDDING(FLOAT,4)", "VARCHAR"]
            .map(String::from)
            .to_vec();
        cat.create_table_typed(
            "default_db.t",
            &columns,
            &[None, None, None],
            &[false, false, false],
            &types,
            &[None, None, None],
        )
        .unwrap();
        cat.create_index_typed(
            "idx_vec",
            "default_db.t",
            "v",
            "HNSW",
            &[("METRIC".to_string(), "l2".to_string())],
        )
        .unwrap();
        cat.create_index_typed(
            "idx_ft",
            "default_db.t",
            "body",
            "FULLTEXT",
            &[("ANALYZER".to_string(), "standard".to_string())],
        )
        .unwrap();

        let out = tempfile::tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(out.path(), "cpp").unwrap());
        cat.write_cpp_layout(store.as_ref()).unwrap();

        // The `next_index_id` tag counts both indexes.
        let db = cat.list_databases().unwrap();
        let db_id = (db.iter().position(|d| d == "default_db").unwrap() + 1).to_string();
        let tables = cat.list_tables().unwrap();
        let tbl_id = tables[0].id.to_string();
        assert_eq!(
            store
                .get(key::catalog_table_tag_key(&db_id, &tbl_id, "next_index_id").as_bytes())
                .unwrap()
                .map(|v| String::from_utf8_lossy(&v).to_string()),
            Some("2".to_string())
        );

        let defs = read_cpp_indexes(store.as_ref()).unwrap();
        assert_eq!(defs.len(), 2);
        let hnsw = defs
            .iter()
            .find(|d| d.base.index_name == "idx_vec")
            .unwrap();
        assert_eq!(hnsw.base.index_type, "HNSW");
        assert_eq!(hnsw.base.file_name, "t_idx_vec");
        assert_eq!(hnsw.base.column_names, vec!["v".to_string()]);
        let ft = defs.iter().find(|d| d.base.index_name == "idx_ft").unwrap();
        assert_eq!(ft.base.index_type, "FULLTEXT");
        assert_eq!(ft.base.file_name, "t_idx_ft");
        assert_eq!(ft.base.column_names, vec!["body".to_string()]);
    }
}

#[cfg(test)]
mod empty_default_probe {
    #[test]
    fn empty_string_default_is_a_constant() {
        let j = super::default_constant_expr("");
        assert!(j.is_some(), "empty default must not be dropped");
        let text = super::default_text_of(j.as_ref().unwrap());
        assert_eq!(text.as_deref(), Some(""));
        // through the stored bytes
        let mut v =
            super::ColumnDefValue::new(super::data_type_of_text("VARCHAR").unwrap(), 0, "c");
        v.default = j.clone();
        let bytes = super::value_format::column_def_bytes(&v);
        let back = super::value_format::column_def_from_bytes(&bytes).unwrap();
        assert_eq!(
            back.default
                .as_ref()
                .and_then(super::default_text_of)
                .as_deref(),
            Some("")
        );
    }
}

/// A `MetaStore` that records writes instead of applying them, so the C++ layout
/// writer can feed a single `atomic_batch` (see [`Catalog::write_cpp_layout_batch`]).
///
/// The writer only ever calls `put` (plus one `flush`), so the read methods are
/// unreachable in practice; they return "absent" rather than panicking.
struct LayoutBatch(std::sync::Mutex<Vec<crate::BatchItem>>);

impl LayoutBatch {
    fn new() -> Self {
        Self(std::sync::Mutex::new(Vec::new()))
    }

    fn into_items(self) -> Vec<crate::BatchItem> {
        self.0.into_inner().unwrap()
    }
}

impl MetaStore for LayoutBatch {
    fn get(&self, _key: &[u8]) -> Result<Option<Vec<u8>>> {
        Ok(None)
    }

    fn put(&self, key: &[u8], value: &[u8]) -> Result<()> {
        self.0
            .lock()
            .unwrap()
            .push((key.to_vec(), Some(value.to_vec())));
        Ok(())
    }

    fn delete(&self, key: &[u8]) -> Result<()> {
        self.0.lock().unwrap().push((key.to_vec(), None));
        Ok(())
    }

    fn scan_prefix(&self, _prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        Ok(Vec::new())
    }

    fn count_prefix(&self, _prefix: &[u8]) -> Result<usize> {
        Ok(0)
    }

    fn flush(&self) -> Result<()> {
        Ok(())
    }
}
