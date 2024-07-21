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
import numpy as np
from numpy import dtype
from infinity.common import VEC, SPARSE, InfinityException, DEFAULT_MATCH_VECTOR_TOPN
from infinity.embedded_infinity_ext import *
from infinity.errors import ErrorCode

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
                match ttype.embedding_type.element_type:
                    case EmbeddingDataType.kElemInt8:
                        return object
                    case EmbeddingDataType.kElemInt16:
                        return object
                    case EmbeddingDataType.kElemInt32:
                        return object
                    case EmbeddingDataType.kElemFloat:
                        return object
                    case EmbeddingDataType.kElemDouble:
                        return object
                    case EmbeddingDataType.kElemBit:
                        return object
                    case _:
                        raise NotImplementedError(f"Unsupported type {ttype.embedding_type}")
        case LogicalType.kTensor:
            return object
        case LogicalType.kTensorArray:
            return object
        case LogicalType.kSparse:
            return object
        case _:
            raise NotImplementedError(f"Unsupported type {ttype}")

def tensor_to_list(column_data_type, binary_data) -> list[list[Any]]:
    dimension = column_data_type.embedding_type.dimension
    if column_data_type.embedding_type.element_type == EmbeddingDataType.kElemBit:
        all_list = list(struct.unpack('<{}B'.format(len(binary_data)), binary_data))
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
    elif column_data_type.embedding_type.element_type == EmbeddingDataType.kElemInt8:
        all_list = list(struct.unpack('<{}b'.format(len(binary_data)), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.embedding_type.element_type == EmbeddingDataType.kElemInt16:
        all_list = list(struct.unpack('<{}h'.format(len(binary_data) // 2), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.embedding_type.element_type == EmbeddingDataType.kElemInt32:
        all_list = list(struct.unpack('<{}i'.format(len(binary_data) // 4), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.embedding_type.element_type == EmbeddingDataType.kElemInt64:
        all_list = list(struct.unpack('<{}q'.format(len(binary_data) // 8), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.embedding_type.element_type == EmbeddingDataType.kElemFloat:
        all_list = list(struct.unpack('<{}f'.format(len(binary_data) // 4), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.embedding_type.element_type == EmbeddingDataType.kElemDouble:
        all_list = list(struct.unpack('<{}d'.format(len(binary_data) // 8), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    else:
        raise NotImplementedError(
            f"Unsupported type {column_data_type.embedding_type.element_type}")


def parse_tensor_bytes(column_data_type, bytes_data):
    results = []
    offset = 0
    while offset < len(bytes_data):
        length = struct.unpack('I', bytes_data[offset:offset + 4])[0]
        offset += 4
        tensor_data = tensor_to_list(column_data_type, bytes_data[offset:offset + length])
        results.append(tensor_data)
        offset += length
    return results


def parse_tensorarray_bytes(column_data_type, bytes_data):
    results = []
    offset = 0
    while offset < len(bytes_data):
        tensor_n = struct.unpack('I', bytes_data[offset:offset + 4])[0]
        offset += 4
        tensorarray_data = []
        for _ in range(tensor_n):
            length = struct.unpack('I', bytes_data[offset:offset + 4])[0]
            offset += 4
            tensor_data = tensor_to_list(column_data_type, bytes_data[offset:offset + length])
            offset += length
            tensorarray_data.append(tensor_data)
        results.append(tensorarray_data)
    return results

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
        case LogicalType.kRowID:
            all_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
            return [all_list[i:i + 2] for i in range(0, len(all_list), 2)]
        case LogicalType.kEmbedding:
            dimension = column_data_type.embedding_type.dimension
            element_type = column_data_type.embedding_type.element_type
            if element_type == EmbeddingDataType.kElemInt8:
                all_list = list(struct.unpack('<{}b'.format(len(column_vector)), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElemInt16:
                all_list = list(struct.unpack('<{}h'.format(len(column_vector) // 2), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElemInt32:
                all_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElemFloat:
                all_list = list(struct.unpack('<{}f'.format(len(column_vector) // 4), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElemDouble:
                all_list = list(struct.unpack('<{}d'.format(len(column_vector) // 8), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElemBit:
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
        case LogicalType.kSparse:
            return parse_sparse_bytes(column_data_type, column_vector)
        case LogicalType.kTensor:
            return parse_tensor_bytes(column_data_type, column_vector)
        case LogicalType.kTensorArray:
            return parse_tensorarray_bytes(column_data_type, column_vector)
        case _:
            raise NotImplementedError(f"Unsupported type {column_type}")

def parse_sparse_bytes(column_data_type, column_vector):
    dimension = column_data_type.sparse_type.dimension
    element_type = column_data_type.sparse_type.element_type
    index_type = column_data_type.sparse_type.index_type
    res = []
    offset = 0
    while offset < len(column_vector):
        nnz = struct.unpack('I', column_vector[offset:offset + 4])[0]
        offset += 4
        indices = []
        values = []
        match index_type:
            case EmbeddingDataType.kElemInt8:
                indices = struct.unpack('<{}b'.format(nnz), column_vector[offset:offset + nnz])
                offset += nnz
            case EmbeddingDataType.kElemInt16:
                indices = struct.unpack('<{}h'.format(nnz), column_vector[offset:offset + nnz * 2])
                offset += nnz * 2
            case EmbeddingDataType.kElemInt32:
                indices = struct.unpack('<{}i'.format(nnz), column_vector[offset:offset + nnz * 4])
                offset += nnz * 4
            case EmbeddingDataType.kElemInt64:
                indices = struct.unpack('<{}q'.format(nnz), column_vector[offset:offset + nnz * 8])
                offset += nnz * 8
            case _:
                raise NotImplementedError(f"Unsupported type {index_type}")
        match element_type:
            case EmbeddingDataType.kElemInt8:
                values = struct.unpack('<{}b'.format(nnz), column_vector[offset:offset + nnz])
                offset += nnz
            case EmbeddingDataType.kElemInt16:
                values = struct.unpack('<{}h'.format(nnz), column_vector[offset:offset + nnz * 2])
                offset += nnz * 2
            case EmbeddingDataType.kElemInt32:
                values = struct.unpack('<{}i'.format(nnz), column_vector[offset:offset + nnz * 4])
                offset += nnz * 4
            case EmbeddingDataType.kElemInt64:
                values = struct.unpack('<{}q'.format(nnz), column_vector[offset:offset + nnz * 8])
                offset += nnz * 8
            case EmbeddingDataType.kElemFloat:
                values = struct.unpack('<{}f'.format(nnz), column_vector[offset:offset + nnz * 4])
                offset += nnz * 4
            case EmbeddingDataType.kElemDouble:
                values = struct.unpack('<{}d'.format(nnz), column_vector[offset:offset + nnz * 8])
                offset += nnz * 8
            case EmbeddingDataType.kElemBit:
                pass
            case _:
                raise NotImplementedError(f"Unsupported type {element_type}")
        res.append({"indices": indices, "values": values})
    return res


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

def make_match_tensor_expr(vector_column_name: str, embedding_data: VEC, embedding_data_type: str,
                           method_type: str, extra_option: str = None) -> WrapMatchTensorExpr:
    match_tensor_expr = WrapMatchTensorExpr()

    match_tensor_expr.column_expr = WrapColumnExpr()
    match_tensor_expr.column_expr.names = [vector_column_name]
    match_tensor_expr.column_expr.star = False

    match_tensor_expr.search_method = method_type
    if (extra_option is not None) and (extra_option != ''):
        match_tensor_expr.options_text = extra_option
    data = EmbeddingData()
    elem_type = EmbeddingDataType.kElemFloat
    if embedding_data_type == 'bit':
        raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Invalid embedding {embedding_data[0]} type")
    elif embedding_data_type == 'tinyint' or embedding_data_type == 'int8' or embedding_data_type == 'i8':
        elem_type = EmbeddingDataType.kElemInt8
        data.i8_array_value = np.asarray(embedding_data, dtype=np.int8).flatten()
    elif embedding_data_type == 'smallint' or embedding_data_type == 'int16' or embedding_data_type == 'i16':
        elem_type = EmbeddingDataType.kElemInt16
        data.i16_array_value = np.asarray(embedding_data, dtype=np.int16).flatten()
    elif embedding_data_type == 'int' or embedding_data_type == 'int32' or embedding_data_type == 'i32':
        elem_type = EmbeddingDataType.kElemInt32
        data.i32_array_value = np.asarray(embedding_data, dtype=np.int32).flatten()
    elif embedding_data_type == 'bigint' or embedding_data_type == 'int64' or embedding_data_type == 'i64':
        elem_type = EmbeddingDataType.kElemInt64
        data.i64_array_value = np.asarray(embedding_data, dtype=np.int64).flatten()
    elif embedding_data_type == 'float' or embedding_data_type == 'float32' or embedding_data_type == 'f32':
        elem_type = EmbeddingDataType.kElemFloat
        data.f32_array_value = np.asarray(embedding_data, dtype=np.float32).flatten()
    elif embedding_data_type == 'double' or embedding_data_type == 'float64' or embedding_data_type == 'f64':
        elem_type = EmbeddingDataType.kElemDouble
        data.f64_array_value = np.asarray(embedding_data, dtype=np.float64).flatten()
    else:
        raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Invalid embedding {embedding_data[0]} type")

    match_tensor_expr.embedding_data_type = elem_type
    match_tensor_expr.embedding_data = data
    return match_tensor_expr

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
        column_vectors = column_field.column_vectors
        data_list = column_vector_to_list(column_type, column_data_type, column_vectors)

        data_dict[column_name] = data_list
        data_type_dict[column_name] = column_data_type

    return data_dict, data_type_dict
