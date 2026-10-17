//! Writing the catalog in the **C++ layout**: `catalog_format` keys (§8.2) with
//! `value_format` column records (§8.6), so a directory written here is what the
//! C++ engine's catalog expects to read (RUST_REWRITE_PLAN.md §2.6.1 item 4).
//!
//! This is additive: the engine's own catalog storage is untouched, and
//! [`Catalog::write_cpp_layout`] / [`read_cpp_layout`] let us prove the layout
//! round-trips on a real catalog. The C++ global counters
//! (`next_database_id`/`next_table_id`) are written too.

use infinity_common::{Error, Result};
use infinity_expression::json::Json;
use infinity_storage::{catalog_format as key, MetaStore};

use crate::value_format::{
    self, array_type_info, data_type_json, embedding_type_info, logical, ColumnDefValue,
    IndexDefValue,
};
use crate::{Catalog, IndexMeta, TableMeta};

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

/// Maps a declared type text to the C++ `DataType::Serialize()` object.
///
/// Returns `None` for shapes this mapping does not cover yet.
pub fn data_type_of_text(type_text: &str) -> Option<Json> {
    let up = type_text.trim().to_ascii_uppercase();
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
        "DATETIME" | "TIMESTAMP" => plain(logical::DATETIME),
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
            // `SPARSE(<elem>,<bound>)` -> `SparseInfo::Serialize()`
            // (`sparse_info.cpp:103-110`, `GetIndexType` `:44-58`).
            let inner = up.split_once('(')?.1.trim_end_matches(')');
            let mut parts = inner.split(',');
            let elem = parts.next()?.trim().to_string();
            let dim: i64 = parts.next()?.trim().parse().ok()?;
            Some(data_type_json(
                logical::SPARSE,
                Some(value_format::sparse_type_info(
                    element_code(&elem)?,
                    value_format::sparse_index_type(dim),
                    dim,
                    value_format::sparse_store::SORT,
                )),
            ))
        }
        _ => None,
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
        value_format::logical::VARCHAR => "VARCHAR",
        value_format::logical::JSON => "JSON",
        value_format::logical::DATE => "DATE",
        value_format::logical::TIME => "TIME",
        value_format::logical::DATETIME => "DATETIME",
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
            Some(format!("{base}({},{})", element_name(elem)?, dim))
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
            value_format::hnsw_build::PLAIN,
            opt_num("M", value_format::HNSW_M),
            opt_num("EF_CONSTRUCTION", value_format::HNSW_EF_CONSTRUCTION),
            opt_num("BLOCK_SIZE", value_format::HNSW_BLOCK_SIZE),
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
            Some(IndexDefValue::ivf_plain(
                &im.name,
                &table.table_name,
                &im.column,
                value_format::metric_code_of(&opt_word("METRIC", "l2")),
                elem,
            ))
        }
        "BMP" => Some(IndexDefValue::bmp(
            &im.name,
            &table.table_name,
            &im.column,
            opt_num("BLOCK_SIZE", 0),
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
        self.write_cpp_layout_from(store, &dbs, &tables, &indexes)
    }

    /// Like [`Self::write_cpp_layout`] but from an explicit snapshot. The
    /// C++-only catalog mode needs this: it mutates the desired lists and then
    /// rewrites the whole layout, and the just-mutated state is not yet readable
    /// back from the store.
    pub fn write_cpp_layout_from(
        &self,
        store: &dyn MetaStore,
        dbs: &[String],
        tables: &[TableMeta],
        indexes: &[IndexMeta],
    ) -> Result<usize> {
        let mut written = 0usize;

        let next_table_id = tables.iter().map(|t| t.id).max().unwrap_or(0) + 1;
        let db_id_of =
            |name: &str| -> u64 { dbs.iter().position(|d| d == name).unwrap_or(0) as u64 };

        for (i, db) in dbs.iter().enumerate() {
            let ts = i as u64 + 1;
            let db_str = i.to_string();
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
            let ts = t.id + 1; // monotonic per write
            let name = table_of(&t.name);

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
                ("comment", 0),
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
                let Some(data_type) = data_type_of_text(&type_text) else {
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
                // both across.
                if t.not_nulls.get(i).copied().unwrap_or(false) {
                    value.constraints = vec![1]; // ConstraintType::kNotNull
                }
                value.default = t
                    .defaults
                    .get(i)
                    .cloned()
                    .flatten()
                    .as_deref()
                    .and_then(default_constant_expr);
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
        }

        // Index records: `idx|{db}|{tbl}|{name}|{ts}` -> index_id (a per-table
        // counter starting at 1, as the C++ engine hands out), plus an
        // `index_base` tag holding the C++
        // `IndexBase::Serialize()` JSON (`table_index_meta_impl.cpp:90-97`).
        for im in indexes {
            let im_table = table_of(&im.table);
            let im_db = db_of(&im.table);
            let Some((db_str, tbl_str, ts, cpp)) = resolved
                .iter()
                .find(|(_, _, _, c)| c.table_name == im_table && c.db_name == im_db)
            else {
                continue; // the index's table is not in the written layout
            };
            let Some(def) = index_def_for(im, cpp) else {
                continue;
            };
            // `list_indexes` is ordered, so ids are stable across runs.
            let index_id = indexes
                .iter()
                .filter(|o| table_of(&o.table) == im_table && db_of(&o.table) == im_db)
                .position(|o| o.name == im.name)
                .map_or(1, |p| p + 1);
            store.put(
                key::catalog_index_key(db_str, tbl_str, &im.name, *ts).as_bytes(),
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

        store.put(
            key::NEXT_DATABASE_ID.as_bytes(),
            dbs.len().to_string().as_bytes(),
        )?;
        store.put(
            key::NEXT_TABLE_ID.as_bytes(),
            next_table_id.to_string().as_bytes(),
        )?;
        written += 2;
        store.flush()?;
        Ok(written)
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
            self.create_index_typed(
                &meta.name,
                &meta.table,
                &meta.column,
                &meta.index_type,
                &meta.options,
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
            if let Some(def) = value_format::index_def_from_bytes(&bytes) {
                out.push(def);
            }
        }
    }
    out.sort_by(|a, b| a.base.index_name.cmp(&b.base.index_name));
    Ok(out)
}

/// Maps a C++ index record onto the engine's `IndexMeta`, resolving the owning
/// table from the C++ tables (which carry db/table names and columns).
pub fn index_meta_of(def: &IndexDefValue, cpp_tables: &[CppTable]) -> Option<IndexMeta> {
    let table = def.base.column_names.first().and_then(|c| {
        cpp_tables
            .iter()
            .find(|t| t.columns.iter().any(|col| &col.column_name == c))
            .map(|t| format!("{}.{}", t.db_name, t.table_name))
    })?;
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
    })
}

/// Rebuilds `TableMeta`s from a C++-layout catalog (for comparison with the
/// engine's own view).
pub fn read_cpp_tables_as_meta(store: &dyn MetaStore) -> Result<Vec<TableMeta>> {
    let mut out = Vec::new();
    for t in read_cpp_layout(store)? {
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
            types: t
                .columns
                .iter()
                .map(|c| type_text_of(&c.column_type).unwrap_or_default())
                .collect(),
            comments: t.columns.iter().map(|c| c.column_comment.clone()).collect(),
            ids: t.columns.iter().map(|c| c.column_id.max(0) as u64).collect(),
        });
    }
    Ok(out)
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_storage::FjallMetaStore;
    use std::sync::Arc;

    #[test]
    fn data_type_mapping_covers_the_common_shapes() {
        assert_eq!(
            data_type_of_text("INTEGER").unwrap().compact(),
            r#"{"data_type":3}"#
        );
        assert_eq!(
            data_type_of_text("EMBEDDING(FLOAT,4)").unwrap().compact(),
            r#"{"data_type":12,"type_info":{"dimension":4,"embedding_type":6}}"#
        );
        assert_eq!(
            data_type_of_text("ARRAY(INTEGER)").unwrap().compact(),
            r#"{"data_type":22,"type_info":{"data_type":3}}"#
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
            r#"{"data_type":15,"type_info":{"data_type":6,"dimension":100,"index_type":2,"sort":0}}"#
        );
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

        // The C++ keys are there.
        assert_eq!(
            store
                .get(key::NEXT_TABLE_ID.as_bytes())
                .unwrap()
                .map(|v| String::from_utf8_lossy(&v).to_string()),
            Some((t.id + 1).to_string())
        );
        assert!(!store
            .scan_prefix(key::catalog_db_table_prefix("0").as_bytes())
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
            r#"{"data_type":12,"type_info":{"dimension":3,"embedding_type":6}}"#
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
            ("SPARSE(FLOAT32,100)", "SPARSE(float,100)"),
            ("SPARSE(DOUBLE,30000)", "SPARSE(double,30000)"),
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
        let db_id = db
            .iter()
            .position(|d| d == "default_db")
            .unwrap()
            .to_string();
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
