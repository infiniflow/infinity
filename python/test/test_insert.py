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
from infinity.common import REMOTE_HOST


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
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")

        db_obj.drop_table(table_name="table_2", if_exists=True)
        # infinity
        res = db_obj.create_table(
            "table_2", {"c1": "int, primary key, not null", "c2": "int, not null"}, None)
        assert res.success

        table_obj = db_obj.get_table("table_2")

        res = table_obj.insert([{"c1": 0, "c2": 0}])
        assert res.success

        res = table_obj.insert([{"c1": 1, "c2": 1}])
        assert res.success

        res = table_obj.insert({"c2": 2, "c1": 2})
        assert res.success

        res = table_obj.insert([{"c2": 3, "c1": 3}, {"c1": 4, "c2": 4}])
        assert res.success

        res = table_obj.query_builder().output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (0, 1, 2, 3, 4), 'c2': (0, 1, 2, 3, 4)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = db_obj.drop_table("table_2")
        assert res.success

        # disconnect
        res = infinity_obj.disconnect()

        assert res.success

    def test_insert_varchar(self):
        """
        target: test insert varchar column
        method: create table with varchar column
        expected: ok
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_varchar", True)
        res = db_obj.create_table("test_insert_varchar", {
            "c1": "varchar"}, None)
        assert res.success
        table_obj = db_obj.get_table("test_insert_varchar")
        assert table_obj
        res = table_obj.insert([{"c1": "test_insert_varchar"}])
        assert res.success
        res = table_obj.insert([{"c1": " test insert varchar "}])
        assert res.success
        res = table_obj.insert([{"c1": "^789$ test insert varchar"}])
        assert res.success

        res = table_obj.query_builder().output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ("test_insert_varchar", " test insert varchar ",
                                                                "^789$ test insert varchar")}))
        db_obj.drop_table("test_insert_varchar")

    def test_insert_big_varchar(self):
        """
        target: test insert varchar with big length
        method: create table with varchar column
        expected: ok
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_big_varchar", True)
        res = db_obj.create_table("test_insert_big_varchar", {
            "c1": "varchar"}, None)
        assert res.success
        table_obj = db_obj.get_table("test_insert_big_varchar")
        assert table_obj
        for i in range(100):
            res = table_obj.insert([{"c1": "test_insert_big_varchar" * 1000}])
            assert res.success

        res = table_obj.query_builder().output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ["test_insert_big_varchar" * 1000] * 100}))

        db_obj.drop_table("test_insert_big_varchar")

    def test_insert_embedding(self):
        """
        target: test insert embedding column
        method: create table with embedding column
        expected: ok
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_embedding", True)
        res = db_obj.create_table("test_insert_embedding", {
            "c1": "vector,3,int"}, None)
        assert res.success
        table_obj = db_obj.get_table("test_insert_embedding")
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.success
        res = table_obj.insert([{"c1": [4, 5, 6]}])
        assert res.success
        res = table_obj.insert([{"c1": [7, 8, 9]}])
        assert res.success
        res = table_obj.insert([{"c1": [-7, -8, -9]}])
        assert res.success
        res = table_obj.query_builder().output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9]}, {"c1": [-7, -8, -9]}])
        assert res.success
        res = table_obj.query_builder().output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9],
                                                                [1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))

        db_obj.drop_table("test_insert_embedding_2", True)
        db_obj.create_table("test_insert_embedding_2", {
            "c1": "vector,3,float"}, None)
        table_obj = db_obj.get_table("test_insert_embedding_2")
        assert table_obj
        res = table_obj.insert([{"c1": [1.1, 2.2, 3.3]}])
        assert res.success
        res = table_obj.insert([{"c1": [4.4, 5.5, 6.6]}])
        assert res.success
        res = table_obj.insert([{"c1": [7.7, 8.8, 9.9]}])
        assert res.success
        res = table_obj.insert([{"c1": [-7.7, -8.8, -9.9]}])
        assert res.success

        res = table_obj.query_builder().output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([1.1, 2.2, 3.3], [4.4, 5.5, 6.6], [7.7, 8.8, 9.9], [-7.7, -8.8, -9.9])}))

        db_obj.drop_table("test_insert_embedding2")

    def test_insert_big_embedding(self):
        """
        target: test insert embedding with big dimension
        method: create table with embedding column
        expected: ok
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_big_embedding", True)
        res = db_obj.create_table("test_insert_big_embedding", {
            "c1": "vector,65535,int"}, None)
        table_obj = db_obj.get_table("test_insert_big_embedding")
        assert table_obj
        res = table_obj.insert([{"c1": [1] * 65535}])
        assert res.success
        res = table_obj.insert([{"c1": [4] * 65535}])
        assert res.success
        res = table_obj.insert([{"c1": [7] * 65535}])
        assert res.success
        res = table_obj.insert([{"c1": [-9999999] * 65535}])
        assert res.success

    def test_insert_big_embedding_float(self):
        """
        target: test insert embedding float with big dimension
        method: create table with embedding column
        expected: ok
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_big_embedding_float", True)
        res = db_obj.create_table("test_insert_big_embedding_float", {
            "c1": "vector,65535,float"}, None)
        assert res.success
        table_obj = db_obj.get_table("test_insert_big_embedding_float")
        assert table_obj
        res = table_obj.insert([{"c1": [1] * 65535}])
        assert res.success
        res = table_obj.insert([{"c1": [-9999999] * 65535}])
        assert res.success
        res = table_obj.insert([{"c1": [1.1] * 65535}])
        assert res.success
        res = table_obj.insert([{"c1": [-9999999.988] * 65535}])
        assert res.success
        db_obj.drop_table("test_insert_big_embedding_float")
