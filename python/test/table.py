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


class TestTable:

    def test_version(self):
        print(infinity.__version__)

    def test_infinity_thrift(self):
        """
        target: test table apis
        method:
        1. create tables
            - 'my_table'        √
                - c1 int primary key
                - c2 float
            - 'my_table!@#'     √
                - c1 int primary key
                - c2 float
            - 'my-table-dash'   √
                - c1 float primary key
                - c2 int
            - '123_table'       √
                - c1 int primary key
                - c2 float
            - 'bad_column'      ❌
                - 123 int primary key
                - c2 float
            - ''                ❌
                - c1 int primary key
                - c2 float
        2. list tables
            - 'my_table'
            - 'my_table!@#'
            - 'my-table_dash'
            - '123_table'
        3. drop tables
            - 'my_table'
            - 'my_table!@#'
            - 'my-table-dash'
            - '123_table'
        4. list tables: empty
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 9090))
        db_obj = infinity_obj.get_database("default")

        # infinity
        res = db_obj.create_table("my_table", {"c1": "int, primary key", "c2": "float"}, None)
        assert res.success

        res = db_obj.create_table("my_table!@#", {"c1": "int, primary key", "c2": "float"}, None)
        assert res.success

        res = db_obj.create_table("my-table-dash", {"c1": "float, primary key", "c2": "int"}, None)
        assert res.success

        res = db_obj.create_table("123_table", {"c1": "int, primary key", "c1": "float"}, None)
        assert res.success

        # FIXME:
        # res = db_obj.create_table("bad_column", {"123": "int, primary key", "c2": "float"}, None)
        # assert not res.success

        res = db_obj.create_table("", {"c1": "int, primary key", "c2": "float"}, None)
        assert not res.success

        # FIXME: res = db_obj.describe_table("my_table")

        res = db_obj.list_tables('default')
        assert res.success

        res.table_names.sort()

        assert res.table_names[0] == '123_table'
        assert res.table_names[1] == 'my_table'
        assert res.table_names[2] == 'my-table-dash'
        assert res.table_names[3] == 'my_table!@#'

        res = db_obj.drop_table("123_table")
        assert res.success
        res = db_obj.drop_table("my_table")
        assert res.success
        res = db_obj.drop_table("my-table-dash")
        assert res.success
        res = db_obj.drop_table("my_table!@#")
        assert res.success

        res = db_obj.list_tables()
        assert len(res.table_names) == 0

        # disconnect
        res = infinity_obj.disconnect()

        assert res.success