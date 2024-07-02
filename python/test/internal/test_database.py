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
from internal.test_sdkbase import TestSdk

class TestDatabase(TestSdk):

    # def _test_version(self):
    #     print(infinity.__version__)

    def _test_database(self):
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

        db_name = "test_pysdk_my_database"

        self.infinity_obj.drop_database(db_name, ConflictType.Ignore)

        # infinity
        db = self.infinity_obj.create_database(db_name)
        assert db

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("test_pysdk_my_database!@#")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("test_pysdk_my-database-dash")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("123_database_test_pysdk")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        res = self.infinity_obj.list_databases()
        assert res is not None

        res.db_names.sort()

        db_names = []
        for db in res.db_names:
            if db == "default_db" or "test_pysdk" in db:
                db_names.append(db)

        db_names.sort()
        assert db_names[0] == "default_db"
        assert db_names[1] == db_name

        res = self.infinity_obj.drop_database(db_name)
        assert res.error_code == ErrorCode.OK

        # res = self.infinity_obj.drop_database("default_db")
        # assert not res.success

        res = self.infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK

        db_names = []
        for db in res.db_names:
            if db == "default_db" or "test_pysdk" in db:
                db_names.append(db)

        for db in db_names:
            assert db == "default_db"


    def _test_create_database_invalid_name(self):
        """
        create database with invalid name

        1. connect server

        2. create ' ' name db

        3. disconnect server
        """

        # 2. create db with invalid name
        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                self.infinity_obj.create_database(db_name)
            assert e.type == infinity.common.InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME


    def _test_create_drop_show_1K_databases(self):

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
        db_count = 100
        for i in range(db_count):
            print('create test_pysdk_db_name' + str(i))
            db = self.infinity_obj.drop_database('test_pysdk_db_name' + str(i), ConflictType.Ignore)
        # db = self.infinity_obj.create_database('db_name')
        # res = self.infinity_obj.drop_database('db_name')
        # 2. create db with invalid name
        for i in range(db_count):
            print('create test_pysdk_db_name' + str(i))
            db = self.infinity_obj.create_database('test_pysdk_db_name' + str(i))

        dbs = self.infinity_obj.list_databases()
        res_dbs = []
        for db_name in dbs.db_names:
            print('db name: ' + db_name)
            if db_name.startswith("test_pysdk") or db_name == "default_db":
                res_dbs.append(db_name)
        assert len(res_dbs) == (db_count + 1)
        # 4. drop 1m database
        for i in range(db_count):
            print('drop test_pysdk_db_name' + str(i))
            db = self.infinity_obj.drop_database('test_pysdk_db_name' + str(i))


    @pytest.mark.slow
    def _test_create_drop_show_1M_databases(self):

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
        db_count = 1000000
        # db = self.infinity_obj.create_database('db_name')
        # res = self.infinity_obj.drop_database('db_name')
        # 2. create db with invalid name
        for i in range(db_count):
            print('create db_name' + str(i))
            db = self.infinity_obj.create_database('db_name' + str(i))

        # 4. drop 1m database
        for i in range(db_count):
            print('drop db_name' + str(i))
            db = self.infinity_obj.drop_database('db_name' + str(i))
        #
        #
        # for db_name in common_values.invalid_name_array:
        #     try:
        #         # print('db name: ', db_name)
        #         db = self.infinity_obj.create_database(db_name)
        #     except Exception as e:
        #         print(e)


    def _test_repeatedly_create_drop_show_databases(self):

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

        loop_count = 100

        # 2. forloop 1k times:

        for i in range(loop_count):
            # 2.1 create database:
            db = self.infinity_obj.create_database('test_pysdk_test_repeatedly_create_drop_show_databases')

            # 2.2 show database
            dbs = self.infinity_obj.list_databases()
            res_dbs = []
            for db_name in dbs.db_names:
                if db_name.startswith("test_pysdk") or db_name == "default_db":
                    assert db_name in ['test_pysdk_test_repeatedly_create_drop_show_databases', "default_db"]
                    res_dbs.append(db_name)
            assert len(res_dbs) == 2

            # 2.3 drop database
            self.infinity_obj.drop_database('test_pysdk_test_repeatedly_create_drop_show_databases')

    def _test_drop_database_with_invalid_name(self):
        """
        drop database with invalid name

        1. connect server

        2. drop ' ' name db

        3. disconnect server
        """
        # 2. drop db with invalid name
        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                self.infinity_obj.drop_database(db_name)
            assert e.type == infinity.common.InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

    def _test_get_db(self):
        """
        target: get db
        method:
        1. show non-existent db
        2. show existent db
        3. show dropped db
        4. show invalid name db
        expect: all operations successfully
        """

        # option: if not exists
        # other options are invalid
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.get_database("db1")
        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        db_name = "my_database"
        self.infinity_obj.drop_database(db_name, ConflictType.Ignore)

        # 1. create db
        db = self.infinity_obj.create_database(db_name, ConflictType.Error)

        # 2. get db(using db)
        db = self.infinity_obj.get_database(db_name)
        print(db._db_name)

        # 3. get "default_db" db(using default), if not switch to default, my_database can't be dropped.
        db = self.infinity_obj.get_database("default_db")
        print(db._db_name)

        # 4.
        res = self.infinity_obj.drop_database(db_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # 2. drop db with invalid name
        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                self.infinity_obj.drop_database(db_name)
            assert e.type == infinity.common.InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME


    def _test_db_ops_after_disconnection(self):
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
        infinity_obj = infinity.connect(self.uri)
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
    def _test_drop_non_existent_db(self):

        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                res = self.infinity_obj.drop_database("my_database")
            assert e.type == infinity.common.InfinityException
            assert e.value.args[0] == ErrorCode.DB_NOT_EXIST



    # one thread get db, another thread drop this db
    def _test_get_drop_db_with_two_threads(self):
        # connect
        self.infinity_obj.drop_database("test_get_drop_db_with_two_thread", ConflictType.Ignore)
        self.infinity_obj.create_database("test_get_drop_db_with_two_thread")

        thread1 = threading.Thread(target=self.infinity_obj.drop_database("test_get_drop_db_with_two_thread"), args=(1,))

        with pytest.raises(InfinityException) as e:
            # with pytest.raises(Exception, match="ERROR:3021, Not existed entry*"):
            thread2 = threading.Thread(target=self.infinity_obj.get_database("test_get_drop_db_with_two_thread"), args=(2,))

            thread1.start()
            thread2.start()

            thread1.join()
            thread2.join()

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        # with pytest.raises(Exception, match="ERROR:3021, Not existed entry*"):
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.get_database("test_get_drop_db_with_two_thread")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        # with pytest.raises(Exception, match="ERROR:3021, Not existed entry*"):
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.drop_database("test_get_drop_db_with_two_thread")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST



    # create same db in different thread to test conflict and show dbs
    def _test_create_same_db_in_different_threads(self):
        # connect

        with pytest.raises(InfinityException) as e:
            thread1 = threading.Thread(target=self.infinity_obj.create_database("test_create_same_db_in_different_threads"),
                                       args=(1,))
            thread2 = threading.Thread(target=self.infinity_obj.create_database("test_create_same_db_in_different_threads"),
                                       args=(2,))
            thread1.start()
            thread2.start()

            thread1.join()
            thread2.join()

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_DATABASE_NAME

        # drop
        self.infinity_obj.drop_database("test_create_same_db_in_different_threads")



    def _test_show_database(self):
        # create db
        self.infinity_obj.drop_database("test_show_database", ConflictType.Ignore)
        self.infinity_obj.create_database("test_show_database", ConflictType.Error)

        res = self.infinity_obj.show_database("test_show_database")
        assert res.database_name == "test_show_database"

        self.infinity_obj.drop_database("test_show_database", ConflictType.Error)



    def _test_create_with_valid_option(self, conflict_type):
        # create db
        self.infinity_obj.drop_database("test_create_option", ConflictType.Ignore)
        self.infinity_obj.create_database("test_create_option", conflict_type)

        # self.infinity_obj.create_database("test_create_option", ConflictType.Ignore)
        # self.infinity_obj.create_database("test_create_option", ConflictType.Replace)

        self.infinity_obj.drop_database("test_create_option")


    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def _test_create_with_invalid_option(self, conflict_type):
        self.infinity_obj.drop_database("test_create_invalid_option", ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("test_create_invalid_option", conflict_type)

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        # self.infinity_obj.drop_database("test_create_option")

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        0,
        1,
    ])
    def _test_drop_option_with_valid_option(self, conflict_type):
        # create db
        self.infinity_obj.drop_database("test_drop_option", ConflictType.Ignore)
        self.infinity_obj.create_database("test_drop_option")
        self.infinity_obj.drop_database("test_drop_option", conflict_type)

        self.infinity_obj.drop_database("test_drop_option", ConflictType.Ignore)

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(ConflictType.Replace),
        pytest.param(2),
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    def _test_drop_option(self, conflict_type):
        # create db
        self.infinity_obj.drop_database("test_drop_option", ConflictType.Ignore)
        self.infinity_obj.create_database("test_drop_option")
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.drop_database("test_drop_option", conflict_type)

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        self.infinity_obj.drop_database("test_drop_option", ConflictType.Error)


    @pytest.mark.parametrize("table_name", ["test_show_table"])
    def _test_show_valid_table(self, table_name):
        db_obj = self.infinity_obj.get_database("default_db")
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
    def _test_show_invalid_table(self, table_name):
        db_obj = self.infinity_obj.get_database("default_db")
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
    def _test_show_not_exist_table(self, table_name):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_table", ConflictType.Ignore)
        db_obj.create_table("test_show_table", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                            ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            db_obj.show_table(table_name)

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

        db_obj.drop_table("test_show_table", ConflictType.Error)

    @pytest.mark.parametrize("column_name", ["test_show_table_columns"])
    def _test_show_table_columns_with_valid_name(self, column_name):
        db_obj = self.infinity_obj.get_database("default_db")
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
    def _test_show_table_columns_with_invalid_name(self, column_name):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_table_columns", ConflictType.Ignore)

        db_obj.create_table("test_show_table_columns", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}})

        with pytest.raises(InfinityException) as e:
            db_obj.show_columns(column_name)

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST or e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        db_obj.drop_table("test_show_table_columns", ConflictType.Error)