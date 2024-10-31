from numpy import dtype
import pandas as pd
from infinity_cluster import InfinityCluster
from docker_infinity_cluster import DockerInfinityCluster
import pytest
import time
from infinity.errors import ErrorCode


class TestInsert:
    def __test_inner_1(self, cluster: InfinityCluster):
        try:
            cluster.add_node("node1", "conf/leader.toml")
            cluster.add_node("node2", "conf/follower.toml")

            print("init nodes")

            cluster.init_leader("node1")
            cluster.init_follower("node2")

            time.sleep(1)
            print("insert in node1")

            infinity1 = cluster.client("node1")
            r = infinity1.list_databases()

            table_name = "table1"
            db1 = infinity1.get_database("default_db")
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

    @pytest.mark.usefixtures("skip_if_docker")
    def test_insert_11(self, cluster: InfinityCluster):
        self.__test_inner_1(cluster)

    @pytest.mark.usefixtures("skip_if_not_docker")
    def test_insert_12(self, docker_cluster: DockerInfinityCluster):
        self.__test_inner_1(docker_cluster)
