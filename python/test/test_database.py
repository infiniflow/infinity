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
import common_values
import pytest
from infinity.errors import ErrorCode
from utils import trace_expected_exceptions


class TestDatabase:

    def test_version(self):
        print(infinity.__version__)

    def test_database(self):
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
            - 'default'
        3. drop databases
            - 'my_database'
        4. list tables:
            - 'default'
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        # infinity
        db = infinity_obj.create_database("my_database")
        assert db

        try:
            db = infinity_obj.create_database("my_database!@#")
        except Exception as e:
            print(e)

        try:
            db = infinity_obj.create_database("my-database-dash")
        except Exception as e:
            print(e)

        try:
            db = infinity_obj.create_database("123_database")
        except Exception as e:
            print(e)

        try:
            db = infinity_obj.create_database("")
        except Exception as e:
            print(e)

        res = infinity_obj.list_databases()
        assert res is not None

        res.db_names.sort()

        assert res.db_names[0] == 'default'
        assert res.db_names[1] == 'my_database'

        res = infinity_obj.drop_database("my_database")
        assert res.error_code == ErrorCode.OK

        # res = infinity_obj.drop_database("default")
        # assert not res.success

        res = infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK

        for db in res.db_names:
            assert db == 'default'

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def test_create_database_invalid_name(self):
        """
        create database with invalid name

        1. connect server

        2. create ' ' name db

        3. disconnect server
        """

        # 1. connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        # 2. create db with invalid name
        for db_name in common_values.invalid_name_array:
            try:
                # print('db name: ', db_name)
                db = infinity_obj.create_database(db_name)
                assert False
            except Exception as e:
                print(e)

        # 3. disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def test_create_drop_show_1K_databases(self):

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
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        db_count = 100

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

    @pytest.mark.skip(reason="Cost too much times")
    def test_create_drop_show_1M_databases(self):

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
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

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

    def test_repeatedly_create_drop_show_databases(self):

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
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        loop_count = 100

        # 2. forloop 1k times:

        for i in range(loop_count):
            # 2.1 create database:
            db = infinity_obj.create_database('test_repeatedly_create_drop_show_databases')

            # 2.2 show database
            dbs = infinity_obj.list_databases()
            for db_name in dbs.db_names:
                assert db_name in ['test_repeatedly_create_drop_show_databases', 'default']
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
            try:
                db = infinity_obj.drop_database(db_name)
            except Exception as e:
                print(e)

        # 3. disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip(reason="No if not exists flag now")
    def test_create_database_with_invalid_option(self):
        """
        target: create database with invalid option
        method:
        1. create database
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        # option: if not exists
        # other options are invalid
        db = infinity_obj.create_database("test_create_database_with_invalid_option", None)
        res = infinity_obj.drop_database("test_create_database_with_invalid_option")

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip(reason="No if not exists flag now")
    def test_create_database_with_if_not_exists(self):
        """
        target: create database if not exists
        method:
        1. create database
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        # option: if not exists
        # other options are invalid
        db = infinity_obj.create_database("my_database", None)
        res = infinity_obj.drop_database("my_database")

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip(reason="No if not exists flag now")
    def test_drop_database_with_invalid_option(self):
        """
        target: drop database with invalid option
        method:
        1. drop database
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        # option: if not exists
        # other options are invalid
        db = infinity_obj.drop_database("my_database", None)
        res = infinity_obj.drop_database("my_database")

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip(reason="No if not exists flag now")
    def test_drop_database_with_if_not_exists(self):
        """
        target: create database if not exists
        method:
        1. create database
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        # option: if not exists
        # other options are invalid
        db = infinity_obj.create_database("my_database", None)
        res = infinity_obj.drop_database("my_database", None)

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def test_describe_db(self):
        """
        target: describe db
        method:
        1. describe non-existent db
        2. describe existent db
        3. describe dropped db
        4. describe invalid name db
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        # option: if not exists
        # other options are invalid
        try:
            # print('db name: ', db_name)
            db = infinity_obj.get_database("db1")
            assert False
        except Exception as e:
            print(e)

        # 1. create db
        db = infinity_obj.create_database("my_database")

        # 2. get db(using db)
        db = infinity_obj.get_database("my_database")
        print(db._db_name)

        # 3. get 'default' db(using default), if not switch to default, my_database can't be dropped.
        db = infinity_obj.get_database("default")
        print(db._db_name)

        # 4.
        res = infinity_obj.drop_database("my_database")
        assert res.error_code == ErrorCode.OK

        # 2. drop db with invalid name
        for db_name in common_values.invalid_name_array:
            try:
                db = infinity_obj.drop_database(db_name)
                raise Exception(f"Can drop db: {db_name}")
            except Exception as e:
                print(e)

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def test_db_ops_after_disconnection(self):
        """
        target: describe db
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
            db = infinity_obj.create_database("my_database")
            assert False
        except Exception as e:
            print(f'Caught exception: {e}')

        # 3. drop db
        try:
            db = infinity_obj.drop_database("my_database")
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

        try:
            res = infinity_obj.drop_database("my_database")
        except Exception as e:
            print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # one thread get db, another thread drop this db
    @trace_expected_exceptions
    def test_get_drop_db_with_two_thread(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        infinity_obj.create_database("test_get_drop_db_with_two_thread")

        thread1 = threading.Thread(target=infinity_obj.drop_database("test_get_drop_db_with_two_thread"), args=(1,))
        thread2 = threading.Thread(target=infinity_obj.get_database("test_get_drop_db_with_two_thread"), args=(2,))

        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        try:
            res = infinity_obj.get_database("test_get_drop_db_with_two_thread")
            print(res)
        except Exception as e:
            print(e)

        try:
            res = infinity_obj.drop_database("test_get_drop_db_with_two_thread")
            print(res)
        except Exception as e:
            print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create same db in different thread to test conflict and show dbs
    @trace_expected_exceptions
    def test_create_same_db_in_different_threads(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        thread1 = threading.Thread(target=infinity_obj.create_database("test_create_same_db_in_different_threads"),
                                   args=(1,))
        thread2 = threading.Thread(target=infinity_obj.create_database("test_create_same_db_in_different_threads"),
                                   args=(2,))

        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK
