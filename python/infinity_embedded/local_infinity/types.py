# Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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
import json
from collections import defaultdict
from typing import Any
import numpy as np
from numpy import dtype
from infinity_embedded.common import VEC, SparseVector, InfinityException
from infinity_embedded.embedded_infinity_ext import *
from infinity_embedded.errors import ErrorCode
from datetime import date, time, datetime, timedelta

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
        case LogicalType.kFloat16:
            return dtype('float32')
        case LogicalType.kBFloat16:
            return dtype('float32')
        case LogicalType.kVarchar:
            return dtype('str')
        case LogicalType.kEmbedding:
            return object
        case LogicalType.kMultiVector:
            return object
        case LogicalType.kTensor:
            return object
        case LogicalType.kTensorArray:
            return object
        case LogicalType.kSparse:
            return object
        case LogicalType.kDate:
            return object
        case LogicalType.kTime:
            return object
        case LogicalType.kDateTime:
            return object
        case LogicalType.kInterval:
            return object
        case LogicalType.kTimestamp:
            return object
        case LogicalType.kArray:
            return object
        case _:
            raise NotImplementedError(f"Unsupported type {ttype}")


def bf16_bytes_to_float32_list(binary_data):
    tmp_u16 = np.frombuffer(binary_data, dtype='<i2')
    result_arr = np.zeros(2 * len(tmp_u16), dtype='<i2')
    result_arr[1::2] = tmp_u16
    view_float32 = result_arr.view('<f4')
    return list(view_float32)


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
    elif column_data_type.embedding_type.element_type == EmbeddingDataType.kElemUInt8:
        all_list = list(struct.unpack('<{}B'.format(len(binary_data)), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
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
    elif column_data_type.embedding_type.element_type == EmbeddingDataType.kElemFloat16:
        all_list = list(struct.unpack('<{}e'.format(len(binary_data) // 2), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.embedding_type.element_type == EmbeddingDataType.kElemBFloat16:
        all_list = bf16_bytes_to_float32_list(binary_data)
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    else:
        raise NotImplementedError(
            f"Unsupported type {column_data_type.embedding_type.element_type}")


def parse_array_bytes(column_data_type: WrapDataType, bytes_data):
    results = []
    offset = 0
    while offset < len(bytes_data):
        array_data, offset = parse_single_array_bytes(column_data_type, bytes_data, offset)
        results.append(array_data)
    return results


def parse_single_array_bytes(column_data_type: WrapDataType, bytes_data, offset):
    assert column_data_type.logical_type == LogicalType.kArray
    element_data_type: WrapDataType = column_data_type.array_type
    array_element_cnt = struct.unpack('I', bytes_data[offset:offset + 4])[0]
    offset += 4
    single_pod_element_size = 0
    parse_single_element_func = None
    # TODO: RowID is always converted to BigInt, check src/function/builtin_functions.cpp
    match element_data_type.logical_type:
        case LogicalType.kBoolean:
            single_pod_element_size = 1
        case LogicalType.kTinyInt:
            single_pod_element_size = 1
        case LogicalType.kSmallInt:
            single_pod_element_size = 2
        case LogicalType.kInteger:
            single_pod_element_size = 4
        case LogicalType.kBigInt:
            single_pod_element_size = 8
        case LogicalType.kFloat:
            single_pod_element_size = 4
        case LogicalType.kDouble:
            single_pod_element_size = 8
        case LogicalType.kFloat16:
            single_pod_element_size = 2
        case LogicalType.kBFloat16:
            single_pod_element_size = 2
        case LogicalType.kEmbedding:
            embedding_dimension = column_data_type.embedding_type.dimension
            match column_data_type.embedding_type.element_type:
                case EmbeddingDataType.kElemBit:
                    single_pod_element_size = embedding_dimension // 8
                case EmbeddingDataType.kElemUInt8:
                    single_pod_element_size = embedding_dimension
                case EmbeddingDataType.kElemInt8:
                    single_pod_element_size = embedding_dimension
                case EmbeddingDataType.kElemInt16:
                    single_pod_element_size = embedding_dimension * 2
                case EmbeddingDataType.kElemInt32:
                    single_pod_element_size = embedding_dimension * 4
                case EmbeddingDataType.kElemInt64:
                    single_pod_element_size = embedding_dimension * 8
                case EmbeddingDataType.kElemFloat:
                    single_pod_element_size = embedding_dimension * 4
                case EmbeddingDataType.kElemDouble:
                    single_pod_element_size = embedding_dimension * 8
                case EmbeddingDataType.kElemFloat16:
                    single_pod_element_size = embedding_dimension * 2
                case EmbeddingDataType.kElemBFloat16:
                    single_pod_element_size = embedding_dimension * 2
                case _:
                    raise NotImplementedError(f"Unsupported type {element_data_type}")
        case LogicalType.kDate:
            single_pod_element_size = 4
        case LogicalType.kTime:
            single_pod_element_size = 4
        case LogicalType.kDateTime:
            single_pod_element_size = 8
        case LogicalType.kTimestamp:
            single_pod_element_size = 8
        case LogicalType.kInterval:
            single_pod_element_size = 4
        case LogicalType.kVarchar:
            parse_single_element_func = parse_single_str_bytes
        case LogicalType.kMultiVector:
            parse_single_element_func = parse_single_tensor_bytes
        case LogicalType.kTensor:
            parse_single_element_func = parse_single_tensor_bytes
        case LogicalType.kTensorArray:
            parse_single_element_func = parse_single_tensorarray_bytes
        case LogicalType.kSparse:
            parse_single_element_func = parse_single_sparse_bytes
        case LogicalType.kArray:
            parse_single_element_func = parse_single_array_bytes
        case _:
            raise NotImplementedError(f"Unexpected type {element_data_type}")
    if parse_single_element_func is not None:
        array_data = []
        for _ in range(array_element_cnt):
            element_data, offset = parse_single_element_func(element_data_type, bytes_data, offset)
            array_data.append(element_data)
        return array_data, offset
    assert single_pod_element_size > 0
    bytes_len = array_element_cnt * single_pod_element_size
    array_data = column_vector_to_list(element_data_type.logical_type, element_data_type,
                                       [bytes_data[offset:offset + bytes_len]])
    offset += bytes_len
    return array_data, offset


def parse_tensor_bytes(column_data_type: WrapDataType, bytes_data):
    results = []
    offset = 0
    while offset < len(bytes_data):
        tensor_data, offset = parse_single_tensor_bytes(column_data_type, bytes_data, offset)
        results.append(tensor_data)
    return results


def parse_single_tensor_bytes(column_data_type: WrapDataType, bytes_data, offset):
    length = struct.unpack('I', bytes_data[offset:offset + 4])[0]
    offset += 4
    tensor_data = tensor_to_list(column_data_type, bytes_data[offset:offset + length])
    offset += length
    return tensor_data, offset


def parse_tensorarray_bytes(column_data_type: WrapDataType, bytes_data):
    results = []
    offset = 0
    while offset < len(bytes_data):
        tensorarray_data, offset = parse_single_tensorarray_bytes(column_data_type, bytes_data, offset)
        results.append(tensorarray_data)
    return results


def parse_single_tensorarray_bytes(column_data_type: WrapDataType, bytes_data, offset):
    tensorarray_data = []
    tensor_n = struct.unpack('I', bytes_data[offset:offset + 4])[0]
    offset += 4
    for _ in range(tensor_n):
        tensor_data, offset = parse_single_tensor_bytes(column_data_type, bytes_data, offset)
        tensorarray_data.append(tensor_data)
    return tensorarray_data, offset


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
        case LogicalType.kFloat16:
            return list(struct.unpack('<{}e'.format(len(column_vector) // 2), column_vector))
        case LogicalType.kBFloat16:
            return bf16_bytes_to_float32_list(column_vector)
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
            if element_type == EmbeddingDataType.kElemUInt8:
                all_list = list(struct.unpack('<{}B'.format(len(column_vector)), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElemInt8:
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
            elif element_type == EmbeddingDataType.kElemFloat16:
                all_list = list(struct.unpack('<{}e'.format(len(column_vector) // 2), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif element_type == EmbeddingDataType.kElemBFloat16:
                all_list = bf16_bytes_to_float32_list(column_vector)
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
        case LogicalType.kMultiVector:
            return parse_tensor_bytes(column_data_type, column_vector)
        case LogicalType.kTensor:
            return parse_tensor_bytes(column_data_type, column_vector)
        case LogicalType.kTensorArray:
            return parse_tensorarray_bytes(column_data_type, column_vector)
        case LogicalType.kDate:
            return parse_date_bytes(column_vector)
        case LogicalType.kTime:
            return parse_time_bytes(column_vector)
        case LogicalType.kDateTime:
            return parse_datetime_bytes(column_vector)
        case LogicalType.kTimestamp:
            return parse_datetime_bytes(column_vector)
        case LogicalType.kInterval:
            return parse_interval_bytes(column_vector)
        case LogicalType.kArray:
            return parse_array_bytes(column_data_type, column_vector)
        case _:
            raise NotImplementedError(f"Unsupported type {column_type}")


def parse_date_bytes(column_vector):
    parsed_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
    date_list = []
    epoch = date(1970, 1, 1)
    for value in parsed_list: 
        date_list.append((epoch + timedelta(days = value)).strftime('%Y-%m-%d'))
    return date_list


def parse_time_bytes(column_vector):
    parsed_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
    time_list = []
    for value in parsed_list:
        hours = (value // 3600) % 24
        minutes = (value % 3600) // 60
        seconds = value % 60
        time_list.append(time(hour=hours, minute=minutes, second=seconds).strftime('%H:%M:%S'))
    return time_list


def parse_datetime_bytes(column_vector):
    parsed_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
    datetime_list = []
    epoch = datetime(1970, 1, 1)
    for i in range(0, len(parsed_list), 2):
        if i + 1 < len(parsed_list):
            datetime_list.append((epoch + timedelta(days = parsed_list[i], seconds = parsed_list[i + 1])).strftime('%Y-%m-%d %H:%M:%S'));
    return datetime_list


def parse_interval_bytes(column_vector):
    parsed_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
    interval_list = []
    for value in parsed_list:
        interval_list.append(timedelta(seconds=value))
    return interval_list


def parse_sparse_bytes(column_data_type: WrapDataType, column_vector):
    res = []
    offset = 0
    while offset < len(column_vector):
        sparse_vector, offset = parse_single_sparse_bytes(column_data_type, column_vector, offset)
        res.append(sparse_vector)
    return res


def parse_single_sparse_bytes(column_data_type: WrapDataType, column_vector, offset):
    dimension = column_data_type.sparse_type.dimension
    element_type = column_data_type.sparse_type.element_type
    index_type = column_data_type.sparse_type.index_type
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
        case EmbeddingDataType.kElemUInt8:
            values = struct.unpack('<{}B'.format(nnz), column_vector[offset:offset + nnz])
            offset += nnz
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
        case EmbeddingDataType.kElemFloat16:
            values = struct.unpack('<{}e'.format(nnz), column_vector[offset:offset + nnz * 2])
            offset += nnz * 2
        case EmbeddingDataType.kElemBFloat16:
            values = bf16_bytes_to_float32_list(column_vector[offset:offset + nnz * 2])
            offset += nnz * 2
        case EmbeddingDataType.kElemBit:
            pass
        case _:
            raise NotImplementedError(f"Unsupported type {element_type}")
    return SparseVector(list(indices), list(values)).to_dict(), offset


def parse_bytes(bytes_data):
    results = []
    offset = 0
    while offset < len(bytes_data):
        string_data, offset = parse_single_str_bytes(None, bytes_data, offset)
        results.append(string_data)
    return results


def parse_single_str_bytes(_, bytes_data, offset):
    length = struct.unpack('I', bytes_data[offset:offset + 4])[0]
    offset += 4
    string_data = bytes_data[offset:offset + length].decode('utf-8')
    offset += length
    return string_data, offset


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
    elif embedding_data_type in ['unsigned tinyint', 'uint8', 'u8']:
        elem_type = EmbeddingDataType.kElemUInt8
        data.u8_array_value = np.asarray(embedding_data, dtype=np.uint8).flatten()
    elif embedding_data_type in ['tinyint', 'int8', 'i8']:
        elem_type = EmbeddingDataType.kElemInt8
        data.i8_array_value = np.asarray(embedding_data, dtype=np.int8).flatten()
    elif embedding_data_type in ['smallint', 'int16', 'i16']:
        elem_type = EmbeddingDataType.kElemInt16
        data.i16_array_value = np.asarray(embedding_data, dtype=np.int16).flatten()
    elif embedding_data_type in ['int', 'int32', 'i32']:
        elem_type = EmbeddingDataType.kElemInt32
        data.i32_array_value = np.asarray(embedding_data, dtype=np.int32).flatten()
    elif embedding_data_type in ['bigint', 'int64', 'i64']:
        elem_type = EmbeddingDataType.kElemInt64
        data.i64_array_value = np.asarray(embedding_data, dtype=np.int64).flatten()
    elif embedding_data_type in ['float', 'float32', 'f32']:
        elem_type = EmbeddingDataType.kElemFloat
        data.f32_array_value = np.asarray(embedding_data, dtype=np.float32).flatten()
    elif embedding_data_type in ['double', 'float64', 'f64']:
        elem_type = EmbeddingDataType.kElemDouble
        data.f64_array_value = np.asarray(embedding_data, dtype=np.float64).flatten()
    elif embedding_data_type in ['float16', 'fp16', 'f16']:
        elem_type = EmbeddingDataType.kElemFloat16
        data.f16_array_value = np.asarray(embedding_data, dtype=np.float16).flatten()
    elif embedding_data_type in ['bfloat16', 'bf16']:
        elem_type = EmbeddingDataType.kElemBFloat16
        data.bf16_array_value = np.asarray(embedding_data, dtype=np.float32).flatten()
    else:
        raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Invalid embedding {embedding_data[0]} type")

    match_tensor_expr.embedding_data_type = elem_type
    match_tensor_expr.embedding_data = data
    return match_tensor_expr

def build_result(res: WrapQueryResult) -> tuple[dict[str | Any, list[Any, Any]], dict[str | Any, Any], Any]:
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

    extra_result = None
    if res.extra_result is not None:
        try:
            extra_result = json.loads(res.extra_result)
        except json.JSONDecodeError:
            pass

    return data_dict, data_type_dict, extra_result
