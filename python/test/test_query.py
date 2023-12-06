from infinity.common import REMOTE_HOST
from infinity.query import InfinityVectorQueryBuilder
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteDatabase
from infinity.remote_thrift.table import RemoteTable


class TestQuery:

    def test_query(self):
        conn = ThriftInfinityClient(REMOTE_HOST)
        db = RemoteDatabase(conn, "default")
        db.create_table("my_table", {"c1": "vector,5,float"}, None)

        table = RemoteTable(conn, "default", "my_table")
        res = table.insert([{"c1": [1.0] * 5}])
        assert res.success
        res = table.insert([{"c1": [4.0] * 5}])
        assert res.success
        res = table.insert([{"c1": [7.0] * 5}])
        assert res.success

        # select_res = table.search().output(["*"]).to_df()
        # print(select_res)
        # Create a query builder
        query_builder = InfinityVectorQueryBuilder.create(
            table=table,
            embedding=[0.1, 0.2, 0.3, 0.4, 0.5],  # query embedding
            vector_column_name="c1",  # vector column name
            distance="L2",  # distance type
            threshold=0.5  # threshold
        )


        query_builder.output(["c1"])

        # set topk
        query_builder.limit(10)
        #
        # result
        res = query_builder.to_df()
        #
        # print result
        print(res)
