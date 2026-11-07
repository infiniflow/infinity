//! The WAL bridge: every `WalCmd` the engine writes, and the replay that puts a
//! recovered directory back the way the reference does.
//!
//! Two directions live here because they are the same mapping read backwards:
//!
//! * **out** - a DDL turns its catalog batch into the reference's typed command
//!   (`create_table_command_from_batch` and friends), so the log matches the
//!   reference frame for frame;
//! * **in** - a replayed command re-creates the catalog records it described.
//!
//! Replay is not DDL-only: the log also carries DML. `Append`/`Delete` reach the
//! blocks through `TxnManager::recover_into` + `apply_replayed_rows` (the block
//! files keep the row data, the command names the commit ts and the slots), and
//! `Import`/`Compact`/`DumpIndex` describe the segment and chunk metadata that
//! the catalog store keeps log-less, so those have to be replayed here too.
//!
//! The build direction starts from the per-column catalog records.
//! The per-column record (`tbl|col|{db}|{tbl}|{name}|{ts}`) is JSON:
//! ```json
//! {"column_id":0,"column_name":"id",
//!  "column_type":{"data_type":3},"default":{"type":4}}
//! ```
//! `data_type` is the `LogicalType` discriminant; `type_info` carries the
//! embedding/sparse/decimal payload. The default's `type` is a `LiteralType`
//! discriminant (`4` = null).
//!
//! Uses the crate's own JSON (`expression::json`) — the workspace
//! does not depend on `serde_json`.

use crate::engine::state::default_literal;
use crate::Value;
use expression::json::{extract, parse, Json};
use storage::wal::ddl_def::{
    ColumnDef, ConstantExpr, ConstraintType, DataType, EmbeddingDataType, LogicalType,
    SparseStoreType, TableDef, TypeInfo,
};

fn as_i64(j: Option<&Json>) -> Option<i64> {
    match j {
        Some(Json::Int(v)) => Some(*v),
        Some(Json::Float(v)) => Some(*v as i64),
        _ => None,
    }
}

fn as_str(j: Option<&Json>) -> Option<&str> {
    match j {
        Some(Json::Str(s)) => Some(s.as_str()),
        _ => None,
    }
}

/// One column's catalog record, decoded.
pub struct ColumnRecord {
    doc: Json,
}

impl ColumnRecord {
    /// Parses the stored JSON of one `tbl|col|…` record.
    pub fn from_json(text: &str) -> Option<Self> {
        Some(Self { doc: parse(text)? })
    }

    pub fn id(&self) -> i64 {
        as_i64(extract(&self.doc, ".column_id")).unwrap_or(0)
    }

    pub fn name(&self) -> String {
        as_str(extract(&self.doc, ".column_name"))
            .unwrap_or("")
            .to_string()
    }

    pub fn to_column_def(&self) -> ColumnDef {
        ColumnDef {
            id: self.id(),
            column_type: self.column_type(),
            name: self.name(),
            constraints: vec![],
            comment: String::new(),
            default_expr: self.default_expr(),
            build_bloom_filter: false,
        }
    }

    fn column_type(&self) -> DataType {
        data_type_from_json(extract(&self.doc, ".column_type"))
    }

    fn default_expr(&self) -> ConstantExpr {
        let literal = as_i64(extract(&self.doc, ".default.type")).unwrap_or(4);
        let value = extract(&self.doc, ".default.value");
        match literal {
            0 => ConstantExpr::Boolean(matches!(value, Some(Json::Bool(true)))),
            1 => ConstantExpr::Double(as_i64(value).unwrap_or(0) as f64),
            2 => ConstantExpr::String(as_str(value).unwrap_or("").to_string()),
            3 => ConstantExpr::Integer(as_i64(value).unwrap_or(0)),
            _ => ConstantExpr::Null,
        }
    }
}

/// Builds a `TableDef` from the table's per-column records (ordered by id).
pub fn table_def_from_columns(
    schema_name: &str,
    table_name: &str,
    columns: &[ColumnRecord],
) -> TableDef {
    TableDef {
        schema_name: schema_name.to_string(),
        table_name: table_name.to_string(),
        table_comment: String::new(),
        columns: columns.iter().map(ColumnRecord::to_column_def).collect(),
    }
}

use storage::WalCmd;

/// Builds a reference `DataType` from the catalog's `column_type` object
/// (`{"data_type": <LogicalType>, "type_info": {...}}`).
pub fn data_type_from_json(doc: Option<&Json>) -> DataType {
    let doc = doc.cloned().unwrap_or(Json::Null);
    let logical_type =
        LogicalType::from_discriminant(as_i64(extract(&doc, ".data_type")).unwrap_or(36) as i8);
    let info = |key: &str| as_i64(extract(&doc, &format!(".type_info.{key}")));
    let type_info = match logical_type {
        LogicalType::Embedding
        | LogicalType::Tensor
        | LogicalType::TensorArray
        | LogicalType::MultiVector => TypeInfo::Embedding {
            element_type: EmbeddingDataType::from_discriminant(
                info("embedding_type").unwrap_or(10) as i8,
            ),
            dimension: info("dimension").unwrap_or(0) as i32,
        },
        LogicalType::Sparse => TypeInfo::Sparse {
            data_type: EmbeddingDataType::from_discriminant(info("data_type").unwrap_or(10) as i8),
            index_type: EmbeddingDataType::from_discriminant(info("index_type").unwrap_or(10) as i8),
            dimension: info("dimension").unwrap_or(0),
            store_type: SparseStoreType::from_discriminant(info("sort").unwrap_or(2) as i8),
        },
        LogicalType::Decimal => TypeInfo::Decimal {
            precision: info("precision").unwrap_or(0),
            scale: info("scale").unwrap_or(0),
        },
        _ => TypeInfo::None,
    };
    DataType {
        logical_type,
        type_info,
    }
}

/// [`data_type_from_json`] from the declared type text (`VECTOR,4,FLOAT`).
pub fn data_type_of_text(text: &str) -> Option<DataType> {
    let doc = catalog::layout::data_type_of_text(text)?;
    Some(data_type_from_json(Some(&doc)))
}

