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


class TestInsert:

    def test_version(self):
        print(infinity.__version__)

    def test_infinity_thrift(self):
        """
        target: test table select apis
        method:
        1. create tables
            - 'table_2'
                - c1 int primary key
                - c2 int null
        2. insert
            - insert into table_2 (c1, c2) values(1, 2)     √
            - insert into table_2 (c2, c1) values(1, 2)     √
            - insert into table_2 (c1) values(3)            √
        3. select all
            - 1, 2
            - 2, 1
            - 3, null
        4. drop tables
            - 'table_2'
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 9090))
        db_obj = infinity_obj.get_database("default")

        # infinity
        res = db_obj.create_table("table_2", {"c1": "int, primary key, not null", "c2": "int, not null"}, None)
        assert res.success

        table_obj = db_obj.get_table("table_2")

        res = table_obj.insert([{"c1": 0, "c2": 0}])
        assert res.success

        res = table_obj.insert([{"c1": 1, "c2": 2}])
        assert res.success

        res = table_obj.insert([{"c2": 1, "c1": 2}])
        assert res.success

        res = table_obj.search().output(["*"]).to_list()
        assert res == {'c1': (1, 2), 'c2': (2, 1)}

        res = db_obj.drop_table("table_2")
        assert res.success

        # disconnect
        res = infinity_obj.disconnect()

        assert res.success
