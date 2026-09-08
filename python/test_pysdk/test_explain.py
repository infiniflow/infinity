import infinity
import polars as pl
import pytest
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.infinity_http import infinity_http
from infinity.table import ExplainType

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

    def test_explain(self, suffix):
        """
            # Analyze = 1
            # Ast = 2
            # UnOpt = 3
            # Opt = 4
            # Physical = 5
            # Pipeline = 6
            # Fragment = 7
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_explain_default"+suffix, ConflictType.Ignore)
        table = db_obj.create_table("test_explain_default"+suffix, {
            "c1": {"type": "varchar", "constraints": ["primary key"]}, "c2": {"type": "float"}}, ConflictType.Error)
        assert table

        table.insert({"c1": "hello", "c2": 1.0})
        table.insert({"c1": "world", "c2": 2.0})
        table.insert({"c1": "hello", "c2": 3.0})

        with pl.Config(fmt_str_lengths=1000):
            res = table.output(["*"]).explain()
            print(res)

            res = table.output(["*"]).explain(ExplainType.Ast)
            print(res)

            res = table.output(["*"]).explain(ExplainType.UnOpt)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Opt)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Physical)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Pipeline)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Fragment)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Analyze)
            print(res)

        db_obj.drop_table("test_explain_default"+suffix, ConflictType.Error)

    def test_explain_group_by_having(self, suffix):
        # explain() must carry group by / having into the explained query
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_explain_group_by_having"+suffix, ConflictType.Ignore)
        table = db_obj.create_table("test_explain_group_by_having"+suffix, {
            "c1": {"type": "varchar"}, "c2": {"type": "float"}}, ConflictType.Error)
        assert table

        table.insert({"c1": "hello", "c2": 1.0})
        table.insert({"c1": "world", "c2": 2.0})
        table.insert({"c1": "hello", "c2": 3.0})

        with pl.Config(fmt_str_lengths=1000):
            res = table.output(["c1", "sum(c2)"]).group_by(["c1"]).having("sum(c2) > 0").explain(ExplainType.Physical)
            plan = str(res)
            print(plan)
            # a group-by query explains through an Aggregate operator; without
            # forwarding group by the plan is a plain projection
            assert "Aggregate" in plan

        db_obj.drop_table("test_explain_group_by_having"+suffix, ConflictType.Error)