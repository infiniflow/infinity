//! Canned responses for the wire-parity harness.
//!
//! Each case reproduces the Rust server's Select reply for the schema in
//! tools/volo_wire/test_wire_parity.py; the test sets VOLO_WIRE_CASE and
//! compares volo's re-encoding with the captured reply byte for byte.

use crate::volo_gen::infinity::*;
use volo_thrift::ServerError;

pub struct Svc;

fn common_ok() -> CommonResponse {
    CommonResponse {
        error_code: Some(0),
        error_msg: Some("OK".into()),
        session_id: Some(1),
    }
}

fn col(id: i32, name: &str, logic: i32, phys: Option<Box<PhysicalType>>) -> ColumnDef {
    ColumnDef {
        id: Some(id),
        name: Some(name.to_string().into()),
        data_type: Some(DataType {
            logic_type: Some(LogicType::from(logic)),
            physical_type: phys,
        }),
        constraints: None,
        constant_expr: None,
        comment: None,
    }
}

/// Column vectors are raw bytes on the wire, so the canned data is written the
/// way the server writes it: hex, one row after another.
fn hex(s: &str) -> Vec<u8> {
    let digits: Vec<u8> = s.bytes().filter(|b| !b.is_ascii_whitespace()).collect();
    digits
        .chunks(2)
        .map(|p| u8::from_str_radix(std::str::from_utf8(p).unwrap(), 16).unwrap())
        .collect()
}

fn fld(column_type: i32, name: &str, bytes: &str, masks: Vec<bool>) -> ColumnField {
    ColumnField {
        column_type: Some(ColumnType::from(column_type)),
        column_vectors: Some(vec![::pilota::Bytes::from(hex(bytes))]),
        column_name: Some(name.to_string().into()),
        bitmasks: Some(masks),
    }
}

fn emb(dim: i32) -> Option<Box<PhysicalType>> {
    Some(Box::new(PhysicalType::EmbeddingType(EmbeddingType {
        dimension: Some(dim),
        element_type: Some(ElementType::ELEMENT_FLOAT32),
    })))
}

fn sparse(dim: i64) -> Option<Box<PhysicalType>> {
    Some(Box::new(PhysicalType::SparseType(SparseType {
        dimension: Some(dim),
        element_type: Some(ElementType::ELEMENT_FLOAT32),
        index_type: Some(ElementType::ELEMENT_INT32),
    })))
}

/// ARRAY element types carry a DataType whose own physical type is the empty
/// NumberType struct; the server sends it and the SDK compares it.
fn array_of(logic: i32) -> Option<Box<PhysicalType>> {
    Some(Box::new(PhysicalType::ArrayType(ArrayType {
        element_data_type: Some(Box::new(DataType {
            logic_type: Some(LogicType::from(logic)),
            physical_type: Some(Box::new(PhysicalType::NumberType(NumberType {}))),
        })),
    })))
}

