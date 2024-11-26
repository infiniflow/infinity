import threading
import time

import pytest
from infinity_cluster import InfinityCluster
from numpy import dtype
import pandas as pd
import time
from infinity.errors import ErrorCode
from infinity.common import InfinityException
from infinity.common import ConflictType
from database_operations import do_some_operations_cluster, clear_instance
from infinity_http import infinity_http
from database_operations import instance_state

# FIXME: when running multiple times without clear_instance, this test case will fail.
def test_cluster_leader_follower_change(cluster : InfinityCluster):
    '''
    n1 : leader
    n2 : follower
    n1 does operations
    
    n1 -> admin
    n2 -> leader
    n1 -> follower

    n2 does operations
    '''
    with cluster:
        cluster.add_node("node1", "conf/leader.toml")
        cluster.add_node("node2", "conf/follower.toml")

        cluster.set_leader("node1")
        cluster.set_follower("node2")
        node1_client = cluster.client("node1")
        node2_client = cluster.client("node2")
        leader_state = instance_state(node1_client)

        res = node1_client.show_node("node1")
        assert (res.node_name == "node1")
        assert (res.node_role == "leader")
        assert (res.node_status == "alive")

        res = node1_client.show_node("node2")
        assert (res.node_name == "node2")
        assert (res.node_role == "follower")
        assert (res.node_status == "alive")

        do_some_operations_cluster(node1_client, [node2_client], leader_state)

        cluster.set_admin("node1")
        cluster.set_leader("node2")
        cluster.set_follower("node1")

        res = node1_client.show_node("node1")
        assert (res.node_name == "node1")
        assert (res.node_role == "follower")
        assert (res.node_status == "alive")

        res = node1_client.show_node("node2")
        assert (res.node_name == "node2")
        assert (res.node_role == "leader")
        assert (res.node_status == "alive")

        do_some_operations_cluster(node2_client, [node1_client], leader_state)

        clear_instance(leader_state, node2_client)
        cluster.remove_node("node1")
        cluster.remove_node("node2")


# @pytest.mark.parametrize("kill", [False, True])
@pytest.mark.skip(reason="bugs")
@pytest.mark.parametrize("leader_shutdown", [True, False])
def test_cluster_shutdown_and_recover(cluster: InfinityCluster, leader_shutdown: bool):
    kill = False
    with cluster:
        logger = cluster.logger

        cluster.add_node("node1", "conf/leader.toml")
        cluster.add_node("node2", "conf/follower.toml")
        time.sleep(0.5)
        cluster.set_leader("node1")
        cluster.set_follower("node2")

        table_name = "test_cluster_leader_shutdown_and_recover_in_follower"

        def init():
            infinity_n1 = cluster.client("node1")
            infinity_n1.get_database("default_db").drop_table(
                table_name, ConflictType.Ignore
            )
            table_n1 = infinity_n1.get_database("default_db").create_table(
                table_name, {"c1": {"type": "int"}}
            )

        init()

        insert_line = 0
        insert_time_in_sec = 3
        shutdown_num = 10

        for i in range(shutdown_num):
            shutdown = False
            leader_name = cluster.leader_name
            follower_name = "node2" if leader_name == "node1" else "node1"
            infinity_leader: infinity_http = cluster.client(leader_name)

            def shutdown_leader(sleep: int, shutdown_leader: bool):
                nonlocal shutdown
                time.sleep(sleep)
                shutdown = True
                if shutdown_leader:
                    cluster.remove_node(cluster.leader_name, kill=kill)
                    logger.debug(
                        f"test_i: {i}, shutdown leader {leader_name}, insert_line: {insert_line}"
                    )
                else:
                    cluster.remove_node(follower_name, kill=kill)
                    logger.debug(
                        f"test_i: {i}, shutdown follower {follower_name}, insert_line: {insert_line}"
                    )

            def insert_data():
                nonlocal insert_line
                table_leader = infinity_leader.get_database("default_db").get_table(
                    table_name
                )
                while not shutdown:
                    data = {"c1": insert_line}
                    try:
                        table_leader.insert([data])
                    except Exception:
                        if not shutdown:
                            raise
                        else:
                            logger.debug(
                                f"test_i: {i}, leader {leader_name} shutdown when insert, insert_line: {insert_line}"
                            )
                    else:
                        insert_line += 1

            t1 = threading.Thread(target=shutdown_leader, args=(insert_time_in_sec, leader_shutdown,))
            t1.start()
            insert_data()
            t1.join()

            def verify_data(node_name: str):
                nonlocal insert_line
                infinity: infinity_http = cluster.client(node_name)
                table = infinity.get_database("default_db").get_table(table_name)
                res = table.output(["*"]).to_df()
                if res.shape[0] == insert_line + 1:
                    insert_line += 1
                logger.debug(f"test_i: {i}, verify data, node_name: {node_name}")
                expected = pd.DataFrame({"c1": list(range(insert_line))}, dtype="int32")
                pd.testing.assert_frame_equal(res, expected)

            if leader_shutdown:
                verify_data(follower_name)

                cluster.set_leader(follower_name)
                # restart leader as follower
                follower_config = (
                    "conf/leader.toml" if leader_name == "node1" else "conf/follower.toml"
                )
                cluster.add_node(leader_name, follower_config)
                cluster.set_follower(leader_name)

                verify_data(leader_name)
            else:
                verify_data(leader_name)

                cluster.add_node(follower_name, "conf/follower.toml")
                cluster.set_follower(follower_name)

                verify_data(follower_name)

        def clear():
            infinity_n1 = cluster.client("node1")
            infinity_n1.get_database("default_db").drop_table(table_name)

        clear()
