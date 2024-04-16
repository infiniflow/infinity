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

from sqlglot import condition

from infinity.remote_thrift.table import traverse_conditions
from test_sdkbase import TestSdk

class TestCondition(TestSdk):

    def test_traverse_conditions(self):
        res = (condition("c1 > 1 and c2 < 2 or c3 = 3.3"))
        print(res)
        res = traverse_conditions(condition("c1 = 1"))
        print(res)
        res = traverse_conditions(condition("-8 < c1 and c1 <= -7"))
        print(res)
        res = traverse_conditions(
            condition("(-7 < c1 or 9 <= c1) and (c1 = 3)"))
        print(res)
