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

from .brpc_pb.infinity_brpc_pb2 import *
from .brpc_pb.infinity_brpc_pb2_grpc import *
from infinity.db import Database
from infinity.remote_brpc.table import RemoteTable


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
                    embedding_type.embedding_data_type = ElementType.kElemBit
                elif element_type == "float32" or element_type == "float":
                    embedding_type.embedding_data_type = ElementType.kElemFloat
                elif element_type == "float64" or element_type == "double":
                    embedding_type.embedding_data_type = ElementType.kElemDouble
                elif element_type == "int8":
                    embedding_type.embedding_data_type = ElementType.kElemInt8
                elif element_type == "int16":
                    embedding_type.embedding_data_type = ElementType.kElemInt16
                elif element_type == "int32" or element_type == "int":
                    embedding_type.embedding_data_type = ElementType.kElemInt32
                elif element_type == "int64":
                    embedding_type.embedding_data_type = ElementType.kElemInt64
                else:
                    raise Exception(f"unknown element type: {element_type}")

                embedding_type.dimension = int(length)
                column_type.embedding_type.CopyFrom(embedding_type)
                proto_column_def.column_type.CopyFrom(column_type)
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
                    proto_column_type.logic_type = LogicType.Varchar
                    proto_column_type.VarcharType.width = 1024
                elif datatype == "bool":
                    proto_column_type.logic_type = LogicType.Bool
                else:
                    raise Exception(f"unknown datatype: {datatype}")

                proto_column_def.column_type.CopyFrom(proto_column_type)
                # process constraints
                for constraint in constraints:
                    if constraint == "null":
                        proto_column_def.constraints.append(Constraint.kNull)
                    elif constraint == "not null":
                        proto_column_def.constraints.append(Constraint.kNotNull)
                    elif constraint == "primary key":
                        proto_column_def.constraints.append(Constraint.kPrimaryKey)
                    elif constraint == "unique":
                        proto_column_def.constraints.append(Constraint.kUnique)
                    else:
                        raise Exception(f"unknown constraint: {constraint}")
                column_defs.append(proto_column_def)
        # print(column_defs)
        return self._conn.client.create_table(db_name=self._conn.db_name, table_name=table_name,
                                              column_defs=column_defs,
                                              options=options)

    def drop_table(self, table_name):
        return self._conn.client.drop_table(db_name=self._conn.db_name, table_name=table_name)

    def list_tables(self):
        return self._conn.client.list_tables(self._db_name)

    def describe_table(self, table_name):
        pass  # implement describe table logic here

    def get_table(self, table_name):

        return RemoteTable(self._conn, self._db_name, table_name)
