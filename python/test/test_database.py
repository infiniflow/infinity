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

import threading
import infinity
import pytest
from common import common_values
from infinity.common import ConflictType, InfinityException
from infinity.errors import ErrorCode
from test_sdkbase import TestSdk


class TestDatabase(TestSdk):

    def test_version(self):
        print(infinity.__version__)

    def test_database(self):
        # remote
        self._test_database(common_values.TEST_REMOTE_HOST)
        # local
        self._test_database(common_values.TEST_LOCAL_PATH)

    def test_create_database_invalid_name(self):
        # remote
        self._test_create_database_invalid_name(common_values.TEST_REMOTE_HOST)
        # local
        self._test_create_database_invalid_name(common_values.TEST_LOCAL_PATH)

    def test_create_drop_show_1K_databases(self):
        # remote
        self._test_create_drop_show_1K_databases(common_values.TEST_REMOTE_HOST)
        # local
        self._test_create_drop_show_1K_databases(common_values.TEST_LOCAL_PATH)

    @pytest.mark.slow
    def test_create_drop_show_1M_databases(self):
        self._test_create_drop_show_1M_databases(common_values.TEST_REMOTE_HOST)
        # self._test_create_drop_show_1M_databases(common_values.TEST_LOCAL_PATH)

    def test_repeatedly_create_drop_show_databases(self):
        self._test_repeatedly_create_drop_show_databases(common_values.TEST_REMOTE_HOST)
        self._test_repeatedly_create_drop_show_databases(common_values.TEST_LOCAL_PATH)

    def _test_database(self, uri):
        """
        target: test table apis
        method:
        1. create databases
            - 'my_database'         √
            - 'my_database!@#'      ❌
            - 'my-database-dash'    ❌
            - '123_database'        ❌
            - ''                    ❌
        2. list databases
            - 'my_database'
            - "default_db"
        3. drop databases
            - 'my_database'
        4. list tables:
            - "default_db"
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(uri)

        infinity_obj.drop_database("my_database", ConflictType.Ignore)

        # infinity
        db = infinity_obj.create_database("my_database")
        assert db

        with pytest.raises(InfinityException) as e:
            infinity_obj.create_database("my_database!@#")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(InfinityException) as e:
            infinity_obj.create_database("my-database-dash")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(InfinityException) as e:
            infinity_obj.create_database("123_database")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(InfinityException) as e:
            infinity_obj.create_database("")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        res = infinity_obj.list_databases()
        assert res is not None

        res.db_names.sort()

        assert res.db_names[0] == "default_db"
        assert res.db_names[1] == 'my_database'

        res = infinity_obj.drop_database("my_database")
        assert res.error_code == ErrorCode.OK

        # res = infinity_obj.drop_database("default_db")
        # assert not res.success

        res = infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK

        for db in res.db_names:
            assert db == "default_db"

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def _test_create_database_invalid_name(self, uri):
        """
        create database with invalid name

        1. connect server

        2. create ' ' name db

        3. disconnect server
        """

        # 1. connect
        infinity_obj = infinity.connect(uri)

        # 2. create db with invalid name
        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                infinity_obj.create_database(db_name)
            assert e.type == infinity.common.InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME


        # 3. disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def _test_create_drop_show_1K_databases(self, uri):

        """
        create 1K dbs, show these dbs, drop these dbs

        1. connect server

        2. create 1k databases

        3. show these databases

        4. drop 1k database

        5. show these databases

        6. disconnect server
        """

        # 1. connect
        infinity_obj = infinity.connect(uri)

        db_count = 100
        for i in range(db_count):
            print('create db_name' + str(i))
            db = infinity_obj.drop_database('db_name' + str(i), ConflictType.Ignore)
        # db = infinity_obj.create_database('db_name')
        # res = infinity_obj.drop_database('db_name')
        # 2. create db with invalid name
        for i in range(db_count):
            print('create db_name' + str(i))
            db = infinity_obj.create_database('db_name' + str(i))

        dbs = infinity_obj.list_databases()
        for db_name in dbs.db_names:
            print('db name: ' + db_name)
        assert len(dbs.db_names) == (db_count + 1)
        # 4. drop 1m database
        for i in range(db_count):
            print('drop db_name' + str(i))
            db = infinity_obj.drop_database('db_name' + str(i))

        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def _test_create_drop_show_1M_databases(self, uri):

        """
        create 1M dbs, show these dbs, drop these dbs

        1. connect server

        2. create 1m databases

        3. show these databases

        4. drop 1m database

        5. show these databases

        6. disconnect server
        """

        # 1. connect
        infinity_obj = infinity.connect(uri)

        db_count = 1000000
        # db = infinity_obj.create_database('db_name')
        # res = infinity_obj.drop_database('db_name')
        # 2. create db with invalid name
        for i in range(db_count):
            print('create db_name' + str(i))
            db = infinity_obj.create_database('db_name' + str(i))

        # 4. drop 1m database
        for i in range(db_count):
            print('drop db_name' + str(i))
            db = infinity_obj.drop_database('db_name' + str(i))
        #
        #
        # for db_name in common_values.invalid_name_array:
        #     try:
        #         # print('db name: ', db_name)
        #         db = infinity_obj.create_database(db_name)
        #     except Exception as e:
        #         print(e)

        # 3. disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def _test_repeatedly_create_drop_show_databases(self, uri):

        """
        create db, show db and drop db, repeat above ops 100 times

        1. connect server

        2. forloop 1k times:
        2.1 create database
        2.2 show database
        2.3 drop database

        3. disconnect server
        """

        # 1. connect
        infinity_obj = infinity.connect(uri)

        loop_count = 100

        # 2. forloop 1k times:

        for i in range(loop_count):
            # 2.1 create database:
            db = infinity_obj.create_database('test_repeatedly_create_drop_show_databases')

            # 2.2 show database
            dbs = infinity_obj.list_databases()
            for db_name in dbs.db_names:
                assert db_name in ['test_repeatedly_create_drop_show_databases', "default_db"]
            assert len(dbs.db_names) == 2

            # 2.3 drop database
            infinity_obj.drop_database('test_repeatedly_create_drop_show_databases')

        # 3. disconnect server
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def test_drop_database_with_invalid_name(self):
        """
        drop database with invalid name

        1. connect server

        2. drop ' ' name db

        3. disconnect server
        """

        # 1. connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        # 2. drop db with invalid name
        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                infinity_obj.drop_database(db_name)
            assert e.type == infinity.common.InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        # 3. disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def test_get_db(self):
        """
        target: get db
        method:
        1. show non-existent db
        2. show existent db
        3. show dropped db
        4. show invalid name db
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)


        # option: if not exists
        # other options are invalid
        with pytest.raises(InfinityException) as e:
            infinity_obj.get_database("db1")
        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        # 1. create db
        db = infinity_obj.create_database("my_database", ConflictType.Error)

        # 2. get db(using db)
        db = infinity_obj.get_database("my_database")
        print(db._db_name)

        # 3. get "default_db" db(using default), if not switch to default, my_database can't be dropped.
        db = infinity_obj.get_database("default_db")
        print(db._db_name)

        # 4.
        res = infinity_obj.drop_database("my_database", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # 2. drop db with invalid name
        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                infinity_obj.drop_database(db_name)
            assert e.type == infinity.common.InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME


        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def test_db_ops_after_disconnection(self):
        """
        target: show db
        method:
        1. connect and disconnect db
        2. create db
        3. drop db
        4. get db
        5. list db
        6. get non-existent db
        expect: all operations successfully
        """

        # 1. connect and disconnect db
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        res = infinity_obj.disconnect()

        # 2. create db
        try:
            db = infinity_obj.create_database("my_database", ConflictType.Error)
            assert False
        except Exception as e:
            print(f'Caught exception: {e}')

        # 3. drop db
        try:
            db = infinity_obj.drop_database("my_database", ConflictType.Error)
        except Exception as e:
            print(f'Caught exception: {e}')

        # 4. list db
        try:
            db = infinity_obj.list_databases()
        except Exception as e:
            print(f'Caught exception: {e}')

        # 5. get db
        try:
            db = infinity_obj.get_database("my_database")
        except Exception as e:
            print(f'Caught exception: {e}')

        # 6. get dummy-db
        try:
            db = infinity_obj.get_database("dummy_db")
        except Exception as e:
            print(f'Caught exception: {e}')

        assert res.error_code == ErrorCode.OK

    # TODO create_database("my_database", IF_NOT_EXISTS)
    # TODO create db with ignore-if-existence option
    # TODO create db with ignore-if-existence option when conflict repeatedly
    # TODO drop non-existent db with ignore if non-existent option repeatedly

    # drop non-existent db
    def test_drop_non_existent_db(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                res = infinity_obj.drop_database("my_database")
            assert e.type == infinity.common.InfinityException
            assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # one thread get db, another thread drop this db
    def test_get_drop_db_with_two_threads(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        infinity_obj.drop_database("test_get_drop_db_with_two_thread", ConflictType.Ignore)
        infinity_obj.create_database("test_get_drop_db_with_two_thread")

        thread1 = threading.Thread(target=infinity_obj.drop_database("test_get_drop_db_with_two_thread"), args=(1,))

        with pytest.raises(InfinityException) as e:
        # with pytest.raises(Exception, match="ERROR:3021, Not existed entry*"):
            thread2 = threading.Thread(target=infinity_obj.get_database("test_get_drop_db_with_two_thread"), args=(2,))

            thread1.start()
            thread2.start()

            thread1.join()
            thread2.join()

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        # with pytest.raises(Exception, match="ERROR:3021, Not existed entry*"):
        with pytest.raises(InfinityException) as e:
            infinity_obj.get_database("test_get_drop_db_with_two_thread")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        # with pytest.raises(Exception, match="ERROR:3021, Not existed entry*"):
        with pytest.raises(InfinityException) as e:
            infinity_obj.drop_database("test_get_drop_db_with_two_thread")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create same db in different thread to test conflict and show dbs
    # @trace_expected_exceptions
    def test_create_same_db_in_different_threads(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        with pytest.raises(InfinityException) as e:
            thread1 = threading.Thread(target=infinity_obj.create_database("test_create_same_db_in_different_threads"),
                                       args=(1,))
            thread2 = threading.Thread(target=infinity_obj.create_database("test_create_same_db_in_different_threads"),
                                       args=(2,))
            thread1.start()
            thread2.start()

            thread1.join()
            thread2.join()

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_DATABASE_NAME

        # drop
        infinity_obj.drop_database("test_create_same_db_in_different_threads")

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_show_database(self):
        # create db
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        infinity_obj.drop_database("test_show_database", ConflictType.Ignore)
        infinity_obj.create_database("test_show_database", ConflictType.Error)

        res = infinity_obj.show_database("test_show_database")
        assert res.database_name == "test_show_database"

        infinity_obj.drop_database("test_show_database", ConflictType.Error)
        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [ConflictType.Error,
                                               ConflictType.Ignore,
                                               ConflictType.Replace,
                                               0,
                                               1,
                                               2,
                                               ])
    def test_create_with_valid_option(self, conflict_type):
        # create db
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        infinity_obj.drop_database("test_create_option", ConflictType.Ignore)
        infinity_obj.create_database("test_create_option", conflict_type)

        # infinity_obj.create_database("test_create_option", ConflictType.Ignore)
        # infinity_obj.create_database("test_create_option", ConflictType.Replace)

        infinity_obj.drop_database("test_create_option")

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def test_create_with_invalid_option(self, conflict_type):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        infinity_obj.drop_database("test_create_option", ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            infinity_obj.create_database("test_create_option", conflict_type)

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        # infinity_obj.drop_database("test_create_option")

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        0,
        1,
    ])
    def test_drop_option_with_valid_option(self, conflict_type):
        # create db

        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        infinity_obj.drop_database("test_drop_option", ConflictType.Ignore)
        infinity_obj.create_database("test_drop_option")
        infinity_obj.drop_database("test_drop_option", conflict_type)

        infinity_obj.drop_database("test_drop_option", ConflictType.Ignore)
        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(ConflictType.Replace),
        pytest.param(2),
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    def test_drop_option(self, conflict_type):
        # create db

        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        infinity_obj.drop_database("test_drop_option", ConflictType.Ignore)
        infinity_obj.create_database("test_drop_option")
        with pytest.raises(InfinityException) as e:
            infinity_obj.drop_database("test_drop_option", conflict_type)

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        infinity_obj.drop_database("test_drop_option", ConflictType.Error)
        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("table_name", ["test_show_table"])
    def test_show_valid_table(self, get_infinity_db, table_name):
        db_obj = get_infinity_db
        db_obj.drop_table("test_show_table", ConflictType.Ignore)
        db_obj.create_table("test_show_table", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                            ConflictType.Error)

        res = db_obj.show_table(table_name)
        db_obj.drop_table("test_show_table", ConflictType.Error)
        print(res)

    @pytest.mark.parametrize("table_name", [pytest.param("Invalid name"),
                                            pytest.param(1),
                                            pytest.param(1.1),
                                            pytest.param(True),
                                            pytest.param([]),
                                            pytest.param(()),
                                            pytest.param({}),
                                            ])
    def test_show_invalid_table(self, get_infinity_db, table_name):
        db_obj = get_infinity_db
        db_obj.drop_table("test_show_table", ConflictType.Ignore)
        db_obj.create_table("test_show_table", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                            ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            db_obj.show_table(table_name)

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        # with pytest.raises(Exception):
        #     db_obj.show_table(table_name)

        db_obj.drop_table("test_show_table", ConflictType.Error)

    @pytest.mark.parametrize("table_name", [pytest.param("not_exist_name")])
    def test_show_not_exist_table(self, get_infinity_db, table_name):
        db_obj = get_infinity_db
        db_obj.drop_table("test_show_table", ConflictType.Ignore)
        db_obj.create_table("test_show_table", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                            ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            db_obj.show_table(table_name)

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

        db_obj.drop_table("test_show_table", ConflictType.Error)

    @pytest.mark.parametrize("column_name", ["test_show_table_columns"])
    def test_show_table_columns_with_valid_name(self, get_infinity_db, column_name):
        db_obj = get_infinity_db
        db_obj.drop_table("test_show_table_columns", ConflictType.Ignore)

        db_obj.create_table("test_show_table_columns", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                            ConflictType.Error)

        res = db_obj.show_columns(column_name)
        db_obj.drop_table("test_show_table_columns", ConflictType.Error)
        print(res)

    @pytest.mark.parametrize("column_name", [pytest.param("Invalid name"),
                                             pytest.param("not_exist_name"),
                                             pytest.param(1),
                                             pytest.param(1.1),
                                             pytest.param(True),
                                             pytest.param([]),
                                             pytest.param(()),
                                             pytest.param({}),
                                             ])
    def test_show_table_columns_with_invalid_name(self, get_infinity_db, column_name):
        db_obj = get_infinity_db
        db_obj.drop_table("test_show_table_columns", ConflictType.Ignore)

        db_obj.create_table("test_show_table_columns", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}})

        with pytest.raises(InfinityException) as e:
            db_obj.show_columns(column_name)

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST or e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        db_obj.drop_table("test_show_table_columns", ConflictType.Error)
