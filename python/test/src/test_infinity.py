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

from infinity.remote_thrift.client import ThriftInfinityClient

from common import common_values
from src.test_sdkbase import TestSdk

class TestInfinity(TestSdk):

    def _test_get_database(self):
        infinity_obj = ThriftInfinityClient(common_values.TEST_REMOTE_HOST)
        database_res = infinity_obj.get_database("default_db")
        print(type(database_res))
        assert database_res.error_code == ErrorCode.OK
        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def _test_list_database(self):
        infinity_obj = infinity.connect(self.uri)
        database_res = infinity_obj.list_databases()
        assert database_res.db_names[0] == "default_db"

