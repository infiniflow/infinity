# Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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
from dataclasses import dataclass, field
import numpy as np


class NetworkAddress:
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port

    def __str__(self):
        return f'IP: {self.ip}, Port: {self.port}'


@dataclass
class SparseVector:
    indices: list[int]
    values: Union[list[float], list[int], None] = None

    def __post_init__(self):
        assert (self.values is None) or (len(self.indices) == len(self.values))

    def to_dict_old(self):
        d = {"indices": self.indices}
        if self.values is not None:
            d["values"] = self.values
        return d

    def to_dict(self):
        if self.values is None:
            raise ValueError("SparseVector.values is None")
        result = {}
        for i, v in zip(self.indices, self.values):
            result[str(i)] = v
        return result

    @staticmethod
    def from_dict(d):
        return SparseVector(d["indices"], d.get("values"))

    def __str__(self):
        return f"SparseVector(indices={self.indices}{'' if self.values is None else f', values={self.values}'})"

    def __repr__(self):
        return str(self)


@dataclass
class Array:
    elements: list = field(default_factory=list)

    def append(self, element):
        self.elements.append(element)

    def __init__(self, *args):
        self.elements = list(args)

    def __str__(self):
        return f"Array({', '.join(str(e) for e in self.elements)})"

    def __repr__(self):
        return str(self)


URI = Union[NetworkAddress, Path]
VEC = Union[list, np.ndarray]
INSERT_DATA = dict[str, Union[str, int, float, list[Union[int, float]]], SparseVector, dict, Array]

LOCAL_HOST = NetworkAddress("127.0.0.1", 23817)

# test embedded_infinity
LOCAL_INFINITY_PATH = "/var/infinity"
LOCAL_INFINITY_CONFIG_PATH = "conf/pytest_embedded_infinity_conf.toml"


class ConflictType(object):
    Ignore = 0
    Error = 1
    Replace = 2


class SortType(object):
    Asc = 0
    Desc = 1


class InfinityException(Exception):
    def __init__(self, error_code=0, error_message=None):
        self.error_code = error_code
        self.error_message = error_message


DEFAULT_MATCH_VECTOR_TOPN = 10
DEFAULT_MATCH_SPARSE_TOPN = 10
