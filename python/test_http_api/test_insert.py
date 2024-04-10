import sys 
import pytest 
import os 
import signal
import time 
import concurrent.futures
from httpapibase import HttpTest
from common.common_values import * 

class TestInsert(HttpTest):
    def test_version(self):
        return 
    def test_insert_basic(self):
        dbname = "default"
        tbname = "table_2"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
                "constraints": ["primary key","not null"],
            },
            "c2":{
                "type":"integer",
                "constraints": ["not null"],
            }
        })

        self.insert(dbname,tbname,[{"c1":0,"c2":0}])
        self.insert(dbname,tbname,[{"c1":1,"c2":1}])
        self.insert(dbname,tbname,[{"c1":2,"c2":2}])
        self.insert(dbname,tbname,[{"c1":3,"c2":3},{"c1":4,"c2":4}])

        self.dropTable(dbname,tbname)
        return 
    #ERROR: make segment fault
    def test_insert_varchar(self):
        dbname = "default"
        tbname = "test_insert_varchar"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"varchar",
            }
        })
        self.insert(dbname,tbname,[{"c1": "aaa"}])
        self.insert(dbname,tbname,[{"c1": " test insert varchar "}])
        self.insert(dbname,tbname,[{"c1": "^789$ test insert varchar"}])

        self.dropTable(dbname,tbname)
        return 
    
    def test_insert_big_varchar(self):
        dbname = "default"
        tbname = "test_insert_big_varchar"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"varchar",
            }
        })
        for i in range(100):
            self.insert(dbname,tbname,[{"c1": "test_insert_big_varchar"*1000}])
        self.dropTable(dbname,tbname)
        return 
    def test_insert_embedding(self):
        dbname = "default"
        tbname = "test_insert_big_embedding"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"vector,3,int",
            }
        })
        self.insert(dbname,tbname,[{"c1": [1,2,3]}])
        self.insert(dbname,tbname,[{"c1": [4,5,6]}])
        self.insert(dbname,tbname,[{"c1": [7,8,9]}])
        self.insert(dbname,tbname,[{"c1": [-7,-8,-9]}])

        self.dropTable(dbname,tbname)
        return 
    #ERROR: create table failed,return 500
    def test_insert_big_embedding(self):
        dbname = "default"
        tbname = "test_insert_big_embedding"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"vector,65535,int",
            }
        })
        self.insert(dbname,tbname,[{"c1": [1]*65535}])
        self.insert(dbname,tbname,[{"c1": [4]*65535}])
        self.insert(dbname,tbname,[{"c1": [7]*65535}])
        self.insert(dbname,tbname,[{"c1": [-999999]*65535}])

        self.dropTable(dbname,tbname)
        return 

    def test_insert_big_embedding_float(self):
        dbname = "default"
        tbname = "test_insert_big_embedding_float"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"vector,65535,float",
            }
        })
        self.insert(dbname,tbname,[{"c1": [1]*65535}])
        self.insert(dbname,tbname,[{"c1": [-999999]*65535}])
        self.insert(dbname,tbname,[{"c1": [1.1]*65535}])
        self.insert(dbname,tbname,[{"c1": [-999999.988]*65535}])

        self.dropTable(dbname,tbname)
        return 

    def test_insert_big_embedding_various_type(self):
        types = ["vector,65535,int", "vector,65535,float"]
        types_examples = [[{"c1": [1] * 65535}],
                                                [{"c1": [4] * 65535}],
                                                [{"c1": [-9999999] * 65535}],
                                                [{"c1": [1.1] * 65535}],
                                                [{"c1": [-9999999.988] * 65535}],
                                                ]
        dbname = "default"
        tbname = "test_insert_big_embedding_various_type"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type": types
            }
        })
        self.insert(dbname,tbname,types_examples)
        self.dropTable(dbname,tbname)
        return 
    
    #PASS
    def test_insert_exceed_block_size(self):
        dbname = "default"
        tbname = "test_insert_exceed_block_size"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type": "float"
            }
        })
        values = [{"c1": 1} for _ in range(8193)]
        self.insert(dbname,tbname,values)
        self.dropTable(dbname,tbname)
        return 
    
    # insert primitive data type not aligned with table definition
    def test_insert_data_not_aligned_with_table_definition(self):
        dbname = "default"
        tbname = "test_insert_data_not_aligned_with_table_definition"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        for type in types:
            self.createTable(dbname,tbname,{
                "c1":{
                    "type": "integer",
                },
                "c2":{
                    "type": type, 
                }
            })
            values = [{"c1": 1, "c2": 1}]
            self.insert(dbname,tbname,values)
            self.dropTable(dbname,tbname)
        return 
    #ERROR: insert after drop table without wrong
    def test_insert_data_into_non_existent_table(self):
        dbname = "default"
        tbname = "test_insert_data_into_non_existent_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{"c1":{"type":"integer",},"c2":{"type":"integer",}})
        self.dropTable(dbname,tbname)

        values = [{"c1": 1, "c2": 1}]
        self.insert(dbname,tbname,values,expect={
            "status_code": 200,
            "error_code": 0,
        })
        return 
    
    def test_insert_empty_into_table(self):
        dbname = "default"
        tbname = "test_insert_empty_into_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        for t in types:
            self.createTable(dbname,tbname,{
                "c1":{
                    "type": "float"
                },
                "c2":{
                    "type": t  
                }
            })
            values = [{}]
            self.insert(dbname,tbname,values)
            self.dropTable(dbname,tbname)
        return 
    
    def test_insert_data_into_index_created_table(self):
        dbname = "default"
        tbname = "test_insert_data_into_non_existent_table"
        
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        #create table
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"vector,1024,float"
            }
        })
        #create index
        self.createIdx(dbname,tbname,"my_index_1",{
            "c1"
        },{
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "l2"
        })

        self.createIdx(dbname,tbname,"my_index_2",{
            "c1"
        },{
            "type": "IVFFlat",
            "centroids_count": "128",
            "metric": "l2",
        })
        #insert
        values = [{"c1": [1.1 for _ in range(1024)]}]
        self.insert(dbname,tbname,values)
        #delete
        self.dropTable(dbname,tbname)
        return 
    
    #PASS
    def test_insert_table_with_10000_columns(self):
        dbname = "default"
        tbname = "test_insert_table_with_10000_columns"
        
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        #create table
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
            "c2":{"type":"integer", }
        })
        #insert
        for i in range(100):
            values = [{"c1": i * 100 + j, "c2": i * 100 + j + 1} for j in range(100)]
            self.insert(dbname,tbname,values)
        return 
    #PASS
    def test_read_after_shutdown(self):
        dbname = "default"
        tbname = "test_read_after_shutdown"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.showTable(dbname,tbname,expect={
            "status_code":500,
            "error_code":3022,
        })
        values = [{"c1": 1, "c2": 1}]
        self.insert(dbname,tbname,values,expect={
            "status_code": 200,
            "error_code": 0,
        })
        return 
    #PASS
    def test_insert_with_not_matched_columns(self):
        values =  [[{"c1": 1}], [{"c1": 1, "c2": 1, "c3": 1}]]
        dbname = "default"
        tbname = "test_insert_with_not_matched_columns"
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
            "c2":{"type":"integer", }
        })
        self.insert(dbname,tbname,values,{
            "status_code": 500,
            "error_code": 3057,
        })
        self.dropTable(dbname,tbname)
        return 
    #@pytest.mark.parametrize("values", [[{"c1": pow(2, 63) - 1, "c2": pow(2, 63) - 1}]])
    def test_insert_with_exceeding_invalid_value_range(self):
        values = [[{"c1": pow(2, 63) - 1, "c2": pow(2, 63) - 1}]]
        dbname = "default"
        tbname = "test_insert_with_exceeding_invalid_value_range"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
            "c2":{"type":"integer", }
        })
        #insert
        self.insert(dbname,tbname,values,expect={
            "status_code": 500,
            "error_code": 3057,
        })
        self.dropTable(dbname,tbname)
        return 
    #PASS
    #@pytest.mark.parametrize("batch", [10, 1024, 2048])
    def test_batch_insert_within_limit(self):
        batch =  [10, 1024, 2048]
        dbname = "default"
        tbname = "test_batch_insert_within_limit"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
            "c2":{"type":"integer", }
        })
        for len in batch:
            values = [{"c1": 1, "c2": 2} for _ in range(len)]
            self.insert(dbname,tbname,values)
        self.dropTable(dbname,tbname)
        return 
    
    def test_batch_insert(self):
        dbname = "default"
        tbname = "test_batch_insert"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
            "c2":{"type":"integer", }
        })

        values = [{"c1": 1, "c2": 2} for _ in range(8192)]
        self.insert(dbname,tbname,values)

        self.dropTable(dbname,tbname)
        return 
    
    @pytest.mark.parametrize("batch", [10, 1024])
    @pytest.mark.parametrize("types", [(1, False), (1.1, False), ("1#$@!adf", False), ([1, 2, 3], True)])
    def test_insert_with_invalid_data_type(self, batch, types):
        dbname = "default"
        tbname = "test_insert_with_invalid_data_type"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
            "c2":{"type":"vector,3,int",}
        })
        for i in range(5):
            values = [{"c1": 1, "c2": types[0]} for _ in range(batch)]
            if not types[1]:
                self.insert(dbname,tbname,values,expect={
                    "status_code": 500,
                    "error_code": 3032,
                })
            else:
                self.insert(dbname,tbname,values)
        self.dropTable(dbname,tbname)
        return 
    
    @pytest.mark.parametrize("batch", [10, 1024])
    def test_batch_insert_with_invalid_column_count(self, batch=[]):
        dbname = "default"
        tbname = "test_batch_insert_with_invalid_column_count"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
        })
        for i in range(5):
             values = [{"c1": 1, "c2": 1} for _ in range(batch)]
             self.insert(dbname,tbname,values,expect={
                 "status_code":500,
                 "error_code":3022
             })
        self.dropTable(dbname,tbname)
        return
    
    @pytest.mark.parametrize('column_types', ["varchar"])
    @pytest.mark.parametrize('column_types_example', [[1, 2, 3], [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]])
    def test_various_insert_types(self, column_types, column_types_example):
        dbname = "default"
        tbname = "test_various_insert_types"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":column_types, },
        })
        self.showTable(dbname,tbname,expect={
            "error_code":0,
        })
        values = [{"c1": column_types_example} for _ in range(5)]
        self.insert(dbname,tbname,values)

        self.dropTable(dbname,tbname)
        return 
    
    #TODO
    @pytest.mark.complex
    @pytest.mark.skip(reason="TODO")
    def test_insert_and_shutdown_output(self):
        os.system("rm -fr /tmp/infinity")
        dbname = "default"
        tbname = "test_insert_and_shutdown_output"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
        })
        for i in range(10):
            values = [{"c1": 1} for _ in range(100)]
            self.insert(dbname,tbname,values)
        os.kill(os.getpid(), signal.SIGINT)
        time.sleep(1)

        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.showTable(dbname,tbname,{
            "status_code":404
        })
        os.kill(os.getpid(), signal.SIGINT)
        return
    
    #PASS
    def test_insert_zero_column(self):
        dbname = "default"
        tbname = "test_insert_zero_colum"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
        })
        self.insert(dbname,tbname,[],{
            "status_code":500,
            "error_code":3067
        })
        self.dropTable(dbname,tbname)
        return 
    
    @pytest.mark.parametrize("column_name", [
        "c2",
        "$%#$sadf",
        # 1,
        # 2.2,
        # [1],
        # (1, "adsf"),
        # {"1": 1}
    ])
    def test_insert_no_match_column(self, column_name):
        dbname = "default"
        tbname = "test_insert_zero_colum"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer", },
        })
        self.insert(dbname,tbname,[{column_name: 1}],{
            "status_code":500,
            "error_code":3024
        })
        self.dropTable(dbname,tbname)
        return 