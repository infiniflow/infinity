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
import os
from shutil import copyfile
import sys

current_path = os.path.abspath(os.path.dirname(__file__))
parent = os.path.join(os.getcwd(), os.pardir)
pparent = os.path.join(parent, os.pardir)
local_infinity_path = os.path.abspath(pparent)
current_python_path = os.path.abspath(pparent) + '/python'

print(current_path, local_infinity_path)

if local_infinity_path in sys.path:
    sys.path.remove(local_infinity_path)

if current_python_path in sys.path:
    sys.path.remove(current_python_path)

print(sys.path)

import infinity
import pytest
from infinity.errors import ErrorCode
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.connection_pool import ConnectionPool


from common import common_values


@pytest.fixture(scope="function")
def connect_infinity():
    return infinity.connect(common_values.TEST_LOCAL_HOST)


@pytest.fixture(scope="function")
def disconnect_infinity():
    res = ThriftInfinityClient(common_values.TEST_LOCAL_HOST).disconnect()
    assert res.error_code == ErrorCode.OK


@pytest.fixture(scope="function")
def get_infinity_db_param(request):
    uri = request.param if hasattr(request, 'param') else common_values.TEST_LOCAL_HOST
    # connect
    infinity_obj = infinity.connect(uri)

    yield infinity_obj.get_database("default_db")

    # disconnect
    res = infinity_obj.disconnect()
    assert res.error_code == ErrorCode.OK

@pytest.fixture(scope="function")
def get_infinity_db(request):
    if request.config.getoption("--local-infinity"):
        uri = common_values.TEST_LOCAL_PATH
    else:
        uri = common_values.TEST_LOCAL_HOST

    # connect
    infinity_obj = infinity.connect(uri)
    yield infinity_obj.get_database("default_db")

    # disconnect
    res = infinity_obj.disconnect()
    assert res.error_code == ErrorCode.OK

@pytest.fixture(scope="function")
def skip_if_local_infinity(request):
    if request.config.getoption("--local-infinity"):
        pytest.skip("Skipping local-infinity test")
    
@pytest.fixture(scope="function")
def skip_if_remote_infinity(request):
    if not request.config.getoption("--local-infinity"):
        pytest.skip("Skipping remote-infinity test")

@pytest.fixture(scope="function")
def skip_if_http(request):
    if request.config.getoption("--http"):
        pytest.skip("Skipping http test")

@pytest.fixture(scope="function", autouse=False)
def get_infinity_connection_pool():
    connection_pool = ConnectionPool(common_values.TEST_LOCAL_HOST)
    yield connection_pool
    connection_pool.destroy()


@pytest.fixture(scope="class")
def check_data(request):
    file_name = request.param["file_name"]
    data_dir = request.param["data_dir"]
    # path not exists
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)
        return False
    if not os.path.exists(data_dir + file_name):
        return False
    return True


def disable_items_with_mark(items, mark, reason):
    skipper = pytest.mark.skip(reason=reason)
    for item in items:
        if mark in item.keywords:
            item.add_marker(skipper)

@pytest.fixture
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture
def http(request):
    return request.config.getoption("--http")

def pytest_addoption(parser):
    parser.addoption(
        "--run-integration",
        action="store_true",
        default=False,
        help="Run integration tests (requires S3 buckets to be setup with access)",
    )

    parser.addoption(
        "--local-infinity",
        action="store_true",
        default=False,
        help="Run local infinity tests (default is remote)",
    )

    parser.addoption(
        "--http",
        action="store_true",
        default=False,
        help="Run http api infinity tests (default is remote)",
    )