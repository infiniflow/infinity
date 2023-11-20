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

import grpc
from infinity.remote.infinity_grpc import infinity_pb2_grpc, infinity_pb2


class GrpcInfinityClient:
    db_name: str
    channel: grpc.Channel
    stub: infinity_pb2_grpc.InfinityServiceStub

    def __init__(self, url='localhost:50051'):
        self.url = url
        self.db_name = "default"
        self.channel = grpc.insecure_channel(url)
        self.stub = infinity_pb2_grpc.InfinityServiceStub(self.channel)
        res = self.stub.Connect(infinity_pb2.Empty())
        self.session_id = res.session_id

    def create_database(self, db_name: str):
        return self.stub.CreateDatabase(infinity_pb2.CreateDatabaseRequest(session_id=self.session_id,
                                                                           db_name=db_name,
                                                                           options=None))

    def drop_database(self, db_name: str):
        return self.stub.DropDatabase(infinity_pb2.DropDatabaseRequest(session_id=self.session_id,
                                                                       db_name=db_name))

    def list_databases(self):
        return self.stub.ListDatabase(infinity_pb2.ListDatabaseRequest(session_id=self.session_id))

    def describe_database(self, db_name: str):
        return self.stub.DescribeDatabase(infinity_pb2.DescribeDatabaseRequest(session_id=self.session_id,
                                                                               db_name=db_name))

    def get_database(self, db_name: str):
        return self.stub.GetDatabase(infinity_pb2.GetDatabaseRequest(session_id=self.session_id,
                                                                     db_name=db_name))

    def create_table(self, db_name: str, table_name: str, column_defs, options):
        return self.stub.CreateTable(infinity_pb2.CreateTableRequest(session_id=self.session_id,
                                                                     db_name=db_name,
                                                                     table_name=table_name,
                                                                     column_defs=column_defs,
                                                                     options=options))

    def drop_table(self, db_name: str, table_name: str):
        return self.stub.DropTable(infinity_pb2.DropTableRequest(session_id=self.session_id,
                                                                 db_name=db_name,
                                                                 table_name=table_name))

    def list_tables(self, db_name: str):
        return self.stub.ListTable(infinity_pb2.ListTableRequest(session_id=self.session_id,
                                                                 db_name=db_name))

    def describe_table(self, db_name: str, table_name: str):
        return self.stub.DescribeTable(infinity_pb2.DescribeTableRequest(session_id=self.session_id,
                                                                         db_name=db_name,
                                                                         table_name=table_name))

    def get_table(self, db_name: str, table_name: str):
        return self.stub.GetTable(infinity_pb2.GetTableRequest(session_id=self.session_id,
                                                               db_name=db_name,
                                                               table_name=table_name))

    def create_index(self, db_name: str, table_name: str, index_name: str, column_names: list[str], method_type: str,
                     index_para_list: infinity_pb2.InitParameter, options):
        return self.stub.CreateIndex(infinity_pb2.CreateIndexRequest(session_id=self.session_id,
                                                                     db_name=db_name,
                                                                     table_name=table_name,
                                                                     index_name=index_name,
                                                                     column_names=column_names,
                                                                     method_type=method_type,
                                                                     index_para_list=index_para_list,
                                                                     options=options))

    def drop_index(self, db_name: str, table_name: str, index_name: str):
        return self.stub.DropIndex(infinity_pb2.DropIndexRequest(session_id=self.session_id,
                                                                 db_name=db_name,
                                                                 table_name=table_name,
                                                                 index_name=index_name))

    def insert(self, db_name: str, table_name: str, column_names: list[str], fields: list[infinity_pb2.Field]):
        return self.stub.Insert(infinity_pb2.InsertRequest(session_id=self.session_id,
                                                           db_name=db_name,
                                                           table_name=table_name,
                                                           column_names=column_names,
                                                           fields=fields))

    def import_data(self, db_name: str, table_name: str, file_path: str, import_options):

        try:
            res = self.stub.Import(infinity_pb2.ImportRequest(session_id=self.session_id,
                                                              db_name=db_name,
                                                              table_name=table_name,
                                                              file_path=file_path,
                                                              import_options=import_options))
            return res
        except Exception as e:
            print(e)
            return None

    def select(self, db_name: str, table_name: str, select_list, where_expr, group_by_list, limit_expr, offset_expr,
               search_expr):
        res = self.stub.Search(infinity_pb2.SelectStatement(session_id=self.session_id,
                                                            db_name=db_name,
                                                            table_name=table_name,
                                                            select_list=select_list,
                                                            where_expr=where_expr,
                                                            group_by_list=group_by_list,
                                                            limit_expr=limit_expr,
                                                            offset_expr=offset_expr,
                                                            search_expr=search_expr))

        if res.success:
            return res
        else:
            return None

    def disconnect(self):
        res = self.stub.DisConnect(infinity_pb2.DisConnectRequest(session_id=self.session_id))
        self.channel.close()
        return res