/// Detects a `CREATE TABLE` batch and turns it into the reference's typed
/// `WalCmdCreateTableV2`, so the WAL carries the reference bytes instead of a
/// `KvBatch`. Returns `None` for any other DDL (CreateIndex, Drop, …).
///
/// The catalog writes are still applied to the store by `commit_at`; replay
/// deliberately skips typed DDL (the catalog is durable at commit), so this
/// only changes the WAL's bytes.
/// `table_name` is the table the statement created: the batch holds the whole
/// C++ layout (the DDL mirrors it, `Catalog::capture`), so "the last table
/// record" is not the new one as soon as the catalog has more than one.
pub fn create_table_command_from_batch(
    batch: &[(Vec<u8>, Option<Vec<u8>>)],
    table_name: &str,
) -> Option<WalCmd> {
    // catalog|tbl|{db_id}|{name}|{ts}; the table id is the value.
    let prefix = "catalog|tbl|";
    let mut hit: Option<(String, String)> = None;
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        let Some(rest) = key.strip_prefix(prefix) else {
            continue;
        };
        let parts: Vec<&str> = rest.split('|').collect();
        if parts.len() < 3 || parts[1] != table_name {
            continue;
        }
        let Some(value) = v.as_ref() else { continue };
        hit = Some((
            parts[0].to_string(),
            String::from_utf8_lossy(value).into_owned(),
        ));
    }
    let (db_id, table_id) = hit?;
    let mut cols: Vec<(i64, ColumnRecord)> = Vec::new();
    let has_index = batch.iter().any(|(k, v)| {
        v.is_some()
            && std::str::from_utf8(k)
                .map(|key| key.starts_with(&format!("catalog|idx|{db_id}|{table_id}|")))
                .unwrap_or(false)
    });
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        if !key.starts_with(&format!("tbl|col|{db_id}|{table_id}|")) {
            continue;
        }
        if let Some(value) = v.as_ref() {
            if let Ok(text) = std::str::from_utf8(value) {
                if let Some(rec) = ColumnRecord::from_json(text) {
                    cols.push((rec.id(), rec));
                }
            }
        }
    }
    if has_index || cols.is_empty() {
        return None;
    }
    let table_name = table_name.to_string();
    cols.sort_by_key(|(id, _)| *id);
    let ordered: Vec<ColumnRecord> = cols.into_iter().map(|(_, r)| r).collect();
    let td = table_def_from_columns("default_db", &table_name, &ordered);
    Some(WalCmd::CreateTable {
        db_name: "default_db".to_string(),
        db_id,
        table_id,
        table_def: td,
    })
}

