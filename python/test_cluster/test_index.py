import time

from numpy import dtype
import pandas as pd
import pytest
from infinity_cluster import InfinityCluster
from infinity.common import ConflictType
import infinity.index as index


class TestIndex:
    def test1(self, cluster: InfinityCluster):
        try:
            cluster.add_node("node1", "conf/leader.toml")
            cluster.add_node("node2", "conf/follower.toml")

            print("init nodes")
            cluster.set_leader("node1")
            cluster.set_follower("node2")
            time.sleep(1)

            table_name = "test_index1"
            index_name = "idx1"

            print("create index in node1")
            infinity1 = cluster.client("node1")
            db_obj1 = infinity1.get_database("default_db")
            db_obj1.drop_table(table_name, ConflictType.Ignore)

            table_obj1 = db_obj1.create_table(
                table_name, {"c1": {"type": "int"}, "c2": {"type": "varchar"}}
            )
            table_obj1.create_index(
                index_name, index.IndexInfo("c1", index.IndexType.Secondary)
            )
            index_list1 = table_obj1.list_indexes().index_list
            # print(index_list1)
            assert len(index_list1) == 1 and index_list1[0]["index_name"] == index_name

            time.sleep(1)

            print("check index in node2")
            infinity2 = cluster.client("node2")
            db_obj2 = infinity2.get_database("default_db")
            table_obj2 = db_obj2.get_table(table_name)
            index_list2 = table_obj2.list_indexes().index_list
            # print(index_list2)
            assert len(index_list2) == 1 and index_list2[0]["index_name"] == index_name

            print("insert in node1")
            table_obj1.insert(
                [
                    {"c1": 1, "c2": "text1"},
                    {"c1": 2, "c2": "text2"},
                    {"c1": 3, "c2": "text3"},
                ]
            )
            res_gt = pd.DataFrame(
                {
                    "c1": (1, 2),
                    "c2": ("text1", "text2"),
                }
            ).astype({"c1": dtype("int32"), "c2": dtype("object")})
            res1 = table_obj1.output(["*"]).filter("c1 < 3").to_df()
            pd.testing.assert_frame_equal(res1, res_gt)

            print("select in node2")
            time.sleep(1)
            res2 = table_obj2.output(["*"]).filter("c1 < 3").to_df()
            # print(res2)
            pd.testing.assert_frame_equal(res2, res_gt)

            print("drop index in node1 and check in node2")
            table_obj1.drop_index(index_name)
            time.sleep(1)
            index_list2 = table_obj2.list_indexes().index_list
            assert len(index_list2) == 0

            print("uninit cluster")
            db_obj1.drop_table(table_name)

        except Exception as e:
            print(e)
            cluster.clear()
            raise
        else:
            cluster.clear()
