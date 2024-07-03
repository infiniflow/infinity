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

from abc import ABC, abstractmethod
from enum import Enum
from typing import Optional, Union

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity.index import IndexInfo
from infinity.common import InfinityException
from infinity.embedded_infinity_ext import ExplainType as LocalExplainType

class ExplainType(Enum):
    Analyze = 1
    Ast = 2
    UnOpt = 3
    Opt = 4
    Physical = 5
    Pipeline = 6
    Fragment = 7

    def to_ttype(self):
        if self is ExplainType.Ast:
            return ttypes.ExplainType.Ast
        elif self is ExplainType.Analyze:
            return ttypes.ExplainType.Analyze
        elif self is ExplainType.UnOpt:
            return ttypes.ExplainType.UnOpt
        elif self is ExplainType.Opt:
            return ttypes.ExplainType.Opt
        elif self is ExplainType.Physical:
            return ttypes.ExplainType.Physical
        elif self is ExplainType.Pipeline:
            return ttypes.ExplainType.Pipeline
        elif self is ExplainType.Fragment:
            return ttypes.ExplainType.Fragment
        else:
            raise InfinityException(3081, "Unknown explain type")
    def to_local_ttype(self):
        if self is ExplainType.Ast:
            return LocalExplainType.kAst
        elif self is ExplainType.Analyze:
            return LocalExplainType.kAnalyze
        elif self is ExplainType.UnOpt:
            return LocalExplainType.kUnOpt
        elif self is ExplainType.Opt:
            return LocalExplainType.kOpt
        elif self is ExplainType.Physical:
            return LocalExplainType.kPhysical
        elif self is ExplainType.Pipeline:
            return LocalExplainType.kPipeline
        elif self is ExplainType.Fragment:
            return LocalExplainType.kFragment
        else:
            raise InfinityException(3081, "Unknown explain type")
class Table(ABC):

    @abstractmethod
    def create_index(self, index_name: str, index_infos: list[IndexInfo], options=None):
        pass

    @abstractmethod
    def drop_index(self, index_name: str):
        pass

    @abstractmethod
    def insert(self, data: list[dict[str, Union[str, int, float, list[Union[int, float]]]]]):
        pass

    @abstractmethod
    def import_data(self, file_path: str, import_options: {} = None):
        pass

    @abstractmethod
    def export_data(self, file_path: str, export_options: {} = None, columns: [str] = None):
        pass

    @abstractmethod
    def delete(self, cond: Optional[str] = None):
        pass

    @abstractmethod
    def update(self, cond: Optional[str], data: Optional[list[dict[str, Union[str, int, float]]]]):
        pass

    @abstractmethod
    def _execute_query(self, query):
        pass

    @abstractmethod
    def _explain_query(self, query):
        pass
