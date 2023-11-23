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

import importlib.metadata
__version__ = importlib.metadata.version("infinity")

from infinity.infinity import URI, InfinityConnection, NetworkAddress
from infinity.remote_grpc.infinity import RemoteInfinityConnection
from infinity.remote_thrift.infinity import RemoteThriftInfinityConnection


def connect(
        uri: URI,
) -> InfinityConnection:
    if isinstance(uri, NetworkAddress) and uri.port == 50002:
        return RemoteInfinityConnection(uri)
    if isinstance(uri, NetworkAddress) and uri.port == 9090:
        return RemoteThriftInfinityConnection(uri)
    else:
        raise Exception(f"unknown uri: {uri}")


