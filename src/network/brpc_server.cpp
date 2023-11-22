// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

#include "brpc_server.h"
#include "network/infinity_brpc/infinity_brpc.pb.h"

import query_options;
import infinity_exception;
import data_block;
import value;
import column_vector;
import logger;

namespace infinity {

void infinity::BrpcServiceImpl::Connect(google::protobuf::RpcController *cntl_base,
                                        const infinity_brpc_proto::Empty *request,
                                        infinity_brpc_proto::CommonResponse *response,
                                        google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = Infinity::RemoteConnect();
    if (infinity == nullptr) {
        response->set_success(false);
        LOG(ERROR) << "Connect failed";
        response->set_error_msg("Connect failed");
    } else {
        infinity_session_map_mutex_.lock();
        infinity_session_map_.emplace(infinity->GetSessionId(), infinity);
        infinity_session_map_mutex_.unlock();
        response->set_session_id(infinity->GetSessionId());
        response->set_success(true);
    }
}

void BrpcServiceImpl::DisConnect(google::protobuf::RpcController *cntl_base,
                                 const infinity_brpc_proto::DisConnectRequest *request,
                                 infinity_brpc_proto::CommonResponse *response,
                                 google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    if (infinity == nullptr) {
        response->set_success(false);
        LOG(ERROR) << "Disconnect failed";
        response->set_error_msg("Disconnect failed");
    } else {
        auto session_id = infinity->GetSessionId();
        infinity->RemoteDisconnect();
        infinity_session_map_mutex_.lock();
        infinity_session_map_.erase(session_id);
        infinity_session_map_mutex_.unlock();
        response->set_success(true);
    }
}

void BrpcServiceImpl::CreateDatabase(google::protobuf::RpcController *cntl_base,
                                     const infinity_brpc_proto::CreateDatabaseRequest *request,
                                     infinity_brpc_proto::CommonResponse *response,
                                     google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto result = infinity->CreateDatabase(request->db_name(), (const CreateDatabaseOptions &)request->options());
    ProcessResult(result, response);
}

void BrpcServiceImpl::DropDatabase(google::protobuf::RpcController *cntl_base,
                                   const infinity_brpc_proto::DropDatabaseRequest *request,
                                   infinity_brpc_proto::CommonResponse *response,
                                   google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto result = infinity->DropDatabase(request->db_name(), (const DropDatabaseOptions &)request->options());
    ProcessResult(result, response);
}

void BrpcServiceImpl::ListDatabase(google::protobuf::RpcController *cntl_base,
                                   const infinity_brpc_proto::ListDatabaseRequest *request,
                                   infinity_brpc_proto::ListDatabaseResponse *response,
                                   google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto result = infinity->ListDatabases();

    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();

        for (int i = 0; i < row_count; ++i) {
            Value value = data_block->GetValue(0, i);
            if (value.value_.varchar.IsInlined()) {
                String prefix = String(value.value_.varchar.prefix, value.value_.varchar.length);
                response->add_db_name(prefix);
            } else {
                String whole_str = String(value.value_.varchar.ptr, value.value_.varchar.length);
                response->add_db_name(whole_str);
            }
        }

        response->set_success(true);
    } else {
        LOG(ERROR) << result.ErrorMsg();
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
    }
}

void BrpcServiceImpl::DescribeDatabase(google::protobuf::RpcController *cntl_base,
                                       const infinity_brpc_proto::DescribeDatabaseRequest *request,
                                       infinity_brpc_proto::DescribeDatabaseResponse *response,
                                       google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    Error<NotImplementException>("Not implement DescribeDatabase", __FILE_NAME__, __LINE__);
}

void BrpcServiceImpl::GetDatabase(google::protobuf::RpcController *cntl_base,
                                  const infinity_brpc_proto::GetDatabaseRequest *request,
                                  infinity_brpc_proto::CommonResponse *response,
                                  google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    if (database != nullptr) {
        response->set_success(true);
    } else {
        LOG(ERROR) << "Database not found";
        response->set_success(false);
        response->set_error_msg("Database not found");
    }
}

