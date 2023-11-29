import os
from sqlglot import condition

import infinity
from infinity.infinity import NetworkAddress
from infinity.remote_thrift.table import traverse_conditions


class TestIndex:

    def test_create_index(self):
        infinity_obj = infinity.connect()
        db_obj = infinity_obj.get_database("default")
        # res = db_obj.create_table("test_index", {"c1": "vector,1024,float"}, None)
        # assert res.success

        table_obj = db_obj.get_table("test_index")
        assert table_obj

        res = table_obj.create_index("my_index", ["c1"], "IVFFlat", [{"centroids_count": 128}, {"metric": "l2"}],
                                     None)
        assert res.success
