#pragma once
//
// Created by xwg on 2023/11/1.
//


#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include "network/infinity_grpc/infinity_grpc.grpc.pb.h"
#include "network/infinity_grpc/infinity_grpc.pb.h"

import stl;
import infinity;
import database;
import table;
import parser;

namespace infinity {

class GrpcServiceImpl : public infinity_grpc_proto::InfinityGrpcService::Service {
public:
    GrpcServiceImpl() = default;
    grpc::Status Connect(grpc::ServerContext *context, const infinity_grpc_proto::Empty *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status DisConnect(grpc::ServerContext *context, const infinity_grpc_proto::DisConnectRequest *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status CreateDatabase(grpc::ServerContext *context,
                                const infinity_grpc_proto::CreateDatabaseRequest *request,
                                infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status
    DropDatabase(grpc::ServerContext *context, const infinity_grpc_proto::DropDatabaseRequest *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status ListDatabase(grpc::ServerContext *context,
                              const infinity_grpc_proto::ListDatabaseRequest *request,
                              infinity_grpc_proto::ListDatabaseResponse *response) override;
    grpc::Status DescribeDatabase(grpc::ServerContext *context,
                                  const infinity_grpc_proto::DescribeDatabaseRequest *request,
                                  infinity_grpc_proto::DescribeDatabaseResponse *response) override;
    grpc::Status
    GetDatabase(grpc::ServerContext *context, const infinity_grpc_proto::GetDatabaseRequest *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status
    CreateTable(grpc::ServerContext *context, const infinity_grpc_proto::CreateTableRequest *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status
    DropTable(grpc::ServerContext *context, const infinity_grpc_proto::DropTableRequest *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status
    ListTable(grpc::ServerContext *context, const infinity_grpc_proto::ListTableRequest *request, infinity_grpc_proto::ListTableResponse *response) override;
    grpc::Status
    GetTable(grpc::ServerContext *context, const infinity_grpc_proto::GetTableRequest *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status
    CreateIndex(grpc::ServerContext *context, const infinity_grpc_proto::CreateIndexRequest *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status
    DropIndex(grpc::ServerContext *context, const infinity_grpc_proto::DropIndexRequest *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status
    Search(grpc::ServerContext *context, const infinity_grpc_proto::SelectStatement *request, infinity_grpc_proto::SelectResponse *response) override;
    grpc::Status
    Import(grpc::ServerContext *context, const infinity_grpc_proto::ImportRequest *request, infinity_grpc_proto::CommonResponse *response) override;
    grpc::Status
    Insert(grpc::ServerContext *context, const infinity_grpc_proto::InsertRequest *request, infinity_grpc_proto::CommonResponse *response) override;

public:
    static void Run(UniquePtr<grpc::Server>& server_ptr);
    void Shutdown();

private:
    static ColumnDef *GetColumnDefFromProto(const infinity_grpc_proto::ColumnDef &column_defs);
    static SharedPtr<DataType> GetColumnTypeFromProto(const infinity_grpc_proto::DataType &type);
    static ConstraintType GetConstraintTypeFromProto(const infinity_grpc_proto::Constraint &constraint);
    static EmbeddingDataType GetEmbeddingDataTypeFromProto(const infinity_grpc_proto::ElementType &type);
    static ConstantExpr *GetConstantFromProto(const infinity_grpc_proto::ConstantExpr_LiteralType &literal_type, const infinity_grpc_proto::ConstantExpr &expr);
    static ColumnExpr *GetColumnExprFromProto(const infinity_grpc_proto::ColumnExpr &column_expr);
    static ParsedExpr *GetParsedExprFromProto(const infinity_grpc_proto::ParsedExpr &expr);
    static FunctionExpr *GetFunctionExprFromProto(const infinity_grpc_proto::FunctionExpr &function_expr);

    static infinity_grpc_proto::DataType *DataTypeToProtoDataType(const SharedPtr<DataType> &data_type);
    static infinity_grpc_proto::ColumnType DataTypeToProtoColumnType (const SharedPtr<DataType>& data_type);

    SharedPtr<Infinity> GetInfinityBySessionID(u64 session_id);

private:
    Mutex infinity_session_map_mutex_{};
    HashMap<u64, SharedPtr<Infinity>> infinity_session_map_{};
};
} // end namespace infinity
