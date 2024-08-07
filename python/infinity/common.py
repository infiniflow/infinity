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
from pathlib import Path
from typing import Union

import numpy as np


class NetworkAddress:
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port

    def __str__(self):
        return f'IP: {self.ip}, Port: {self.port}'


URI = Union[NetworkAddress, Path]
VEC = Union[list, np.ndarray]
SPARSE = dict[str, Union[list[int], list[float]]]
INSERT_DATA = dict[str, Union[str, int, float, list[Union[int, float]]], SPARSE]

LOCAL_HOST = NetworkAddress("127.0.0.1", 23817)

# test embedded_infinity
LOCAL_INFINITY_PATH = "/var/infinity"

class ConflictType(object):
    Ignore = 0
    Error = 1
    Replace = 2


class InfinityException(Exception):
    def __init__(self, error_code=0, error_message=None):
        self.error_code = error_code
        self.error_message = error_message

class CommonMatchTensorExpr:
    def __init__(self, vector_column_name: str, embedding_data: VEC, embedding_data_type: str,
                 method_type: str, extra_option: str = None):
        self.vector_column_name = vector_column_name
        self.embedding_data = embedding_data
        self.embedding_data_type = embedding_data_type
        self.method_type = method_type
        self.extra_option = extra_option

DEFAULT_MATCH_VECTOR_TOPN = 10
DEFAULT_MATCH_SPARSE_TOPN = 10
