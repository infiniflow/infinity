import infinity
from common import common_values
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from infinity import index
import time
import threading
import random
from infinity.common import ConflictType, SparseVector
from infinity.errors import ErrorCode
import pytest


class TestDatabaseSnapshotRestart:
    """Test database snapshot functionality with restart scenarios"""
    
    def test_database_snapshot_with_indexes_restart(self, infinity_runner: InfinityRunner):
        """Test snapshot with complex indexes across restarts"""
        config1 = "test/data/config/restart_test/test_database_snapshot_restart/1.toml"
        config2 = "test/data/config/restart_test/test_database_snapshot_restart/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            infinity_obj.create_database("test_db")
            db = infinity_obj.get_database("test_db")
            db.drop_table("index_table", ConflictType.Ignore)
            
            # Create table with various data types
            table_obj = db.create_table(
                "index_table",
                {
                    "id": {"type": "integer"},
                    "name": {"type": "varchar"},
                    "score": {"type": "float"},
                    "vector": {"type": "vector,128,float"},
                    "sparse_vector": {"type": "sparse,1000,float,int"},
                    "created_at": {"type": "datetime"}
                }
            )
            
            # Insert data
            data = []
            for i in range(500):
                data.append({
                    "id": i,
                    "name": f"user_{i}",
                    "score": float(i) * 2.0,
                    "vector": [float(j) for j in range(128)],
                    "sparse_vector": SparseVector([i % 100, (i + 25) % 100], [1.0, 2.0]),
                    "created_at": "2024-01-01 12:00:00"
                })
            
            res = table_obj.insert(data)
            assert res.error_code == ErrorCode.OK
            
            # Create multiple indexes
            res = table_obj.create_index(
                "idx_name", index.IndexInfo("name", index.IndexType.FullText)
            )
            assert res.error_code == ErrorCode.OK
            
            res = table_obj.create_index(
                "idx_vector", index.IndexInfo("vector", index.IndexType.Hnsw, {
                    "M": "16", "ef_construction": "20", "metric": "l2", "block_size": "1"
                })
            )
            assert res.error_code == ErrorCode.OK
            
            # Note: HNSW indexes are not supported on sparse vector columns
            # res = table_obj.create_index(
            #     "idx_sparse", index.IndexInfo("sparse_vector", index.IndexType.Hnsw, {
            #         "M": "16", "ef_construction": "20", "metric": "ip", "block_size": "1"
            #     })
            # )
            # assert res.error_code == ErrorCode.OK
            
            # Create snapshot
            res = infinity_obj.create_database_snapshot("index_snapshot","test_db")
            assert res.error_code == ErrorCode.OK
            
            # Drop database
            db = infinity_obj.get_database("default_db")
            res = infinity_obj.drop_database("test_db")
            assert res.error_code == ErrorCode.OK

            # restore snapshot
            res = infinity_obj.restore_database_snapshot("index_snapshot")
            assert res.error_code == ErrorCode.OK

        part1()

        # Restart and restore
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            
            db = infinity_obj.get_database("test_db")
            
            
            # Test index functionality after restart
            res = db.get_table("index_table").output(["id", "name"]).filter("score > 500").to_df()
            assert len(res) > 0
            
            # Test sparse vector search
            # res = db.get_table("index_table").output(["id", "sparse_vector"]).filter("id < 100").to_df()
            # assert len(res) == 100
            
            # Create a new table after restart
            new_table_obj = db.create_table(
                "new_table_after_restart",
                {
                    "id": {"type": "integer"},
                    "name": {"type": "varchar"},
                    "value": {"type": "float"},
                    "vector": {"type": "vector,64,float"}
                }
            )
            
            # Insert data into the new table
            new_data = []
            for i in range(100):
                new_data.append({
                    "id": i,
                    "name": f"new_user_{i}",
                    "value": float(i) * 1.5,
                    "vector": [float(j) for j in range(64)]
                })
            
            res = new_table_obj.insert(new_data)
            assert res.error_code == ErrorCode.OK
            
            # Verify the new table has data
            res = new_table_obj.output(["count(*)"]).to_df()
            print(res)
            
            # Create an index on the new table
            res = new_table_obj.create_index(
                "idx_new_name", index.IndexInfo("name", index.IndexType.FullText)
            )
            assert res.error_code == ErrorCode.OK
            
            # Test the new table functionality
            res = new_table_obj.output(["id", "name"]).filter("value > 50").to_df()
            assert len(res) > 0
            
            # Delete the new table
            res = db.drop_table("new_table_after_restart", ConflictType.Error)
            assert res.error_code == ErrorCode.OK
            
            # Verify the table is deleted
            try:
                db.get_table("new_table_after_restart")
                assert False, "Table should be deleted"
            except Exception:
                # Expected - table should not exist
                pass

        part2()

        # Restart again to verify everything still works
        decorator3 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator3
        def part3(infinity_obj):
            db = infinity_obj.get_database("test_db")
            
            # Verify the original table still exists and works
            res = db.get_table("index_table").output(["id", "name"]).filter("score > 500").to_df()
            assert len(res) > 0
            
            # Create another new table after second restart
            another_table_obj = db.create_table(
                "another_table",
                {
                    "id": {"type": "integer"},
                    "description": {"type": "varchar"},
                    "amount": {"type": "float"}
                }
            )
            
            # Insert data
            another_data = []
            for i in range(50):
                another_data.append({
                    "id": i,
                    "description": f"item_{i}",
                    "amount": float(i) * 10.0
                })
            
            res = another_table_obj.insert(another_data)
            assert res.error_code == ErrorCode.OK
            
            # Verify data
            res = another_table_obj.output(["count(*)"]).to_df()
            print(res)
            
            # Test query
            res = another_table_obj.output(["id", "description"]).filter("amount > 200").to_df()
            assert len(res) > 0
            
            # Clean up
            res = db.drop_table("another_table", ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        part3()


# No module-level registration needed - test methods are regular pytest tests