void BrpcServiceImpl::CreateTable(google::protobuf::RpcController *cntl_base,
                                  const infinity_brpc_proto::CreateTableRequest *request,
                                  infinity_brpc_proto::CommonResponse *response,
                                  google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    Vector<ColumnDef *> column_defs;
    for (auto &proto_column_def : request->column_defs()) {
        auto column_def = GetColumnDefFromProto(proto_column_def);
        column_defs.emplace_back(column_def);
    }

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    CreateTableOptions create_table_opts;
    auto result = database->CreateTable(request->table_name(), column_defs, Vector<TableConstraint *>(), create_table_opts);

    ProcessResult(result, response);
}

void BrpcServiceImpl::DropTable(google::protobuf::RpcController *cntl_base,
                                const infinity_brpc_proto::DropTableRequest *request,
                                infinity_brpc_proto::CommonResponse *response,
                                google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto result = database->DropTable(request->table_name(), (const DropTableOptions &)request->options());

    ProcessResult(result, response);
}

void BrpcServiceImpl::ListTable(google::protobuf::RpcController *cntl_base,
                                const infinity_brpc_proto::ListTableRequest *request,
                                infinity_brpc_proto::ListTableResponse *response,
                                google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto result = database->ListTables();
    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);

        auto row_count = data_block->row_count();
        for (int i = 0; i < row_count; ++i) {
            Value value = data_block->GetValue(1, i);
            if (value.value_.varchar.IsInlined()) {
                String prefix = String(value.value_.varchar.prefix, value.value_.varchar.length);
                response->add_table_name(prefix);
            } else {
                String whole_str = String(value.value_.varchar.ptr, value.value_.varchar.length);
                response->add_table_name(whole_str);
            }
        }

        response->set_success(true);

    } else {
        LOG(ERROR) << result.ErrorMsg();
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
    }
}

void BrpcServiceImpl::CreateIndex(google::protobuf::RpcController *cntl_base,
                                  const infinity_brpc_proto::CreateIndexRequest *request,
                                  infinity_brpc_proto::CommonResponse *response,
                                  google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto table = database->GetTable(request->table_name());
    auto column_names = new Vector<String>();

    column_names->reserve(request->column_names_size());
    for (auto &column_name : request->column_names()) {
        column_names->emplace_back(column_name);
    }

    const String &method_type = request->method_type();
    auto *index_para_list = new Vector<InitParameter *>();

    for (auto &index_para : request->index_para_list()) {
        auto init_parameter = new InitParameter();
        init_parameter->para_name_ = index_para.para_name();
        init_parameter->para_value_ = index_para.para_value();
        index_para_list->emplace_back(init_parameter);
    }

    auto result = table->CreateIndex(request->index_name(), column_names, method_type, index_para_list, (CreateIndexOptions &)request->options());

    ProcessResult(result, response);
}

void BrpcServiceImpl::DropIndex(google::protobuf::RpcController *cntl_base,
                                const infinity_brpc_proto::DropIndexRequest *request,
                                infinity_brpc_proto::CommonResponse *response,
                                google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto table = database->GetTable(request->table_name());
    auto result = table->DropIndex(request->index_name());

    ProcessResult(result, response);
}

