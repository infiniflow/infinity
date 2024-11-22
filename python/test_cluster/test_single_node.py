from infinity_cluster import InfinityCluster
import time
from database_operations import do_some_operations

def test_admin2standalone(cluster: InfinityCluster):
    '''
    test: admin->(standalone, operations)->admin->(standalone, operations)
    '''
    with cluster:
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

def test_standalone2admin(cluster: InfinityCluster):
    '''
    test: (standalone, operations)->admin->(standalone, operations)->admin
    '''
    with cluster:
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

def test_admin2standalone2admin2leader(cluster: InfinityCluster):
    '''
    test: admin->(standalone, operations)->admin->(leader, operations)
    '''
    with cluster:
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

def test_standalone2admin2leader2admin(cluster: InfinityCluster):
    '''
    test: (standalone, operations)->admin->(leader, operations)->admin
    '''
    with cluster:
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
