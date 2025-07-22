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

import logging
from functools import wraps
from readerwriterlock import rwlock

from thrift.protocol import TBinaryProtocol
from thrift.transport import TSocket
from thrift.transport.TTransport import TTransportException

from infinity import URI
from infinity.remote_thrift.infinity_thrift_rpc import *
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *
from infinity.errors import ErrorCode
from infinity.common import InfinityException

TRY_TIMES = 10


class ThriftInfinityClient:
    def __init__(self, uri: URI, *, try_times: int = TRY_TIMES, logger: logging.Logger = None):
        self.lock = rwlock.RWLockRead()

        self.session_id = -1
        self.uri = uri
        self.transport = None
        self._reconnect()
        self._is_connected = True

        self.session_i = 0
        self.try_times = try_times
        self._init_logger(logger)

    def _init_logger(self, logger):
        if logger is not None:
            self.logger = logger
            return
        self.logger = logging.getLogger("ThriftInfinityClient")
        self.logger.setLevel(logging.DEBUG)
        if not self.logger.handlers:
            formatter = logging.Formatter('%(asctime)s - %(threadName)s - %(levelname)s - %(message)s')
            ch = logging.StreamHandler()
            ch.setFormatter(formatter)
            self.logger.addHandler(ch)

    def __del__(self):
        if self._is_connected:
            self.disconnect()

    def _reconnect(self):
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
        # version: 0.2.1.dev5, client_version: 9
        # version: 0.2.1, client_version: 10
        # version: 0.3.0.dev1, client_version: 11
        # version: 0.3.0.dev3, client_version: 12
        # version: 0.3.0.dev4, client_version: 13
        # version: 0.3.0.dev5, client_version: 14
        # version: 0.3.0.dev6, client_version: 15
        # version: 0.3.0.dev7, client_version: 16
        # version: 0.3.0.dev8 and 0.3.0, client_version: 17
        # version: 0.4.0.dev1, client_version: 18
        # version: 0.4.0.dev2, client_version: 19
        # version: 0.4.0.dev3, client_version: 20
        # version: 0.4.0.dev4, client_version: 21
        # version: 0.4.0.dev5 and 0.4.0, client_version: 22
        # version: 0.5.0.dev1, client_version: 23
        # version: 0.5.0.dev2, client_version: 24
        # version: 0.5.0.dev3, client_version: 25
        # version: 0.5.0.dev4 and 0.5.0.dev5, client_version: 26
        # version: 0.5.0.dev6 and 0.5.0.dev7 and 0.5.0 and 0.5.1 and 0.5.2, client_version: 27
        # version: 0.6.0.dev1 and 0.6.0.dev2, client_version: 28
        # version: 0.6.0.dev3, client_version: 29
        # version: 0.6.0.dev4, client_version: 30
        res = self.client.Connect(ConnectRequest(client_version=30)) # 0.6.0.dev4
        if res.error_code != 0:
            raise InfinityException(res.error_code, res.error_msg)
        self.session_id = res.session_id

    def retry_wrapper(func):
        @wraps(func)
        def wrapper(self, *args, **kwargs):
            for i in range(self.try_times):
                try:
                    with self.lock.gen_rlock():
                        old_session_i = self.session_i
                        ret = func(self, *args, **kwargs)
                        break
                except TTransportException as e:
                    with self.lock.gen_wlock():
                        if old_session_i == self.session_i:
                            self._reconnect()
                            self.session_i += 1
                            self.logger.debug(
                                f"Tried {i} times, session_id: {self.session_id}, session_i: {self.session_i}, exception: {str(e)}")
                except Exception as e:
                    raise
            else:
                return CommonResponse(ErrorCode.TOO_MANY_CONNECTIONS, f"Try {self.try_times} times, but still failed")
            return ret

        return wrapper

    @retry_wrapper
    def create_database(self, db_name: str, conflict_type: CreateConflict = CreateConflict.Error, comment: str = None):
        db_comment: str
        if comment is None:
            db_comment = ""
        else:
            db_comment = comment
        return self.client.CreateDatabase(CreateDatabaseRequest(session_id=self.session_id,
                                                                db_name=db_name,
                                                                db_comment=db_comment,
                                                                create_option=CreateOption(
                                                                    conflict_type=conflict_type)))

    @retry_wrapper
    def drop_database(self, db_name: str, conflict_type: DropConflict = DropConflict.Error):
        return self.client.DropDatabase(DropDatabaseRequest(session_id=self.session_id,
                                                            db_name=db_name,
                                                            drop_option=DropOption(conflict_type=conflict_type)))

    @retry_wrapper
    def list_databases(self):
        return self.client.ListDatabase(ListDatabaseRequest(session_id=self.session_id))

    @retry_wrapper
    def show_database(self, db_name: str):
        return self.client.ShowDatabase(ShowDatabaseRequest(session_id=self.session_id,
                                                            db_name=db_name))

    @retry_wrapper
    def get_database(self, db_name: str):
        return self.client.GetDatabase(GetDatabaseRequest(session_id=self.session_id,
                                                          db_name=db_name))

    @retry_wrapper
    def create_table(self, db_name: str, table_name: str, column_defs,
                     conflict_type: CreateConflict = CreateConflict.Error, properties: list = None):
        return self.client.CreateTable(CreateTableRequest(session_id=self.session_id,
                                                          db_name=db_name,
                                                          table_name=table_name,
                                                          column_defs=column_defs,
                                                          create_option=CreateOption(conflict_type=conflict_type,
                                                                                     properties=properties)))

    @retry_wrapper
    def drop_table(self, db_name: str, table_name: str, conflict_type: DropConflict = DropConflict.Error):
        return self.client.DropTable(DropTableRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name,
                                                      drop_option=DropOption(conflict_type=conflict_type)))

    @retry_wrapper
    def list_tables(self, db_name: str):
        return self.client.ListTable(ListTableRequest(session_id=self.session_id,
                                                      db_name=db_name))

    @retry_wrapper
    def show_table(self, db_name: str, table_name: str):
        return self.client.ShowTable(ShowTableRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name))

    @retry_wrapper
    def show_columns(self, db_name: str, table_name: str):
        return self.client.ShowColumns(ShowColumnsRequest(session_id=self.session_id,
                                                          db_name=db_name,
                                                          table_name=table_name))

    @retry_wrapper
    def get_table(self, db_name: str, table_name: str):
        return self.client.GetTable(GetTableRequest(session_id=self.session_id,
                                                    db_name=db_name,
                                                    table_name=table_name))

    @retry_wrapper
    def create_index(self, db_name: str, table_name: str, index_name: str, index_info: IndexInfo,
                     conflict_type: CreateConflict = CreateConflict.Error, index_comment: str = ""):
        return self.client.CreateIndex(CreateIndexRequest(session_id=self.session_id,
                                                          db_name=db_name,
                                                          table_name=table_name,
                                                          index_name=index_name,
                                                          index_comment=index_comment,
                                                          index_info=index_info,
                                                          create_option=CreateOption(conflict_type=conflict_type)))

    @retry_wrapper
    def drop_index(self, db_name: str, table_name: str, index_name: str,
                   conflict_type: DropConflict = DropConflict.Error):
        return self.client.DropIndex(DropIndexRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name,
                                                      index_name=index_name,
                                                      drop_option=DropOption(conflict_type=conflict_type)))

    @retry_wrapper
    def show_index(self, db_name: str, table_name: str, index_name: str):
        return self.client.ShowIndex(ShowIndexRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name,
                                                      index_name=index_name))

    @retry_wrapper
    def list_indexes(self, db_name: str, table_name: str):
        return self.client.ListIndex(ListIndexRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name))

    @retry_wrapper
    def insert(self, db_name: str, table_name: str, fields: list[Field]):
        return self.client.Insert(
            InsertRequest(
                session_id=self.session_id,
                db_name=db_name,
                table_name=table_name,
                fields=fields,
            )
        )

    @retry_wrapper
    def import_data(self, db_name: str, table_name: str, file_name: str, import_options):
        return self.client.Import(ImportRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                file_name=file_name,
                                                import_option=import_options))

    @retry_wrapper
    def export_data(self, db_name: str, table_name: str, file_name: str, export_options: dict, columns: list[str]):
        return self.client.Export(ExportRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                columns=columns,
                                                file_name=file_name,
                                                export_option=export_options))

    @retry_wrapper
    def select(self, db_name: str, table_name: str, select_list, highlight_list, search_expr,
               where_expr, group_by_list, having_expr, limit_expr, offset_expr, order_by_list, total_hits_count):
        return self.client.Select(SelectRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                select_list=select_list,
                                                highlight_list=highlight_list,
                                                search_expr=search_expr,
                                                where_expr=where_expr,
                                                group_by_list=group_by_list,
                                                having_expr=having_expr,
                                                limit_expr=limit_expr,
                                                offset_expr=offset_expr,
                                                order_by_list=order_by_list,
                                                total_hits_count=total_hits_count
                                                ))

    @retry_wrapper
    def explain(self, db_name: str, table_name: str, select_list, highlight_list, search_expr,
                where_expr, group_by_list, limit_expr, offset_expr, explain_type):
        return self.client.Explain(ExplainRequest(session_id=self.session_id,
                                                  db_name=db_name,
                                                  table_name=table_name,
                                                  select_list=select_list,
                                                  highlight_list=highlight_list,
                                                  search_expr=search_expr,
                                                  where_expr=where_expr,
                                                  group_by_list=group_by_list,
                                                  limit_expr=limit_expr,
                                                  offset_expr=offset_expr,
                                                  explain_type=explain_type
                                                  ))

    @retry_wrapper
    def delete(self, db_name: str, table_name: str, where_expr):
        return self.client.Delete(DeleteRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                where_expr=where_expr))

    @retry_wrapper
    def update(self, db_name: str, table_name: str, where_expr, update_expr_array):
        return self.client.Update(UpdateRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                where_expr=where_expr,
                                                update_expr_array=update_expr_array))

    @retry_wrapper
    def disconnect(self):
        res = None
        if not self._is_connected:
            return CommonResponse(ErrorCode.OK, "Already disconnected")
        try:
            res = self.client.Disconnect(CommonRequest(session_id=self.session_id))
        except Exception as e:
            res = CommonResponse(ErrorCode.CLIENT_CLOSE, str(e))
        self.transport.close()
        self._is_connected = False
        return res

    @retry_wrapper
    def show_tables(self, db_name: str):
        return self.client.ShowTables(ShowTablesRequest(session_id=self.session_id, db_name=db_name))

    @retry_wrapper
    def show_segments(self, db_name: str, table_name: str):
        return self.client.ShowSegments(
            ShowSegmentsRequest(session_id=self.session_id, db_name=db_name, table_name=table_name))

    @retry_wrapper
    def show_segment(self, db_name: str, table_name: str, segment_id: int):
        return self.client.ShowSegment(
            ShowSegmentRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                               segment_id=segment_id))

    @retry_wrapper
    def show_blocks(self, db_name: str, table_name: str, segment_id: int):
        return self.client.ShowBlocks(
            ShowBlocksRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                              segment_id=segment_id))

    @retry_wrapper
    def show_block(self, db_name: str, table_name: str, segment_id: int, block_id: int):
        return self.client.ShowBlock(
            ShowBlockRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                             segment_id=segment_id, block_id=block_id))

    @retry_wrapper
    def show_block_column(self, db_name: str, table_name: str, segment_id: int, block_id: int, column_id: int):
        return self.client.ShowBlockColumn(
            ShowBlockColumnRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                                   segment_id=segment_id, block_id=block_id, column_id=column_id))

    @retry_wrapper
    def show_current_node(self):
        return self.client.ShowCurrentNode(ShowCurrentNodeRequest(session_id=self.session_id))

    @retry_wrapper
    def optimize(self, db_name: str, table_name: str, optimize_opt: ttypes.OptimizeOptions):
        return self.client.Optimize(OptimizeRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                                                    optimize_options=optimize_opt))

    @retry_wrapper
    def add_columns(self, db_name: str, table_name: str, column_defs: list):
        return self.client.AddColumns(
            AddColumnsRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                              column_defs=column_defs))

    @retry_wrapper
    def drop_columns(self, db_name: str, table_name: str, column_names: list):
        return self.client.DropColumns(
            DropColumnsRequest(session_id=self.session_id, db_name=db_name, table_name=table_name,
                               column_names=column_names))

    @retry_wrapper
    def cleanup(self):
        return self.client.Cleanup(CommonRequest(session_id=self.session_id))
    
    @retry_wrapper
    def dump_index(self, db_name: str, table_name: str, index_name: str):
        return self.client.DumpIndex(DumpIndexRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name,
                                                      index_name=index_name))

    @retry_wrapper
    def command(self, command: ttypes.CommandRequest):
        command.session_id = self.session_id
        return self.client.Command(command)

    @retry_wrapper
    def flush(self, flush_request: ttypes.FlushRequest):
        flush_request.session_id = self.session_id
        return self.client.Flush(flush_request)

    @retry_wrapper
    def compact(self, db_name: str, table_name: str):
        return self.client.Compact(CompactRequest(session_id=self.session_id, db_name=db_name, table_name=table_name))
    
    @retry_wrapper
    def create_table_snapshot(self, db_name: str, table_name: str, snapshot_name: str):
        return self.client.CreateTableSnapshot(CreateTableSnapshotRequest(session_id=self.session_id,
                                                                             db_name=db_name,
                                                                             table_name=table_name,
                                                                             snapshot_name=snapshot_name))

    @retry_wrapper
    def create_database_snapshot(self, db_name: str, snapshot_name: str):
        return self.client.CreateDatabaseSnapshot(CreateDatabaseSnapshotRequest(session_id=self.session_id,
                                                                             db_name=db_name,
                                                                             snapshot_name=snapshot_name))

    @retry_wrapper
    def create_system_snapshot(self, snapshot_name: str):
        return self.client.CreateSystemSnapshot(CreateSystemSnapshotRequest(session_id=self.session_id,
                                                                             snapshot_name=snapshot_name))
    
    @retry_wrapper
    def restore_snapshot(self, snapshot_name: str, scope: str):
        return self.client.RestoreSnapshot(RestoreSnapshotRequest(session_id=self.session_id,
                                                                             snapshot_name=snapshot_name,
                                                                             scope=scope))
    
    @retry_wrapper
    def list_snapshots(self):
        return self.client.ListSnapshots(ListSnapshotsRequest(session_id=self.session_id))
    
    @retry_wrapper
    def show_snapshot(self, snapshot_name: str):
        return self.client.ShowSnapshot(ShowSnapshotRequest(session_id=self.session_id,
                                                                             snapshot_name=snapshot_name))
    
    @retry_wrapper
    def drop_snapshot(self, snapshot_name: str):
        return self.client.DropSnapshot(DropSnapshotRequest(session_id=self.session_id,
                                                                             snapshot_name=snapshot_name))