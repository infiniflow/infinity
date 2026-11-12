# Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

"""The SDK maps a type-3 frame to InfinityException(PROTOCOL_ERROR) and does not retry.

`TApplicationException` (message type 3) is what `volo-thrift` sends when it
cannot decode a request or does not know the method (`PARALLEL_ASYNC.md` §6.4 /
O12). It is a protocol failure, so retrying the same bytes cannot help and a
write may already have run - the SDK must surface it and stop. The stub service
counts the calls, so a retry would be caught.
"""

import logging

import pytest
from infinity import NetworkAddress
from infinity.common import InfinityException
from infinity.errors import ErrorCode
from infinity.remote_thrift import client as sdk_client
from readerwriterlock import rwlock
from thrift.Thrift import TApplicationException


class _ProtocolFailingService:
    """Every call raises the frame `volo-thrift` sends for a bad request."""

    def __init__(self, kind, message):
        self.calls = 0
        self.kind = kind
        self.message = message

    def CreateDatabase(self, request):
        self.calls += 1
        raise TApplicationException(self.kind, self.message)


def _client(service):
    """A `ThriftInfinityClient` wired to `service` without a socket."""
    client = sdk_client.ThriftInfinityClient.__new__(sdk_client.ThriftInfinityClient)
    client.lock = rwlock.RWLockWrite()
    client.session_id = 1
    client.uri = NetworkAddress("127.0.0.1", 1)
    client.transport = None
    client._is_connected = False
    client.session_i = 0
    client.try_times = 10
    client.logger = logging.getLogger("test-protocol-error")
    client.client = service
    return client


@pytest.mark.parametrize(
    "kind,message",
    [
        (TApplicationException.UNKNOWN_METHOD, "unknown method: BogusMethod"),
        (
            TApplicationException.PROTOCOL_ERROR,
            "invalid thrift version: unknown protocol, first byte: 222",
        ),
    ],
)
def test_a_protocol_error_is_not_retried(kind, message):
    service = _ProtocolFailingService(kind, message)
    client = _client(service)
    with pytest.raises(InfinityException) as raised:
        client.create_database("x")
    assert raised.value.error_code == ErrorCode.PROTOCOL_ERROR
    assert message in raised.value.error_msg
    assert service.calls == 1, "a protocol error must not be retried"