fn select_response() -> SelectResponse {
    let case = std::env::var("VOLO_WIRE_CASE").unwrap_or_default();
    let (defs, fields): (Vec<ColumnDef>, Vec<ColumnField>) = match case.as_str() {
        // t_sel(id INT, name VARCHAR), 3 rows, last NULL.
        "int_varchar" => (
            vec![col(0, "id", 3, None), col(1, "name", 11, None)],
            vec![
                fld(3, "id", "01000000 02000000 03000000", vec![]),
                fld(
                    9,
                    "name",
                    "03000000 616263  02000000 6465  04000000 6e756c6c",
                    vec![true, true, false],
                ),
            ],
        ),
        // The same shape with no NULL: no bitmask list at all.
        "int_varchar_no_null" => (
            vec![col(0, "id", 3, None), col(1, "name", 11, None)],
            vec![
                fld(3, "id", "01000000 02000000 03000000", vec![]),
                fld(
                    9,
                    "name",
                    "03000000 616263  02000000 6465  02000000 6667",
                    vec![],
                ),
            ],
        ),
        // t_num(id INT, f FLOAT, v EMBEDDING(FLOAT,4)), 3 rows, last NULL.
        "float_embedding" => (
            vec![
                col(0, "id", 3, None),
                col(1, "f", 7, None),
                col(2, "v", 12, emb(4)),
            ],
            vec![
                fld(3, "id", "01000000 02000000 03000000", vec![]),
                fld(
                    5,
                    "f",
                    "0000c03f 000000be 00000000",
                    vec![true, true, false],
                ),
                fld(
                    10,
                    "v",
                    "0000803e 0000003f 000040bf 0000803f
                     00000000 cdcccc3d cdcc4c3e 9a99993e
                     00000000 00000000 00000000 00000000",
                    vec![true, true, false],
                ),
            ],
        ),
        // t_dt(id INT, d DOUBLE, ts TIMESTAMP), 3 rows, last NULL.
        "double_timestamp" => (
            vec![
                col(0, "id", 3, None),
                col(1, "d", 8, None),
                col(2, "ts", 19, None),
            ],
            vec![
                fld(3, "id", "01000000 02000000 03000000", vec![]),
                fld(
                    6,
                    "d",
                    "00000000 0000f83f
                     9a999999 9999b9bf
                     00000000 00000000",
                    vec![true, true, false],
                ),
                fld(
                    18,
                    "ts",
                    "0c4d0000 a8930000
                     0b4d0000 01000000
                     00000000 00000000",
                    vec![],
                ),
            ],
        ),
        // t_temporal(id INT, d DATE, tm TIME, dtm DATETIME, js JSON).
        "date_time_json" => (
            vec![
                col(0, "id", 3, None),
                col(1, "d", 17, None),
                col(2, "tm", 18, None),
                col(3, "dtm", 19, None),
                col(4, "js", 23, None),
            ],
            vec![
                fld(3, "id", "01000000 02000000 03000000", vec![]),
                fld(
                    16,
                    "d",
                    "0c4d0000 00000000 00000000",
                    vec![true, true, false],
                ),
                fld(17, "tm", "a8930000 01000000 00000000", vec![]),
                fld(
                    18,
                    "dtm",
                    "0c4d0000 a8930000
                     0b4d0000 01000000
                     00000000 00000000",
                    vec![],
                ),
                fld(
                    22,
                    "js",
                    "07000000 7b2261223a317d
                     05000000 5b312c325d
                     04000000 6e756c6c",
                    vec![true, true, false],
                ),
            ],
        ),
        // t_arr(id INT, a ARRAY(INT)): u32 count then i32 elements.
        "array_int" => (
            vec![col(0, "id", 3, None), col(1, "a", 22, array_of(3))],
            vec![
                fld(3, "id", "01000000 02000000 03000000", vec![]),
                fld(
                    21,
                    "a",
                    "03000000 01000000 02000000 03000000
                     01000000 04000000
                     00000000",
                    vec![true, true, false],
                ),
            ],
        ),
        // t_sparse(id INT, s SPARSE(1000, FLOAT, INT32)): u32 count then
        // (i32 index, f32 value) pairs.
        "sparse_float" => (
            vec![col(0, "id", 3, None), col(1, "s", 15, sparse(1000))],
            vec![
                fld(3, "id", "01000000 02000000 03000000", vec![]),
                fld(
                    13,
                    "s",
                    "02000000 00000000 03000000 00002040 0000c03f
                     01000000 07000000 000080bf
                     00000000",
                    vec![true, true, false],
                ),
            ],
        ),
        // t_tensor(id INT, tv TENSOR(FLOAT,4)): u32 byte length then floats.
        "tensor_float" => (
            vec![col(0, "id", 3, None), col(1, "tv", 13, emb(4))],
            vec![
                fld(3, "id", "01000000 02000000 03000000", vec![]),
                fld(
                    11,
                    "tv",
                    "10000000 0000803f 00000040 00004040 00008040
                     10000000 0000003f 0000803e 000000bf 000080bf
                     00000000",
                    vec![true, true, false],
                ),
            ],
        ),
        // t_mv(id INT, mv MULTIVECTOR(FLOAT,4)): u32 byte length then the
        // flattened vectors of that row.
        "multivector_float" => (
            vec![col(0, "id", 3, None), col(1, "mv", 16, emb(4))],
            vec![
                fld(3, "id", "01000000 02000000 03000000", vec![]),
                fld(
                    14,
                    "mv",
                    "20000000 0000803f 00000040 00004040 00008040
                     0000003f 0000003f 0000003f 0000003f
                     10000000 0000803f 00000000 00000000 00000000
                     00000000",
                    vec![true, true, false],
                ),
            ],
        ),
        _ => (vec![], vec![]),
    };
    SelectResponse {
        error_code: Some(0),
        error_msg: Some("OK".into()),
        column_defs: Some(defs),
        column_fields: Some(fields),
        extra_result: Some("".into()),
    }
}

