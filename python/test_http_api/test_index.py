import sys 
import pytest 
from httpapibase import HttpTest
from common.common_values import * 
import infinity.index as index
import pandas
import os 
import time 
import httputils
#PASS
class TestIndex(HttpTest):
    def test_create_index_IVFFlat(self):
        dbname = "default"
        tbname = "test_create_index_IVFFlat"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {
                "c1":{
                    "type":"vector",
                    "dimension":1024,
                    "element_type":"float",
                }
            }
        )

        self.createIdx(dbname,tbname,idxname,
        [
            "c1",
        ],
        {
            "type": "IVFFlat",
            "centroids_count": "128",
            "metric":"l2",
        })
        self.dropIdx(dbname,tbname,idxname)
        self.dropTable(dbname,tbname)
        return
    #PASS
    def test_create_index_HNSW(self):
        dbname = "default"
        tbname = "test_create_index_HMSW"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {
                "c1":{
                    "type":"vector",
                    "dimension":1024,
                    "element_type":"float",
                }
            }
        )
        self.createIdx(dbname,tbname,idxname,
        [
            "c1",
        ],
        {
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "l2"
        })
        self.dropIdx(dbname,tbname,idxname)
        self.dropTable(dbname,tbname)
        return
    #PASS
    def test_create_index_fulltext(self):
        dbname = "default"
        tbname = "test_create_index_fulltext"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {
                "doctitle":{
                    "type":"varchar",
                },
                "docdate":{
                    "type":"varchar",
                },
                "body":{
                    "type":"varchar",
                }
            }
        )
        self.createIdx(dbname,tbname,idxname,
        [
            "body",
        ],
        {
            "type": "FULLTEXT",
        })

        self.dropIdx(dbname,tbname,idxname)
        self.dropTable(dbname,tbname)
        return
    
    #PASS
    def test_drop_non_existent_index(self):
        dbname = "default"
        tbname = "test_drop_non_existent_index"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {
                "doctitle":{
                    "type":"varchar",
                }
            }
        )
        self.dropIdx(dbname,tbname,idxname,{
            "status_code": 500,
            "error_code":3023,
        })
        return
    #PASS
    def test_create_created_index(self):
        dbname = "default"
        tbname = "test_create_index_fulltext"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {"doctitle":{"type":"varchar",},"docdate":{"type":"varchar",},"body":{"type":"varchar",}}
        )
        self.createIdx(dbname,tbname,idxname,["body",],{"type": "FULLTEXT",})
        self.createIdx(dbname,tbname,idxname,["body",],{"type": "FULLTEXT",})
        self.dropIdx(dbname,tbname,idxname)
        self.dropTable(dbname,tbname)
        return
    
    # create / drop index with invalid options
    @pytest.mark.parametrize("column_name",
                             [(1, False), (2.2, False), ((1, 2), False), ([1, 2, 3], False), ("c1", True)])
    @pytest.mark.parametrize("index_type", [
        (1, False),
        (2.2, False),
        ([1, 2], False),
        ("$#%dfva", False),
        ((1, 2), False),
        ({"1": 2}, False),
        (index.IndexType.Hnsw, False),
        (index.IndexType.IVFFlat, True)
    ])
    @pytest.mark.parametrize("params", [
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False), ((1, 2), False), ({"1": 2}, False)
    ])
    def test_create_drop_vector_index_invalid_options(self,column_name,index_type,params):
        dbname = "default"
        tbname = "test_create_drop_vector_index_invalid_options"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {
                "type":"vector",
                "dimension":3, 
                "element_type":"float", 
            }
        )
        if not column_name[1] or not index_type[1] or not params[1]:
                self.createIdx(
                    dbname,
                    tbname,
                    idxname, 
                    [str(column_name[0])],
                    {
                        "type":str(index_type[0]),
                        "centroids_count":128,
                        "metric":"l2",
                    },
                    {
                        "status_code":500,
                        "error_code":3022,
                    }
                )
        else:
            self.createIdx(
                    dbname,
                    tbname,
                    idxname, 
                    [str(column_name[0])],
                    {
                        "type":str(index_type[0]),
                        "centroids_count":128,
                        "metric":"l2",
                    },
                )
        return
    

    @pytest.mark.parametrize("column_name", [
        (1, False),
        (2.2, False),
        ([1, 2], False),
        ("$#%dfva", False),
        ((1, 2), False),
        ({"1": 2}, False),
        ("c1", True)])
    @pytest.mark.parametrize("index_type", ["FullText"])
    @pytest.mark.parametrize("params", [
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False),
        ((1, 2), False), ({"1": 2}, False), ([], True)
    ])
    @pytest.mark.parametrize("types", ["int", "int8", "int16", "int32", "int64", "integer",
                                       "float", "float32", "double", "float64",
                                       "varchar", "bool", "vector, 3, float"])
    def test_create_drop_different_fulltext_index_invalid_options(self,column_name, index_type,
                                                                  params, types):
        dbname = "default"
        tbname = "test_create_drop_different_fulltext_index_invalid_options"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {
                "type":types
            }
        )
        if types != "varchar" or not column_name[1] or not index_type[1] or not params[1]:
                   self.createIdx(dbname,tbname,idxname, 
                    [str(column_name[0])],
                    {
                        "type":str(index_type[0]),
                    },
                    {
                        "status_code":500,
                        "error_code":3022,
                    }
                )
        else:
            self.createIdx(dbname,tbname,idxname, 
                    [str(column_name[0])],
                    {
                        "type":str(index_type[0]),
                    },
            )

        self.drop_table(dbname,tbname)
        return
    #PASS
    def test_create_index_on_dropped_table(self):
        dbname = "default"
        tbname = "test_create_index_on_dropped_table"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,tbname,{
                "c1":{
                     "type":"vector",
                     "dimension":3,
                     "element_type":"float",
                }
               
            }
        )
        self.dropTable(dbname,tbname)
        self.createIdx(dbname,tbname,idxname,
        ["c1",],{
            "type": "IVFFlat",
            "centroids_count": 128,
            "metric":"l2",
        },{
            "status_code":500,
            "error_code":3022,
        })
        return
    #PASS
    def test_create_index_show_index(self):
        dbname = "default"
        tbname = "test_create_index_show_index"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,tbname,{
                "c1":{"type":"vector","dimension":3,"element_type":"float",}}
        )
        self.createIdx(dbname,tbname,idxname,
        ["c1",],{"type": "IVFFlat","centroids_count": 128,"metric":"l2",
        })
        self.showIdx(dbname,tbname,idxname,expect={
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "index_name": idxname,
            "index_type": "IVFFlat",
            "other_parameters": "metric = l2, centroids_count = 128",
        })
        self.dropIdx(dbname,tbname,idxname)
        return
    def test_drop_index_show_index(self):
        dbname = "default"
        tbname = "test_drop_index_show_index"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"vector","dimension":3,"element_type":"float",}}
        )
        self.createIdx(dbname,tbname,idxname,
            ["c1",],{"type": "IVFFlat","centroids_count": 128,"metric":"l2",
        })
        self.showIdx(dbname,tbname,idxname,expect={
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "index_name": idxname,
            "index_type": "IVFFlat",
            "other_parameters": "metric = l2, centroids_count = 128",
        })
        tlist = self.listIdx(dbname,tbname,expect={
            "error_code":0
        })
        assert len(tlist) == 0

        self.dropIdx(dbname,tbname,idxname)
        self.dropIdx(dbname,tbname,idxname,{
            "status_code":500,
            "error_code":3023,
        })
        return
    # create index on different type of column and show index
    @pytest.mark.parametrize("types", {"type":"vector","dimension":3,"element_type":"float",})
    @pytest.mark.parametrize("index_type", [
        ("Hnsw", False, "ERROR:3061*"),
        ("IVFFlat", True),
        ("FullText", False, "ERROR:3009*")
    ])
    def test_create_index_on_different_type_of_column(self, types, index_type):
        dbname = "default"
        tbname = "test_create_index_on_different_type_of_column"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":types}
        )
        if not index_type[1]:
            self.createIdx(dbname,tbname,idxname, 
                    ["c1"],
                    {
                        "type":str(index_type[0]),
                        "centroids_count": "128",
                        "metric": "l2",
                    },
                    {
                        "status_code":500,
                        "error_code":3022,
                    }
            )
        else:
            self.createIdx(dbname,tbname,idxname, 
                    ["c1"],
                    {
                        "type":str(index_type[0]),
                        "centroids_count": "128",
                        "metric": "l2",
                    },
            )
            return 
        return
    @pytest.mark.parametrize("index_type", [
        "IVFFlat"
    ])
    def test_insert_data_create_index(self,index_type):
        dbname = "default"
        tbname = "test_insert_data_create_index"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"vector","dimension":1024,"element_type":"float",
            }}
        )
        values = [{"c1": [1.1 for _ in range(1024)]}]
        self.insert(dbname,tbname,values)
        self.createIdx(
            dbname,tbname,idxname,
            {"c1",},{"type":index_type,"centroids_count": "128","metric": "l2",}
        )
        return 
   
    def test_import_data_create_index(self):
        dbname = "default"
        tbname = "test_import_data_create_index"
        idxname = "my_index"
        index_type = ["IVFFlat","FullText"]
        for t in index_type:
            self.dropTable(dbname,tbname)
            self.createTable(dbname,tbname,{
                "c1":{"type":"integer"},
                "c2":{"type":"vector","dimension":3,"element_type":"float",}
            })
            httputils.copy_data("embedding_int_dim3.csv")
            test_csv_dir = TEST_TMP_DIR + "embedding_int_dim3.csv"
            assert os.path.exists(test_csv_dir)

            self.importData(dbname,tbname,{
                "file_path":test_csv_dir,
                "file_type":"csv",
                "header":False,
                "delimiter":","
            })
            if t == "IVFFlat":
                self.createIdx(dbname,tbname,idxname,["c2"],{
                    "type":"IVFFlat",
                    "centroids_count": "128",
                    "metric": "l2"  
                })
                continue 
        return
    
    #ERROR: IVFFlat realtime index is not supported yet
    def test_create_vector_index_import_data(self):
        dbname = "default"
        tbname = "test_create_vector_index_import_data"
        idxname = "my_index"
      
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{"type":"integer"},
            "c2":{"type":"vector","dimension":4,"element_type":"float",}
        })
        httputils.copy_data("embedding_float_dim4.csv")
        test_csv_dir = TEST_TMP_DIR + "embedding_float_dim4.csv"
        assert os.path.exists(test_csv_dir)
        #ERROR
        self.createIdx(dbname,tbname,idxname,["c2"],{
            "type":"IVFFlat",
            "centroids_count": "128",
            "metric": "l2"  
        })
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })    
        return
    
    #ERROR: IVFFlat realtime index is not supported yet
    def test_create_index_import_data(self):
        dbname = "default"
        tbname = "test_create_index_import_data"
        idxname = "my_index"
        index_type = ["IVFFlat","FullText"]
        for t in index_type:
            self.dropTable(dbname,tbname)
            self.createTable(dbname,tbname,{
                "c1":{"type":"integer"},
                "c2":{"type":"vector","dimension":3,"element_type":"float",}
            })
            httputils.copy_data("embedding_int_dim3.csv")
            test_csv_dir = TEST_TMP_DIR + "embedding_int_dim3.csv"
            assert os.path.exists(test_csv_dir)
            #ERROR
            if t == "IVFFlat":
                self.createIdx(dbname,tbname,idxname,["c2"],{
                    "type":"IVFFlat",
                    "centroids_count": "128",
                    "metric": "l2"  
                })
            else:
                # self.createIdx(dbname,tbname,idxname,["c2"],{
                #     "type":"FullText",
                # })
                continue 
            self.importData(dbname,tbname,{
                "file_path":test_csv_dir,
                "file_type":"csv",
                "header":False,
                "delimiter":","
            })    
        return
    
    #ERROR: IVFFlat realtime index is not supported yet
    def test_insert_data_fulltext_index_search(self):
        httputils.copy_data("enwiki_99.csv")
        dbname = "default"
        tbname = "test_insert_data_fulltext_index_search"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "doctitle": {"type":"varchar"},
            "docdate": {"type":"varchar"}, 
            "body": {"type":"varchar"},
        })
        test_csv_dir = TEST_TMP_DIR +  "enwiki_99.csv"
        print("test_csv_dir: "+ test_csv_dir )
        assert os.path.exists(test_csv_dir)

        self.createIdx(dbname,tbname,idxname,["body"],{
            "type":"FULLTEXT",
        })

        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })

        return 
    def test_fulltext_match_with_invalid_analyzer(self):
        dbname = "default"
        tbname = "test_fulltext_match_with_invalid_analyzer"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "doctitle": {"type":"varchar"},
            "docdate": {"type":"varchar"}, 
            "body": {"type":"varchar"},
        })
        httputils.copy_data("enwiki_9.csv")
        test_csv_dir = TEST_TMP_DIR +  "enwiki_9.csv"
        print("test_csv_dir: "+ test_csv_dir )
        assert os.path.exists(test_csv_dir)

        self.importData(dbname,tbname,{
                "file_path":test_csv_dir,
                "file_type":"csv",
                "header":False,
                "delimiter":"\t"
        }) 

        self.createIdx(dbname,tbname,idxname,["body"],{
            "type":"FULLTEXT",
            "ANALYZER":"segmentation"
        },{
            "status_code":500,
        })

        self.select(dbname,tbname,["*"],"",{
        },{},{
            "error_code":0,
        })

        return 
    #ERROR
    def test_create_index_on_deleted_table(self):
        dbname = "default"
        tbname = "test_create_index_on_deleted_table"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"vector",
                "dimension":128,
                "element_type":"float"
            }
        })
        embedding_data = [float(i) for i in range(128)]
        value = [{"c1": embedding_data} for _ in range(1024)]
        print("value: ",value)
        self.insert(dbname,tbname,value)

        self.dropTable(dbname,tbname)
        self.select(dbname,tbname,["*"],"",{
        },{},{
            "error_code":0,
        })
        self.createIdx(dbname,tbname,idxname,["c1"],{
            "type":"IVFFlat",
            "centroids_count":"128",
            "metric": "l2"
        },{
            "status_code":500,
        })
        return
    
    #ERROR
    @pytest.mark.skip
    @pytest.mark.xfail(reason="Not support to convert Embedding to Embedding")
    def test_create_index_on_update_table(self):
        dbname = "default"
        tbname = " test_create_index_on_update_table"
        idxname = "my_index"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"vector",
                "dimension":128,
                "element_type":"float"
            },
            "c2":{
                "type":"integer",
            }
        })
        embedding_data = [i for i in range(128)]
        value = [{"c1": embedding_data, "c2": i} for i in range(10)]
        self.insert(dbname,tbname,value)

        embedding_data = [i + 0.1 * i for i in range(128)]
        value = [{"c1": embedding_data} for _ in range(10)]
        for i in range(10):
             self.update(dbname,tbname,{"c1":embedding_data},"c2 = "+str(i))
        self.dropTable(dbname,tbname)

        self.select(dbname,tbname,["c1","c2"])
        return
    
    def test_create_index_with_various_options(self):
        dbname = "default"
        tbname = " test_create_index_with_various_options"
        idxname = "my_index"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "c1":{
                "type":"vector",
                "dimension":4,
                "element_type":"float"
            },
            "c2":{
                "type":"integer",
            }
        })
        idx_option = ["kError","kIgnore","kReplace"]
        for opt in idx_option:
            self.createIdx(dbname,tbname,idxname,["c1"],{
                "type": "HNSW",
                "M": "16",
                "ef_construction": "50",
                "ef": "50",
                "metric": "l2"
            },{"error_code":0},opt)
        self.dropIdx(dbname,tbname,idxname)
        self.dropTable(dbname,tbname)
        return
    def test_create_duplicated_index_with_various_options(self, get_infinity_db, conflict_type):
        return
    def test_show_index(self, get_infinity_db):
        return
    def test_show_various_name_index(self, get_infinity_db, index_name):
        return
    def test_list_index(self, get_infinity_db):
        return
    def test_drop_index_with_various_options(self, get_infinity_db, conflict_type):
        return
    def test_same_column_with_different_parameters(self, get_infinity_db, index_distance_type):
        return