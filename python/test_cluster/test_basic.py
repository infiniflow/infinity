import time

import pytest
from infinity_cluster import InfinityCluster
from mocked_infinity_cluster import MockInfinityCluster
from docker_infinity_cluster import DockerInfinityCluster, MinioParams
from numpy import dtype
import pandas as pd
import time
from infinity.errors import ErrorCode
from infinity.common import InfinityException
from infinity.common import ConflictType

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

@pytest.mark.usefixtures("skip_if_docker")
def test_tc1(cluster: InfinityCluster):
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
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.init_leader("node1")
    infinity1 = cluster.client("node1")
    res = infinity1.show_node("node1")
    assert(res.node_name == "node1")
    assert(res.node_role == "leader")
    assert(res.node_status == "alive")

    cluster.init_follower("node2")
    infinity2 = cluster.client("node2")
    res = infinity1.show_node("node1")
    assert(res.node_name == "node1")
    assert(res.node_role == "leader")
    assert(res.node_status == "alive")
    res = infinity1.show_node("node2")
    assert(res.node_name == "node2")
    assert(res.node_role == "follower")
    assert(res.node_status == "alive")

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

    try:
        table2.insert([{"c1": 1, "c2": [1.0, 2.0, 3.0, 4.0]}])
    except InfinityException as e:
        print(e)


    infinity1.set_role_admin()
    time.sleep(1)
    res = infinity2.show_node("node1")
    assert(res.node_status == "timeout")
    res = infinity2.show_node("node2")
    assert(res.node_name == "node2")
    assert(res.node_role == "follower")
    assert(res.node_status == "alive")

    table_name = "table2"
    db2 = infinity2.get_database("default_db")
    try:
        table2 = db1.create_table(
            table_name, {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}
        )
    except InfinityException as e:
        print(e)

    infinity1.set_role_leader("node1")
    time.sleep(1)
    res = infinity1.show_node("node1")
    assert(res.node_name == "node1")
    assert(res.node_role == "leader")
    assert(res.node_status == "alive")
    res = infinity1.show_node("node2")
    assert(res.node_name == "node2")
    assert(res.node_role == "follower")
    assert(res.node_status == "alive")

    table_name  = "table1"
    db1 = infinity1.get_database("default_db")
    table1 = db1.get_table(table_name)
    res = table1.output(["*"]).to_df()
    print(res)
    #table1.insert([{"c1": 2, "c2": [1.0, 2.0, 3.0, 4.0]}])
    #res_gt = pd.DataFrame(
    #    {
    #        "c1": (1, 2),
    #        "c2": ([[1.0, 2.0, 3.0, 4.0]], [[1.0, 2.0, 3.0, 4.0]]),
    #    }
    #).astype({"c1": dtype("int32"), "c2": dtype("object")})

    #res = table1.output(["*"]).to_df()
    #pd.testing.assert_frame_equal(res, res_gt)
    #res = db1.drop_table(table_name)
    #assert res.error_code == ErrorCode.OK

    time.sleep(1)
    cluster.remove_node("node2")
    cluster.remove_node("node1")
    cluster.clear()