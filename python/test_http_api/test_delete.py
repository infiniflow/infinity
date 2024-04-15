import sys 
import pytest 
import time 
from httpapibase import HttpTest
from common.common_values import * 

class TestDelete(HttpTest):
    def test_version(self):
        
        return 
    #pass
    def test_delete(self):
        dbname = "default"
        tbname = "test_delete"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
                "constraints": ["primary key","not null"]
            },
            "c2":{
                "type":"integer",
            }
        })
        self.showTable(dbname,tbname,{
            "error_code" : 0,
        })
        self.insert(dbname,tbname,[{"c1": 1, "c2": 10, "c3": 100}, {"c1": 2, "c2": 20, "c3": 200}, {"c1": 3, "c2": 30, "c3": 300},
             {"c1": 4, "c2": 40, "c3": 400}])
        self.delete(dbname,tbname,"c1 = 1")
        # self.select(dbname,tbname,[
        #     "c1,c2",
        # ],"",{},{
        #     "error_code":0,
        #     "output":[
        #         {'c1': (), 'c2': (), 'c3': ()}
        #     ]
        # })
        self.dropTable(dbname,tbname)
        return 
    
    #PASS  error: delete empty table without any error resp
    def test_delete_empty_table(self):
        dbname = "default"
        tbname = "test_delete_empty_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
                "constraints": ["primary key","not null"]
            },
        })
        self.showTable(dbname,tbname,{
            "error_code" : 0,
        })
        self.delete(dbname,tbname,"c1=1",{
            "status_code":200,
            "error_code":0,
        })
        self.dropTable(dbname,tbname)
        return 
    #pass
    def test_delete_non_existent_table(self):
        dbname = "default"
        tbname = "test_delete_non_existent_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.delete(dbname,tbname,"",{
            "status_code":500,
            "error_code":3063,
        })
        return 
    #PASS
    def test_delete_table_all_row_met_the_condition(self):
        dbname = "default"
        tbname = "test_delete_table_all_row_met_the_condition"
        types = [
            "integer", "tinyint", "smallint","bigint","float","double",#"hugeint"
        ]
        types_example = [
            1, 127, 32767,10,10.10,10.1010,#10
        ]
        self.showdb(dbname)
        i = 0
        for t in types:
            self.dropTable(dbname,tbname)
            self.createTable(dbname,tbname,{
                "c1":{"type":str(t)
                }
            })
            self.showTable(dbname,tbname,{
                "error_code": 0,
            })
            self.insert(dbname,tbname,[
                {"c1": str(types_example[i])}
            ])
            self.delete(dbname,tbname,"c1=" + str(types_example[i]-1))
            i+=1 
        return 
    #PASS
    def test_delete_table_no_rows_met_condition(self):
        dbname = "default"
        tbname = "test_delete_table_no_rows_met_condition"
        types = [
            "integer", "tinyint", "smallint","bigint","float","double",#"hugeint"
        ]
        types_example = [
            1, 127, 32767,10,10.10,10.1010,#10
        ]
        for i in range(len(types)):
            self.dropTable(dbname,tbname+str(i))
        for i in range(len(types)):
            self.createTable(dbname,tbname+str(i),{
                "c1":{"type":str(types[i])
                }
            })
            self.showTable(dbname,tbname+str(i),{
                "error_code":0,
            })
            self.insert(dbname,tbname+str(i),[
                {"c1": str(types_example[i])}
            ])
            self.delete(dbname,tbname+str(i),"c1 = 0")
        for i in range(len(types)):
            self.dropTable(dbname,tbname+str(i))
        return
    #PASS
    def test_delete_table_with_one_block(self):
        dbname = "default"
        tbname = "test_delete_table_with_one_block"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{"c1":{"type":"integer"}},)
        values = [{"c1": 1} for _ in range(8192)]
        self.insert(dbname,tbname,values)
        self.delete(dbname,tbname,"c1 = 1")
        self.dropTable(dbname,tbname)
        return
    #PASS
    def test_delete_table_with_one_segment(self):
        dbname = "default"
        tbname = "test_delete_table_with_one_segment"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{"c1":{"type":"integer"}},)
        for i in range(1024):
            values = [{"c1": i} for _ in range(10)]
            self.insert(dbname,tbname,values)
        for i in range(1024):
            self.delete(dbname,tbname,"c1 = " + str(i))
        self.dropTable(dbname,tbname)
        return 
    #PASS
    def test_select_before_after_delete(self):
        dbname = "default"
        tbname = "test_select_before_after_delete"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{"c1":{"type":"integer"}},)

        for i in range(10):
            values = [{"c1": i} for _ in range(10)]
            self.insert(dbname,tbname,values)
        self.delete(dbname,tbname,"c1=1")
        self.dropTable(dbname,tbname)
        return 
    #PASS
    def test_delete_insert_data(self):
        dbname = "default"
        tbname = "test_delete_insert_data"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{"c1":{"type":"integer"}},)

        values = [{"c1": 1} for _ in range(10)]
        self.insert(dbname,tbname,values)

        self.delete(dbname,tbname,"c1 = 1")
        self.dropTable(dbname,tbname)
        return 
    
    @pytest.mark.slow
    def test_delete_inserted_long_before_data(self):
        dbname = "default"
        tbname = "test_delete_inserted_long_before_data"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{"c1":{"type":"integer"}},)

        for i in range(1024):
            values = [{"c1": i} for _ in range(5)]
            self.insert(dbname,tbname,values)
        time.sleep(5)
        self.delete(dbname,tbname,"c1 = 1")
        self.dropTable(dbname,tbname)
        return 
    
    # Make Critical Error
    @pytest.mark.skip(reason="make critical error")
    def test_delete_dropped_table(self):
        dbname = "default"
        tbname = "test_delete_dropped_table"
        self.dropTable(dbname,tbname)
        self.showTable(dbname,tbname,{
            "status_code":500,
            "error_code":3022,
        })
        self.delete(dbname,tbname,"c1 = 0",{
                "status_code":500,
                "error_code":3057,
        })
        self.dropTable(dbname,tbname)
        return
    
    def test_various_expression_in_where_clause(self):
        dbname = "default"
        tbname = "test_various_expression_in_where_clause"
        self.dropTable(dbname,tbname)
        types = [
            "integer", "tinyint", "smallint","bigint","float","double"
        ]
        types_example = [
            1, 127, 32767,10,10.10,10.1010,
        ]
        for i in range(len(types)):
            self.createTable(dbname,tbname,{
                "c1":{
                    "type":str(types[i]),
                }
            })
            self.showTable(dbname,tbname,{
            "error_code":0,
            })
            values = [{"c1": types_example[i]} for _ in range(5)]
            self.insert(dbname,tbname,values)
            self.delete(dbname,tbname,"c1 = " + str(types_example[i]))
            self.dropTable(dbname,tbname)
        return
    
    @pytest.mark.skip(reason="make critical error")
    def test_delete_one_block_without_expression(self):
        dbname = "default"
        tbname = "test_delete_one_block_without_expression"
        self.createTable(dbname,tbname,{"c1":{"type":"integer",}})
        self.dropTable(dbname,tbname)
        values = [{"c1": 1} for _ in range(8192)]
        self.insert(dbname,tbname,values)
        self.delete(dbname,tbname,"c1 = 1")
        self.dropTable(dbname,tbname)
        return
    
  
    def test_delete_one_segment_without_expression(self):
        dbname = "default"
        tbname = "test_delete_one_segment_without_expression"
        self.createTable(dbname,tbname,{"c1":{"type":"integer",}})
        for i in range(1024):
            values = [{"c1": i} for _ in range(10)]
            self.insert(dbname,tbname,values)
        self.dropTable(dbname,tbname)
        return
    
    
    def test_filter_with_valid_expression(self):
        filter_list = [ 
            "c1 > 10",
            "c2 > 1",
            "c1 > 0.1 && c2 < 3.0",
            "c1 > 0.1 && c2 < 1.0",
            "c1 < 0.1 && c2 < 1.0",
            "c1 < 0.1 && c1 > 1.0",
            "c1 = 0",
        ]
        dbname = "default"
        tbname = "test_delete_one_segment_without_expression"
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer",},
            "c2":{"type":"float"}}
        )
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            self.insert(dbname,tbname,values)
        for i in range(len(filter_list)):
            self.delete(dbname,tbname,filter_list[i])
        self.dropTable(dbname,tbname)
        return
    @pytest.mark.skip(reason="make critical error")
    def test_filter_with_invalid_expression(self):
        filter_list = [ 
            "c1",
            "_row_id",
            "*",
            "#@$%@#f",
            "c1 + 0.1 and c2 - 1.0",
            "c1 * 0.1 and c2 / 1.0",
            "c1 > 0.1 %@#$sf c2 < 1.0",
        ]
        dbname = "default"
        tbname = "test_filter_with_invalid_expression"
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer",},
            "c2":{"type":"float"}}
        )
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            self.insert(dbname,tbname,values)

        for i in range(len(filter_list)):
            self.delete(dbname,tbname,filter_list[i],{
                "status_code":500,
                "error_code":3022,
            })
        self.dropTable(dbname,tbname)
        return 
