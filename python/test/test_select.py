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

import pandas as pd
from numpy import dtype

import infinity
from infinity.common import REMOTE_HOST


class TestSelect:

    def test_version(self):
        print(infinity.__version__)

    def test_infinity_select(self):
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
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")

        # infinity
        db_obj.drop_table("test_infinity_select", True)
        table_obj = db_obj.create_table(
            "test_infinity_select", {"c1": "int, primary key, not null", "c2": "int, not null"}, None)

        assert table_obj is not None

        res = table_obj.insert(
            [{"c1": -3, "c2": -3}, {"c1": -2, "c2": -2}, {"c1": -1, "c2": -1}, {"c1": 0, "c2": 0}, {"c1": 1, "c2": 1},
             {"c1": 2, "c2": 2}, {"c1": 3, "c2": 3}])
        assert res.success

        res = table_obj.insert(
            [{"c1": -8, "c2": -8}, {"c1": -7, "c2": -7}, {"c1": -6, "c2": -6}, {"c1": 7, "c2": 7}, {"c1": 8, "c2": 8},
             {"c1": 9, "c2": 9}])
        assert res.success

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                         'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = table_obj.output(["c1", "c2"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                         'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = table_obj.output(
            ["c1 + c2"]).filter("c1 = 3").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'(c1 + c2)': (6,)})
                                      .astype({'(c1 + c2)': dtype('int32')}))

        res = table_obj.output(
            ["c1"]).filter("c1 > 2 and c2 < 4").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (3,)})
                                      .astype({'c1': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "(-7 < c1 or 9 <= c1) and (c1 = 3)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (3,)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "(-8 < c1 and c1 <= -7) or (c1 >= 1 and 2 > c1)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "((c1 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c1)) and ((c1 > 0 and c1 <= 1) or (c1 > -8 and c1 < -6))").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "(-7 < c1 or 9 <= c1) and (c2 = 3)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (3,)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "(-8 < c1 and c2 <= -7) or (c1 >= 1 and 2 > c2)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        # Need fix rbo caused it Planner Error: Indices must be in order @src/planner/bound/base_table_ref.cppm:45
        # res = table_obj.output(["c2"]).filter(
        #     "((c2 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c2)) and ((c2 > 0 and c1 <= 1) or (c1 > -8 and c2 < -6))").to_df()
        # pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
        #                               .astype({'c2': dtype('int32')}))

        res = db_obj.drop_table("table_1")

    def test_select_varchar(self):
        """
        target: test table select apis
        method:
        1. create tables
            - 'table_1'
                - c1 varchar primary key
                - c2 varchar not null
        2. insert
            - ('a', 'a')
            - ('b', 'b')
            - ('c', 'c')
            - ('d', 'd')
            - ('e', 'e')
            - ('f', 'f')
            - ('g', 'g')
            - ('h', 'h')
            - ('i', 'i')
            - ('j', 'j')
            - ('k', 'k')
            - ('l', 'l')
            - ('m', 'm')
        3. select
            - select * from table_1
                - Consistent with 2. Insert
            - select c1, c2 from table_1
                - Consistent with 2. Insert
            - select c1 + c2 from table_1 where (c1 = 'a')
                - 'aa'
            - select c1 from table_1 where c1 > 'a' and c2 < 'c'
                - 'b'
            - select c2 from table_1 where ('a' < c1 or 'm' <= c1) and (c1 = 'a')
                - 'a'
            - select c2 from table_1 where ('a' < c1 and c1 <= 'b') or (c1 >= 'c' and 'd' > c1)
                - 'b'
                - 'c'
            - select c2 from table_1 where ((c1 >= 'a' and 'd' >= c1) or (c1 >= 'e' and 'j' > c1)) and ((c1 > 'e' and c1 <= 'f') or (c1 > 'a' and c1 < 'c'))
                - 'b'
                - 'c'
            - select c2 from table_1 where ('a' < c1 or 'm' <= c1) and (c2 = 'a')
                - 'a'
            - select c2 from table_1 where ('a' < c1 and c2 <= 'b') or (c1 >= 'c' and 'd' > c2)`
                - 'b'
                - 'c'
        4. drop tables
               - 'table_1'
        expect: all operations successfully

        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_select_varchar", True)
        db_obj.create_table("test_select_varchar", {"c1": "varchar, primary key, not null", "c2": "varchar, not null"},
                            None)
        table_obj = db_obj.get_table("test_select_varchar")
        table_obj.insert(
            [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'},
             {"c1": 'e', "c2": 'e'}, {"c1": 'f', "c2": 'f'}, {
                 "c1": 'g', "c2": 'g'}, {"c1": 'h', "c2": 'h'},
             {"c1": 'i', "c2": 'i'}, {"c1": 'j', "c2": 'j'}, {
                 "c1": 'k', "c2": 'k'}, {"c1": 'l', "c2": 'l'},
             {"c1": 'm', "c2": 'm'}])
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                                'l', 'm'),
                                                         'c2': ('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                                'l', 'm')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res = table_obj.output(
            ["c1", "c2"]).filter("c1 = 'a'").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('a',), 'c2': ('a',)}).astype(
            {'c1': dtype('O'), 'c2': dtype('O')}))

        # TODO NotImplement Error: Not implement: varchar > varchar
        # res = table_obj.output(["c1"]).filter("c1 > 'a' and c2 < 'c'").to_df()
        # pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('b',)}).astype({'c1': dtype('O')}))
        res = db_obj.drop_table("test_select_varchar")

    def test_select_big(self):
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        res = db_obj.drop_table("test_select_big", True)
        db_obj.create_table("test_select_big", {"c1": "varchar, primary key, not null", "c2": "varchar, not null"},
                            None)

        table_obj = db_obj.get_table("test_select_big")

        for i in range(1000):
            table_obj.insert(
                [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'}])

    def test_select_embedding_int32(self):
        """

        TestSelect.test_select_embedding()

        This method tests the functionality of selecting embeddings from a table in the database.

        Parameters:
        None

        Return Type:
        None

        Example Usage:
        test_obj = TestSelect()
        test_obj.test_select_embedding()

        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")

        db_obj.drop_table("test_select_embedding", True)

        res = db_obj.create_table("test_select_embedding", {
            "c1": "int", "c2": "vector,3,int"}, None)

        table_obj = db_obj.get_table("test_select_embedding")

        test_dir = "/tmp/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir, None)
        assert res.success

        res = table_obj.output(["c2"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c2': ([2, 3, 4], [6, 7, 8], [10, 11, 12])}))

        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (1, 5, 9), 'c2': ([2, 3, 4], [6, 7, 8], [10, 11, 12])})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('O')}))

    def test_select_embedding_float(self):
        """
        Method: test_select_embedding_float

        This method performs a series of tests on the `test_select_embedding_float` table in the Infinity database.

        Parameters:
        None

        Returns:
        None

        Example Usage:
        test_select_embedding_float()

        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")

        db_obj.drop_table("test_select_embedding_float", True)

        res = db_obj.create_table("test_select_embedding_float", {
            "c1": "float", "c2": "vector,4,float"}, None)

        table_obj = db_obj.get_table("test_select_embedding_float")

        test_dir = "/tmp/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_float_dim4.csv"
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir, None)
        assert res.success

        res = table_obj.output(["c2"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c2': ([0.1, 0.2, 0.3, -0.2], [0.2, 0.1, 0.3, 0.4],
                    [0.3, 0.2, 0.1, 0.4], [0.4, 0.3, 0.2, 0.1])}))

        res = table_obj.output(["*"]).to_df()
        print(res)

        pd.testing.assert_frame_equal(res,
                                      pd.DataFrame({'c1': (2, 4, 6, 8), 'c2': (
                                          [0.1, 0.2, 0.3, -0.2], [0.2,
                                                                  0.1, 0.3, 0.4],
                                          [0.3, 0.2, 0.1, 0.4], [0.4, 0.3, 0.2, 0.1])})
                                      .astype({'c1': dtype('float32'), 'c2': dtype('O')}))

    def test_select_big_embedding(self):
        """

        Method: test_select_big_embedding

        Description:
        This method performs a series of operations to test the selection of a large embedding from a table. It imports
        data from a CSV file, creates a table, imports the data into the table,
        and then searches for and retrieves a specific column from the table.

        Parameters:
        None

        Return Type:
        None

        Example Usage:
        test_select_big_embedding()
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")

        db_obj.drop_table("test_select_big_embedding", True)

        db_obj.create_table("test_select_big_embedding", {
            "c1": "int", "c2": "vector,3,int"}, None)

        table_obj = db_obj.get_table("test_select_big_embedding")

        test_dir = "/tmp/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        for i in range(1000):
            res = table_obj.import_data(test_csv_dir, None)
            assert res.success

    def test_select_same_output(self):

        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_select_same_output", True)
        db_obj.create_table("test_select_same_output", {
            "c1": "int", "c2": "int"}, None)

        table_obj = db_obj.get_table("test_select_same_output")
        table_obj.insert([{"c1": 1, "c2": 2}])
        print()
        res = table_obj.output(["c1", "c2"]).to_df()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_df()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_df()
        print(res)

    def test_empty_table(self):
        infinity_obj = infinity.connect(REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_empty_table", True)
        db_obj.create_table("test_empty_table", {
            "c1": "int", "c2": "int"}, None)

        table_obj = db_obj.get_table("test_empty_table")
        print()
        res = table_obj.output(["c1", "c2"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c2': ()}).astype(
            {'c1': dtype('int32'), 'c2': dtype('int32')}))
        res = table_obj.output(["c1", "c1"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c1_2': ()}).astype(
            {'c1': dtype('int32'), 'c1_2': dtype('int32')}))
        res = table_obj.output(["c1", "c2", "c1"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c2': (), 'c1_2': ()}).astype(
            {'c1': dtype('int32'), 'c2': dtype('int32'), 'c1_2': dtype('int32')}))
