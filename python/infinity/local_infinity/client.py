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

from infinity.errors import ErrorCode as PyErrorCode
from infinity.common import LOCAL_INFINITY_PATH
from embedded_infinity import *

class LocalQueryResult:
    def __init__(self, error_code: PyErrorCode, error_msg: str, db_names=None, table_names=None):
        self.error_code = error_code
        self.error_msg = error_msg
        self.db_names = db_names
        self.table_names = table_names

class LocalInfinityClient:
    def __init__(self, path: str = LOCAL_INFINITY_PATH):
        self.path = path
        Infinity.LocalInit(path)
        self.client = Infinity.LocalConnect()

    def disconnect(self):
        Infinity.LocalUnInit()
        return LocalQueryResult(PyErrorCode.OK, "")

    # convert embedded_error code to python error code
    def convert_res(self, res, has_db_name=False, has_table_name=False):
        if has_db_name:
            return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg, db_names=res.names)
        if has_table_name:
            return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg, table_names=res.names)
        return LocalQueryResult(PyErrorCode(res.error_code.value), res.error_msg)

    def create_database(self, db_name: str, conflict_type: ConflictType = ConflictType.kError):
        create_database_options = CreateDatabaseOptions()
        create_database_options.conflict_type = conflict_type
        return self.convert_res(self.client.CreateDatabase(db_name, create_database_options))

    def drop_database(self, db_name: str, conflict_type: ConflictType = ConflictType.kError):
        drop_database_options = DropDatabaseOptions()
        drop_database_options.conflict_type = conflict_type
        return self.convert_res(self.client.DropDatabase(db_name, drop_database_options))

    def list_databases(self):
        return self.convert_res(self.client.ListDatabases(), has_db_name=True)

    def show_database(self, db_name: str):
        return self.convert_res(self.client.ShowDatabase(db_name))

    def get_database(self, db_name: str):
        return self.convert_res(self.client.GetDatabase(db_name))

    def create_table(self, db_name: str, table_name: str, column_defs: list[WrapColumnDef],
                     conflict_type: ConflictType = ConflictType.kError, properties: list = None):
        create_table_options = CreateTableOptions()
        create_table_options.conflict_type = conflict_type
        print("py client create_table column_defs: ", column_defs)
        return self.convert_res(self.client.CreateTable(db_name,
                                                        table_name,
                                                        column_defs,
                                                        create_table_options))

    def drop_table(self, db_name: str, table_name: str, conflict_type: ConflictType = ConflictType.kError):
        drop_table_options = DropTableOptions()
        drop_table_options.conflict_type = conflict_type
        return self.convert_res(self.client.DropTable(db_name, table_name, drop_table_options))

    def list_tables(self, db_name: str):
        return self.convert_res(self.client.ListTables(db_name), has_table_name=True)

    def show_table(self, db_name: str, table_name: str):
        return self.convert_res(self.client.ShowTable(db_name, table_name))

    def show_index(self, db_name: str, table_name: str, index_name: str):
        return self.convert_res(self.client.ShowIndex(db_name, table_name, index_name))

    def list_indexes(self, db_name: str, table_name: str):
        return self.convert_res(self.client.ListIndex(db_name, table_name))

    # todo fields : list[ParsedExpr], ParsedExpr is abstract class, all sub class need to bind
    def insert(self, db_name: str, table_name: str, column_names: list[str], fields):
        retry = 0
        inner_ex = None
        while retry <= 2:
            try:
                res = self.client.Insert(db_name, table_name, column_names, fields)
                return self.convert_res(res)
            except Exception as ex:
                inner_ex = ex
                retry += 1
        return PyErrorCode.TOO_MANY_CONNECTIONS, "insert failed with exception: " + str(inner_ex)

    def import_data(self, db_name: str, table_name: str, file_name: str, import_options):
        return self.convert_res(self.client.Import(db_name, table_name, file_name, import_options))

    def select(self, db_name: str, table_name: str, select_list, search_expr,
               where_expr, group_by_list, limit_expr, offset_expr):
        return self.convert_res(self.client.Select(db_name, table_name, select_list,
                                  search_expr, where_expr, group_by_list, limit_expr, offset_expr))

    def explain(self, db_name: str, table_name: str, select_list, search_expr,
                where_expr, group_by_list, limit_expr, offset_expr, explain_type):
        return self.convert_res(self.client.Explain(db_name, table_name, select_list,
                                   search_expr, where_expr, group_by_list, limit_expr, offset_expr, explain_type))

    def delete(self, db_name: str, table_name: str, where_expr):
        return self.convert_res(self.client.Delete(db_name, table_name, where_expr))

    def update(self, db_name: str, table_name: str, where_expr, update_expr_array):
        return self.convert_res(self.client.Update(db_name, table_name, where_expr, update_expr_array))

    def show_tables(self, db_name: str):
        return self.convert_res(self.client.ShowTables(db_name))

    def show_segments(self, db_name: str, table_name: str):
        return self.convert_res(self.client.ShowSegments(db_name, table_name))

    def show_segment(self, db_name: str, table_name: str, segment_id: int):
        return self.convert_res(self.client.ShowSegment(db_name, table_name, segment_id))

    def show_blocks(self, db_name: str, table_name: str, segment_id: int):
        return self.convert_res(self.client.ShowBlocks(db_name, table_name, segment_id))

    def show_block(self, db_name: str, table_name: str, segment_id: int, block_id: int):
        return self.convert_res(self.client.ShowBlock(db_name, table_name, segment_id, block_id))

    def show_block_column(self, db_name: str, table_name: str, segment_id: int, block_id: int, column_id: int):
        return self.convert_res(self.client.ShowBlockColumn(db_name, table_name, segment_id, block_id, column_id))