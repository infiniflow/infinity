import time

from numpy import dtype
import pandas as pd
import pytest
from infinity_cluster import InfinityCluster
from infinity.common import ConflictType


class TestDatabase:
    def test1(self, cluster: InfinityCluster):
        try:
            cluster.add_node("node1", "conf/leader.toml")
            cluster.add_node("node2", "conf/follower.toml")

            print("init nodes")
            cluster.set_leader("node1")
            cluster.set_follower("node2")
            time.sleep(1)

            infinity1 = cluster.client("node1")
            infinity2 = cluster.client("node2")

            db_count = 100
            for i in range(db_count):
                print('drop test_cluster_db_name' + str(i))
                infinity1.drop_database('test_cluster_db_name' + str(i), ConflictType.Ignore)
            for i in range(db_count):
                print('create test_cluster_db_name' + str(i))
                infinity1.create_database('test_cluster_db_name' + str(i))

            time.sleep(1)
            dbs = infinity1.list_databases()
            res_dbs = []
            for db_name in dbs.db_names:
                print('db name: ' + db_name)
                if db_name.startswith("test_cluster_db_name") or db_name == "default_db":
                    res_dbs.append(db_name)
            assert len(res_dbs) == (db_count + 1)

            dbs = infinity2.list_databases()
            res_dbs = []
            for db_name in dbs.db_names:
                print('db name: ' + db_name)
                if db_name.startswith("test_cluster_db_name") or db_name == "default_db":
                    res_dbs.append(db_name)
            assert len(res_dbs) == (db_count + 1)

            for i in range(db_count):
                print('drop test_cluster_db_name' + str(i))
                infinity1.drop_database('test_cluster_db_name' + str(i), ConflictType.Ignore)

            time.sleep(1)
            dbs = infinity1.list_databases()
            res_dbs = []
            for db_name in dbs.db_names:
                print('db name: ' + db_name)
                if db_name.startswith("test_cluster_db_name") or db_name == "default_db":
                    res_dbs.append(db_name)
            assert len(res_dbs) == (1)

            dbs = infinity2.list_databases()
            res_dbs = []
            for db_name in dbs.db_names:
                print('db name: ' + db_name)
                if db_name.startswith("test_cluster_db_name") or db_name == "default_db":
                    res_dbs.append(db_name)
            assert len(res_dbs) == (1)
        except Exception as e:
            print(e)
            cluster.clear()
            raise
        else:
            cluster.clear()
