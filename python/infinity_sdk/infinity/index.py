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


class IndexType(Enum):
    IVF = 1
    Hnsw = 2
    FullText = 3
    Secondary = 4
    EMVB = 5
    BMP = 6
    DiskAnn = 7

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
            case IndexType.EMVB:
                return ttypes.IndexType.EMVB
            case IndexType.BMP:
                return ttypes.IndexType.BMP
            case IndexType.DiskAnn:
                return ttypes.IndexType.DiskAnn
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
    def __init__(self, column_name: str, index_type: IndexType, params: dict = None):
        self.column_name = column_name
        self.index_type = index_type
        if params is not None:
            if isinstance(params, dict):
                self.params = params
            else:
                raise InfinityException(ErrorCode.INVALID_INDEX_PARAM, f"{params} should be dictionary type")
        else:
            self.params = None

    def __str__(self):
        return f"IndexInfo({self.column_name}, {self.index_type}, {self.params})"

    def __repr__(self):
        return self.__str__()

    def __eq__(self, other):
        return self.column_name == other.index_name and self.index_type == other.index_type and self.params == other.params

    def __hash__(self):
        return hash((self.column_name, self.index_type, self.params))

    def to_ttype(self):
        init_params_list = []
        if self.params is not None:
            for key, value in self.params.items():
                if isinstance(value, str):
                    init_params_list.append(ttypes.InitParameter(key, value))
                else:
                    raise InfinityException(ErrorCode.INVALID_INDEX_PARAM, f"{value} should be string type")

        return ttypes.IndexInfo(
            self.column_name.strip(),
            self.index_type.to_ttype(),
            init_params_list
        )
