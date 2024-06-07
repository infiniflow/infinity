from common import common_values
class TestSdk():
    def __init__(self, uri=common_values.TEST_REMOTE_HOST):
        self.uri = uri
    def test_version(self):
        print("test_sdk")
