import sys 
import pytest 
import concurrent.futures
from httpapibase import HttpTest
from common.common_values import * 

class TestIndex(HttpTest):
    def test_create_index_IVFFlat(self, get_infinity_db):
        return
    def test_create_index_HNSW(self, get_infinity_db):
        return
    def test_create_index_fulltext(self, get_infinity_db):
        return
    def test_drop_non_existent_index(self, get_infinity_db):
        return
    def test_create_created_index(self, get_infinity_db):
        return
    def test_create_drop_vector_index_invalid_options(self, get_infinity_db, column_name, index_type, params, types):
        return
    def test_create_drop_different_fulltext_index_invalid_options(self, get_infinity_db, column_name, index_type,
                                                                  params, types):
        return
    def test_create_index_on_dropped_table(self, get_infinity_db):
        return
    def test_create_index_show_index(self, get_infinity_db):
        return
    def test_drop_index_show_index(self, get_infinity_db):
        return
    def test_create_index_on_different_type_of_column(self, get_infinity_db, types, index_type):
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
    
    def createIdx(self,dbname,tbname,idxname,fields,index,expect):
        url = f"databases/{dbname}/tables/{tbname}/indexes/{idxname}"
        create_option = {
            "ignore_if_exists":True,
        }
        h = super().SetUpHeader(['accept'],)
        d = super().SetUpData([],{fields,index,create_option})
        r = super().Request(url,"post",h)
        super().TearDown(r,expect)
        return 
    
    def dropIdx(self,dbname,tbname,idxname,expect={
        "error_code":0,
    }):
        url = f"databases/{dbname}/tables/{tbname}/indexes/{idxname}"
        h = super().SetUpHeader(['accept'])
        r = super().Request(url,"delete",h)
        super().TearDown(r,expect)
        return 
    def showIdx(self,dbname,tbname,idxname,expect):
        url = f"databases/{dbname}/tables/{tbname}/indexes/{idxname}"
        h = super().SetUpHeader(['accept'])
        r = super().Request(url,"get",h)
        super().TearDown(r,expect)
        return 
    def listIdx(self,dbname,tbname,idxname,expect):
        url = f"databases/{dbname}/tables/{tbname}/indexes"
        h = super().SetUpHeader(['accept'])
        r = super().Request(url,"get",h)
        super().TearDown(r,expect)
        return 

        