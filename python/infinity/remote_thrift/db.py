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
from infinity.db import Database
from infinity.remote_thrift.table import RemoteTable


def get_ordinary_info(column_big_info, column_defs, column_name, index):
    # "c1": "int, primary key"
    datatype = column_big_info[0]
    constraints = column_big_info[1:]
    # process column definition
    proto_column_def = ttypes.ColumnDef()
    proto_column_def.id = index
    proto_column_def.name = column_name
    proto_column_type = ttypes.DataType()
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
        raise Exception(f"unknown datatype: {datatype}")

    proto_column_def.data_type = proto_column_type
    # process constraints
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
            raise Exception(f"unknown constraint: {constraint}")
    column_defs.append(proto_column_def)


def get_embedding_info(column_big_info, column_defs, column_name, index):
    # "vector,1024,float32"
    length = column_big_info[1]
    element_type = column_big_info[2]
    proto_column_def = ttypes.ColumnDef()
    proto_column_def.id = index
    proto_column_def.name = column_name
    column_type = ttypes.DataType()
    column_type.logic_type = ttypes.LogicType.Embedding
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
        raise Exception(f"unknown element type: {element_type}")
    embedding_type.dimension = int(length)
    assert isinstance(embedding_type, ttypes.EmbeddingType)
    assert embedding_type.element_type is not None
    assert embedding_type.dimension is not None
    physical_type = ttypes.PhysicalType()
    physical_type.embedding_type = embedding_type
    column_type.physical_type = physical_type
    proto_column_def.data_type = column_type
    column_defs.append(proto_column_def)


class RemoteDatabase(Database, ABC):

    def __init__(self, conn, name: str):
        self._conn = conn
        self._db_name = name

    def create_table(self, table_name: str, columns_definition: dict[str, str], options=None):
        # process column definitions
        # {"c1":'int, primary key',"c2":'vector,1024,float32'}
        # db_obj.create_table("my_table", {"c1": "int, primary key", "c2": "vector,1024,float32"}, None)
        # to column_defs
        column_defs = []
        for index, (column_name, column_info) in enumerate(columns_definition.items()):
            column_big_info = [item.strip() for item in column_info.split(",")]
            if column_big_info[0] == "vector":
                get_embedding_info(
                    column_big_info, column_defs, column_name, index)

            else:  # numeric or varchar
                get_ordinary_info(
                    column_big_info, column_defs, column_name, index)
        # print(column_defs)
        res = self._conn.create_table(db_name=self._db_name, table_name=table_name,
                                      column_defs=column_defs,
                                      option=options)
        if res.success is True:
            return RemoteTable(self._conn, self._db_name, table_name)
        else:
            raise Exception(res.error_msg)

    def drop_table(self, table_name, if_exists=True):
        return self._conn.drop_table(db_name=self._db_name, table_name=table_name, if_exists=if_exists)

    def list_tables(self):
        return self._conn.list_tables(self._db_name)

    def describe_table(self, table_name):
        pass  # implement describe table logic here

    def get_table(self, table_name):
        res = self._conn.get_table(
            db_name=self._db_name, table_name=table_name)
        if res.success is True:
            return RemoteTable(self._conn, self._db_name, table_name)
        else:
            raise Exception("Get Table Error")
