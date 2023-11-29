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

import infinity
from infinity.infinity import NetworkAddress


class TestDelete:

    def test_version(self):
        print(infinity.__version__)

    def test_infinity_thrift(self):
        """
        target: test table update apis
        method:
        1. create tables
            - 'table_4'
                - c1 int primary key
                - c2 int
                - c3 int
        2. insert
            - (1, 10, 100)
            - (2, 20, 200)
            - (3, 30, 300)
            - (4, 40, 400)
        3. delete
            - update table_4 set c2 = 90, c3 = 900 where c1 = 1
            - after:
                - (1, 90, 900)
                - (2, 20, 200)
                - (3, 30, 300)
                - (4, 40, 400)
            - update table_4 set c2 = 80, c3 = 800
            - after:
                - (1, 80, 800)
                - (2, 80, 800)
                - (3, 80, 800)
                - (4, 80, 800)
        4. drop tables
            - 'table_4'
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 9090))
        db_obj = infinity_obj.get_database("default")

        # infinity
        res = db_obj.create_table("table_4", {"c1": "int, primary key, not null", "c2": "int", "c3": "int"}, None)
        assert res.success

        table_obj = db_obj.get_table("table_4")

        res = table_obj.insert(
            [{"c1": 1, "c2": 10, "c3": 100}, {"c1": 2, "c2": 20, "c3": 200}, {"c1": 3, "c2": 30, "c3": 300},
             {"c1": 4, "c2": 40, "c3": 400}])
        assert res.success

        res = table_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
        assert res.success

        res = table_obj.search().output(["*"]).to_list()
        assert res == {'c1': (1, 2, 3, 4), 'c2': (90, 20, 30, 40), 'c3': (900, 200, 300, 400)}

        res = table_obj.update([{"c2": 80, "c3": 800}])
        assert res.success

        res = table_obj.search().output(["*"]).to_list()
        assert res == {'c1': (1, 2, 3, 4), 'c2': (80, 80, 80, 80), 'c3': (800, 800, 800, 800)}

        res = db_obj.drop_table("table_4")
        assert res.success

        # disconnect
        res = infinity_obj.disconnect()

        assert res.success