impl InfinityService for Svc {
    async fn connect(&self, _r: ConnectRequest) -> Result<CommonResponse, ServerError> {
        Ok(common_ok())
    }
    async fn disconnect(&self, _r: CommonRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn create_database(
        &self,
        _r: CreateDatabaseRequest,
    ) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn drop_database(&self, _r: DropDatabaseRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn create_table(&self, _r: CreateTableRequest) -> Result<CommonResponse, ServerError> {
        Ok(common_ok())
    }
    async fn drop_table(&self, _r: DropTableRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn rename_table(&self, _r: RenameTableRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn insert(&self, _r: InsertRequest) -> Result<CommonResponse, ServerError> {
        Ok(common_ok())
    }
    async fn import(&self, _r: ImportRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn export(&self, _r: ExportRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn select(&self, _r: SelectRequest) -> Result<SelectResponse, ServerError> {
        Ok(select_response())
    }
    async fn explain(&self, _r: ExplainRequest) -> Result<SelectResponse, ServerError> {
        unimplemented!()
    }
    async fn delete(&self, _r: DeleteRequest) -> Result<DeleteResponse, ServerError> {
        unimplemented!()
    }
    async fn update(&self, _r: UpdateRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn list_database(
        &self,
        _r: ListDatabaseRequest,
    ) -> Result<ListDatabaseResponse, ServerError> {
        unimplemented!()
    }
    async fn list_table(&self, _r: ListTableRequest) -> Result<ListTableResponse, ServerError> {
        unimplemented!()
    }
    async fn list_index(&self, _r: ListIndexRequest) -> Result<ListIndexResponse, ServerError> {
        unimplemented!()
    }
    async fn show_table(&self, _r: ShowTableRequest) -> Result<ShowTableResponse, ServerError> {
        unimplemented!()
    }
    async fn show_columns(&self, _r: ShowColumnsRequest) -> Result<SelectResponse, ServerError> {
        unimplemented!()
    }
    async fn show_database(
        &self,
        _r: ShowDatabaseRequest,
    ) -> Result<ShowDatabaseResponse, ServerError> {
        unimplemented!()
    }
    async fn show_segments(&self, _r: ShowSegmentsRequest) -> Result<SelectResponse, ServerError> {
        unimplemented!()
    }
    async fn show_segment(
        &self,
        _r: ShowSegmentRequest,
    ) -> Result<ShowSegmentResponse, ServerError> {
        unimplemented!()
    }
    async fn show_blocks(&self, _r: ShowBlocksRequest) -> Result<SelectResponse, ServerError> {
        unimplemented!()
    }
    async fn show_block(&self, _r: ShowBlockRequest) -> Result<ShowBlockResponse, ServerError> {
        unimplemented!()
    }
    async fn get_database(&self, _r: GetDatabaseRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn get_table(&self, _r: GetTableRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn create_index(&self, _r: CreateIndexRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn drop_index(&self, _r: DropIndexRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn show_index(&self, _r: ShowIndexRequest) -> Result<ShowIndexResponse, ServerError> {
        unimplemented!()
    }
    async fn optimize(&self, _r: OptimizeRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn alter_index(&self, _r: AlterIndexRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn add_columns(&self, _r: AddColumnsRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn drop_columns(&self, _r: DropColumnsRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn cleanup(&self, _r: CommonRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn dump_index(&self, _r: DumpIndexRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn command(&self, _r: CommandRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn flush(&self, _r: FlushRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn compact(&self, _r: CompactRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn create_table_snapshot(
        &self,
        _r: CreateTableSnapshotRequest,
    ) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn create_database_snapshot(
        &self,
        _r: CreateDatabaseSnapshotRequest,
    ) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn create_system_snapshot(
        &self,
        _r: CreateSystemSnapshotRequest,
    ) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn restore_snapshot(
        &self,
        _r: RestoreSnapshotRequest,
    ) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn show_snapshot(
        &self,
        _r: ShowSnapshotRequest,
    ) -> Result<ShowSnapshotResponse, ServerError> {
        unimplemented!()
    }
    async fn list_snapshots(
        &self,
        _r: ListSnapshotsRequest,
    ) -> Result<ListSnapshotsResponse, ServerError> {
        unimplemented!()
    }
    async fn drop_snapshot(&self, _r: DropSnapshotRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn set_config(&self, _r: SetConfigRequest) -> Result<CommonResponse, ServerError> {
        unimplemented!()
    }
    async fn show_config(&self, _r: ShowConfigRequest) -> Result<ShowConfigResponse, ServerError> {
        unimplemented!()
    }
    async fn show_block_column(
        &self,
        _r: ShowBlockColumnRequest,
    ) -> Result<ShowBlockColumnResponse, ServerError> {
        unimplemented!()
    }
    async fn show_current_node(
        &self,
        _r: ShowCurrentNodeRequest,
    ) -> Result<ShowCurrentNodeResponse, ServerError> {
        unimplemented!()
    }
}
