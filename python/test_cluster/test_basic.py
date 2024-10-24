import time
from infinity_cluster import InfinityCluster, MockedInfinityRunner


def test_0(infinity_path: str):
    cluster = InfinityCluster(infinity_path)
    cluster.add_node("node1", "conf/leader.toml")
    cluster.add_node("node2", "conf/follower.toml")

    cluster.init_leader("node1")
    cluster.init_follower("node2")

    time.sleep(1)

# def test_mock(infinity_path: str):
#     cluster = MockedInfinityRunner(infinity_path)
#     cluster.add_node("node1", "conf/leader.toml")
#     cluster.add_node("node2", "conf/follower.toml")

#     cluster.init_leader("node1")
#     cluster.init_follower("node2")

#     time.sleep(1)
