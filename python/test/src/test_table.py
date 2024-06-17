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
import concurrent.futures
import os
import sys

import pytest
import polars as pl
print(sys.path)
from common import common_values
from infinity.common import ConflictType, InfinityException
import infinity
from infinity.errors import ErrorCode
from src.utils import trace_expected_exceptions
from src.test_sdkbase import TestSdk


class TestTable(TestSdk):

    def _test_version(self):
        print(infinity.__version__)

    def _test_table(self):
        """
        target: test table apis
        method:
        1. create tables
            - 'my_table'        √
                - c1 int primary key
                - c2 float
            - 'my_table!@#'     ❌
                - c1 int primary key
                - c2 float
            - 'my-table-dash'   ❌
                - c1 float primary key
                - c2 int
            - '123_table'       ❌
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
        3. drop tables
            - 'my_table'
        4. list tables: empty
        expect: all operations successfully
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("my_table", ConflictType.Ignore)

        # infinity
        tb = db_obj.create_table(
            "my_table", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb is not None

        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "my_table!@#", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "my-table-dash", {"c1": "float, primary key", "c2": "int"}, ConflictType.Error)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "123_table", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "bad_column", {"123": "int, primary key", "c2": "float"}, ConflictType.Error)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error)

        # FIXME: res = db_obj.show_table("my_table")

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("my_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

    def _test_show_tables(self):

        db = self.infinity_obj.get_database("default_db")

        with pl.Config(fmt_str_lengths=1000):
            res = db.show_tables()
            print(res)
            # check the polars dataframe
            assert res.columns == ["database", "table", "type", "column_count", "block_count", "block_capacity",
                                   "segment_count", "segment_capacity"]

    def _test_create_varchar_table(self):
        """
        target: test create table with varchar column
        method: create table with varchar column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_varchar_table", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_varchar_table",
                                        {"c1": {"type": "varchar", "constraints": ["primary key"]},
                                         "c2": {"type": "float"}}, ConflictType.Error)
        assert table_obj

        res = db_obj.drop_table("test_create_varchar_table")
        assert res.error_code == ErrorCode.OK

    def _test_create_embedding_table(self):
        """
        target: test create table with embedding column
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_embedding_table", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_embedding_table", {
            "c1": {"type": "vector,128,float"}}, ConflictType.Error)
        assert table_obj

        res = db_obj.drop_table("test_create_embedding_table")
        assert res.error_code == ErrorCode.OK

    def _test_create_tensor_table(self):
        """
        target: test create table with tensor column
        method: create table with tensor column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_tensor_table", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_tensor_table", {
            "c1": {"type": "tensor,128,float"}}, ConflictType.Error)
        assert table_obj

        res = db_obj.drop_table("test_create_tensor_table")
        assert res.error_code == ErrorCode.OK

    def _test_create_tensorarray_table(self):
        """
        target: test create table with tensorarray column
        method: create table with tensorarray column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_tensorarray_table", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_tensorarray_table", {
            "c1": {"type": "tensorarray,128,float"}}, ConflictType.Error)
        assert table_obj

        res = db_obj.drop_table("test_create_tensorarray_table")
        assert res.error_code == ErrorCode.OK

    def _test_create_table_with_invalid_column_name(self):
        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                " ": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "12": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "[]": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "()": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "{}": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "1": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "1.1": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name", ConflictType.Error)

    def _test_create_table_with_invalid_column_name_python(self, column_name):
        """
        target: create with invalid column name
        methods: create table with invalid column name
        expect: all operations throw exception on python side
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("my_table", ConflictType.Ignore)

        try:
            tb = db_obj.create_table("my_table", {column_name: {"type": "int"}}, ConflictType.Error)
        except Exception as e:
            print(e)

    def _test_table_with_different_column_name(self):
        """
        target: test create/drop/show/get valid table name with different column names
        methods:
        1. create table
        2. get table
        3. list table
        4. show table
        expect: all operations successfully

        """
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("my_table", ConflictType.Ignore)

        for column_name in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table("my_table", {column_name: {"type": "int"}}, ConflictType.Error)
                raise Exception(f"Can create column_name: {column_name}")
            except Exception as e:
                print(e)

            # list table
            try:
                res = db_obj.list_tables()
            except Exception as e:
                print(e)

            # get table
            try:
                res = db_obj.get_table("my_table")
            except Exception as e:
                print(e)

            # drop table
            try:
                res = db_obj.drop_table("my_table")
            except Exception as e:
                print(e)
        # FIXME: res = db_obj.show_table("my_table")

        # disconnect
        # res = infinity_obj.disconnect()
        # assert res

        try:
            res = infinity_obj.disconnect()
        except Exception as e:
            print(e)

    # create/drop/show/get valid table name with different column types
    def _test_table_with_different_column_types(self):
        """
        target: test create/drop/show/get valid table name with different column types
        methods:
        1. create table
            - 'my_table'            √
                - c1 bool primary key
                - c2 int
                - c3 int8
                - c4 int16
                - c5 int32
                - c6 int64
                - c7 float
                - c8 float32
                - c9 double
                - c10 float64
                - c11 varchar
                - c12 integer
                - c13 int128 # invalid now
            - 'my_table'             ❌
                - c1 int!@#
             - 'my_table'            ❌
                - c1 ''
             - 'my_table'            ❌
                - c1 123int
             - 'my_table'            ❌
                - c1 int-varchar
             - 'my_table'            ❌
                - c1 ['int']
             - 'my_table'            ❌
                - c1 {"int"}
             - 'my_table'            ❌
                - c1 ("int")
        2. get table
        3. list table
        4. show table
        expect: all operations successfully

        """
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_different_column_types", ConflictType.Ignore)

        # infinity
        tb = db_obj.create_table(
            "test_table_with_different_column_types", {
                "c1": {"type": "bool", "constraints": ["primary key"]},
                "c2": {"type": "int"}, "c3": {"type": "int8"}, "c4": {"type": "int16"},
                "c5": {"type": "int32"}, "c6": {"type": "int64"}, "c7": {"type": "float"}, "c8": {"type": "float32"},
                "c9": {"type": "double"}, "c10": {"type": "float64"}, "c11": {"type": "varchar"},
                "c12": {"type": "integer"}}, ConflictType.Error)
        assert tb is not None

        for tb_type in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table("test_table_with_different_column_types", {"c1": {"type": tb_type}},
                                         ConflictType.Error)
                raise Exception(f"Can create tb: {tb_type}")
            except Exception as e:
                print(e)

        # FIXME: res = db_obj.show_table("my_table")

        # list table
        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # get table
        try:
            res = db_obj.get_table("test_table_with_different_column_types")
        except Exception as e:
            print(e)

        # drop table
        res = db_obj.drop_table("test_table_with_different_column_types", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create/drop/show/get table with 10000 columns with various column types.
    def _test_table_with_various_column_types(self):
        """
        target: create/drop/show/get table with 10000 columns with various column types.
        methods: create table with various column types
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_various_column_types", ConflictType.Ignore)
        c_count = 10000

        types = [
            "int", "int8", "int16", "int32", "int64", "float",
            "float32", "double", "float64", "varchar", "integer", "bool",
        ]
        # make params
        params = {}
        for i in range(c_count - 12):
            params.update({
                "c" + str(i): {"type": types[i % 12]}
            })

        # create tb with 10000 columns with various column types
        try:
            tb = db_obj.create_table('test_table_with_various_column_types', params, ConflictType.Error)
        except Exception as e:
            print(e)

        # FIXME: res = db_obj.show_table("my_table")

        # list table
        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # get table
        try:
            res = db_obj.get_table("test_table_with_various_column_types")
            print(res.output(["c2"]))
        except Exception as e:
            print(e)

        # drop table
        res = db_obj.drop_table("test_table_with_various_column_types", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create/drop table with different invalid options
    def _test_table_with_different_invalid_options(self, invalid_option_array):
        """
        target: create/drop table with different invalid options.
        methods: create table with various options
        expect: all operations successfully
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_different_invalid_options", ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            db_obj.create_table("test_table_with_different_invalid_options", {"c1": {"type": "int"}},
                                invalid_option_array)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

    # create/drop/show/get 1000 tables with 10000 columns with various column types.
    def _test_various_tables_with_various_columns(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_tables_with_various_columns", ConflictType.Ignore)

        tb_count = 1000
        column_count = 10000

        types = [
            "int", "int8", "int16", "int32", "int64", "float",
            "float32", "double", "float64", "varchar", "integer", "bool",
        ]

        # make params
        params = {}
        for i in range(column_count - len(types)):
            params.update({
                "c" + str(i): {"type": types[i % len(types)]}
            })

        for i in range(tb_count):
            try:
                tb = db_obj.create_table("test_various_tables_with_various_columns" + str(i), params,
                                         ConflictType.Error)
                print(i)
                # raise Exception(f"Can create table")
            except Exception as e:
                print(e)

        for i in range(tb_count):
            db_obj.drop_table("test_various_tables_with_various_columns" + str(i), ConflictType.Error)

    # after disconnection, create / drop / show / list / get table
    def _test_after_disconnect_use_table(self):
        """
        target: after disconnection, create / drop / show / list / get table
        methods:
        1. disconnect database
        2. create / drop / show / list / get table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_after_disconnect_use_table", ConflictType.Ignore)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

        # create table
        try:
            res = db_obj.create_table("test_after_disconnect_use_table", {"c1": {"type": "int"}}, ConflictType.Error)
        except Exception as e:
            print(e)

        # drop table
        try:
            res = db_obj.drop_table("test_after_disconnect_use_table", ConflictType.Error)
        except Exception as e:
            print(e)

        # FIXME: res = db_obj.show_table("my_table")

        # list table
        try:
            res = db_obj.list_tables()
        except Exception as e:
            print(e)

        # get table
        try:
            res = db_obj.get_table("my_table")
        except Exception as e:
            print(e)

    # create/drop table with invalid options
    def _test_table_with_invalid_options(self):
        """
        target: create/drop table with invalid options.
        methods: create table with various options
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_invalid_options", ConflictType.Ignore)

        for option_name in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table("test_table_with_invalid_options", {"c1": {"type": "int"}}, option_name)
                # raise Exception(f"Can create option_name: {option_name}")
            except Exception as e:
                print(e)

        db_obj.drop_table("test_table_with_invalid_options", ConflictType.Error)
        # try:
        #     res = self.infinity_obj.disconnect()
        # except Exception as e:
        #     print(e)

    # create created table, drop dropped table.
    def _test_create_drop_table(self):
        """
        target: create created table, drop dropped table
        methods: create table ,drop table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_drop_table", ConflictType.Ignore)

        # create
        tb = db_obj.create_table(
            "test_create_drop_table", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb is not None

        try:
            tb = db_obj.create_table("test_create_drop_table", {"c1": {"type": "int"}}, ConflictType.Error)
        except Exception as e:
            print(e)

        # drop
        db_obj.drop_table("test_create_drop_table", ConflictType.Error)

        try:
            tb = db_obj.drop_table("test_create_drop_table", ConflictType.Error)
        except Exception as e:
            print(e)

    # show created table, show not-created table, show dropped table
    def _test_describe_various_table(self):
        """
        target: show created table, show not-created table, show dropped table
        methods: show table
        expect: all operations successfully
        """
        pass

    # create/drop/list/get 1K table to reach the limit
    def _test_create_1K_table(self):
        """
        target: create/drop/list/get 1K table
        methods: show table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("my_table", ConflictType.Ignore)

        tb_count = 1000
        for i in range(tb_count):
            try:
                tb = db_obj.create_table("my_table" + str(i), {"c1": {"type": "int"}}, ConflictType.Error)
            except Exception as e:
                print(e)

        # list table
        try:
            res = db_obj.list_tables()
            print(res)
        except Exception as e:
            print(e)

        # get table
        for i in range(tb_count):
            try:
                res = db_obj.get_table("my_table" + str(i))
            except Exception as e:
                print(e)

        for i in range(tb_count):
            try:
                tb = db_obj.drop_table("my_table" + str(i), ConflictType.Error)
            except Exception as e:
                print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create/drop/list/get 1M table to reach the limit
    def _test_create_10K_table(self):
        """
        target: create/drop/list/get 1K table
        methods: show table
        expect: all operations successfully
        """
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("my_table", ConflictType.Ignore)

        tb_count = 10000
        for i in range(tb_count):
            res = db_obj.create_table("my_table" + str(i), {"c1": {"type": "int"}}, ConflictType.Error)
            assert res

        for i in range(tb_count):
            res = db_obj.drop_table("my_table" + str(i), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

    # create/drop same table in different thread to test conflict
    def _test_create_or_drop_same_table_in_different_thread(self):
        """
        target: create/drop same table in different thread to test conflict
        methods: create table at same time for 16 times
        expect: all operations successfully
        """
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("my_table", ConflictType.Ignore)

        # create table
        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into processpool
            futures = [executor.submit(db_obj.create_table("my_table", {"c1": {"type": "int"}}, ConflictType.Error), i)
                       for i in
                       range(16)]
            # wait all processes finished
            concurrent.futures.wait(futures)

        # drop table
        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into threadpool
            futures = [executor.submit(db_obj.drop_table("my_table"), i) for i in range(16)]
            # wait all threads finished
            concurrent.futures.wait(futures)

    # create empty column table
    def _test_create_empty_column_table(self):
        """
        target: create empty column table
        methods: create empty column table
        expect: all operations successfully
        """
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("my_table", ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            db_obj.create_table("my_table", {}, ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.NO_COLUMN_DEFINED

    def _test_create_valid_option(self, types):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_valid_option", ConflictType.Ignore)

        db_obj.create_table("test_valid_option", {"c1": {"type": types}}, ConflictType.Error)

        res = db_obj.drop_table("test_valid_option", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    #     # disconnect
    #     res = self.infinity_obj.disconnect()
    #     assert res.error_code == ErrorCode.OK

    def _test_drop_option(self, types, bool):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_drop_option", ConflictType.Ignore)

        db_obj.create_table("test_drop_option", {"c1": {"type": types}}, ConflictType.Error)
        res = db_obj.drop_table("test_drop_option", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_create_same_name_table(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_same_name", ConflictType.Ignore)

        # create
        db_obj.create_table("test_create_same_name", {"c1": {"type": "int"}}, ConflictType.Error)
        with pytest.raises(InfinityException) as e:
            db_obj.create_table("test_create_same_name", {"c1": {"type": "int"}}, ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_TABLE_NAME

        res = db_obj.drop_table("test_create_same_name", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_drop_same_name_table(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_drop_same_name", ConflictType.Ignore)
        # drop
        db_obj.drop_table("test_drop_same_name")
    def _test_same_column_name(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_same_column_name", ConflictType.Ignore)

        db_obj.create_table("test_same_column_name", {"c1": {"type": "int"},
                                                      "c1": {"type": "int"}}, ConflictType.Error)

        res = db_obj.drop_table("test_same_column_name", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_column_numbers(self, types, column_number):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_column_numbers", ConflictType.Ignore)

        values = {"c" + str(i): {"type": types} for i in column_number}
        db_obj.create_table("test_column_numbers", values, ConflictType.Error)

        res = db_obj.drop_table("test_column_numbers", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_table_create_valid_option(self, conflict_type):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_create_valid_option", ConflictType.Ignore)
        db_obj.create_table("test_table_create_valid_option", {"c1": {"type": "int"}}, conflict_type)
        res = db_obj.drop_table("test_table_create_valid_option", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_table_create_invalid_option(self, conflict_type):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_table_create_option", ConflictType.Ignore)
        with pytest.raises(InfinityException) as e:
            db_obj.create_table("test_various_table_create_option", {"c1": {"type": "int"}}, conflict_type)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

    def _test_table_drop_valid_option(self, conflict_type):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.create_table("test_table_drop_valid_option", {"c1": {"type": "int"}}, ConflictType.Ignore)
        db_obj.drop_table("test_table_drop_valid_option", conflict_type)

    def _test_table_drop_invalid_option(self, conflict_type):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.create_table("test_various_table_drop_option", {"c1": {"type": "int"}}, ConflictType.Ignore)
        with pytest.raises(InfinityException) as e:
            db_obj.drop_table("test_various_table_drop_option", conflict_type)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        res = db_obj.drop_table("test_various_table_drop_option", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_create_duplicated_table_with_ignore_option(self):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_duplicated_table_with_ignore_option", ConflictType.Ignore)

        for i in range(100):
            db_obj.create_table("test_create_duplicated_table_with_ignore_option", {"c1": {"type": "int"}},
                                ConflictType.Ignore)

        res = db_obj.drop_table("test_create_duplicated_table_with_ignore_option", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_create_duplicated_table_with_error_option(self):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_duplicated_table_with_error_option", ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            for i in range(100):
                db_obj.create_table("test_create_duplicated_table_with_error_option", {"c1": {"type": "int"}},
                                    ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_TABLE_NAME

        res = db_obj.drop_table("test_create_duplicated_table_with_error_option", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_create_duplicated_table_with_replace_option(self):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_duplicated_table_with_replace_option", ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            for i in range(100):
                db_obj.create_table("test_create_duplicated_table_with_replace_option", {"c" + str(i): {"type": "int"}},
                                    ConflictType.Replace)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_TABLE_NAME

        res = db_obj.drop_table("test_create_duplicated_table_with_replace_option", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
