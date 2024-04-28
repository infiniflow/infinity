import os
import sys
import pytest
import threading
from httpapibase import HttpTest
from common.common_values import *


class TestDataBase(HttpTest):

    def test_http_verison(self):

        pass

    def test_http_database(self):

        db_name = "my_database"

        self.drop_database(db_name)
        self.create_database(db_name)

        self.create_database("my_database@#$", {
            "status_code": 500,
            "error_code": 3003,
        })

        self.create_database("my_databasse-dash", {
            "status_code": 500,
            "error_code": 3003,
        })

        self.create_database("123_database", {
            "status_code": 500,
            "error_code": 3003,
        })

        # list database
        self.list_database({
            "error_code": 0,
            "databases": [
                "my_database",
                "default_db",
            ]
        })

        # drop my_database
        self.drop_database(db_name)

        # list database
        self.list_database({
            "error_code": 0,
            "databases": [
                "default_db",
            ]
        })
        self.clear_database()

    def test_http_create_database_invalid_name(self):
        '''
            create databse with invalid name
        '''
        for db_name in invalid_name_array:
            if db_name == '':
                continue
            else:
                self.create_database(db_name, {
                    "status_code": 500,
                    "error_code": 3003,
                })
        self.clear_database()

    def test_http_create_drop_show_1K_databases(self):
        '''
            create 1K dbs,show these dbs,drop these dbs
        '''
        dbs = self.get_database()
        # list all databases
        for db_name in dbs:
            if db_name == "default_db":
                continue
            self.drop_database(db_name)

        # create databases
        db_count = 100
        for i in range(db_count):
            print('create db_name' + str(i))
            self.drop_database('db_name' + str(i))
        for i in range(db_count):
            self.create_database('db_name' + str(i))

        dbs = self.get_database()

        # list all databases
        for db_name in dbs:
            print("db name:" + db_name)
        assert len(dbs) == (db_count + 1)

        # drop databases
        for i in range(db_count):
            print('drop db_name' + str(i))
            self.drop_database('db_name' + str(i))
        self.clear_database()

    @pytest.mark.skipif(condition=os.getenv("SKIPTIMECOST")!="0", reason="Cost too much times")
    def test_http_create_drop_show_1M_databases(self):
        """
        create 100K dbs, show these dbs, drop these dbs
        """
        dbs = self.get_database()
        # list all databases
        for db_name in dbs:
            if db_name == "default_db":
                continue
            self.drop_database(db_name)

        # create databases
        db_count = 100000
        for i in range(db_count):
            print('create db_name' + str(i))
            self.drop_database('db_name' + str(i))
        for i in range(db_count):
            self.create_database('db_name' + str(i))

        dbs = self.get_database()

        # list all databases
        for db_name in dbs:
            print("db name:" + db_name)
        assert len(dbs) == (db_count + 1)

        # drop databases
        for i in range(db_count):
            print('drop db_name' + str(i))
            self.drop_database('db_name' + str(i))

    def test_http_repeatedly_create_drop_show_databases(self):
        """
        create db, show db and drop db, repeat above ops 100 times
        """
        loop_count = 100
        for i in range(loop_count):
            # create database
            self.create_database('test_repeatedly_create_drop_show_databases')
            # show database
            dbs = self.get_database()
            for db_name in dbs:
                assert db_name in ['test_repeatedly_create_drop_show_databases', "default_db"]
            assert len(dbs) == 2
            # drop databses
            self.drop_database('test_repeatedly_create_drop_show_databases')
        self.clear_database()

    def test_http_drop_database_with_invalid_name(self):
        """
        drop database with invalid name
        """
        for db_name in invalid_name_array:
            if db_name == '':
                continue
            self.drop_database(db_name, {
                "status_code": 500,
                "error_code": 3003,
            })
        self.clear_database()

    def test_http_get_db(self):
        """
        target: get db
        method:
        1. show non-existent db
        2. show existent db
        3. show dropped db
        4. show invalid name db
        expect: all operations successfully
        """
        # show database not exist
        self.show_database("db1", {
            "status_code": 500,
            "error_code": 3021,
            # "error_message": "{database_name} doesn't exist."
        })
        # create database
        self.create_database("my_database")
        # show database
        self.show_database("my_database", {
            "error_code": 0,
            "database_name": "default_db",
            "table_count": 0,
        })
        # show default database
        self.show_database("default_db")
        # drop database
        self.drop_database("my_database")
        # drop database with invalid name
        for db_name in invalid_float_array:
            self.drop_database(db_name, {
                "status_code": 500,
                "error_code": 3021,
                # "error_message": "{database_name} doesn't exist."
            })
        self.clear_database()

    def test_http_drop_non_existent_db(self):
        '''
            drop not exist db
        '''
        self.drop_database("my_database2", {
            "status_code": 200,
            # "error_message": "{database_name} doesn't exist."
        })
        self.clear_database()

    def test_http_get_drop_db_with_two_threads(self):
        '''
            one thread get db, another thread drop this db
        '''
        self.create_database("test_get_drop_db_with_two_thread")
        thread1 = threading.Thread(
            self.drop_database("test_get_drop_db_with_two_thread", {
                "status_code": 200,
            }), args=(1,)
        )
        thread2 = threading.Thread(
            self.show_database("test_get_drop_db_with_two_thread", {
                "status_code": 500,
                "error_code": 3021,
            }), args=(2,)
        )
        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        # drop databases
        self.drop_database("test_get_drop_db_with_two_thread")
        self.clear_database()

    def test_http_create_same_db_in_different_threads(self):
        '''
            create same db in different thread to test conflict and show dbs
        '''
        thread1 = threading.Thread(
            self.create_database("test_create_same_db_in_different_threads"), args=(1,)
        )
        thread2 = threading.Thread(
            self.create_database("test_create_same_db_in_different_threads"), args=(2,)
        )
        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        # drop databases
        self.drop_database("test_get_drop_db_with_two_thread")
        self.clear_database()

    def test_http_show_database(self):
        # create database
        self.drop_database("test_show_database")
        self.create_database("test_show_database")

        self.show_database("test_show_database", {
            "error_code": 0,
            "database_name": "test_show_database",
            "table_count": 0
        })
        self.drop_database("test_show_database")
        self.clear_database()

    def test_http_create_option(self):
        db_name = "test_create_option"
        # create with option: key ignore
        self.create_database(db_name)
        #create with option: key error
        self.create_database(db_name,{
            "status_code":500,
            "error_code":3016,
        },"kError")
        #create with option: key replace
        # self.create_database(db_name,{
        #     "error_code":0
        # },"kReplace")
        self.clear_database()

    def test_http_drop_option(self):
        db_name = "test_drop_option"
        # drop with option: key ignore
        self.drop_database(db_name)
        # drop with option: key error
        self.drop_database(db_name,{
            "status_code":500,
            "error_code":3021,
        },"kError")
        self.clear_database()
