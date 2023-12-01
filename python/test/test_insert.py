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
import pandas as pd
from numpy import dtype

import infinity
from infinity.infinity import NetworkAddress


class TestInsert:

    def test_version(self):
        print(infinity.__version__)

    def test_insert_basic(self):
        """
        target: test table insert apis
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
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 9080))
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

        res = table_obj.search().output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (0, 1, 2), 'c2': (0, 2, 1)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = db_obj.drop_table("table_2")
        assert res.success

        # disconnect
        res = infinity_obj.disconnect()

        assert res.success

    def test_insert_varchar(self):
        """
        target: test insert varchar
        method: create table with varchar column
        expected: ok
        """
        infinity_obj = infinity.connect(NetworkAddress('192.168.200.151', 9080))
        db_obj = infinity_obj.get_database("default")
        res = db_obj.create_table("test_insert_varchar", {"c1": "varchar"}, None)
        assert res.success
        table_obj = db_obj.get_table("test_insert_varchar")
        assert table_obj
        res = table_obj.insert([{"c1": "test_insert_varchar"}])
        assert res.success
        res = table_obj.insert([{"c1": " test insert varchar "}])
        assert res.success
        res = table_obj.insert([{"c1": "^789$ test insert varchar"}])
        assert res.success

        res = table_obj.search().output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ("test_insert_varchar", " test insert varchar ",
                                                                "^789$ test insert varchar")}))
        db_obj.drop_table("test_insert_varchar")