void BrpcServiceImpl::Search(google::protobuf::RpcController *cntl_base,
                             const infinity_brpc_proto::SelectStatement *request,
                             infinity_brpc_proto::SelectResponse *response,
                             google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto table = database->GetTable(request->table_name());
    Vector<ParsedExpr *> *output_columns;
    output_columns = new Vector<ParsedExpr *>();
    output_columns->reserve(request->select_list_size());

    for (auto &expr : request->select_list()) {
        auto parsed_expr = GetColumnExprFromProto(expr.column_expr());
        output_columns->emplace_back(parsed_expr);
    }

    Vector<Pair<ParsedExpr *, ParsedExpr *>> vector_expr{};

    Vector<Pair<ParsedExpr *, ParsedExpr *>> fts_expr{};

    ParsedExpr *filter = GetParsedExprFromProto(request->where_expr());

    // TODO:
    //    ParsedExpr *offset;
    //    offset = new ParsedExpr();
    //    ParsedExpr *limit;
    //    limit = new ConstantExpr (0);

    auto result = table->Search(vector_expr, fts_expr, filter, output_columns, nullptr, nullptr);

    if (result.IsOk()) {
        auto data_block_count = result.result_table_->DataBlockCount();
        auto column_count = result.result_table_->ColumnCount();

        Vector<SharedPtr<ColumnVector>> all_column_vectors;
        all_column_vectors.reserve(column_count);
        SizeT all_row_count = 0;
        for (SizeT block_idx = 0; block_idx < data_block_count; ++block_idx) {
            auto data_block = result.result_table_->GetDataBlockById(block_idx);
            auto row_count = data_block->row_count();
            all_row_count += row_count;

            for (SizeT col_index = 0; col_index < column_count; ++col_index) {
                auto column_vector = data_block->column_vectors[col_index];

                if (block_idx == 0) {
                    all_column_vectors.emplace_back(column_vector);
                } else {
                    all_column_vectors[col_index]->AppendWith(*column_vector.get());
                }
            }
        }

        for (auto &column_vector : all_column_vectors) {
            auto size = column_vector->data_type()->Size() * all_row_count;
            char *dst = new char[size];
            memcpy(dst, column_vector->data(), size);

            infinity_brpc_proto::ColumnField columnField;
            columnField.set_column_vector(dst, size);
            columnField.set_column_type(DataTypeToProtoColumnType(column_vector->data_type()));
            response->add_column_fields()->CopyFrom(columnField);
        }

        auto table_def = result.result_table_->definition_ptr_;

        for (SizeT col_index = 0; col_index < column_count; ++col_index) {
            auto column_def = table_def->columns()[col_index];
            infinity_brpc_proto::ColumnDef proto_column_def;
            proto_column_def.set_id(column_def->id());
            proto_column_def.set_name(column_def->name());
            infinity_brpc_proto::DataType proto_data_type;
            proto_column_def.set_allocated_column_type(DataTypeToProtoDataType(column_def->type()));
            response->add_column_defs()->CopyFrom(proto_column_def);
        }

        response->set_success(true);
    } else {
        LOG(ERROR) << result.ErrorMsg();
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
    }
}

void BrpcServiceImpl::Import(google::protobuf::RpcController *cntl_base,
                             const infinity_brpc_proto::ImportRequest *request,
                             infinity_brpc_proto::CommonResponse *response,
                             google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto table = database->GetTable(request->table_name());
    auto result = table->Import(request->file_path(), (const ImportOptions &)request->import_options());

    ProcessResult(result, response);
}

void BrpcServiceImpl::Insert(google::protobuf::RpcController *cntl_base,
                             const infinity_brpc_proto::InsertRequest *request,
                             infinity_brpc_proto::CommonResponse *response,
                             google::protobuf::Closure *done) {

    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = SetUpController(cntl_base);

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto table = database->GetTable(request->table_name());
    auto columns = new Vector<String>();
    columns->reserve(request->column_names().size());

    for (auto &column : request->column_names()) {
        columns->emplace_back(column);
    }

    auto values = new Vector<Vector<ParsedExpr *> *>();
    values->reserve(request->fields().size());

    for (auto &value : request->fields()) {
        auto value_list = new Vector<ParsedExpr *>();
        value_list->reserve(value.parse_exprs_size());
        for (auto &expr : value.parse_exprs()) {
            auto parsed_expr = GetConstantFromProto(expr.constant_expr().literal_type(), expr.constant_expr());
            value_list->emplace_back(parsed_expr);
        }
        values->emplace_back(value_list);
    }

    auto result = table->Insert(columns, values);

    ProcessResult(result, response);
}

