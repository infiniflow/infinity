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
import numpy as np
from infinity.common import VEC, SparseVector, InfinityException
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *
from collections import defaultdict
from typing import Any, Tuple, Dict, List, Optional
from datetime import date, time, datetime, timedelta

import polars as pl
from numpy import dtype
from infinity.errors import ErrorCode

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes


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
        case ttypes.LogicType.Float16:
            return dtype('float32')
        case ttypes.LogicType.BFloat16:
            return dtype('float32')
        case ttypes.LogicType.Varchar:
            return dtype('str')
        case ttypes.LogicType.Embedding:
            return object
        case ttypes.LogicType.MultiVector:
            return object
        case ttypes.LogicType.Tensor:
            return object
        case ttypes.LogicType.TensorArray:
            return object
        case ttypes.LogicType.Sparse:
            return object
        case ttypes.LogicType.Date:
            return object
        case ttypes.LogicType.Time:
            return object
        case ttypes.LogicType.DateTime:
            return object
        case ttypes.LogicType.Interval:
            return object
        case ttypes.LogicType.Timestamp:
            return object
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
        case ttypes.ColumnType.ColumnFloat16:
            return list(struct.unpack('<{}e'.format(len(column_vector) // 2), column_vector))
        case ttypes.ColumnType.ColumnBFloat16:
            tmp_u16 = np.frombuffer(column_vector, dtype='<i2')
            result_arr = np.zeros(2 * len(tmp_u16), dtype='<i2')
            result_arr[1::2] = tmp_u16
            view_float32 = result_arr.view('<f4')
            return list(view_float32)
        case ttypes.ColumnType.ColumnVarchar:
            return list(parse_bytes(column_vector))
        case ttypes.ColumnType.ColumnBool:
            return list(struct.unpack('<{}?'.format(len(column_vector)), column_vector))
        case ttypes.ColumnType.ColumnInt8:
            return list(struct.unpack('<{}b'.format(len(column_vector)), column_vector))
        case ttypes.ColumnType.ColumnInt16:
            return list(struct.unpack('<{}h'.format(len(column_vector) // 2), column_vector))
        case ttypes.ColumnType.ColumnRowID:
            return list(struct.unpack('<{}q'.format(len(column_vector) // 8), column_vector))
        case ttypes.ColumnType.ColumnEmbedding:
            dimension = column_data_type.physical_type.embedding_type.dimension
            if column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementUInt8:
                all_list = list(struct.unpack('<{}B'.format(len(column_vector)), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt8:
                all_list = list(struct.unpack('<{}b'.format(len(column_vector)), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt16:
                all_list = list(struct.unpack('<{}h'.format(len(column_vector) // 2), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt32:
                all_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt64:
                all_list = list(struct.unpack('<{}q'.format(len(column_vector) // 8), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementFloat32:
                all_list = list(struct.unpack('<{}f'.format(len(column_vector) // 4), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementFloat64:
                all_list = list(struct.unpack('<{}d'.format(len(column_vector) // 8), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementFloat16:
                all_list = list(struct.unpack('<{}e'.format(len(column_vector) // 2), column_vector))
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementBFloat16:
                tmp_u16 = np.frombuffer(column_vector, dtype='<i2')
                result_arr = np.zeros(2 * len(tmp_u16), dtype='<i2')
                result_arr[1::2] = tmp_u16
                view_float32 = result_arr.view('<f4')
                all_list = list(view_float32)
                return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
            elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementBit:
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
                    f"Unsupported type {column_data_type.physical_type.embedding_type.element_type}")
        case ttypes.ColumnType.ColumnMultiVector:
            return parse_tensor_bytes(column_data_type, column_vector)
        case ttypes.ColumnType.ColumnTensor:
            return parse_tensor_bytes(column_data_type, column_vector)
        case ttypes.ColumnType.ColumnTensorArray:
            return parse_tensorarray_bytes(column_data_type, column_vector)
        case ttypes.ColumnType.ColumnSparse:
            return parse_sparse_bytes(column_data_type, column_vector)
        case ttypes.ColumnType.ColumnDate:
            return parse_date_bytes(column_vector)
        case ttypes.ColumnType.ColumnTime:
            return parse_time_bytes(column_vector)
        case ttypes.ColumnType.ColumnDateTime:
            return parse_datetime_bytes(column_vector)
        case ttypes.ColumnType.ColumnTimestamp:
            return parse_datetime_bytes(column_vector)
        case ttypes.ColumnType.ColumnInterval:
            return parse_interval_bytes(column_vector)
        case _:
            raise NotImplementedError(f"Unsupported type {column_type}")

def parse_date_bytes(column_vector):
    parsed_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
    date_list = []
    epoch = date(1970, 1, 1)
    for value in parsed_list: 
        date_list.append(epoch + timedelta(days = value))
    return date_list

def parse_time_bytes(column_vector):
    parsed_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
    time_list = []
    for value in parsed_list:
        hours = (value // 3600) % 24
        minutes = (value % 3600) // 60
        seconds = value % 60
        time_list.append(time(hour=hours, minute=minutes, second=seconds))
    return time_list

def parse_datetime_bytes(column_vector):
    parsed_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
    datetime_list = []
    epoch = datetime(1970, 1, 1)
    for i in range(0, len(parsed_list), 2):
        if i + 1 < len(parsed_list):
            datetime_list.append(epoch + timedelta(days = parsed_list[i], seconds = parsed_list[i + 1]));
    return datetime_list

def parse_interval_bytes(column_vector):
    parsed_list = list(struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector))
    interval_list = []
    for value in parsed_list:
        time_list.append(timedelta(seconds=value))
    return interval_list

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


def parse_tensor_bytes(column_data_type: ttypes.DataType, bytes_data):
    results = []
    offset = 0
    while offset < len(bytes_data):
        length = struct.unpack('I', bytes_data[offset:offset + 4])[0]
        offset += 4
        tensor_data = tensor_to_list(column_data_type, bytes_data[offset:offset + length])
        results.append(tensor_data)
        offset += length
    return results


def parse_tensorarray_bytes(column_data_type: ttypes.DataType, bytes_data):
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


def tensor_to_list(column_data_type: ttypes.DataType, binary_data) -> list[list[Any]]:
    dimension = column_data_type.physical_type.embedding_type.dimension
    if column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementBit:
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
    elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementUInt8:
        all_list = list(struct.unpack('<{}B'.format(len(binary_data)), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt8:
        all_list = list(struct.unpack('<{}b'.format(len(binary_data)), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt16:
        all_list = list(struct.unpack('<{}h'.format(len(binary_data) // 2), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt32:
        all_list = list(struct.unpack('<{}i'.format(len(binary_data) // 4), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt64:
        all_list = list(struct.unpack('<{}q'.format(len(binary_data) // 8), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementFloat32:
        all_list = list(struct.unpack('<{}f'.format(len(binary_data) // 4), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementFloat64:
        all_list = list(struct.unpack('<{}d'.format(len(binary_data) // 8), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementFloat16:
        all_list = list(struct.unpack('<{}e'.format(len(binary_data) // 2), binary_data))
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    elif column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementBFloat16:
        tmp_u16 = np.frombuffer(binary_data, dtype='<i2')
        result_arr = np.zeros(2 * len(tmp_u16), dtype='<i2')
        result_arr[1::2] = tmp_u16
        view_float32 = result_arr.view('<f4')
        all_list = list(view_float32)
        return [all_list[i:i + dimension] for i in range(0, len(all_list), dimension)]
    else:
        raise NotImplementedError(
            f"Unsupported type {column_data_type.physical_type.embedding_type.element_type}")

def parse_sparse_bytes(column_data_type: ttypes.DataType, column_vector):
    dimension = column_data_type.physical_type.sparse_type.dimension
    element_type = column_data_type.physical_type.sparse_type.element_type
    index_type = column_data_type.physical_type.sparse_type.index_type
    res = []
    offset = 0
    # print(len(column_vector))
    while offset < len(column_vector):
        nnz = struct.unpack('I', column_vector[offset:offset + 4])[0]
        offset += 4
        # print(nnz)
        indices = []
        values = []
        match index_type:
            case ttypes.ElementType.ElementInt8:
                indices = struct.unpack('<{}b'.format(nnz), column_vector[offset:offset + nnz])
                offset += nnz
            case ttypes.ElementType.ElementInt16:
                indices = struct.unpack('<{}h'.format(nnz), column_vector[offset:offset + nnz * 2])
                offset += nnz * 2
            case ttypes.ElementType.ElementInt32:
                indices = struct.unpack('<{}i'.format(nnz), column_vector[offset:offset + nnz * 4])
                offset += nnz * 4
            case ttypes.ElementType.ElementInt64:
                indices = struct.unpack('<{}q'.format(nnz), column_vector[offset:offset + nnz * 8])
                offset += nnz * 8
            case _:
                raise NotImplementedError(f"Unsupported type {index_type}")
        match element_type:
            case ttypes.ElementType.ElementUInt8:
                values = struct.unpack('<{}B'.format(nnz), column_vector[offset:offset + nnz])
                offset += nnz
            case ttypes.ElementType.ElementInt8:
                values = struct.unpack('<{}b'.format(nnz), column_vector[offset:offset + nnz])
                offset += nnz
            case ttypes.ElementType.ElementInt16:
                values = struct.unpack('<{}h'.format(nnz), column_vector[offset:offset + nnz * 2])
                offset += nnz * 2
            case ttypes.ElementType.ElementInt32:
                values = struct.unpack('<{}i'.format(nnz), column_vector[offset:offset + nnz * 4])
                offset += nnz * 4
            case ttypes.ElementType.ElementInt64:
                values = struct.unpack('<{}q'.format(nnz), column_vector[offset:offset + nnz * 8])
                offset += nnz * 8
            case ttypes.ElementType.ElementFloat32:
                values = struct.unpack('<{}f'.format(nnz), column_vector[offset:offset + nnz * 4])
                offset += nnz * 4
            case ttypes.ElementType.ElementFloat64:
                values = struct.unpack('<{}d'.format(nnz), column_vector[offset:offset + nnz * 8])
                offset += nnz * 8
            case ttypes.ElementType.ElementFloat16:
                values = struct.unpack('<{}e'.format(nnz), column_vector[offset:offset + nnz * 2])
                offset += nnz * 2
            case ttypes.ElementType.ElementBFloat16:
                tmp_u16 = np.frombuffer(column_vector[offset:offset + nnz * 2], dtype='<i2')
                result_arr = np.zeros(2 * len(tmp_u16), dtype='<i2')
                result_arr[1::2] = tmp_u16
                view_float32 = result_arr.view('<f4')
                values = list(view_float32)
                offset += nnz * 2
            case ttypes.ElementType.ElementBit:
                raise NotImplementedError(f"Unsupported type {element_type}")
            case _:
                raise NotImplementedError(f"Unsupported type {element_type}")
        # print("indices: {}, values: {}".format(indices, values))
        res.append(SparseVector(list(indices), values).to_dict())
    return res


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


def make_match_tensor_expr(vector_column_name: str, embedding_data: VEC, embedding_data_type: str, method_type: str,
                           extra_option: str = None, filter_expr: Optional[ParsedExpr] = None) -> MatchTensorExpr:
    match_tensor_expr = MatchTensorExpr()
    match_tensor_expr.column_expr = ColumnExpr(column_name=[vector_column_name], star=False)
    match_tensor_expr.search_method = method_type
    match_tensor_expr.extra_options = extra_option
    match_tensor_expr.filter_expr = filter_expr
    data = EmbeddingData()
    if embedding_data_type == 'bit':
        raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Invalid embedding {embedding_data[0]} type")
    elif embedding_data_type in ['unsigned tinyint', 'uint8', 'u8']:
        elem_type = ElementType.ElementUInt8
        data.u8_array_value = np.asarray(embedding_data, dtype=np.uint8).flatten()
    elif embedding_data_type in ['tinyint', 'int8', 'i8']:
        elem_type = ElementType.ElementInt8
        data.i8_array_value = np.asarray(embedding_data, dtype=np.int8).flatten()
    elif embedding_data_type in ['smallint', 'int16', 'i16']:
        elem_type = ElementType.ElementInt16
        data.i16_array_value = np.asarray(embedding_data, dtype=np.int16).flatten()
    elif embedding_data_type in ['int', 'int32', 'i32']:
        elem_type = ElementType.ElementInt32
        data.i32_array_value = np.asarray(embedding_data, dtype=np.int32).flatten()
    elif embedding_data_type in ['bigint', 'int64', 'i64']:
        elem_type = ElementType.ElementInt64
        data.i64_array_value = np.asarray(embedding_data, dtype=np.int64).flatten()
    elif embedding_data_type in ['float', 'float32', 'f32']:
        elem_type = ElementType.ElementFloat32
        data.f32_array_value = np.asarray(embedding_data, dtype=np.float32).flatten()
    elif embedding_data_type in ['double', 'float64', 'f64']:
        elem_type = ElementType.ElementFloat64
        data.f64_array_value = np.asarray(embedding_data, dtype=np.float64).flatten()
    elif embedding_data_type in ['float16', 'fp16', 'f16']:
        elem_type = ElementType.ElementFloat16
        data.f16_array_value = np.asarray(embedding_data, dtype=np.float16).flatten()
    elif embedding_data_type in ['bfloat16', 'bf16']:
        elem_type = ElementType.ElementBFloat16
        data.bf16_array_value = np.asarray(embedding_data, dtype=np.float32).flatten()
    else:
        raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Invalid embedding {embedding_data[0]} type")

    match_tensor_expr.embedding_data_type = elem_type
    match_tensor_expr.embedding_data = data
    return match_tensor_expr


def make_match_sparse_expr(vector_column_name: str, sparse_data: SparseVector | dict, metric_type: str, topn: int,
                           opt_params: Optional[dict] = None, filter_expr: Optional[ParsedExpr] = None):
    column_expr = ColumnExpr(column_name=[vector_column_name], star=False)

    query_sparse_expr = ConstantExpr()

    match sparse_data:
        case SparseVector([int(), *_] as indices, [int(), *_] as values):
            query_sparse_expr.literal_type = LiteralType.SparseIntegerArray
            query_sparse_expr.i64_array_idx = indices
            query_sparse_expr.i64_array_value = values
        case SparseVector([int(), *_] as indices, [float(), *_] as values):
            query_sparse_expr.literal_type = LiteralType.SparseDoubleArray
            query_sparse_expr.i64_array_idx = indices
            query_sparse_expr.f64_array_value = values
        case SparseVector([int(), *_], None):
            raise InfinityException(ErrorCode.INVALID_CONSTANT_TYPE,
                                    f"No values! Sparse data does not support bool value type now")
        case dict():
            if len(sparse_data) == 0:
                raise InfinityException(ErrorCode.INVALID_EXPRESSION, "Empty sparse vector")
            match next(iter(sparse_data.values())):
                case int():
                    query_sparse_expr.literal_type = LiteralType.SparseIntegerArray
                    query_sparse_expr.i64_array_idx = [int(kk) for kk in sparse_data.keys()]
                    query_sparse_expr.i64_array_value = [int(vv) for vv in sparse_data.values()]
                case float():
                    query_sparse_expr.literal_type = LiteralType.SparseDoubleArray
                    query_sparse_expr.i64_array_idx = [int(kk) for kk in sparse_data.keys()]
                    query_sparse_expr.f64_array_value = [float(vv) for vv in sparse_data.values()]
                case _:
                    raise InfinityException(ErrorCode.INVALID_EXPRESSION,
                                            f"Invalid sparse vector value type: {type(next(iter(sparse_data.values())))}")
        case _:
            raise InfinityException(ErrorCode.INVALID_CONSTANT_TYPE, f"Invalid sparse data type {type(sparse_data)}")

    match_sparse_options = []
    if opt_params is not None:
        for k, v in opt_params.items():
            match_sparse_options.append(InitParameter(param_name=k, param_value=v))

    match_sparse_expr = MatchSparseExpr(column_expr=column_expr, query_sparse_expr=query_sparse_expr, metric_type=metric_type,
                                        topn=topn, opt_params=match_sparse_options, filter_expr=filter_expr)
    return match_sparse_expr
