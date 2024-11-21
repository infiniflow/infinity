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
import logging


def test_admin(cluster: InfinityCluster):
    logger = logging.getLogger("run_parallel_test")
    logger.info("test_admin")
    cluster.add_node("test", "conf/leader.toml")
    infinity1 = cluster.client("test")
    assert infinity1 is not None
    res = infinity1.show_current_node()
    logger.info(f'{res.node_role}, {res.node_status}')
    assert (res.node_role == "admin")
    assert (res.node_status == "starting")
    cluster.remove_node("test")
    cluster.clear()


def test_admin_fail0(cluster: InfinityCluster):
    logger = logging.getLogger("run_parallel_test")
    logger.info("test_admin_fail0")
    cluster.add_node("test", "conf/leader.toml")
    infinity1 = cluster.client("test")
    assert infinity1 is not None
    with pytest.raises(InfinityException) as e:
        infinity1.show_node('test')
    logger.info(e.value)

    cluster.remove_node("test")
    cluster.clear()


def test_admin_fail2(cluster: InfinityCluster):
    logger = logging.getLogger("run_parallel_test")
    logger.info("test_admin_fail2")
    cluster.add_node("test", "conf/leader.toml")
    infinity1 = cluster.client("test")
    assert infinity1 is not None
    with pytest.raises(InfinityException) as e:
        infinity1.list_nodes()
    logger.info(e.value)

    cluster.remove_node("test")
    cluster.clear()
