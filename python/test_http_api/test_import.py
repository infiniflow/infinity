import os 
import sys 
import pytest 
from httpapibase import HttpTest
from common.common_values import * 
import infinity.index as index
import httputils

class TestImport(HttpTest):
    def test_version(self):
        return
    def test_import(self):
        file_name = "csv/embedding_int_dim3.csv"
        dbname = "default"
        tbname = "test_import"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
            },
            "c2":{
                "type":"vector",
                "dimension":3,
                "element_type":"integer",
            }
        })

        test_csv_dir = self.get_project_path() + TEST_DATA_DIR +  file_name
        print("test_csv_dir: "+ test_csv_dir )
        assert os.path.exists(test_csv_dir)

        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })

        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
            },
            "c2":{
                "type":"vector",
                "dimension":4,
                "element_type":"float",
            }
        })

        file_path = self.get_project_path() + TEST_DATA_DIR + "csv/embedding_float_dim4.csv"
        assert os.path.exists(file_path)
        self.importData(dbname,tbname,{
            "file_path":file_path,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })

        self.dropTable(dbname,tbname)
        return
    
    def  test_import_different_file_format_data(self):
        dbname = "default"
        tbname = "test_import_different_file_format_data"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":{
                    "type":"vector",
                    "dimension":3,
                    "element_type":"integer",
                }
        })

        file_format = ["csv","fvecs"]
        for format in file_format:
            if format == "fvecs":
                self.dropTable(dbname,tbname)
                self.createTable(dbname,tbname,{
                        "c1":{
                            "type":"integer",
                        },
                        "c2":{
                            "type":"vector",
                            "dimension":128,
                            "element_type":"float",
                        }
                })
                file_path = self.get_project_path() + TEST_DATA_DIR + format +"/http_test." + format
                assert os.path.exists(file_path)
                self.importData(dbname,tbname,{
                    "file_path":file_path,
                    "file_type":format,
                    "header":False,
                    "delimiter":","
                })
                print(file_path)
            else:
                file_path = self.get_project_path() + TEST_DATA_DIR + format + "/embedding_int_dim3." + format
                assert os.path.exists(file_path)
                self.importData(dbname,tbname,{
                    "file_path":file_path,
                    "file_type":format,
                    "header":False,
                    "delimiter":","
                })
        self.dropTable(dbname,tbname)
        return
    def test_import_empty_file_fvecs(self):
        dbname = "default"
        tbname = "test_import_different_file_format_data"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
            },
            "c2":{
                "type":"vector",
                "dimension":1024,
                "element_type":"float",
            }
        })
        file_path = self.get_project_path() + TEST_DATA_DIR + "fvecs/test_empty.fvecs"
        assert os.path.exists(file_path)
        self.importData(dbname,tbname,{
            "file_path":file_path,
            "file_type":"fvecs",
            "header":False,
            "delimiter":","
        },{
            "status_code":500,
            "error_code":3037,
        })
        self.dropTable(dbname,tbname)
        return
    
    def test_import_empty_file_csv(self):
        dbname = "default"
        tbname = "test_import_different_file_format_data"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
            },
            "c2":{
                "type":"vector","dimension":3,"element_type":"integer",
            }
        })
        file_path = self.get_project_path() + TEST_DATA_DIR + "csv/test_empty.csv"
        assert os.path.exists(file_path)

        self.importData(dbname,tbname,{
            "file_path":file_path,"file_type":"csv","header":False,"delimiter":","
        },{
            "status_code":200,"error_code":0,
        })
        self.dropTable(dbname,tbname)
        return
    def test_import_empty_file_jsonl(self):
        dbname = "default"
        tbname = "test_import_different_file_format_data"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
            },
            "c2":{
                "type":"vector","dimension":3,"element_type":"integer",
            }
        })

        file_path = self.get_project_path() + TEST_DATA_DIR + "json/test_empty.json"
        self.importData(dbname,tbname,{
            "file_path":file_path,"file_type":"json","header":False,"delimiter":","
        },{
            "status_code":500,"error_code":3032,
        })
        assert os.path.exists(file_path)
        self.dropTable(dbname,tbname)
        return
    
    def test_import_format_unrecognized_data(self):
        file_format = ["json","txt"]
        dbname = "default"
        tbname = "test_import_format_unrecognized_data"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
            },
            "c2":{
                "type":"vector","dimension":3,"element_type":"integer",
            }
        })
        for format in file_format:
            file_path = self.get_project_path() + TEST_DATA_DIR + format +"/http_test." + format
            assert os.path.exists(file_path)
            self.importData(dbname,tbname,{
                    "file_path":file_path,
                    "file_type":format,
                    "header":False,
                    "delimiter":","
            },{
                "status_code":500,
                "error_code":3032,
            })
        self.dropTable(dbname,tbname)
        return
    
    def test_csv_with_different_delimiter(self):
        
       
        dbname = "default"
        tbname = "test_csv_with_different_delimiter"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
      
        for i in range(len(delimiter)):
                self.createTable(dbname,tbname,{
                    "c1":{"type":"integer",},
                    "c2":{"type":"integer"},
                })
                file_path = self.get_project_path() +TEST_DATA_DIR +"/csv/"+ check_file_data[i]["file_name"]
                assert os.path.exists(file_path)
                self.importData(dbname,tbname,{
                    "file_path":file_path,
                    "file_type":"csv",
                    "header":False,
                    "delimiter":delimiter[i][1],
         })
        return
    def test_csv_with_different_delimiter_more_than_one_character(self):
        dbname = "default"
        tbname = "test_csv_with_different_delimiter_more_than_one_character"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)


        self.createTable(dbname,tbname,{
                    "c1":{"type":"integer",},
                    "c2":{"type":"integer"},
                })
        file_path = self.get_project_path() +TEST_DATA_DIR +"/csv/pysdk_test_blankspace.csv" 
        assert os.path.exists(file_path)
        self.importData(dbname,tbname,{
            "file_path":file_path,
            "file_type":"csv",
            "header":False,
            "delimiter":' ',}
        )
        self.dropTable(dbname,tbname)
        return
    
    def test_import_csv_with_headers(self):
        file_name = "csv/embedding_int_dim3.csv"
        dbname = "default"
        tbname = "test_import"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
            },
            "c2":{
                "type":"vector",
                "dimension":3,
                "element_type":"integer",
            }
        })

        test_csv_dir = self.get_project_path() + TEST_DATA_DIR +  file_name
        print("test_csv_dir: "+ test_csv_dir )
        assert os.path.exists(test_csv_dir)

        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":True,
            "delimiter":","
        })
        return
    
    def test_import_fvecs_table_with_more_columns(self):
        file_name = "http_test.fvecs"
        httputils.generate_fvecs(100,128,file_name)
        httputils.copy_data(file_name)
        dbname = "default"
        tbname = "test_import_fvecs_table_with_more_columns"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        self.createTable(dbname,tbname,{
            "c1":{
                "type":"integer",
            },
            "c2":{
                "type":"vector",
                "dimension":128,
                "element_type":"float",
            }
        })
        test_csv_dir = TEST_TMP_DIR + file_name
        print("test_file_dir: "+test_csv_dir)
        assert os.path.exists(test_csv_dir)

        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"fevcs",
            "header":False,
            "delimiter":","
        })
        self.dropTable(dbname,tbname)
        return
    #PASS
    def test_import_embedding_with_not_match_definition(self):
        types =  [{"type":"vector","dimension":3,"element_type":"integer",
        },{"type":"vector","dimension":128,"element_type":"integer",
        },{"type":"vector","dimension":3,"element_type":"float",
        },{"type":"vector","dimension":128,"element_type":"float",
        },{"type":"vector","dimension":3,"element_type":"double",
        }]
        file_name = "embedding_int_dim3.csv"
        httputils.copy_data(file_name)
        dbname = "default"
        tbname = "test_import_embedding_with_not_match_definition"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        for i in range(len(types)):
            self.dropTable(dbname,tbname)
            self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":types[i]
            })
            test_csv_dir = TEST_TMP_DIR + file_name
            self.importData(dbname,tbname,{
                "file_path":test_csv_dir,
                "file_type":"fevcs",
                "header":False,
                "delimiter":","
            },{
                "status_code":500,
                "error_code":3032,
            })
        return
    #PASS
    def test_import_embedding_with_dimension_unmatch(self):
        file_name = "embedding_int_dim3.csv"
        httputils.copy_data(file_name)
        dbname = "default"
        tbname = "test_import_embedding_with_dimension_unmatch"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        self.createTable(dbname,tbname,{
                "c1":{"type":"integer",},
                "c2":{"type":"vector","dimension":2,"element_type":"integer"}
        })
        test_csv_dir = TEST_TMP_DIR + file_name
        self.importData(dbname,tbname,{
                "file_path":test_csv_dir,
                "file_type":"fevcs",
                "header":False,
                "delimiter":","
            },{
                "status_code":500,
                "error_code":3032,
        })
        return
    #PASS
    def test_import_embedding_with_unmatched_elem_type(self):
        types =  [{"type":"vector","dimension":3,"element_type":"bool",
        },{"type":"vector","dimension":128,"element_type":"varchar",}]
        file_name = "embedding_int_dim3.csv"
        httputils.copy_data(file_name)
        dbname = "default"
        tbname = "test_import_embedding_with_unmatched_elem_type"

        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        for i in range(len(types)):
            self.dropTable(dbname,tbname)
            self.createTable(dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
                "c2":types[i]
            })
            test_csv_dir = TEST_TMP_DIR + file_name
            self.importData(dbname,tbname,{
                "file_path":test_csv_dir,
                "file_type":"csv",
                "header":False,
                "delimiter":","
            },{
                "status_code":500,
                "error_code":7012,
            })
        return
    #PASS
    def test_import_varchar_with_not_match_definition(self):
        file_name = "pysdk_test_varchar.csv"
        httputils.copy_data(file_name)
        dbname = "default"
        tbname = "test_import_varchar_with_not_match_definition"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)

        self.createTable(dbname,tbname,{
                "c1":{"type":"integer",},
                "c2":{"type":"varchar",}
        })

        test_csv_dir = TEST_TMP_DIR + file_name
        self.importData(dbname,tbname,{
                "file_path":test_csv_dir,"file_type":"csv","header":False,"delimiter":","
            })
        return
    #PASS
    def test_import_10000_columns(self):
        httputils.generate_big_int_csv(10000, "pysdk_test_big_int.csv")
        httputils.copy_data("pysdk_test_big_int.csv")
        dbname = "default"
        tbname = "test_import_10000_columns"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer"},
            "c2":{"type":"integer"}
        })
        test_csv_dir = TEST_TMP_DIR + "pysdk_test_big_int.csv"
        assert os.path.exists(test_csv_dir)
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        self.dropTable(dbname,tbname)
        return
    #PASS
    def test_table_with_not_matched_columns(self):
        httputils.copy_data("pysdk_test_commas.csv")
        dbname = "default"
        tbname = "test_table_with_not_matched_columns"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer"},
            "c2":{"type":"integer"},
            "c3":{"type":"integer"},
        })
        test_csv_dir = TEST_TMP_DIR + "pysdk_test_commas.csv"
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        },{
            "status_code":500,
            "error_code":3039,
        })
        return
    
    #PASS
    def test_import_with_different_size(self):
        
        dbname = "default"
        tbname = "test_import_with_different_size"
        filename = "pysdk_test_import_with_different_size.csv"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer"},
            "c2":{"type":"varchar"}
        })
        data_size = [1, 8191, 8192, 8193, 16*8192]
        for sz in data_size:
            httputils.generate_big_rows_csv(sz, filename)
            httputils.copy_data(filename)
            test_csv_dir = TEST_TMP_DIR + filename 
            self.importData(dbname,tbname,{
                "file_path":test_csv_dir,
                "file_type":"csv",
                "header":False,
                "delimiter":","
        })
        self.dropTable(dbname,tbname)
        return
    
    def test_import_exceeding_rows(self):
        httputils.generate_big_rows_csv(1024 * 8192, "pysdk_test_big_varchar_rows.csv")
        httputils.copy_data("pysdk_test_big_varchar_rows.csv")
        dbname = "default"
        tbname = "test_import_exceeding_rows"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer"},
            "c2":{"type":"varchar"}
        })
        test_csv_dir = TEST_TMP_DIR + "pysdk_test_big_varchar_rows.csv"
        res = self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        self.dropTable(dbname,tbname)
        return
    #PASS
    def test_import_exceeding_columns(self):
        httputils.generate_big_columns_csv(1024, "pysdk_test_big_columns.csv")
        httputils.copy_data("pysdk_test_big_columns.csv")
        dbname = "default"
        tbname = "test_import_exceeding_rows"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        columns = {}
        for i in range(1024):
            columns.update({"c"+str(i): {"type":"integer"}})
        self.createTable(dbname,tbname,columns)
        test_csv_dir = TEST_TMP_DIR + "pysdk_test_big_columns.csv"
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        return
