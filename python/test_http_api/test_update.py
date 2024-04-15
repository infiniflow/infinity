import sys 
import pytest 
import time 
from httpapibase import HttpTest
from common.common_values import * 


class TestUpdate(HttpTest):
    def test_version(self):
        return
    #PASS
    def test_update(self):
        dbname = "default"
        tbname = "test_update"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
                "constraints": ["primary key","not null"],
            },
            "c2":{
                "type":"integer",
            },
            "c3":{
                "type":"integer",
            }
        })
        self.insert(dbname,tbname,
             [{"c1": 1, "c2": 10, "c3": 100}, {"c1": 2, "c2": 20, "c3": 200}, {"c1": 3, "c2": 30, "c3": 300},
             {"c1": 4, "c2": 40, "c3": 400}]
        )
        self.update(dbname,tbname,{"c2": 90, "c3": 900},"c1 = 1")
        self.select(dbname,tbname,
        ["c1","c2",],"",{},{},
        {
            "error_code":0,
            "output":[{'c1': '2', 'c2': '20'}, {'c1': '3', 'c2': '30'}, {'c1': '4', 'c2': '40'}, {'c1': '1', 'c2': '90'}]
        })
        self.update(dbname,tbname,{"c2": 90, "c3": 900},None,{
            "status_code":500,
            "error_code":3067,
        })
        return 
   
    def test_update_empty_table(self):
        dbname = "default"
        tbname = "test_update_empty_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{},{
            "status_code":500,
            "error_code":3022,
        })
        self.showTable(dbname,tbname,{
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : 0,
        })
        self.update(dbname,tbname,{"c2": 90, "c3": 900},"c1 = 1")
        self.dropTable(dbname,tbname)
        return 
    #PASS
    def test_update_non_existent_table(self):
        dbname = "default"
        tbname = "test_update_non_existent_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer",},
            "c2":{"type":"integer",},
            "c3":{"type":"integer",}
        })
        self.showTable(dbname,tbname,{
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : 3,
        })
        self.update(dbname,tbname,{"c2":90,"c3":900},"c1 = 1",{
            "status_code": 500,
            "error_code": 3069,
        })
        return 
    #PASS
    def test_update_no_row_is_met_the_condition(self):
        dbname = "default"
        tbname = "test_update_no_row_is_met_the_condition"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        types = [
            "integer", "tinyint", "smallint","bigint","float","double",#"hugeint"
        ]
        types_example = [
            1, 127, 32767,10,10.10,10.1010,#10
        ]

        for i in range(len(types)):
            self.createTable(dbname,tbname,{
                "c1":{"type":types[i]},
                "c2":{"type":types[i]}
            })
            self.showTable(dbname,tbname,{
                "error_code":0,
                "database_name": dbname,
                "table_name": tbname,
                "column_count" : 2,
            })
            self.insert(dbname,tbname,
                [{"c1": types_example[i],"c2": types_example[i]}]
            )
            self.update(dbname,tbname,{"c2": types_example[i]},"c1 = 2")
            self.dropTable(dbname,tbname)

        return 
    def test_update_all_row_is_met_the_condition(self):
        dbname = "default"
        tbname = "test_update_all_row_is_met_the_condition"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        types = [
            "integer", "tinyint", "smallint","bigint","float","double",#"hugeint"
        ]
        types_example = [
            1, 127, 32767,10,10.10,10.1010,#10
        ]

        for i in range(len(types)):
            self.createTable(dbname,tbname+str(i),{
                "c1":{"type":types[i]},
                "c2":{"type":types[i]}
            })
            self.showTable(dbname,tbname+str(i),{
                "error_code":0,
                "database_name": dbname,
                "table_name": tbname+str(i),
                "column_count" : 2,
            })
            self.insert(dbname,tbname+str(i),
                [{"c1": types_example[i],"c2": types_example[i]}]
            )
            self.update(dbname,tbname+str(i),{"c2": types_example[i]},"c1 = "+str(types_array[i]))
            self.dropTable(dbname,tbname+str(i))
        return 
    #PASS
    def test_update_table_with_one_block(self):
        dbname = "default"
        tbname = "test_update_all_row_is_met_the_condition"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        values = [{"c1": 1, "c2": 2} for _ in range(8192)]
        # values = [{"c1": 1, "c2": 2}]
        self.insert(dbname,tbname,values)
        self.update(dbname,tbname,{"c2":20},"c1 = 1")
        return
    
    def test_update_table_with_one_segment(self):
        dbname = "default"
        tbname = "test_update_all_row_is_met_the_condition"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        for _ in range(1024):
            values = [{"c1": 1, "c2": 2} for _ in range(8)]
            self.insert(dbname,tbname,values)
        self.update(dbname,tbname,{"c2":20},"c1 = 1")
        return
    
    def test_update_before_delete(self):
        dbname = "default"
        tbname = "test_update_before_delete"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer",},
            "c2":{"type":"integer",}
        })

        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        self.insert(dbname,tbname,values)

        self.delete(dbname,tbname,"c1 = 1")
        self.update(dbname,tbname,{"c2": 20},"c1 = 1")
        return
    
    def test_update_inserted_data(self):
        dbname = "default"
        tbname = "test_update_before_delete"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer",},
            "c2":{"type":"integer",}
        })
        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        self.insert(dbname,tbname,values)
        self.update(dbname,tbname,{"c2":21},"c1 = 1")

        return
    
    @pytest.mark.slow
    @pytest.mark.skip(reason="Taking too much time.")
    def test_update_inserted_long_before(self):
        dbname = "default"
        tbname = "test_update_before_delete"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer",},
            "c2":{"type":"integer",}
        })
        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        self.insert(dbname,tbname,values)

        time.sleep(3600)

        self.update(dbname,tbname,{"c2": 21},"c1 = 1")
        return 
    
    def test_update_dropped_table(self):
        dbname = "default"
        tbname = "test_update_before_delete"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer",},
            "c2":{"type":"integer",}
        })
        self.update(dbname,tbname,{"c2":21},"c1 = 1")
        self.dropTable(dbname,tbname)
        return 
    
    def test_update_invalid_value(self):
        dbname = "default"
        tbname = "test_update_before_delete"
        types = [
            "varchar"
        ]
        types_example = [
            [1,2,3]
        ]
        for i in range(len(types)):
            self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":{
                    "type":str(types[i]),
                }
            }) 
            self.update(dbname,tbname,{"c2": types_example[i]},"c1 = 1",{
                "status_code":500,
                "error_code":3067,
            })
            self.dropTable(dbname,tbname)
        return 

    def test_update_new_value(self):
        types = ["integer","float"]
        types_example = [1,1.333,"1"]
        dbname = "default"
        tbname = "test_update_before_delete"
        self.dropTable(dbname,tbname)
        for i in range(len(types)):
            self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":{
                    "type":str(types[i]),
                }
            }) 
            self.update(dbname,tbname,{"c2": types_example[i]},"c1 = 1")
            self.dropTable(dbname,tbname)
        return 
   
    def test_valid_filter_expression(self):
        filter_list = [
            "c1 > 10",
            "c2 > 1",
            "c1 > 0.1 && c2 < 3.0",
            "c1 > 0.1 && c2 < 1.0",
            "c1 < 0.1 && c2 < 1.0",
            "c1 < 0.1 && c1 > 1.0",
            "c1 = 0",
        ]
        types_example = ["1","1.333"]

        dbname = "default"
        tbname = "test_valid_filter_expression"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":{
                    "type":"float",
                }
        }) 
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            self.insert(dbname,tbname,values)

        for i in range(len(filter_list)):
            self.update(dbname,tbname,{"c2": 1},filter_list[i])
        return
    def test_invalid_filter_expression(self):
        filter_list = [
            "_row_id",
            "#@$%@#f",
            "c1 + 0.1 and c2 - 1.0","c1 * 0.1 and c2 / 1.0","c1 > 0.1 %@#$sf c2 < 1.0",
        ]
        types_example = [1,1.333]

        dbname = "default"
        tbname = "test_valid_filter_expression"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":{
                    "type":"float",
                }
        }) 
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            self.insert(dbname,tbname,values)
        for i in range(len(filter_list)):
            self.update(dbname,tbname,{"c2": types_example[i%2]},filter_list[i],{
                "status_code":500,
                "error_code":3063,
            })
        return
    