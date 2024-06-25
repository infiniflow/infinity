from common import common_values
import infinity
from infinity.errors import ErrorCode
class TestSdk():
    def __init__(self, uri=common_values.TEST_LOCAL_HOST, connect=True):
        self.uri = uri
        if connect:
            self.connect()
    def connect(self):
        self.infinity_obj = infinity.connect(self.uri)
        assert self.infinity_obj

    def disconnect(self):
        res = self.infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK
    # def setup(self):
    #     print("test sdk setup")
    #     self.infinity_obj = infinity.connect(self.uri)
    #     assert self.infinity_obj
    #     # assert self.infinity_obj.disconnect()
    # def teardown(self):
    #     print("test sdk teardown")
    #     res = self.infinity_obj.disconnect()
    #     assert res.error_code == ErrorCode.OK
    def test_version(self):
        print("test_sdk")
