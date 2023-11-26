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


#include "definition/column_def.h"
#include "expr/column_expr.h"
#include "expr/constant_expr.h"
#include "expr/function_expr.h"
#include "infinity.pb.h"
#include <brpc/server.h>

import stl;
import singleton;
import boost;
import infinity;
import database;
import table;
import parser;
import query_result;

namespace infinity {

class BrpcServiceImpl : public infinity_proto::InfinityService {
public:
    BrpcServiceImpl() = default;
    void Connect(google::protobuf::RpcController *cntl_base,
                 const infinity_proto::Empty *request,
                 infinity_proto::CommonResponse *response,
                 google::protobuf::Closure *done) override;

    void DisConnect(google::protobuf::RpcController *cntl_base,
                    const infinity_proto::DisConnectRequest *request,
                    infinity_proto::CommonResponse *response,
                    google::protobuf::Closure *done) override;

    void CreateDatabase(google::protobuf::RpcController *cntl_base,
                        const infinity_proto::CreateDatabaseRequest *request,
                        infinity_proto::CommonResponse *response,
                        google::protobuf::Closure *done) override;

    void DropDatabase(google::protobuf::RpcController *cntl_base,
                      const infinity_proto::DropDatabaseRequest *request,
                      infinity_proto::CommonResponse *response,
                      google::protobuf::Closure *done) override;

    void ListDatabase(google::protobuf::RpcController *cntl_base,
                      const infinity_proto::ListDatabaseRequest *request,
                      infinity_proto::ListDatabaseResponse *response,
                      google::protobuf::Closure *done) override;

    void DescribeDatabase(google::protobuf::RpcController *cntl_base,
                          const infinity_proto::DescribeDatabaseRequest *request,
                          infinity_proto::DescribeDatabaseResponse *response,
                          google::protobuf::Closure *done) override;

    void GetDatabase(google::protobuf::RpcController *cntl_base,
                     const infinity_proto::GetDatabaseRequest *request,
                     infinity_proto::CommonResponse *response,
                     google::protobuf::Closure *done) override;

    void CreateTable(google::protobuf::RpcController *cntl_base,
                     const infinity_proto::CreateTableRequest *request,
                     infinity_proto::CommonResponse *response,
                     google::protobuf::Closure *done) override;

    void DropTable(google::protobuf::RpcController *cntl_base,
                   const infinity_proto::DropTableRequest *request,
                   infinity_proto::CommonResponse *response,
                   google::protobuf::Closure *done) override;

    void CreateIndex(google::protobuf::RpcController *cntl_base,
                     const infinity_proto::CreateIndexRequest *request,
                     infinity_proto::CommonResponse *response,
                     google::protobuf::Closure *done) override;

    void DropIndex(google::protobuf::RpcController *cntl_base,
                   const infinity_proto::DropIndexRequest *request,
                   infinity_proto::CommonResponse *response,
                   google::protobuf::Closure *done) override;

    void Search(google::protobuf::RpcController *cntl_base,
                const infinity_proto::SelectStatement *request,
                infinity_proto::SelectResponse *response,
                google::protobuf::Closure *done) override;
    void Import(google::protobuf::RpcController *cntl_base,
                const infinity_proto::ImportRequest *request,
                infinity_proto::CommonResponse *response,
                google::protobuf::Closure *done) override;

    void Insert(google::protobuf::RpcController *cntl_base,
                const infinity_proto::InsertRequest *request,
                infinity_proto::CommonResponse *response,
                google::protobuf::Closure *done) override;

    void ListTable(google::protobuf::RpcController *cntl_base,
                   const infinity_proto::ListTableRequest *request,
                   infinity_proto::ListTableResponse *response,
                   google::protobuf::Closure *done) override;

public:
    static void Run(brpc::Server& server);
    static void Stop(brpc::Server& server);

private:
    static ColumnDef *GetColumnDefFromProto(const infinity_proto::ColumnDef &column_defs);
    static SharedPtr<DataType> GetColumnTypeFromProto(const infinity_proto::DataType &type);
    static ConstraintType GetConstraintTypeFromProto(const infinity_proto::Constraint &constraint);
    static EmbeddingDataType GetEmbeddingDataTypeFromProto(const infinity_proto::ElementType &type);
    static ConstantExpr *GetConstantFromProto(const infinity_proto::ConstantExpr_LiteralType &literal_type, const infinity_proto::ConstantExpr &expr);
    static ColumnExpr *GetColumnExprFromProto(const infinity_proto::ColumnExpr &column_expr);
    static ParsedExpr *GetParsedExprFromProto(const infinity_proto::ParsedExpr &expr);
    static FunctionExpr *GetFunctionExprFromProto(const infinity_proto::FunctionExpr &function_expr);

    static infinity_proto::DataType *DataTypeToProtoDataType(const SharedPtr<DataType> &data_type);
    static infinity_proto::ColumnType DataTypeToProtoColumnType (const SharedPtr<DataType>& data_type);

    SharedPtr<Infinity> GetInfinityBySessionID(u64 session_id);
    brpc::Controller *SetUpController(google::protobuf::RpcController *cntl_base);
    void ProcessResult(const QueryResult &result, infinity_proto::CommonResponse *response);

private:
    Mutex infinity_session_map_mutex_{};
    HashMap<u64, SharedPtr<Infinity>> infinity_session_map_{};
};
} // end namespace infinity
