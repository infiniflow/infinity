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

# import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from embedded_infinity import *

# def column_type_to_dtype(ttype: ttypes.ColumnType):
#     match ttype:
#         case ttypes.ColumnType.ColumnBool:
#             return dtype('bool')
#         case ttypes.ColumnType.ColumnInt8:
#             return dtype('int8')
#         case ttypes.ColumnType.ColumnInt16:
#             return dtype('int16')
#         case ttypes.ColumnType.ColumnInt32:
#             return dtype('int32')
#         case ttypes.ColumnType.ColumnInt64:
#             return dtype('int64')
#         case ttypes.ColumnType.ColumnFloat32:
#             return dtype('float32')
#         case ttypes.ColumnType.ColumnFloat64:
#             return dtype('float64')
#         case ttypes.ColumnType.ColumnVarchar:
#             return dtype('str')
#         case _:
#             raise NotImplementedError(f"Unsupported type {ttype}")


def logic_type_to_dtype(ttype: WrapDataType):
    match ttype.logical_type:
        case LogicalType.kBoolean:
            return dtype('bool')
        case LogicalType.kTinyInt:
            return dtype('int8')
        case LogicalType.kSmallInt:
            return dtype('int16')
        case LogicalType.kInteger:
            return dtype('int32')
        case LogicalType.kBigInt:
            return dtype('int64')
        case LogicalType.kFloat:
            return dtype('float32')
        case LogicalType.kDouble:
            return dtype('float64')
        case LogicalType.kVarchar:
            return dtype('str')
        case LogicalType.kEmbedding:
            if ttype.embedding_type is not None:
                match ttype.EmbeddingDataType.element_type:
                    case EmbeddingDataType.kElementInt8:
                        return object
                    case EmbeddingDataType.kElementInt16:
                        return object
                    case EmbeddingDataType.kElementInt32:
                        return object
                    case EmbeddingDataType.kElementFloat32:
                        return object
                    case EmbeddingDataType.kElementFloat64:
                        return object
                    case EmbeddingDataType.kElementBit:
                        return object
                    case _:
                        raise NotImplementedError(f"Unsupported type {ttype}")
        case _:
            raise NotImplementedError(f"Unsupported type {ttype}")