void BrpcServiceImpl::Run() {

    brpc::Server server;

    BrpcServiceImpl service;
    if (server.AddService(&service, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "Fail to add service" << std::endl;
    }

    // Start the server.
    brpc::ServerOptions options;
    options.idle_timeout_sec = 2;
    options.num_threads = 4;
    if (server.Start("0.0.0.0:50051", &options) != 0) {
        LOG(ERROR) << "Failed to start server";
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    LOG(INFO) << "0.0.0.0:50051 Start BRPC";
    server.RunUntilAskedToQuit();
}

ColumnDef *BrpcServiceImpl::GetColumnDefFromProto(const infinity_brpc_proto::ColumnDef &column_def) {
    auto column_def_data_type_ptr = GetColumnTypeFromProto(column_def.column_type());
    auto constraints = HashSet<ConstraintType>();
    auto constraints_size = column_def.constraints_size();

    for (int i = 0; i < constraints_size; ++i) {
        auto type = GetConstraintTypeFromProto(column_def.constraints(i));
        constraints.insert(type);
    }

    const auto &column_def_name = column_def.name();
    auto col_def = new ColumnDef(column_def.id(), column_def_data_type_ptr, column_def_name, constraints);
    return col_def;
}

infinity_brpc_proto::ColumnType BrpcServiceImpl::DataTypeToProtoColumnType(const SharedPtr<DataType> &data_type) {
    switch (data_type->type()) {
        case LogicalType::kBoolean:
            return infinity_brpc_proto::ColumnType::kColumnBool;
        case LogicalType::kTinyInt:
            return infinity_brpc_proto::ColumnType::kColumnInt8;
        case LogicalType::kSmallInt:
            return infinity_brpc_proto::ColumnType::kColumnInt16;
        case LogicalType::kInteger:
            return infinity_brpc_proto::ColumnType::kColumnInt32;
        case LogicalType::kBigInt:
            return infinity_brpc_proto::ColumnType::kColumnInt64;
        case LogicalType::kFloat:
            return infinity_brpc_proto::ColumnType::kColumnFloat;
        case LogicalType::kDouble:
            return infinity_brpc_proto::ColumnType::kColumnDouble;
        case LogicalType::kVarchar:
            return infinity_brpc_proto::ColumnType::kColumnVarchar;
        case LogicalType::kEmbedding:
            return infinity_brpc_proto::ColumnType::kColumnEmbedding;
        default:
            Error<TypeException>("Invalid data type", __FILE_NAME__, __LINE__);
    }
    return infinity_brpc_proto::ColumnType::kColumnInvalid;
}

infinity_brpc_proto::DataType *BrpcServiceImpl::DataTypeToProtoDataType(const SharedPtr<DataType> &data_type) {
    switch (data_type->type()) {
        case LogicalType::kBoolean: {
            auto *data_type_proto = new infinity_brpc_proto::DataType();
            data_type_proto->set_logic_type(infinity_brpc_proto::Boolean);
            return data_type_proto;
        }
        case LogicalType::kTinyInt: {
            auto *data_type_proto = new infinity_brpc_proto::DataType();
            data_type_proto->set_logic_type(infinity_brpc_proto::TinyInt);
            return data_type_proto;
        }
        case LogicalType::kSmallInt: {
            auto *data_type_proto = new infinity_brpc_proto::DataType();
            data_type_proto->set_logic_type(infinity_brpc_proto::SmallInt);
            return data_type_proto;
        }
        case LogicalType::kInteger: {
            auto *data_type_proto = new infinity_brpc_proto::DataType();
            data_type_proto->set_logic_type(infinity_brpc_proto::Integer);
            return data_type_proto;
        }
        case LogicalType::kBigInt: {
            auto *data_type_proto = new infinity_brpc_proto::DataType();
            data_type_proto->set_logic_type(infinity_brpc_proto::BigInt);
            return data_type_proto;
        }
        case LogicalType::kFloat: {
            auto *data_type_proto = new infinity_brpc_proto::DataType();
            data_type_proto->set_logic_type(infinity_brpc_proto::Float);
            return data_type_proto;
        }
        case LogicalType::kDouble: {
            auto *data_type_proto = new infinity_brpc_proto::DataType();
            data_type_proto->set_logic_type(infinity_brpc_proto::Double);
            return data_type_proto;
        }
        case LogicalType::kVarchar: {
            auto *data_type_proto = new infinity_brpc_proto::DataType();
            infinity_brpc_proto::VarcharType varchar_type;
            // FIXME
            varchar_type.set_width(0);
            data_type_proto->set_logic_type(infinity_brpc_proto::Varchar);
            data_type_proto->set_allocated_varchar_type(&varchar_type);
            return data_type_proto;
        }
        case LogicalType::kEmbedding: {
            infinity_brpc_proto::DataType *data_type_proto = new infinity_brpc_proto::DataType();
            infinity_brpc_proto::EmbeddingType embedding_type;
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
            embedding_type.set_dimension(embedding_info->Dimension());
            data_type_proto->set_logic_type(infinity_brpc_proto::Embedding);
            data_type_proto->set_allocated_embedding_type(&embedding_type);
            return data_type_proto;
        }
        default:
            Error<TypeException>("Invalid data type", __FILE_NAME__, __LINE__);
    }
    return nullptr;
}

SharedPtr<DataType> BrpcServiceImpl::GetColumnTypeFromProto(const infinity_brpc_proto::DataType &type) {
    switch (type.logic_type()) {
        case infinity_brpc_proto::Boolean:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kBoolean);
        case infinity_brpc_proto::TinyInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kTinyInt);
        case infinity_brpc_proto::SmallInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kSmallInt);
        case infinity_brpc_proto::Integer:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kInteger);
        case infinity_brpc_proto::BigInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kBigInt);
        case infinity_brpc_proto::HugeInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kHugeInt);
        case infinity_brpc_proto::Decimal:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kDecimal);
        case infinity_brpc_proto::Float:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kFloat);
        case infinity_brpc_proto::Double:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kDouble);
        case infinity_brpc_proto::Varchar:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kVarchar);
        case infinity_brpc_proto::Embedding: {
            auto embedding_type = GetEmbeddingDataTypeFromProto(type.embedding_type().embedding_data_type());
            auto embedding_info = EmbeddingInfo::Make(embedding_type, type.embedding_type().dimension());
            return MakeShared<infinity::DataType>(infinity::LogicalType::kEmbedding, embedding_info);
        };
        default:
            Error<TypeException>("Invalid data type", __FILE_NAME__, __LINE__);
    }
    return nullptr;
}

