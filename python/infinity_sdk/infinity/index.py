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

from enum import Enum

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity.common import InfinityException
from infinity.errors import ErrorCode
from infinity.remote_thrift.utils import parse_expr
from sqlglot import maybe_parse


class IndexType(Enum):
    IVF = 1
    Hnsw = 2
    FullText = 3
    Secondary = 4
    SecondaryFunctional = 5
    EMVB = 6
    BMP = 7
    DiskAnn = 8
    PLAID = 9

    def to_ttype(self):
        match self:
            case IndexType.IVF:
                return ttypes.IndexType.IVF
            case IndexType.Hnsw:
                return ttypes.IndexType.Hnsw
            case IndexType.FullText:
                return ttypes.IndexType.FullText
            case IndexType.Secondary:
                return ttypes.IndexType.Secondary
            case IndexType.SecondaryFunctional:
                return ttypes.IndexType.SecondaryFunctional
            case IndexType.EMVB:
                return ttypes.IndexType.EMVB
            case IndexType.BMP:
                return ttypes.IndexType.BMP
            case IndexType.DiskAnn:
                return ttypes.IndexType.DiskAnn
            case IndexType.PLAID:
                return ttypes.IndexType.PLAID
            case _:
                raise InfinityException(ErrorCode.INVALID_INDEX_TYPE, "Unknown index type")


class InitParameter:
    def __init__(self, param_name: str, param_value: str):
        self.param_name = param_name
        self.param_value = param_value

    def __str__(self):
        return f"InitParameter({self.param_name}, {self.param_value})"

    def __repr__(self):
        return self.__str__()

    def to_ttype(self):
        return ttypes.InitParameter(self.param_name, self.param_value)


class IndexInfo:
    def __init__(self, target_name: str, index_type: IndexType, params: dict = None):
        self.target_name = target_name
        self.index_type = index_type
        if params is not None:
            if isinstance(params, dict):
                self.params = params
            else:
                raise InfinityException(ErrorCode.INVALID_INDEX_PARAM, f"{params} should be dictionary type")
        else:
            self.params = None

    def __str__(self):
        return f"IndexInfo({self.target_name}, {self.index_type}, {self.params})"

    def __repr__(self):
        return self.__str__()

    def __eq__(self, other):
        return self.target_name == other.target_name and self.index_type == other.index_type and self.params == other.params

    def __hash__(self):
        return hash((self.target_name, self.index_type, self.params))

    def to_ttype(self):
        init_params_list = []
        if self.params is not None:
            for key, value in self.params.items():
                if isinstance(value, str):
                    init_params_list.append(ttypes.InitParameter(key, value))
                else:
                    raise InfinityException(ErrorCode.INVALID_INDEX_PARAM, f"{value} should be string type")

        column_name = None
        function_expr = None
        if self.index_type == IndexType.SecondaryFunctional:
            parsed_expr = parse_expr(maybe_parse(self.target_name))
            function_expr = parsed_expr.type.function_expr
        else:
            column_name = self.target_name.strip()

        index_info = ttypes.IndexInfo(
            column_name,
            self.index_type.to_ttype(),
            init_params_list,
            function_expr
        )
        return index_info