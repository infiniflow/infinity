import time

import pytest
from infinity_cluster import InfinityCluster
from mocked_infinity_cluster import MockInfinityCluster
from docker_infinity_cluster import DockerInfinityCluster
import time


def test_admin(cluster: InfinityCluster):
    with cluster:
        cluster.add_node("test", "conf/leader.toml")
        infinity1 = cluster.client("test")
        assert infinity1 is not None
        res = infinity1.show_current_node()
        print(res.node_name)
        assert (res.node_role == "admin")


def test_standalone(cluster: InfinityCluster):
    with cluster:
        cluster.add_node("test", "conf/pytest_parallel_infinity_conf.toml")
        cluster.set_standalone("test")
        test_client = cluster.client("test")
        assert test_client is not None

        test_client.create_database("db1")
        test_client.drop_database("db1")


def test_0(cluster: InfinityCluster):
    with cluster:
        cluster.add_node("node1", "conf/leader.toml")
        cluster.add_node("node2", "conf/follower.toml")

        cluster.set_leader("node1")
        cluster.set_follower("node2")

        time.sleep(1)

        cluster.remove_node("node2")
        cluster.remove_node("node1")


@pytest.mark.skip(reason="deprecated")
def test_mock(mock_cluster: MockInfinityCluster):
    cluster = mock_cluster
    with cluster:
        cluster.add_node("node1", "conf/leader.toml")
        cluster.add_node("node2", "conf/follower.toml")

        cluster.set_leader("node1")
        cluster.set_follower("node2")

        time.sleep(1)

        cluster.disconnect("node2")
        time.sleep(0.1)
        cluster.reconnect("node2")

        time.sleep(1)

        cluster.remove_node("node2")
        cluster.remove_node("node1")


@pytest.mark.docker
def test_docker(docker_cluster: DockerInfinityCluster):
    cluster = docker_cluster

    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    print("init nodes")

    cluster.set_leader("node1")
    cluster.set_follower("node2")

    time.sleep(1)

    cluster.disconnect("node2")
    time.sleep(0.1)
    cluster.reconnect("node2")

    res = cluster.client("node1").list_databases()
    print(res.db_names)

    time.sleep(1)

    print("remove nodes")

    cluster.clear()
