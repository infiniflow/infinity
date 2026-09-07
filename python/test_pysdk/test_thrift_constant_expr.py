# Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import numpy as np
import pytest
from infinity.common import InfinityException
from infinity.errors import ErrorCode
from infinity.remote_thrift.infinity_thrift_rpc import ttypes
from infinity.remote_thrift.utils import get_remote_constant_expr_from_python_value


def test_empty_list_constant_is_invalid_expression():
    with pytest.raises(InfinityException) as exc_info:
        get_remote_constant_expr_from_python_value([])

    assert exc_info.value.error_code == ErrorCode.INVALID_EXPRESSION


@pytest.mark.parametrize(
    ("value", "literal_type"),
    [
        ([1], ttypes.LiteralType.IntegerArray),
        (np.array([1]), ttypes.LiteralType.IntegerArray),
        ([np.array([1, 2])], ttypes.LiteralType.IntegerTensor),
    ],
)
def test_non_empty_array_constants_remain_supported(value, literal_type):
    constant_expression = get_remote_constant_expr_from_python_value(value)

    assert constant_expression.literal_type == literal_type
