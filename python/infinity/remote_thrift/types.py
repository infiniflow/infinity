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

import struct
from typing import Dict, Any

import pandas as pd
from numpy import dtype

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes


def column_type_to_dtype(ttype: ttypes.ColumnType):
    match ttype:
        case ttypes.ColumnType.ColumnBool:
            return dtype('bool')
        case ttypes.ColumnType.ColumnInt8:
            return dtype('int8')
        case ttypes.ColumnType.ColumnInt16:
            return dtype('int16')
        case ttypes.ColumnType.ColumnInt32:
            return dtype('int32')
        case ttypes.ColumnType.ColumnInt64:
            return dtype('int64')
        case ttypes.ColumnType.ColumnFloat32:
            return dtype('float32')
        case ttypes.ColumnType.ColumnFloat64:
            return dtype('float64')
        case ttypes.ColumnType.ColumnVarchar:
            return dtype('str')
        case _:
            raise NotImplementedError(f"Unsupported type {ttype}")


def logic_type_to_dtype(ttype: ttypes.LogicType):
    match ttype:
        case ttypes.LogicType.Boolean:
            return dtype('bool')
        case ttypes.LogicType.TinyInt:
            return dtype('int8')
        case ttypes.LogicType.SmallInt:
            return dtype('int16')
        case ttypes.LogicType.Integer:
            return dtype('int32')
        case ttypes.LogicType.BigInt:
            return dtype('int64')
        case ttypes.LogicType.Float:
            return dtype('float32')
        case ttypes.LogicType.Double:
            return dtype('float64')
        case ttypes.LogicType.Varchar:
            return dtype('str')
        case _:
            raise NotImplementedError(f"Unsupported type {ttype}")


def column_vector_to_tuple_list(column_type: ttypes.ColumnType, column_vector) -> tuple[Any, ...]:
    match column_type:
        case ttypes.ColumnType.ColumnInt32:
            return struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector)
        case ttypes.ColumnType.ColumnInt64:
            return struct.unpack('<{}q'.format(len(column_vector) // 8), column_vector)
        case ttypes.ColumnType.ColumnFloat32:
            return struct.unpack('<{}f'.format(len(column_vector) // 4), column_vector)
        case ttypes.ColumnType.ColumnFloat64:
            return struct.unpack('<{}d'.format(len(column_vector) // 8), column_vector)
        case ttypes.ColumnType.ColumnVarchar:
            return tuple(parse_bytes(column_vector))
        case _:
            raise NotImplementedError(f"Unsupported type {column_type}")


def parse_bytes(bytes_data):
    results = []
    offset = 0
    while offset < len(bytes_data):
        length = struct.unpack('Q', bytes_data[offset:offset + 8])[0]
        offset += 8
        string_data = bytes_data[offset:offset + length].decode('utf-8')
        results.append(string_data)
        offset += length
    return results


def build_result(res: ttypes.SelectResponse) -> pd.DataFrame:
    data_dict: Dict[str, tuple[Any, ...]] = {}
    column_names = []
    types = []
    for column_def in res.column_defs:
        column_names.append(column_def.name)
        types.append(logic_type_to_dtype(column_def.data_type.logic_type))
        # print()
        # print(res.column_fields)
        match res.column_fields.__len__():
            case 0:
                data_dict[column_def.name] = []
            case _:
                column_field = res.column_fields[column_def.id]
                column_type = column_field.column_type
                column_vector = column_field.column_vector
                data_dict[column_def.name] = column_vector_to_tuple_list(column_type, column_vector)

    type_dict = dict(zip(column_names, types))
    # print()
    # print(data_dict)
    # print(type_dict)
    # print()
    # print(pd.DataFrame.from_dict(data_dict, orient='columns').astype(type_dict))
    return pd.DataFrame.from_dict(data_dict, orient='columns').astype(type_dict)
