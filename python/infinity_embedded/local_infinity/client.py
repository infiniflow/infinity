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

from infinity_embedded.errors import ErrorCode as PyErrorCode
from infinity_embedded.common import LOCAL_INFINITY_PATH, LOCAL_INFINITY_CONFIG_PATH
from infinity_embedded.embedded_infinity_ext import *
from typing import List


class LocalQueryResult:
    def __init__(self, error_code: PyErrorCode, error_msg: str, db_names=None, table_names=None, index_names=None,
                 column_defs=None, column_fields=None, database_name=None, store_dir=None, table_count=None,
                 comment=None,
                 table_name=None, index_name=None, index_type=None, index_comment=None, deleted_rows=0, extra_result=None):
        self.error_code = error_code
        self.error_msg = error_msg
        self.db_names = db_names
        self.table_names = table_names
        self.index_names = index_names
        # self.result_data = result_data
        self.column_defs = column_defs
        self.column_fields = column_fields
        self.database_name = database_name
        self.store_dir = store_dir
        self.table_count = table_count
        self.comment = comment
        self.table_name = table_name
        self.index_name = index_name
        self.index_type = index_type
        self.index_comment = index_comment
        self.deleted_rows = deleted_rows
        self.extra_result = extra_result


class LocalInfinityClient:
    def __init__(self, path: str = LOCAL_INFINITY_PATH, config_path = LOCAL_INFINITY_CONFIG_PATH):
        self.path = path
        Infinity.LocalInit(path, config_path)
        self.client = Infinity.LocalConnect()

    def __del__(self):
        if self.client is not None:
            self.disconnect()

    def disconnect(self):
        Infinity.LocalUnInit()
        self.client = None
        return LocalQueryResult(PyErrorCode.OK, "")

    def hello(self):
        self.client.Hello()

    # convert embedded_error code to python error code
    def convert_res(self, res, has_db_names=False, has_table_names=False, has_result_data=False, has_db_name=False,
                    has_index_names=False, has_index_info=False, has_deleted_rows=False):
        if has_db_names:
            return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg, db_names=res.names)
        if has_table_names:
            return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg, table_names=res.names)
        if has_result_data:
            return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg, column_defs=res.column_defs,
                                    column_fields=res.column_fields, extra_result=res.extra_result)
        if has_db_name:
            return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg, database_name=res.database_name,
                                    store_dir=res.store_dir, table_count=res.table_count, comment=res.comment)
        if has_index_names:
            return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg, index_names=res.names)
        if has_index_info:
            return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg, database_name=res.database_name,
                                    table_name=res.table_name, index_name=res.index_name, index_comment=res.comment,
                                    index_type=res.index_type)
        if has_deleted_rows:
            return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg, deleted_rows=res.deleted_rows)

        return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg)

    def create_database(self, db_name: str, conflict_type: ConflictType = ConflictType.kError, comment: str = None):
        create_database_options = CreateDatabaseOptions()
        create_database_options.conflict_type = conflict_type
        db_comment: str = None
        if comment is None:
            db_comment = ""
        else:
            db_comment = comment
        return self.convert_res(self.client.CreateDatabase(db_name, create_database_options, db_comment))

    def drop_database(self, db_name: str, conflict_type: ConflictType = ConflictType.kError):
        drop_database_options = DropDatabaseOptions()
        drop_database_options.conflict_type = conflict_type
        return self.convert_res(self.client.DropDatabase(db_name, drop_database_options))

    def list_databases(self):
        return self.convert_res(self.client.ListDatabases(), has_db_names=True)

    def show_database(self, db_name: str):
        return self.convert_res(self.client.ShowDatabase(db_name), has_db_name=True)

    def show_info(self, info_name: str):
        if self.client is None:
            raise Exception("Local infinity is not connected")

        return self.convert_res(self.client.ShowInfo(info_name))

    def get_database(self, db_name: str):
        return self.convert_res(self.client.GetDatabase(db_name))

    def create_table(self, db_name: str, table_name: str, column_defs: list[WrapColumnDef],
                     conflict_type: ConflictType = ConflictType.kError, properties: list = None):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        create_table_options = WrapCreateTableOptions()
        create_table_options.conflict_type = conflict_type
        return self.convert_res(self.client.CreateTable(db_name,
                                                        table_name,
                                                        column_defs,
                                                        create_table_options))

    def drop_table(self, db_name: str, table_name: str, conflict_type: ConflictType = ConflictType.kError):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        drop_table_options = DropTableOptions()
        drop_table_options.conflict_type = conflict_type
        return self.convert_res(self.client.DropTable(db_name, table_name, drop_table_options))

    def get_table(self, db_name: str, table_name: str):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.GetTable(db_name, table_name))

    def list_tables(self, db_name: str):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ListTables(db_name), has_table_names=True)

    def show_table(self, db_name: str, table_name: str):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowTable(db_name, table_name))

    def create_index(self, db_name: str, table_name: str, index_name: str, index_info: WrapIndexInfo,
                     conflict_type: ConflictType = ConflictType.kError, index_comment: str = ""):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        create_index_options = CreateIndexOptions()
        create_index_options.conflict_type = conflict_type
        return self.convert_res(
            self.client.CreateIndex(db_name, table_name, index_name, index_info, create_index_options, index_comment))

    def show_index(self, db_name: str, table_name: str, index_name: str):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowIndex(db_name, table_name, index_name), has_index_info=True)

    def list_indexes(self, db_name: str, table_name: str):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ListTableIndexes(db_name, table_name), has_index_names=True)

    def drop_index(self, db_name: str, table_name: str, index_name: str,
                   conflict_type: ConflictType = ConflictType.kError):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        drop_index_options = DropIndexOptions()
        drop_index_options.conflict_type = conflict_type
        return self.convert_res(self.client.DropIndex(db_name, table_name, index_name, drop_index_options))

    def insert(self, db_name: str, table_name: str, fields):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        retry = 0
        inner_ex = None
        while retry <= 2:
            try:
                res = self.client.Insert(db_name, table_name, fields)
                return self.convert_res(res)
            except Exception as ex:
                inner_ex = ex
                retry += 1
        return PyErrorCode.TOO_MANY_CONNECTIONS, "insert failed with exception: " + str(inner_ex)

    def import_data(self, db_name: str, table_name: str, file_name: str, import_options):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.Import(db_name, table_name, file_name, import_options))

    def export_data(self, db_name: str, table_name: str, file_name: str, export_options, columns: list[str]):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.Export(db_name, table_name, columns, file_name, export_options))

    def search(self,
               db_name: str,
               table_name: str,
               select_list: list[WrapParsedExpr],
               highlight_list: list[WrapParsedExpr] = [],
               order_by_list: list[WrapOrderByExpr] = [],
               group_by_list: list[WrapParsedExpr] = [],
               total_hits_count_flag: bool = False,
               search_expr: WrapSearchExpr = None,
               where_expr: WrapParsedExpr = None,
               limit_expr: WrapParsedExpr = None,
               offset_expr: WrapParsedExpr = None):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.Search(db_name,
                                                   table_name,
                                                   select_list,
                                                   highlight_list,
                                                   order_by_list,
                                                   group_by_list,
                                                   total_hits_count_flag,
                                                   search_expr,
                                                   where_expr,
                                                   limit_expr,
                                                   offset_expr),
                                has_result_data=True)

    def explain(self,
                db_name: str,
                table_name: str,
                explain_type,
                select_list: list[WrapParsedExpr],
                highlight_list: list[WrapParsedExpr] = [],
                order_by_list: list[WrapOrderByExpr] = [],
                group_by_list: list[WrapParsedExpr] = [],
                search_expr: WrapSearchExpr = None,
                where_expr: WrapParsedExpr = None,
                limit_expr: WrapParsedExpr = None,
                offset_expr: WrapParsedExpr = None):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.Explain(db_name,
                                                    table_name,
                                                    explain_type,
                                                    select_list,
                                                    highlight_list,
                                                    order_by_list,
                                                    group_by_list,
                                                    search_expr,
                                                    where_expr,
                                                    limit_expr,
                                                    offset_expr),
                                has_result_data=True)

    def delete(self, db_name: str, table_name: str, where_expr):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.Delete(db_name, table_name, where_expr), has_deleted_rows=True)

    def update(self, db_name: str, table_name: str, where_expr, update_expr_array):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.Update(db_name, table_name, where_expr, update_expr_array))

    def show_tables(self, db_name: str):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowTables(db_name), has_result_data=True)

    def show_columns(self, db_name: str, table_name: str):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowColumns(db_name, table_name), has_result_data=True)

    def show_segments(self, db_name: str, table_name: str):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowSegments(db_name, table_name))

    def show_segment(self, db_name: str, table_name: str, segment_id: int):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowSegment(db_name, table_name, segment_id))

    def show_blocks(self, db_name: str, table_name: str, segment_id: int):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowBlocks(db_name, table_name, segment_id))

    def show_block(self, db_name: str, table_name: str, segment_id: int, block_id: int):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowBlock(db_name, table_name, segment_id, block_id))

    def show_block_column(self, db_name: str, table_name: str, segment_id: int, block_id: int, column_id: int):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowBlockColumn(db_name, table_name, segment_id, block_id, column_id))

    def show_current_node(self):
        if self.client is None:
            raise Exception("Local infinity is not connected")
        return self.convert_res(self.client.ShowCurrentNode())

    def optimize(self, db_name: str, table_name: str, optimize_opt: WrapOptimizeOptions):
        return self.convert_res(self.client.Optimize(db_name, table_name, optimize_opt))

    def add_columns(self, db_name: str, table_name: str, column_defs: list[WrapColumnDef]):
        return self.convert_res(self.client.AddColumns(db_name, table_name, column_defs))

    def drop_columns(self, db_name: str, table_name: str, column_names: list[str]):
        return self.convert_res(self.client.DropColumns(db_name, table_name, column_names))