/// Maps the port's `idx|{db}|{tbl}|{idx_id}|index_base` JSON to a full
/// reference `IndexDef` (base + the per-type option fields the record carries).
///
/// IVF's `IndexIVFOption` is a 32-byte POD copied as one blob; its five padding
/// bytes are written as zero by both engines (`IndexIVF::WriteAdv` zeroes them),
/// so the payload is comparable. EMVB/PLAID/SMVE write their handful of scalars
/// field by field. DiskANN is not ported, so that kind falls back to the
/// `KvBatch`.
pub fn index_def_from_json(text: &str) -> Option<storage::wal::index_def::IndexDef> {
    use storage::wal::index_def::{
        HnswBuildType, HnswEncodeType, IndexBase, IndexDef, IndexType, IvfStorageType, MetricType,
    };
    let doc = parse(text)?;
    let type_text = as_str(extract(&doc, ".index_type"))?;
    let index_type = match type_text {
        "SECONDARY" => IndexType::Secondary,
        "FULLTEXT" => IndexType::FullText,
        "HNSW" => IndexType::Hnsw,
        "BMP" => IndexType::Bmp,
        "IVF" => IndexType::Ivf,
        "EMVB" => IndexType::Emvb,
        "PLAID" => IndexType::Plaid,
        "SMVE" => IndexType::Smve,
        _ => return None,
    };
    let column_names = match extract(&doc, ".column_names") {
        Some(Json::Arr(items)) => items
            .iter()
            .filter_map(|x| as_str(Some(x)).map(str::to_string))
            .collect(),
        _ => Vec::new(),
    };
    let base = IndexBase {
        index_type,
        index_name: as_str(extract(&doc, ".index_name"))?.to_string(),
        index_comment: as_str(extract(&doc, ".index_comment"))
            .unwrap_or("")
            .to_string(),
        file_name: as_str(extract(&doc, ".file_name"))
            .unwrap_or("")
            .to_string(),
        column_names,
    };
    Some(match index_type {
        IndexType::Secondary => IndexDef::Secondary {
            base,
            cardinality: as_i64(extract(&doc, ".cardinality").or(None))
                .map(|v| v as u8)
                .unwrap_or(0),
            is_json: matches!(extract(&doc, ".is_json_index"), Some(Json::Bool(true))),
        },
        IndexType::FullText => IndexDef::FullText {
            base,
            analyzer: as_str(extract(&doc, ".analyzer")).unwrap_or("").to_string(),
            flag: as_i64(extract(&doc, ".flag")).unwrap_or(0) as u8,
        },
        IndexType::Bmp => IndexDef::Bmp {
            base,
            block_size: as_i64(extract(&doc, ".block_size")).unwrap_or(0) as u64,
            compress_type: as_i64(extract(&doc, ".compress_type")).unwrap_or(0) as i8,
        },
        IndexType::Emvb => IndexDef::Emvb {
            base,
            residual_pq_subspace_num: as_i64(extract(&doc, ".pq_subspace_num")).unwrap_or(0) as u32,
            residual_pq_subspace_bits: as_i64(extract(&doc, ".pq_subspace_bits")).unwrap_or(0)
                as u32,
        },
        IndexType::Plaid => IndexDef::Plaid {
            base,
            nbits: as_i64(extract(&doc, ".nbits")).unwrap_or(4) as u32,
            n_centroids: as_i64(extract(&doc, ".n_centroids")).unwrap_or(0) as u32,
            colbertsar_mode: matches!(extract(&doc, ".colbertsar_mode"), Some(Json::Bool(true))),
        },
        IndexType::Smve => IndexDef::Smve {
            base,
            width: as_i64(extract(&doc, ".width")).unwrap_or(2048) as u32,
            topk: as_i64(extract(&doc, ".topk")).unwrap_or(8) as u32,
        },
        IndexType::Ivf => IndexDef::Ivf {
            base,
            metric_type: match as_i64(extract(&doc, ".ivf_option.metric_")).unwrap_or(3) {
                0 => MetricType::Cosine,
                1 => MetricType::InnerProduct,
                2 => MetricType::L2,
                _ => MetricType::Invalid,
            },
            centroids_num_ratio: match extract(
                &doc,
                ".ivf_option.centroid_option_.centroids_num_ratio_",
            ) {
                Some(Json::Float(v)) => *v as f32,
                Some(Json::Int(v)) => *v as f32,
                _ => 1.0,
            },
            min_points_per_centroid: as_i64(extract(
                &doc,
                ".ivf_option.centroid_option_.min_points_per_centroid_",
            ))
            .unwrap_or(32) as u32,
            max_points_per_centroid: as_i64(extract(
                &doc,
                ".ivf_option.centroid_option_.max_points_per_centroid_",
            ))
            .unwrap_or(256) as u32,
            storage_type: match as_i64(extract(&doc, ".ivf_option.storage_option_.type_"))
                .unwrap_or(0)
            {
                1 => IvfStorageType::ScalarQuantization,
                2 => IvfStorageType::ProductQuantization,
                _ => IvfStorageType::Plain,
            },
            plain_storage_data_type: storage::wal::ddl_def::EmbeddingDataType::from_discriminant(
                as_i64(extract(
                    &doc,
                    ".ivf_option.storage_option_.plain_storage_data_type_",
                ))
                .unwrap_or(10) as i8,
            ),
            scalar_quantization_bits: as_i64(extract(
                &doc,
                ".ivf_option.storage_option_.scalar_quantization_bits_",
            ))
            .unwrap_or(0) as u32,
            product_quantization_subspace_num: as_i64(extract(
                &doc,
                ".ivf_option.storage_option_.product_quantization_subspace_num_",
            ))
            .unwrap_or(0) as u32,
            product_quantization_subspace_bits: as_i64(extract(
                &doc,
                ".ivf_option.storage_option_.product_quantization_subspace_bits_",
            ))
            .unwrap_or(0) as u32,
        },
        IndexType::Hnsw => IndexDef::Hnsw {
            base,
            metric_type: match as_str(extract(&doc, ".metric_type")).unwrap_or("") {
                "l2" => MetricType::L2,
                "ip" => MetricType::InnerProduct,
                "cosine" => MetricType::Cosine,
                _ => MetricType::Invalid,
            },
            encode_type: match as_str(extract(&doc, ".encode_type")).unwrap_or("plain") {
                "plain" => HnswEncodeType::Plain,
                "lvq" => HnswEncodeType::Lvq,
                "rabitq" => HnswEncodeType::Rabitq,
                _ => HnswEncodeType::Invalid,
            },
            build_type: match as_str(extract(&doc, ".build_type")).unwrap_or("plain") {
                "plain" => HnswBuildType::Plain,
                "lsg" => HnswBuildType::Lsg,
                _ => HnswBuildType::Invalid,
            },
            m: as_i64(extract(&doc, ".M")).unwrap_or(0) as u64,
            ef_construction: as_i64(extract(&doc, ".ef_construction")).unwrap_or(0) as u64,
            block_size: as_i64(extract(&doc, ".block_size")).unwrap_or(0) as u64,
            has_lsg: false,
        },
        _ => return None,
    })
}

/// Detects a `CREATE INDEX` batch for a **secondary** index and turns it into
/// the reference's `WalCmdCreateIndexV2`. Other index kinds need their own
/// option structs (`IndexIVFOption`, HNSW M/ef, …), which are not serialized
/// yet, so they fall back to the `KvBatch`.
///
/// `table_name` and `table_key` are not in the batch; the caller supplies them
/// (the reference's `table_key` is the table's own `catalog|tbl|…` key).
/// `index_name` is the index the statement created: the batch carries the whole
/// C++ layout, so the last `catalog|idx|` record is any index, not this one.
pub fn create_index_command_from_batch(
    batch: &[(Vec<u8>, Option<Vec<u8>>)],
    table_name: &str,
    table_key: &str,
    index_name: &str,
) -> Option<WalCmd> {
    use storage::wal::index_def::IndexDef;
    if table_key.is_empty() {
        return None;
    }
    // catalog|idx|{db_id}|{table_id}|{index_name}|{ts}; the index id is the value.
    let mut ids: Option<(String, String, String)> = None;
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        let Some(rest) = key.strip_prefix("catalog|idx|") else {
            continue;
        };
        let parts: Vec<&str> = rest.split('|').collect();
        if parts.len() < 4 || parts[2] != index_name {
            continue;
        }
        let Some(value) = v.as_ref() else { continue };
        ids = Some((
            parts[0].to_string(),
            parts[1].to_string(),
            String::from_utf8_lossy(value).into_owned(),
        ));
    }
    let (db_id, table_id, index_id) = ids?;
    let tag = format!("idx|{db_id}|{table_id}|{index_id}|index_base");
    let mut index_base: Option<IndexDef> = None;
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        if key != tag {
            continue;
        }
        if let Some(value) = v.as_ref() {
            if let Ok(text) = std::str::from_utf8(value) {
                if let Some(def) = index_def_from_json(text) {
                    index_base = Some(def);
                }
            }
        }
    }
    let index_base = index_base?;
    // The database's **name**, which the reference's command carries for the
    // replay to resolve the table by name. The layout rewrite in this batch has
    // the `catalog|db|{name}|{ts}` records, whose value is the id the index's
    // key names.
    let db_name = batch
        .iter()
        .find_map(|(k, v)| {
            let key = std::str::from_utf8(k).ok()?;
            let name = key.strip_prefix("catalog|db|")?.split('|').next()?;
            let value = std::str::from_utf8(v.as_ref()?).ok()?;
            (value == db_id).then(|| name.to_string())
        })
        .unwrap_or_else(|| "default_db".to_string());
    Some(WalCmd::CreateIndex {
        db_name,
        db_id,
        table_name: table_name.to_string(),
        table_id,
        index_id,
        index_base,
        segment_index_infos: Vec::new(),
        table_key: table_key.to_string(),
    })
}