#
# def logic_type_to_pl_type(ttype: WrapDataType):
#     match ttype.logic_type:
#         case LogicalType.kBoolean:
#             return pl.Boolean
#         case LogicalType.kTinyInt:
#             return pl.Int8
#         case LogicalType.kSmallInt:
#             return pl.Int16
#         case LogicalType.kInteger:
#             return pl.Int32
#         case LogicalType.kBigInt:
#             return pl.Int64
#         case LogicalType.kFloat:
#             return pl.Float32
#         case LogicalType.kDouble:
#             return pl.Float64
#         case LogicalType.kVarchar:
#             return pl.Utf8
#         case LogicalType.kEmbedding:
#             if ttype.physical_type.embedding_type is not None:
#                 match ttype.physical_type.embedding_type.element_type:
#                     case EmbeddingDataType.kElementInt8:
#                         return pl.List
#                     case EmbeddingDataType.kElementInt16:
#                         return pl.List
#                     case EmbeddingDataType.kElementInt32:
#                         return pl.List
#                     case EmbeddingDataType.kElementFloat32:
#                         return pl.List
#                     case EmbeddingDataType.kElementFloat64:
#                         return pl.List
#                     case EmbeddingDataType.kElementBit:
#                         return pl.List
#                     case _:
#                         raise NotImplementedError(f"Unsupported type {ttype}")
#
#
def column_vector_to_list(column_type, column_data_type, column_vectors) -> \
        list[Any, ...]:
    column_vector = b''.join(column_vectors)
    match column_type:
        case LogicalType.kInteger:
            return list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
        case LogicalType.kBigInt:
            return list(struct.unpack('<{}q'.format(len(column_vector) // 8), column_vector))
        case LogicalType.kFloat:
            return list(struct.unpack('<{}f'.format(len(column_vector) // 4), column_vector))
        case LogicalType.kDouble:
            return list(struct.unpack('<{}d'.format(len(column_vector) // 8), column_vector))
        case LogicalType.kVarchar:
            return list(parse_bytes(column_vector))
        case LogicalType.kBoolean:
            return list(struct.unpack('<{}?'.format(len(column_vector)), column_vector))
        case LogicalType.kTinyInt:
            return list(struct.unpack('<{}b'.format(len(column_vector)), column_vector))
        case LogicalType.kSmallInt:
            return list(struct.unpack('<{}h'.format(len(column_vector) // 2), column_vector))
        # case LogicalType.ColumnRowID:
        #     all_list = list(struct.unpack('<{}i'.format(
        #         len(column_vector) // 4), column_vector))
        #     return [all_list[i:i + 2] for i in range(0, len(all_list), 2)]
        case LogicalType.kEmbedding:
            dimension = column_data_type.embedding_type.dimension
            # print(dimension)
            # print(len(column_vector))
            # print(len(column_vector) // dimension)
            element_type = column_data_type.embedding_type.element_type
            if element_type == EmbeddingDataType.kElementInt8:
                all_list = list(struct.unpack('<{}b'.format(len(column_vector)), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElementInt16:
                all_list = list(struct.unpack('<{}h'.format(len(column_vector) // 2), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElementInt32:
                all_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElementFloat32:
                all_list = list(struct.unpack('<{}f'.format(len(column_vector) // 4), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElementFloat64:
                all_list = list(struct.unpack('<{}d'.format(len(column_vector) // 8), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElementBit:
                all_list = list(struct.unpack('<{}B'.format(len(column_vector)), column_vector))
                result = []
                if dimension % 8 != 0:
                    raise ValueError(f"Unsupported dimension {dimension}")
                sub_dim: int = dimension // 8
                for i in range(0, len(all_list), sub_dim):
                    mid_res = all_list[i:i + sub_dim]
                    mid_res_int: int = 0
                    for j in reversed(mid_res):
                        mid_res_int = mid_res_int * 256 + j
                    result.append([f"\u007b0:0{dimension}b\u007d".format(mid_res_int)[::-1]])
                return result
            else:
                raise NotImplementedError(
                    f"Unsupported type {element_type}")
        case _:
            raise NotImplementedError(f"Unsupported type {column_type}")

#
# def parse_bytes(bytes_data):
#     results = []
#     offset = 0
#     while offset < len(bytes_data):
#         length = struct.unpack('I', bytes_data[offset:offset + 4])[0]
#         offset += 4
#         string_data = bytes_data[offset:offset + length].decode('utf-8')
#         results.append(string_data)
#         offset += length
#     return results
#
#
# def find_data_type(column_name: str, column_defs: list[ttypes.ColumnDef]) -> ttypes.DataType:
#     for column_def in column_defs:
#         if column_def.name == column_name:
#             return column_def.data_type
#     raise KeyError(f"column name {column_name} not found in column defs")
#
#
def build_result(res: WrapQueryResult) -> tuple[dict[str | Any, list[Any, Any]], dict[str | Any, Any]]:
    data_dict = {}
    data_type_dict = {}
    column_counter = defaultdict(int)
    for column_def, column_field in zip(res.column_defs, res.column_fields):
        original_column_name = column_def.column_name
        column_counter[original_column_name] += 1
        column_name = f"{original_column_name}_{column_counter[original_column_name]}" \
            if column_counter[original_column_name] > 1 \
            else original_column_name

        column_type = column_field.column_type
        column_data_type = column_def.column_type
        # convert to [ bytes:8 ]
        # column_vectors = [bytes(s, 'latin1') for s in column_field.column_vectors]
        # column_vectors = [bytes([ord(c) for c in s]) for s in column_field.column_vectors]
        column_vectors = column_field.column_vectors
        print(column_vectors)
        data_list = column_vector_to_list(column_type, column_data_type, column_vectors)
        print("data list = ", data_list)
        # data_series = pd.Series(data_list, dtype=logic_type_to_dtype(column_data_type))
        data_dict[column_name] = data_list
        data_type_dict[column_name] = column_data_type

    return data_dict, data_type_dict
