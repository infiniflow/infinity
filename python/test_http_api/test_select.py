import sys 
import os 
import pytest 
from httpapibase import HttpTest
from common.common_values import * 
import httputils


class TestSelect(HttpTest):
    def test_version(self):
        httputils.check_data(TEST_TMP_DIR)
        return 
    #ERROR
    def test_select(self):
        dbname = "default"
        tbname = "test_select"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                    "constraints": ["primary key","not null"]
                },
                "c2":{
                    "type":"integer",
                    "constraints": ["not null"]
                }
            }
        )
        self.insert(dbname,tbname,[{"c1": -3, "c2": -3}, {"c1": -2, "c2": -2}, {"c1": -1, "c2": -1}, {"c1": 0, "c2": 0}, {"c1": 1, "c2": 1},
             {"c1": 2, "c2": 2}, {"c1": 3, "c2": 3}])
        self.insert(dbname,tbname,[{"c1": -8, "c2": -8}, {"c1": -7, "c2": -7}, {"c1": -6, "c2": -6}, {"c1": 7, "c2": 7}, {"c1": 8, "c2": 8},
             {"c1": 9, "c2": 9}])
        
        self.select(dbname,tbname,[
            "c1","c2",
        ],"",{},{},{
            "error_code":0,
            "output": [{'c1': '-3', 'c2': '-3'}, {'c1': '-2', 'c2': '-2'}, {'c1': '-1', 'c2': '-1'}, {'c1': '0', 'c2': '0'}, {'c1': '1', 'c2': '1'}, {'c1': '2', 'c2': '2'}, {'c1': '3', 'c2': '3'}, {'c1': '-8', 'c2': '-8'}, {'c1': '-7', 'c2': '-7'}, {'c1': '-6', 'c2': '-6'}, {'c1': '7', 'c2': '7'}, {'c1': '8', 'c2': '8'}, {'c1': '9', 'c2': '9'}]
        })

        self.select(dbname,tbname,[
            "c1","c2",
        ],"",{},{},{
            "error_code":0,
            'output': [{'c1': '-3', 'c2': '-3'}, {'c1': '-2', 'c2': '-2'}, {'c1': '-1', 'c2': '-1'}, {'c1': '0', 'c2': '0'}, {'c1': '1', 'c2': '1'}, {'c1': '2', 'c2': '2'}, {'c1': '3', 'c2': '3'}, {'c1': '-8', 'c2': '-8'}, {'c1': '-7', 'c2': '-7'}, {'c1': '-6', 'c2': '-6'}, {'c1': '7', 'c2': '7'}, {'c1': '8', 'c2': '8'}, {'c1': '9', 'c2': '9'}]
        })

        self.select(dbname,tbname,[
            "c1 + c2",
        ],"c1 = 3",{},{},{
            "error_code":0,
            "output":[
                {'(c1 + c2)': '6'}
            ]
        })

        #TODO: fix me

        # self.select(dbname,tbname,[
        #     "c1",
        # ],"c1 > 2 && c2 < 4",{},{},{
        #     "error_code":0,
        #     "output":[
        #         {'c1': '3'}, {'c1': '7'}, {'c1': '8'}, {'c1': '9'}
        #     ]
        # })
        # self.select(dbname,tbname,[
        #     "c2",
        # ],"(c1 > -7 || c1 >= 9) && c1 = 3",{},{},{
        #     "error_code":0,
        #     "output":[
        #         {'c2': (3,)},
        #     ]
        # })

    #PASS
    def test_select_aggregate(self):
        dbname = "default"
        tbname = "test_select"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                    "constraints": ["primary key","not null"]
                },
                "c2":{
                    "type":"float",
                    "constraints": ["not null"]
                }
            }
        )
        self.insert(dbname,tbname,
            [{"c1": -30, "c2": -1.43}, {"c1": -2, "c2": -2.5}, {"c1": 42, "c2": -5.1}, {"c1": 0, "c2": 0.0},
             {"c1": 1, "c2": 1.0},{"c1": 2, "c2": 2.0}, {"c1": 3, "c2": 3.0}])
        self.insert(dbname,tbname,
            [{"c1": -8, "c2": -8.0}, {"c1": -13, "c2": -7.5}, {"c1": -6, "c2": 1.1}, {"c1": 17, "c2": 0.47},
             {"c1": 85, "c2": 62.0},{"c1": 90, "c2": -19.0}])
        
        self.select(dbname,tbname,
            ["count(*)"],"",
            {},{},{
            "error_code":0,
            'output': [{'count(star)': '13'}]
        })
        return
    
    #ERROR: insert varchar leads segment fault
    @pytest.mark.skip(reason="make critical error")
    def test_select_varchar(self):
        dbname = "default"
        tbname = "test_select_varchar"
        self.showdb(dbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"varchar",
                    "constraints": ["primary key","not null"]
                },
                "c2":{
                    "type":"varchar",
                    "constraints": ["not null"]
                }
            }
        )
        self.showTable(dbname,tbname,{
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count":2,
        })

        self.insert(dbname,tbname,
            [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'},
             {"c1": 'e', "c2": 'e'}, {"c1": 'f', "c2": 'f'}, {
                 "c1": 'g', "c2": 'g'}, {"c1": 'h', "c2": 'h'},
             {"c1": 'i', "c2": 'i'}, {"c1": 'j', "c2": 'j'}, {
                 "c1": 'k', "c2": 'k'}, {"c1": 'l', "c2": 'l'},
             {"c1": 'm', "c2": 'm'}])
        self.select(dbname,tbname,["*"])
        return 
    
    #ERROR: insert varchar leads segment fault
    @pytest.mark.skip(reason="make critical error")
    def test_select_big(self):
        dbname = "default"
        tbname = "test_select_big"
        self.showdb(dbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"varchar",
                    "constraints": ["primary key","not null"]
                },
                "c2":{
                    "type":"varchar",
                    "constraints": ["not null"]
                }
            }
        )
        for i in range(1000):
            self.insert(dbname,tbname,[
                {"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'}
            ])
        
        
        return
    
    #PASS
    def test_select_embedding_int32(self):
        httputils.check_data(TEST_TMP_DIR)
        dbname = "default"
        tbname = "test_select_embedding_int32"
        filename = "embedding_int_dim3.csv"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"varchar",
                    "constraints": ["primary key","not null"]
                },
                "c2":{
                    "type":"varchar",
                    "dimension":3,
                    "element_type":"integer",
                }
            }
        )
        httputils.copy_data("embedding_int_dim3.csv")
        test_csv_dir = TEST_TMP_DIR + filename
        assert os.path.exists(test_csv_dir)

        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })

        self.select(dbname,tbname,["c2",],"",{},{},{
            "error_code":0,
            "output":[{'c2': '[2 3 4]'}, {'c2': '[6,7,8]'}, {'c2': '[10,11,12]'}]
        })
        self.dropTable(dbname,tbname)
        return
    #PASS
    def test_select_embedding_float(self):
        httputils.check_data(TEST_TMP_DIR)
        dbname = "default"
        tbname = "test_select_embedding_int32"
        filename = "embedding_float_dim4.csv"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"varchar",
                    "constraints": ["primary key","not null"]
                },
                "c2":{
                    "type":"varchar",
                    "dimension":4,
                    "element_type":"float",
                }
            }
        )
        httputils.copy_data(filename)
        test_csv_dir = TEST_TMP_DIR + filename
        assert os.path.exists(test_csv_dir)

        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })

        self.select(dbname,tbname,["c2"],"",{},{},{
            "error_code":0,
            "output":[
                {'c2': '[0.1, 0.2, 0.3, -0.2]'}, 
                {'c2': '[0.2, 0.1, 0.3, 0.4]'}, 
                {'c2': '[0.3, 0.2, 0.1, 0.4]'}, 
                {'c2': '[0.4, 0.3, 0.2, 0.1]'}
            ]
        })
        self.dropTable(dbname,tbname)
        return
    #PASS
    def test_select_big_embedding(self):
        httputils.check_data(TEST_TMP_DIR)
        dbname = "default"
        tbname = "test_select_embedding_int32"
        filename = "embedding_int_dim3.csv"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"varchar",
                    "constraints": ["primary key","not null"]
                },
                "c2":{
                    "type":"varchar",
                    "dimension":3,
                    "element_type":"integer",
                }
            }
        )
        httputils.copy_data(filename)
        csv_test_dir = TEST_TMP_DIR + filename
        assert os.path.exists(csv_test_dir)

        for i in range(1000):
            self.importData(dbname,tbname,{
                 "file_path":csv_test_dir,
                 "file_type":"csv",
                 "header":False,
                 "delimiter":","
            })
        self.select(dbname,tbname,["c1"],"",{
        },{},{
            "error_code":0,
        })
        self.dropTable(dbname,tbname)
        return 
    #PASS
    def test_select_same_output(self):
        dbname = "default"
        tbname = "test_select_same_output"
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
            }
        )

        self.insert(dbname,tbname,[{"c1": 1, "c2": 2}])
        self.select(dbname,tbname,["c1","c2"],"",{},{},{
            "error_code":0,
            "output":[{"c1": '1', "c2": '2'}]
            
        })
        self.select(dbname,tbname,["c1","c1"],"",{
            "match":
            {
                "fields": "c2", 
                "query": "bloom",
                "operator": "topn=1"
            }
        },{},{
            "error_code":0,
        })
        self.select(dbname,tbname,["*"],"",{},{},{
            "error_code":0,
            "output":[{"c1": '1', "c2": '2'}]
        })
        self.dropTable(dbname,tbname)
        return
    
    #PASS
    def test_empty_table(self):
        dbname = "default"
        tbname = "test_empty_table"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":{
                    "type":"integer",
                }
            }
        )
        
        self.select(dbname,tbname,["c1","c2"])
        return 
    #PASS
    def test_valid_filter_expression(self):
        filter_list = [
            "c1 > 10",
            "c2 > 1",
            "c1 > 0.1 and c2 < 3.0",
            "c1 > 0.1 and c2 < 1.0",
            "c1 < 0.1 and c2 < 1.0",
            "c1 < 0.1 and c1 > 1.0",
            "c1 = 0",
        ]
        dbname = "default"
        tbname = "test_valid_filter_expression"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":{
                    "type":"float",
                }
            }
        )
        self.insert(dbname,tbname,[{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])
        for f in filter_list:
            self.select(dbname,tbname,["c1","c2"])
        self.dropTable(dbname,tbname)
        return 
    
    #ERROR
    @pytest.mark.skip(reason="invalid filter lead no error")
    def test_invalid_filter_expression(self):
        filter_list = [
            "c1",
            "row_id",
            "*",
            "#@$%@#f",
            "c1 + 0.1 and c2 - 1.0",
            "c1 * 0.1 and c2 / 1.0",
            "c1 > 0.1 %@#$sf c2 < 1.0",
        ]
        dbname = "default"
        tbname = "test_valid_filter_expression"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":{
                    "type":"float",
                }
            }
        )
        #TODO:fix
        self.insert(dbname,tbname,[{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])
        for f in filter_list:
            self.select(dbname,tbname,["*"],f,{},{},{
                "status_code":500,
            })
        self.dropTable(dbname,tbname)
        return 
    
     



