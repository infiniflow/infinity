// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module infinity_thrift_service;

import infinity_thrift_types;
import infinity;
import stl;
import query_options;
import column_def;
import statement_common;
import data_type;
import status;
import embedding_info;
import constant_expr;
import column_expr;
import function_expr;
import knn_expr;
import match_sparse_expr;
import match_tensor_expr;
import match_expr;
import fusion_expr;
import parsed_expr;
import update_statement;
import search_expr;
import explain_statement;
import create_index_info;
import data_block;
import table_def;
import internal_types;
import column_vector;
import query_result;

namespace infinity {

struct ClientVersions {
    ClientVersions();

    HashMap<i64, String> client_version_map_;

    Pair<const char*, Status> GetVersionByIndex(i64);
};

export class InfinityThriftService final : public infinity_thrift_rpc::InfinityServiceIf {
private:
    static constexpr std::string_view ErrorMsgHeader = "[THRIFT ERROR]";
    static constexpr i64 current_version_index_{9}; // 0.2.1.dev3

    static std::mutex infinity_session_map_mutex_;
    static HashMap<u64, SharedPtr<Infinity>> infinity_session_map_;

    static ClientVersions client_version_;

public:
    InfinityThriftService() = default;

    void Connect(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::ConnectRequest& request) final;

    void Disconnect(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::CommonRequest &request) final;

    void CreateDatabase(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::CreateDatabaseRequest &request) final;

    void DropDatabase(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::DropDatabaseRequest &request) final;

    void CreateTable(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::CreateTableRequest &request) final;

    void DropTable(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::DropTableRequest &request) final;

    void Insert(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::InsertRequest &request) final;

    Tuple<CopyFileType, Status> GetCopyFileType(infinity_thrift_rpc::CopyFileType::type copy_file_type);

    void Import(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::ImportRequest &request) final;

    void Export(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::ExportRequest &request) final;

    void Select(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::SelectRequest &request) final;

    void Explain(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::ExplainRequest &request) final;

    void Delete(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::DeleteRequest &request) final;

    void Update(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::UpdateRequest &request) final;

    void Optimize(infinity_thrift_rpc::CommonResponse& response, const infinity_thrift_rpc::OptimizeRequest& request) final;

    void ListDatabase(infinity_thrift_rpc::ListDatabaseResponse &response, const infinity_thrift_rpc::ListDatabaseRequest &request) final;

    void ListTable(infinity_thrift_rpc::ListTableResponse &response, const infinity_thrift_rpc::ListTableRequest &request) final;

    void ShowDatabase(infinity_thrift_rpc::ShowDatabaseResponse &response, const infinity_thrift_rpc::ShowDatabaseRequest &request) final;

    void ShowTable(infinity_thrift_rpc::ShowTableResponse &response, const infinity_thrift_rpc::ShowTableRequest &request) final;

    void ShowColumns(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::ShowColumnsRequest &request) final;

    void ShowTables(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::ShowTablesRequest &request) final;

    void ShowSegments(infinity_thrift_rpc::SelectResponse &_return, const infinity_thrift_rpc::ShowSegmentsRequest &request) final;

    void ShowSegment(infinity_thrift_rpc::ShowSegmentResponse &_return, const infinity_thrift_rpc::ShowSegmentRequest &request) final;

    void ShowBlocks(infinity_thrift_rpc::SelectResponse &_return, const infinity_thrift_rpc::ShowBlocksRequest &request) final;

    void ShowBlock(infinity_thrift_rpc::ShowBlockResponse &_return, const infinity_thrift_rpc::ShowBlockRequest &request) final;

    void ShowBlockColumn(infinity_thrift_rpc::ShowBlockColumnResponse &_return, const infinity_thrift_rpc::ShowBlockColumnRequest &request) final;

    void GetDatabase(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::GetDatabaseRequest &request) final;

    void GetTable(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::GetTableRequest &request) final;

    void CreateIndex(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::CreateIndexRequest &request) final;

    void DropIndex(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::DropIndexRequest &request) final;

    void ListIndex(infinity_thrift_rpc::ListIndexResponse &response, const infinity_thrift_rpc::ListIndexRequest &request) final;

    void ShowIndex(infinity_thrift_rpc::ShowIndexResponse &response, const infinity_thrift_rpc::ShowIndexRequest &request) final;

private:
    Tuple<Infinity *, Status> GetInfinityBySessionID(i64 session_id);

    Status GetAndRemoveSessionID(i64 session_id);

    static Tuple<ColumnDef *, Status> GetColumnDefFromProto(const infinity_thrift_rpc::ColumnDef &column_def);

    static SharedPtr<DataType> GetColumnTypeFromProto(const infinity_thrift_rpc::DataType &type);

    static ConstraintType GetConstraintTypeFromProto(infinity_thrift_rpc::Constraint::type constraint);

