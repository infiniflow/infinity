import sys 
import pytest 
import time 
from httpapibase import HttpTest
from common.common_values import *
import httputils

class TestKnn(HttpTest):
    #PASS
    @pytest.mark.skip(reason="can not import tmp_20240116.csv")
    def test_knn(self):
        httputils.check_data(TEST_TMP_DIR)
        dbname = "default"
        tbname = "test_knn"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"varchar"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"varchar"},
            "query_gender":{"type":"varchar"},
            "query_color":{"type":"varchar"},
            "query_price":{"type":"float"}
        })

        httputils.copy_data("tmp_20240116.csv")
        test_csv_dir = TEST_TMP_DIR + "tmp_20240116.csv"
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        self.dropTable(dbname,tbname)
        return 
    #PASS
    def test_insert_multi_column(self):
        dbname = "default"
        tbname = "test_knn"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"varchar"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"varchar"},
            "query_gender":{"type":"varchar"},
            "query_color":{"type":"varchar"},
            "query_price":{"type":"float"}
        })
        self.insert(dbname,tbname,[{"variant_id": "123",
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": "ok",
                           "query_gender": "varchar",
                           # "query_color": "red",
                           "query_price": 1.0
                           }])
        return 
    #PASS
    def test_knn_on_vector_column(self):
        dbname = "default"
        tbname = "test_knn_on_vector_column"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })
        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])
        self.showTable(dbname,tbname,{
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : 10,
            "segment_count" : 1,
            "row_count" : 1
        })
        
        self.select(dbname,tbname,["variant_id"],"",{
            "knn":{
                    "fields": ["gender_vector"],
                    "query_vector": [1.0]*4,
                    "element_type" : "float",
                    "top_k": 2,
                    "metric_type": "l2"
                }
        },{},{
            "error_code":0,
            "output": [{'variant_id': '123'}]
        })
        return
    #PASS
    def test_knn_on_non_vector_column(self):
        dbname = "default"
        tbname = "test_knn_on_vector_column"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })
        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])
        self.showTable(dbname,tbname,{
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : 10,
            "segment_count" : 1,
            "row_count" : 1
        })
        
        self.select(dbname,tbname,["variant_id"],"",{
            "knn":{
                    "fields": ["variant_id"],
                    "query_vector": [1.0]*4,
                    "element_type" : "float",
                    "top_k": 2,
                    "metric_type": "l2"
                }
        },{},{
            "status_code":500,
            "error_code":3013,
            #"output": [{'variant_id': '123'}]
        })
        return
    #PASS
    def test_valid_embedding_data(self):
        dbname = "default"
        tbname = "test_valid_embedding_data"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })
        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])
        self.showTable(dbname,tbname,{
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : 10,
            "segment_count" : 1,
            "row_count" : 1
        })
        
        self.select(dbname,tbname,["variant_id"],"",{
            "knn":{
                    "fields": ["gender_vector"],
                    "query_vector": [1.0,2.0,3.0,4.0],
                    "element_type" : "float",
                    "top_k": 2,
                    "metric_type": "l2"
                }
        },{},{
            "error_code":0,
            "output": [{'variant_id': '123'}]
        })
        return
    #PASS
    def test_invalid_embedding_data(self):
        embedding_data = [
            pytest.param("variant_id"),
            pytest.param("gender_vector"),
            pytest.param(1),
            pytest.param(2.4),
            pytest.param([1] * 3),
            pytest.param((1, 2, 3)),
            pytest.param({"c": "12"}),
        ]
        dbname = "default"
        tbname = "test_invalid_embedding_data"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })
        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])
        self.showTable(dbname,tbname,{
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : 10,
            "segment_count" : 1,
            "row_count" : 1
        })
        for d in embedding_data:
            self.select(dbname,tbname,["variant_id"],"",{
            "knn":{
                    "fields": ["gender_vector"],
                    "query_vector": d,
                    "element_type" : "float",
                    "top_k": 2,
                    "metric_type": "l2"
                }
            },{},{
                "status_code":500,
                "error_code":3013,
            })

        return 
    #PASS
    def test_valid_embedding_data_type(self):
        dbname = "default"
        tbname = "test_valid_embedding_data_type"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })
        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])
        self.showTable(dbname,tbname,{
            "error_code":0,
            "database_name": dbname,
            "table_name": tbname,
            "column_count" : 10,
            "segment_count" : 1,
            "row_count" : 1
        })
        
        self.select(dbname,tbname,["variant_id"],"",{
            "knn":{
                    "fields": ["gender_vector"],
                    "query_vector": [1.0]*4,
                    "element_type" : "float",
                    "top_k": 2,
                    "metric_type": "l2"
                }
        },{},{
           
            "error_code":0,
            "output": [{'variant_id': '123'}]
        })

        self.select(dbname,tbname,["variant_id"],"",{
            "knn":{
                    "fields": ["gender_vector"],
                    "query_vector": [1]*4,
                    "element_type" : "float",
                    "top_k": 2,
                    "metric_type": "l2"
                }
        },{},{
            "status_code":500,
            "error_code":3013,
        })
        return
    
    #ERROR
    @pytest.mark.skip(reason="knn use element_type:integer leads to segment fault")
    def test_invalid_embedding_data_type(self):
        dbname = "default"
        tbname = "test_invalid_embedding_data_type"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })
        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])
        self.select(dbname,tbname,["variant_id"],"",{
            "knn":{
                    "fields": ["gender_vector"],
                    "query_vector": [1.0]*4,
                    "element_type" : "integer",
                    "top_k": 2,
                    "metric_type": "l2"
                }
        },{},{
           
            "status_code":500,
            "error_code":3013,
        })

        return
    #PASS
    def test_various_distance_type(self):
        distance_type = [
            "l2","cosine","hamming"
        ]
        dbname = "default"
        tbname = "test_various_distance_type"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })
        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])
        for t in distance_type:
            if t == 'l2':
                self.select(dbname,tbname,["variant_id"],"",{
                "knn":{
                        "fields": ["gender_vector"],
                        "query_vector": [1.0]*4,
                        "element_type" : "float",
                        "top_k": 2,
                        "metric_type": t
                    }
                },{},{
                    "error_code":0,
                })
            else:
                self.select(dbname,tbname,["variant_id"],"",{
                "knn":{
                        "fields": ["gender_vector"],
                        "query_vector": [1.0]*4,
                        "element_type" : "float",
                        "top_k": 2,
                        "metric_type": t
                    }
                },{},{
                    "status_code":500,
                    "error_code":3032,
                })
        return 
    #PASS
    def test_various_topn(self):
        topn = [
            0,-1,"test",{},(),[1] * 4
        ]
        dbname = "default"
        tbname = "test_various_topn"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })
        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])
        for t in topn:
            self.select(dbname,tbname,["variant_id"],"",{
                "knn":{
                        "fields": ["gender_vector"],
                        "query_vector": [1.0]*4,
                        "element_type" : "float",
                        "top_k": t,
                        "metric_type": "l2"
                    }
            },{},{
                "status_code":500,
            })
        return
    
    def test_with_index_before(self):
        dbname = "default"
        tbname = "test_with_index_before"
        idxname = "my_index"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })

        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])

        index_column_name = ["gender_vector",
                            "color_vector",
                            "category_vector",
                            "tag_vector",
                            "other_vector"]
        knn_column_name = ["gender_vector","color_vector",
                            "category_vector","tag_vector","other_vector"]
        for name in index_column_name:
            self.createIdx(dbname,tbname,idxname,[name,],{
                 "type": "HNSW",
                 "M": "16",
                 "ef_construction": "50",
                 "ef": "50",
                 "metric": "l2"
            })
            for k in knn_column_name:
                self.select(dbname,tbname,["variant_id"],"",{
                "knn":{
                        "fields": [k],
                        "query_vector": [1.0]*4,
                        "element_type" : "float",
                        "top_k": 2,
                        "metric_type": "l2"
                    }
                },{},{
                    "error_code":0,
                    "output":[{'variant_id': '123'}]
                })

        return 
    
    def test_with_index_after(self):
        dbname = "default"
        tbname = "test_with_index_after"
        idxname = "my_index"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
            "variant_id":{"type":"integer"},
            "gender_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "color_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "category_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "tag_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "other_vector":{"type":"vector","dimension":4,"element_type":"float"},
            "query_is_recommend":{"type":"integer"},
            "query_gender":{"type":"integer"},
            "query_color":{"type":"integer"},
            "query_price":{"type":"float"}
        })

        self.insert(dbname,tbname,[{"variant_id": 123,
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": 1,
                           "query_gender": 1,
                           "query_color": 2,
                           "query_price": 1.0
                           }])

        index_column_name = ["gender_vector",
                            "color_vector",
                            "category_vector",
                            "tag_vector",
                            "other_vector"]
        knn_column_name = ["gender_vector","color_vector",
                            "category_vector","tag_vector","other_vector"]
        for name in index_column_name:
            for k in knn_column_name:
                self.select(dbname,tbname,["variant_id"],"",{
                "knn":{
                        "fields": [k],
                        "query_vector": [1.0]*4,
                        "element_type" : "float",
                        "top_k": 2,
                        "metric_type": "l2"
                    }
                },{},{
                    "error_code":0,
                    "output":[{'variant_id': '123'}]
                })
            self.createIdx(dbname,tbname,idxname,[name,],{
                 "type": "HNSW",
                 "M": "16",
                 "ef_construction": "50",
                 "ef": "50",
                 "metric": "l2"
            })
        return 
    def test_with_fulltext_match_with_valid_columns(self):
        httputils.check_data(TEST_TMP_DIR)
        match_param_1 = ["doctitle","num","body"]
        dbname = "default"
        tbname = "test_with_fulltext_match_with_valid_columns"
        idxname = "my_index"
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
           "doctitle":{"type":"varchar"},
           "docdate":{"type":"varchar"},
           "body":{"type":"varchar"},
           "num":{"type":"integer"},
           "vec":{
               "type":"vector","dimension":4,"element_type":"integer"
           }
        })
        httputils.generate_commas_enwiki("enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
        httputils.copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        self.select(dbname,tbname,["num"],"",{
                    #"method": "rrf",
                    # "knn":{
                    #         "fields": "vec",
                    #         "query_vector": [3.0, 2.8, 2.7, 3.1],
                    #         "element_type" : "float",
                    #         "top_k": 2,
                    #         "metric_type": "l2"
                    # },
                    "match":{
                        "fields": "body^5", 
                        "query": "black",
                        "operator": "topn=1"
                    }
                },{},{
                    "error_code":0,
                    "output":[{'variant_id': '123'}]
                })
        return
    def test_with_fulltext_match_with_invalid_columns(self):
        httputils.check_data(TEST_TMP_DIR)
        dbname = "default"
        tbname = "test_with_fulltext_match_with_invalid_columns"
        
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
           "doctitle":{"type":"varchar"},
           "docdate":{"type":"varchar"},
           "body":{"type":"varchar"},
           "num":{"type":"integer"},
           "vec":{
               "type":"vector","dimension":4,"element_type":"integer"
           }
        })
        httputils.generate_commas_enwiki("enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
        httputils.copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        self.select(dbname,tbname,["num"],"",{
                    #"method": "rrf",
                    # "knn":{
                    #         "fields": "vec",
                    #         "query_vector": [3.0, 2.8, 2.7, 3.1],
                    #         "element_type" : "float",
                    #         "top_k": 2,
                    #         "metric_type": "l2"
                    # },
                    "match":{
                        "fields": [], 
                        "query": "black",
                        "operator": "topn=1"
                    }
                },{},{
                    "status_code":500,
                    "error_code":3067,
                    "output":[{'variant_id': '123'}]
                })
        return
    def test_with_fulltext_match_with_valid_words(self):
        httputils.check_data(TEST_TMP_DIR)
        match_param_2 = ["a word a segment","body=Greek"]
        dbname = "default"
        tbname = "test_with_fulltext_match_with_valid_words"
        
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
           "doctitle":{"type":"varchar"},
           "docdate":{"type":"varchar"},
           "body":{"type":"varchar"},
           "num":{"type":"integer"},
           "vec":{
               "type":"vector","dimension":4,"element_type":"integer"
           }
        })
        httputils.generate_commas_enwiki("enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
        httputils.copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        for m in match_param_2:
            self.select(dbname,tbname,["num"],"",{
                    # "method": "rrf",
                    # "knn":{
                    #         "fields": "vec",
                    #         "query_vector": [3.0, 2.8, 2.7, 3.1],
                    #         "element_type" : "float",
                    #         "top_k": 2,
                    #         "metric_type": "l2"
                    # },
                    "match":{
                        "fields": "body", 
                        "query": m,
                        "operator": "topn=1"
                    }
                },{},{
                   "error_code":0,
                   "output":[]
                })
        return 
    def test_with_fulltext_match_with_invalid_words(self):
        httputils.check_data(TEST_TMP_DIR)
        match_param_2 = [1,1.1,[],{},()]
        dbname = "default"
        tbname = "test_with_fulltext_match_with_valid_words"
        
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
           "doctitle":{"type":"varchar"},
           "docdate":{"type":"varchar"},
           "body":{"type":"varchar"},
           "num":{"type":"integer"},
           "vec":{
               "type":"vector","dimension":4,"element_type":"integer"
           }
        })
        httputils.generate_commas_enwiki("enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
        httputils.copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        for m in match_param_2:
            self.select(dbname,tbname,["num"],"",{
                    # "method": "rrf",
                    # "knn":{
                    #         "fields": "vec",
                    #         "query_vector": [3.0, 2.8, 2.7, 3.1],
                    #         "element_type" : "float",
                    #         "top_k": 2,
                    #         "metric_type": "l2"
                    # },
                    "match":{
                        "fields": "body", 
                        "query": m,
                        "operator": "topn=1"
                    }
                },{},{
                   "status_code":500,
                   "error_code":3067, 
                })
        return 
    def test_with_fulltext_match_with_options(self):
        httputils.check_data(TEST_TMP_DIR)
        match_param_3 = ["@#$!#@$SDa^sdf3!@#$","topn=1","1"]
        dbname = "default"
        tbname = "test_with_fulltext_match_with_options"
        
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
           "doctitle":{"type":"varchar"},
           "docdate":{"type":"varchar"},
           "body":{"type":"varchar"},
           "num":{"type":"integer"},
           "vec":{
               "type":"vector","dimension":4,"element_type":"integer"
           }
        })
        httputils.generate_commas_enwiki("enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
        httputils.copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        for m in match_param_3:
            self.select(dbname,tbname,["num"],"",{
                    # "method": "rrf",
                    # "knn":{
                    #         "fields": "vec",
                    #         "query_vector": [3.0, 2.8, 2.7, 3.1],
                    #         "element_type" : "float",
                    #         "top_k": 2,
                    #         "metric_type": "l2"
                    # },
                    "match":{
                        "fields": "doctitle,num,body^5", 
                        "query": "word",
                        "operator": m
                    }
                },{},{
                   "error_code":0,
                   "output":[]
                })
        return
    def test_with_fulltext_match_with_invalid_options(self):
        httputils.check_data(TEST_TMP_DIR)
        match_param_3 = [1,1.1,[],{},()]
        dbname = "default"
        tbname = "test_with_fulltext_match_with_invalid_words"
        
        self.showdb(dbname)
        self.dropTable(dbname,tbname)
        self.createTable(dbname,tbname,{
           "doctitle":{"type":"varchar"},
           "docdate":{"type":"varchar"},
           "body":{"type":"varchar"},
           "num":{"type":"integer"},
           "vec":{
               "type":"vector","dimension":4,"element_type":"integer"
           }
        })
        httputils.generate_commas_enwiki("enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
        httputils.copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        self.importData(dbname,tbname,{
            "file_path":test_csv_dir,
            "file_type":"csv",
            "header":False,
            "delimiter":","
        })
        for m in match_param_3:
            self.select(dbname,tbname,["num"],"",{
                    # "method": "rrf",
                    # "knn":{
                    #         "fields": "vec",
                    #         "query_vector": [3.0, 2.8, 2.7, 3.1],
                    #         "element_type" : "float",
                    #         "top_k": 2,
                    #         "metric_type": "l2"
                    # },
                    "match":{
                        "fields": "doctitle,num,body^5", 
                        "query": "word",
                        "operator": m
                    }
                },{},{
                   "status_code":500,
                   "error_code":3067,
                })
        return