import os
import time

from numpy import dtype
import pandas as pd
import pytest
from infinity_cluster import InfinityCluster
from infinity.common import ConflictType
import common_values


class TestImport:
    def test1(self, cluster: InfinityCluster):
        try:
            cluster.add_node("node1", "conf/leader.toml")
            cluster.add_node("node2", "conf/follower.toml")

            print("init nodes")

            cluster.set_leader("node1")
            cluster.set_follower("node2")

            time.sleep(1)
            print("import in node1")
            table_name = "test_import1"

            infinity1 = cluster.client("node1")
            db_obj1 = infinity1.get_database("default_db")
            db_obj1.drop_table(table_name, ConflictType.Ignore)
            table_obj1 = db_obj1.create_table(
                table_name, {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}
            )

            test_csv_dir = common_values.TEST_DATA_DIR + "csv/embedding_int_dim3.csv"
            print(f"import file: {test_csv_dir}")
            assert os.path.exists(test_csv_dir)

            res = table_obj1.import_data(test_csv_dir)
            assert res.error_code == 0

            res_gt = pd.DataFrame(
                {
                    "c1": (1, 5, 9),
                    "c2": ([2, 3, 4], [6, 7, 8], [10, 11, 12]),
                }
            ).astype({"c1": dtype("int32"), "c2": dtype("object")})

            res = table_obj1.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, res_gt)

            time.sleep(1)
            print("select in node2")

            infinity2 = cluster.client("node2")
            db_obj2 = infinity2.get_database("default_db")
            table_obj2 = db_obj2.get_table(table_name)
            res = table_obj2.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, res_gt)

            db_obj1.drop_table(table_name)

        except Exception as e:
            print(e)
            cluster.clear()
            raise
        else:
            cluster.clear()
