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

#pragma once

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/channel.h>
#include "network/infinity_grpc/infinity_grpc.grpc.pb.h"
#include "network/infinity_grpc/infinity_grpc.pb.h"

import stl;
import infinity;
import database;
import table;
import parser;
import query_result;

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
    static void Run();
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
    grpc::Status ProcessResult(infinity_grpc_proto::CommonResponse *response, const QueryResult& result) const;

private:
    Mutex infinity_session_map_mutex_{};
    HashMap<u64, SharedPtr<Infinity>> infinity_session_map_{};
};

class GrpcAsyncServiceImpl {
public:
    ~GrpcAsyncServiceImpl() {
        server_->Shutdown();
        cq_->Shutdown();
    }

    static void Run();
    void Shutdown();

    class CallData {
    public:
        CallData(GrpcAsyncServiceImpl *service)
            : service_(service), status_(CREATE) { }

        virtual void Proceed() = 0;

    protected:
        GrpcAsyncServiceImpl* service_;
        grpc::ServerContext ctx_;

        enum CallStatus { CREATE, PROCESS, FINISH };
        CallStatus status_;  // The current serving state.
    };

    class ConnectCallData: public CallData {
    public:
        ConnectCallData(GrpcAsyncServiceImpl *service)
            : CallData(service), responder_(&ctx_) {
            Proceed();
        }

        void Proceed() final;

    private:
        infinity_grpc_proto::Empty request_;
        infinity_grpc_proto::CommonResponse response_;
        grpc::ServerAsyncResponseWriter<infinity_grpc_proto::CommonResponse> responder_;
    };

    class DisConnectCallData: public CallData {
    public:
        DisConnectCallData(GrpcAsyncServiceImpl *service)
            : CallData(service), responder_(&ctx_) {
            Proceed();
        }

        void Proceed() final;

    private:
        infinity_grpc_proto::DisConnectRequest request_;
        infinity_grpc_proto::CommonResponse response_;
        grpc::ServerAsyncResponseWriter<infinity_grpc_proto::CommonResponse> responder_;
    };

private:
    void RegisterSession(SharedPtr<Infinity> instance) {
        infinity_session_map_mutex_.lock();
        infinity_session_map_.emplace(instance->GetSessionId(), instance);
        infinity_session_map_mutex_.unlock();
    };
    SharedPtr<Infinity> GetInfinityBySessionID(u64 session_id);

    Mutex infinity_session_map_mutex_{};
    HashMap<u64, SharedPtr<Infinity>> infinity_session_map_{};

    UniquePtr<grpc::ServerCompletionQueue> cq_;
    infinity_grpc_proto::InfinityGrpcService::AsyncService service_{};
    UniquePtr<grpc::Server> server_{};
};

class GrpcAsyncClient {
public:
    explicit GrpcAsyncClient(SharedPtr<grpc::Channel> channel)
        : stub_(infinity_grpc_proto::InfinityGrpcService::NewStub(channel)) {}

    infinity_grpc_proto::CommonResponse Connect();

    infinity_grpc_proto::CommonResponse DisConnect(u64 session_id);
private:
    UniquePtr<infinity_grpc_proto::InfinityGrpcService::Stub> stub_;
};

} // end namespace infinity
