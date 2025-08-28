import sys
import os
import threading
import pytest
from infinity.common import ConflictType, InfinityException
from common import common_values
import infinity
from infinity.errors import ErrorCode

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http


@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")


@pytest.fixture(scope="class")
def setup_class(request, http):
    if http:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity_http()
    else:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity.connect(uri)
    request.cls.uri = uri
    yield
    request.cls.infinity_obj.disconnect()


@pytest.mark.usefixtures("setup_class")
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    def _test_database(self, suffix):
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

        db_name = "test_pysdk_my_database" + suffix

        self.infinity_obj.drop_database(db_name, ConflictType.Ignore)

        # infinity
        db = self.infinity_obj.create_database(db_name)
        assert db

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("test_pysdk_my_database!@#")
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("test_pysdk_my-database-dash")
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("123_database_test_pysdk")
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("")
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        res = self.infinity_obj.drop_database(db_name)
        assert res.error_code == ErrorCode.OK

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.drop_database("default_db")
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DROPPING_USING_DB

    def _test_create_database_invalid_name(self, suffix):
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
            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

    def _test_repeatedly_create_drop_show_databases(self, suffix):
        loop_count = 100

        for i in range(loop_count):
            # 2.1 create database:
            db = self.infinity_obj.create_database('test_pysdk_test_repeatedly_create_drop_show_databases' + suffix)

            # 2.2 show database
            dbs = self.infinity_obj.list_databases()
            res_dbs = []
            for db_name in dbs.db_names:
                if (db_name.startswith("test_pysdk") and db_name.endswith(suffix)) or db_name == "default_db":
                    assert db_name in ['test_pysdk_test_repeatedly_create_drop_show_databases' + suffix, "default_db"]
                    res_dbs.append(db_name)
            assert len(res_dbs) == 2

            self.infinity_obj.drop_database('test_pysdk_test_repeatedly_create_drop_show_databases' + suffix)

    def _test_drop_database_with_invalid_name(self, suffix):
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
            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

    def _test_get_db(self, suffix):
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
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        db_name = "my_database" + suffix
        self.infinity_obj.drop_database(db_name, ConflictType.Ignore)

        # 1. create db
        db = self.infinity_obj.create_database(db_name, ConflictType.Error)

        # 2. get db(using db)
        db = self.infinity_obj.get_database(db_name)
        assert db
        print(db._db_name)

        # 3. get "default_db" db(using default), if not switch to default, my_database can't be dropped.
        db = self.infinity_obj.get_database("default_db")
        assert db
        print(db._db_name)

        # 4.
        res = self.infinity_obj.drop_database(db_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # 5. drop db with invalid name
        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                self.infinity_obj.drop_database(db_name)
            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

    def _test_drop_non_existent_db(self, suffix):

        for db_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                res = self.infinity_obj.drop_database("my_database")
            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

    def _test_get_drop_db_with_two_threads(self, suffix):
        # connect
        self.infinity_obj.drop_database("test_get_drop_db_with_two_thread" + suffix, ConflictType.Ignore)
        self.infinity_obj.create_database("test_get_drop_db_with_two_thread" + suffix)

        thread1 = threading.Thread(target=self.infinity_obj.drop_database("test_get_drop_db_with_two_thread" + suffix),
                                   args=(1,))

        with pytest.raises(InfinityException) as e:
            # with pytest.raises(Exception, match="ERROR:3021, Not existed entry*"):
            thread2 = threading.Thread(
                target=self.infinity_obj.get_database("test_get_drop_db_with_two_thread" + suffix),
                args=(2,))

            thread1.start()
            thread2.start()

            thread1.join()
            thread2.join()

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        # with pytest.raises(Exception, match="ERROR:3021, Not existed entry*"):
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.get_database("test_get_drop_db_with_two_thread" + suffix)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

        # with pytest.raises(Exception, match="ERROR:3021, Not existed entry*"):
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.drop_database("test_get_drop_db_with_two_thread" + suffix)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DB_NOT_EXIST

    def _test_create_same_db_in_different_threads(self, suffix):
        # connect

        with pytest.raises(InfinityException) as e:
            thread1 = threading.Thread(
                target=self.infinity_obj.create_database("test_create_same_db_in_different_threads" + suffix),
                args=(1,))
            thread2 = threading.Thread(
                target=self.infinity_obj.create_database("test_create_same_db_in_different_threads" + suffix),
                args=(2,))
            thread1.start()
            thread2.start()

            thread1.join()
            thread2.join()

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_DATABASE_NAME

        # drop
        self.infinity_obj.drop_database("test_create_same_db_in_different_threads" + suffix)

    def test_database(self, suffix):
        self._test_database(suffix)
        self._test_create_database_invalid_name(suffix)
        self._test_repeatedly_create_drop_show_databases(suffix)
        self._test_drop_database_with_invalid_name(suffix)
        self._test_get_db(suffix)
        self._test_drop_non_existent_db(suffix)
        self._test_get_drop_db_with_two_threads(suffix)
        self._test_create_same_db_in_different_threads(suffix)

    @pytest.mark.parametrize("conflict_type", [ConflictType.Error,
                                               ConflictType.Ignore,
                                               0,
                                               1,
                                               ])
    def test_create_with_valid_option(self, conflict_type, suffix):
        # create db
        self.infinity_obj.drop_database("test_create_option" + suffix, ConflictType.Ignore)
        self.infinity_obj.create_database("test_create_option" + suffix, conflict_type)

        self.infinity_obj.drop_database("test_create_option" + suffix)

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def test_create_with_invalid_option(self, conflict_type, suffix):
        self.infinity_obj.drop_database("test_create_invalid_option" + suffix, ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_database("test_create_invalid_option" + suffix, conflict_type)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        0,
        1,
    ])
    def test_drop_option_with_valid_option(self, conflict_type, suffix):
        # create db
        self.infinity_obj.drop_database("test_drop_option" + suffix, ConflictType.Ignore)
        self.infinity_obj.create_database("test_drop_option" + suffix)
        self.infinity_obj.drop_database("test_drop_option" + suffix, conflict_type)

        self.infinity_obj.drop_database("test_drop_option" + suffix, ConflictType.Ignore)

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(ConflictType.Replace),
        pytest.param(2),
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    def test_drop_option(self, conflict_type, suffix):
        # create db
        self.infinity_obj.drop_database("test_drop_option" + suffix, ConflictType.Ignore)
        self.infinity_obj.create_database("test_drop_option" + suffix)
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.drop_database("test_drop_option" + suffix, conflict_type)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        self.infinity_obj.drop_database("test_drop_option" + suffix, ConflictType.Error)

    @pytest.mark.usefixtures("skip_if_http")
    def test_create_upper_database_name(self, suffix):
        db_upper_name = "MY_DATABASE" + suffix.upper()
        db_lower_name = "my_database" + suffix.lower()
        self.infinity_obj.drop_database(db_lower_name, ConflictType.Ignore)

        db = self.infinity_obj.create_database(db_upper_name, ConflictType.Error)
        assert db._db_name == db_upper_name

        db = self.infinity_obj.get_database(db_lower_name)
        assert db._db_name == db_lower_name
        db = self.infinity_obj.get_database(db_upper_name)
        assert db._db_name == db_upper_name
        db = self.infinity_obj.get_database("default_db")
        assert db._db_name == "default_db"
        db = self.infinity_obj.get_database("DEFAULT_DB")
        print(db)
        print(db._db_name)
        assert db._db_name == "DEFAULT_DB"

        res = self.infinity_obj.show_database(db_lower_name)
        assert res.database_name == db_lower_name

        res = self.infinity_obj.show_database(db_upper_name)
        assert res.database_name == db_lower_name

        res = self.infinity_obj.show_database("DEFAULT_DB")
        print(res)
        assert res.database_name == "default_db"

        res = self.infinity_obj.drop_database(db_upper_name)
        assert res.error_code == ErrorCode.OK
