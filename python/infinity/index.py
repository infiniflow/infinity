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

from embedded_infinity import IndexType as LocalIndexType
from embedded_infinity import InitParameter as LocalInitParameter
from embedded_infinity import WrapIndexInfo as LocalIndexInfo

class IndexType(Enum):
    IVFFlat = 1
    HnswLVQ = 2
    Hnsw = 3
    FullText = 4

    def to_ttype(self):
        if self == IndexType.IVFFlat:
            return ttypes.IndexType.IVFFlat
        elif self == IndexType.HnswLVQ:
            return ttypes.IndexType.HnswLVQ
        elif self == IndexType.Hnsw:
            return ttypes.IndexType.Hnsw
        elif self == IndexType.FullText:
            return ttypes.IndexType.FullText
        else:
            raise InfinityException(3060, "Unknown index type")

    def to_local_type(self):
        if self == IndexType.IVFFlat:
            return LocalIndexType.kIVFFlat
        elif self == IndexType.HnswLVQ:
            return LocalIndexType.kHnswLVQ
        elif self == IndexType.Hnsw:
            return LocalIndexType.kHnsw
        elif self == IndexType.FullText:
            return LocalIndexType.kFullText
        else:
            raise Exception("Unknown index type")

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