//
// Created by xwg on 2023/11/1.
//
#include "grpc_server.h"
#include "infinity.grpc.pb.h"
#include "infinity.pb.h"
#include <grpcpp/server.h>
#include <grpcpp/server_context.h>

import query_options;
import infinity_exception;
import data_block;
import value;
import column_vector;

namespace infinity {

grpc::Status GrpcServiceImpl::Connect(grpc::ServerContext *context, const infinity_proto::Empty *request, infinity_proto::CommonResponse *response) {
    auto infinity = Infinity::RemoteConnect();
    if (infinity == nullptr) {
        response->set_success(false);
        response->set_error_msg("Connect failed");
        return grpc::Status::CANCELLED;
    } else {
        infinity_session_map_mutex_.lock();
        infinity_session_map_.emplace(infinity->GetSessionId(), infinity);
        infinity_session_map_mutex_.unlock();
        response->set_session_id(infinity->GetSessionId());
        response->set_success(true);
        return grpc::Status::OK;
    }
}
grpc::Status GrpcServiceImpl::DisConnect(grpc::ServerContext *context,
                                         const infinity_proto::DisConnectRequest *request,
                                         infinity_proto::CommonResponse *response) {
    auto infinity = GetInfinityBySessionID(request->session_id());
    if (infinity == nullptr) {
        response->set_success(false);
        response->set_error_msg("Disconnect failed");
        return grpc::Status::CANCELLED;
    } else {
        auto session_id = infinity->GetSessionId();
        infinity->RemoteDisconnect();
        infinity_session_map_mutex_.lock();
        infinity_session_map_.erase(session_id);
        infinity_session_map_mutex_.unlock();
        response->set_success(true);
        return grpc::Status::OK;
    }
}

grpc::Status GrpcServiceImpl::CreateDatabase(grpc::ServerContext *context, const infinity_proto::CreateDatabaseRequest *request, infinity_proto::CommonResponse *response) {
    auto infinity = GetInfinityBySessionID(request->session_id());
    auto result = infinity->CreateDatabase(request->db_name(), (const CreateDatabaseOptions &)request->options());
    if (result.IsOk()) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status GrpcServiceImpl::DropDatabase(grpc::ServerContext *context, const infinity_proto::DropDatabaseRequest *request, infinity_proto::CommonResponse *response) {
    auto infinity = GetInfinityBySessionID(request->session_id());
    auto result = infinity->DropDatabase(request->db_name(), (const DropDatabaseOptions &)request->options());

    if (result.IsOk()) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status GrpcServiceImpl::ListDatabase(grpc::ServerContext *context, const infinity_proto::ListDatabaseRequest *request, infinity_proto::ListDatabaseResponse *response) {
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

        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status GrpcServiceImpl::DescribeDatabase(grpc::ServerContext *context, const infinity_proto::DescribeDatabaseRequest *request, infinity_proto::DescribeDatabaseResponse *response) {
    return grpc::Status::OK;
}

grpc::Status GrpcServiceImpl::GetDatabase(grpc::ServerContext *context, const infinity_proto::GetDatabaseRequest *request, infinity_proto::CommonResponse *response) {
    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    if (database != nullptr) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg("Database not found");
    }
}

grpc::Status GrpcServiceImpl::CreateTable(grpc::ServerContext *context, const infinity_proto::CreateTableRequest *request, infinity_proto::CommonResponse *response) {
    Vector<ColumnDef *> column_defs;

    for (auto &proto_column_def : request->column_defs()) {
        auto column_def = GetColumnDefFromProto(proto_column_def);
        column_defs.emplace_back(column_def);
    }

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    CreateTableOptions create_table_opts;
    Assert<NetworkException>(database != nullptr, "Database is null", __FILE_NAME__, __LINE__);
    auto result = database->CreateTable(request->table_name(), column_defs, Vector<TableConstraint *>(), create_table_opts);

    if (result.IsOk()) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status GrpcServiceImpl::DropTable(grpc::ServerContext *context, const infinity_proto::DropTableRequest *request, infinity_proto::CommonResponse *response) {
    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto result = database->DropTable(request->table_name(), (const DropTableOptions &)request->options());

    if (result.IsOk()) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status GrpcServiceImpl::ListTable(grpc::ServerContext *context, const infinity_proto::ListTableRequest *request, infinity_proto::ListTableResponse *response) {
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
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status
GrpcServiceImpl::GetTable(grpc::ServerContext *context, const infinity_proto::GetTableRequest *request, infinity_proto::CommonResponse *response) {
    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto table = database->GetTable(request->table_name());

    if (table != nullptr) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg("Table not found");
        return grpc::Status::CANCELLED;
    }
}

grpc::Status GrpcServiceImpl::CreateIndex(grpc::ServerContext *context,
                                          const infinity_proto::CreateIndexRequest *request,
                                          infinity_proto::CommonResponse *response) {

    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto table = database->GetTable(request->table_name());
    auto column_names = new Vector<String>();

    column_names->reserve(request->column_names_size());
    for (auto &column_name : request->column_names()) {
        column_names->emplace_back(column_name);
    }

    String method_type = request->method_type();
    auto *index_para_list = new Vector<InitParameter *>();

    for (auto &index_para : request->index_para_list()) {
        auto init_parameter = new InitParameter();
        init_parameter->para_name_ = index_para.para_name();
        init_parameter->para_value_ = index_para.para_value();
        index_para_list->emplace_back(init_parameter);
    }

    auto result = table->CreateIndex(request->index_name(), column_names, method_type, index_para_list, (CreateIndexOptions &)request->options());
    if (result.IsOk()) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status
GrpcServiceImpl::DropIndex(grpc::ServerContext *context, const infinity_proto::DropIndexRequest *request, infinity_proto::CommonResponse *response) {
    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto table = database->GetTable(request->table_name());
    auto result = table->DropIndex(request->index_name());

    if (result.IsOk()) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status
GrpcServiceImpl::Search(grpc::ServerContext *context, const infinity_proto::SelectStatement *request, infinity_proto::SelectResponse *response) {
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

            infinity_proto::ColumnField columnField;
            columnField.set_column_vector(dst, size);
            columnField.set_column_type(DataTypeToProtoColumnType(column_vector->data_type()));
            response->add_column_fields()->CopyFrom(columnField);
        }

        auto table_def = result.result_table_->definition_ptr_;

        for (SizeT col_index = 0; col_index < column_count; ++col_index) {
            auto column_def = table_def->columns()[col_index];
            infinity_proto::ColumnDef proto_column_def;
            proto_column_def.set_id(column_def->id());
            proto_column_def.set_name(column_def->name());
            infinity_proto::DataType proto_data_type;
            proto_column_def.set_allocated_column_type(DataTypeToProtoDataType(column_def->type()));
            response->add_column_defs()->CopyFrom(proto_column_def);
        }

        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status
GrpcServiceImpl::Import(grpc::ServerContext *context, const infinity_proto::ImportRequest *request, infinity_proto::CommonResponse *response) {
    auto infinity = GetInfinityBySessionID(request->session_id());
    auto database = infinity->GetDatabase(request->db_name());
    auto table = database->GetTable(request->table_name());
    auto result = table->Import(request->file_path(), (const ImportOptions &)request->import_options());

    if (result.IsOk()) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

grpc::Status
GrpcServiceImpl::Insert(grpc::ServerContext *context, const infinity_proto::InsertRequest *request, infinity_proto::CommonResponse *response) {
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

    if (result.IsOk()) {
        response->set_success(true);
        return grpc::Status::OK;
    } else {
        response->set_success(false);
        response->set_error_msg(result.ErrorMsg());
        return grpc::Status::CANCELLED;
    }
}

void GrpcServiceImpl::Run() {
    // Grpc service starts immediately
    std::string server_address("0.0.0.0:50051");
    GrpcServiceImpl service;
    grpc::ServerBuilder builder;
    builder.RegisterService(&service);
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    std::unique_ptr<grpc::Server> server = builder.BuildAndStart();
    std::cout << "GRPC server listening on " << server_address << std::endl;
    server->Wait();
}

void GrpcServiceImpl::Shutdown() {
    // TODO:
}

ColumnDef *GrpcServiceImpl::GetColumnDefFromProto(const infinity_proto::ColumnDef &column_def) {
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

infinity_proto::ColumnType GrpcServiceImpl::DataTypeToProtoColumnType(const SharedPtr<DataType> &data_type) {
    switch (data_type->type()) {
        case LogicalType::kBoolean:
            return infinity_proto::ColumnType::kColumnBool;
        case LogicalType::kTinyInt:
            return infinity_proto::ColumnType::kColumnInt8;
        case LogicalType::kSmallInt:
            return infinity_proto::ColumnType::kColumnInt16;
        case LogicalType::kInteger:
            return infinity_proto::ColumnType::kColumnInt32;
        case LogicalType::kBigInt:
            return infinity_proto::ColumnType::kColumnInt64;
        case LogicalType::kFloat:
            return infinity_proto::ColumnType::kColumnFloat;
        case LogicalType::kDouble:
            return infinity_proto::ColumnType::kColumnDouble;
        case LogicalType::kVarchar:
            return infinity_proto::ColumnType::kColumnVarchar;
        case LogicalType::kEmbedding:
            return infinity_proto::ColumnType::kColumnEmbedding;
        default:
            Error<TypeException>("Invalid data type", __FILE_NAME__, __LINE__);
    }
}

infinity_proto::DataType *GrpcServiceImpl::DataTypeToProtoDataType(const SharedPtr<DataType> &data_type) {
    switch (data_type->type()) {
        case LogicalType::kBoolean: {
            infinity_proto::DataType *data_type_proto = new infinity_proto::DataType();
            data_type_proto->set_logic_type(infinity_proto::Boolean);
            return data_type_proto;
        }
        case LogicalType::kTinyInt: {
            infinity_proto::DataType *data_type_proto = new infinity_proto::DataType();
            data_type_proto->set_logic_type(infinity_proto::TinyInt);
            return data_type_proto;
        }
        case LogicalType::kSmallInt: {
            infinity_proto::DataType *data_type_proto = new infinity_proto::DataType();
            data_type_proto->set_logic_type(infinity_proto::SmallInt);
            return data_type_proto;
        }
        case LogicalType::kInteger: {
            infinity_proto::DataType *data_type_proto = new infinity_proto::DataType();
            data_type_proto->set_logic_type(infinity_proto::Integer);
            return data_type_proto;
        }
        case LogicalType::kBigInt: {
            infinity_proto::DataType *data_type_proto = new infinity_proto::DataType();
            data_type_proto->set_logic_type(infinity_proto::BigInt);
            return data_type_proto;
        }
        case LogicalType::kFloat: {
            infinity_proto::DataType *data_type_proto = new infinity_proto::DataType();
            data_type_proto->set_logic_type(infinity_proto::Float);
            return data_type_proto;
        }
        case LogicalType::kDouble: {
            infinity_proto::DataType *data_type_proto = new infinity_proto::DataType();
            data_type_proto->set_logic_type(infinity_proto::Double);
            return data_type_proto;
        }
        case LogicalType::kVarchar: {
            infinity_proto::DataType *data_type_proto = new infinity_proto::DataType();
            infinity_proto::VarcharType varchar_type;
            auto varchar_info = static_cast<VarcharInfo *>(data_type->type_info().get());
            varchar_type.set_width(varchar_info->dimension());
            data_type_proto->set_logic_type(infinity_proto::Varchar);
            data_type_proto->set_allocated_varchar_type(&varchar_type);
            return data_type_proto;
        }
        case LogicalType::kEmbedding: {
            infinity_proto::DataType *data_type_proto = new infinity_proto::DataType();
            infinity_proto::EmbeddingType embedding_type;
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
            embedding_type.set_dimension(embedding_info->Dimension());
            data_type_proto->set_logic_type(infinity_proto::Embedding);
            data_type_proto->set_allocated_embedding_type(&embedding_type);
            return data_type_proto;
        }
        default:
            Error<TypeException>("Invalid data type", __FILE_NAME__, __LINE__);
    }
}

SharedPtr<DataType> GrpcServiceImpl::GetColumnTypeFromProto(const infinity_proto::DataType &type) {
    switch (type.logic_type()) {
        case infinity_proto::Boolean:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kBoolean);
        case infinity_proto::TinyInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kTinyInt);
        case infinity_proto::SmallInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kSmallInt);
        case infinity_proto::Integer:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kInteger);
        case infinity_proto::BigInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kBigInt);
        case infinity_proto::HugeInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kHugeInt);
        case infinity_proto::Decimal:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kDecimal);
        case infinity_proto::Float:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kFloat);
        case infinity_proto::Double:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kDouble);
        case infinity_proto::Varchar:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kVarchar);
        case infinity_proto::Embedding: {
            auto embedding_type = GetEmbeddingDataTypeFromProto(type.embedding_type().embedding_data_type());
            auto embedding_info = EmbeddingInfo::Make(embedding_type, type.embedding_type().dimension());
            return MakeShared<infinity::DataType>(infinity::LogicalType::kEmbedding, embedding_info);
        };
        default:
            Error<TypeException>("Invalid data type", __FILE_NAME__, __LINE__);
    }
}

