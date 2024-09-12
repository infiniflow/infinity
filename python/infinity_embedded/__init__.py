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

# import importlib.metadata
#
# __version__ = importlib.metadata.version("infinity_sdk")

import os
# import pkg_resources
# __version__ = pkg_resources.get_distribution("infinity_sdk").version

from infinity_embedded.common import URI, NetworkAddress, LOCAL_HOST, LOCAL_INFINITY_PATH, InfinityException
from infinity_embedded.infinity import InfinityConnection
from infinity_embedded.local_infinity.infinity import LocalInfinityConnection
from infinity_embedded.errors import ErrorCode

def connect(uri = LOCAL_INFINITY_PATH) -> InfinityConnection:
    if isinstance(uri, str) and len(uri) != 0:
        return LocalInfinityConnection(uri)
    else:
        raise InfinityException(ErrorCode.INVALID_SERVER_ADDRESS, f"Unknown uri: {uri}")
