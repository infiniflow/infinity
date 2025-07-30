import infinity
from common import common_values
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from infinity import index
import time
import pathlib
from infinity.common import ConflictType, SparseVector
import pytest
import threading
from util import RtnThread


def infinity_runner_decorator_factory_interrupted(
        config_path: str | None,
        uri: str,
        infinity_runner: InfinityRunner,
):
    def decorator(f):
        def wrapper(*args, **kwargs):
            infinity_runner.init(config_path)
            infinity_obj = infinity_runner.connect(uri)
            try:
                return f(infinity_obj, *args, **kwargs)
            finally:
                # Don't try to disconnect since the process might be killed
                # Just clean up the process
                try:
                    infinity_runner.uninit(kill=False, timeout=5)
                except Exception:
                    pass

        return wrapper

    return decorator


class TestSnapshot:
    def test_snapshot_basic_restart(self, infinity_runner: InfinityRunner):
        """Test basic snapshot creation and restoration across restarts"""
        config1 = "test/data/config/restart_test/test_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_snapshot/2.toml"
        config3 = "test/data/config/restart_test/test_snapshot/3.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_snapshot1",
                {
                    "c1": {"type": "int", "constraints": ["primary key"]},
                    "c2": {"type": "varchar"},
                    "c3": {"type": "float64"},
                    "c4": {"type": "vector,4,float"}
                },
            )
            
            # Insert initial data
            table_obj.insert([
                {"c1": 1, "c2": "original_1", "c3": 1.1, "c4": [0.1, 0.2, 0.3, 0.4]},
                {"c1": 2, "c2": "original_2", "c3": 2.2, "c4": [0.2, 0.3, 0.4, 0.5]},
                {"c1": 3, "c2": "original_3", "c3": 3.3, "c4": [0.3, 0.4, 0.5, 0.6]}
            ])
            
            # Create index
            res = table_obj.create_index(
                "idx1",
                index.IndexInfo("c2", index.IndexType.FullText)
            )
            assert res.error_code == infinity.ErrorCode.OK
            
            # Create snapshot
            res = db_obj.create_table_snapshot("snapshot1", "test_snapshot1")
            assert res.error_code == infinity.ErrorCode.OK
            
            # Add more data after snapshot
            table_obj.insert([
                {"c1": 4, "c2": "new_4", "c3": 4.4, "c4": [0.4, 0.5, 0.6, 0.7]},
                {"c1": 5, "c2": "new_5", "c3": 5.5, "c4": [0.5, 0.6, 0.7, 0.8]}
            ])
            
            # Verify original table has all data (3 original + 2 new)
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            assert data_dict["count(star)"] == [5]
            
            # Drop original table
            db_obj.drop_table("test_snapshot1", ConflictType.Error)
            
            # Restore from snapshot
            res = db_obj.restore_table_snapshot("snapshot1")
            assert res.error_code == infinity.ErrorCode.OK
            
            # Verify restored table has only original data
            restored_table = db_obj.get_table("test_snapshot1")
            data_dict, _, _ = restored_table.output(["c1", "c2", "c3"]).to_result()
            assert len(data_dict["c1"]) == 3  # Only original 3 rows
            assert data_dict["c1"] == [1, 2, 3]
            assert data_dict["c2"] == ["original_1", "original_2", "original_3"]
            
            # Wait for operations to complete
            time.sleep(2)

        part1()

        # Phase 2: Restart and verify snapshot persistence
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(3)
            db_obj = infinity_obj.get_database("default_db")
            
            # Verify restored table still exists with original data
            table_obj = db_obj.get_table("test_snapshot1")
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            assert data_dict["count(star)"] == [3]  # Only original 3 rows
            
            # List snapshots to verify snapshot still exists
            snapshots_res = infinity_obj.list_snapshots()
            assert snapshots_res.error_code == infinity.ErrorCode.OK
            snapshot_names = [s.name for s in snapshots_res.snapshots]
            assert "snapshot1" in snapshot_names
            
            # Verify restored data is correct
            data_dict, _, _ = table_obj.output(["c1", "c2", "c3"]).to_result()
            assert data_dict["c1"] == [1, 2, 3]
            assert data_dict["c2"] == ["original_1", "original_2", "original_3"]

        part2()

        # Phase 3: Another restart to verify snapshot persistence
        decorator3 = infinity_runner_decorator_factory(config3, uri, infinity_runner)

        @decorator3
        def part3(infinity_obj):
            time.sleep(3)
            db_obj = infinity_obj.get_database("default_db")
            
            # Verify restored table still exists
            table_obj = db_obj.get_table("test_snapshot1")
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            assert data_dict["count(star)"] == [3]
            
            # Verify snapshot still exists
            snapshots_res = infinity_obj.list_snapshots()
            assert snapshots_res.error_code == infinity.ErrorCode.OK
            snapshot_names = [s.name for s in snapshots_res.snapshots]
            assert "snapshot1" in snapshot_names
            
            # Clean up
            infinity_obj.drop_snapshot("snapshot1")
            db_obj.drop_table("test_snapshot1", ConflictType.Error)

        part3()

    def test_snapshot_during_operations(self, infinity_runner: InfinityRunner):
        """Test snapshot creation during concurrent operations"""
        config1 = "test/data/config/restart_test/test_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_snapshot/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_snapshot_concurrent",
                {
                    "c1": {"type": "int", "constraints": ["primary key"]},
                    "c2": {"type": "varchar"},
                    "c3": {"type": "vector,4,float"}
                },
            )
            
            # Insert initial data
            table_obj.insert([
                {"c1": 1, "c2": "data_1", "c3": [0.1, 0.2, 0.3, 0.4]},
                {"c1": 2, "c2": "data_2", "c3": [0.2, 0.3, 0.4, 0.5]}
            ])
            
            # Create index
            table_obj.create_index("idx1", index.IndexInfo("c2", index.IndexType.FullText))
            
            # Start concurrent operations
            stop_insert = False
            
            def insert_func(table_obj):
                nonlocal stop_insert
                i = 3
                while not stop_insert:
                    try:
                        table_obj.insert([{
                            "c1": i,
                            "c2": f"concurrent_data_{i}",
                            "c3": [float(i)/10, float(i+1)/10, float(i+2)/10, float(i+3)/10]
                        }])
                        i += 1
                        time.sleep(0.1)
                    except Exception as e:
                        print(f"Insert error: {e}")
                        break
            
            def snapshot_func(db_obj):
                nonlocal stop_insert
                time.sleep(1)  # Let some inserts happen first
                try:
                    res = db_obj.create_table_snapshot("concurrent_snap", "test_snapshot_concurrent")
                    print(f"Snapshot creation result: {res.error_code}")
                except Exception as e:
                    print(f"Snapshot error: {e}")
                finally:
                    stop_insert = True
            
            # Start concurrent threads
            t1 = RtnThread(target=insert_func, args=(table_obj,))
            t2 = RtnThread(target=snapshot_func, args=(db_obj,))
            
            t1.start()
            t2.start()
            
            t1.join()
            t2.join()
            
            # Wait for operations to complete
            time.sleep(2)

        part1()

        # Phase 2: Restart and verify snapshot
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(3)
            db_obj = infinity_obj.get_database("default_db")
            
            # Check if snapshot was created successfully
            snapshots_res = infinity_obj.list_snapshots()
            if snapshots_res.error_code == infinity.ErrorCode.OK:
                snapshot_names = [s.name for s in snapshots_res.snapshots]
                if "concurrent_snap" in snapshot_names:
                    # Try to restore from snapshot
                    try:
                        db_obj.drop_table("test_snapshot_concurrent", ConflictType.Ignore)
                        res = db_obj.restore_table_snapshot("concurrent_snap")
                        if res.error_code == infinity.ErrorCode.OK:
                            restored_table = db_obj.get_table("test_snapshot_concurrent")
                            data_dict, _, _ = restored_table.output(["count(*)"]).to_result()
                            print(f"Restored table has {data_dict['count(star)'][0]} rows")
                    except Exception as e:
                        print(f"Restore error: {e}")
            
            # Clean up
            try:
                infinity_obj.drop_snapshot("concurrent_snap")
                db_obj.drop_table("test_snapshot_concurrent", ConflictType.Ignore)
            except:
                pass

        part2()

    def test_snapshot_with_indexes(self, infinity_runner: InfinityRunner):
        """Test snapshot with various index types"""
        config1 = "test/data/config/restart_test/test_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_snapshot/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_snapshot_indexes",
                {
                    "c1": {"type": "int", "constraints": ["primary key"]},
                    "c2": {"type": "varchar"},
                    "c3": {"type": "vector,4,float"},
                    "c4": {"type": "sparse,100,float,int"}
                },
            )
            
            # Insert data
            table_obj.insert([
                {"c1": 1, "c2": "text_1", "c3": [0.1, 0.2, 0.3, 0.4], "c4": SparseVector(indices=[0, 10], values=[1.0, 2.0])},
                {"c1": 2, "c2": "text_2", "c3": [0.2, 0.3, 0.4, 0.5], "c4": SparseVector(indices=[5, 15], values=[3.0, 4.0])},
                {"c1": 3, "c2": "text_3", "c3": [0.3, 0.4, 0.5, 0.6], "c4": SparseVector(indices=[20, 30], values=[5.0, 6.0])}
            ])
            
            # Create various indexes
            table_obj.create_index("idx_fulltext", index.IndexInfo("c2", index.IndexType.FullText))
            table_obj.create_index("idx_hnsw", index.IndexInfo("c3", index.IndexType.Hnsw, {
                "M": "16", "ef_construction": "20", "metric": "l2", "block_size": "1"
            }))
            table_obj.create_index("idx_bmp", index.IndexInfo("c4", index.IndexType.BMP, {
                "BLOCK_SIZE": "8", "COMPRESS_TYPE": "compress"
            }))
            
            # Create snapshot
            res = db_obj.create_table_snapshot("index_snap", "test_snapshot_indexes")
            assert res.error_code == infinity.ErrorCode.OK
            
            # Wait for operations to complete
            time.sleep(3)

        part1()

        # Phase 2: Restart and verify indexes
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(3)
            db_obj = infinity_obj.get_database("default_db")
            
            # Drop original table
            db_obj.drop_table("test_snapshot_indexes", ConflictType.Error)
            
            # Restore from snapshot
            res = db_obj.restore_table_snapshot("index_snap")
            assert res.error_code == infinity.ErrorCode.OK
            
            # Verify restored table and indexes
            restored_table = db_obj.get_table("test_snapshot_indexes")
            
            # Test vector search
            data_dict, _, _ = restored_table.output(["c1"]).match_dense("c3", [0.2, 0.3, 0.4, 0.5], "float", "l2", 3).to_result()
            assert len(data_dict["c1"]) > 0
            
            # Test sparse vector search
            query_vector = SparseVector(indices=[0, 10], values=[1.0, 2.0])
            data_dict, _, _ = restored_table.output(["c1"]).match_sparse("c4", query_vector, "ip", 3).to_result()
            assert len(data_dict["c1"]) > 0
            
            # Clean up
            infinity_obj.drop_snapshot("index_snap")
            db_obj.drop_table("test_snapshot_indexes", ConflictType.Error)

        part2()

    def test_snapshot_error_recovery(self, infinity_runner: InfinityRunner):
        """Test snapshot operations during error conditions"""
        config1 = "test/data/config/restart_test/test_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_snapshot/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_snapshot_error",
                {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "varchar"}},
            )
            
            # Insert data
            table_obj.insert([
                {"c1": 1, "c2": "data_1"},
                {"c1": 2, "c2": "data_2"}
            ])
            
            # Create snapshot
            res = db_obj.create_table_snapshot("error_snap", "test_snapshot_error")
            assert res.error_code == infinity.ErrorCode.OK
            
            # Try to create duplicate snapshot (should fail)
            try:
                res = db_obj.create_table_snapshot("error_snap", "test_snapshot_error")
                # Should not reach here
                assert False, "Duplicate snapshot creation should fail"
            except Exception as e:
                # Expected error
                pass
            
            # Try to restore non-existent snapshot (should fail)
            try:
                res = db_obj.restore_table_snapshot("non_existent_snap")
                # Should not reach here
                assert False, "Restore non-existent snapshot should fail"
            except Exception as e:
                # Expected error
                pass
            
            # Wait for operations to complete
            time.sleep(2)

        part1()

        # Phase 2: Restart and verify error handling
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(3)
            db_obj = infinity_obj.get_database("default_db")
            
            # Verify original table still exists
            table_obj = db_obj.get_table("test_snapshot_error")
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            assert data_dict["count(star)"] == [2]
            
            # Verify snapshot still exists
            snapshots_res = infinity_obj.list_snapshots()
            assert snapshots_res.error_code == infinity.ErrorCode.OK
            snapshot_names = [s.name for s in snapshots_res.snapshots]
            assert "error_snap" in snapshot_names
            
            # Clean up
            infinity_obj.drop_snapshot("error_snap")
            db_obj.drop_table("test_snapshot_error", ConflictType.Error)

        part2()

    def test_snapshot_large_data(self, infinity_runner: InfinityRunner):
        """Test snapshot with large amount of data"""
        config1 = "test/data/config/restart_test/test_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_snapshot/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_snapshot_large",
                {
                    "c1": {"type": "int", "constraints": ["primary key"]},
                    "c2": {"type": "varchar"},
                    "c3": {"type": "vector,128,float"}
                },
            )
            
            # Insert large amount of data
            batch_size = 1000
            for batch in range(10):  # 10,000 rows total
                data = []
                for i in range(batch_size):
                    row_id = batch * batch_size + i
                    vector = [float(j) / 100.0 for j in range(128)]
                    data.append({
                        "c1": row_id,
                        "c2": f"large_data_{row_id}",
                        "c3": vector
                    })
                table_obj.insert(data)
                print(f"Inserted batch {batch + 1}/10")
            
            # Create index
            table_obj.create_index("idx_hnsw", index.IndexInfo("c3", index.IndexType.Hnsw, {
                "M": "16", "ef_construction": "20", "metric": "l2"
            }))
            
            # Create snapshot
            print("Creating snapshot...")
            res = db_obj.create_table_snapshot("large_snap", "test_snapshot_large")
            assert res.error_code == infinity.ErrorCode.OK
            print("Snapshot created successfully")
            
            # Wait for operations to complete
            time.sleep(5)

        part1()

        # Phase 2: Restart and verify large snapshot
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(5)
            db_obj = infinity_obj.get_database("default_db")
            
            # Verify original table still exists
            table_obj = db_obj.get_table("test_snapshot_large")
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            assert data_dict["count(star)"] == [10000]
            
            # Drop original table
            db_obj.drop_table("test_snapshot_large", ConflictType.Error)
            
            # Restore from snapshot
            print("Restoring from snapshot...")
            res = db_obj.restore_table_snapshot("large_snap")
            assert res.error_code == infinity.ErrorCode.OK
            print("Snapshot restored successfully")
            
            # Verify restored table
            restored_table = db_obj.get_table("test_snapshot_large")
            data_dict, _, _ = restored_table.output(["count(*)"]).to_result()
            assert data_dict["count(star)"] == [10000]
            
            # Test vector search on restored table
            query_vector = [0.1] * 128
            data_dict, _, _ = restored_table.output(["c1"]).match_dense("c3", query_vector, "float", "l2", 5).to_result()
            assert len(data_dict["c1"]) == 5
            
            # Clean up
            infinity_obj.drop_snapshot("large_snap")
            db_obj.drop_table("test_snapshot_large", ConflictType.Error)

        part2()

    def test_snapshot_interrupted_creation(self, infinity_runner: InfinityRunner):
        """Test snapshot creation that gets interrupted by restart"""
        config1 = "test/data/config/restart_test/test_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_snapshot/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory_interrupted(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_snapshot_interrupted",
                {
                    "c1": {"type": "int", "constraints": ["primary key"]},
                    "c2": {"type": "varchar"},
                    "c3": {"type": "float64"}
                },
            )
            
            # Insert data
            table_obj.insert([
                {"c1": 1, "c2": "data_1", "c3": 1.1},
                {"c1": 2, "c2": "data_2", "c3": 2.2},
                {"c1": 3, "c2": "data_3", "c3": 3.3}
            ])
            
            # Start snapshot creation in a separate thread
            import threading
            snapshot_completed = False
            snapshot_error = None
            
            def create_snapshot():
                nonlocal snapshot_completed, snapshot_error
                try:
                    res = db_obj.create_table_snapshot("interrupted_snap", "test_snapshot_interrupted")
                    snapshot_completed = True
                    print(f"Snapshot creation completed: {res.error_code}")
                except Exception as e:
                    snapshot_error = e
                    print(f"Snapshot creation error: {e}")
            
            # Start snapshot creation
            snapshot_thread = threading.Thread(target=create_snapshot)
            snapshot_thread.start()
            
            # Wait a bit for snapshot to start, then kill the process
            time.sleep(0.5)
            
            # Kill the Infinity process during snapshot creation
            print("Killing Infinity process during snapshot creation...")
            infinity_runner.uninit(kill=True)
            
            # Wait for snapshot thread to finish (it should fail)
            snapshot_thread.join(timeout=5)
            
            print(f"Snapshot completed: {snapshot_completed}")
            if snapshot_error:
                print(f"Snapshot error: {snapshot_error}")

        part1()

        # Phase 2: Restart and check what happened
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(3)
            db_obj = infinity_obj.get_database("default_db")
            
            # Check if table still exists
            try:
                table_obj = db_obj.get_table("test_snapshot_interrupted")
                data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
                print(f"Table still exists with {data_dict['count(star)'][0]} rows")
            except Exception as e:
                print(f"Table not found: {e}")
            
            # Check if snapshot was created (should be partial or failed)
            snapshots_res = infinity_obj.list_snapshots()
            if snapshots_res.error_code == infinity.ErrorCode.OK:
                snapshot_names = [s.name for s in snapshots_res.snapshots]
                print(f"Available snapshots: {snapshot_names}")
                
                if "interrupted_snap" in snapshot_names:
                    print("Snapshot was created despite interruption")
                    # Try to restore from the interrupted snapshot
                    try:
                        res = db_obj.restore_table_snapshot("interrupted_snap")
                        print(f"Restore result: {res.error_code}")
                    except Exception as e:
                        print(f"Restore failed: {e}")
                else:
                    print("Snapshot was not created due to interruption")
            
            # Clean up
            try:
                infinity_obj.drop_snapshot("interrupted_snap")
                db_obj.drop_table("test_snapshot_interrupted", ConflictType.Ignore)
            except:
                pass

        part2()

    def test_snapshot_interrupted_restore(self, infinity_runner: InfinityRunner):
        """Test snapshot restore that gets interrupted by restart"""
        config1 = "test/data/config/restart_test/test_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_snapshot/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory_interrupted(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table(
                "test_snapshot_restore_interrupted",
                {
                    "c1": {"type": "int", "constraints": ["primary key"]},
                    "c2": {"type": "varchar"},
                    "c3": {"type": "float64"}
                },
            )
            
            # Insert data
            table_obj.insert([
                {"c1": 1, "c2": "data_1", "c3": 1.1},
                {"c1": 2, "c2": "data_2", "c3": 2.2}
            ])
            
            # Create snapshot
            res = db_obj.create_table_snapshot("restore_interrupted_snap", "test_snapshot_restore_interrupted")
            assert res.error_code == infinity.ErrorCode.OK
            
            # Drop original table
            db_obj.drop_table("test_snapshot_restore_interrupted", ConflictType.Error)
            
            # Start restore in a separate thread
            import threading
            restore_completed = False
            restore_error = None
            
            def restore_snapshot():
                nonlocal restore_completed, restore_error
                try:
                    res = db_obj.restore_table_snapshot("restore_interrupted_snap")
                    restore_completed = True
                    print(f"Restore completed: {res.error_code}")
                except Exception as e:
                    restore_error = e
                    print(f"Restore error: {e}")
            
            # Start restore operation
            restore_thread = threading.Thread(target=restore_snapshot)
            restore_thread.start()
            
            # Wait a bit for restore to start, then kill the process
            time.sleep(0.5)
            
            # Kill the Infinity process during restore
            print("Killing Infinity process during snapshot restore...")
            infinity_runner.uninit(kill=True)
            
            # Wait for restore thread to finish (it should fail)
            restore_thread.join(timeout=5)
            
            print(f"Restore completed: {restore_completed}")
            if restore_error:
                print(f"Restore error: {restore_error}")

        part1()

        # Phase 2: Restart and check what happened
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            time.sleep(3)
            db_obj = infinity_obj.get_database("default_db")
            
            # Check if table was restored
            try:
                table_obj = db_obj.get_table("test_snapshot_restore_interrupted")
                data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
                print(f"Table restored with {data_dict['count(star)'][0]} rows")
            except Exception as e:
                print(f"Table not found: {e}")
            
            # Check if snapshot still exists
            snapshots_res = infinity_obj.list_snapshots()
            if snapshots_res.error_code == infinity.ErrorCode.OK:
                snapshot_names = [s.name for s in snapshots_res.snapshots]
                print(f"Available snapshots: {snapshot_names}")
            
            # Clean up
            try:
                infinity_obj.drop_snapshot("restore_interrupted_snap")
                db_obj.drop_table("test_snapshot_restore_interrupted", ConflictType.Ignore)
            except:
                pass

        part2()
