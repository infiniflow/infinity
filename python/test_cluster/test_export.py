import time
import os
from numpy import dtype
import pandas as pd
import pytest
from infinity_cluster import InfinityCluster
from infinity.common import ConflictType
from infinity.common import InfinityException
from infinity.errors import ErrorCode
import common_values

def count_lines(file_path: str):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    return len(lines)

def delete_file(file_path: str):
    if os.path.exists(file_path):
        os.remove(file_path)

class TestExport:
    def test_export_csv(self, cluster: InfinityCluster):
        try:
            cluster.add_node("node1", "conf/leader.toml")
            cluster.add_node("node2", "conf/follower.toml")

            print("init nodes")
            cluster.set_leader("node1")
            cluster.set_follower("node2")
            time.sleep(1)

            infinity1 = cluster.client("node1")
            infinity2 = cluster.client("node2")

            db_obj = infinity1.get_database("default_db")

            test_csv_dir = common_values.TEST_DATA_DIR + "csv/enwiki_embedding_9999.csv"
            print(f"import file: {test_csv_dir}")
            assert os.path.exists(test_csv_dir)
            if not os.path.exists(common_values.TEST_TMP_DIR):
                os.makedirs(common_values.TEST_TMP_DIR)

            db_obj.drop_table("test_export_csv", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_export_csv", {"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}, "num": {"type": "integer"}, "vec": {"type": "vector, 4, float"}})
            res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv", "delimiter" : "\t"})
            assert res.error_code == ErrorCode.OK
            test_export_csv_file_path = common_values.TEST_TMP_DIR + "leader_" +"test_export_csv.csv"
            res = table_obj.export_data(test_export_csv_file_path, {"file_type": "csv", "delimiter" : "\t"})
            assert res.error_code == ErrorCode.OK
            assert count_lines(test_export_csv_file_path) == 9999
            delete_file(test_export_csv_file_path)

            time.sleep(1)
            db_obj_2 = infinity2.get_database("default_db")
            table_obj_2 = db_obj_2.get_table("test_export_csv")
            test_export_csv_file_path = common_values.TEST_TMP_DIR + "follower_" +"test_export_csv.csv"
            res = table_obj_2.export_data(test_export_csv_file_path, {"file_type": "csv", "delimiter" : "\t"})
            assert res.error_code == ErrorCode.OK
            assert count_lines(test_export_csv_file_path) == 9999
            delete_file(test_export_csv_file_path)

            res = db_obj.drop_table("test_export_csv", ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        except Exception as e:
            print(e)
            cluster.clear()
            raise
        else:
            cluster.clear()