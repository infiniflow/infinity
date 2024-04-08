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

# def test_table(test_object):
#     t: test_object = TestTable()
#     # 获取测试类对象的所有属性和方法
#     methods = dir(t)
#     # 筛选出所有以 test_ 开头的方法名
#     test_methods = [method for method in methods if method.startswith('test_')]
#     # 逐个调用测试方法
#     for method in test_methods:
#         getattr(t, method)()
#     return 

def debugTest(test_object):
    t: test_object = TestTable()
    t.test_table()


#test_database(HttpTest())
debugTest(HttpTest())

