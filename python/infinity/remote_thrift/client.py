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

from thrift.protocol import TBinaryProtocol
from thrift.protocol import TCompactProtocol
from thrift.transport import TSocket
from thrift.transport.TTransport import TTransportException

from infinity import URI
from infinity.remote_thrift.infinity_thrift_rpc import *
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *
from infinity.errors import ErrorCode
from infinity.common import InfinityException

class ThriftInfinityClient:
    def __init__(self, uri: URI):
        self.session_id = -1
        self.uri = uri
        self.transport = None
        self.reconnect()

    def reconnect(self):
        if self.transport is not None:
            self.transport.close()
            self.transport = None
        # self.transport = TTransport.TFramedTransport(TSocket.TSocket(self.uri.ip, self.uri.port))  # async
        self.transport = TTransport.TBufferedTransport(
            TSocket.TSocket(self.uri.ip, self.uri.port))  # sync
        self.protocol = TBinaryProtocol.TBinaryProtocol(self.transport)
        # self.protocol = TCompactProtocol.TCompactProtocol(self.transport)
        self.client = InfinityService.Client(self.protocol)
        self.transport.open()

        # version: 0.2.0.dev2, client_version: 1
        # version: 0.2.0.dev3, client_version: 2
        # version: 0.2.0.dev4, client_version: 3
        # version: 0.2.0.dev5, client_version: 4
        # version: 0.2.0.dev6, client_version: 5
        # version: 0.2.0.dev7, client_version: 6
        # version: 0.2.0.dev8, client_version: 7
        # version: 0.2.0, client_version: 8
        res = self.client.Connect(ConnectRequest(client_version=8))
        if res.error_code != 0:
            raise InfinityException(res.error_code, res.error_msg)
        self.session_id = res.session_id

    def create_database(self, db_name: str, conflict_type: CreateConflict = CreateConflict.Error):
        return self.client.CreateDatabase(CreateDatabaseRequest(session_id=self.session_id,
                                                                db_name=db_name,
                                                                create_option=CreateOption(conflict_type=conflict_type)))

    def drop_database(self, db_name: str, conflict_type: DropConflict = DropConflict.Error):
        return self.client.DropDatabase(DropDatabaseRequest(session_id=self.session_id,
                                                            db_name=db_name,
                                                            drop_option=DropOption(conflict_type=conflict_type)))

    def list_databases(self):
        return self.client.ListDatabase(ListDatabaseRequest(session_id=self.session_id))

    def show_database(self, db_name: str):
        return self.client.ShowDatabase(ShowDatabaseRequest(session_id=self.session_id,
                                                            db_name=db_name))

    def get_database(self, db_name: str):
        return self.client.GetDatabase(GetDatabaseRequest(session_id=self.session_id,
                                                          db_name=db_name))

    def create_table(self, db_name: str, table_name: str, column_defs,
                     conflict_type: CreateConflict = CreateConflict.Error, properties: list = None):
        return self.client.CreateTable(CreateTableRequest(session_id=self.session_id,
                                                          db_name=db_name,
                                                          table_name=table_name,
                                                          column_defs=column_defs,
                                                          create_option=CreateOption(conflict_type=conflict_type,
                                                                                     properties=properties)))

    def drop_table(self, db_name: str, table_name: str, conflict_type: DropConflict = DropConflict.Error):
        return self.client.DropTable(DropTableRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name,
                                                      drop_option=DropOption(conflict_type=conflict_type)))

    def list_tables(self, db_name: str):
        return self.client.ListTable(ListTableRequest(session_id=self.session_id,
                                                      db_name=db_name))

    def show_table(self, db_name: str, table_name: str):
        return self.client.ShowTable(ShowTableRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name))

    def show_columns(self, db_name: str, table_name: str):
        return self.client.ShowColumns(ShowColumnsRequest(session_id=self.session_id,
                                                          db_name=db_name,
                                                          table_name=table_name))

    def get_table(self, db_name: str, table_name: str):
        return self.client.GetTable(GetTableRequest(session_id=self.session_id,
                                                    db_name=db_name,
                                                    table_name=table_name))

    def create_index(self, db_name: str, table_name: str, index_name: str, index_info_list,
                     conflict_type: CreateConflict = CreateConflict.Error):
        return self.client.CreateIndex(CreateIndexRequest(session_id=self.session_id,
                                                          db_name=db_name,
                                                          table_name=table_name,
                                                          index_name=index_name,
                                                          index_info_list=index_info_list,
                                                          create_option=CreateOption(conflict_type=conflict_type)))

    def drop_index(self, db_name: str, table_name: str, index_name: str,
                   conflict_type: DropConflict = DropConflict.Error):
        return self.client.DropIndex(DropIndexRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name,
                                                      index_name=index_name,
                                                      drop_option=DropOption(conflict_type=conflict_type)))

    def show_index(self, db_name: str, table_name: str, index_name: str):
        return self.client.ShowIndex(ShowIndexRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name,
                                                      index_name=index_name))

    def list_indexes(self, db_name: str, table_name: str):
        return self.client.ListIndex(ListIndexRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name))

    def insert(self, db_name: str, table_name: str, column_names: list[str], fields: list[Field]):
        retry = 0
        inner_ex = None
        while retry <= 2:
            try:
                res = self.client.Insert(InsertRequest(session_id=self.session_id,
                                                       db_name=db_name,
                                                       table_name=table_name,
                                                       column_names=column_names,
                                                       fields=fields))
                return res
            except TTransportException as ex:
                #import traceback
                #traceback.print_exc()
                self.reconnect()
                inner_ex = ex
                retry += 1
            except Exception as ex:
                inner_ex = ex
        return CommonResponse(ErrorCode.TOO_MANY_CONNECTIONS, "insert failed with exception: " + str(inner_ex))

    # Can be used in compact mode
    # def insert(self, db_name: str, table_name: str, column_names: list[str], fields: list[Field]):
    #     return self.client.Insert(InsertRequest(session_id=self.session_id,
    #                                             db_name=db_name,
    #                                             table_name=table_name,
    #                                             column_names=column_names,
    #                                             fields=fields))

    def import_data(self, db_name: str, table_name: str, file_name: str, import_options):
        return self.client.Import(ImportRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                file_name=file_name,
                                                import_option=import_options))

    def export_data(self, db_name: str, table_name: str, file_name: str, export_options: {}, columns: [str]):
        return self.client.Export(ExportRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                columns=columns,
                                                file_name=file_name,
                                                export_option=export_options))

    def select(self, db_name: str, table_name: str, select_list, search_expr,
               where_expr, group_by_list, limit_expr, offset_expr):
        return self.client.Select(SelectRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                select_list=select_list,
                                                search_expr=search_expr,
                                                where_expr=where_expr,
                                                group_by_list=group_by_list,
                                                limit_expr=limit_expr,
                                                offset_expr=offset_expr,
                                                ))

    def explain(self, db_name: str, table_name: str, select_list, search_expr,
                where_expr, group_by_list, limit_expr, offset_expr, explain_type):
        return self.client.Explain(ExplainRequest(session_id=self.session_id,
                                                  db_name=db_name,
                                                  table_name=table_name,
                                                  select_list=select_list,
                                                  search_expr=search_expr,
                                                  where_expr=where_expr,
                                                  group_by_list=group_by_list,
                                                  limit_expr=limit_expr,
                                                  offset_expr=offset_expr,
                                                  explain_type=explain_type
                                                  ))

    def delete(self, db_name: str, table_name: str, where_expr):
        return self.client.Delete(DeleteRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                where_expr=where_expr))

    def update(self, db_name: str, table_name: str, where_expr, update_expr_array):
        return self.client.Update(UpdateRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                where_expr=where_expr,
                                                update_expr_array=update_expr_array))

    def disconnect(self):
        res = None
        try:
            res = self.client.Disconnect(CommonRequest(session_id=self.session_id))
        except Exception:
            pass
        self.transport.close()
        return res

    def show_tables(self, db_name: str):
        return self.client.ShowTables(ShowTablesRequest(session_id=self.session_id, db_name=db_name))

    def show_segments(self, db_name: str, table_name: str):
        return self.client.ShowSegments(
            ShowSegmentsRequest(session_id=self.session_id, db_name=db_name, table_name=table_name))

    def show_segment(self, db_name: str, table_name: str, segment_id: int):
        return self.client.ShowSegment(
            ShowSegmentRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                               segment_id=segment_id))

    def show_blocks(self, db_name: str, table_name: str, segment_id: int):
        return self.client.ShowBlocks(
            ShowBlocksRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                              segment_id=segment_id))

    def show_block(self, db_name: str, table_name: str, segment_id: int, block_id: int):
        return self.client.ShowBlock(
            ShowBlockRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                             segment_id=segment_id, block_id=block_id))

    def show_block_column(self, db_name: str, table_name: str, segment_id: int, block_id: int, column_id: int):
        return self.client.ShowBlockColumn(
            ShowBlockColumnRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                                   segment_id=segment_id, block_id=block_id, column_id=column_id))

    def optimize(self, db_name: str, table_name: str, optimize_opt: ttypes.OptimizeOptions):
        return self.client.Optimize(OptimizeRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                                                    optimize_options=optimize_opt))
