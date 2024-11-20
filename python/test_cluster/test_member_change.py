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
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.set_leader("node1")
    cluster.set_follower("node2")
    node1_client = cluster.client("node1")
    node2_client = cluster.client("node2")

    res = node1_client.show_node("node1");
    assert (res.node_name == "node1")
    assert (res.node_role == "leader")
    assert (res.node_status == "alive")

    res = node1_client.show_node("node2");
    assert (res.node_name == "node2")
    assert (res.node_role == "follower")
    assert (res.node_status == "alive")

    do_some_operations_cluster(node1_client, [node2_client])

    cluster.set_admin("node1")
    cluster.set_leader("node2")
    cluster.set_follower("node1");

    res = node1_client.show_node("node1");
    assert (res.node_name == "node1")
    assert (res.node_role == "follower")
    assert (res.node_status == "alive")

    res = node1_client.show_node("node2");
    assert (res.node_name == "node2")
    assert (res.node_role == "leader")
    assert (res.node_status == "alive")

    do_some_operations_cluster(node2_client, [node1_client])

    cluster.remove_node("node1")
    cluster.remove_node("node2")
    cluster.clear()
