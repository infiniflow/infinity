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

import os

import infinity
from infinity.common import NetworkAddress


class TestImport:

    def test_version(self):
        print(infinity.__version__)

    def test_import(self):
        """
        target: test import data to remote server
        method: connect server, create table, import data, search, drop table, disconnect
        expect: all operations successfully
        """
        ports = [9080]
        for port in ports:
            infinity_obj = infinity.connect(NetworkAddress('192.168.200.151', 9080))
            assert infinity_obj

            # infinity

            db_obj = infinity_obj.get_database("default")
            assert db_obj

            # import
            res = db_obj.create_table("my_table4", {"c1": "int", "c2": "vector,3,int"}, None)
            assert res.success
            table_obj = db_obj.get_table("my_table4")
            assert table_obj

            parent_dir = os.path.dirname(os.path.dirname(os.getcwd()))
            test_csv_dir = parent_dir + "/test/data/csv/embedding_int_dim3.csv"
            assert os.path.exists(test_csv_dir)

            res = table_obj.import_data(test_csv_dir, None)
            assert res.success

            # search
            res = table_obj.search().output(["c1"]).filter("c1 > 1").to_df()
            print(res)
            res = db_obj.drop_table("my_table4")
            assert res.success

            # disconnect
            res = infinity_obj.disconnect()
            assert res.success
