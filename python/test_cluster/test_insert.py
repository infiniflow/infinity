from numpy import dtype
import pandas as pd
from infinity_cluster import InfinityCluster
from docker_infinity_cluster import DockerInfinityCluster
import pytest
import time
from infinity.errors import ErrorCode
from infinity.common import ConflictType
import timeout_decorator


class TestInsert:
    def __test_inner_1(self, cluster: InfinityCluster):
        try:
            cluster.add_node("node1", "conf/leader.toml")
            cluster.add_node("node2", "conf/follower.toml")

            print("init nodes")

            cluster.set_leader("node1")
            cluster.set_follower("node2")

            time.sleep(1)
            print("insert in node1")

            infinity1 = cluster.client("node1")
            r = infinity1.list_databases()

            table_name = "table1"
            db1 = infinity1.get_database("default_db")
            res = db1.drop_table(table_name, ConflictType.Ignore)
            table1 = db1.create_table(
                table_name, {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}
            )
            table1.insert([{"c1": 1, "c2": [1.0, 2.0, 3.0, 4.0]}])

            res_gt = pd.DataFrame(
                {
                    "c1": (1),
                    "c2": ([[1.0, 2.0, 3.0, 4.0]]),
                }
            ).astype({"c1": dtype("int32"), "c2": dtype("object")})

            res = table1.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, res_gt)

            time.sleep(1)
            print("select in node2")

            infinity2 = cluster.client("node2")
            db2 = infinity2.get_database("default_db")
            table2 = db2.get_table(table_name)
            res = table2.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, res_gt)

            res = db1.drop_table(table_name)
            assert res.error_code == ErrorCode.OK
        except Exception as e:
            print(e)
            cluster.clear()
            raise
        else:
            cluster.clear()

    def test_insert_11(self, cluster: InfinityCluster):
        self.__test_inner_1(cluster)

    @pytest.mark.docker
    def test_insert_12(self, docker_cluster: DockerInfinityCluster):
        self.__test_inner_1(docker_cluster)

    # read/write when leader/follower is disconnected
    def test_insert_2(self, docker_cluster: DockerInfinityCluster):
        try:
            docker_cluster.add_node("node1", "conf/leader.toml")
            docker_cluster.add_node("node2", "conf/follower.toml")
            print("init nodes")
            time.sleep(1)
            docker_cluster.set_leader("node1")
            docker_cluster.set_follower("node2")

            time.sleep(1)
            print("insert in node1")

            infinity1 = docker_cluster.client("node1")

            table_name = "table2"
            db1 = infinity1.get_database("default_db")
            db1.drop_table(table_name, ConflictType.Ignore)
            table1 = db1.create_table(
                table_name, {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}
            )
            table1.insert([{"c1": 1, "c2": [1.0, 2.0, 3.0, 4.0]}])

            res_gt = pd.DataFrame(
                {
                    "c1": (1),
                    "c2": ([[1.0, 2.0, 3.0, 4.0]]),
                }
            ).astype({"c1": dtype("int32"), "c2": dtype("object")})

            time.sleep(1)
            print("select in node2")

            infinity2 = docker_cluster.client("node2")
            db2 = infinity2.get_database("default_db")
            table2 = db2.get_table(table_name)
            res = table2.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, res_gt)

            docker_cluster.disconnect("node2")
            try:

                @timeout_decorator.timeout(1)
                def noreturn_request():
                    table1.insert([{"c1": 2, "c2": [5.0, 6.0, 7.0, 8.0]}])

                noreturn_request()

            except Exception as e:
                pass
            docker_cluster.reconnect("node2")

            docker_cluster.disconnect("node1")
            res = table2.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, res_gt)
            docker_cluster.reconnect("node1")

            db1.drop_table(table_name)
        except Exception as e:
            print(e)
            docker_cluster.clear()
            raise
        else:
            docker_cluster.clear()

    @pytest.mark.skip("Bug")
    def test_insert_3(self, docker_cluster: DockerInfinityCluster):
        try:
            docker_cluster.add_node("node1", "conf/leader.toml")
            docker_cluster.add_node("node2", "conf/follower.toml")
            print("init nodes")
            time.sleep(1)
            docker_cluster.set_leader("node1")
            docker_cluster.set_follower("node2")

            time.sleep(1)
            print("insert in node1")

            infinity1 = docker_cluster.client("node1")

            table_name = "table2"
            db1 = infinity1.get_database("default_db")
            db1.drop_table(table_name, ConflictType.Ignore)
            table1 = db1.create_table(
                table_name, {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}
            )
            table1.insert([{"c1": 1, "c2": [1.0, 2.0, 3.0, 4.0]}])
            time.sleep(1)

            # reconnect follower
            docker_cluster.remove_node("node2")
            docker_cluster.add_node("node2", "conf/follower.toml")
            time.sleep(1)
            docker_cluster.set_follower("node2")

            table1.insert([{"c1": 2, "c2": [5.0, 6.0, 7.0, 8.0]}])
            res_gt = pd.DataFrame(
                {
                    "c1": (1, 2),
                    "c2": ([[1.0, 2.0, 3.0, 4.0]], [[5.0, 6.0, 7.0, 8.0]]),
                }
            ).astype({"c1": dtype("int32"), "c2": dtype("object")})

            infinity2 = docker_cluster.client("node2")
            db2 = infinity2.get_database("default_db")
            table2 = db2.get_table(table_name)
            res = table2.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, res_gt)

            # reconnect leader
            docker_cluster.remove_node("node1")
            docker_cluster.add_node("node1", "conf/leader.toml")
            time.sleep(1)
            docker_cluster.set_leader("node1")

            table1.insert([{"c1": 3, "c2": [9.0, 10.0, 11.0, 12.0]}])
            res_gt = pd.DataFrame(
                {
                    "c1": (1, 2, 3),
                    "c2": (
                        [[1.0, 2.0, 3.0, 4.0]],
                        [[5.0, 6.0, 7.0, 8.0]],
                        [[9.0, 10.0, 11.0, 12.0]],
                    ),
                }
            ).astype({"c1": dtype("int32"), "c2": dtype("object")})
            res = table2.output(["*"]).to_df()
            pd.testing.assert_frame_equal(res, res_gt)

            db1.drop_table(table_name)
        except Exception as e:
            print(e)
            docker_cluster.clear()
            raise
        else:
            docker_cluster.clear()
