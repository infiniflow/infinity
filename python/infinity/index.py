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

from infinity.embedded_infinity_ext import IndexType as LocalIndexType
from infinity.embedded_infinity_ext import InitParameter as LocalInitParameter
from infinity.embedded_infinity_ext import WrapIndexInfo as LocalIndexInfo

class IndexType(Enum):
    IVFFlat = 1
    HnswLVQ = 2
    Hnsw = 3
    FullText = 4
    Secondary = 5
    EMVB = 6
    BMP = 7

    def to_ttype(self):
        match self:
            case IndexType.IVFFlat:
                return ttypes.IndexType.IVFFlat
            case IndexType.HnswLVQ:
                return ttypes.IndexType.HnswLVQ
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
            case _:
                raise InfinityException(3060, "Unknown index type")

    def to_local_type(self):
        match self:
            case IndexType.IVFFlat:
                return LocalIndexType.kIVFFlat
            case IndexType.HnswLVQ:
                return LocalIndexType.kHnswLVQ
            case IndexType.Hnsw:
                return LocalIndexType.kHnsw
            case IndexType.FullText:
                return LocalIndexType.kFullText
            case IndexType.Secondary:
                return LocalIndexType.kSecondary
            case IndexType.EMVB:
                return LocalIndexType.kEMVB
            case IndexType.BMP:
                return LocalIndexType.kBMP
            case _:
                raise InfinityException(3060, "Unknown index type")

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

    def to_local_type(self):
        local_init_parameter = LocalInitParameter()
        local_init_parameter.param_name = self.param_name
        local_init_parameter.param_value = self.param_value
        return local_init_parameter

class IndexInfo:
    def __init__(self, column_name: str, index_type: IndexType, params: list[InitParameter]):
        self.column_name = column_name
        self.index_type = index_type
        self.params = params

    def __str__(self):
        return f"IndexInfo({self.column_name}, {self.index_type}, {self.params})"

    def __repr__(self):
        return self.__str__()

    def __eq__(self, other):
        return self.column_name == other.index_name and self.index_type == other.index_type and self.params == other.params

    def __hash__(self):
        return hash((self.column_name, self.index_type, self.params))

    def to_ttype(self):
        return ttypes.IndexInfo(
            self.column_name,
            self.index_type.to_ttype(),
            [p.to_ttype() for p in self.params]
        )

    def to_local_type(self):
        local_index_info = LocalIndexInfo()
        local_index_info.index_type = self.column_name
        local_index_info.column_name = self.index_type.to_local_type()
        local_index_info.index_param_list = [p.to_local_type() for p in self.params]
        return local_index_info