/// Detects a `CREATE DATABASE` batch (`catalog|db|{name}|{ts}` → db id) and
/// builds the reference's `WalCmdCreateDatabaseV2`.
pub fn create_database_command_from_batch(
    batch: &[(Vec<u8>, Option<Vec<u8>>)],
    target: &str,
) -> Option<WalCmd> {
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        if let Some(rest) = key.strip_prefix("catalog|db|") {
            // catalog|db|{name}|{ts}
            let parts: Vec<&str> = rest.split('|').collect();
            if parts.len() >= 2 && parts[0] == target {
                let db_id = v
                    .as_ref()
                    .map(|b| String::from_utf8_lossy(b).into_owned())
                    .unwrap_or_default();
                if !db_id.is_empty() {
                    return Some(WalCmd::CreateDatabase {
                        db_name: parts[0].to_string(),
                        db_id,
                        db_comment: String::new(),
                    });
                }
            }
        }
    }
    None
}

/// Detects an `ADD COLUMNS` batch (its new `tbl|col|…` records) and builds the
/// reference's `WalCmdAddColumnsV2`. `base_index` is the table's column count
/// before the ADD, so the appended columns' positions are `base_index..`.
pub fn add_columns_command_from_batch(
    batch: &[(Vec<u8>, Option<Vec<u8>>)],
    db_id: &str,
    table_id: &str,
    table_name: &str,
    table_key: &str,
    base_index: u32,
    new_names: &[String],
) -> Option<WalCmd> {
    if table_key.is_empty() {
        return None;
    }
    let mut cols: Vec<(i64, ColumnRecord)> = Vec::new();
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        if key.starts_with("tbl|col|") {
            if let Some(Some(val)) = v.as_ref().map(Some) {
                if let Ok(text) = std::str::from_utf8(val) {
                    if let Some(rec) = ColumnRecord::from_json(text) {
                        if new_names.iter().any(|n| n == &rec.name()) {
                            cols.push((rec.id(), rec));
                        }
                    }
                }
            }
        }
    }
    if cols.is_empty() {
        return None;
    }
    cols.sort_by_key(|(id, _)| *id);
    let defs: Vec<_> = cols.into_iter().map(|(_, r)| r.to_column_def()).collect();
    // The reference stores the new columns' **1-based** positions
    // (one existing column -> the first added column is index 2).
    let column_idx_list: Vec<u32> = (0..defs.len() as u32).map(|i| base_index + i + 1).collect();
    Some(WalCmd::AddColumns {
        db_name: "default_db".to_string(),
        db_id: db_id.to_string(),
        table_name: table_name.to_string(),
        table_id: table_id.to_string(),
        column_idx_list,
        column_defs: defs,
        table_key: table_key.to_string(),
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    // The exact JSON the port writes for `tt(id int, name varchar, v
    // vector,4,float, sp sparse,8,float,int, sc float, b bool)`.
    const RECORDS: &[&str] = &[
        r#"{"column_id":0,"column_name":"id","column_type":{"data_type":3},"default":{"type":4}}"#,
        r#"{"column_id":1,"column_name":"name","column_type":{"data_type":9},"default":{"type":4}}"#,
        r#"{"column_id":2,"column_name":"v","column_type":{"data_type":23,"type_info":{"dimension":4,"embedding_type":5}},"default":{"type":4}}"#,
        r#"{"column_id":3,"column_name":"sp","column_type":{"data_type":30,"type_info":{"data_type":5,"dimension":8,"index_type":3,"sort":0}},"default":{"type":4}}"#,
        r#"{"column_id":4,"column_name":"sc","column_type":{"data_type":7},"default":{"type":4}}"#,
        r#"{"column_id":5,"column_name":"b","column_type":{"data_type":0},"default":{"type":4}}"#,
    ];

    #[test]
    fn maps_the_port_catalog_records_to_a_table_def() {
        let cols: Vec<ColumnRecord> = RECORDS
            .iter()
            .map(|s| ColumnRecord::from_json(s).unwrap())
            .collect();
        let td = table_def_from_columns("default_db", "tt", &cols);
        assert_eq!(td.table_name, "tt");
        let kinds: Vec<_> = td
            .columns
            .iter()
            .map(|c| c.column_type.logical_type)
            .collect();
        assert_eq!(
            kinds,
            vec![
                LogicalType::Integer,
                LogicalType::Varchar,
                LogicalType::Embedding,
                LogicalType::Sparse,
                LogicalType::Float,
                LogicalType::Boolean,
            ]
        );
        assert_eq!(
            td.columns[2].column_type.type_info,
            TypeInfo::Embedding {
                element_type: EmbeddingDataType::Float,
                dimension: 4
            }
        );
        assert_eq!(
            td.columns[3].column_type.type_info,
            TypeInfo::Sparse {
                data_type: EmbeddingDataType::Float,
                index_type: EmbeddingDataType::Int32,
                dimension: 8,
                store_type: SparseStoreType::Sort
            }
        );
    }

    #[test]
    fn maps_the_port_index_record_to_an_index_base() {
        use storage::wal::index_def::IndexType;
        let text = r#"{"cardinality":"high","column_names":["name"],"file_name":"ix_sec","index_comment":"","index_name":"sec","index_type":"SECONDARY","is_json_index":false}"#;
        let d = index_def_from_json(text).unwrap();
        match d {
            storage::wal::index_def::IndexDef::Secondary { base, .. } => {
                assert_eq!(base.index_type, IndexType::Secondary);
                assert_eq!(base.index_name, "sec");
                assert_eq!(base.file_name, "ix_sec");
                assert_eq!(base.column_names, vec!["name".to_string()]);
            }
            other => panic!("expected Secondary, got {other:?}"),
        }
    }
}

// ---------------------------------------------------------------------------
// Replay: a recovered directory is put back the way the reference does - the
// catalog store keeps no RocksDB log, so a committed DDL survives a crash only
// because its typed command is replayed here.
// ---------------------------------------------------------------------------

use crate::engine::Engine;
use catalog::{Catalog, TableMeta};
use common::Result as CommonResult;

/// EmbeddingDataType -> the element names accepted by the catalog's type parser.
fn element_text(t: EmbeddingDataType) -> Option<&'static str> {
    Some(match t {
        EmbeddingDataType::Bit => "bit",
        EmbeddingDataType::Int8 => "int8",
        EmbeddingDataType::Int16 => "int16",
        EmbeddingDataType::Int32 => "int32",
        EmbeddingDataType::Int64 => "int64",
        EmbeddingDataType::Float => "float",
        EmbeddingDataType::Double => "double",
        EmbeddingDataType::UInt8 => "uint8",
        EmbeddingDataType::Float16 => "float16",
        EmbeddingDataType::BFloat16 => "bfloat16",
        EmbeddingDataType::Invalid => return None,
    })
}

