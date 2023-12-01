namespace py infinity_thrift_rpc
namespace cpp infinity_thrift_rpc

// https://github.com/apache/thrift/blob/master/test/Recursive.thrift

enum LogicType{
Boolean,
TinyInt,
SmallInt,
Integer,
BigInt,
HugeInt,
Decimal,
Float,
Double,
Varchar,
Embedding,
Invalid
}

struct Option {}

struct NumberType {}

struct VarcharType {}

enum ElementType {
ElementBit,
ElementInt8,
ElementInt16,
ElementInt32,
ElementInt64,
ElementFloat32,
ElementFloat64,
}

struct EmbeddingType {
1:  i32 dimension,
2:  ElementType element_type,
}

union PhysicalType {
1:  NumberType number_type,
2:  VarcharType varchar_type,
3:  EmbeddingType embedding_type,
}

struct DataType {
1:  LogicType logic_type,
2:  PhysicalType physical_type,
}

enum Constraint {
PrimaryKey,
NotNull,
Null,
Unique,
}

struct ColumnDef {
1:  i32 id,
2:  string name,
3:  DataType data_type,
4:  list<Constraint> constraints = [],
}

enum LiteralType {
Boolean,
Double,
String,
Int64,
Null,
IntegerArray,
DoubleArray,
}

union ParsedExprType {
1: ConstantExpr & constant_expr,
2: ColumnExpr & column_expr,
3: FunctionExpr & function_expr,
4: KnnExpr  & knn_expr,
5: BetweenExpr & between_expr,
}

struct ParsedExpr {
1: ParsedExprType type,
2: string alias_name,
}

struct ColumnExpr {
1: list<string> column_name = [],
2: bool star,
}

enum KnnDistanceType{
L2,
Cosine,
InnerProduct,
Hamming,
}

struct KnnExpr {
1: ParsedExpr  column_expr,
2: list<string> embedding_data,
3: i64 dimension,
4: KnnDistanceType distance_type,
5: EmbeddingType embedding_type,
}

struct ConstantExpr {
1: LiteralType literal_type,
2: bool bool_value,
3: i64 i64_value,
4: double f64_value,
5: string str_value,
6: list<i64> i64_array_value,
7: list<double> f64_array_value,
}

struct FunctionExpr {
1: string function_name,
2: list<ParsedExpr> arguments,
}


struct BetweenExpr {
1: ParsedExpr value,
2: ParsedExpr upper_bound,
3: ParsedExpr lower_bound,
}

struct UpdateExpr {
1: string column_name,
2: ParsedExpr value,
}


struct Field {
1: list<ParsedExpr> parse_exprs = [],
}

enum CopyFileType {
CSV,
JSON,
FVECS,
}

enum ColumnType {
ColumnBool,
ColumnInt8,
ColumnInt16,
ColumnInt32,
ColumnInt64,
ColumnFloat32,
ColumnFloat64,
ColumnVarchar,
ColumnEmbedding,
ColumnInvalid,
}

struct ColumnField {
1: ColumnType column_type,
2: binary column_vector,
}

struct ImportOption {
1:  string delimiter,
2:  string file_type,
3:  string table_name,
4:  string schema_name,
5:  bool copy_from,
6:  bool has_header,
7:  CopyFileType copy_file_type,
}

struct CommonResponse {
1:  bool success,
2:  string error_msg,
3:  i64 session_id,
}

struct CommonRequest {
1:  i64 session_id,
}

struct ListDatabaseRequest {
1: i64 session_id,
}

struct ListDatabaseResponse {
1: bool success,
2: string error_msg,
3: list<string> db_names = [],
}

struct ListTableRequest {
1: string db_name,
2: i64 session_id,
}

struct ListTableResponse {
1: bool success,
2: string error_msg,
3: list<string> table_names = [],
}

struct DescribeDatabaseRequest {
1: string db_name,
2: i64 session_id,
}

struct DescribeDatabaseResponse {
1: bool success,
2: string error_msg,
3: i64 num_segments,
4: i64 num_rows,
5: i64 num_blocks,
}

