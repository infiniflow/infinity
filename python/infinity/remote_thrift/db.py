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

from abc import ABC

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
import numpy as np
from infinity.db import Database
from infinity.errors import ErrorCode
from infinity.remote_thrift.table import RemoteTable
from infinity.remote_thrift.utils import check_valid_name, name_validity_check, select_res_to_polars
from infinity.common import ConflictType
from infinity.common import InfinityException

def get_constant_expr(column_info):
    # process constant expression
    default = None
    if "default" in column_info:
        default = column_info["default"]

    if default is None:
        constant_exp = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Null)
        return constant_exp
    else:
        constant_expression = None
        if isinstance(default, str):
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.String,
                                                      str_value=default)

        elif isinstance(default, int):
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Int64,
                                                      i64_value=default)

        elif isinstance(default, float) or isinstance(default, np.float32):
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Double,
                                                      f64_value=default)
        elif isinstance(default, list):
            if isinstance(default[0], int):
                constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.IntegerArray,
                                                          i64_array_value=default)
            elif isinstance(default[0], float):
                constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.DoubleArray,
                                                          f64_array_value=default)
        else:
            raise InfinityException(ErrorCode.INVALID_EXPRESSION, "Invalid constant expression")
        return constant_expression

def get_ordinary_info(column_info, column_defs, column_name, index):
    # "c1": {"type": "int", "constraints":["primary key", ...], "default": 1/"asdf"/[1,2]/...}
    # process column definition

    proto_column_def = ttypes.ColumnDef(None, None, None, [], None)
    proto_column_def.id = index
    proto_column_def.name = column_name

    proto_column_type = ttypes.DataType()
    datatype = column_info["type"]
    if datatype == "int8":
        proto_column_type.logic_type = ttypes.LogicType.TinyInt
    elif datatype == "int16":
        proto_column_type.logic_type = ttypes.LogicType.SmallInt
    elif datatype == "int32" or datatype == "int" or datatype == "integer":
        proto_column_type.logic_type = ttypes.LogicType.Integer
    elif datatype == "int64":
        proto_column_type.logic_type = ttypes.LogicType.BigInt
    elif datatype == "int128":
        proto_column_type.logic_type = ttypes.LogicType.HugeInt
    elif datatype == "float" or datatype == "float32":
        proto_column_type.logic_type = ttypes.LogicType.Float
    elif datatype == "double" or datatype == "float64":
        proto_column_type.logic_type = ttypes.LogicType.Double
    elif datatype == "varchar":
        proto_column_type.logic_type = ttypes.LogicType.Varchar
        proto_column_type.physical_type = ttypes.VarcharType()
    elif datatype == "bool":
        proto_column_type.logic_type = ttypes.LogicType.Boolean
    else:
        raise InfinityException(ErrorCode.INVALID_DATA_TYPE, f"Unknown datatype: {datatype}")

    # process constraints
    proto_column_def.data_type = proto_column_type
    if "constraints" in column_info:
        constraints = column_info["constraints"]
        for constraint in constraints:
            if constraint == "null":
                proto_column_def.constraints.append(ttypes.Constraint.Null)
            elif constraint == "not null":
                proto_column_def.constraints.append(ttypes.Constraint.NotNull)
            elif constraint == "primary key":
                proto_column_def.constraints.append(ttypes.Constraint.PrimaryKey)
            elif constraint == "unique":
                proto_column_def.constraints.append(ttypes.Constraint.Unique)
            else:
                raise InfinityException(ErrorCode.INVALID_CONSTRAINT_TYPE, f"Unknown constraint: {constraint}")

    proto_column_def.constant_expr = get_constant_expr(column_info)
    column_defs.append(proto_column_def)


