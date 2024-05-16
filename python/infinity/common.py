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
INSERT_DATA = dict[str, Union[str, int, float, list[Union[int, float]]]]

REMOTE_HOST = NetworkAddress("127.0.0.1", 23817)
LOCAL_HOST = NetworkAddress("0.0.0.0", 23817)


class ConflictType(object):
    Ignore = 0
    Error = 1
    Replace = 2