/// Renders a DataType as the type text the catalog stores.
///
/// The column record keeps the *structured* type, so the text only has to parse
/// back to the same DataType (catalog::layout::data_type_of_text); it does not
/// have to be the spelling the user typed.
pub fn data_type_text(dt: &DataType) -> Option<String> {
    use LogicalType::*;
    let plain = |s: &str| Some(s.to_string());
    match dt.logical_type {
        Boolean => plain("BOOLEAN"),
        TinyInt => plain("TINYINT"),
        SmallInt => plain("SMALLINT"),
        Integer => plain("INTEGER"),
        BigInt => plain("BIGINT"),
        HugeInt => plain("HUGEINT"),
        Float => plain("FLOAT"),
        Double => plain("DOUBLE"),
        Float16 => plain("FLOAT16"),
        BFloat16 => plain("BFLOAT16"),
        Varchar => plain("VARCHAR"),
        Json => plain("JSON"),
        Date => plain("DATE"),
        Time => plain("TIME"),
        DateTime => plain("DATETIME"),
        Timestamp => plain("TIMESTAMP"),
        Uuid => plain("UUID"),
        Decimal => match &dt.type_info {
            TypeInfo::Decimal { precision, scale } => Some(format!("DECIMAL({precision},{scale})")),
            _ => None,
        },
        Array => match &dt.type_info {
            TypeInfo::Array(elem) => Some(format!("ARRAY({})", data_type_text(elem)?)),
            _ => None,
        },
        Embedding | Tensor | TensorArray | MultiVector => {
            let TypeInfo::Embedding {
                element_type,
                dimension,
            } = &dt.type_info
            else {
                return None;
            };
            let base = match dt.logical_type {
                Embedding => "EMBEDDING",
                Tensor => "TENSOR",
                TensorArray => "TENSORARRAY",
                _ => "MULTIVECTOR",
            };
            Some(format!(
                "{base}({},{dimension})",
                element_text(*element_type)?
            ))
        }
        Sparse => {
            let TypeInfo::Sparse {
                data_type,
                index_type,
                dimension,
                store_type,
            } = &dt.type_info
            else {
                return None;
            };
            let text = format!(
                "SPARSE({},{},{dimension})",
                element_text(*data_type)?,
                element_text(*index_type)?
            );
            if *store_type == SparseStoreType::Sorted {
                return Some(format!("{text} WITH (SORTED)"));
            }
            Some(text)
        }
        _ => None,
    }
}

/// A column default, in the .slt text the catalog stores (None = no default,
/// which is also what a NULL literal means).
fn constant_text(c: &ConstantExpr) -> Option<String> {
    match c {
        ConstantExpr::Null => None,
        ConstantExpr::Boolean(b) => Some(if *b { "true" } else { "false" }.to_string()),
        ConstantExpr::Double(d) => Some(format!("{d}")),
        ConstantExpr::Integer(i) => Some(i.to_string()),
        // The stored form is the bare text, not an SQL literal: `TableMeta::
        // defaults` is what `ALTER`/`INSERT` fill a missing cell with, and the
        // live path (the AST's `to_slt_string`) writes it bare. Quoting it here
        // made a replayed directory pad cells with `'default'` (and store that
        // in the column's catalog record).
        ConstantExpr::String(s) => Some(s.clone()),
    }
}

/// Rebuilds the TableMeta a CreateTableV2 command describes.
fn table_meta(db_name: &str, table_id: u64, td: &TableDef) -> Option<TableMeta> {
    let mut meta = TableMeta {
        id: table_id,
        name: format!("{db_name}.{}", td.table_name),
        columns: Vec::new(),
        defaults: Vec::new(),
        not_nulls: Vec::new(),
        types: Vec::new(),
        comments: Vec::new(),
        ids: Vec::new(),
        sparse_stores: Vec::new(),
    };
    for c in &td.columns {
        meta.columns.push(c.name.clone());
        meta.types.push(data_type_text(&c.column_type)?);
        meta.comments
            .push((!c.comment.is_empty()).then(|| c.comment.clone()));
        meta.ids.push(c.id as u64);
        meta.defaults.push(constant_text(&c.default_expr));
        meta.not_nulls
            .push(c.constraints.iter().any(|x| *x == ConstraintType::NotNull));
        meta.sparse_stores
            .push(catalog::split_sparse_store_suffix(meta.types.last()?).1);
    }
    Some(meta)
}

fn qualified(db_name: &str, name: &str) -> String {
    format!("{db_name}.{name}")
}

