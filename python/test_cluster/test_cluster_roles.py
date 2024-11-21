import time

import pytest
from infinity_cluster import InfinityCluster
from numpy import dtype
import pandas as pd
import time
from infinity.errors import ErrorCode
from infinity.common import InfinityException
from infinity.common import ConflictType
from database_operations import do_some_operations, do_some_operations_cluster
from infinity_http import database_result
import logging

def test_leader(cluster : InfinityCluster):
    '''
    node1 leader
    node2 follower

    node2 try disconnect and then connect
    check the presence of node2
    '''
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.set_leader("node1")
    cluster.set_follower("node2")
    node1_client = cluster.client("node1")
    node2_client = cluster.client("node2")

    res = node1_client.show_node("node1")
    expected = database_result(node_name="node1", node_role="leader", node_status="alive")
    assert res == expected

    res = node1_client.show_node("node2")
    expected = database_result(node_name="node2", node_role="follower", node_status="alive")
    assert res == expected

    node1_client.remove_node("node2")
    with pytest.raises(InfinityException) as e:
        node1_client.show_node("node2")

    leader_ip, leader_port = cluster.leader_addr()
    node2_client.set_role_follower("node2", f"{leader_ip}:{leader_port}")

    res = node2_client.show_node("node1")
    expected = database_result(node_name="node1", node_role="leader", node_status="alive")
    assert res == expected

    res = node2_client.show_node("node2")
    expected = database_result(node_name="node2", node_role="follower", node_status="alive")
    assert res == expected

    cluster.clear()

def test_leader_failed(cluster : InfinityCluster):
    '''
    leader can't do:
    remove it self from the cluster
    switch directly to follower or learner
    '''
    cluster.add_node("node1", "conf/leader.toml")
    cluster.set_leader("node1")
    node1_client = cluster.client("node1")

    with pytest.raises(InfinityException) as e:
        node1_client.remove_node("node1");

    with pytest.raises(InfinityException) as e:
        cluster.set_follower("node1")

    cluster.clear()

def test_followerlearner(cluster : InfinityCluster):
    '''
    a follower/learner can do:
    connect to a leader
    check all node in the cluster
    '''
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")
    cluster.set_leader("node1")
    cluster.set_follower("node2")
    node1_client = cluster.client("node1")
    node2_client = cluster.client("node2") 

    res1 = node1_client.show_nodes()
    res2 = node2_client.show_nodes()
    assert res1 == res2

    cluster.clear()

def test_followerlearner_failed(cluster : InfinityCluster):
    '''
    a follower/learner can't do:
    try to switch to follower/learner connecting to a non-leader node
    remove node
    tries to write data
    '''
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")
    node1_client = cluster.client("node1")
    node2_client = cluster.client("node2")

    with pytest.raises(ValueError) as e:
        cluster.set_follower("node2")

    cluster.set_leader("node1")
    cluster.set_follower("node2")

    with pytest.raises(InfinityException) as e:
        node2_client.remove_node("node1")

    with pytest.raises(InfinityException) as e:
        do_some_operations_cluster(node2_client, [node1_client])

    cluster.clear()