struct DescribeTableRequest {
1: string db_name,
2: string table_name,
3: i64 session_id,
}

struct DescribeTableResponse {
1: bool success,
2: string error_msg,
3: i64 num_segments,
4: i64 num_rows,
5: i64 num_blocks,
}

struct GetTableRequest {
1: string db_name,
2: string table_name,
3: i64 session_id,
}

struct InitParameter {
1: string param_name,
2: string param_value,
}

enum IndexType {
IVFFlat,
HnswLVQ,
Hnsw,
IRSFullText,
}

struct IndexInfo {
1: string column_name,
2: IndexType index_type,
3: list<InitParameter> index_param_list = [],
}

struct CreateIndexRequest {
1: string db_name,
2: string table_name,
3: string index_name,
5: list<IndexInfo> index_info_list = [],
6: i64 session_id,
7: optional Option option,
}

struct DropIndexRequest {
1: string db_name,
2: string table_name,
3: string index_name,
4: i64 session_id,
}

struct GetDatabaseRequest {
1: string db_name,
2: i64 session_id,
}

struct CreateDatabaseRequest {
1:  string db_name,
2:  i64 session_id,
3:  Option option,
}

struct DropDatabaseRequest {
1:  string db_name,
2:  i64 session_id,
3:  Option option,
}

struct CreateTableRequest {
1:  string db_name,
2:  string table_name,
3:  list<ColumnDef> column_defs = [],
6:  i64 session_id,
7:  Option option,
}

struct DropTableRequest {
1:  string db_name,
2:  string table_name,
3:  i64 session_id,
4:  Option option,
}

struct InsertRequest {
1:  string db_name,
2:  string table_name,
3:  list<string> column_names = [],
4:  list<Field> fields = [],
5:  i64 session_id,
}

struct ImportRequest{
1:  string db_name,
2:  string table_name,
3:  string file_name,
4:  binary file_content,
5:  ImportOption import_option,
6:  i64 session_id,
}

struct SelectRequest {
1:  string db_name,
2:  string table_name,
3:  list<ParsedExpr> select_list = [],
4:  ParsedExpr where_expr,
5:  list<ParsedExpr> group_by_list = [],
6:  ParsedExpr having_expr,
7:  ParsedExpr limit_expr,
8:  ParsedExpr offset_expr,
9:  i64 session_id,
}

struct SelectResponse {
1: bool success,
2: string error_msg,
3: list<ColumnDef> column_defs = [],
4: list<ColumnField> column_fields = [];
}

struct DeleteRequest {
1:  string db_name,
2:  string table_name,
3:  ParsedExpr where_expr,
4:  i64 session_id,
}

struct UpdateRequest {
1:  string db_name,
2:  string table_name,
3:  ParsedExpr where_expr,
4:  list<UpdateExpr> update_expr_array = [],
5:  i64 session_id,
}

// Service
service InfinityService {
CommonResponse Connect(),
CommonResponse Disconnect(1:CommonRequest request),

CommonResponse CreateDatabase(1:CreateDatabaseRequest request),
CommonResponse DropDatabase(1:DropDatabaseRequest request),
CommonResponse CreateTable(1:CreateTableRequest request),
CommonResponse DropTable(1:DropTableRequest request),
CommonResponse Insert(1:InsertRequest request),
CommonResponse Import(1:ImportRequest request),
SelectResponse Select(1:SelectRequest request),
CommonResponse Delete(1:DeleteRequest request),
CommonResponse Update(1:UpdateRequest request),

ListDatabaseResponse ListDatabase(1:ListDatabaseRequest request),
ListTableResponse ListTable(1:ListTableRequest request),

DescribeDatabaseResponse DescribeDatabase(1:DescribeDatabaseRequest request),
DescribeTableResponse DescribeTable(1:DescribeTableRequest request),

CommonResponse GetDatabase(1:GetDatabaseRequest request),
CommonResponse GetTable(1:GetTableRequest request),

CommonResponse CreateIndex(1:CreateIndexRequest request),
CommonResponse DropIndex(1:DropIndexRequest request),
}
