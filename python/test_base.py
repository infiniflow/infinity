import pytest
import sys 
sys.path.append("/home/yyk/work/infinity/python/test")
sys.path.append("/home/yyk/work/infinity/python/test_http_api/common")
sys.path.append("/home/yyk/work/infinity/python/test/common")
sys.path.append("/home/yyk/work/infinity/python/test/data_type")
sys.path.append("/home/yyk/work/infinity/python")
sys.path.append("/home/yyk/work/infinity/python/test_http_api")

from test_http_api.httpapibase import HttpTest
#from test.sdktestbase import SdkTest 
from test_http_api.test_database import TestDataBase
from test_http_api.test_table import TestTable
from test_http_api.test_index import TestIndex
from test_http_api.test_insert import TestInsert
from test_http_api.test_query import TestQuery
from test_http_api.test_select import TestSelect
from test_http_api.test_delete import TestDelete
from test_http_api.test_show import TestShow
from test_http_api.test_update import TestUpdate
from test_http_api.test_import import TestImport
from test_http_api.test_knn import TestKnn

class SdkTest():
    pass 


@pytest.fixture(params=[SdkTest(), HttpTest()])
def testobject(request):
    return request.param

def SingleTest(testobject):
    t: testobject =  TestIndex()
    t.test_http_drop_index_with_invalid_options()
()

SingleTest(HttpTest())