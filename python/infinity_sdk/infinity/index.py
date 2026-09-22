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

from sqlglot import maybe_parse

from infinity.common import InfinityException
from infinity.errors import ErrorCode
from infinity.remote_thrift.infinity_thrift_rpc import ttypes
from infinity.remote_thrift.utils import parse_expr


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
    SMVE = 10

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
            case IndexType.SMVE:
                return ttypes.IndexType.SMVE
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


SPARSEGRAM_DEFAULT_MIN_GRAM = 3
SPARSEGRAM_DEFAULT_MAX_GRAM = 12


def sparsegram_analyzer(min_gram: int = SPARSEGRAM_DEFAULT_MIN_GRAM,
                        max_gram: int = SPARSEGRAM_DEFAULT_MAX_GRAM,
                        fold_case: bool = False) -> str:
    """Build the analyzer name of a sparse gram full-text index.

    The analyzer emits content-defined n-grams of the whole value instead of
    tokens, which is what lets ``regex(column, pattern)`` filters use the index:
    the literals the pattern proves mandatory are turned into gram lookups
    before the regular expression runs, so the regular expression only verifies
    candidates. Chinese, Japanese and Korean values additionally get one and two
    character grams, so a single character or a two character word can narrow as
    well.

    Args:
        min_gram: Shortest window considered, in characters. Defaults to 3.
        max_gram: Longest window considered, in characters. Defaults to 12.
            Raising it adds rare, high information grams for little index space.
        fold_case: Emit the ASCII-lowercased form of every gram too. Required for
            a case insensitive pattern such as ``(?i)nobel prize`` to use the
            index at all; without it such a pattern falls back to a scan.

    Returns:
        An analyzer name such as ``"sparsegram-3-12"`` or
        ``"sparsegram-3-12-fold"``.

    Note:
        The name is stored in the index definition, so build a new index to
        change it.
    """
    for name, value in (("min_gram", min_gram), ("max_gram", max_gram)):
        if isinstance(value, bool) or not isinstance(value, int):
            raise InfinityException(ErrorCode.INVALID_INDEX_PARAM, f"{name} should be an integer, but got {value!r}")
    if min_gram < 1 or max_gram < min_gram:
        raise InfinityException(ErrorCode.INVALID_INDEX_PARAM,
                                f"Expected 1 <= min_gram <= max_gram, but got min_gram={min_gram} and max_gram={max_gram}")
    if not isinstance(fold_case, bool):
        raise InfinityException(ErrorCode.INVALID_INDEX_PARAM, f"fold_case should be a boolean, but got {fold_case!r}")
    name = f"sparsegram-{min_gram}-{max_gram}"
    return f"{name}-fold" if fold_case else name


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

    @staticmethod
    def sparsegram(target_name: str,
                   min_gram: int = SPARSEGRAM_DEFAULT_MIN_GRAM,
                   max_gram: int = SPARSEGRAM_DEFAULT_MAX_GRAM,
                   fold_case: bool = False) -> "IndexInfo":
        """Build a full-text index that makes `regex()` filters use grams.

        Shorthand for ``IndexInfo(target_name, IndexType.FullText,
        {"analyzer": sparsegram_analyzer(...)})``.

        Example::

            table.create_index("idx", IndexInfo.sparsegram("doc", fold_case=True))
            table.filter(regex_filter("doc", r"(?i)colou?r of the (sky|sea)"))
        """
        return IndexInfo(target_name, IndexType.FullText,
                         {"analyzer": sparsegram_analyzer(min_gram, max_gram, fold_case)})

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