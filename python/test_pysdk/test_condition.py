import infinity
import pytest
from infinity.common import InfinityException
from infinity.errors import ErrorCode
from infinity.infinity_http import infinity_http
from infinity.remote_thrift.table import traverse_conditions
from sqlglot import condition

from common import common_values


@pytest.mark.usefixtures("http")
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, http):
        if http:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity_http()
        else:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity.connect(self.uri)
        assert self.infinity_obj

    def teardown(self):
        res = self.infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_condition(self):
        for cond_str in [
            "c1 = 1",
            "c1 != 1",
            "c1 > 1 and c2 < 2 or c3 = 3.3",
            "-8 < c1 and c1 <= -7",
            "(-7 < c1 or 9 <= c1) and (c1 = 3)",
            "!(9 <= c1)",
            "c1 IN (1,2,3)",
            "c1 NOT IN (1,2,3)",
            "c1 LIKE '%test%'",
            "c1 NOT LIKE '%test%'",
            "c1 LIKE '%test%' ESCAPE '!'",
            "c1 NOT LIKE '%test%' ESCAPE '!'",
        ]:
            print(cond_str)
            cond = condition(cond_str)
            print(cond)
            res = traverse_conditions(cond)
            print(res)
            assert res

    def test_condition_unsupported_expression(self):
        # Unsupported predicates must fail with a clean InfinityException.
        # The fallback arm of traverse_conditions used to recurse on
        # cons[1], a same-class sqlglot copy, so filters like these crashed
        # the client with RecursionError instead.
        for cond_str in [
            "c1 = (select 1)",
            "(c1, c2) = (1, 2)",
            "c1 = interval 1 day",
            "c1[1] = 5",
            "c1 = any (1)",
        ]:
            with pytest.raises(InfinityException, match="unknown expression type") as exc_info:
                traverse_conditions(condition(cond_str))
            assert exc_info.value.error_code == ErrorCode.INVALID_EXPRESSION
