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

class TestSelect:
    def test_select(self, cluster: InfinityCluster):
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

            db_obj.drop_table("test_select", ConflictType.Ignore)
            table_obj = db_obj.create_table(
                "test_select", {
                    "c1": {"type": "int", "constraints": ["primary key", "not null"]},
                    "c2": {"type": "int", "constraints": ["not null"]}}, ConflictType.Error)

            res = table_obj.insert(
                [{"c1": -3, "c2": -3}, {"c1": -2, "c2": -2}, {"c1": -1, "c2": -1}, {"c1": 0, "c2": 0}, {"c1": 1, "c2": 1},
                {"c1": 2, "c2": 2}, {"c1": 3, "c2": 3}])
            assert res.error_code == ErrorCode.OK

            res = table_obj.insert(
                [{"c1": -8, "c2": -8}, {"c1": -7, "c2": -7}, {"c1": -6, "c2": -6}, {"c1": 7, "c2": 7}, {"c1": 8, "c2": 8},
                {"c1": 9, "c2": 9}])
            assert res.error_code == ErrorCode.OK


            res = table_obj.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                            'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                        .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

            res = table_obj.output(["c1", "c2"]).to_df()
            pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                            'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                        .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

            res = table_obj.output(
                ["c1 + c2"]).filter("c1 = 3").to_df()
            pd.testing.assert_frame_equal(res, pd.DataFrame({'(c1 + c2)': (6,)})
                                        .astype({'(c1 + c2)': dtype('int32')}))

            time.sleep(1)
            db_obj_2 = infinity2.get_database("default_db")
            table_obj_2 = db_obj_2.get_table("test_select")
            res = table_obj_2.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                            'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                        .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

            res = table_obj_2.output(["c1", "c2"]).to_df()
            pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                            'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                        .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

            res = table_obj_2.output(
                ["c1 + c2"]).filter("c1 = 3").to_df()
            pd.testing.assert_frame_equal(res, pd.DataFrame({'(c1 + c2)': (6,)})
                                        .astype({'(c1 + c2)': dtype('int32')}))

            res = db_obj.drop_table("test_select", ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        except Exception as e:
            print(e)
            cluster.clear()
            raise
        else:
            cluster.clear()