ConstraintType GrpcServiceImpl::GetConstraintTypeFromProto(const infinity_proto::Constraint &constraint) {
    switch (constraint) {
        case infinity_proto::kNotNull:
            return ConstraintType::kNotNull;
        case infinity_proto::kNull:
            return ConstraintType::kNull;
        case infinity_proto::kPrimaryKey:
            return ConstraintType::kPrimaryKey;
        case infinity_proto::kUnique:
            return ConstraintType::kUnique;
        default:
            Error<TypeException>("Invalid constraint type", __FILE_NAME__, __LINE__);
    }
}
EmbeddingDataType GrpcServiceImpl::GetEmbeddingDataTypeFromProto(const infinity_proto::ElementType &type) {
    switch (type) {
        case infinity_proto::kElemBit:
            return EmbeddingDataType::kElemBit;
        case infinity_proto::kElemInt8:
            return EmbeddingDataType::kElemInt8;
        case infinity_proto::kElemInt16:
            return EmbeddingDataType::kElemInt16;
        case infinity_proto::kElemInt32:
            return EmbeddingDataType::kElemInt32;
        case infinity_proto::kElemInt64:
            return EmbeddingDataType::kElemInt64;
        case infinity_proto::kElemFloat:
            return EmbeddingDataType::kElemFloat;
        case infinity_proto::kElemDouble:
            return EmbeddingDataType::kElemDouble;
        default:
            Error<TypeException>("Invalid embedding data type", __FILE_NAME__, __LINE__);
    }
}
ConstantExpr *GrpcServiceImpl::GetConstantFromProto(const infinity_proto::ConstantExpr_LiteralType &literal_type,
                                                    const infinity_proto::ConstantExpr &expr) {
    switch (literal_type) {
        case infinity_proto::ConstantExpr_LiteralType_kBoolean: {
            auto parsed_expr = new ConstantExpr(LiteralType::kBoolean);
            parsed_expr->bool_value_ = expr.bool_value();
            return parsed_expr;
        }
        case infinity_proto::ConstantExpr_LiteralType_kDouble: {
            auto parsed_expr = new ConstantExpr(LiteralType::kDouble);
            parsed_expr->double_value_ = expr.f64_value();
            return parsed_expr;
        }
        case infinity_proto::ConstantExpr_LiteralType_kString: {
            auto parsed_expr = new ConstantExpr(LiteralType::kString);
            parsed_expr->str_value_ = strdup(expr.str_value().c_str());
            return parsed_expr;
        }
        case infinity_proto::ConstantExpr_LiteralType_kInt64: {
            auto parsed_expr = new ConstantExpr(LiteralType::kInteger);
            parsed_expr->integer_value_ = expr.i64_value();
            return parsed_expr;
        }
        case infinity_proto::ConstantExpr_LiteralType_kNull: {
            auto parsed_expr = new ConstantExpr(LiteralType::kNull);
            return parsed_expr;
        }
        case infinity_proto::ConstantExpr_LiteralType_kIntegerArray: {
            auto parsed_expr = new ConstantExpr(LiteralType::kIntegerArray);
            auto array_size = expr.long_array_size();
            for (int i = 0; i < array_size; ++i) {
                parsed_expr->long_array_.emplace_back(expr.long_array(i));
            }
            return parsed_expr;
        }
        case infinity_proto::ConstantExpr_LiteralType_kDoubleArray: {
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
ColumnExpr *GrpcServiceImpl::GetColumnExprFromProto(const infinity_proto::ColumnExpr &column_expr) {
    auto parsed_expr = new ColumnExpr();

    for (auto &column_name : column_expr.column_name()) {
        parsed_expr->names_.emplace_back(column_name);
    }

    parsed_expr->star_ = column_expr.star();
    return parsed_expr;
}

FunctionExpr *GrpcServiceImpl::GetFunctionExprFromProto(const infinity_proto::FunctionExpr &function_expr) {
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

ParsedExpr *GrpcServiceImpl::GetParsedExprFromProto(const infinity_proto::ParsedExpr &expr) {
    if (expr.has_column_expr()) {
        auto parsed_expr = GetColumnExprFromProto(expr.column_expr());
        return parsed_expr;
    } else if (expr.has_constant_expr()) {
        auto parsed_expr = GetConstantFromProto(expr.constant_expr().literal_type(), expr.constant_expr());
        return parsed_expr;
    } else if (expr.has_function_expr()) {
        auto parsed_expr = GetFunctionExprFromProto(expr.function_expr());
        return parsed_expr;
    }
}

SharedPtr<Infinity> GrpcServiceImpl::GetInfinityBySessionID(u64 session_id) {
    auto it = infinity_session_map_.find(session_id);
    if (it == infinity_session_map_.end()) {
        Error<NetworkException>("session id not found", __FILE_NAME__, __LINE__);
    }
    return it->second;
}

} // namespace infinity