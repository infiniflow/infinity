import time

import pytest
from infinity_cluster import InfinityCluster
from mocked_infinity_cluster import MockInfinityCluster
from docker_infinity_cluster import DockerInfinityCluster


@pytest.mark.usefixtures("skip_if_docker")
def test_standalone(infinity_path: str):
    cluster = InfinityCluster(infinity_path)
    cluster.add_node("test", "conf/pytest_parallel_infinity_conf.toml")
    cluster.init_standalone("test")
    test_client = cluster.client("test")
    assert test_client is not None

    test_client.create_database("db1")
    test_client.drop_database("db1")

    cluster.clear()


@pytest.mark.usefixtures("skip_if_docker")
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

    cluster.clear()


@pytest.mark.usefixtures("skip_if_docker")
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

    cluster.clear()


@pytest.mark.usefixtures("skip_if_not_docker")
def test_docker(infinity_path: str):
    cluster = DockerInfinityCluster(infinity_path)

    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    print("init nodes")

    cluster.init_leader("node1")
    cluster.init_follower("node2")

    time.sleep(1)

    cluster.disconnect("node2")
    time.sleep(0.1)
    cluster.reconnect("node2")

    time.sleep(1)

    print("remove nodes")

    cluster.remove_node("node2")
    cluster.remove_node("node1")

    cluster.clear()
