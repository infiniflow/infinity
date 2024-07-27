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
import signal
import time

import pandas as pd
import pytest
import numpy as np
from numpy import dtype

from common import common_values
import infinity
import infinity.index as index
from infinity.common import ConflictType, InfinityException
from infinity.errors import ErrorCode
from internal.utils import start_infinity_service_in_subporcess
from internal.test_sdkbase import TestSdk


class TestInsert(TestSdk):

    # def _test_version(self):
    #     print(infinity.__version__)

    def _test_insert_basic(self):
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
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table(table_name="table_2",
                          conflict_type=ConflictType.Ignore)
        # infinity
        table_obj = db_obj.create_table(
            "table_2", {
                "c1": {"type": "int", "constraints": ["primary key", "not null"]},
                "c2": {"type": "int", "constraints": ["not null"]}
            },
            ConflictType.Error)
        assert table_obj is not None

        res = table_obj.insert([{"c1": 0, "c2": 0}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert([{"c1": 1, "c2": 1}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert({"c2": 2, "c1": 2})
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert([{"c2": 3, "c1": 3}, {"c1": 4, "c2": 4}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (0, 1, 2, 3, 4), 'c2': (0, 1, 2, 3, 4)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = db_obj.drop_table("table_2")
        assert res.error_code == ErrorCode.OK

    def _test_insert_float16_bfloat16(self):
        """
        target: test insert float16 bfloat16 column
        method: create table with float16 bfloat16 column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("python_test_fp16_bf16", ConflictType.Ignore)
        table_obj = db_obj.create_table("python_test_fp16_bf16", {"c1": {"type": "float"}, "c2": {"type": "float16"},
                                                                  "c3": {"type": "bfloat16"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert(
            [{"c1": -1, "c2": 1, "c3": -1}, {"c1": 2, "c2": -2, "c3": 2}, {"c1": -3, "c2": 3, "c3": -3},
             {"c1": 4, "c2": -4, "c3": 4}, {"c1": -5, "c2": 5, "c3": -5}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (-1, 2, -3, 4, -5), 'c2': (1, -2, 3, -4, 5), 'c3': (-1, 2, -3, 4, -5)}).astype(
            {'c1': dtype('float32'), 'c2': dtype('float32'), 'c3': dtype('float32')}))
        res = db_obj.drop_table("python_test_fp16_bf16", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_bool(self):
        """
        target: test insert bool column
        method: create table with bool column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("python_test_bool_insert", ConflictType.Ignore)
        table_obj = db_obj.create_table("python_test_bool_insert", {"c1": {"type": "float"}, "c2": {"type": "bool"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": -1, "c2": True}, {"c1": 2, "c2": False}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-1, 2), 'c2': (True, False)}).astype(
            {'c1': dtype('float32'), 'c2': dtype('bool')}))
        res = db_obj.drop_table("python_test_bool_insert", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        db_obj.drop_table("python_test_bool_insert_default", ConflictType.Ignore)
        table_instance = db_obj.create_table("python_test_bool_insert_default", {
            "c1": {"type": "int8", "default": 0},
            "c2": {"type": "int16", "default": 0},
            "c3": {"type": "int", "default": 0},
            "c4": {"type": "int32", "default": 0},  # Same as int
            "c5": {"type": "integer", "default": 0},  # Same as int
            "c6": {"type": "int64", "default": 0},
            "c7": {"type": "varchar"},
            "c8": {"type": "float", "default": 1.0},
            "c9": {"type": "float32", "default": 1.0},  # Same as float
            "c10": {"type": "double", "default": 1.0},
            "c11": {"type": "float64", "default": 1.0},  # Same as double
            "c12": {"type": "bool", "default": False}
        })
        assert table_instance
        res = table_instance.insert({"c1": 1, "c7": "Tom"})
        assert res.error_code == ErrorCode.OK
        res = table_instance.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (1,), 'c2': (0,), 'c3': (0,), 'c4': (0,), 'c5': (0,), 'c6': (0,), 'c7': ("Tom",), 'c8': (1.0,),
             'c9': (1.0,), 'c10': (1.0,), 'c11': (1.0,), 'c12': (False,)}).astype(
            {'c1': dtype('int8'), 'c2': dtype('int16'), 'c3': dtype('int32'), 'c4': dtype('int32'),
             'c5': dtype('int32'), 'c6': dtype('int64'), 'c7': dtype('object'), 'c8': dtype('float32'),
             'c9': dtype('float32'), 'c10': dtype('float64'), 'c11': dtype('float64'), 'c12': dtype('bool')}))
        res = db_obj.drop_table("python_test_bool_insert_default", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_varchar(self):
        """
        target: test insert varchar column
        method: create table with varchar column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_varchar", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_varchar", {"c1": {"type": "varchar"}}, ConflictType.Error)
        assert table_obj

        res = table_obj.insert([{"c1": "test_insert_varchar"}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": " test insert varchar "}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": "^789$ test insert varchar"}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ("test_insert_varchar", " test insert varchar ",
                                                                "^789$ test insert varchar")}))
        res = db_obj.drop_table("test_insert_varchar", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_big_varchar(self):
        """
        target: test insert varchar with big length
        method: create table with varchar column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_big_varchar", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_varchar", {"c1": {"type": "varchar"}}, ConflictType.Error)
        assert table_obj
        for i in range(100):
            res = table_obj.insert([{"c1": "test_insert_big_varchar" * 1000}])
            assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ["test_insert_big_varchar" * 1000] * 100}))

        res = db_obj.drop_table("test_insert_big_varchar", ConflictType.Error)
        assert res.error_code == ErrorCode.OK


    def _test_insert_embedding(self):
        """
        target: test insert embedding column
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_embedding", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_embedding", {"c1": {"type": "vector,3,int"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [4, 5, 6]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [7, 8, 9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9]}, {"c1": [-7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9],
                                                                [1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))

        res = db_obj.drop_table("test_insert_embedding", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_embedding_2", ConflictType.Ignore)
        db_obj.create_table("test_insert_embedding_2", {"c1": {"type": "vector,3,float"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_embedding_2")
        assert table_obj
        res = table_obj.insert([{"c1": [1.1, 2.2, 3.3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [4.4, 5.5, 6.6]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [7.7, 8.8, 9.9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-7.7, -8.8, -9.9]}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([1.1, 2.2, 3.3], [4.4, 5.5, 6.6], [7.7, 8.8, 9.9], [-7.7, -8.8, -9.9])}))

        res = db_obj.drop_table("test_insert_embedding_2", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_tensor(self):
        """
        target: test insert tensor column
        method: create table with tensor column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_tensor", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_tensor", {"c1": {"type": "tensor,3,int"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4, 5, 6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": np.array([[7, 8, 9], [-7, -8, -9]])}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9, -7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]],
                                                                [[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))

        res = db_obj.drop_table("test_insert_tensor", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_tensor_2", ConflictType.Ignore)
        db_obj.create_table("test_insert_tensor_2", {"c1": {"type": "tensor,3,float"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_tensor_2")
        assert table_obj
        res = table_obj.insert([{"c1": [1.1, 2.2, 3.3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4.4, 5.5, 6.6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]]}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1.1, 2.2, 3.3]], [[4.4, 5.5, 6.6]], [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]])}))

        res = db_obj.drop_table("test_insert_tensor_2", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_tensor_array(self):
        """
        target: test insert tensor_array column
        method: create table with tensor_array column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_tensor_array", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_tensor_array", {"c1": {"type": "tensorarray,2,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [[[1, 2], [3, 4]], [[5, 6]]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [np.array([[7, 8]]), np.array([[9, 10], [11, 12]])]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[[13, 14], [15, 16], [17, 18]]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[[1, 2], [3, 4]], [[5, 6]]], [[[7, 8]], [[9, 10], [11, 12]]], [[[13, 14], [15, 16], [17, 18]]])}))
        res = db_obj.drop_table("test_insert_tensor_array", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_sparse(self):
        """
        target: test insert sparse column
        method: create table with sparse column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_sparse", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_sparse", {"c1": {"type": "sparse,100,float,int"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": {"indices": [10, 20, 30], "values": [1.1, 2.2, 3.3]}}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": {"indices": [40, 50, 60], "values": [4.4, 5.5, 6.6]}}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": {"indices": [70, 80, 90], "values": [7.7, 8.8, 9.9]}}, {"c1": {"indices": [70, 80, 90], "values": [-7.7, -8.8, -9.9]}}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ({"indices": [10, 20, 30], "values": [1.1, 2.2, 3.3]}, {"indices": [40, 50, 60], "values": [4.4, 5.5, 6.6]},
                    {"indices": [70, 80, 90], "values": [7.7, 8.8, 9.9]}, {"indices": [70, 80, 90], "values": [-7.7, -8.8, -9.9]})}))

        res = db_obj.drop_table("test_insert_sparse", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        pass

    def _test_insert_big_embedding(self):
        """
        target: test insert embedding with big dimension
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_big_embedding", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_embedding", {"c1": {"type": "vector,16384,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [4] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [7] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-9999999] * 16384}])
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_big_embedding", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_big_embedding_float(self):
        """
        target: test insert embedding float with big dimension
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_big_embedding_float",
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_embedding_float", {"c1": {"type": "vector,16384,float"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-9999999] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [1.1] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-9999999.988] * 16384}])
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_big_embedding_float", ConflictType.Error)
        assert res.error_code == ErrorCode.OK


    def _test_insert_big_embedding_various_type(self, types, types_examples):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_big_embedding_various_type", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_embedding_various_type", {
            "c1": {"type": types}}, ConflictType.Error)
        res = table_obj.insert(types_examples)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_big_embedding_various_type", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_exceed_block_size(self):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_exceed_block_size", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_exceed_block_size", {
            "c1": {"type": "float"}}, ConflictType.Error)
        assert table_obj
        values = [{"c1": 1} for _ in range(8193)]

        with pytest.raises(InfinityException) as exception:
            table_obj.insert(values)
            assert exception.type == InfinityException
            assert exception.value.args[0] == "Insert batch row limit shouldn\'t more than 8193"

        res = db_obj.drop_table("test_insert_exceed_block_size", ConflictType.Error)
        assert res.error_code == ErrorCode.OK


    # insert primitive data type not aligned with table definition
    def _test_insert_data_not_aligned_with_table_definition(self, types, types_example):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_data_not_aligned_with_table_definition", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_data_not_aligned_with_table_definition",
                                        {"c1": {"type": "int"}, "c2": {"type": types}}, ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": types_example}]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_data_not_aligned_with_table_definition", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # insert large varchar which exceeds the limit to table
    # insert embedding data which type info isn't match with table definition
    # insert data into non-existent table, dropped table
    def _test_insert_data_into_non_existent_table(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_data_into_non_existent_table", ConflictType.Ignore)

        # create and drop table
        table_obj = db_obj.create_table("test_insert_data_into_non_existent_table",
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)
        res = db_obj.drop_table(
            "test_insert_data_into_non_existent_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # insert
        values = [{"c1": 1, "c2": 1}]
        # check whether throw exception TABLE_NOT_EXIST
        with pytest.raises(InfinityException) as e:
            table_obj.insert(values)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST


    # insert empty into table
    def _test_insert_empty_into_table(self, types):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_empty_into_table", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_empty_into_table",
                                        {"c1": {"type": "int"}, "c2": {"type": types}}, ConflictType.Error)

        # insert
        with pytest.raises(InfinityException) as e:
            values = [{}]
            table_obj.insert(values)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.SYNTAX_ERROR

        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_empty_into_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # insert data into index created table
    def _test_insert_data_into_index_created_table(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_data_into_index_created_table", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_data_into_index_created_table",
                                        {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)

        # create index
        table_obj.create_index("my_index_1",
                               [index.IndexInfo("c1",
                                                index.IndexType.Hnsw,
                                                [
                                                    index.InitParameter(
                                                        "M", "16"),
                                                    index.InitParameter(
                                                        "ef_construction", "50"),
                                                    index.InitParameter(
                                                        "ef", "50"),
                                                    index.InitParameter(
                                                        "metric", "l2")
                                                ])], ConflictType.Error)

        table_obj.create_index("my_index_2",
                               [index.IndexInfo("c1",
                                                index.IndexType.IVFFlat,
                                                [index.InitParameter("centroids_count", "128"),
                                                 index.InitParameter("metric", "l2")])], ConflictType.Error)

        # insert
        values = [{"c1": [1.1 for _ in range(1024)]}]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_pl()
        print(insert_res)

        res = table_obj.drop_index("my_index_1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index_2", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_data_into_index_created_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # insert table with 10000 columns.
    def _test_insert_table_with_10000_columns(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_table_with_10000_columns", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_table_with_10000_columns",
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        for i in range(100):
            values = [{"c1": i * 100 + j, "c2": i * 100 + j + 1}
                      for j in range(100)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

    def _test_read_after_shutdown(self):
        db_obj = self.infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("test_insert_table_with_10000_columns")
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_table_with_10000_columns", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # insert table with columns isn't matched (more and less)
    def _test_insert_with_not_matched_columns(self, values):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_not_matched_columns", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_not_matched_columns",
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        # insert
        with pytest.raises(Exception):
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_not_matched_columns", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # insert table with column value exceeding invalid value range
    def _test_insert_with_exceeding_invalid_value_range(self, values):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_exceeding_invalid_value_range", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_exceeding_invalid_value_range",
                                        {"c1": {"type": "int"}, "c2": {"type": "int32"}}, ConflictType.Error)

        # insert
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_pl()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_exceeding_invalid_value_range", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # batch insert, within limit
    def _test_batch_insert_within_limit(self, batch):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_batch_insert_within_limit",
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_batch_insert_within_limit",
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(batch)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_batch_insert_within_limit", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # batch insert, batch size limit 8192
    def _test_batch_insert(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_batch_insert", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_batch_insert", {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8192)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table("test_batch_insert", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # batch insert, with invalid data type inside.
    def _test_insert_with_invalid_data_type(self, batch, types):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_invalid_data_type", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_invalid_data_type",
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        # insert
        for i in range(5):
            values = [{"c1": 1, "c2": types[0]} for _ in range(batch)]
            if not types[1]:
                with pytest.raises(InfinityException) as e:
                    table_obj.insert(values)

                assert e.type == InfinityException
                assert e.value.args[0] == ErrorCode.NOT_SUPPORTED
            else:
                table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_invalid_data_type", ConflictType.Error)
        assert res.error_code == ErrorCode.OK


    # batch insert, with invalid column count
    def _test_batch_insert_with_invalid_column_count(self, batch):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_invalid_column_count", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_invalid_column_count", {
            "c1": {"type": "int"}}, ConflictType.Error)

        # insert
        with pytest.raises(Exception):
            for i in range(5):
                values = [{"c1": 1, "c2": 1} for _ in range(batch)]
                table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_invalid_column_count", ConflictType.Error)
        assert res.error_code == ErrorCode.OK


    def _test_various_insert_types(self, column_types, column_types_example):
        # connect

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_insert_types", ConflictType.Ignore)
        db_obj.create_table("test_various_insert_types", {
            "c1": {"type": column_types}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_various_insert_types")

        values = [{"c1": column_types_example} for _ in range(5)]
        table_obj.insert(values)

        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_various_insert_types", ConflictType.Error)
        assert res.error_code == ErrorCode.OK


    def _test_insert_and_shutdown_output(self):

        os.system("rm -fr /var/infinity")
        # start service
        infinity_service_1 = start_infinity_service_in_subporcess()

        # connect
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_and_shutdown_output",
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_and_shutdown_output", {
            "c1": {"type": "int"}}, ConflictType.Error)

        for i in range(10):
            values = [{"c1": 1} for _ in range(100)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

        # shutdown service
        os.kill(infinity_service_1.pid, signal.SIGINT)

        time.sleep(1)

        # restart
        infinity_service_2 = start_infinity_service_in_subporcess()

        # connect
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("test_insert_and_shutdown_output")
        new_insert_res = table_obj.output(["*"]).to_df()
        print(new_insert_res)

        res = db_obj.drop_table(
            "test_insert_and_shutdown_output", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # shutdown service
        os.kill(infinity_service_2.pid, signal.SIGINT)

    def _test_insert_zero_column(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_zero_column", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_zero_column", {
            "c1": {"type": "int"}}, ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            table_obj.insert([])
            insert_res = table_obj.output(["*"]).to_df()
            print(insert_res)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INSERT_WITHOUT_VALUES

        res = db_obj.drop_table("test_insert_zero_column", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_no_match_column(self, column_name):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_no_match_column", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_no_match_column", {
            "c1": {"type": "int"}}, ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            table_obj.insert([{column_name: 1}])
            insert_res = table_obj.output(["*"]).to_df()
            print(insert_res)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.COLUMN_NOT_EXIST

        res = db_obj.drop_table(
            "test_insert_no_match_column", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