def get_embedding_info(column_info, column_defs, column_name, index):
    # "vector,1024,float32"
    column_big_info = [item.strip() for item in column_info["type"].split(",")]
    length = column_big_info[1]
    element_type = column_big_info[2]

    proto_column_def = ttypes.ColumnDef()
    proto_column_def.id = index
    proto_column_def.name = column_name
    column_type = ttypes.DataType()
    if column_big_info[0] == "vector":
        column_type.logic_type = ttypes.LogicType.Embedding
    elif column_big_info[0] == "tensor":
        column_type.logic_type = ttypes.LogicType.Tensor
    elif column_big_info[0] == "tensorarray":
        column_type.logic_type = ttypes.LogicType.TensorArray
    else:
        raise InfinityException(ErrorCode.INVALID_DATA_TYPE, f"Unknown data type: {column_big_info[0]}")
    embedding_type = ttypes.EmbeddingType()
    if element_type == "bit":
        embedding_type.element_type = ttypes.ElementType.ElementBit
    elif element_type == "float32" or element_type == "float":
        embedding_type.element_type = ttypes.ElementType.ElementFloat32
    elif element_type == "float64" or element_type == "double":
        embedding_type.element_type = ttypes.ElementType.ElementFloat64
    elif element_type == "int8":
        embedding_type.element_type = ttypes.ElementType.ElementInt8
    elif element_type == "int16":
        embedding_type.element_type = ttypes.ElementType.ElementInt16
    elif element_type == "int32" or element_type == "int":
        embedding_type.element_type = ttypes.ElementType.ElementInt32
    elif element_type == "int64":
        embedding_type.element_type = ttypes.ElementType.ElementInt64
    else:
        raise InfinityException(3057, f"Unknown element type: {element_type}")
    embedding_type.dimension = int(length)
    assert isinstance(embedding_type, ttypes.EmbeddingType)
    assert embedding_type.element_type is not None
    assert embedding_type.dimension is not None
    physical_type = ttypes.PhysicalType()
    physical_type.embedding_type = embedding_type
    column_type.physical_type = physical_type
    proto_column_def.data_type = column_type

    proto_column_def.constant_expr = get_constant_expr(column_info)

    column_defs.append(proto_column_def)

def get_sparse_info(column_info, column_defs, column_name, index):
    # "sparse,30000,float32,int16"
    column_big_info = [item.strip() for item in column_info["type"].split(",")]
    length = column_big_info[1]
    value_type = column_big_info[2]
    index_type = column_big_info[3]

    proto_column_def = ttypes.ColumnDef()
    proto_column_def.id = index
    proto_column_def.name = column_name
    column_type = ttypes.DataType()
    if column_big_info[0] == "sparse":
        column_type.logic_type = ttypes.LogicType.Sparse
    else:
        raise InfinityException(ErrorCode.INVALID_DATA_TYPE, f"Unknown data type: {column_big_info[0]}")

    sparse_type = ttypes.SparseType()
    if value_type == "bit":
        sparse_type.element_type = ttypes.ElementType.ElementBit
    elif value_type == "float32" or value_type == "float":
        sparse_type.element_type = ttypes.ElementType.ElementFloat32
    elif value_type == "float64" or value_type == "double":
        sparse_type.element_type = ttypes.ElementType.ElementFloat64
    elif value_type == "int8":
        sparse_type.element_type = ttypes.ElementType.ElementInt8
    elif value_type == "int16":
        sparse_type.element_type = ttypes.ElementType.ElementInt16
    elif value_type == "int32" or value_type == "int":
        sparse_type.element_type = ttypes.ElementType.ElementInt32
    elif value_type == "int64":
        sparse_type.element_type = ttypes.ElementType.ElementInt64
    else:
        raise InfinityException(3057, f"Unknown value type: {value_type}")
    
    if index_type == "int8":
        sparse_type.index_type = ttypes.ElementType.ElementInt8
    elif index_type == "int16":
        sparse_type.index_type = ttypes.ElementType.ElementInt16
    elif index_type == "int32" or index_type == "int":
        sparse_type.index_type = ttypes.ElementType.ElementInt32
    elif index_type == "int64":
        sparse_type.index_type = ttypes.ElementType.ElementInt64
    else:
        raise InfinityException(3057, f"Unknown index type: {index_type}")

    sparse_type.dimension = int(length)
    
    physical_type = ttypes.PhysicalType()
    physical_type.sparse_type = sparse_type
    column_type.physical_type = physical_type
    proto_column_def.data_type = column_type

    proto_column_def.constant_expr = get_constant_expr(column_info)

    column_defs.append(proto_column_def)

