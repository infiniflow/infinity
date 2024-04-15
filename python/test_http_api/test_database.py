
import sys 
import pytest 
import threading 
from httpapibase import HttpTest
from common.common_values import * 

class TestDataBase(HttpTest):

    def test_verison(self):
        
        pass 
    
    def test_database(self):

        dbname = "my_database"
        self.createdb("my_database")

        self.createdb("my_database@#$",{
            "status_code": 500,
            "error_code": 3003,
        })

        self.createdb("my_databasse-dash",{
            "status_code": 500,
            "error_code": 3003,
        })

        self.createdb("123_database",{
            "status_code": 500,
            "error_code": 3003,
        })

        #list database
        self.listdb({
            "error_code": 0,
            "databases":[
                "my_database",
                "default",
            ]
        })

        #drop my_database
        self.dropdb("my_database")

        #list database
        self.listdb({
            "error_code": 0,
            "databases":[
                "default",
            ]
        })
        self.cleardbs()
    
    def test_create_database_invalid_name(self):
        '''
            create databse with invalid name
        '''
        for db_name in invalid_name_array:
            if db_name == '':
                continue
            else:
                self.createdb(db_name,{
                    "status_code": 500,
                    "error_code": 3003,
                })
        self.cleardbs()

    def test_create_drop_show_1K_databases(self):
        '''
            create 1K dbs,show these dbs,drop these dbs
        '''
        dbs = self.getdbs()
        #list all databases
        for db_name in dbs:
            if db_name == 'default':
                continue 
            self.dropdb(db_name)
        
        #create databases
        db_count = 100
        for i in range(db_count):
            print('create db_name' + str(i))
            self.dropdb('db_name' + str(i))
        for i in range(db_count):
            self.createdb('db_name' + str(i))

        dbs = self.getdbs()

        #list all databases
        for db_name in dbs:
            print("db name:" + db_name)
        assert len(dbs) == (db_count + 1)

        #drop databases
        for i in range(db_count):
            if db_name == 'default':
                continue 
            print('drop db_name' + str(i))
            self.dropdb('db_name'+str(i))
        self.cleardbs()


    @pytest.mark.skip(reason="Cost too much times")
    def test_create_drop_show_1M_databases(self):
        """
        create 1M dbs, show these dbs, drop these dbs
        """
        dbs = self.getdbs()
        #list all databases
        for db_name in dbs:
            if db_name == 'default':
                continue 
            self.dropdb(db_name)
        
        #create databases
        db_count = 1000000
        for i in range(db_count):
            print('create db_name' + str(i))
            self.dropdb('db_name' + str(i))
        for i in range(db_count):
            self.createdb('db_name' + str(i))

        dbs = self.getdbs()

        #list all databases
        for db_name in dbs:
            print("db name:" + db_name)
        assert len(dbs) == (db_count + 1)

        #drop databases
        for i in range(db_count):
            if db_name == 'default':
                continue 
            print('drop db_name' + str(i))
            self.dropdb('db_name'+str(i))

    def test_repeatedly_create_drop_show_databases(self):
        """
        create db, show db and drop db, repeat above ops 100 times
        """
        loop_count = 100
        for i in range(loop_count):
            #create database
            self.createdb('test_repeatedly_create_drop_show_databases')
            #show database
            dbs = self.getdbs()
            for db_name in dbs:
                assert db_name in ['test_repeatedly_create_drop_show_databases', 'default']
            assert len(dbs) == 2
            #drop databses
            self.dropdb('test_repeatedly_create_drop_show_databases')
        self.cleardbs()
        
    def test_drop_database_with_invalid_name(self):
        """
        drop database with invalid name
        """
        for db_name in invalid_name_array:
            if db_name == '':
                continue 
            self.dropdb(db_name,{
                "status_code":500,
                "error_code": 3003,
            })
        self.cleardbs()

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
        #show database not exist
        self.showdb("db1",{
            "status_code":500,
            "error_code": 3021,
            #"error_message": "{database_name} doesn't exist."
        })
        #create database
        self.createdb("my_database")
        #show database
        self.showdb("my_database",{
            "error_code": 0,
            "database_name": "default",
            "table_count": 0,
        })
        #show default database
        self.showdb("default")
        #drop database
        self.dropdb("my_database")
        #drop database with invalid name
        for db_name in invalid_float_array:
            self.dropdb(db_name,{
                "status_code":500,
                "error_code": 3021,
                #"error_message": "{database_name} doesn't exist."
            })
        self.cleardbs()
    
    def test_drop_non_existent_db(self):
        '''
            drop not exist db
        '''
        self.dropdb("my_database2",{
            "status_code":200,
            #"error_message": "{database_name} doesn't exist."
        })
        self.cleardbs()
        
    def test_get_drop_db_with_two_threads(self):
        '''
            one thread get db, another thread drop this db
        '''
        self.createdb("test_get_drop_db_with_two_thread")
        thread1 = threading.Thread(
             self.dropdb("test_get_drop_db_with_two_thread",{
                 "status_code":200,
             }), args=(1,)
        )
        thread2 = threading.Thread(
            self.showdb("test_get_drop_db_with_two_thread",{
                "status_code":500,
                "error_code": 3021,
            }), args=(2,)
        )
        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        #drop databases
        self.dropdb("test_get_drop_db_with_two_thread")
        self.cleardbs()


    def test_create_same_db_in_different_threads(self):
        '''
            create same db in different thread to test conflict and show dbs
        '''
        thread1 = threading.Thread(
             self.createdb("test_create_same_db_in_different_threads"), args=(1,)
        )
        thread2 = threading.Thread(
            self.createdb("test_create_same_db_in_different_threads"), args=(2,)
        )
        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        #drop databases
        self.dropdb("test_get_drop_db_with_two_thread")
        self.cleardbs()


    def test_show_database(self):
        #create database
        self.dropdb("test_show_database")
        self.createdb("test_show_database")

        self.showdb("test_show_database",{
              "error_code": 0,
              "database_name": "test_show_database",
              "table_count": 0
        })
        self.dropdb("test_show_database")
        self.cleardbs()

    def test_create_option(self): 
        dbname = "test_create_option"
        #create with option: key ignore
        self.createdb(dbname)
        #drop with option: key error
        url = f"databases/{dbname}"
        h = self.SetUpHeader(['accept','content-type'])
        r = self.Request(url,"post",h)
        assert r.status_code == 500
        self.cleardbs()
    
    def test_drop_option(self):
        dbname = "test_drop_option"

        #drop with option: key ignore
        self.dropdb(dbname)

        # drop with option: key error
        url = f"databases/{dbname}"
        h = self.SetUpHeader(['accept','content-type'])
        r = self.Request(url,"delete",h)
        assert r.status_code == 500
        self.cleardbs()
    