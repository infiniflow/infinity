import time

from numpy import dtype
import pandas as pd
import pytest
from infinity_cluster import InfinityCluster
from infinity.common import ConflictType
from infinity.common import InfinityException


class TestTable:
    def test_create_100_table(self, cluster: InfinityCluster):
        try:
            cluster.add_node("node1", "conf/leader.toml")
            cluster.add_node("node2", "conf/follower.toml")

            print("init nodes")
            cluster.set_leader("node1")
            cluster.set_follower("node2")
            time.sleep(1)

            infinity1 = cluster.client("node1")
            infinity2 = cluster.client("node2")

            db1 = infinity1.get_database("default_db")
            table_count = 100
            for i in range(table_count):
                print('drop test_cluster_table_name' + str(i))
                db1.drop_table('test_cluster_table_name' + str(i), ConflictType.Ignore)
            for i in range(table_count):
                print('create test_cluster_table_name' + str(i))
                db1.create_table('test_cluster_table_name' + str(i), {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)

            time.sleep(1)
            tables = db1.get_all_tables()
            res_tables = []
            for table_name in tables:
                if table_name.startswith("test_cluster_table_name") :
                    res_tables.append(table_name)
            assert len(res_tables) == (table_count)

            db2 = infinity2.get_database("default_db")
            tables = db2.get_all_tables()
            res_tables = []
            for table_name in tables:
                if table_name.startswith("test_cluster_table_name") :
                    res_tables.append(table_name)
            assert len(res_tables) == (table_count)

            for i in range(table_count):
                print('drop test_cluster_table_name' + str(i))
                db1.drop_table('test_cluster_table_name' + str(i), ConflictType.Ignore)

            time.sleep(1)
            tables = db1.get_all_tables()
            res_tables = []
            for table_name in tables:
                if table_name.startswith("test_cluster_table_name") :
                    res_tables.append(table_name)
            assert len(res_tables) == 0

            db2 = infinity2.get_database("default_db")
            tables = db2.get_all_tables()
            res_tables = []
            for table_name in tables:
                if table_name.startswith("test_cluster_table_name") :
                    res_tables.append(table_name)
            assert len(res_tables) == 0
        except Exception as e:
            print(e)
            cluster.clear()
            raise
        else:
            cluster.clear()

    def test_create_table_on_follower(self, cluster: InfinityCluster):
        try:
            cluster.add_node("node1", "conf/leader.toml")
            cluster.add_node("node2", "conf/follower.toml")

            print("init nodes")
            cluster.set_leader("node1")
            cluster.set_follower("node2")
            time.sleep(1)

            infinity1 = cluster.client("node1")
            infinity2 = cluster.client("node2")

            db = infinity2.get_database("default_db")
            try:
                db.create_table('test_cluster_follwer_table', {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
            except InfinityException as e:
                print(e)
                assert(e.error_code == 8007)

        except Exception as e:
            print(e)
            cluster.clear()
            raise
        else:
            cluster.clear()