import common_values
import infinity.index as index
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteDatabase
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
from infinity.remote_thrift.table import RemoteTable


class TestQuery:
    def test_query(self):
        conn = ThriftInfinityClient(common_values.TEST_REMOTE_HOST)
        db = RemoteDatabase(conn, "default")
        db.drop_table("my_table")
        db.create_table(
            "my_table", {"num": "integer", "body": "varchar", "vec": "vector,5,float"}, None)

        table = RemoteTable(conn, "default", "my_table")
        res = table.insert(
            [{"num": 1, "body": "undesirable, unnecessary, and harmful", "vec": [1.0] * 5}])
        assert res.success
        res = table.insert(
            [{"num": 2, "body": "publisher=US National Office for Harmful Algal Blooms", "vec": [4.0] * 5}])
        assert res.success
        res = table.insert(
            [{"num": 3, "body": "in the case of plants, growth and chemical", "vec": [7.0] * 5}])
        assert res.success

        res = table.create_index("my_index",
                                 [index.IndexInfo("body",
                                                  index.IndexType.FullText,
                                                  [index.InitParameter("ANALYZER", "segmentation")]),
                                  ], None)
        assert res.success

        # select_res = table.query_builder().output(["*"]).to_df()
        # print(select_res)
        # Create a query builder
        query_builder = InfinityThriftQueryBuilder(table)
        query_builder.output(["num", "body"])
        query_builder.knn('vec', [3.0] * 5, 'float', 'ip', 2)
        query_builder.match('body', 'harmful', 'topn=2')
        query_builder.fusion('rrf')
        res = query_builder.to_df()
        print(res)