/// Replays one WAL command into the catalog.
///
/// Row commands (Append/Delete) are skipped: their rows live in the block files
/// and are applied by Engine::apply_replayed_rows in the same recovery pass.
/// Everything else re-creates the catalog records the reference writes inside
/// the command's own transaction - the ids come from the catalog counters, which
/// the checkpoint flushed at the same point the command's transaction saw them,
/// and the commit ts is stamped from the frame.
/// How a schema change reshapes the rows an **earlier** append wrote.
///
/// The replay buffers every row that is not in a block yet and parses it
/// against the schema the replay ends with, so a row written before an
/// ADD/DROP COLUMN has to be reshaped first. The reference gets this for free:
/// its replay writes each append straight into a block, and the later DDL's
/// replay patches that block.
pub enum RowShapeChange {
    /// The added columns' stored cell texts, the first at `first`.
    Added { first: usize, texts: Vec<String> },
    /// The cell at `position` is gone.
    Dropped { position: usize },
}

/// Replays one WAL command into the catalog and the block layer.
///
/// `shapes` collects the schema changes a row still in the log may have to
/// follow (`(commit ts, qualified table, change)`); see [`RowShapeChange`].
pub fn replay(
    engine: &Engine,
    cmd: &WalCmd,
    commit_ts: u64,
    shapes: &mut Vec<(u64, String, RowShapeChange)>,
) -> CommonResult<()> {
    let catalog = engine.catalog();
    // A restore recreates a table (or a whole database) from the snapshot it
    // names and copies its files back. Its catalog writes are in the memtable
    // like any DDL's, so a crash before the next checkpoint leaves them to the
    // log; re-running the restore is idempotent (a table that is already there
    // is skipped). A snapshot that no longer exists leaves the directory
    // readable with the restore's rows missing - refusing the whole open would
    // be worse.
    match cmd {
        WalCmd::RestoreTableSnapshot { snapshot_name, .. } => {
            let _ = engine.apply_table_snapshot(snapshot_name, true);
            return Ok(());
        }
        WalCmd::RestoreDatabaseSnapshot { snapshot_name, .. } => {
            let _ = engine.apply_database_snapshot(snapshot_name, true);
            return Ok(());
        }
        WalCmd::RestoreSystemSnapshot { snapshot_name, .. } => {
            let _ = engine.apply_system_snapshot(snapshot_name, true);
            return Ok(());
        }
        _ => {}
    }
    let mut batch: Vec<(Vec<u8>, Option<Vec<u8>>)> = match cmd {
        WalCmd::CreateTable {
            db_name,
            db_id,
            table_id,
            table_def,
        } => {
            let Ok(id) = table_id.parse::<u64>() else {
                return Ok(());
            };
            let Some(meta) = table_meta(db_name, id, table_def) else {
                return Ok(());
            };
            if catalog.get_table_by_name(&meta.name)?.is_some() {
                return Ok(());
            }
            let db = db_id.parse::<u64>().map_err(|_| {
                common::Error::Invalid(format!("replay: bad database id '{db_id}'"))
            })?;
            catalog::layout::stage_new_table(db, &meta, id + 1)?
        }
        // A dump names the chunk records of one index segment; the reference
        // writes them inside the dump transaction, so the frame's commit ts is
        // the value of every idx_chunk|...|{chunk_id} record it creates.
        WalCmd::DumpIndex {
            db_name,
            db_id,
            table_id,
            index_id,
            segment_id,
            chunk_infos,
            ..
        } => {
            let Some(index_id) = index_id.parse::<u64>().ok() else {
                return Ok(());
            };
            let chunks: Vec<catalog::layout::IndexChunk> = chunk_infos
                .iter()
                .filter(|c| c.deprecate_ts == u64::MAX)
                .map(|c| catalog::layout::IndexChunk {
                    chunk_id: c.chunk_id,
                    base_name: c.base_name.clone(),
                    base_row_id: c.base_row_id.to_u64(),
                    row_count: u64::from(c.row_count),
                    term_count: u64::from(c.term_count),
                    index_size: u64::from(c.index_size),
                    commit_ts,
                })
                .collect();
            if chunks.is_empty() {
                return Ok(());
            }
            let _ = db_name;
            catalog.publish_index_segments(
                &catalog::layout::SegmentSet {
                    db_id: db_id.clone(),
                    table_id: table_id.clone(),
                    segments: Vec::new(),
                },
                &[catalog::layout::IndexSegment {
                    index_id,
                    segment_id: *segment_id,
                    chunks,
                }],
            )?;
            return Ok(());
        }
        // An import fills a segment and seals it: its block files are on disk,
        // the catalog records that name them are not (the store is log-less).
        WalCmd::Import {
            db_id,
            table_id,
            segment_info,
            ..
        } => {
            let set = catalog::layout::SegmentSet {
                db_id: db_id.clone(),
                table_id: table_id.clone(),
                segments: vec![catalog::layout::SegmentRecord {
                    segment_id: segment_info.segment_id,
                    create_ts: commit_ts,
                    first_delete_ts: catalog::layout::UNCOMMIT_TS,
                    blocks: segment_info
                        .block_infos
                        .iter()
                        .map(|b| catalog::layout::BlockRecord {
                            block_id: b.block_id,
                            commit_ts,
                        })
                        .collect(),
                }],
            };
            publish_segments_merged(catalog, &set)?;
            return Ok(());
        }
        // A compaction moves the rows into a new segment and deprecates the
        // sources. Both halves are catalog records: the merged segment's
        // catalog|seg|/catalog|blk| layer (its block files are already on disk)
        // and one drop|seg| marker per source, whose ts is the compaction's.
        WalCmd::Compact {
            db_id,
            table_id,
            index_ids,
            new_segment_infos,
            deprecated_segment_ids,
            ..
        } => {
            let set = catalog::layout::SegmentSet {
                db_id: db_id.clone(),
                table_id: table_id.clone(),
                segments: new_segment_infos
                    .iter()
                    .map(|s| catalog::layout::SegmentRecord {
                        segment_id: s.segment_id,
                        create_ts: commit_ts,
                        first_delete_ts: catalog::layout::UNCOMMIT_TS,
                        blocks: s
                            .block_infos
                            .iter()
                            .map(|b| catalog::layout::BlockRecord {
                                block_id: b.block_id,
                                commit_ts,
                            })
                            .collect(),
                    })
                    .collect(),
            };
            if !set.segments.is_empty() {
                publish_segments_merged(catalog, &set)?;
            }
            if !deprecated_segment_ids.is_empty() {
                let ids: Vec<u64> = index_ids.iter().filter_map(|i| i.parse().ok()).collect();
                catalog.drop_segments(&set, &ids, deprecated_segment_ids, commit_ts)?;
            }
            return Ok(());
        }
        // `ADD COLUMNS`: the statement wrote the appended columns' `tbl|col|`
        // records inside its own transaction, and the schema a reader rebuilds
        // comes from those records (`layout::read_columns`), not from the native
        // table record. A replay that skipped them lost every added column the
        // next time the directory was opened.
        WalCmd::AddColumns {
            db_name,
            db_id,
            table_name,
            column_defs,
            ..
        } => {
            let full = qualified(db_name, table_name);
            let Some(mut meta) = catalog.get_table_by_name(&full)? else {
                return Ok(());
            };
            let mut first: Option<usize> = None;
            for def in column_defs {
                // Already replayed once: the entry sits above the frame's floor
                // but its records are in the SST (see the drop arms below).
                if meta.ids.contains(&(def.id as u64)) {
                    continue;
                }
                let Some(type_text) = data_type_text(&def.column_type) else {
                    continue;
                };
                first.get_or_insert(meta.columns.len());
                meta.ids.push(def.id as u64);
                meta.columns.push(def.name.clone());
                meta.types.push(type_text);
                meta.defaults.push(constant_text(&def.default_expr));
                meta.not_nulls.push(
                    def.constraints
                        .contains(&storage::wal::ddl_def::ConstraintType::NotNull),
                );
                meta.comments.push(if def.comment.is_empty() {
                    None
                } else {
                    Some(def.comment.clone())
                });
                meta.sparse_stores.push(None);
            }
            let db = db_id.parse::<u64>().map_err(|_| {
                common::Error::Invalid(format!("replay: bad database id '{db_id}'"))
            })?;
            let Some(first) = first else {
                return Ok(()); // every column is already there
            };
            // Read before the records land: the bitmap coverage comes from the
            // append journal, derived from the block files under the schema
            // they still have.
            let blocks = engine.add_column_blocks(&full);
            let mut batch = catalog::layout::stage_added_columns(db, &meta, first)?;
            Catalog::restamp(&mut batch, commit_ts);
            catalog.store().atomic_batch(&batch)?;
            // These records bypass the catalog's own writers, so drop the table
            // cache: the next frame - and the checkpoint that publishes the
            // layout - has to see the new columns.
            catalog.invalidate_tables();
            // `PrepareCommitAddColumns` also fills the new column in every
            // existing block (`AddColumnsData`), so a crash between the
            // catalog commit and the file write is repaired by the replay.
            engine.add_column_files(&full, first, &blocks)?;
            // Rows the replay put in the append buffer are not in a block yet;
            // they get the default cell the schema now has (the mirror writes
            // it when the block is).
            let added: Vec<Value> = (first..meta.columns.len())
                .map(|i| match meta.defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                })
                .collect();
            engine.patch_buffered_add_columns(&full, first, &added);
            // Rows the replay has *not* buffered yet (an append earlier in the
            // log) are reshaped when they are parsed.
            // The cell text a default renders to: the catalog's text form is
            // already the stored form (a string is stored bare, not as an SQL
            // literal), and `parse_value` reads it back.
            let texts: Vec<String> = (first..meta.columns.len())
                .map(|i| {
                    meta.defaults
                        .get(i)
                        .cloned()
                        .flatten()
                        .unwrap_or_else(|| "null".to_string())
                })
                .collect();
            shapes.push((
                commit_ts,
                full.clone(),
                RowShapeChange::Added { first, texts },
            ));
            Vec::new()
        }
        _ => {
            let ((), staged) = catalog.capture(|| -> CommonResult<()> {
                match cmd {
                    WalCmd::CreateDatabase { db_name, .. } => {
                        catalog.create_database(db_name, true)?;
                    }
                    WalCmd::DropDatabase { db_name, .. } => {
                        catalog.drop_database(db_name, true)?;
                    }
                    // A DDL that is already in the store must not fail the open.
                    // The frame's floor is the *completed-commit* clock, while the
                    // checkpoint flushes the store at the end, so a commit that
                    // landed while the mirror ran is durable but sits above the
                    // floor and is replayed once more. The reference re-applies
                    // the recorded KV batch and is idempotent for that reason;
                    // these arms recompute the layout, so they have to tolerate
                    // the change they are about to make already being there.
                    WalCmd::DropTable {
                        db_name,
                        table_name,
                        ..
                    } => {
                        let full = qualified(db_name, table_name);
                        if catalog.get_table_by_name(&full)?.is_some() {
                            let _ = catalog.drop_table(&full)?;
                        }
                    }
                    WalCmd::RenameTable {
                        db_name,
                        table_name,
                        new_table_name,
                        ..
                    } => {
                        let (from, to) = (
                            qualified(db_name, table_name),
                            qualified(db_name, new_table_name),
                        );
                        // Renamed already: the source is gone and the target
                        // holds the table.
                        if catalog.get_table_by_name(&from)?.is_some()
                            && catalog.get_table_by_name(&to)?.is_none()
                        {
                            catalog.rename_table(&from, &to)?;
                        }
                    }
                    WalCmd::DropColumns {
                        db_name,
                        table_name,
                        column_names,
                        ..
                    } => {
                        let full = qualified(db_name, table_name);
                        let blocks: Vec<(u32, u16)> = engine
                            .visible_block_ids(&full)
                            .into_iter()
                            .map(|b| (b.segment_id, b.block_id))
                            .collect();
                        for column in column_names {
                            let Some(meta) = catalog.get_table_by_name(&full)? else {
                                return Ok(());
                            };
                            let position = meta.columns.iter().position(|c| c == column);
                            if position.is_none() {
                                continue; // dropped already
                            }
                            catalog.drop_column(&full, column, &blocks)?;
                            // A replayed row has no block file to leave alone:
                            // the buffer loses the cell the schema had, and a row
                            // still in the log is reshaped when it is parsed.
                            engine.patch_buffered_drop_column(&full, position.unwrap());
                            shapes.push((
                                commit_ts,
                                full.clone(),
                                RowShapeChange::Dropped {
                                    position: position.unwrap(),
                                },
                            ));
                        }
                    }
                    WalCmd::DropIndex {
                        db_name,
                        table_name,
                        index_name,
                        ..
                    } => {
                        let _ =
                            catalog.drop_index_on(&qualified(db_name, table_name), index_name)?;
                    }
                    WalCmd::CreateIndex {
                        db_name,
                        table_name,
                        index_base,
                        ..
                    } => {
                        let full = qualified(db_name, table_name);
                        let base = match index_base {
                            storage::wal::index_def::IndexDef::Secondary { base, .. }
                            | storage::wal::index_def::IndexDef::FullText { base, .. }
                            | storage::wal::index_def::IndexDef::Bmp { base, .. }
                            | storage::wal::index_def::IndexDef::Hnsw { base, .. }
                            | storage::wal::index_def::IndexDef::Emvb { base, .. }
                            | storage::wal::index_def::IndexDef::Plaid { base, .. }
                            | storage::wal::index_def::IndexDef::Smve { base, .. }
                            | storage::wal::index_def::IndexDef::Ivf { base, .. } => base,
                        };
                        if !catalog
                            .list_indexes()?
                            .iter()
                            .any(|im| im.name == base.index_name && im.table == full)
                        {
                            let column = base.column_names.first().cloned().unwrap_or_default();
                            catalog.create_index_typed(
                                &base.index_name,
                                &full,
                                &column,
                                index_type_text(base.index_type),
                                &index_options(index_base),
                            )?;
                        }
                    }
                    WalCmd::Append { .. } | WalCmd::Delete { .. } => {}
                    _ => {}
                }
                Ok(())
            })?;
            staged
        }
    };
    if batch.is_empty() {
        return Ok(());
    }
    Catalog::restamp(&mut batch, commit_ts);
    catalog.store().atomic_batch(&batch)?;
    Ok(())
}

