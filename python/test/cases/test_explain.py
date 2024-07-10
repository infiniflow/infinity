import pytest
from common import common_values
from internal.test_explain import TestExplain
from infinity.common import ConflictType
from internal.utils import trace_expected_exceptions
import numpy as np
import pandas as pd

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_LOCAL_HOST
        self.test_infinity_obj = TestExplain(self.uri)

    def teardown(self):
        self.test_infinity_obj.disconnect()

    def test_explain(self):
        self.test_infinity_obj._test_explain_default()
