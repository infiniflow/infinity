import pytest
from common import common_values
from src.test_explain import TestExplain
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
import numpy as np
import pandas as pd

class TestRemoteInfinity():
    def test_explain(self):
        test_infinity_obj = TestExplain(common_values.TEST_REMOTE_HOST)
        test_infinity_obj._test_explain_default()
