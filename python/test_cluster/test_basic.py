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

def test_standalone(cluster: InfinityCluster):
    cluster.add_node("test", "conf/pytest_parallel_infinity_conf.toml")
    cluster.set_standalone("test")
    test_client = cluster.client("test")
    assert test_client is not None

    test_client.create_database("db1")
    test_client.drop_database("db1")

    cluster.clear()


def test_0(cluster: InfinityCluster):
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.set_leader("node1")
    cluster.set_follower("node2")

    time.sleep(1)

    cluster.remove_node("node2")
    cluster.remove_node("node1")

    cluster.clear()


@pytest.mark.skip(reason="deprecated")
def test_mock(mock_cluster: MockInfinityCluster):
    cluster = mock_cluster

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

    cluster.clear()


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
    n2->admin
    n2->follower
    show node on n1 & n2->n1 leader, n2 follower;
    insert data into t1 on n1;
    select data t1 on n1 and n2;
    n2->admin
    show node on n1;
    insert data into t1 on n2;
    n2->follower
    show node on n1 & n2;
    select data t1 on n1 and n2;
    drop t1 on n1;
    show table on n1 & n2;
    shutdown all nodes;
    '''
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.set_leader("node1")
    infinity1 = cluster.client("node1")
    res = infinity1.show_node("node1")
    assert(res.node_name == "node1")
    assert(res.node_role == "leader")
    assert(res.node_status == "alive")

    cluster.set_follower("node2")
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


    cluster.set_admin("node1")
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

    cluster.set_leader("node1")
    time.sleep(1)
    res = infinity1.show_node("node1")
    assert(res.node_name == "node1")
    assert(res.node_role == "leader")
    assert(res.node_status == "alive")

    try:
        infinity1.show_node("node2")
    except InfinityException as e:
        print(e)
        assert(e.error_code == 7019) # Not found node2

    res = infinity2.show_node("node1")
    assert(res.node_name == "node1")
    assert(res.node_role == "leader")
    assert(res.node_status == "lost connection")
    # reconnect leader and check the status
    cluster.set_admin("node2")
    cluster.set_follower("node2")
    time.sleep(1)
    res = infinity1.show_node("node2")
    assert(res.node_name == "node2")
    assert(res.node_role == "follower")
    assert(res.node_status == "alive")
    res = infinity2.show_node("node1")
    assert(res.node_name == "node1")
    assert(res.node_role == "leader")
    assert(res.node_status == "alive")


    table_name  = "table1"
    db1 = infinity1.get_database("default_db")
    table1 = db1.get_table(table_name)
    table1.insert([{"c1": 2, "c2": [1.0, 2.0, 3.0, 4.0]}])
    res_gt = pd.DataFrame(
        {
            "c1": (1, 2),
            "c2": ([1.0, 2.0, 3.0, 4.0], [1.0, 2.0, 3.0, 4.0]),
        }
    ).astype({"c1": dtype("int32"), "c2": dtype("object")})

    res = table1.output(["*"]).to_df()
    pd.testing.assert_frame_equal(res, res_gt)

    db2 = infinity2.get_database("default_db")
    table2 = db2.get_table(table_name)
    res = table2.output(["*"]).to_df()
    pd.testing.assert_frame_equal(res, res_gt)

    cluster.set_admin("node2")
    time.sleep(1)
    try:
        infinity1.show_node("node2")
    except InfinityException as e:
        print(e)
        assert(e.error_code == 7019) # Not found node2
    try:
        table2.insert([{"c1": 1, "c2": [1.0, 2.0, 3.0, 4.0]}])
    except InfinityException as e:
        print(e)

    cluster.set_follower("node2")
    time.sleep(1)
    res = infinity1.show_node("node2")
    assert(res.node_name == "node2")
    assert(res.node_role == "follower")
    assert(res.node_status == "alive")
    res = infinity2.show_node("node1")
    assert(res.node_name == "node1")
    assert(res.node_role == "leader")
    assert(res.node_status == "alive")

    res = table1.output(["*"]).to_df()
    pd.testing.assert_frame_equal(res, res_gt)
    res = table2.output(["*"]).to_df()
    pd.testing.assert_frame_equal(res, res_gt)

    res = db1.drop_table(table_name, ConflictType.Ignore)
    assert(res.error_code == ErrorCode.OK)

    try:
        db1.show_table(table_name)
    except InfinityException as e:
        print(e)

    try:
        db2.show_table(table_name)
    except InfinityException as e:
        print(e)

    time.sleep(1)
    cluster.remove_node("node2")
    cluster.remove_node("node1")
    cluster.clear()

def test_tc2(cluster: InfinityCluster):
    '''
    tc2:
    n1: admin
    n2: admin
    n3: admin
    n4: admin
    n1->leader
    create table t1 on n1;
    insert several rows data;
    n2->follower
    n3->learner
    n4->learner
    show nodes on n1 & n2 & n3 & n4
    '''
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")
    cluster.add_node("node3", "conf/learner.toml")
    cluster.add_node("node4", "conf/learner2.toml")
    cluster.set_admin("node1")
    cluster.set_admin("node2")
    cluster.set_admin("node3")
    cluster.set_admin("node4")

    cluster.set_leader("node1")
    infinity1 = cluster.client("node1")
    db1 = infinity1.get_database("default_db")
    table_name = "table1_tc2"
    db1.drop_table(table_name, ConflictType.Ignore)
    table1 = db1.create_table(
        table_name, {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}
    )
    for i in range(10):
        table1.insert([{"c1": i, "c2": [1.0, 2.0, 3.0, 4.0]}])

    infinity2 = cluster.client("node2")
    cluster.set_follower("node2")

    infinity3 = cluster.client("node3")
    cluster.set_learner("node3")

    infinity4 = cluster.client("node4")
    cluster.set_learner("node4")

    for server in [infinity1, infinity2, infinity3, infinity4]:
        time.sleep(1)
        res = server.show_node("node1")
        assert(res.node_name == "node1")
        assert(res.node_role == "leader")
        assert(res.node_status == "alive")
        res = server.show_node("node2")
        assert(res.node_name == "node2")
        assert(res.node_role == "follower")
        assert(res.node_status == "alive")
        res = server.show_node("node3")
        assert(res.node_name == "node3")
        assert(res.node_role == "learner")
        assert(res.node_status == "alive")
        res = server.show_node("node4")
        assert(res.node_name == "node4")
        assert(res.node_role == "learner")
        assert(res.node_status == "alive")        

    res = db1.drop_table(table_name, ConflictType.Ignore)
    assert(res.error_code == ErrorCode.OK)

    time.sleep(1)
    cluster.remove_node("node4")
    cluster.remove_node("node3")
    cluster.remove_node("node2")
    cluster.remove_node("node1")
    cluster.clear()