ConstraintType BrpcServiceImpl::GetConstraintTypeFromProto(const infinity_brpc_proto::Constraint &constraint) {
    switch (constraint) {
        case infinity_brpc_proto::kNotNull:
            return ConstraintType::kNotNull;
        case infinity_brpc_proto::kNull:
            return ConstraintType::kNull;
        case infinity_brpc_proto::kPrimaryKey:
            return ConstraintType::kPrimaryKey;
        case infinity_brpc_proto::kUnique:
            return ConstraintType::kUnique;
        default:
            Error<TypeException>("Invalid constraint type", __FILE_NAME__, __LINE__);
    }
}

EmbeddingDataType BrpcServiceImpl::GetEmbeddingDataTypeFromProto(const infinity_brpc_proto::ElementType &type) {
    switch (type) {
        case infinity_brpc_proto::kElemBit:
            return EmbeddingDataType::kElemBit;
        case infinity_brpc_proto::kElemInt8:
            return EmbeddingDataType::kElemInt8;
        case infinity_brpc_proto::kElemInt16:
            return EmbeddingDataType::kElemInt16;
        case infinity_brpc_proto::kElemInt32:
            return EmbeddingDataType::kElemInt32;
        case infinity_brpc_proto::kElemInt64:
            return EmbeddingDataType::kElemInt64;
        case infinity_brpc_proto::kElemFloat:
            return EmbeddingDataType::kElemFloat;
        case infinity_brpc_proto::kElemDouble:
            return EmbeddingDataType::kElemDouble;
        default:
            Error<TypeException>("Invalid embedding data type", __FILE_NAME__, __LINE__);
    }
}

