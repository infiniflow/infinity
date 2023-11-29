import infinity
import infinity.index as index
from infinity.infinity import NetworkAddress


class TestIndex:

    def test_create_index_IVFFlat(self):
        infinity_obj = infinity.connect(NetworkAddress('192.168.200.151', 9080))
        db_obj = infinity_obj.get_database("default")
        res = db_obj.create_table("test_index_ivfflat", {"c1": "vector,1024,float"}, None)
        assert res.success

        table_obj = db_obj.get_table("test_index_ivfflat")
        assert table_obj

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)
        assert res.success

        res = table_obj.drop_index("my_index")
        assert res.success

    def test_create_index_HNSW(self):
        # CREATE INDEX idx1 ON test_hnsw (col1) USING Hnsw WITH (M = 16, ef_construction = 50, ef = 50, metric = l2);
        infinity_obj = infinity.connect(NetworkAddress('192.168.200.151', 9080))
        db_obj = infinity_obj.get_database("default")
        res = db_obj.create_table("test_index_hnsw", {"c1": "vector,1024,float"}, None)
        assert res.success

        table_obj = db_obj.get_table("test_index_hnsw")
        assert table_obj

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.Hnsw,
                                                      [index.InitParameter("M", "16"),
                                                       index.InitParameter("ef_construction", "50"),
                                                       index.InitParameter("ef", "50"),
                                                       index.InitParameter("metric", "l2")])], None)

        assert res.success

        res = table_obj.drop_index("my_index")
        assert res.success
