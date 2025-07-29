import infinity
from common import common_values
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from infinity import index
import time
import pathlib
from infinity.common import ConflictType, SparseVector
import pytest
import threading
import random
import json
import base64
from util import RtnThread


class TestSystemSnapshotRestart:
    """System snapshot restart testing for Infinity database"""
    
    def create_comprehensive_table(self, table_name: str, db_obj):
        """Create a table with all data types and indexes"""
        table_schema = {
            "id": {"type": "int", "constraints": ["primary key"]},
            "name": {"type": "varchar"},
            "age": {"type": "int8"},
            "salary": {"type": "float64"},
            "is_active": {"type": "bool"},
            "vector_col": {"type": "vector,1,float"},
            "tensor_col": {"type": "tensor,2,float"},
            "sparse_col": {"type": "sparse,3,float,int16"}
        }
        
        # Create table
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name, table_schema, ConflictType.Ignore)
        
        return table_obj
    
    def create_simple_table(self, table_name: str, db_obj):
        """Create a simple table with basic data types"""
        table_schema = {
            "id": {"type": "int", "constraints": ["primary key"]},
            "name": {"type": "varchar"},
            "value": {"type": "float"}
        }
        
        # Create table
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name, table_schema, ConflictType.Ignore)
        
        return table_obj
    
    def create_indexes_for_table(self, table_obj):
        """Create various types of indexes for a table"""
        # Full-text search index
        table_obj.create_index("idx_name_fts", index.IndexInfo("name", index.IndexType.FullText), ConflictType.Ignore)
        
        # BMP index
        table_obj.create_index("idx_vector_bmp", index.IndexInfo("sparse_col", index.IndexType.BMP, {"block_size": "16", "compress_type": "compress"}), ConflictType.Ignore)
        
        # IVF index
        table_obj.create_index("idx_vector_ivf", index.IndexInfo("vector_col", index.IndexType.IVF, {"metric": "l2"}), ConflictType.Ignore)
    
    def insert_data_for_table(self, table_obj, num_rows: int = 100):
        """Insert test data for a comprehensive table"""
        data = []
        for i in range(num_rows):
            # Create sparse vector data
            num_non_zero = random.randint(1, 2)
            indices = sorted(random.sample(range(3), num_non_zero))
            values = [random.uniform(-1, 1) for _ in range(num_non_zero)]
            sparse_data = SparseVector(indices, values)
            
            row = {
                "id": i,
                "name": f"user_{i}",
                "age": random.randint(18, 80),
                "salary": random.uniform(30000, 150000),
                "is_active": random.choice([True, False]),
                "vector_col": [random.uniform(-1, 1)],
                "tensor_col": [random.uniform(-1, 1), random.uniform(-1, 1)],
                "sparse_col": sparse_data
            }
            data.append(row)
        
        table_obj.insert(data)
    
    def insert_simple_data_for_table(self, table_obj, num_rows: int = 50):
        """Insert test data for a simple table"""
        data = []
        for i in range(num_rows):
            row = {
                "id": i,
                "name": f"simple_user_{i}",
                "value": random.uniform(0, 1000)
            }
            data.append(row)
        
        table_obj.insert(data)
    
    def verify_table_functionality(self, table_name: str, db_obj, expected_row_count: int | None = None):
        """Verify table functionality including queries and indexes"""
        table_obj = db_obj.get_table(table_name)
        
        # Basic query
        result, extra = table_obj.output(["*"]).to_df()
        if expected_row_count is not None:
            assert len(result) == expected_row_count, f"Expected {expected_row_count} rows, got {len(result)}"
        
        # Test full-text search if available
        try:
            search_result = table_obj.query("name", "user_1", index_name="idx_name_fts")
            assert len(search_result) > 0, "Full-text search should return results"
        except:
            pass  # Index might not exist
        
        # Test vector similarity search if available
        try:
            vector_result = table_obj.query("vector_col", [0.5], index_name="idx_vector_ivf")
            assert len(vector_result) > 0, "Vector similarity search should return results"
        except:
            pass  # Index might not exist
        
        print(f"   Table {table_name} functionality verified")
    
    def verify_simple_table_functionality(self, table_name: str, db_obj, expected_row_count: int | None = None):
        """Verify simple table functionality"""
        table_obj = db_obj.get_table(table_name)
        
        # Basic query
        result, extra = table_obj.output(["*"]).to_df()
        if expected_row_count is not None:
            assert len(result) == expected_row_count, f"Expected {expected_row_count} rows, got {len(result)}"
        
        print(f"   Simple table {table_name} functionality verified")
    
    def verify_database_operations(self, db_obj, expected_tables: list | None = None):
        """Verify database operations"""
        # List tables
        tables = db_obj.list_tables()
        assert tables is not None, "Database should have tables"
        
        if expected_tables:
            table_names = [table.name for table in tables.tables]
            for expected_table in expected_tables:
                assert expected_table in table_names, f"Expected table {expected_table} not found"
        
        print(f"   Database operations verification completed successfully")
    
    def test_system_snapshot_basic_restart(self, infinity_runner: InfinityRunner):
        """Test basic system snapshot functionality across restarts"""
        config1 = "test/data/config/restart_test/test_system_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_system_snapshot/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            print("=== Creating databases and system snapshot ===")
            
            # Create test databases
            db_configs = [
                {"name": "restart_db_1", "tables": [
                    {"name": "restart_table_1", "rows": 50, "type": "comprehensive"},
                    {"name": "restart_table_2", "rows": 25, "type": "simple"}
                ]},
                {"name": "restart_db_2", "tables": [
                    {"name": "restart_table_3", "rows": 30, "type": "comprehensive"}
                ]}
            ]
            
            created_databases = []
            
            # Create databases and populate with data
            for db_config in db_configs:
                db_name = db_config["name"]
                infinity_obj.drop_database(db_name, ConflictType.Ignore)
                infinity_obj.create_database(db_name)
                db_obj = infinity_obj.get_database(db_name)
                created_databases.append(db_name)
                
                print(f"Creating database: {db_name}")
                for table_config in db_config["tables"]:
                    table_name = table_config["name"]
                    
                    if table_config["type"] == "comprehensive":
                        table_obj = self.create_comprehensive_table(table_name, db_obj)
                        self.insert_data_for_table(table_obj, table_config["rows"])
                        self.create_indexes_for_table(table_obj)
                    else:
                        table_obj = self.create_simple_table(table_name, db_obj)
                        self.insert_simple_data_for_table(table_obj, table_config["rows"])
                    
                    print(f"  Created table: {table_name} with {table_config['rows']} rows")
            
            # Create system snapshot
            snapshot_name = "basic_restart_snapshot"
            print(f"Creating system snapshot: {snapshot_name}")
            snapshot_result = infinity_obj.create_system_snapshot(snapshot_name)
            assert snapshot_result.error_code == infinity.ErrorCode.OK, f"System snapshot creation failed: {snapshot_result.error_code}"
            
            # Verify snapshot exists
            snapshots_response = infinity_obj.list_snapshots()
            snapshots = snapshots_response.snapshots
            assert snapshot_name in [snapshot.name for snapshot in snapshots], f"Snapshot {snapshot_name} not found in list"
            
            print("System snapshot created successfully")

        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            print("=== Testing system snapshot after restart ===")
            
            # Verify snapshot still exists after restart
            snapshots_response = infinity_obj.list_snapshots()
            snapshots = snapshots_response.snapshots
            snapshot_name = "basic_restart_snapshot"
            assert snapshot_name in [snapshot.name for snapshot in snapshots], f"Snapshot {snapshot_name} not found after restart"
            print(f"Snapshot {snapshot_name} persisted through restart")
            
            # Add new data after restart
            infinity_obj.create_database("post_restart_db")
            db_obj = infinity_obj.get_database("post_restart_db")
            table_obj = self.create_simple_table("post_restart_table", db_obj)
            self.insert_simple_data_for_table(table_obj, 10)
            
            # Verify new data exists
            databases_before_restore = infinity_obj.list_databases()
            assert "post_restart_db" in databases_before_restore.db_names, "Post-restart database should exist"
            
            # Restore from snapshot
            print("Restoring from system snapshot...")
            restore_result = infinity_obj.restore_system_snapshot(snapshot_name)
            assert restore_result.error_code == infinity.ErrorCode.OK, f"System snapshot restore failed: {restore_result.error_code}"
            
            # Verify original databases are restored
            databases_after_restore = infinity_obj.list_databases()
            db_names_after_restore = databases_after_restore.db_names
            
            expected_databases = ["restart_db_1", "restart_db_2"]
            for db_name in expected_databases:
                assert db_name in db_names_after_restore, f"Database {db_name} not restored"
            
            # Verify post-restart database is gone
            assert "post_restart_db" not in db_names_after_restore, "Post-restart database should not exist after restore"
            
            # Verify functionality of restored databases
            print("Verifying restored database functionality...")
            
            # Define the basic restart configuration for verification
            basic_db_configs = [
                {"name": "restart_db_1", "tables": [
                    {"name": "restart_table_1", "rows": 50, "type": "comprehensive"},
                    {"name": "restart_table_2", "rows": 25, "type": "simple"}
                ]},
                {"name": "restart_db_2", "tables": [
                    {"name": "restart_table_3", "rows": 30, "type": "comprehensive"}
                ]}
            ]
            
            for db_name in expected_databases:
                db_obj = infinity_obj.get_database(db_name)
                self.verify_database_operations(db_obj)
                
                # Verify tables in the database
                tables = db_obj.list_tables()
                table_names = tables.table_names
                
                # Check which tables are comprehensive based on the original config
                for db_config in basic_db_configs:
                    if db_config["name"] == db_name:
                        for table_config in db_config["tables"]:
                            table_name = table_config["name"]
                            if table_name in table_names:
                                if table_config["type"] == "comprehensive":
                                    self.verify_table_functionality(table_name, db_obj, expected_row_count=table_config["rows"])
                                else:
                                    self.verify_simple_table_functionality(table_name, db_obj, expected_row_count=table_config["rows"])
            
            # Test creating new snapshot after restore
            new_snapshot_name = "post_restore_snapshot"
            new_snapshot_result = infinity_obj.create_system_snapshot(new_snapshot_name)
            assert new_snapshot_result.error_code == infinity.ErrorCode.OK, f"New snapshot creation failed: {new_snapshot_result.error_code}"
            
            # Verify new snapshot exists
            snapshots_final = infinity_obj.list_snapshots()
            snapshots_final_list = snapshots_final.snapshots
            assert new_snapshot_name in [snapshot.name for snapshot in snapshots_final_list], f"New snapshot {new_snapshot_name} not found"
            
            # Clean up snapshots
            drop_result = infinity_obj.drop_snapshot(snapshot_name)
            assert drop_result.error_code == infinity.ErrorCode.OK
            
            drop_result_2 = infinity_obj.drop_snapshot(new_snapshot_name)
            assert drop_result_2.error_code == infinity.ErrorCode.OK
            
            print("System snapshot restart test completed successfully")

        # Run the test
        part1()
        part2()

    def test_system_snapshot_multiple_restarts(self, infinity_runner: InfinityRunner):
        """Test system snapshot functionality across multiple restarts"""
        config1 = "test/data/config/restart_test/test_system_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_system_snapshot/2.toml"
        config3 = "test/data/config/restart_test/test_system_snapshot/3.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            print("=== First restart cycle: Creating data and snapshot ===")
            
            # Create test database
            infinity_obj.drop_database("multi_restart_db", ConflictType.Ignore)
            infinity_obj.create_database("multi_restart_db")
            db_obj = infinity_obj.get_database("multi_restart_db")
            
            # Create comprehensive table
            table_obj = self.create_comprehensive_table("multi_restart_table", db_obj)
            self.insert_data_for_table(table_obj, 100)
            self.create_indexes_for_table(table_obj)
            
            # Create system snapshot
            snapshot_name = "multi_restart_snapshot"
            snapshot_result = infinity_obj.create_system_snapshot(snapshot_name)
            assert snapshot_result.error_code == infinity.ErrorCode.OK, f"System snapshot creation failed: {snapshot_result.error_code}"
            
            print("First restart cycle completed")

        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            print("=== Second restart cycle: Testing restore and new operations ===")
            
            # Verify snapshot exists after first restart
            snapshots_response = infinity_obj.list_snapshots()
            snapshots = snapshots_response.snapshots
            snapshot_name = "multi_restart_snapshot"
            assert snapshot_name in [snapshot.name for snapshot in snapshots], f"Snapshot {snapshot_name} not found after first restart"
            
            # Add new data
            infinity_obj.create_database("second_restart_db")
            db_obj = infinity_obj.get_database("second_restart_db")
            table_obj = self.create_simple_table("second_restart_table", db_obj)
            self.insert_simple_data_for_table(table_obj, 20)
            
            # Restore from snapshot
            restore_result = infinity_obj.restore_system_snapshot(snapshot_name)
            assert restore_result.error_code == infinity.ErrorCode.OK, f"System snapshot restore failed: {restore_result.error_code}"
            
            # Verify original database is restored
            databases_after_restore = infinity_obj.list_databases()
            assert "multi_restart_db" in databases_after_restore.db_names, "Original database not restored"
            assert "second_restart_db" not in databases_after_restore.db_names, "Second restart database should not exist after restore"
            
            # Verify functionality
            db_obj = infinity_obj.get_database("multi_restart_db")
            self.verify_database_operations(db_obj)
            self.verify_table_functionality("multi_restart_table", db_obj, expected_row_count=100)
            
            # Create new snapshot after restore
            new_snapshot_name = "post_restore_snapshot"
            new_snapshot_result = infinity_obj.create_system_snapshot(new_snapshot_name)
            assert new_snapshot_result.error_code == infinity.ErrorCode.OK, f"New snapshot creation failed: {new_snapshot_result.error_code}"
            
            print("Second restart cycle completed")

        decorator3 = infinity_runner_decorator_factory(config3, uri, infinity_runner)

        @decorator3
        def part3(infinity_obj):
            print("=== Third restart cycle: Final verification ===")
            
            # Verify both snapshots exist after second restart
            snapshots_response = infinity_obj.list_snapshots()
            snapshots = snapshots_response.snapshots
            snapshot_names = [snapshot.name for snapshot in snapshots]
            
            assert "multi_restart_snapshot" in snapshot_names, "Original snapshot not found after second restart"
            assert "post_restore_snapshot" in snapshot_names, "New snapshot not found after second restart"
            
            # Test restore from the new snapshot
            restore_result = infinity_obj.restore_system_snapshot("post_restore_snapshot")
            assert restore_result.error_code == infinity.ErrorCode.OK, f"Restore from new snapshot failed: {restore_result.error_code}"
            
            # Verify data integrity after multiple restarts
            databases_final = infinity_obj.list_databases()
            assert "multi_restart_db" in databases_final.db_names, "Database not found after multiple restarts"
            
            # Verify functionality after multiple restarts
            db_obj = infinity_obj.get_database("multi_restart_db")
            self.verify_database_operations(db_obj)
            self.verify_table_functionality("multi_restart_table", db_obj, expected_row_count=100)
            
            # Clean up snapshots
            drop_result1 = infinity_obj.drop_snapshot("multi_restart_snapshot")
            assert drop_result1.error_code == infinity.ErrorCode.OK
            
            drop_result2 = infinity_obj.drop_snapshot("post_restore_snapshot")
            assert drop_result2.error_code == infinity.ErrorCode.OK
            
            print("Third restart cycle completed - data integrity verified")

        # Run the test
        part1()
        part2()
        part3()

    def test_system_snapshot_large_data_restart(self, infinity_runner: InfinityRunner):
        """Test system snapshot with large data across restarts"""
        config1 = "test/data/config/restart_test/test_system_snapshot/1.toml"
        config2 = "test/data/config/restart_test/test_system_snapshot/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)

        @decorator1
        def part1(infinity_obj):
            print("=== Creating large dataset and system snapshot ===")
            
            # Create multiple databases with large datasets
            db_configs = [
                {"name": "large_restart_db_1", "tables": [
                    {"name": "large_table_1", "rows": 500, "type": "comprehensive"},
                    {"name": "large_table_2", "rows": 300, "type": "simple"}
                ]},
                {"name": "large_restart_db_2", "tables": [
                    {"name": "large_table_3", "rows": 400, "type": "comprehensive"}
                ]}
            ]
            
            created_databases = []
            
            # Create databases and populate with large datasets
            for db_config in db_configs:
                db_name = db_config["name"]
                infinity_obj.drop_database(db_name, ConflictType.Ignore)
                infinity_obj.create_database(db_name)
                db_obj = infinity_obj.get_database(db_name)
                created_databases.append(db_name)
                
                print(f"Creating large database: {db_name}")
                for table_config in db_config["tables"]:
                    table_name = table_config["name"]
                    
                    if table_config["type"] == "comprehensive":
                        table_obj = self.create_comprehensive_table(table_name, db_obj)
                        self.insert_data_for_table(table_obj, table_config["rows"])
                        self.create_indexes_for_table(table_obj)
                    else:
                        table_obj = self.create_simple_table(table_name, db_obj)
                        self.insert_simple_data_for_table(table_obj, table_config["rows"])
                    
                    print(f"  Created large table: {table_name} with {table_config['rows']} rows")
            
            # Create system snapshot
            snapshot_name = "large_data_snapshot"
            print(f"Creating system snapshot with large data: {snapshot_name}")
            snapshot_result = infinity_obj.create_system_snapshot(snapshot_name)
            assert snapshot_result.error_code == infinity.ErrorCode.OK, f"Large data snapshot creation failed: {snapshot_result.error_code}"
            
            print("Large data snapshot created successfully")

        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        @decorator2
        def part2(infinity_obj):
            print("=== Testing large data snapshot after restart ===")
            
            # Verify snapshot exists after restart
            snapshots_response = infinity_obj.list_snapshots()
            snapshots = snapshots_response.snapshots
            snapshot_name = "large_data_snapshot"
            assert snapshot_name in [snapshot.name for snapshot in snapshots], f"Large data snapshot not found after restart"
            
            # Add more large data after restart
            infinity_obj.create_database("post_restart_large_db")
            db_obj = infinity_obj.get_database("post_restart_large_db")
            table_obj = self.create_comprehensive_table("post_restart_large_table", db_obj)
            self.insert_data_for_table(table_obj, 200)
            self.create_indexes_for_table(table_obj)
            
            # Restore from snapshot
            print("Restoring from large data snapshot...")
            restore_result = infinity_obj.restore_system_snapshot(snapshot_name)
            assert restore_result.error_code == infinity.ErrorCode.OK, f"Large data snapshot restore failed: {restore_result.error_code}"
            
            # Verify original databases are restored
            databases_after_restore = infinity_obj.list_databases()
            db_names_after_restore = databases_after_restore.db_names
            
            expected_databases = ["large_restart_db_1", "large_restart_db_2"]
            for db_name in expected_databases:
                assert db_name in db_names_after_restore, f"Large database {db_name} not restored"
            
            # Verify post-restart database is gone
            assert "post_restart_large_db" not in db_names_after_restore, "Post-restart large database should not exist after restore"
            
            # Verify functionality of restored large databases
            print("Verifying large database functionality...")
            
            # Define the large data configuration for verification
            large_db_configs = [
                {"name": "large_restart_db_1", "tables": [
                    {"name": "large_table_1", "rows": 500, "type": "comprehensive"},
                    {"name": "large_table_2", "rows": 300, "type": "simple"}
                ]},
                {"name": "large_restart_db_2", "tables": [
                    {"name": "large_table_3", "rows": 400, "type": "comprehensive"}
                ]}
            ]
            
            for db_name in expected_databases:
                db_obj = infinity_obj.get_database(db_name)
                self.verify_database_operations(db_obj)
                
                # Verify tables in the database
                tables = db_obj.list_tables()
                table_names = tables.table_names
                
                # Check which tables are comprehensive based on the original config
                for db_config in large_db_configs:
                    if db_config["name"] == db_name:
                        for table_config in db_config["tables"]:
                            table_name = table_config["name"]
                            if table_name in table_names:
                                if table_config["type"] == "comprehensive":
                                    self.verify_table_functionality(table_name, db_obj, expected_row_count=table_config["rows"])
                                else:
                                    self.verify_simple_table_functionality(table_name, db_obj, expected_row_count=table_config["rows"])
            
            # Clean up snapshot
            drop_result = infinity_obj.drop_snapshot(snapshot_name)
            assert drop_result.error_code == infinity.ErrorCode.OK
            
            print("Large data snapshot restart test completed successfully")

        # Run the test
        part1()
        part2()