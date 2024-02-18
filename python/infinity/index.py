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
