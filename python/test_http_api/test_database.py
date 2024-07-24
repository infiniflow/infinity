import logging
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
        logging.error("test_http_databasetest_http_databasetest_http_databasetest_http_databasetest_http_databasetest_http_databasetest_http_databasetest_http_databasetest_http_database")
        db_name = "test_http_my_database"

        self.drop_database(db_name)
        self.create_database(db_name)

        self.create_database("test_http_my_database@#$", {
            "status_code": 500,
            "error_code": 3003,
        })

        self.create_database("test_http_my_databasse-dash", {
            "status_code": 500,
            "error_code": 3003,
        })

        self.create_database("123_database_test_http", {
            "status_code": 500,
            "error_code": 3003,
        })

        # list database
        self.list_database({
            "error_code": 0,
            "databases": [
                db_name,
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
        self.clear_http_database()

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
        self.clear_http_database()

    def test_http_create_drop_show_1K_databases(self):
        '''
            create 1K dbs,show these dbs,drop these dbs
        '''
        self.clear_http_database()

        # create databases
        db_count = 100
        for i in range(db_count):
            # print('create test_http_db_name' + str(i))
            self.drop_database('test_http_db_name' + str(i))
        for i in range(db_count):
            self.create_database('test_http_db_name' + str(i))

        dbs = self.get_database()

        # list all databases
        http_database_count = 0
        for db_name in dbs:
            # print("db name:" + db_name)
            if db_name.startswith("test_http") or db_name == "default_db":
                http_database_count += 1
        assert http_database_count == (db_count + 1)

        # drop databases
        for i in range(db_count):
            # print('drop test_http_db_name' + str(i))
            self.drop_database('test_http_db_name' + str(i))
        self.clear_http_database()

    @pytest.mark.skipif(condition=os.getenv("RUNSLOWTEST")!="1", reason="Cost too much times")
    def test_http_create_drop_show_100K_databases(self):
        """
        create 100K dbs, show these dbs, drop these dbs
        """
        self.clear_http_database()

        # create databases
        db_count = 100000
        for i in range(db_count):
            # print('create test_http_db_name' + str(i))
            self.drop_database('test_http_db_name' + str(i))
        for i in range(db_count):
            self.create_database('test_http_db_name' + str(i))

        dbs = self.get_database()

        # list all databases
        http_database_count = 0
        for db_name in dbs:
            # print("db name:" + db_name)
            if db_name.startswith("test_http") or db_name == "default_db":
                http_database_count += 1
        assert http_database_count == (db_count + 1)

        # drop databases
        for i in range(db_count):
            # print('drop test_http_db_name' + str(i))
            self.drop_database('test_http_db_name' + str(i))


    def test_http_repeatedly_create_drop_show_databases(self):
        """
        create db, show db and drop db, repeat above ops 100 times
        """
        loop_count = 100
        self.clear_http_database()
        db_name = 'test_http_test_repeatedly_create_drop_show_databases'
        for i in range(loop_count):
            # create database
            self.create_database(db_name)
            # show database
            dbs = self.get_database()
            for now_db_name in dbs:
                if now_db_name.startswith("test_http"):
                    assert now_db_name == db_name
            # drop databses
            self.drop_database(db_name)
        self.clear_http_database()

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
        self.clear_http_database()

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
        self.show_database("test_http_db1", {
            "status_code": 500,
            "error_code": 3021,
            # "error_message": "{database_name} doesn't exist."
        })
        # create database
        db_name = "test_http_my_database"
        self.create_database(db_name)
        # show database
        self.show_database(db_name, {
            "error_code": 0,
            "database_name": "default_db",
            "table_count": 0,
        })
        # show default database
        self.show_database("default_db")
        # drop database
        self.drop_database(db_name)
        # drop database with invalid name
        for invalid_db_name in invalid_float_array:
            self.drop_database(invalid_db_name, {
                "status_code": 500,
                "error_code": 3021,
                # "error_message": "{database_name} doesn't exist."
            })
        self.clear_http_database()

    def test_http_drop_non_existent_db(self):
        '''
            drop not exist db
        '''
        db_name = "test_http_my_database2"
        self.drop_database(db_name, {
            "status_code": 200,
            # "error_message": "{database_name} doesn't exist."
        })
        self.clear_http_database()

    def test_http_get_drop_db_with_two_threads(self):
        '''
            one thread get db, another thread drop this db
        '''
        db_name = "test_http_test_get_drop_db_with_two_thread"
        self.create_database(db_name)
        thread1 = threading.Thread(
            self.drop_database(db_name, {
                "status_code": 200,
            }), args=(1,)
        )
        thread2 = threading.Thread(
            self.show_database(db_name, {
                "status_code": 500,
                "error_code": 3021,
            }), args=(2,)
        )
        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        # drop databases
        self.drop_database(db_name)
        self.clear_http_database()

    def test_http_create_same_db_in_different_threads(self):
        '''
            create same db in different thread to test conflict and show dbs
        '''
        db_name = "test_http_test_create_same_db_in_different_threads"
        thread1 = threading.Thread(
            self.create_database(db_name), args=(1,)
        )
        thread2 = threading.Thread(
            self.create_database(db_name), args=(2,)
        )
        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        # drop databases
        self.drop_database(db_name)
        self.clear_http_database()

    def test_http_show_database(self):
        # create database
        db_name = "test_http_test_show_database"
        self.drop_database(db_name)
        self.create_database(db_name)

        self.show_database(db_name, {
            "error_code": 0,
            "database_name": db_name,
            "table_count": 0
        })
        self.drop_database(db_name)
        self.clear_http_database()

    def test_http_create_option(self):
        db_name = "test_http_test_create_option"
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
        self.clear_http_database()

    def test_http_drop_option(self):
        db_name = "test_http_test_drop_option"
        # drop with option: key ignore
        self.drop_database(db_name)
        # drop with option: key error
        self.drop_database(db_name,{
            "status_code":500,
            "error_code":3021,
        },"kError")
        self.clear_http_database()
