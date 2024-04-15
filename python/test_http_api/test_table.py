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

        self.ListTables("default")
        self.dropTable("default","my_table")
        self.ListTables("default")  
        return 
    
    def test_show_tables(self):
        self.showdb("default")
        tblist = self.GetAllTables("default")
        print(tblist)
        return
    
    def test_create_varchar_table(self):
        '''
            test create table with varchar column
        '''
        tbname = "test_create_varchar_table"
        self.showdb("default")
        self.dropTable("default",tbname)
        self.createTable(
            "default",
            tbname,
            {
                "c1": 
                {
                    "type": "varchar",
                    "constraints": ["primary key"]
                },
                "c2":
                {
                    "type":"float",
                }
            },
        )
        self.dropTable("default",tbname)
        return
    
    def test_create_embedding_table(self):
        """
            test create table with embedding column
        """
        tbname = "test_create_embedding_table"
        self.showdb("default")
        self.dropTable("default",tbname)
        self.createTable(
            "default",
            tbname,
            {
                "c1": 
                {
                    "type": "integer",
                    "constraints": []
                },
            },
        )
        self.dropTable("default",tbname)
        return
    
    def test_create_table_with_invalid_column_name(self):
        """
            test create table with invalid column_name
        """
        dbname = "default"
        tbname = "test_create_invalid_column_name"

        invalid_clumn_name = [""," ","12","[]","()","{}","10",1.1]
        for i in invalid_clumn_name:
            self.showdb(dbname)
            self.dropTable("default",tbname)
            if i == "":
                self.createTable(
                    dbname,
                    tbname,
                    {i:  {
                        "type":"float",
                    }},
                    {},
                    {
                        "status_code":500,
                        "error_code": 3042, #empty column name
                        #"error_message": "Duplicate table: {tbname} in {dbname}.",
                    }
                ) 
            else:
                self.createTable(
                    dbname,
                    tbname,
                    {i:  {
                        "type":"float",
                    }},
                    {},
                    {
                        "status_code":500,
                        "error_code": 3005, #invalid column name
                        #"error_message": "Duplicate table: {tbname} in {dbname}.",
                    }
                )
            self.dropTable(dbname,tbname)
        return 
    
    
    def test_create_table_with_invalid_column_name_python(self):
        dbname = "default"
        tbname = "my_table"

        self.showdb(dbname)
        self.dropTable("default",tbname)

        for column_name in invalid_name_array:
            if column_name == "":
                self.createTable(
                    dbname,
                    tbname,
                    {str(column_name):  {
                        "type":"float",
                    }},
                    {},
                    {
                        "status_code":500,
                        "error_code": 3042, #empty column name
                        #"error_message": "Duplicate table: {tbname} in {dbname}.",
                    }
                ) 
            else:
                self.createTable(
                    dbname,
                    tbname,
                    {
                        str(column_name): 
                        {
                            "type": "integer",
                            "constraints": []
                        },
                    },
                    {},
                    {
                        "status_code":500,
                        "error_code": 3005,
                        #"error_message": "Duplicate table: {tbname} in {dbname}.",
                    }
            )
        return
    
    def test_table_with_different_column_name(self):
        """
        test create/drop/show/get valid table name with different column names
        """
        dbname = "default"
        tbname = "my_table"
        self.showdb(dbname)
        self.dropTable("default",tbname)
        for column_name in invalid_name_array:
            #create table
            if column_name == '':
                continue
            self.createTable(
                dbname,
                tbname,
                {
                    str(column_name): 
                    {
                        "type": "integer" 
                    }
                },
                {},
                {
                    "status_code":500,
                    "error_code": 3005,
                    #"error_message": "Duplicate table: {tbname} in {dbname}.",
                }
            )
            #list table
            self.ListTables(dbname)
            #get tables
            self.showTable(dbname,tbname,expect={
                "status_code":500,
                "error_code": 3022,
            })
            #drop table
            self.dropTable(dbname,tbname)
        return
    
    def test_table_with_different_column_types(self):
        dbname = "default"
        tbname = "my_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {
                "c1":{"type":"BOOLEAN",},
                "c2":{"type":"INTEGER",},
                "c3":{"type":"TINYINT",},
                "c4":{"type":"SMALLINT",},
                "c5":{"type":"BIGINT",},
                "c6":{"type":"HUGEINT",},
                "c7":{"type":"FLOAT", },
                "c8":{"type":"DOUBLE",},
                "c9":{"type":"VARCHAR",},
                #"c10":{"type":"ARRAY",},
            }
        )
        self.ListTables(dbname)
        self.showTable(dbname,tbname,expect={
            "error_code": 0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : 9,
            "segment_count" : 0,
            "row_count" : 0
        })
        self.dropTable(dbname,tbname)
        return
    
    def test_table_with_various_column_types(self):
        dbname = "default"
        tbname = "my_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        c_count = 100
        types = [
            "integer", "tinyint", "smallint", "bigint", "hugeint", "float",
             "double", "varchar", "boolean"
        ]
        sz = len(types)
        # make params
        params = {}
        for i in range(c_count - sz):
            params.update({
                "c" + str(i):{"type": types[i%sz]}
            })
        
        # create tb with 10000 columns with various column types
        self.createTable(dbname,tbname,params)
        #list tables
        self.ListTables(dbname,{
            "error_code": 0,
        })
        #show tables
        self.showTable(dbname,tbname,{
            "error_code": 0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : c_count-sz,
        })
        #drop tables
        self.dropTable(dbname,tbname)
        return

    #INFO:if use invalid options, then infinity will use default options: kInvalid
    def test_table_with_different_invalid_options(self):
        dbname = "default"
        tbname = "test_table_with_different_invalid_options"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        #create tables
        url = f"databases/{dbname}/tables/{tbname}"
        h = self.SetUpHeader(['accept','content-type'])
        idx = 0
        for name in invalid_name_array:
            if idx == 0:
                d = self.SetUpData([],{'fields': {"c1": {"type": "integer",},}, 'properties': {},
                                    'create_option': str(name) })
                r = self.Request(url,"post",h,d)
                #print(r)
                expect = {"status_code":200,"error_code": 0}
                self.TearDown(r,expect)
            else: 
                d = self.SetUpData([],{'fields': {"c1": {"type": "integer",},}, 'properties': {},
                                    'create_option': str(name) })
                r = self.Request(url,"post",h,d)
                #print(r)
                expect = {"status_code":500,"error_code": 3017}
                self.TearDown(r,expect)
            idx+=1
      
        return 
    
    @pytest.mark.slow
    @pytest.mark.skip(reason="Cost too much times,and may cause the serve to terminate")
    def test_various_tables_with_various_columns(self):
        dbname = "default"
        tbname = "my_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        
        tb_count = 1000
        column_count = 10000

        types = [
            "integer", "tinyint", "smallint", "bigint", "hugeint", "float",
            "double", "varchar", "boolean",
        ]
        sz = len(types)
        # make params
        params = {}
        for i in range(column_count - sz):
            params.update({
                "c" + str(i):{"type": types[i%sz]}
            })

        for i in range(tb_count):
            self.createTable(
                dbname,
                "my_table"+str(i),
                params,
            )
        return
    def test_after_disconnect_use_table(self):
        pass 
    # def test_table_with_invalid_options(self):
    #     pass 
    def test_create_drop_table(self):
        dbname = "default"
        tbname = "my_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {"c1":{"type":"integer","constraints": ["primary key"],},"c2":{"type":"float",}}
        )
        self.createTable(
            dbname,
            tbname,
            {"c1":{"type":"integer","constraints": ["primary key"],},"c2":{"type":"float",}},
            {},
            {"status_code": 200,"error_code": 0, },
            "kIgnore",
        )

        self.createTable(
            dbname,
            tbname,
            {"c1":{"type":"integer","constraints": ["primary key"],},"c2":{"type":"float",}
            },
            {},
            {"status_code": 500,"error_code": 3017, },
            "kError",
        )

        self.dropTable(dbname,tbname)
        self.dropTable(dbname,tbname)

    @pytest.mark.skip(reason="Feature request")
    def test_describe_various_table(self):
        pass 
    
    def test_create_1K_table(self):
        dbname = "default"
        tbname = "my_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        tb_count = 100
        for i in range(tb_count):
            self.createTable(
                dbname,
                tbname+str(i),
                {"c1":{"type":"integer"}}
            ) 
        #get all tables
        res = self.GetAllTables(dbname)
        print("table nums: "+str(len(res)))
        #assert len(res) == tb_count

        #show table
        for i in range(tb_count):
            self.showTable(dbname,tbname+str(i),expect={
                "error_code": 0,
                "database_name": dbname,
                "table_name": tbname+str(i),
                #"column_count" : 1,
                "segment_count" : 0,
                "row_count" : 0
            }) 
        #drop table
        for i in range(tb_count):
            self.dropTable(dbname,tbname+str(i))
        return 
    

    # create/drop/list/get 1M table to reach the limit
    @pytest.mark.slow
    @pytest.mark.skip(reason="Cost too much times")
    def test_create_1M_table(self):
        dbname = "default"
        tbname = "my_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        tb_count = 1000000
        for i in range(tb_count):
            self.createTable(
                dbname,
                tbname+str(i),
                {"c1":{"type":"integer"}}
            ) 
        #get all tables
        res = self.GetAllTables(dbname)
        print("table nums: "+str(len(res)))
        #assert len(res) == tb_count

        #show table
        for i in range(tb_count):
            self.showTable(dbname,tbname+str(i),expect={
                "error_code": 0,
                "database_name": dbname,
                "table_name": tbname+str(i),
                #"column_count" : 1,
                "segment_count" : 0,
                "row_count" : 0
            }) 
        #drop table
        for i in range(tb_count):
            self.dropTable(dbname,tbname+str(i))
        return 
    

    def test_create_or_drop_same_table_in_different_thread(self):
        dbname = "default"
        tbname = "my_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into processpool
            futures = [executor.submit(self.createTable(
                dbname,
                tbname,
                {
                    "c1":{
                        "type":"integer",
                    }
                }
            ), i) for i in range(16)]
            # wait all processes finished
            concurrent.futures.wait(futures)

        # drop table
        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into threadpool
            futures = [executor.submit(self.dropTable(dbname,tbname), i) for i in range(16)]
            # wait all threads finished
            concurrent.futures.wait(futures)
        return 
    
    def test_create_empty_column_table(self):
        dbname = "default"
        tbname = "my_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,None,None,expect={
            "status_code":500,
            "error_code":3048,
        })
        return 
    
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
    
    def test_drop_option(self):
        dbname = "default"
        tbname = "test_drop_option"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        self.createTable(dbname,tbname,{"c1":{"type":"integer"}})
        self.dropTable(dbname,tbname)
        self.dropTable(
            dbname,
            tbname,
            {
                "status_code": 500,
                "error_code": 3022,
            },
            "kError"
        )
        return 
    
    def test_create_same_name_table(self):
        dbname = "default"
        tbname = "test_create_same_name"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{"c1": {'type':'integer'}})
        self.createTable(dbname,tbname,{"c1": {'type':'integer'}})
        self.createTable(dbname,tbname, {"c1": {'type':'integer'}},{},{
            "status_code":500,
            "error_code":3017,
        },"kError")
        return 
    
    def test_drop_same_name_table(self):
        dbname = "default"
        tbname = "test_drop_same_name"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.dropTable(dbname,tbname)
        self.dropTable(dbname,tbname,{
            "status_code": 500,
            "error_code": 3022,
        },"kError")
        return 
    
    def test_same_column_name(self):
        dbname = "default"
        tbname = "test_same_column_name"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        #self.createTable(dbname,tbname,{"c1": {'type':"integer"},"c1": {"type":"integer"}})
        url = f"databases/{dbname}/tables/{tbname}"
        h = self.SetUpHeader(['accept','content-type'])
        d = {'create_option': 'ignore_if_exists', 'fields': {'c1': {'type': 'integer'},'c1': {'type': 'integer'}}, 'properties': []}
        r = self.Request(url,"post",h,d)
        self.TearDown(r,{})
        return 
    
    def test_column_numbers(self):
        dbname = "default"
        tbname = "test_same_column_name"

        column_number = [0, 1, pow(2, 63) - 1]
        params = {}
        for i in column_number:
            params.update({
                "c" + str(i): {"type": types[i%len(types)]}
        })
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,params)
        return 
    
    def test_table_create_valid_option(self):
        dbname = "default"
        tbname = "test_table_create_valid_option"
    
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        for i  in create_valid_option:
            url = f"databases/{dbname}/tables/{tbname}"
            h = self.SetUpHeader(['accept','content-type'])
            d = self.SetUpData(['create_option'],{'fields': {"c1": {"type":"integer"}}, 'properties': {},'create_option':str(i)})
            r = self.Request(url,"post",h,d)
            print(r)
            self.TearDown(r,{
                "status_code":200,
                "error_code":0,    
            })
            self.dropTable(dbname,tbname)
        return 
    
    def test_table_create_invalid_option(self):
        dbname = "default"
        tbname = "test_table_create_invalid_option"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        for i  in create_invalid_option:
            url = f"databases/{dbname}/tables/{tbname}"
            h = self.SetUpHeader(['accept','content-type'])
            d = self.SetUpData(['create_option'],{'fields': {"c1": {"type":"integer"}}, 'properties': {},'create_option':str(i)})
            r = self.Request(url,"post",h,d)
            print(r)
            self.TearDown(r,{
                "status_code":200,
                "error_code":0,    
            })
            self.dropTable(dbname,tbname)
        return 
    def test_table_drop_valid_option(self):
        dbname = "default"
        tbname = "test_table_drop_valid_option"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        for i  in drop_valid_option:
            self.createTable(dbname,tbname+str(i), {"c1": {"type":"integer"}},{},{})
            self.dropTable(dbname,tbname+str(i),{
                "status_code":200,
                "error_code":0,  
            },i)
        return
    def test_table_drop_invalid_option(self):
        dbname = "default"
        tbname = "test_table_drop_invalid_option"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        for i  in drop_invalid_option:
            self.createTable(dbname,tbname, {"c1": {"type":"integer"}},{},{})

            url = f"databases/{dbname}/tables/{tbname}"
            h = self.SetUpHeader(['accept','content-type'])
            d = self.SetUpData(['drop_option'],{'drop_option':str(i)})
            r = self.Request(url,"delete",h,d)
            self.TearDown(r,{
                "status_code":200,
                "error_code":0,
            })
        return 
    
    def test_create_duplicated_table_with_ignore_option(self):
        dbname = "default"
        tbname = "test_create_duplicated_table_with_error_option"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        for i in range(100):
            self.createTable(dbname,tbname, {"c1": {"type":"integer"}})
        self.dropTable(dbname,tbname)
    
    def test_create_duplicated_table_with_error_option(self):
        dbname = "default"
        tbname = "test_create_duplicated_table_with_error_option"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        for i in range(100):
            if i == 0:
                self.createTable(dbname,tbname, {"c1": {"type":"integer"}})
            else:
                self.createTable(dbname,tbname, {"c1": {"type":"integer"}},{},{
                    "status_code": 500,
                    "error_code":3017
                },"kError")
        self.dropTable(dbname,tbname)
        return 
    
    #TODO: Replace options 
    def test_create_duplicated_table_with_replace_option(self):
        dbname = "default"
        tbname = "test_create_duplicated_table_with_error_option"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        for i in range(100):
                self.createTable(dbname,tbname, {"c1": {"type":"integer"}},{},{
                    "status_code": 200,
                    "error_code": 0 
                },"kIgnore")
        self.dropTable(dbname,tbname)
        return 