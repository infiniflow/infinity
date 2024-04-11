import pytest
import sys 
import pytest 

sys.path.append("/home/yyk/work/infinity/python/test_http_api/common")
sys.path.append("/home/yyk/work/infinity/python/test")
sys.path.append("/home/yyk/work/infinity/python/test/common")
sys.path.append("/home/yyk/work/infinity/python/test/data_type")
sys.path.append("/home/yyk/work/infinity/python")
sys.path.append("/home/yyk/work/infinity/python/test_http_api")


from test_http_api.httpapibase import HttpTest
from test_http_api.test_database import TestDataBase
from test_http_api.test_table import TestTable
from test_http_api.test_index import TestIndex
from test_http_api.test_insert import TestInsert
#from test.localapibase import SdkTest

class SdkTest:
    pass 

@pytest.fixture(params=[SdkTest(), HttpTest()])
def test_object(request):
    return request.param


def test_database(test_object):
    t: test_object = TestDataBase()
    methods = dir(t)
    test_methods = [method for method in methods if method.startswith('test_')]
    for method in test_methods:
         getattr(t, method)()

def test_table(test_object):
    t: test_object = TestTable()
    methods = dir(t)
    test_methods = [method for method in methods if method.startswith('test_')]
    for method in test_methods:
        getattr(t, method)()
    return

def test_index(test_object):
    t: test_object = TestIndex()
    methods = dir(t)
    test_methods = [method for method in methods if method.startswith('test_')]
    for method in test_methods:
        getattr(t, method)()
    return

def test_insert(test_object):
    return 

def SingleTest(test_object):
    t: test_object = TestIndex()
    t.test_create_index_IVFFlat()

#test_table(HttpTest())
#test_database(HttpTest())
SingleTest(HttpTest())

