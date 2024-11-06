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

import infinity
import pytest
from infinity.errors import ErrorCode
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.connection_pool import ConnectionPool


from common import common_values
import logging


@pytest.fixture(scope="function")
def connect_infinity():
    return infinity.connect(common_values.TEST_LOCAL_HOST)


@pytest.fixture(scope="function")
def disconnect_infinity():
    res = ThriftInfinityClient(common_values.TEST_LOCAL_HOST).disconnect()
    assert res.error_code == ErrorCode.OK


@pytest.fixture(scope="function")
def get_infinity_db():
    # connect
    infinity_obj = infinity.connect(common_values.TEST_LOCAL_HOST)

    yield infinity_obj.get_database("default_db")

    # disconnect
    res = infinity_obj.disconnect()
    assert res.error_code == ErrorCode.OK

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
        os.makedirs(data_dir, exist_ok=True)
        return False
    if not os.path.exists(data_dir + file_name):
        return False
    return True


def disable_items_with_mark(items, mark, reason):
    skipper = pytest.mark.skip(reason=reason)
    for item in items:
        if mark in item.keywords:
            item.add_marker(skipper)


def pytest_addoption(parser):
    parser.addoption(
        "--run-integration",
        action="store_true",
        default=False,
        help="Run integration tests (requires S3 buckets to be setup with access)",
    )

log_output_file = "run_parallel_test.log"

def pytest_configure(config):
    logger = logging.getLogger("run_parallel_test")
    logger.setLevel(logging.INFO)
    handler = logging.FileHandler(log_output_file)
    logger.addHandler(handler)
    logger.addHandler(logging.StreamHandler())
    formatter = logging.Formatter('%(asctime)s - %(threadName)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
