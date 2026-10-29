import infinity
import pytest
from infinity import index
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.infinity_http import infinity_http
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteDatabase
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
from infinity.remote_thrift.table import RemoteTable

from common import common_values


@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")


@pytest.fixture(scope="class")
def setup_class(request, http):
    if http:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity_http()
    else:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity.connect(uri)
    request.cls.uri = uri
    yield
    request.cls.infinity_obj.disconnect()


@pytest.mark.usefixtures("setup_class")
@pytest.mark.ubsan
class TestInfinity:
    @pytest.mark.usefixtures("skip_if_local_infinity")
    @pytest.mark.usefixtures("skip_if_http")
    def test_query(self):
        conn = ThriftInfinityClient(common_values.TEST_LOCAL_HOST)
        db = RemoteDatabase(conn, "default_db")
        db.drop_table("my_table", conflict_type=ConflictType.Ignore)
        db.create_table(
            "my_table", {
                "num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector,5,float"}},
            ConflictType.Error)

        table = RemoteTable(conn, "default_db", "my_table")
        res = table.insert(
            [{"num": 1, "body": "undesirable, unnecessary, and harmful", "vec": [1.0] * 5}])
        assert res.error_code == ErrorCode.OK
        res = table.insert(
            [{"num": 2, "body": "publisher=US National Office for Harmful Algal Blooms", "vec": [4.0] * 5}])
        assert res.error_code == ErrorCode.OK
        res = table.insert(
            [{"num": 3, "body": "in the case of plants, growth and chemical", "vec": [7.0] * 5}])
        assert res.error_code == ErrorCode.OK

        res = table.create_index("my_index",
                                 index.IndexInfo("body",
                                                 index.IndexType.FullText),
                                 ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # Create a query builder
        query_builder = InfinityThriftQueryBuilder(table)
        query_builder.output(["num", "body"])
        query_builder.match_dense('vec', [3.0] * 5, 'float', 'ip', 2)
        query_builder.match_text('body', 'harmful', 2, None)
        query_builder.fusion(method='rrf', topn=10, fusion_params=None)
        res, extra_result = query_builder.to_df()
        print(res)
        res = table.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db.drop_table("my_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = conn.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    def test_query_builder(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_query_builder",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_query_builder", {"c1": {"type": "int"}}, ConflictType.Error)
        query_builder = table_obj.query_builder
        res = query_builder.output(["*"]).to_df()
        print(res)

        res = db_obj.drop_table("test_query_builder", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_none_clears_optional_clauses(self, request):
        # the optional query clauses all accept None in their type hints, but
        # the builders crashed on it: sort/highlight/output raised TypeError
        # iterating None, filter/having died inside sqlglot with "ParseError:
        # SQL cannot be None", and only group_by had a None guard. None now
        # clears the clause, matching limit(None)/offset(None).
        from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
        qb = InfinityThriftQueryBuilder(None)
        for clause in (qb.output, qb.highlight, qb.filter, qb.having, qb.group_by, qb.sort):
            clause(None)
        assert qb._columns is None and qb._highlight is None
        assert qb._filter is None and qb._having is None
        assert qb._groupby is None and qb._sort is None
        # a real clause still applies after the None clears
        qb.output(["c1"])
        assert qb._columns is not None

        if not request.config.getoption("--local-infinity"):
            pytest.skip("embedded part needs the embedded engine")
        from infinity_embedded.local_infinity.query_builder import InfinityLocalQueryBuilder
        qb = InfinityLocalQueryBuilder(None)
        for clause in (qb.output, qb.highlight, qb.filter, qb.having, qb.group_by, qb.sort):
            clause(None)
        assert qb._columns is None and qb._highlight is None
        assert qb._filter is None and qb._having is None
        assert qb._group_by is None and qb._sort is None
