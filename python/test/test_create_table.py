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
from infinity.common import NetworkAddress, REMOTE_HOST


class TestCreateTable:

    def test_create_varchar_table(self):
        """
        target: test create table with varchar column
        method: create table with varchar column
        expected: ok
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_create_varchar_table", True)
        table_obj = db_obj.create_table("test_create_varchar_table", {
                                  "c1": "varchar, primary key", "c2": "float"}, None)
        assert table_obj

        db_obj.drop_table("test_create_varchar_table")

    def test_create_embedding_table(self):
        """
        target: test create table with embedding column
        method: create table with embedding column
        expected: ok
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_create_embedding_table", True)
        table_obj = db_obj.create_table("test_create_embedding_table", {
                                  "c1": "vector,128,float"}, None)
        assert table_obj

        db_obj.drop_table("test_create_embedding_table")
