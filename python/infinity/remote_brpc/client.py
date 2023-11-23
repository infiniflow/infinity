# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from .brpc_pb.infinity_brpc_pb2 import *
from .brpc_pb.infinity_brpc_pb2_grpc import *
from .. import URI


class BrpcInfinityClient:
    db_name: str
    channel: grpc.Channel
    stub: InfinityBrpcServiceStub

    def __init__(self, uri: URI):
        self.url = uri.ip + ":" + str(uri.port)
        self.db_name = "default"
        self.channel = grpc.insecure_channel(self.url)
        self.stub = InfinityBrpcServiceStub(self.channel)
        res = self.stub.Connect(Empty())
        self.session_id = res.session_id

    def create_database(self, db_name: str):
        return self.stub.CreateDatabase(CreateDatabaseRequest(session_id=self.session_id,
                                                              db_name=db_name,
                                                              options=None))

    def drop_database(self, db_name: str):
        return self.stub.DropDatabase(DropDatabaseRequest(session_id=self.session_id,
                                                          db_name=db_name))

    def list_databases(self):
        return self.stub.ListDatabase(ListDatabaseRequest(session_id=self.session_id))

    def describe_database(self, db_name: str):
        return self.stub.DescribeDatabase(DescribeDatabaseRequest(session_id=self.session_id,
                                                                  db_name=db_name))

    def get_database(self, db_name: str):
        return self.stub.GetDatabase(GetDatabaseRequest(session_id=self.session_id,
                                                        db_name=db_name))

    def create_table(self, db_name: str, table_name: str, column_defs, options):
        return self.stub.CreateTable(CreateTableRequest(session_id=self.session_id,
                                                        db_name=db_name,
                                                        table_name=table_name,
                                                        column_defs=column_defs,
                                                        options=options))

    def drop_table(self, db_name: str, table_name: str):
        return self.stub.DropTable(DropTableRequest(session_id=self.session_id,
                                                    db_name=db_name,
                                                    table_name=table_name))

    def list_tables(self, db_name: str):
        return self.stub.ListTable(ListTableRequest(session_id=self.session_id,
                                                    db_name=db_name))

    def describe_table(self, db_name: str, table_name: str):
        return self.stub.DescribeTable(DescribeTableRequest(session_id=self.session_id,
                                                            db_name=db_name,
                                                            table_name=table_name))

    def get_table(self, db_name: str, table_name: str):
        return self.stub.GetTable(GetTableRequest(session_id=self.session_id,
                                                  db_name=db_name,
                                                  table_name=table_name))

    def create_index(self, db_name: str, table_name: str, index_name: str, column_names: list[str], method_type: str,
                     index_para_list: InitParameter, options):
        return self.stub.CreateIndex(CreateIndexRequest(session_id=self.session_id,
                                                        db_name=db_name,
                                                        table_name=table_name,
                                                        index_name=index_name,
                                                        column_names=column_names,
                                                        method_type=method_type,
                                                        index_para_list=index_para_list,
                                                        options=options))

    def drop_index(self, db_name: str, table_name: str, index_name: str):
        return self.stub.DropIndex(DropIndexRequest(session_id=self.session_id,
                                                    db_name=db_name,
                                                    table_name=table_name,
                                                    index_name=index_name))

    def insert(self, db_name: str, table_name: str, column_names: list[str], fields: list[Field]):
        return self.stub.Insert(InsertRequest(session_id=self.session_id,
                                              db_name=db_name,
                                              table_name=table_name,
                                              column_names=column_names,
                                              fields=fields))

    def import_data(self, db_name: str, table_name: str, file_path: str, import_options):
        return self.stub.Import(ImportRequest(session_id=self.session_id,
                                              db_name=db_name,
                                              table_name=table_name,
                                              file_path=file_path,
                                              import_options=import_options))

    def select(self, db_name: str, table_name: str, select_list, where_expr, group_by_list, limit_expr, offset_expr,
               search_expr):
        return self.stub.Search(SelectStatement(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                select_list=select_list,
                                                where_expr=where_expr,
                                                group_by_list=group_by_list,
                                                limit_expr=limit_expr,
                                                offset_expr=offset_expr,
                                                search_expr=search_expr))

    def disconnect(self):
        res = self.stub.DisConnect(DisConnectRequest(session_id=self.session_id))
        self.channel.close()
        return res
