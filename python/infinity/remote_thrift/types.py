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
from collections import defaultdict
from typing import Any, Tuple, Dict, List

import polars as pl
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


def logic_type_to_dtype(ttype: ttypes.DataType):
    match ttype.logic_type:
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
        case ttypes.LogicType.Embedding:
            if ttype.physical_type.embedding_type is not None:
                match ttype.physical_type.embedding_type.element_type:
                    case ttypes.ElementType.ElementInt8:
                        return object
                    case ttypes.ElementType.ElementInt16:
                        return object
                    case ttypes.ElementType.ElementInt32:
                        return object
                    case ttypes.ElementType.ElementFloat32:
                        return object
                    case ttypes.ElementType.ElementFloat64:
                        return object
                    case ttypes.ElementType.ElementBit:
                        return object
                    case _:
                        raise NotImplementedError(f"Unsupported type {ttype}")
        case _:
            raise NotImplementedError(f"Unsupported type {ttype}")


def logic_type_to_pl_type(ttype: ttypes.DataType):
    match ttype.logic_type:
        case ttypes.LogicType.Boolean:
            return pl.Boolean
        case ttypes.LogicType.TinyInt:
            return pl.Int8
        case ttypes.LogicType.SmallInt:
            return pl.Int16
        case ttypes.LogicType.Integer:
            return pl.Int32
        case ttypes.LogicType.BigInt:
            return pl.Int64
        case ttypes.LogicType.Float:
            return pl.Float32
        case ttypes.LogicType.Double:
            return pl.Float64
        case ttypes.LogicType.Varchar:
            return pl.Utf8
        case ttypes.LogicType.Embedding:
            if ttype.physical_type.embedding_type is not None:
                match ttype.physical_type.embedding_type.element_type:
                    case ttypes.ElementType.ElementInt8:
                        return pl.List
                    case ttypes.ElementType.ElementInt16:
                        return pl.List
                    case ttypes.ElementType.ElementInt32:
                        return pl.List
                    case ttypes.ElementType.ElementFloat32:
                        return pl.List
                    case ttypes.ElementType.ElementFloat64:
                        return pl.List
                    case ttypes.ElementType.ElementBit:
                        return pl.List
                    case _:
                        raise NotImplementedError(f"Unsupported type {ttype}")


def column_vector_to_list(column_type: ttypes.ColumnType, column_data_type: ttypes.DataType, column_vectors) -> \
        list[Any, ...]:
    column_vector = b''.join(column_vectors)
    match column_type:
        case ttypes.ColumnType.ColumnInt32:
            return list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
        case ttypes.ColumnType.ColumnInt64:
            return list(struct.unpack('<{}q'.format(len(column_vector) // 8), column_vector))
        case ttypes.ColumnType.ColumnFloat32:
            return list(struct.unpack('<{}f'.format(len(column_vector) // 4), column_vector))
        case ttypes.ColumnType.ColumnFloat64:
            return list(struct.unpack('<{}d'.format(len(column_vector) // 8), column_vector))
        case ttypes.ColumnType.ColumnVarchar:
            return list(parse_bytes(column_vector))
        case ttypes.ColumnType.ColumnBool:
            return list(struct.unpack('<{}?'.format(len(column_vector)), column_vector))
        case ttypes.ColumnType.ColumnInt8:
            return list(struct.unpack('<{}b'.format(len(column_vector)), column_vector))
        case ttypes.ColumnType.ColumnInt16:
            return list(struct.unpack('<{}h'.format(len(column_vector) // 2), column_vector))
        case ttypes.ColumnType.ColumnRowID:
            all_list = list(struct.unpack('<{}i'.format(
                len(column_vector) // 4), column_vector))
            return [all_list[i:i + 2] for i in range(0, len(all_list), 2)]
        case ttypes.ColumnType.ColumnEmbedding:
            dimension = column_data_type.physical_type.embedding_type.dimension
            # print(dimension)
            # print(len(column_vector))
            # print(len(column_vector) // dimension)
            if column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt8:
                all_list = list(struct.unpack(
                    '<{}b'.format(len(column_vector)), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt16:
                all_list = list(struct.unpack('<{}h'.format(
                    len(column_vector) // 2), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt32:
                all_list = list(struct.unpack('<{}i'.format(
                    len(column_vector) // 4), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementFloat32:
                all_list = list(struct.unpack('<{}f'.format(
                    len(column_vector) // 4), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementFloat64:
                all_list = list(struct.unpack('<{}d'.format(
                    len(column_vector) // 8), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementBit:
                all_list = list(struct.unpack(
                    '<{}?'.format(len(column_vector)), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            else:
                raise NotImplementedError(
                    f"Unsupported type {column_data_type.physical_type.embedding_type.element_type}")
        case _:
            raise NotImplementedError(f"Unsupported type {column_type}")


def parse_bytes(bytes_data):
    results = []
    offset = 0
    while offset < len(bytes_data):
        length = struct.unpack('I', bytes_data[offset:offset + 4])[0]
        offset += 4
        string_data = bytes_data[offset:offset + length].decode('utf-8')
        results.append(string_data)
        offset += length
    return results


def find_data_type(column_name: str, column_defs: list[ttypes.ColumnDef]) -> ttypes.DataType:
    for column_def in column_defs:
        if column_def.name == column_name:
            return column_def.data_type
    raise KeyError(f"column name {column_name} not found in column defs")


def build_result(res: ttypes.SelectResponse) -> tuple[dict[str | Any, list[Any, Any]], dict[str | Any, Any]]:
    data_dict = {}
    data_type_dict = {}
    column_counter = defaultdict(int)
    for column_def, column_field in zip(res.column_defs, res.column_fields):
        original_column_name = column_def.name
        column_counter[original_column_name] += 1
        column_name = f"{original_column_name}_{column_counter[original_column_name]}" \
            if column_counter[original_column_name] > 1 \
            else original_column_name

        column_type = column_field.column_type
        column_data_type = column_def.data_type
        column_vectors = column_field.column_vectors

        data_list = column_vector_to_list(column_type, column_data_type, column_vectors)
        # data_series = pd.Series(data_list, dtype=logic_type_to_dtype(column_data_type))
        data_dict[column_name] = data_list
        data_type_dict[column_name] = column_data_type

    return data_dict, data_type_dict