class RemoteDatabase(Database, ABC):
    def __init__(self, conn, name: str):
        self._conn = conn
        self._db_name = name

    @name_validity_check("table_name", "Table")
    def create_table(self, table_name: str, columns_definition,
                     conflict_type: ConflictType = ConflictType.Error):
        # process column definitions
        """
        db_obj.create_table("my_table",
            {
                "c1": {
                    "type": "int",
                    "constraints":["primary key", ...],
                    "default"(optional): 1/"asdf"/[1,2]/...
                },
                "c2": {
                    "type":"vector,1024,float32",
                }
            }, None)
        """
        # to column_defs
        column_defs = []
        for index, (column_name, column_info) in enumerate(columns_definition.items()):
            check_valid_name(column_name, "Column")
            column_big_info = [item.strip() for item in column_info["type"].split(",")]
            if column_big_info[0] == "vector" or column_big_info[0] == "tensor" or column_big_info[0] == "tensorarray":
                get_embedding_info(column_info, column_defs, column_name, index)
            elif column_big_info[0] == "sparse":
                get_sparse_info(column_info, column_defs, column_name, index)
            else:  # numeric or varchar
                get_ordinary_info(column_info, column_defs, column_name, index)

        create_table_conflict: ttypes.CreateConflict
        if conflict_type == ConflictType.Error:
            create_table_conflict = ttypes.CreateConflict.Error
        elif conflict_type == ConflictType.Ignore:
            create_table_conflict = ttypes.CreateConflict.Ignore
        elif conflict_type == ConflictType.Replace:
            create_table_conflict = ttypes.CreateConflict.Replace
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, "Invalid conflict type")

        res = self._conn.create_table(db_name=self._db_name, table_name=table_name,
                                      column_defs=column_defs,
                                      conflict_type=create_table_conflict)

        if res.error_code == ErrorCode.OK:
            return RemoteTable(self._conn, self._db_name, table_name)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("table_name", "Table")
    def drop_table(self, table_name, conflict_type: ConflictType = ConflictType.Error):
        if conflict_type == ConflictType.Error:
            return self._conn.drop_table(db_name=self._db_name, table_name=table_name,
                                         conflict_type=ttypes.DropConflict.Error)
        elif conflict_type == ConflictType.Ignore:
            return self._conn.drop_table(db_name=self._db_name, table_name=table_name,
                                         conflict_type=ttypes.DropConflict.Ignore)
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, "nvalid conflict type")

    def list_tables(self):
        res = self._conn.list_tables(self._db_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("table_name", "Table")
    def show_table(self, table_name):
        res = self._conn.show_table(
            db_name=self._db_name, table_name=table_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("table_name", "Table")
    def show_columns(self, table_name):
        res = self._conn.show_columns(
            db_name=self._db_name, table_name=table_name)
        if res.error_code == ErrorCode.OK:
            return select_res_to_polars(res)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("table_name", "Table")
    def get_table(self, table_name):
        res = self._conn.get_table(
            db_name=self._db_name, table_name=table_name)
        if res.error_code == ErrorCode.OK:
            return RemoteTable(self._conn, self._db_name, table_name)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def show_tables(self):
        res = self._conn.show_tables(self._db_name)
        if res.error_code == ErrorCode.OK:
            return select_res_to_polars(res)
        else:
            raise InfinityException(res.error_code, res.error_msg)
