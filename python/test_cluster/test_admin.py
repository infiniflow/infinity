import pytest
from infinity_cluster import InfinityCluster
from infinity.common import InfinityException


def test_admin(cluster: InfinityCluster):
    with cluster:
        logger = cluster.logger
        logger.info("test_admin")
        cluster.add_node("test", "conf/leader.toml")
        infinity1 = cluster.client("test")
        assert infinity1 is not None
        res = infinity1.show_current_node()
        logger.info(f'{res.node_role}, {res.node_status}')
        assert (res.node_role == "admin")
        assert (res.node_status == "starting")

        res = infinity1.show_admin_variables()
        logger.info(res.data)

        res = infinity1.show_admin_configs()
        logger.info(res.data)

        res = infinity1.show_admin_catalogs()
        logger.info(res.data)

        res = infinity1.show_admin_logs()
        logger.info(res.data)

        cluster.remove_node("test")


def test_admin_fail0(cluster: InfinityCluster):
    with cluster:
        logger = cluster.logger
        logger.info("test_admin_fail0")
        cluster.add_node("test", "conf/leader.toml")
        infinity1 = cluster.client("test")
        assert infinity1 is not None

        with pytest.raises(InfinityException) as e:
            infinity1.show_node('test')
        logger.info(e.value)

        with pytest.raises(InfinityException) as e:
            infinity1.list_nodes()
        logger.info(e.value)

        cluster.remove_node("test")

# show catalogs
# show wal
