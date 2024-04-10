import sys 
import pytest 
import concurrent.futures
from httpapibase import HttpTest
from common.common_values import * 

class TestIndex(HttpTest):
    def test_create_index_IVFFlat(self):
        dbname = "default"
        tbname = "test_create_index_HMSW"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,
            tbname,
            {
                "c1":{
                    "type":["vector","1024","float"],
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
                    "type":["vector,1024,integer"]
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
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False), ((
                                                                                1, 2), False), ({"1": 2}, False),
        ([index.InitParameter("centroids_count", "128"),
          index.InitParameter("metric", "l2")], True)
    ])
    @pytest.mark.parametrize("types", ["vector, 3, float"])
    def test_create_drop_vector_index_invalid_options(self):
        
        return
    def test_create_drop_different_fulltext_index_invalid_options(self):
        return
    def test_create_index_on_dropped_table(self):
        return
    def test_create_index_show_index(self):
        return
    def test_drop_index_show_index(self):
        return
    def test_create_index_on_different_type_of_column(self):
        return
    def test_insert_data_create_index(self, get_infinity_db, index_type):
        return 
    def test_import_data_create_index(self, get_infinity_db, index_type, file_format):
        return
    def test_create_index_import_data(self, get_infinity_db, index_type, file_format):
        return
    def test_insert_data_fulltext_index_search(self, get_infinity_db, file_format):
        return 
    def test_fulltext_match_with_invalid_analyzer(self, get_infinity_db, check_data):
        return 
    def test_create_index_on_deleted_table(self, get_infinity_db):
        return
    def test_create_index_on_update_table(self, get_infinity_db):
        return
    def test_create_index_with_various_options(self, get_infinity_db, conflict_type):
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