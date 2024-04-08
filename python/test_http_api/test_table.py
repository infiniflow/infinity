import sys 
import pytest 
import concurrent.futures
from httpapibase import HttpTest
from common.common_values import * 

class TestTable(HttpTest):
    def test_verison(self):
        return 
    def test_table(self):
        dbname = "default"
        self.showdb(dbname)
        self.dropTable(dbname,"my_table")
        self.createTable(
            dbname,
            "my_table",
            {
                "name": 
                {
                    "type": "varchar",
                    "constraints": ["not null"]
                },
                "age":
                {
                    "type": "integer",
                    "constraints": ["not null"]
                },
                "score":
                {
                    "type": "integer",
                    "constraints": ["not null"]
                },
            },
        )
        self.createTable(
            dbname,
            "mytable!@#",
             {
                "c1": 
                {
                    "type": "integer",
                    "constraints": ["primary key"]
                },
                "c2":
                {
                    "type":"float",
                }
            },
            {},
            {
                "status_code":500,
                "error_code": 3004,
                #"error_message": "Duplicate table: mytable!@# in {dbname}.",
            },
        )

        self.createTable(
            dbname,
            "123_table",
            {
                "c1": 
                {
                    "type": "integer",
                    "constraints": ["primary key"]
                },
                "c2":
                {
                    "type":"float",
                }
            },
            {},
            {
                "status_code":500,
                "error_code": 3004,
                #"error_message": "Duplicate table: 123_table in {dbname}.",
            }
        )

        self.createTable(
            dbname,
            "my-table-dash",
            {
                "c1": 
                {
                    "type": "integer",
                    "constraints": ["primary key"]
                },
                "c2":
                {
                    "type":"float",
                }
            },
            {},
            {
                "status_code":500,
                "error_code": 3004,
                #"error_message": "Duplicate table: my-table-dash in {dbname}.",
            }
        )

        self.createTable(
            dbname,
            "bad_column",
            {
                "123": 
                {
                    "type": "integer",
                    "constraints": ["int","primary key"]
                },
                "c2":
                {
                    "type":"float",
                }
            },
            {},
            {
                "status_code":500,
                "error_code": 3005,
                #"error_message": "Duplicate table: bad_column in {dbname}.",
            }
        )

        # self.ListTables("default",expect={
        #     "error_code":0,
        #     # "tables":[
        #     #     {
        #     #         "name":"my_table",
        #     #         "type":"table",
        #     #         "column_count": 2,
        #     #         # "block_count": 21,
        #     #         # "block_capacity": 8192,
        #     #         # "segment_count": 3,
        #     #         # "segment_capacity": 8388608,
        #     #     },
        #     #]
        # })
        self.dropTable("default","my_table")

        # self.ListTables("default",expect={
        #     "error_code":0,
        #     "tables":{
        #     }
        # })  
        return 
    
    # def test_show_tables(self):
    #     self.showdb("default")
    #     tblist = self.GetAllTables("default")
    #     print(tblist)
    #     return
    
    # def test_create_varchar_table(self):
    #     '''
    #         test create table with varchar column
    #     '''
    #     tbname = "test_create_varchar_table"
    #     self.showdb("default")
    #     self.dropTable("default",tbname)
    #     self.createTable(
    #         "default",
    #         tbname,
    #         {"c1": "varchar, primary key", "c2": "float"},
    #     )
    #     self.dropTable("default",tbname)
    #     return
    
    # def test_create_embedding_table(self):
    #     """
    #         test create table with embedding column
    #     """
    #     tbname = "test_create_embedding_table"
    #     self.showdb("default")
    #     self.dropTable("default",tbname)
    #     self.createTable(
    #         "default",
    #         tbname,
    #         {"c1": "vector,128,float"},
    #     )
    #     self.dropTable("default",tbname)
    #     return
    
    # def test_create_table_with_invalid_column_name(self):
    #     """
    #         test create table with invalid column_name
    #     """
    #     dbname = "default"
    #     tbname = "test_create_invalid_column_name"

    #     invalid_clumn_name = [""," ","12","[]","()","{}","10",1.1],
    #     for i in range(invalid_clumn_name):
    #         self.showdb(dbname)
    #         self.dropTable("default",tbname)
    #         self.createTable(
    #             dbname,
    #             tbname,
    #             {{invalid_clumn_name[i]}: "vector,128,float"},
    #             {},
    #             {
    #                 "error_code": 3017,
    #                 "error_message": "Duplicate table: {tbname} in {dbname}.",
    #             }
    #         )
    #         self.dropTable(dbname,tbname)
    #     return 
    
    # @pytest.mark.parametrize("column_name", invalid_name_array)
    # def test_create_table_with_invalid_column_name_python(self, column_name):
    #     dbname = "default"
    #     tbname = "my_table"

    #     self.showdb(dbname)
    #     self.dropTable("default",tbname)

    #     self.createTable(
    #         dbname,
    #         tbname,
    #         {column_name: "int"},
    #         {},
    #         {
    #             "error_code": 3017,
    #             "error_message": "Duplicate table: {tbname} in {dbname}.",
    #         }
    #     )
    #     return
    
    # def test_table_with_different_column_name(self):
    #     """
    #     test create/drop/show/get valid table name with different column names
    #     """
    #     dbname = "default"
    #     tbname = "my_table"
    #     self.showdb(dbname)
    #     self.dropTable("default",tbname)
    #     for column_name in invalid_name_array:
    #         #create table
    #         self.createTable(
    #             dbname,
    #             tbname,
    #             {column_name: "int"},
    #             {},
    #             {
    #                 "error_code": 3017,
    #                 "error_message": "Duplicate table: {tbname} in {dbname}.",
    #             }
    #         )
    #         #list table
    #         self.ListTables(dbname)
    #         #get tables
    #         self.showTable(dbname,tbname,expect={
    #             "error_code": 0,
    #             "database_name": dbname,
    #             "table_name": tbname,
    #             # "store_dir": "/tmp/infinity/data/nIHniKeHIB_db_default/h1abZcWuBs_table_my_table",
    #             "column_count" : 1,
    #             "segment_count" : 1,
    #             # "row_count" : 5
    #         })
    #         #drop table
    #         self.dropdb(dbname)
    #     return
    
    # def test_table_with_different_column_types(self):
    #     dbname = "default"
    #     tbname = "my_table"
    #     self.showdb(dbname)
    #     self.dropTable("default",tbname)
    #     self.createTable(
    #         dbname,
    #         tbname,
    #         {"c1": "bool, primary key", "c2": "int", "c3": "int8", "c4": "int16",
    #                      "c5": "int32", "c6": "int64", "c7": "int128", "c8": "float", "c9": "float32",
    #                      "c10": "double", "c11": "float64", "c12": "varchar", "c13": "integer"},
    #     )
    #     self.ListTables(dbname,tbname)
    #     self.showTable(dbname,tbname,expect={
    #         "error_code": 0,
    #         "database_name": dbname,
    #         "table_name": tbname,
    #         # "store_dir": "/tmp/infinity/data/nIHniKeHIB_db_default/h1abZcWuBs_table_my_table",
    #         "column_count" : 13,
    #         # "segment_count" : 1,
    #         # "row_count" : 5
    #     })
    #     return
    
    # def test_table_with_various_column_types(self):
    #     dbname = "default"
    #     tbname = "my_table"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     c_count = 10000
    #     types = [
    #         "int", "int8", "int16", "int32", "int64", "int128", "float",
    #         "float32", "double", "float64", "varchar", "integer", "bool",
    #     ]
    #     # make params
    #     params = {}
    #     for i in range(c_count - 13):
    #         params.update({
    #             "c" + str(i): types[i % 13]
    #         })
    #     # create tb with 10000 columns with various column types
    #     self.createTable(dbname,tbname,params)
    #     #list tables
    #     self.ListTables(dbname,{
    #         "error_code": 0,
    #         "tables": 
    #         {
    #             {
    #                 "name": dbname,
    #                 "type": "table",
    #                 "column_count": c_count-13,
    #                 # "block_count": 21,
    #                 # "block_capacity": 8192,
    #                 # "segment_count": 3,
    #                 # "segment_capacity": 8388608
    #             },
    #         }
    #     })
    #     #show tables
    #     self.showTable(dbname,tbname,{
    #         "error_code": 0,
    #         "database_name": dbname,
    #         "table_name": tbname,
    #         #"store_dir": "/tmp/infinity/data/nIHniKeHIB_db_default/h1abZcWuBs_table_my_table",
    #         "column_count" : c_count-13,
    #         # "segment_count" : 1,
    #         # "row_count" : 5
    #     })
    #     #drop tables
    #     self.dropTable(dbname,tbname)
    #     return
    # @pytest.mark.parametrize("invalid_option_array", [
    #     pytest.param([], marks=pytest.mark.xfail),
    #     pytest.param((), marks=pytest.mark.xfail),
    #     pytest.param({}, marks=pytest.mark.xfail),
    #     pytest.param(1, marks=pytest.mark.xfail),
    #     pytest.param(1.1, marks=pytest.mark.xfail),
    #     pytest.param('', marks=pytest.mark.xfail),
    #     pytest.param(' ', marks=pytest.mark.xfail),
    #     pytest.param('12', marks=pytest.mark.xfail),
    #     pytest.param('name-12', marks=pytest.mark.xfail),
    #     pytest.param('12name', marks=pytest.mark.xfail),
    #     pytest.param('数据库名', marks=pytest.mark.xfail),
    #     pytest.param(''.join('x' for i in range(65536 + 1)), marks=pytest.mark.xfail),
    #     None,
    # ])
    # def test_table_with_different_invalid_options(self):
    #     dbname = "default"
    #     tbname = "test_table_with_different_invalid_options"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)

    #     expect = {
    #         "error_code": 3017,
    #         "error_message": "Duplicate table: {tbname} in {dbname}.",
    #     }
    #     return 
    
    # @pytest.mark.slow
    # @pytest.mark.skip(reason="Cost too much times,and may cause the serve to terminate")
    # def test_various_tables_with_various_columns(self):
    #     dbname = "default"
    #     tbname = "my_table"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
        
    #     tb_count = 1000
    #     column_count = 10000

    #     tb_count = 1000
    #     column_count = 10000

    #     types = [
    #         "int", "int8", "int16", "int32", "int64", "int128", "float",
    #         "float32", "double", "float64", "varchar", "integer", "bool",
    #     ]

    #     # make params
    #     params = {}
    #     for i in range(column_count - 13):
    #         params.update({
    #             "c" + str(i): types[i % 13]
    #         })

    #     for i in range(tb_count):
    #         self.createTable(
    #             dbname,
    #             "my_table"+str[i],
    #             params,
    #         )
    #     return
    # def test_after_disconnect_use_table(self):
    #     pass 
    # def test_table_with_invalid_options(self):
    #     pass 
    # def test_create_drop_table(self):
    #     dbname = "default"
    #     tbname = "my_table"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     self.createTable(
    #         dbname,
    #         tbname,
    #         {"c1": "int, primary key", "c2": "float"},
    #     )
    #     self.createTable(
    #         dbname,
    #         tbname,
    #         {"c1": "int"},
    #     )
    #     self.dropTable(dbname,tbname)
    #     self.dropTable(dbname,tbname)

    # @pytest.mark.skip(reason="Feature request")
    # def test_describe_various_table(self):
    #     pass 
    
    # def test_create_1K_table(self):
    #     dbname = "default"
    #     tbname = "my_table"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)

    #     tb_count = 1000
    #     for i in range(tb_count):
    #         self.createTable(
    #             dbname,
    #             tbname+str(i),
    #             {"c1": "int"},
    #         ) 
    #     #get all tables
    #     res = self.GetAllTables(dbname,tbname)
    #     assert len(res) == tb_count

    #     #show table
    #     for i in range(tb_count):
    #         self.showTable(dbname,tbname,expect={
    #             "error_code": 0,
    #             "database_name": dbname,
    #             "table_name": tbname+str(i),
    #             "column_count" : 1,
    #             "segment_count" : 1,
    #             #"row_count" : 5
    #         }) 
    #     #drop table
    #     for i in range(tb_count):
    #         self.dropTable(dbname,tbname+str(i))
    #     return 
    # # create/drop/list/get 1M table to reach the limit
    # @pytest.mark.slow
    # @pytest.mark.skip(reason="Cost too much times")
    # def test_create_1M_table(self):
    #     dbname = "default"
    #     tbname = "my_table"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)

    #     tb_count = 1000000
    #     for i in range(tb_count):
    #         self.createTable(
    #             dbname,
    #             tbname+str(i),
    #             {"c1": "int"},
    #         ) 
    #     #get all tables
    #     res = self.GetAllTables(dbname,tbname)
    #     assert len(res) == tb_count

    #     #show table
    #     for i in range(tb_count):
    #         self.showTable(dbname,tbname,expect={
    #             "error_code": 0,
    #             "database_name": dbname,
    #             "table_name": tbname+str(i),
    #             "column_count" : 1,
    #             "segment_count" : 1,
    #             #"row_count" : 5
    #         }) 
    #     #drop table
    #     for i in range(tb_count):
    #         self.dropTable(dbname,tbname+str(i))
    #     return 
    
    # def test_create_or_drop_same_table_in_different_thread(self):
    #     dbname = "default"
    #     tbname = "my_table"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)

    #     with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
    #         # commit task into processpool
    #         futures = [executor.submit(self.createTable(
    #             dbname,
    #             tbname,
    #             {"c1": "int"},
    #         ), i) for i in
    #                    range(16)]
    #         # wait all processes finished
    #         concurrent.futures.wait(futures)

    #     # drop table
    #     with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
    #         # commit task into threadpool
    #         futures = [executor.submit(self.dropTable(dbname,tbname), i) for i in range(16)]
    #         # wait all threads finished
    #         concurrent.futures.wait(futures)
    #     return 
    
    # def test_create_empty_column_table(self):
    #     dbname = "default"
    #     tbname = "my_table"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     self.createTable(dbname,tbname,None)
    #     return 
    
    # @pytest.mark.parametrize("types", [
    #     "int", "int8", "int16", "int32", "int64", "integer",
    #     "float", "float32", "double", "float64",
    #     "varchar",
    #     "bool",
    #     "vector, 3, float"])
    # def test_create_valid_option(self, types):
    #     dbname = "default"
    #     tbname = "test_valid_option"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     self.createTable(dbname,tbname,{"c1":types})
    #     return
    
    # @pytest.mark.parametrize("types", [
    #     "int", "int8", "int16", "int32", "int64", "integer",
    #     "float", "float32", "double", "float64",
    #     "varchar",
    #     "bool",
    #     "vector, 3, float"])
    # @pytest.mark.parametrize("bool", [True, False])
    # def test_drop_option(self, types, bool):
    #     dbname = "default"
    #     tbname = "test_drop_option"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     #TODO: 
    #     self.createTable(dbname,tbname,{"c1":types})
    #     self.dropTable(dbname,tbname)
    #     return 
    
    # def test_create_same_name_table(self):
    #     dbname = "default"
    #     tbname = "test_create_same_name"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     self.createTable(dbname,tbname,{{"c1": "int"}})
    #     with pytest.raises(Exception, match="ERROR:3017*"):
    #         self.createTable(dbname,tbname, {"c1": "int"})
    #     return 
    
    # def test_drop_same_name_table(self):
    #     dbname = "default"
    #     tbname = "test_drop_same_name"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     self.dropTable(dbname,tbname)
    #     return 
    
    # def test_same_column_name(self):
    #     dbname = "default"
    #     tbname = "test_same_column_name"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     self.createTable(dbname,tbname,{"c1": "int","c1": "int"})
    #     return 
    
    # @pytest.mark.parametrize("types", [
    #     "int", "int8", "int16", "int32", "int64", "integer",
    #     "float", "float32", "double", "float64",
    #     "varchar",
    #     "bool",
    #     "vector, 3, float"])
    # @pytest.mark.parametrize("column_number", [[
    #     0, 1, pow(2, 63) - 1
    # ]])
    # def test_column_numbers(self, types, column_number):
    #     dbname = "default"
    #     tbname = "test_same_column_name"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     values = {"c" + str(i): types for i in column_number}
    #     self.createTable(dbname,tbname,values)
    #     return 
    
    
    # def test_various_table_create_option(self,conflict_type):
        
    #     return 
    # def test_various_table_drop_option(self, get_infinity_db, conflict_type):
    #     return 
    
    # def test_create_duplicated_table_with_ignore_option(self, get_infinity_db):
    #     dbname = "default"
    #     tbname = "test_create_duplicated_table_with_error_option"
    #     self.showdb(dbname)
    #     self.dropTable(dbname,tbname)
    #     with pytest.raises(Exception, match="ERROR:3017*"):
    #         for i in range(100):
    #             self.createTable("test_create_duplicated_table_with_error_option", {"c1": "int"})
    
    #     return 
    # def test_create_duplicated_table_with_error_option(self, get_infinity_db):
    #     return 
    # def test_create_duplicated_table_with_replace_option(self, get_infinity_db):
    #     return
    
    