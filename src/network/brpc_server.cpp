//
// Created by xwg on 2023/11/1.
//
#include "brpc_server.h"
#include "infinity.pb.h"

import query_options;
import infinity_exception;
import data_block;
import value;
import column_vector;

namespace infinity {

void infinity::BrpcServiceImpl::Connect(google::protobuf::RpcController *cntl_base,
                                        const infinity_proto::Empty *request,
                                        infinity_proto::CommonResponse *response,
                                        google::protobuf::Closure *done) {
    auto infinity = Infinity::RemoteConnect();
    if (infinity == nullptr) {
        response->set_success(false);
        response->set_error_msg("Connect failed");
    } else {
        infinity_session_map_mutex_.lock();
        infinity_session_map_.emplace(infinity->GetSessionId(), infinity);
        infinity_session_map_mutex_.unlock();
        response->set_session_id(infinity->GetSessionId());
        response->set_success(true);
    }
}

void BrpcServiceImpl::Run() {

    brpc::Server server;

    BrpcServiceImpl service;
    if (server.AddService(&service,
                          brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "Fail to add service" << std::endl;
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;
    if (server.Start(50051, &options) != 0) {
        std::cout << "Fail to start EchoServer" << std::endl;
    }
    server.RunUntilAskedToQuit();
    std::cout << "BRPC server listening on " << "0.0.0.0:50051" << std::endl;
}

ColumnDef *BrpcServiceImpl::GetColumnDefFromProto(const infinity_proto::ColumnDef &column_def) {
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

infinity_proto::ColumnType BrpcServiceImpl::DataTypeToProtoColumnType(const SharedPtr<DataType> &data_type) {
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

infinity_proto::DataType *BrpcServiceImpl::DataTypeToProtoDataType(const SharedPtr<DataType> &data_type) {
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

SharedPtr<DataType> BrpcServiceImpl::GetColumnTypeFromProto(const infinity_proto::DataType &type) {
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

ConstraintType BrpcServiceImpl::GetConstraintTypeFromProto(const infinity_proto::Constraint &constraint) {
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
EmbeddingDataType BrpcServiceImpl::GetEmbeddingDataTypeFromProto(const infinity_proto::ElementType &type) {
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
ConstantExpr *BrpcServiceImpl::GetConstantFromProto(const infinity_proto::ConstantExpr_LiteralType &literal_type,
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
ColumnExpr *BrpcServiceImpl::GetColumnExprFromProto(const infinity_proto::ColumnExpr &column_expr) {
    auto parsed_expr = new ColumnExpr();

    for (auto &column_name : column_expr.column_name()) {
        parsed_expr->names_.emplace_back(column_name);
    }

    parsed_expr->star_ = column_expr.star();
    return parsed_expr;
}

FunctionExpr *BrpcServiceImpl::GetFunctionExprFromProto(const infinity_proto::FunctionExpr &function_expr) {
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

ParsedExpr *BrpcServiceImpl::GetParsedExprFromProto(const infinity_proto::ParsedExpr &expr) {
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

SharedPtr<Infinity> BrpcServiceImpl::GetInfinityBySessionID(u64 session_id) {
    auto it = infinity_session_map_.find(session_id);
    if (it == infinity_session_map_.end()) {
        Error<NetworkException>("session id not found", __FILE_NAME__, __LINE__);
    }
    return it->second;
}

} // namespace infinity