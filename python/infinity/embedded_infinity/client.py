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

from infinity.errors import ErrorCode
from infinity.common import EMBEDDED_INFINITY_PATH
from embedded_infinity import *
class EmbeddedInfinityClient:
    def __init__(self, path: str = EMBEDDED_INFINITY_PATH):
        self.path = path
        Infinity.LocalInit(path)
        self.client = Infinity.LocalConnect()

    def disconnect(self):
        Infinity.LocalUnInit()

    def create_database(self, db_name: str, conflict_type: ConflictType = ConflictType.kError):
        create_database_options = CreateDatabaseOptions()
        create_database_options.conflict_type = conflict_type
        return self.client.CreateDatabase(db_name, create_database_options)

    def drop_database(self, db_name: str, conflict_type: ConflictType = ConflictType.kError):
        drop_database_options = DropDatabaseOptions()
        drop_database_options.conflict_type = conflict_type
        return self.client.DropDatabase(db_name, drop_database_options)

    def list_databases(self):
        return self.client.ListDatabase()

    def show_database(self, db_name: str):
        return self.client.ShowDatabase(db_name)

    def get_database(self, db_name: str):
        return self.client.GetDatabase(db_name)

    def create_table(self, db_name: str, table_name: str, column_defs,
                     constraints, create_table_options):
        return self.client.CreateTable(db_name,
                                       table_name,
                                       column_defs,
                                       constraints,
                                       create_table_options)

    def drop_table(self, db_name: str, table_name: str, conflict_type: ConflictType = ConflictType.kError):
        drop_table_options = DropTableOptions()
        drop_table_options.conflict_type = conflict_type
        return self.client.DropTable(db_name, table_name, drop_table_options)

    def list_tables(self, db_name: str):
        return self.client.ListTable(db_name)

    def show_table(self, db_name: str, table_name: str):
        return self.client.ShowTable(db_name, table_name)

    def show_index(self, db_name: str, table_name: str, index_name: str):
        return self.client.ShowIndex(db_name, table_name, index_name)

    def list_indexes(self, db_name: str, table_name: str):
        return self.client.ListIndex(db_name, table_name)

    # todo fields : list[ParsedExpr], ParsedExpr is abstract class, all sub class need to bind
    def insert(self, db_name: str, table_name: str, column_names: list[str], fields):
        retry = 0
        inner_ex = None
        while retry <= 2:
            try:
                res = self.client.Insert(db_name, table_name, column_names, fields)
                return res
            except Exception as ex:
                inner_ex = ex
                retry += 1
        return ErrorCode.TOO_MANY_CONNECTIONS, "insert failed with exception: " + str(inner_ex)

    def import_data(self, db_name: str, table_name: str, file_name: str, import_options):
        return self.client.Import(db_name, table_name, file_name, import_options)

    def select(self, db_name: str, table_name: str, select_list, search_expr,
               where_expr, group_by_list, limit_expr, offset_expr):
        return self.client.Select(db_name, table_name, select_list,
                                  search_expr, where_expr, group_by_list, limit_expr, offset_expr)

    def explain(self, db_name: str, table_name: str, select_list, search_expr,
                where_expr, group_by_list, limit_expr, offset_expr, explain_type):
        return self.client.Explain(db_name, table_name, select_list,
                                   search_expr, where_expr, group_by_list, limit_expr, offset_expr, explain_type)

    def delete(self, db_name: str, table_name: str, where_expr):
        return self.client.Delete(db_name, table_name, where_expr)

    def update(self, db_name: str, table_name: str, where_expr, update_expr_array):
        return self.client.Update(db_name, table_name, where_expr, update_expr_array)

    def show_tables(self, db_name: str):
        return self.client.ShowTables(db_name)

    def show_segments(self, db_name: str, table_name: str):
        return self.client.ShowSegments(db_name, table_name)

    def show_segment(self, db_name: str, table_name: str, segment_id: int):
        return self.client.ShowSegment(db_name, table_name, segment_id)

    def show_blocks(self, db_name: str, table_name: str, segment_id: int):
        return self.client.ShowBlocks(db_name, table_name, segment_id)

    def show_block(self, db_name: str, table_name: str, segment_id: int, block_id: int):
        return self.client.ShowBlock(db_name, table_name, segment_id, block_id)

    def show_block_column(self, db_name: str, table_name: str, segment_id: int, block_id: int, column_id: int):
        return self.client.ShowBlockColumn(db_name, table_name, segment_id, block_id, column_id)