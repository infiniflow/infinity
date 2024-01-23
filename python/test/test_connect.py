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

import pytest

import infinity
from infinity.common import REMOTE_HOST
from infinity.common import NetworkAddress

class TestConnection:
    def test_connect_and_disconnect_ok(self):
        """
        target: test connect and server ok
        method: connect server
        expected: ok
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        assert infinity_obj
        assert infinity_obj.disconnect()

    @pytest.mark.level(2)
    @pytest.mark.timeout(5)
    def test_connect_invalid_address(self):
        """
        target: test connect invalid address, wrong address, invalid port, wrong port
        method: connect server with (invalid/wrong address and invalid/wrong port)
        expected: failed
        """

        INVALID_IP_HOST = NetworkAddress("127.0.0.1111", 23817)
        WRONG_IP_HOST = NetworkAddress("192.168.1.255", 23817)

        INVALID_PORT_HOST = NetworkAddress("127.0.0.1", -1)
        WRONG_PORT_HOST = NetworkAddress("127.0.0.1", 23815)

        try:
            infinity_instance = infinity.connect(INVALID_IP_HOST)
        except Exception as e:
            print(e)

        # try:
        #     # how to speed up the timeout connection?
        #     infinity_instance = infinity.connect(WRONG_IP_HOST)
        # except Exception as e:
        #     print(e)


        try:
            infinity_instance = infinity.connect(INVALID_PORT_HOST)
        except Exception as e:
            print(e)

        try:
            infinity_instance = infinity.connect(WRONG_PORT_HOST)
        except Exception as e:
            print(e)


        INVALID_PORT_LOCAL_HOST = NetworkAddress("0.0.0.0", -1)
        WRONG_PORT_LOCAL_HOST = NetworkAddress("0.0.0.0", 23815)

        try:
            infinity_instance = infinity.connect(INVALID_PORT_LOCAL_HOST)
        except Exception as e:
            print(e)

        try:
            infinity_instance = infinity.connect(WRONG_PORT_LOCAL_HOST)
        except Exception as e:
            print(e)

    def test_repeat_connect(self):
        """
        target: disconnect the infinity which is already disconnected.
        method: connect server -> connect server
        expected: success
        """
        infinity_instance = infinity.connect(REMOTE_HOST)
        infinity_instance = infinity.connect(REMOTE_HOST)

    def test_multiple_connect(self):
        """
        target: disconnect the infinity which is already disconnected.
        method: multiple connection to server
        expected: success
        """
        infinity_instance1 = infinity.connect(REMOTE_HOST)
        infinity_instance2 = infinity.connect(REMOTE_HOST)
        infinity_instance3 = infinity.connect(REMOTE_HOST)
        infinity_instance4 = infinity.connect(REMOTE_HOST)
        infinity_instance1.disconnect()
        infinity_instance2.disconnect()
        infinity_instance3.disconnect()
        infinity_instance4.disconnect()

    def test_repeat_disconnect(self):
        """
        target: disconnect the infinity which is already disconnected.
        method: connect server -> disconnect server -> disconnect server
        expected: failed
        """
        infinity_instance = infinity.connect(REMOTE_HOST)
        infinity_instance.disconnect()

        try:
            infinity_instance.disconnect()
        except Exception as e:
            print(e)

# possible cases
# 1. connections are more than connection limit defined by infinity variables.
# 2. Concurrently run DDL/DML and disconnect in different thread