ConstantExpr *BrpcServiceImpl::GetConstantFromProto(const infinity_brpc_proto::ConstantExpr_LiteralType &literal_type,
                                                    const infinity_brpc_proto::ConstantExpr &expr) {
    switch (literal_type) {
        case infinity_brpc_proto::ConstantExpr_LiteralType_kBoolean: {
            auto parsed_expr = new ConstantExpr(LiteralType::kBoolean);
            parsed_expr->bool_value_ = expr.bool_value();
            return parsed_expr;
        }
        case infinity_brpc_proto::ConstantExpr_LiteralType_kDouble: {
            auto parsed_expr = new ConstantExpr(LiteralType::kDouble);
            parsed_expr->double_value_ = expr.f64_value();
            return parsed_expr;
        }
        case infinity_brpc_proto::ConstantExpr_LiteralType_kString: {
            auto parsed_expr = new ConstantExpr(LiteralType::kString);
            parsed_expr->str_value_ = strdup(expr.str_value().c_str());
            return parsed_expr;
        }
        case infinity_brpc_proto::ConstantExpr_LiteralType_kInt64: {
            auto parsed_expr = new ConstantExpr(LiteralType::kInteger);
            parsed_expr->integer_value_ = expr.i64_value();
            return parsed_expr;
        }
        case infinity_brpc_proto::ConstantExpr_LiteralType_kNull: {
            auto parsed_expr = new ConstantExpr(LiteralType::kNull);
            return parsed_expr;
        }
        case infinity_brpc_proto::ConstantExpr_LiteralType_kIntegerArray: {
            auto parsed_expr = new ConstantExpr(LiteralType::kIntegerArray);
            auto array_size = expr.long_array_size();
            for (int i = 0; i < array_size; ++i) {
                parsed_expr->long_array_.emplace_back(expr.long_array(i));
            }
            return parsed_expr;
        }
        case infinity_brpc_proto::ConstantExpr_LiteralType_kDoubleArray: {
            auto parsed_expr = new ConstantExpr(LiteralType::kDoubleArray);
            auto array_size = expr.double_array_size();
            for (int i = 0; i < array_size; ++i) {
                parsed_expr->double_array_.emplace_back(expr.double_array(i));
            }
            return parsed_expr;
        }
        default:
            Error<TypeException>("Invalid constant type", __FILE_NAME__, __LINE__);
    }
}

ColumnExpr *BrpcServiceImpl::GetColumnExprFromProto(const infinity_brpc_proto::ColumnExpr &column_expr) {
    auto parsed_expr = new ColumnExpr();

    for (auto &column_name : column_expr.column_name()) {
        parsed_expr->names_.emplace_back(column_name);
    }

    parsed_expr->star_ = column_expr.star();
    return parsed_expr;
}

FunctionExpr *BrpcServiceImpl::GetFunctionExprFromProto(const infinity_brpc_proto::FunctionExpr &function_expr) {
    auto *parsed_expr = new FunctionExpr();
    parsed_expr->func_name_ = function_expr.function_name();
    Vector<ParsedExpr *> *arguments;
    arguments = new Vector<ParsedExpr *>();
    arguments->reserve(function_expr.arguments_size());

    for (auto &args : function_expr.arguments()) {
        arguments->emplace_back(GetParsedExprFromProto(args));
    }

    parsed_expr->arguments_ = arguments;
    return parsed_expr;
}

ParsedExpr *BrpcServiceImpl::GetParsedExprFromProto(const infinity_brpc_proto::ParsedExpr &expr) {
    ParsedExpr *parsed_expr{nullptr};
    if (expr.has_column_expr()) {
        parsed_expr = GetColumnExprFromProto(expr.column_expr());
    } else if (expr.has_constant_expr()) {
        parsed_expr = GetConstantFromProto(expr.constant_expr().literal_type(), expr.constant_expr());
    } else if (expr.has_function_expr()) {
        parsed_expr = GetFunctionExprFromProto(expr.function_expr());
    }
    return parsed_expr;
}

SharedPtr<Infinity> BrpcServiceImpl::GetInfinityBySessionID(u64 session_id) {
    auto it = infinity_session_map_.find(session_id);
    if (it == infinity_session_map_.end()) {
        Error<NetworkException>("session id not found", __FILE_NAME__, __LINE__);
    }
    return it->second;
}

brpc::Controller *BrpcServiceImpl::SetUpController(google::protobuf::RpcController *cntl_base) {
    auto *cntl = static_cast<brpc::Controller *>(cntl_base);
    cntl->set_response_compress_type(brpc::COMPRESS_TYPE_GZIP);
    return cntl;
}

void BrpcServiceImpl::ProcessResult(const QueryResult &result, infinity_brpc_proto::CommonResponse *response) {
    if (result.IsOk()) {
        response->set_success(true);
    } else {
        LOG(ERROR) << result.ErrorMsg();
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
    }
}

} // namespace infinity
