import time
import os
from numpy import dtype
import pandas as pd
import pytest
from infinity_cluster import InfinityCluster
from infinity.common import ConflictType
from infinity.common import InfinityException
from infinity.errors import ErrorCode
import common_values

class TestKnn:
    def test_knn(self, cluster: InfinityCluster):
        try:
            cluster.add_node("node1", "conf/leader.toml")
            cluster.add_node("node2", "conf/follower.toml")

            print("init nodes")
            cluster.set_leader("node1")
            cluster.set_follower("node2")
            time.sleep(1)

            infinity1 = cluster.client("node1")
            infinity2 = cluster.client("node2")

            db_obj = infinity1.get_database("default_db")

            test_csv_dir = common_values.TEST_DATA_DIR + "csv/tmp_20240116.csv"
            print(f"import file: {test_csv_dir}")
            assert os.path.exists(test_csv_dir)
            if not os.path.exists(common_values.TEST_TMP_DIR):
                os.makedirs(common_values.TEST_TMP_DIR)

            db_obj.drop_table("test_knn", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_knn", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
            }, ConflictType.Error)
            res = table_obj.import_data(test_csv_dir, None)
            assert res.error_code == ErrorCode.OK

            res = table_obj.output(["variant_id", "_row_id"]).match_dense("gender_vector", [1.0] * 4, "float", "ip", 10).to_pl()
            print(res)

            time.sleep(1)
            db_obj_2 = infinity2.get_database("default_db")
            table_obj_2 = db_obj_2.get_table("test_knn")
            res = table_obj_2.output(["variant_id", "_row_id"]).match_dense("gender_vector", [1.0] * 4, "float", "ip", 10).to_pl()
            print(res)

            res = db_obj.drop_table("test_knn", ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        except Exception as e:
            print(e)
            cluster.clear()
            raise
        else:
            cluster.clear()