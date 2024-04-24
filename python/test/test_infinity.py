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
import time

import infinity
import pytest
from infinity.errors import ErrorCode

from infinity.infinity import ShowVariable
from infinity.remote_thrift.client import ThriftInfinityClient

from common import common_values
from test_sdkbase import TestSdk

class TestInfinity(TestSdk):

    def test_get_database(self):
        infinity_obj = ThriftInfinityClient(common_values.TEST_REMOTE_HOST)
        database_res = infinity_obj.get_database("default_db")
        assert database_res.error_code == ErrorCode.OK
        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_list_database(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        database_res = infinity_obj.list_databases()
        assert database_res.db_names[0] == "default_db"

    def test_show_variable(self):
        # QUERY_COUNT = "query_count"
        # SESSION_COUNT = "session_count"
        # BUFFER_POOL_USAGE = "buffer_pool_usage"
        # VERSION = "version"
        # QUERY_MEMORY_LIMIT = "query_memory_limit"
        # QUERY_CPU_LIMIT = "query_cpu_limit"
        # LOG_LEVEL = "log_level"
        # SCHEDULE_POLICY = "schedule_policy"
        # LISTEN_ADDRESS = "listen_address"
        # SQL_PORT = "sql_port"
        # SDK_PORT = "sdk_port"
        # HTTP_API_PORT = "http_api_port"
        # DATA_URL = "data_url"
        # TIME_ZONE = "time_zone"
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        res = infinity_obj.show_variable(ShowVariable.QUERY_COUNT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.SESSION_COUNT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.BUFFER_POOL_USAGE)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.VERSION)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.QUERY_MEMORY_LIMIT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.QUERY_CPU_LIMIT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.LOG_LEVEL)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.SCHEDULE_POLICY)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.LISTEN_ADDRESS)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.SQL_PORT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.SDK_PORT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.HTTP_API_PORT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.DATA_URL)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.TIME_ZONE)
        print(res)

    @pytest.mark.slow
    def test_timeout_infinity(self):
        infinity_obj = ThriftInfinityClient(common_values.TEST_REMOTE_HOST)
        time.sleep(3600)
        database_res = infinity_obj.get_database("default_db")
        print(database_res)
        res = infinity_obj.show_variable(ShowVariable.QUERY_COUNT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.SESSION_COUNT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.BUFFER_POOL_USAGE)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.VERSION)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.QUERY_MEMORY_LIMIT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.QUERY_CPU_LIMIT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.LOG_LEVEL)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.SCHEDULE_POLICY)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.LISTEN_ADDRESS)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.SQL_PORT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.SDK_PORT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.HTTP_API_PORT)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.DATA_URL)
        print(res)

        res = infinity_obj.show_variable(ShowVariable.TIME_ZONE)
        print(res)
