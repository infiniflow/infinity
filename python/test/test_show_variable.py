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

import common_values
import infinity
from infinity.infinity import ShowVariable


class TestShowVariable:

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
