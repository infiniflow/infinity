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
import pytest

import common_values
import infinity
from infinity.errors import ErrorCode
from utils import trace_expected_exceptions


class TestTable:

    def test_version(self):
        print(infinity.__version__)

    def test_table(self):
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
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        # infinity
        tb = db_obj.create_table(
            "my_table", {"c1": "int, primary key", "c2": "float"}, None)
        assert tb is not None

        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "my_table!@#", {"c1": "int, primary key", "c2": "float"}, None)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "my-table-dash", {"c1": "float, primary key", "c2": "int"}, None)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "123_table", {"c1": "int, primary key", "c2": "float"}, None)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "bad_column", {"123": "int, primary key", "c2": "float"}, None)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "", {"c1": "int, primary key", "c2": "float"}, None)

        # FIXME: res = db_obj.describe_table("my_table")

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("my_table")
        assert res.error_code == ErrorCode.OK

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_table_with_different_column_name(self):
        """
        target: test create/drop/describe/get valid table name with different column names
        methods:
        1. create table
        2. get table
        3. list table
        4. describe table
        expect: all operations successfully

        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        for column_name in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table("my_table", {column_name: "int"}, None)
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
        # FIXME: res = db_obj.describe_table("my_table")

        # disconnect
        # res = infinity_obj.disconnect()
        # assert res

        try:
            res = infinity_obj.disconnect()
        except Exception as e:
            print(e)

    # create/drop/describe/get valid table name with different column types
    def test_table_with_different_column_types(self):
        """
        target: test create/drop/describe/get valid table name with different column types
        methods:
        1. create table
            - 'my_table'            √
                - c1 bool primary key
                - c2 int
                - c3 int8
                - c4 int16
                - c5 int32
                - c6 int64
                - c7 int128
                - c8 float
                - c9 float32
                - c10 double
                - c11 float64
                - c12 varchar
                - c13 integer
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
        4. describe table
        expect: all operations successfully

        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        # infinity
        tb = db_obj.create_table(
            "my_table", {"c1": "bool, primary key", "c2": "int", "c3": "int8", "c4": "int16",
                         "c5": "int32", "c6": "int64", "c7": "int128", "c8": "float", "c9": "float32",
                         "c10": "double", "c11": "float64", "c12": "varchar", "c13": "integer"}, None)
        assert tb is not None

        for tb_type in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table("my_table", {"c1": tb_type}, None)
                raise Exception(f"Can create tb: {tb_type}")
            except Exception as e:
                print(e)

        # FIXME: res = db_obj.describe_table("my_table")

        # list table
        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # get table
        try:
            res = db_obj.get_table("my_table")
        except Exception as e:
            print(e)

        # drop table
        res = db_obj.drop_table("my_table")
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create/drop/describe/get table with 10000 columns with various column types.
    def test_table_with_various_column_types(self):
        """
        target: create/drop/describe/get table with 10000 columns with various column types.
        methods: create table with various column types
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")
        c_count = 10000

        types = [
            "int", "int8", "int16", "int32", "int64", "int128", "float",
            "float32", "double", "float64", "varchar", "integer", "bool",
        ]
        # make params
        params = {}
        for i in range(c_count - 13):
            params.update({
                "c" + str(i): types[i % 13]
            })

        # create tb with 10000 columns with various column types
        try:
            tb = db_obj.create_table('my_table', params, None)
        except Exception as e:
            print(e)

        # FIXME: res = db_obj.describe_table("my_table")

        # list table
        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # get table
        try:
            res = db_obj.get_table("my_table")
            print(res.output(["c2"]))
        except Exception as e:
            print(e)

        # drop table
        res = db_obj.drop_table("my_table")
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create/drop table with different invalid options
    def test_table_with_different_invalid_options(self):
        """
        target: create/drop table with different invalid options.
        methods: create table with various options
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        for option_name in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table("my_table", {"c1": "int"}, option_name)
                # raise Exception(f"Can create option_name: {option_name}")
            except Exception as e:
                print(e)

        # # disconnect
        # res = infinity_obj.disconnect()
        # assert res.error_code == ErrorCode.OK

        try:
            res = infinity_obj.disconnect()
        except Exception as e:
            print(e)

    # create/drop/describe/get 1000 tables with 10000 columns with various column types.
    @pytest.mark.skip(reason="Cost too much times,and may cause the serve to terminate")
    def test_various_tables_with_various_columns(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        tb_count = 1000
        column_count = 10000

        types = [
            "int", "int8", "int16", "int32", "int64", "int128", "float",
            "float32", "double", "float64", "varchar", "integer", "bool",
        ]

        # make params
        params = {}
        for i in range(column_count - 13):
            params.update({
                "c" + str(i): types[i % 13]
            })

        for i in range(tb_count):
            try:
                tb = db_obj.create_table("my_table" + str(i), params, None)
                print(i)
                # raise Exception(f"Can create table")
            except Exception as e:
                print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # after disconnection, create / drop / describe / list / get table
    def test_after_disconnect_use_table(self):
        """
        target: after disconnection, create / drop / describe / list / get table
        methods:
        1. disconnect database
        2. create / drop / describe / list / get table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

        # create table
        try:
            res = db_obj.create_table("my_table", {"c1": "int"}, None)
        except Exception as e:
            print(e)

        # drop table
        try:
            res = db_obj.drop_table("my_table")
        except Exception as e:
            print(e)

        # FIXME: res = db_obj.describe_table("my_table")

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
    def test_table_with_invalid_options(self):
        """
        target: create/drop table with invalid options.
        methods: create table with various options
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        for option_name in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table("my_table", {"c1": "int"}, option_name)
                # raise Exception(f"Can create option_name: {option_name}")
            except Exception as e:
                print(e)

        # disconnect
        # FIXME
        try:
            res = infinity_obj.disconnect()
        except Exception as e:
            print(e)

    # create created table, drop dropped table.
    def test_create_drop_table(self):
        """
        target: create created table, drop dropped table
        methods: create table ,drop table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        # create
        tb = db_obj.create_table(
            "my_table", {"c1": "int, primary key", "c2": "float"}, None)
        assert tb is not None

        try:
            tb = db_obj.create_table("my_table", {"c1": "int"}, None)
        except Exception as e:
            print(e)

        # drop
        db_obj.drop_table("my_table")

        try:
            tb = db_obj.drop_table("my_table")
        except Exception as e:
            print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # describe created table, describe not-created table, describe dropped table
    @pytest.mark.skip(reason="Feature request")
    def test_describe_various_table(self):
        """
        target: describe created table, describe not-created table, describe dropped table
        methods: describe table
        expect: all operations successfully
        """
        pass

    # create/drop/list/get 1K table to reach the limit
    def test_create_1K_table(self):
        """
        target: create/drop/list/get 1K table
        methods: describe table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        tb_count = 1000
        for i in range(tb_count):
            try:
                tb = db_obj.create_table("my_table" + str(i), {"c1": "int"}, None)
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
                tb = db_obj.drop_table("my_table" + str(i))
            except Exception as e:
                print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create/drop/list/get 1M table to reach the limit
    @pytest.mark.skip(reason="Cost too much times")
    def test_create_1M_table(self):
        """
        target: create/drop/list/get 1K table
        methods: describe table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        tb_count = 1000000
        for i in range(tb_count):
            try:
                tb = db_obj.create_table("my_table" + str(i), {"c1": "int"}, None)
                print(i)
                # raise Exception(f"Can create table")
            except Exception as e:
                print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create/drop same table in different thread to test conflict
    @trace_expected_exceptions
    def test_create_or_drop_same_table_in_different_thread(self):
        """
        target: create/drop same table in different thread to test conflict
        methods: create table at same time for 16 times
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        # create table
        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into processpool
            futures = [executor.submit(db_obj.create_table("my_table", {"c1": "int"}, None), i) for i in range(16)]
            # wait all processes finished
            concurrent.futures.wait(futures)

        # drop table
        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into threadpool
            futures = [executor.submit(db_obj.drop_table("my_table"), i) for i in range(16)]
            # wait all threads finished
            concurrent.futures.wait(futures)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create empty column table
    def test_create_empty_column_table(self):
        """
        target: create empty column table
        methods: create empty column table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        try:
            db_obj.create_table("my_table", None, None)
        except Exception as e:
            print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