/// The catalog's index-kind spelling for an IndexType.
fn index_type_text(t: storage::wal::index_def::IndexType) -> &'static str {
    use storage::wal::index_def::IndexType;
    match t {
        IndexType::Hnsw => "HNSW",
        IndexType::FullText => "FULLTEXT",
        IndexType::Bmp => "BMP",
        IndexType::Ivf => "IVF",
        IndexType::Emvb => "EMVB",
        IndexType::Plaid => "PLAID",
        IndexType::Smve => "SMVE",
        IndexType::DiskAnn => "DISKANN",
        IndexType::Secondary | IndexType::SecondaryFunctional | IndexType::Invalid => "SECONDARY",
    }
}

/// The WITH options the create path passed, rebuilt from the typed definition.
///
/// The catalog record is derived from these (`layout::index_def_for`), so they
/// only have to name the same values: the option lookup is case-insensitive and
/// trims quotes.
fn index_options(def: &storage::wal::index_def::IndexDef) -> Vec<(String, String)> {
    use storage::wal::index_def::{IndexDef, MetricType};
    let pair = |k: &str, v: String| (k.to_string(), v);
    match def {
        IndexDef::Hnsw {
            m,
            ef_construction,
            block_size,
            metric_type,
            ..
        } => vec![
            pair("M", m.to_string()),
            pair("EF_CONSTRUCTION", ef_construction.to_string()),
            pair("BLOCK_SIZE", block_size.to_string()),
            pair(
                "METRIC",
                match metric_type {
                    MetricType::Cosine => "cosine",
                    MetricType::InnerProduct => "ip",
                    _ => "l2",
                }
                .to_string(),
            ),
        ],
        IndexDef::FullText { analyzer, .. } => vec![pair("ANALYZER", analyzer.clone())],
        IndexDef::Bmp { block_size, .. } => vec![pair("BLOCK_SIZE", block_size.to_string())],
        IndexDef::Emvb {
            residual_pq_subspace_num,
            residual_pq_subspace_bits,
            ..
        } => vec![
            pair("PQ_SUBSPACE_NUM", residual_pq_subspace_num.to_string()),
            pair("PQ_SUBSPACE_BITS", residual_pq_subspace_bits.to_string()),
        ],
        _ => Vec::new(),
    }
}

