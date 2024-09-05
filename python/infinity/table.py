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
from typing import Optional, Union, Any

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity.index import IndexInfo
from infinity.common import InfinityException, INSERT_DATA
from infinity.errors import ErrorCode

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
            raise InfinityException(ErrorCode.INVALID_EXPLAIN_TYPE, "Unknown explain type")