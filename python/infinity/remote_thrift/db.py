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
from infinity.db import Database
from infinity.remote_thrift.table import RemoteTable
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *


class RemoteThriftDatabase(Database, ABC):

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
                # "vector,1024,float32"
                length = column_big_info[1]
                element_type = column_big_info[2]
                proto_column_def = ColumnDef()
                proto_column_def.id = index
                proto_column_def.name = column_name

                column_type = DataType()
                column_type.logic_type = LogicType.Embedding

                embedding_type = EmbeddingType()
                if element_type == "bit":
                    embedding_type.element_type = ElementType.ElementBit
                elif element_type == "float32" or element_type == "float":
                    embedding_type.element_type = ElementType.ElementFloat32
                elif element_type == "float64" or element_type == "double":
                    embedding_type.element_type = ElementType.ElementFloat64
                elif element_type == "int8":
                    embedding_type.element_type = ElementType.ElementInt8
                elif element_type == "int16":
                    embedding_type.element_type = ElementType.ElementInt16
                elif element_type == "int32" or element_type == "int":
                    embedding_type.element_type = ElementType.ElementInt32
                elif element_type == "int64":
                    embedding_type.element_type = ElementType.ElementInt64
                else:
                    raise Exception(f"unknown element type: {element_type}")

                embedding_type.dimension = int(length)
                assert isinstance(embedding_type, EmbeddingType)
                assert embedding_type.element_type is not None
                assert embedding_type.dimension is not None

                physical_type = PhysicalType()
                physical_type.embedding_type = embedding_type

                column_type.physical_type = physical_type
                proto_column_def.data_type = column_type
                column_defs.append(proto_column_def)

            else:  # numeric or varchar
                # "c1": "int, primary key"
                datatype = column_big_info[0]
                constraints = column_big_info[1:]

                # process column definition
                proto_column_def = ColumnDef()
                proto_column_def.id = index
                proto_column_def.name = column_name
                proto_column_type = DataType()

                if datatype == "int8":
                    proto_column_type.logic_type = LogicType.TinyInt
                elif datatype == "int16":
                    proto_column_type.logic_type = LogicType.SmallInt
                elif datatype == "int32" or datatype == "int":
                    proto_column_type.logic_type = LogicType.Integer
                elif datatype == "int64":
                    proto_column_type.logic_type = LogicType.BigInt
                elif datatype == "int128":
                    proto_column_type.logic_type = LogicType.HugeInt
                elif datatype == "float":
                    proto_column_type.logic_type = LogicType.Float
                elif datatype == "double":
                    proto_column_type.logic_type = LogicType.Double
                elif datatype == "varchar":
                    varchar_type = VarcharType()
                    varchar_type.dimension = 1024
                    proto_column_type.logic_type = LogicType.Varchar
                    proto_column_type.physical_type = varchar_type
                elif datatype == "bool":
                    proto_column_type.logic_type = LogicType.Boolean
                else:
                    raise Exception(f"unknown datatype: {datatype}")

                proto_column_def.data_type = proto_column_type

                # process constraints
                for constraint in constraints:
                    if constraint == "null":
                        proto_column_def.constraints.append(Constraint.Null)
                    elif constraint == "not null":
                        proto_column_def.constraints.append(Constraint.NotNull)
                    elif constraint == "primary key":
                        proto_column_def.constraints.append(Constraint.PrimaryKey)
                    elif constraint == "unique":
                        proto_column_def.constraints.append(Constraint.Unique)
                    else:
                        raise Exception(f"unknown constraint: {constraint}")
                column_defs.append(proto_column_def)
        # print(column_defs)
        return self._conn.client.create_table(db_name=self._conn.db_name, table_name=table_name,
                                              column_defs=column_defs,
                                              option=options)

    def drop_table(self, table_name):
        return self._conn.client.drop_table(db_name=self._conn.db_name, table_name=table_name)

    def list_tables(self, db_name):
        return self._conn.client.list_tables(db_name=self._db_name)

    def describe_table(self, table_name):
        pass  # implement describe table logic here

    def get_table(self, table_name):
        return RemoteTable(self._conn, self._db_name, table_name)