    static EmbeddingDataType GetEmbeddingDataTypeFromProto(const infinity_thrift_rpc::ElementType::type &type);

    static IndexType GetIndexTypeFromProto(const infinity_thrift_rpc::IndexType::type &type);

    static ConstantExpr *GetConstantFromProto(Status &status, const infinity_thrift_rpc::ConstantExpr &expr);

    static ColumnExpr *GetColumnExprFromProto(const infinity_thrift_rpc::ColumnExpr &column_expr);

    static FunctionExpr *GetFunctionExprFromProto(Status &status, const infinity_thrift_rpc::FunctionExpr &function_expr);

    static Tuple<KnnExpr *, Status> GetKnnExprFromProto(const infinity_thrift_rpc::KnnExpr &expr);

    static Tuple<MatchSparseExpr *, Status> GetMatchSparseExprFromProto(const infinity_thrift_rpc::MatchSparseExpr &expr);

    static Pair<MatchTensorExpr *, Status> GetMatchTensorExprFromProto(const infinity_thrift_rpc::MatchTensorExpr &expr);

    static MatchExpr *GetMatchExprFromProto(const infinity_thrift_rpc::MatchExpr &expr);

    static FusionExpr *GetFusionExprFromProto(const infinity_thrift_rpc::FusionExpr &expr);

    static ParsedExpr *GetParsedExprFromProto(Status &status, const infinity_thrift_rpc::ParsedExpr &expr);

    static KnnDistanceType GetDistanceTypeFormProto(const infinity_thrift_rpc::KnnDistanceType::type &type);

    static ExplainType GetExplainTypeFromProto(const infinity_thrift_rpc::ExplainType::type &type);

    static Tuple<void *, i64, Status> GetEmbeddingDataTypeDataPtrFromProto(const infinity_thrift_rpc::EmbeddingData &embedding_data);

    static Tuple<UpdateExpr *, Status> GetUpdateExprFromProto(const infinity_thrift_rpc::UpdateExpr &update_expr);

    static OptimizeOptions GetParsedOptimizeOptionFromProto(const infinity_thrift_rpc::OptimizeOptions &options);

    static infinity_thrift_rpc::ColumnType::type DataTypeToProtoColumnType(const SharedPtr<DataType> &data_type);

    UniquePtr<infinity_thrift_rpc::DataType> DataTypeToProtoDataType(const SharedPtr<DataType> &data_type);

    infinity_thrift_rpc::ElementType::type EmbeddingDataTypeToProtoElementType(const EmbeddingDataType &embedding_data_type);

    void
    ProcessDataBlocks(const QueryResult &result, infinity_thrift_rpc::SelectResponse &response, Vector<infinity_thrift_rpc::ColumnField> &columns);

    Status ProcessColumns(const SharedPtr<DataBlock> &data_block, SizeT column_count, Vector<infinity_thrift_rpc::ColumnField> &columns);

    void HandleColumnDef(infinity_thrift_rpc::SelectResponse &response,
                         SizeT column_count,
                         SharedPtr<TableDef> table_def,
                         Vector<infinity_thrift_rpc::ColumnField> &all_column_vectors);

    Status
    ProcessColumnFieldType(infinity_thrift_rpc::ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector);

    static void HandleBoolType(infinity_thrift_rpc::ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector);

    static void HandlePodType(infinity_thrift_rpc::ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector);

    static void
    HandleVarcharType(infinity_thrift_rpc::ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector);

    static void
    HandleEmbeddingType(infinity_thrift_rpc::ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector);

    static void
    HandleTensorType(infinity_thrift_rpc::ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector);

    static void
    HandleTensorArrayType(infinity_thrift_rpc::ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector);

    static void
    HandleSparseType(infinity_thrift_rpc::ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector);

    static void HandleRowIDType(infinity_thrift_rpc::ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector);

    static void ProcessStatus(infinity_thrift_rpc::CommonResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::ShowDatabaseResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::ShowTableResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::ShowIndexResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::SelectResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::ListDatabaseResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::ListTableResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::ListIndexResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::ShowSegmentResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::ShowBlockResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessStatus(infinity_thrift_rpc::ShowBlockColumnResponse &response, const Status &status, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::CommonResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::SelectResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::ListDatabaseResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::ListTableResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::ListIndexResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::ShowDatabaseResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::ShowTableResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::ShowIndexResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::ShowSegmentResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void
    ProcessQueryResult(infinity_thrift_rpc::ShowBlockResponse &response, const QueryResult &result, const std::string_view error_header = ErrorMsgHeader);

    static void ProcessQueryResult(infinity_thrift_rpc::ShowBlockColumnResponse &response,
                                   const QueryResult &result,
                                   const std::string_view error_header = ErrorMsgHeader);
};

} // namespace infinity
