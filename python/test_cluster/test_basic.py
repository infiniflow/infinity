import time

import pytest
from infinity_cluster import InfinityCluster
from mocked_infinity_cluster import MockInfinityCluster


def test_standalone(infinity_path: str):
    cluster = InfinityCluster(infinity_path)
    cluster.add_node("test", "conf/pytest_parallel_infinity_conf.toml")
    cluster.init_standalone("test")
    test_client = cluster.client("test")
    assert test_client is not None

    test_client.create_database("db1")
    test_client.drop_database("db1")


@pytest.mark.skip(reason="bug")
def test_0(infinity_path: str):
    cluster = InfinityCluster(infinity_path)
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.init_leader("node1")
    cluster.init_follower("node2")

    time.sleep(1)

    cluster.remove_node("node2")
    cluster.remove_node("node1")


@pytest.mark.skip(reason="bug")
def test_mock(infinity_path: str):
    cluster = MockInfinityCluster(infinity_path)
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.init_leader("node1")
    cluster.init_follower("node2")

    time.sleep(1)

    cluster.disconnect("node2")
    time.sleep(0.1)
    cluster.reconnect("node2")

    time.sleep(1)

    cluster.remove_node("node2")
    cluster.remove_node("node1")