/// Publishes `incoming` on top of the table's current segment layer.
///
/// `layout::write_segments` rebuilds the layer from an empty state (that is what
/// a checkpoint wants), so replaying one command's segment on its own would drop
/// every other segment of the table. Merge first: a segment the command names
/// replaces its record, the rest stay as they are.
fn publish_segments_merged(
    catalog: &Catalog,
    incoming: &catalog::layout::SegmentSet,
) -> CommonResult<()> {
    let mut layer = catalog::layout::read_segments(catalog.store().as_ref())?;
    let set = match layer
        .iter_mut()
        .find(|s| s.db_id == incoming.db_id && s.table_id == incoming.table_id)
    {
        Some(set) => set,
        None => {
            layer.push(incoming.clone());
            layer.last_mut().unwrap()
        }
    };
    for seg in &incoming.segments {
        match set
            .segments
            .iter_mut()
            .find(|s| s.segment_id == seg.segment_id)
        {
            // An import appends to the segment it targets, and its command
            // names only the blocks **it** wrote. Replacing the record
            // dropped the earlier batches: the 100k-row import suite imports
            // in slices into one segment, and after a replay only the last
            // slice's rows were visible (measured: half the rows gone).
            Some(slot) => {
                for block in &seg.blocks {
                    if !slot.blocks.iter().any(|b| b.block_id == block.block_id) {
                        slot.blocks.push(block.clone());
                    }
                }
                slot.blocks.sort_by_key(|b| b.block_id);
            }
            None => set.segments.push(seg.clone()),
        }
    }
    catalog.publish_segments(set).map(|_| ())
}
