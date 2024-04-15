import sys 
import pytest 
import time 
from httpapibase import HttpTest
from common.common_values import * 


class TestShow(HttpTest):
    def test_show_table(self):
        dbname = "default"
        tbname = "test_show_table"
        self.dropTable(dbname,tbname)
        self.showTable(dbname,tbname,{
            "status_code":500,
            "error_code":3022,
        })
        self.createTable(
            dbname,tbname,{
                "num":{
                    "type":"integer",
                },
                 "body":{
                    "type":"varchar",
                },"vec":{
                    "type":"vector",
                    "dimension":5,
                    "element_type":"float",
                }
            }
        )
        self.showTable(dbname,tbname,{
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : 3,
            "row_count" : 0
        })
        return
    
    def test_show_cloumns(self):
        dbname = "default"
        tbname = "test_show_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "num":{
                "type":"integer",
            },
            "body":{
                "type":"varchar",
            },"vec":{
                "type":"vector",
                "dimension":5,
                "element_type":"float",
            }
        })

        self.showTableColums(dbname,tbname,{
            "error_code": 0,
            "columns":[
                {'column_name': 'body', 'column_type': 'Varchar', 'constraint': ''}, 
                {'column_name': 'num', 'column_type': 'Integer', 'constraint': ''}, 
                {'column_name': 'vec', 'column_type': 'Embedding(float,5)', 'constraint': ''}
            ]
        })
        return 
    