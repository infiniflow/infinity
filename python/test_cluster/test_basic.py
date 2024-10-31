import time

import pytest
from infinity_cluster import InfinityCluster
from mocked_infinity_cluster import MockInfinityCluster
from docker_infinity_cluster import DockerInfinityCluster, MinioParams


@pytest.mark.usefixtures("skip_if_docker")
def test_standalone(cluster: InfinityCluster):
    cluster.add_node("test", "conf/pytest_parallel_infinity_conf.toml")
    cluster.init_standalone("test")
    test_client = cluster.client("test")
    assert test_client is not None

    test_client.create_database("db1")
    test_client.drop_database("db1")

    cluster.clear()


@pytest.mark.usefixtures("skip_if_docker")
def test_0(cluster: InfinityCluster):
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.init_leader("node1")
    cluster.init_follower("node2")

    time.sleep(1)

    cluster.remove_node("node2")
    cluster.remove_node("node1")

    cluster.clear()


@pytest.mark.usefixtures("skip_if_docker")
@pytest.mark.skip(reason="deprecated")
def test_mock(mock_cluster: MockInfinityCluster):
    cluster = mock_cluster

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
def test_docker(docker_cluster: DockerInfinityCluster):
    cluster = docker_cluster

    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    print("init nodes")

    cluster.init_leader("node1")
    cluster.init_follower("node2")

    time.sleep(1)

    cluster.disconnect("node2")
    time.sleep(0.1)
    cluster.reconnect("node2")

    res = cluster.client("node1").list_databases()
    print(res.db_names)

    time.sleep(1)

    print("remove nodes")

    cluster.clear()
'''
tc1:
n1: admin
n2: admin
n1->leader
show node->n1 leader;
n2->follower
show node on n1 & n2->n1 leader, n2 follower;
create table t1 on n1;
insert data into t1 on n1;
select data t1 on n2;
try to insert data into t1 on n2; # fail
n1->admin
show node on n2->n1 timeout, n2 follower;
create table t2 on n2; # fail
n1->leader
show node on n1 & n2->n1 leader, n2 follower;
insert data into t1 on n1;
select data t1 on n1 and n2;
n2->admin
show node on n1;
insert data into t1 on n2;
n2->follower
show node on n1 & n2;
select data t1 on n1 and n2;

'''
