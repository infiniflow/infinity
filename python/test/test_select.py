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


class TestSelect:

    def test_version(self):
        print(infinity.__version__)

    def test_infinity_thrift(self):
        """
        target: test table select apis
        method:
        1. create tables
            - 'table_1'
                - c1 int primary key
                - c2 int not null
        2. insert
            - (-3, -3)
            - (-2, -2)
            - (-1, -1)
            - (0, 0)
            - (1, 1)
            - (2, 2)
            - (3, 3)
            - (-8, -8)
            - (-7, -7)
            - (-6, -6)
            - (7, 7)
            - (8, 8)
            - (9, 9)
        3. select
            - select * from table_1
                - Consistent with 2. Insert
            - select c1, c2 from table_1
                - Consistent with 2. Insert
            - select c1 + c2 from table_1 where (c1 = 3)
                - 6
            - select c1 from table_1 where c1 > 2 and c2 < 4
                - 3
            - select c2 from table_1 where (-7 < c1 or 9 <= c1) and (c1 = 3)
                - 3
            - select c2 from table_1 where (-8 < c1 and c1 <= -7) or (c1 >= 1 and 2 > c1)
                - -7
                - 1
            - select c2 from table_1 where ((c1 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c1)) and ((c1 > 0 and c1 <= 1) or (c1 > -8 and c1 < -6))
                - -7
                - 1
            - select c2 from table_1 where (-7 < c1 or 9 <= c1) and (c2 = 3)
                - 3
            - select c2 from table_1 where (-8 < c1 and c2 <= -7) or (c1 >= 1 and 2 > c2)
                - -7
                - 1
            - select c2 from table_1 where ((c2 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c2)) and ((c2 > 0 and c1 <= 1) or (c1 > -8 and c2 < -6))
                - -7
                - 1
        4. drop tables
            - 'table_1'
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 9090))
        db_obj = infinity_obj.get_database("default")

        # infinity
        res = db_obj.create_table("table_1", {"c1": "int, primary key, not null", "c2": "int, not null"}, None)
        assert res.success

        table_obj = db_obj.get_table("table_1")

        res = table_obj.insert(
            [{"c1": -3, "c2": -3}, {"c1": -2, "c2": -2}, {"c1": -1, "c2": -1}, {"c1": 0, "c2": 0}, {"c1": 1, "c2": 1},
             {"c1": 2, "c2": 2}, {"c1": 3, "c2": 3}])
        assert res.success

        res = table_obj.insert(
            [{"c1": -8, "c2": -8}, {"c1": -7, "c2": -7}, {"c1": -6, "c2": -6}, {"c1": 7, "c2": 7}, {"c1": 8, "c2": 8},
             {"c1": 9, "c2": 9}])
        assert res.success

        res = table_obj.search().output(["*"]).to_list()
        assert res == {'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                       'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)}

        res = table_obj.search().output(["c1", "c2"]).to_list()
        assert res == {'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                       'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)}

        res = table_obj.search().output(["c1 + c2"]).filter("c1 = 3").to_list()
        assert res == {'(c1 + c2)': (6,)}

        res = table_obj.search().output(["c1"]).filter("c1 > 2 and c2 < 4").to_list()
        assert res == {'c1': (3,)}

        res = table_obj.search().output(["c2"]).filter("(-7 < c1 or 9 <= c1) and (c1 = 3)").to_list()
        assert res == {'c1': (3,)}

        res = table_obj.search().output(["c2"]).filter("(-8 < c1 and c1 <= -7) or (c1 >= 1 and 2 > c1)").to_list()
        assert res == {'c1': (-7, 1)}

        res = table_obj.search().output(["c2"]).filter(
            "((c1 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c1)) and ((c1 > 0 and c1 <= 1) or (c1 > -8 and c1 < -6))").to_list()
        assert res == {'c1': (-7, 1)}

        res = table_obj.search().output(["c2"]).filter("(-7 < c1 or 9 <= c1) and (c2 = 3)").to_list()
        assert res == {'c1': (3,)}

        res = table_obj.search().output(["c2"]).filter("(-8 < c1 and c2 <= -7) or (c1 >= 1 and 2 > c2)").to_list()
        assert res == {'c1': (-7, 1)}

        res = table_obj.search().output(["c2"]).filter(
            "((c2 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c2)) and ((c2 > 0 and c1 <= 1) or (c1 > -8 and c2 < -6))").to_list()
        assert res == {'c1': (-7, 1)}

        res = db_obj.drop_table("table_1")
        assert res.success

        # disconnect
        res = infinity_obj.disconnect()

        assert res.success
