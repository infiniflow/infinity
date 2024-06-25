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
from infinity.common import VEC, SPARSE, InfinityException
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *
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
        case ttypes.LogicType.Tensor:
            return object
        case ttypes.LogicType.TensorArray:
            return object
        case ttypes.LogicType.Sparse:
            return object
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
            return list(struct.unpack('<{}q'.format(len(column_vector) // 8), column_vector))
        case ttypes.ColumnType.ColumnEmbedding:
            dimension = column_data_type.physical_type.embedding_type.dimension
            if column_data_type.physical_type.embedding_type.element_type == ttypes.ElementType.ElementInt8:
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
        case ttypes.ColumnType.ColumnTensor:
            return parse_tensor_bytes(column_data_type, column_vector)
        case ttypes.ColumnType.ColumnTensorArray:
            return parse_tensorarray_bytes(column_data_type, column_vector)
        case ttypes.ColumnType.ColumnSparse:
            return parse_sparse_bytes(column_data_type, column_vector)
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
            case ttypes.ElementType.ElementBit:
                raise NotImplementedError(f"Unsupported type {element_type}")
            case _:
                raise NotImplementedError(f"Unsupported type {element_type}")
        # print("indices: {}, values: {}".format(indices, values))
        res.append({"indices": indices, "values": values})
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


def make_match_tensor_expr(vector_column_name: str, embedding_data: VEC, embedding_data_type: str,
                           method_type: str, extra_option: str = None) -> MatchTensorExpr:
    match_tensor_expr = MatchTensorExpr()
    match_tensor_expr.column_expr = ColumnExpr(column_name=[vector_column_name], star=False)
    match_tensor_expr.search_method = method_type
    match_tensor_expr.extra_options = extra_option
    data = EmbeddingData()
    if embedding_data_type == 'bit':
        raise InfinityException(3057, f"Invalid embedding {embedding_data[0]} type")
    elif embedding_data_type == 'tinyint' or embedding_data_type == 'int8' or embedding_data_type == 'i8':
        elem_type = ElementType.ElementInt8
        data.i8_array_value = np.asarray(embedding_data, dtype=np.int8).flatten()
    elif embedding_data_type == 'smallint' or embedding_data_type == 'int16' or embedding_data_type == 'i16':
        elem_type = ElementType.ElementInt16
        data.i16_array_value = np.asarray(embedding_data, dtype=np.int16).flatten()
    elif embedding_data_type == 'int' or embedding_data_type == 'int32' or embedding_data_type == 'i32':
        elem_type = ElementType.ElementInt32
        data.i32_array_value = np.asarray(embedding_data, dtype=np.int32).flatten()
    elif embedding_data_type == 'bigint' or embedding_data_type == 'int64' or embedding_data_type == 'i64':
        elem_type = ElementType.ElementInt64
        data.i64_array_value = np.asarray(embedding_data, dtype=np.int64).flatten()
    elif embedding_data_type == 'float' or embedding_data_type == 'float32' or embedding_data_type == 'f32':
        elem_type = ElementType.ElementFloat32
        data.f32_array_value = np.asarray(embedding_data, dtype=np.float32).flatten()
    elif embedding_data_type == 'double' or embedding_data_type == 'float64' or embedding_data_type == 'f64':
        elem_type = ElementType.ElementFloat64
        data.f64_array_value = np.asarray(embedding_data, dtype=np.float64).flatten()
    else:
        raise InfinityException(3057, f"Invalid embedding {embedding_data[0]} type")

    match_tensor_expr.embedding_data_type = elem_type
    match_tensor_expr.embedding_data = data
    return match_tensor_expr

def make_match_sparse_expr(vector_column_name: str, sparse_data: SPARSE, metric_type: str, topn: int, opt_params: {} = None):
    column_expr = ColumnExpr(column_name=[vector_column_name], star=False)

    query_sparse_expr = ConstantExpr()
    if isinstance(sparse_data["values"][0], int):
        query_sparse_expr.literal_type = LiteralType.SparseIntegerArray
        query_sparse_expr.i64_array_idx = sparse_data["indices"]
        query_sparse_expr.i64_array_value = sparse_data["values"]
    elif isinstance(sparse_data["values"][0], float):
        query_sparse_expr.literal_type = LiteralType.SparseDoubleArray
        query_sparse_expr.i64_array_idx = sparse_data["indices"]
        query_sparse_expr.f64_array_value = sparse_data["values"]
    else:
        raise InfinityException(3058, f"Invalid sparse data {sparse_data['values'][0]} type")

    match_sparse_options = []
    if opt_params is not None:
        for k, v in opt_params.items():
            match_sparse_options.append(InitParameter(param_name=k, param_value=v))

    match_sparse_expr = MatchSparseExpr(column_expr=column_expr, query_sparse_expr=query_sparse_expr, metric_type=metric_type,
                                        topn=topn, opt_params=match_sparse_options)
    return match_sparse_expr
