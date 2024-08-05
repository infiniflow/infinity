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

from infinity_runner import InfinityRunner
import infinity
from common import common_values
import time
from infinity.common import ConflictType
from infinity.errors import ErrorCode


def test_0(infinity_runner: InfinityRunner):
    infinity_runner.clear()
    for i in range(2):
        print(f"i: {i}")
        infinity_runner.init(None)

        # sleep for 3 second
        time.sleep(3)

        uri = common_values.TEST_LOCAL_HOST
        infinity_obj = infinity.connect(uri)
        db_obj = infinity_obj.create_database("my_db", ConflictType.Error)
        assert db_obj is not None
        res = infinity_obj.drop_database("my_db", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        infinity_obj.disconnect()

        infinity_runner.uninit()
