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
from database_operations import do_some_operations

def test_admin2standalone(cluster: InfinityCluster):
    '''
    test: admin->(standalone, operations)->admin->(standalone, operations)
    '''
    cluster.add_node("test", "conf/leader.toml")
    test_client = cluster.client("test")
    assert test_client.show_current_node().node_role == "admin"

    cluster.set_standalone("test")
    assert test_client.show_current_node().node_role == "standalone"
    do_some_operations(test_client)

    cluster.set_admin("test") 
    assert test_client.show_current_node().node_role == "admin"

    cluster.set_standalone("test")
    assert test_client.show_current_node().node_role == "standalone"
    do_some_operations(test_client)

    cluster.remove_node("test")
    cluster.clear()

def test_standalone2admin(cluster: InfinityCluster):
    '''
    test: (standalone, operations)->admin->(standalone, operations)->admin
    '''
    cluster.add_node("test", "conf/infinity_conf.toml")
    test_client = cluster.client("test")
    assert test_client.show_current_node().node_role == "standalone"
    do_some_operations(test_client)

    cluster.set_admin("test")
    assert test_client.show_current_node().node_role == "admin"

    cluster.set_standalone("test")
    assert test_client.show_current_node().node_role == "standalone"
    do_some_operations(test_client)

    cluster.set_admin("test")
    assert test_client.show_current_node().node_role == "admin"

    cluster.remove_node("test")
    cluster.clear()

def test_admin2standalone2admin2leader(cluster: InfinityCluster):
    '''
    test: admin->(standalone, operations)->admin->(standalone, operations)
    '''
    cluster.add_node("test", "conf/leader.toml")
    test_client = cluster.client("test")
    assert test_client.show_current_node().node_role == "admin"

    cluster.set_standalone("test")
    assert test_client.show_current_node().node_role == "standalone"
    do_some_operations(test_client)

    cluster.set_admin("test") 
    assert test_client.show_current_node().node_role == "admin"

    cluster.set_leader("test")
    assert test_client.show_node("test").node_role == "leader"
    assert test_client.show_node("test").node_name == "test"
    assert test_client.show_node("test").node_status == "alive"
    do_some_operations(test_client)

    cluster.remove_node("test")
    cluster.clear()

def test_standalone2admin2leader2admin(cluster: InfinityCluster):
    '''
    test: (standalone, operations)->admin->(leader, operations)->admin
    '''
    cluster.add_node("test", "conf/infinity_conf.toml")
    test_client = cluster.client("test")
    assert test_client.show_current_node().node_role == "standalone"
    do_some_operations(test_client)

    cluster.set_admin("test")
    assert test_client.show_current_node().node_role == "admin"

    cluster.set_leader("test")
    assert test_client.show_node("test").node_role == "leader"
    assert test_client.show_node("test").node_name == "test"
    assert test_client.show_node("test").node_status == "alive"
    do_some_operations(test_client)

    cluster.set_admin("test")
    assert test_client.show_current_node().node_role == "admin"

    cluster.remove_node("test")
    cluster.clear()
