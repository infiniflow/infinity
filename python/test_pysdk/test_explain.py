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
    def test_explain_with_match_dense(self, suffix):
        """
        Regression test: explain() must send the search expression to the server.
        The HTTP client used to build the explain request from stale fields
        (_knn/_match/_match_tensor/_match_sparse/_fusion) that were never
        populated, so every match_*/fusion clause was silently dropped and
        the server explained a plain table scan instead of the search query.
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_explain_match_dense"+suffix, ConflictType.Ignore)
        table = db_obj.create_table("test_explain_match_dense"+suffix, {
            "c1": {"type": "varchar", "constraints": ["primary key"]},
            "vec": {"type": "vector,4,float"}}, ConflictType.Error)
        assert table

        res = table.output(["c1"]).match_dense("vec", [0.1, 0.2, 0.3, 0.4], "float", "l2", 10) \
            .explain(ExplainType.Physical)
        # the physical plan of a match_dense query must contain a KNN SCAN operator
        assert "KNN SCAN" in str(res)

        db_obj.drop_table("test_explain_match_dense"+suffix, ConflictType.Error)
