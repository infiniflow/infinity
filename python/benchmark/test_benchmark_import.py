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
from infinity.infinity import NetworkAddress


class TestImport:

    def test_import(self):
        """
        target: test import data to remote server
        method: connect server, create table, import data, search, drop table, disconnect
        expect: all operations successfully
        """

        infinity_obj = infinity.connect(NetworkAddress('192.168.200.151', 9080))
        assert infinity_obj

        # infinity

        db_obj = infinity_obj.get_database("default")
        assert db_obj

        # import
        db_obj.create_table("benchmark", {"c1": "vector,128,float"}, None)
        table_obj = db_obj.get_table("benchmark")
        assert table_obj

        parent_dir = os.path.dirname(os.path.dirname(os.getcwd()))
        test_fvecs_dir = parent_dir + "/sift/sift/base.fvecs"
        assert os.path.exists(test_fvecs_dir)


        res = table_obj.import_data(test_fvecs_dir, None)
        assert res.success

    def test_search(self):
        """
        target: test search
        method: connect server, create table, import data, search, drop table, disconnect
        expect: all operations successfully
        """

        infinity_obj = infinity.connect(NetworkAddress('192.168.200.151', 9080))
        assert infinity_obj

        # infinity

        db_obj = infinity_obj.get_database("default")

        table_obj = db_obj.get_table("benchmark")

        # search ordinary
        table_obj.search().output(["*"]).to_df()


