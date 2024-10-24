import time

# import pytest
from infinity_cluster import InfinityCluster
from mocked_infinity_cluster import MockInfinityCluster


def test_0(infinity_path: str):
    cluster = InfinityCluster(infinity_path)
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.init_leader("node1")
    cluster.init_follower("node2")

    time.sleep(1)

    # cluster.remove("node2")
    # cluster.remove("node1")


# @pytest.mark.skip(reason="tmp")
def test_mock(infinity_path: str):
    cluster = MockInfinityCluster(infinity_path)
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.init_leader("node1")
    cluster.init_follower("node2")

    time.sleep(1)

    # cluster.disconnect("node2")
    # time.sleep(0.1)
    # cluster.reconnect("node2")

    # time.sleep(1)

    # cluster.remove("node2")
    # cluster.remove("node1")
