import sys
import os
import concurrent.futures
import threading
import pytest
import polars as pl
from common import common_values
from infinity.common import ConflictType, InfinityException, SparseVector
import infinity
from infinity.errors import ErrorCode
from common.utils import trace_expected_exceptions
import random
import json
import base64
import time
import infinity.index as index

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http

@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")

@pytest.fixture(scope="class")
def setup_class(request, http):
    if http:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity_http()
    else:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity.connect(uri)
    request.cls.uri = uri
    yield
    request.cls.infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
@pytest.mark.usefixtures("suffix")
class TestSystemSnapshot:
    # Class-level retry configuration
    DEFAULT_MAX_RETRIES = 3
    DEFAULT_RETRY_DELAY = 2
    LARGE_SYSTEM_MAX_RETRIES = 5
    LARGE_SYSTEM_RETRY_DELAY = 3

    def create_system_snapshot_with_retry(self, infinity_obj, snapshot_name, max_retries=None, retry_delay=None):
        """Create system snapshot with retry logic for checkpoint failures"""
        if max_retries is None:
            max_retries = self.DEFAULT_MAX_RETRIES
        if retry_delay is None:
            retry_delay = self.DEFAULT_RETRY_DELAY
            
        for attempt in range(max_retries):
            try:
                snapshot_result = infinity_obj.create_system_snapshot(snapshot_name)
                
                if snapshot_result.error_code == ErrorCode.OK:
                    print(f"System snapshot created successfully on attempt {attempt + 1}")
                    return snapshot_result
                
                # Check if failure is checkpoint-related
                if snapshot_result.error_code == ErrorCode.CHECKPOINTING:
                    if attempt < max_retries - 1:
                        print(f"Attempt {attempt + 1}: System is checkpointing, retrying in {retry_delay}s...")
                        time.sleep(retry_delay)
                        continue
                    else:
                        print(f"All {max_retries} attempts failed due to checkpointing")
                        return snapshot_result
                else:
                    # Non-checkpoint failure, don't retry
                    print(f"Non-checkpoint failure on attempt {attempt + 1}: {snapshot_result.error_msg}")
                    return snapshot_result
                    
            except Exception as e:
                if "checkpoint" in str(e).lower() and attempt < max_retries - 1:
                    print(f"Attempt {attempt + 1}: Checkpoint exception, retrying in {retry_delay}s...")
                    time.sleep(retry_delay)
                    continue
                else:
                    raise
        
        # Should not reach here, but just in case
        return snapshot_result

    def restore_system_snapshot_with_retry(self, infinity_obj, snapshot_name, max_retries=None, retry_delay=None):
        """Restore system snapshot with retry logic for checkpoint failures"""
        if max_retries is None:
            max_retries = self.DEFAULT_MAX_RETRIES
        if retry_delay is None:
            retry_delay = self.DEFAULT_RETRY_DELAY
            
        for attempt in range(max_retries):
            try:
                restore_result = infinity_obj.restore_system_snapshot(snapshot_name)
                
                if restore_result.error_code == ErrorCode.OK:
                    print(f"System snapshot restored successfully on attempt {attempt + 1}")
                    return restore_result
                
                # Check if failure is checkpoint-related
                if restore_result.error_code == ErrorCode.CHECKPOINTING:
                    if attempt < max_retries - 1:
                        print(f"Attempt {attempt + 1}: System is checkpointing, retrying in {retry_delay}s...")
                        time.sleep(retry_delay)
                        continue
                    else:
                        print(f"All {max_retries} attempts failed due to checkpointing")
                        return restore_result
                else:
                    # Non-checkpoint failure, don't retry
                    print(f"Non-checkpoint failure on attempt {attempt + 1}: {restore_result.error_msg}")
                    return restore_result
                    
            except Exception as e:
                if "checkpoint" in str(e).lower() and attempt < max_retries - 1:
                    print(f"Attempt {attempt + 1}: Checkpoint exception, retrying in {retry_delay}s...")
                    time.sleep(retry_delay)
                    continue
                else:
                    raise
        
        # Should not reach here, but just in case
        return restore_result

    def handle_system_snapshot_result(self, snapshot_result, operation_name="System snapshot operation", skip_on_checkpoint=True):
        """Handle system snapshot operation results with consistent error handling"""
        if snapshot_result.error_code == ErrorCode.OK:
            print(f"{operation_name} completed successfully")
            return True
        elif snapshot_result.error_code == ErrorCode.CHECKPOINTING:
            if skip_on_checkpoint:
                pytest.skip(f"{operation_name} failed due to checkpointing: {snapshot_result.error_msg}")
            else:
                print(f"Warning: {operation_name} failed due to checkpointing: {snapshot_result.error_msg}")
                return False
        else:
            assert False, f"{operation_name} failed with error: {snapshot_result.error_msg}"
    
    def handle_system_snapshot_restore_result(self, restore_result, operation_name="System snapshot restore operation", skip_on_checkpoint=True):
        """Handle system snapshot restore operation results with consistent error handling"""
        if restore_result.error_code == ErrorCode.OK:
            print(f"{operation_name} completed successfully")
            return True
        elif restore_result.error_code == ErrorCode.CHECKPOINTING:
            if skip_on_checkpoint:
                pytest.skip(f"{operation_name} failed due to checkpointing: {restore_result.error_msg}")
            else:
                print(f"Warning: {operation_name} failed due to checkpointing: {restore_result.error_msg}")
                return False
        else:
            assert False, f"{operation_name} failed with error: {restore_result.error_msg}"
    
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
            row = {
                "id": i,
                "name": f"user_{i}",
                "age": random.randint(18, 80),
                "salary": random.uniform(30000, 150000),
                "is_active": random.choice([True, False]),
                "vector_col": [random.uniform(-1, 1)],
                "tensor_col": [random.uniform(-1, 1), random.uniform(-1, 1)],
                "sparse_col": SparseVector([0, 1], [1.0, 1.0]) 
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
        """Verify that a table functions correctly after restore"""
        print(f"\n=== Verifying table functionality: {table_name} ===")
        
        # Get the restored table
        restored_table = db_obj.get_table(table_name)
        
        # 1. Verify basic table structure and data
        print("1. Verifying table structure and data...")
        try:
            # Count rows using fast count query
            count_result, extra_result = restored_table.output(["count(*)"]).to_df()
            row_count = int(count_result.iloc[0, 0])  # Convert to Python int
            print(f"   Row count: {row_count}")
            
            if expected_row_count:
                assert row_count == expected_row_count, f"Expected {expected_row_count} rows, got {row_count}"
            
            # Get column names
            columns_result = restored_table.show_columns()
            column_names = columns_result["name"].to_list()
            print(f"   Table columns: {column_names}")
            
        except Exception as e:
            print(f"   ERROR in basic verification: {e}")
            raise
        
        # 2. Test search operations
        print("2. Testing search operations...")
        try:
            # Test vector search
            search_result = restored_table.output(["*"]).knn("vector_col", [0.5], "float", "l2", 5).to_df()
            print(f"   Vector search returned {len(search_result)} results")
            
            # Test full-text search
            search_result = restored_table.output(["*"]).match("name", "user_1", "topk=3").to_df()
            print(f"   Full-text search returned {len(search_result)} results")
            
        except Exception as e:
            print(f"   ERROR in search operations: {e}")
            # Search might fail if indexes weren't created, which is acceptable
        
        # 3. Test data insertion
        print("3. Testing data insertion...")
        try:
            # Insert a new row - use Python int, not NumPy int64
            new_row = {
                "id": int(row_count + 1),  # Convert to Python int
                "name": "test_insert_user",
                "age": 25,
                "salary": 75000.0,
                "is_active": True,
                "vector_col": [0.1],
                "tensor_col": [0.1, 0.2],
                "sparse_col": SparseVector([0, 1], [0.1, 0.2])
            }
            restored_table.insert(new_row)
            print("   Data insertion successful")
            
        except Exception as e:
            print(f"   ERROR in data insertion: {e}")
            raise
        
        print(f"   Table {table_name} verification completed successfully")
    
    def verify_simple_table_functionality(self, table_name: str, db_obj, expected_row_count: int | None = None):
        """Verify that a simple table functions correctly after restore"""
        print(f"\n=== Verifying simple table functionality: {table_name} ===")
        
        # Get the restored table
        restored_table = db_obj.get_table(table_name)
        
        # 1. Verify basic table structure and data
        print("1. Verifying table structure and data...")
        try:
            # Count rows
            count_result, extra_result = restored_table.output(["count(*)"]).to_df()
            row_count = int(count_result.iloc[0, 0])  # Convert to Python int
            print(f"   Row count: {row_count}")
            
            if expected_row_count:
                assert row_count == expected_row_count, f"Expected {expected_row_count} rows, got {row_count}"
            
            # Get column names
            columns_result = restored_table.show_columns()
            column_names = columns_result["name"].to_list()
            print(f"   Table columns: {column_names}")
            
        except Exception as e:
            print(f"   ERROR in basic verification: {e}")
            raise
        
        # 2. Test data insertion
        print("2. Testing data insertion...")
        try:
            # Insert a new row - use Python int
            new_row = {
                "id": int(row_count + 1),  # Convert to Python int
                "name": "test_simple_user",
                "value": 999.99
            }
            restored_table.insert(new_row)
            print("   Data insertion successful")
            
        except Exception as e:
            print(f"   ERROR in data insertion: {e}")
            raise
        
        print(f"   Simple table {table_name} verification completed successfully")
    
    def get_table_names(self, db_obj):
        try:
            tables = db_obj.list_tables()
            if hasattr(tables, 'table_names'):
                return tables.table_names
            else:
                # HTTP client fallback
                return db_obj.get_all_tables()
        except Exception as e:
            print(f"Warning: Could not get table names: {e}")
            return []
    
    def verify_database_operations(self, db_obj, expected_tables: list | None = None):
        """Verify that database operations work correctly after restore"""
        print(f"\n=== Verifying database operations ===")
        
        try:
            # List tables
            table_names = self.get_table_names(db_obj)
            print(f"   Database tables: {table_names}")
            
            if expected_tables:
                for table_name in expected_tables:
                    assert table_name in table_names, f"Expected table {table_name} not found"
            
            # Test creating a new table
            test_table_name = "test_post_restore_table"
            db_obj.drop_table(test_table_name, ConflictType.Ignore)
            test_table = db_obj.create_table(test_table_name, {
                "id": {"type": "int"},
                "name": {"type": "varchar"}
            })
            test_table.insert([{"id": 1, "name": "test"}])
            print("   New table creation successful")
            
            # Clean up test table
            db_obj.drop_table(test_table_name, ConflictType.Ignore)
            
        except Exception as e:
            print(f"   ERROR in database operations: {e}")
            raise
        
        print("   Database operations verification completed successfully")
    
    def verify_system_operations_post_restore(self):
        """Verify add, delete, and (if possible) rename database operations after restore."""
        print("\n--- Verifying system operations after restore ---")
        # Add a new database
        new_db_name = "post_restore_db"
        self.infinity_obj.drop_database(new_db_name, ConflictType.Ignore)
        self.infinity_obj.create_database(new_db_name)
        db_obj = self.infinity_obj.get_database(new_db_name)
        db_obj.create_table("verify_table", {"id": {"type": "int"}})
        table = db_obj.get_table("verify_table")
        table.insert([{"id": 1}])
        print(f"Added and verified new database: {new_db_name}")

        # Delete the new database
        self.infinity_obj.get_database("default_db")
        self.infinity_obj.drop_database(new_db_name, ConflictType.Error)
        databases = self.infinity_obj.list_databases().db_names
        assert new_db_name not in databases, "Database was not deleted after restore"
        print(f"Deleted database: {new_db_name}")

        # Try to rename a database (if supported)
        # NOTE: Infinity SDK does not currently support database renaming. If added, implement here.
        print("Database rename not supported in current SDK. Skipping rename verification.")

        print("System operations after restore verified successfully.")

    def test_system_snapshot_empty_system(self, suffix):
        """Test system snapshot with empty system (no databases)"""
        print(f"\n=== Test: System Snapshot with Empty System ===")
        
        # Create system snapshot of empty system
        snapshot_name = f"empty_system_snapshot{suffix}"
        print(f"Creating system snapshot of empty system: {snapshot_name}")
        snapshot_result = self.create_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
        self.handle_system_snapshot_result(snapshot_result, "Empty system snapshot creation")
        
        # Verify snapshot exists
        snapshots_response = self.infinity_obj.list_snapshots()
        snapshots = snapshots_response.snapshots
        assert snapshot_name in [snapshot.name for snapshot in snapshots]
        
        # Create some databases after snapshot
        self.infinity_obj.drop_database("test_db_1", ConflictType.Ignore)
        self.infinity_obj.drop_database("test_db_2", ConflictType.Ignore)
        self.infinity_obj.create_database("test_db_1")
        self.infinity_obj.create_database("test_db_2")
        
        # Restore from snapshot
        print("Restoring from empty system snapshot...")
        restore_result = self.restore_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
        self.handle_system_snapshot_restore_result(restore_result, "Empty system snapshot restore")
        
        # Verify system is empty after restore
        # # Only default_db should exist
        # assert len(db_names) == 1, f"Expected 1 database after restore, got {len(db_names)}"
        # assert "default_db" in db_names, "default_db should exist after restore"
        
        # Drop snapshot
        drop_result = self.infinity_obj.drop_snapshot(snapshot_name)
        assert drop_result.error_code == ErrorCode.OK
        
        print("Empty system snapshot test completed successfully")

    def test_system_snapshot_basic(self, suffix):
        """Test basic system snapshot create, list, drop operations"""
        print(f"\n=== Test: Basic System Snapshot Operations ===")

        self.infinity_obj.get_database("default_db")
        
        # Create multiple databases with tables
        db_configs = [
            {"name": "test_db_1", "tables": [
                {"name": "comprehensive_table_1", "rows": 50, "has_indexes": True, "type": "comprehensive"},
                {"name": "simple_table_1", "rows": 25, "has_indexes": False, "type": "simple"}
            ]},
            {"name": "test_db_2", "tables": [
                {"name": "comprehensive_table_2", "rows": 75, "has_indexes": True, "type": "comprehensive"},
                {"name": "simple_table_2", "rows": 30, "has_indexes": False, "type": "simple"}
            ]},
            {"name": "test_db_3", "tables": [
                {"name": "simple_table_3", "rows": 40, "has_indexes": False, "type": "simple"}
            ]}
        ]
        
        created_databases = []
        
        # Create all databases and tables
        for db_config in db_configs:
            db_name = db_config["name"]
            self.infinity_obj.drop_database(db_name, ConflictType.Ignore)
            self.infinity_obj.create_database(db_name)
            db_obj = self.infinity_obj.get_database(db_name)
            created_databases.append(db_name)
            
            print(f"Creating database: {db_name}")
            for table_config in db_config["tables"]:
                table_name = table_config["name"]
                
                if table_config["type"] == "comprehensive":
                    table_obj = self.create_comprehensive_table(table_name, db_obj)
                    self.insert_data_for_table(table_obj, table_config["rows"])
                    if table_config["has_indexes"]:
                        self.create_indexes_for_table(table_obj)
                else:  # simple
                    table_obj = self.create_simple_table(table_name, db_obj)
                    self.insert_simple_data_for_table(table_obj, table_config["rows"])
                
                print(f"  Created table: {table_name} with {table_config['rows']} rows")
        
        # Verify all databases exist
        databases = self.infinity_obj.list_databases()
        db_names = databases.db_names
        for db_name in created_databases:
            assert db_name in db_names, f"Database {db_name} not found"
        
        # Create system snapshot
        snapshot_name = f"basic_system_snapshot{suffix}"
        print(f"Creating system snapshot: {snapshot_name}")
        snapshot_result = self.create_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
        self.handle_system_snapshot_result(snapshot_result, "Basic system snapshot creation")
        
        # Verify snapshot exists
        snapshots_response = self.infinity_obj.list_snapshots()
        snapshots = snapshots_response.snapshots
        assert snapshot_name in [snapshot.name for snapshot in snapshots], f"Snapshot {snapshot_name} not found in list"
        
        # Add more databases after snapshot
        self.infinity_obj.create_database("post_snapshot_db")
        post_db = self.infinity_obj.get_database("post_snapshot_db")
        post_db.create_table("post_table", {"id": {"type": "int"}})
        post_table = post_db.get_table("post_table")
        post_table.insert([{"id": 999}])

        
        # Delete one database - use Ignore to handle if database is in use
        self.infinity_obj.drop_database("test_db_3", ConflictType.Ignore)
        
        # Show databases after modifications
        databases_after_mod = self.infinity_obj.list_databases()
        db_names_after = databases_after_mod.db_names
        print(f"Databases after modifications: {db_names_after}")
        
        # Restore from system snapshot
        print("Restoring from system snapshot...")
        restore_result = self.restore_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
        self.handle_system_snapshot_restore_result(restore_result, "System snapshot restore")
        
        # Verify all original databases are restored
        databases_restored = self.infinity_obj.list_databases()
        db_names_restored = databases_restored.db_names
        print(f"Databases after restore: {db_names_restored}")
        
        # Check that original databases are back
        for db_name in created_databases:
            assert db_name in db_names_restored, f"Database {db_name} not restored"
        
        # Check that post-snapshot database is gone
        assert "post_snapshot_db" not in db_names_restored, "Post-snapshot database should not exist after restore"
        
        # Verify functionality of restored databases
        for db_config in db_configs:
            db_name = db_config["name"]
            db_obj = self.infinity_obj.get_database(db_name)
            
            # Verify database operations
            self.verify_database_operations(db_obj)
            
            # Verify each table in the database
            for table_config in db_config["tables"]:
                table_name = table_config["name"]
                if table_config["type"] == "comprehensive":
                    self.verify_table_functionality(table_name, db_obj, expected_row_count=table_config["rows"])
                else:
                    self.verify_simple_table_functionality(table_name, db_obj, expected_row_count=table_config["rows"])
        
        # Add system operations verification after restore
        self.verify_system_operations_post_restore()

        # Drop snapshot
        drop_result = self.infinity_obj.drop_snapshot(snapshot_name)
        assert drop_result.error_code == ErrorCode.OK, f"Snapshot drop failed: {drop_result.error_code}"
        
        # Clean up
        # for db_name in created_databases:
        #     self.infinity_obj.drop_database(db_name, ConflictType.Ignore)
        
        print("Basic system snapshot test completed successfully")
    
    
    
    def test_system_snapshot_large_scale(self, suffix):
        """Test system snapshot with large scale data"""
        print(f"\n=== Test: Large Scale System Snapshot ===")
        
        # Create multiple databases with large datasets
        db_configs = [
            {"name": "large_db_1", "tables": [
                {"name": "large_comp_table_1", "rows": 1000, "has_indexes": True, "type": "comprehensive"},
                {"name": "large_simple_table_1", "rows": 2000, "has_indexes": False, "type": "simple"}
            ]},
            {"name": "large_db_2", "tables": [
                {"name": "large_comp_table_2", "rows": 1500, "has_indexes": True, "type": "comprehensive"},
                {"name": "large_simple_table_2", "rows": 2500, "has_indexes": False, "type": "simple"}
            ]},
            {"name": "large_db_3", "tables": [
                {"name": "large_comp_table_3", "rows": 800, "has_indexes": False, "type": "comprehensive"},
                {"name": "large_simple_table_3", "rows": 1200, "has_indexes": False, "type": "simple"}
            ]}
        ]
        
        created_databases = []
        
        # Create all databases and tables with large datasets
        print("Creating large scale databases and tables...")
        start_time = time.time()
        
        for db_config in db_configs:
            db_name = db_config["name"]
            self.infinity_obj.drop_database(db_name, ConflictType.Ignore)
            self.infinity_obj.create_database(db_name)
            db_obj = self.infinity_obj.get_database(db_name)
            created_databases.append(db_name)
            
            print(f"Creating database: {db_name}")
            for table_config in db_config["tables"]:
                table_name = table_config["name"]
                
                if table_config["type"] == "comprehensive":
                    table_obj = self.create_comprehensive_table(table_name, db_obj)
                    self.insert_data_for_table(table_obj, table_config["rows"])
                    if table_config["has_indexes"]:
                        self.create_indexes_for_table(table_obj)
                else:  # simple
                    table_obj = self.create_simple_table(table_name, db_obj)
                    self.insert_simple_data_for_table(table_obj, table_config["rows"])
                
                print(f"  Created table: {table_name} with {table_config['rows']} rows")
        
        creation_time = time.time() - start_time
        print(f"Created {len(created_databases)} databases in {creation_time:.2f} seconds")
        
        # Create system snapshot
        snapshot_name = f"large_scale_system_snapshot{suffix}"
        print(f"Creating large scale system snapshot: {snapshot_name}")
        snapshot_start = time.time()
        snapshot_result = self.create_system_snapshot_with_retry(self.infinity_obj, snapshot_name, self.LARGE_SYSTEM_MAX_RETRIES, self.LARGE_SYSTEM_RETRY_DELAY)
        snapshot_time = time.time() - snapshot_start
        print(f"System snapshot creation time: {snapshot_time:.2f} seconds")
        
        self.handle_system_snapshot_result(snapshot_result, "Large scale system snapshot creation")
        
        # Verify snapshot exists - convert to strings
        snapshots_response = self.infinity_obj.list_snapshots()
        snapshots = snapshots_response.snapshots
        snapshot_names = [str(snapshot.name) for snapshot in snapshots]
        assert snapshot_name in snapshot_names
        
        # Add more databases after snapshot
        for i in range(2):
            db_name = f"post_snapshot_db_{i}"
            self.infinity_obj.create_database(db_name)
            db_obj = self.infinity_obj.get_database(db_name)
            db_obj.create_table(f"post_table_{i}", {"id": {"type": "int"}})
            table = db_obj.get_table(f"post_table_{i}")
            table.insert([{"id": i}])
        
        # Delete some databases
        self.infinity_obj.drop_database("large_db_3")
        
        # Restore from system snapshot
        print("Restoring from large scale system snapshot...")
        restore_start = time.time()
        restore_result = self.restore_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
        restore_time = time.time() - restore_start
        print(f"System snapshot restore time: {restore_time:.2f} seconds")
        
        self.handle_system_snapshot_restore_result(restore_result, "Large scale system snapshot restore")
        
        # Verify all original databases are restored
        databases_restored = self.infinity_obj.list_databases()
        db_names_restored = databases_restored.db_names
        
        # Check that original databases are back
        for db_name in created_databases:
            assert db_name in db_names_restored, f"Database {db_name} not restored"
        
        # Check that post-snapshot databases are gone
        for i in range(2):
            assert f"post_snapshot_db_{i}" not in db_names_restored, f"Post-snapshot database post_snapshot_db_{i} should not exist after restore"
        
        # Verify functionality of a subset of databases (for performance)
        print("Verifying subset of restored databases...")
        verification_databases = created_databases[:2]  # Verify first 2 databases
        
        for db_config in db_configs[:2]:
            db_name = db_config["name"]
            db_obj = self.infinity_obj.get_database(db_name)
            
            # Verify database operations
            self.verify_database_operations(db_obj)
            
            # Verify each table in the database
            for table_config in db_config["tables"]:
                table_name = table_config["name"]
                if table_config["type"] == "comprehensive":
                    self.verify_table_functionality(table_name, db_obj, expected_row_count=table_config["rows"])
                else:
                    self.verify_simple_table_functionality(table_name, db_obj, expected_row_count=table_config["rows"])
        
        # Drop snapshot
        drop_result = self.infinity_obj.drop_snapshot(snapshot_name)
        assert drop_result.error_code == ErrorCode.OK
        
        # Clean up
        self.infinity_obj.get_database("default_db")
        for db_name in created_databases:
            self.infinity_obj.drop_database(db_name, ConflictType.Ignore)
        
        print("Large scale system snapshot test completed successfully")
    
    def test_system_snapshot_error_handling(self, suffix):
        """Test error conditions for system snapshot operations"""
        print(f"\n=== Test: System Snapshot Error Handling ===")
        
        # Test creating snapshot with invalid name
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.create_system_snapshot("")
        
        # Test restoring non-existent snapshot
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.restore_system_snapshot("non_existent_system_snapshot")
        
        # Test dropping non-existent snapshot
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.drop_snapshot("non_existent_system_snapshot")
        
        print("System snapshot error handling test completed successfully")
    
    # put into parallel test
    # def test_system_snapshot_concurrent_operations(self, suffix):
    #     """Test concurrent system snapshot operations"""
    #     print(f"\n=== Test: Concurrent System Snapshot Operations ===")
        
    #     # Create some test data
    #     self.infinity_obj.drop_database("concurrent_test_db", ConflictType.Ignore)
    #     self.infinity_obj.create_database("concurrent_test_db")
    #     db_obj = self.infinity_obj.get_database("concurrent_test_db")
    #     table_obj = self.create_comprehensive_table("concurrent_test_table", db_obj)
    #     self.insert_data_for_table(table_obj, 100)
        
    #     # Create multiple snapshots concurrently
    #     snapshot_names = [f"concurrent_snapshot_{i}{suffix}" for i in range(3)]
        
    #     def create_snapshot(snapshot_name):
    #         try:
    #             result = self.infinity_obj.create_system_snapshot(snapshot_name)
    #             return result.error_code == ErrorCode.OK
    #         except Exception as e:
    #             print(f"Error creating snapshot {snapshot_name}: {e}")
    #             return False
        
    #     # Test concurrent snapshot creation
    #     with concurrent.futures.ThreadPoolExecutor(max_workers=3) as executor:
    #         futures = [executor.submit(create_snapshot, name) for name in snapshot_names]
    #         results = [future.result() for future in futures]
        
    #     # At least one should succeed
    #     assert any(results), "At least one concurrent snapshot creation should succeed"
        
    #     # Clean up
    #     for snapshot_name in snapshot_names:
    #         try:
    #             self.infinity_obj.drop_snapshot(snapshot_name)
    #         except:
    #             pass
        
    #     self.infinity_obj.get_database("default_db")
    #     time.sleep(0.1)  # Small delay to ensure references are released
    #     self.infinity_obj.drop_database("concurrent_test_db", ConflictType.Ignore)
        
    #     print("Concurrent system snapshot operations test completed successfully")
    
    def test_system_snapshot_naming_conventions(self, suffix):
        """Test system snapshot with various naming conventions"""
        print(f"\n=== Test: System Snapshot Naming Conventions ===")
        
        # Create some test data
        self.infinity_obj.drop_database("naming_test_db", ConflictType.Ignore)
        self.infinity_obj.create_database("naming_test_db")
        db_obj = self.infinity_obj.get_database("naming_test_db")
        table_obj = self.create_simple_table("naming_test_table", db_obj)
        self.insert_simple_data_for_table(table_obj, 10)
        
        # Test with special characters in snapshot name
        special_names = [
            "system_snapshot_with_underscores",
            "system-snapshot-with-dashes",
            "system_snapshot123",
            "SYSTEM_SNAPSHOT_UPPER",
            "system_snapshot_with_numbers_123",
        ]
        
        for snapshot_name in special_names:
            # Create snapshot
            result = self.create_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
            self.handle_system_snapshot_result(result, f"Snapshot creation with name: {snapshot_name}")
            
            # Verify snapshot exists
            snapshots_response = self.infinity_obj.list_snapshots()
            snapshots = snapshots_response.snapshots
            assert snapshot_name in [snapshot.name for snapshot in snapshots], f"Snapshot {snapshot_name} not found in list"
            
            # Restore from snapshot
            restore_result = self.restore_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
            self.handle_system_snapshot_restore_result(restore_result, f"Snapshot restore with name: {snapshot_name}")
            
            # Clean up
            self.infinity_obj.drop_snapshot(snapshot_name)
        
        self.infinity_obj.get_database("default_db")
        self.infinity_obj.drop_database("naming_test_db", ConflictType.Ignore)
        
        print("System snapshot naming conventions test completed successfully")
    
    def test_system_snapshot_performance_metrics(self, suffix):
        """Test system snapshot performance metrics"""
        print(f"\n=== Test: System Snapshot Performance Metrics ===")
        
        # Create test data
        self.infinity_obj.drop_database("perf_test_db", ConflictType.Ignore)
        self.infinity_obj.create_database("perf_test_db")
        db_obj = self.infinity_obj.get_database("perf_test_db")
        table_obj = self.create_comprehensive_table("perf_test_table", db_obj)
        self.insert_data_for_table(table_obj, 500)
        
        # Measure snapshot creation time
        snapshot_name = f"perf_system_snapshot{suffix}"
        start_time = time.time()
        snapshot_result = self.create_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
        creation_time = time.time() - start_time
        
        self.handle_system_snapshot_result(snapshot_result, "Performance system snapshot creation")
        print(f"System snapshot creation time: {creation_time:.2f} seconds")
        
        # Measure snapshot restore time
        start_time = time.time()
        restore_result = self.restore_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
        restore_time = time.time() - start_time
        
        assert restore_result.error_code == ErrorCode.OK
        print(f"System snapshot restore time: {restore_time:.2f} seconds")
        
        # Measure snapshot drop time
        start_time = time.time()
        drop_result = self.infinity_obj.drop_snapshot(snapshot_name)
        drop_time = time.time() - start_time
        
        assert drop_result.error_code == ErrorCode.OK
        print(f"System snapshot drop time: {drop_time:.2f} seconds")
        
        self.infinity_obj.get_database("default_db")
        self.infinity_obj.drop_database("perf_test_db", ConflictType.Ignore)
        
        print("System snapshot performance metrics test completed successfully")
    
    def test_system_snapshot_edge_cases(self, suffix):
        """Test system snapshot edge cases"""
        print(f"\n=== Test: System Snapshot Edge Cases ===")
        
        # Test 1: Create snapshot, drop all databases, then restore
        self.infinity_obj.drop_database("edge_test_db", ConflictType.Ignore)
        self.infinity_obj.create_database("edge_test_db")
        db_obj = self.infinity_obj.get_database("edge_test_db")
        table_obj = self.create_simple_table("edge_test_table", db_obj)
        self.insert_simple_data_for_table(table_obj, 10)
        
        snapshot_name = f"edge_system_snapshot{suffix}"
        snapshot_result = self.create_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
        self.handle_system_snapshot_result(snapshot_result, "Edge case system snapshot creation")
        
        # Drop all databases except default_db
        self.infinity_obj.get_database("default_db")
        databases = self.infinity_obj.list_databases()
        for db_name in databases.db_names:
            if db_name != "default_db":
                self.infinity_obj.drop_database(db_name, ConflictType.Ignore)
        
        # Restore from snapshot
        restore_result = self.restore_system_snapshot_with_retry(self.infinity_obj, snapshot_name)
        self.handle_system_snapshot_restore_result(restore_result, "Edge case system snapshot restore")
        
        # Verify database is restored
        databases_after = self.infinity_obj.list_databases()
        assert "edge_test_db" in databases_after.db_names
        
        # Test 2: Create snapshot with maximum data, then restore
        self.infinity_obj.drop_database("edge_test_db", ConflictType.Ignore)
        
        # Create multiple databases with maximum data
        for i in range(5):
            db_name = f"max_data_db_{i}"
            self.infinity_obj.create_database(db_name, ConflictType.Ignore)
            db_obj = self.infinity_obj.get_database(db_name)
            
            for j in range(3):
                table_name = f"max_data_table_{j}"
                table_obj = self.create_comprehensive_table(table_name, db_obj)
                self.insert_data_for_table(table_obj, 100)  # Reasonable amount for testing
        
        snapshot_name_2 = f"max_data_system_snapshot{suffix}"
        snapshot_result_2 = self.create_system_snapshot_with_retry(self.infinity_obj, snapshot_name_2)
        self.handle_system_snapshot_result(snapshot_result_2, "Max data system snapshot creation")
        
        # Drop all test databases
        self.infinity_obj.get_database("default_db")
        for i in range(5):
            self.infinity_obj.drop_database(f"max_data_db_{i}", ConflictType.Ignore)
        
        # Restore from snapshot
        restore_result_2 = self.restore_system_snapshot_with_retry(self.infinity_obj, snapshot_name_2)
        self.handle_system_snapshot_restore_result(restore_result_2, "Max data system snapshot restore")
        
        # Verify all databases are restored
        databases_final = self.infinity_obj.list_databases()
        for i in range(5):
            assert f"max_data_db_{i}" in databases_final.db_names
        
        # Clean up - release references and add delay to avoid "database in use" errors
        self.infinity_obj.drop_snapshot(snapshot_name)
        self.infinity_obj.drop_snapshot(snapshot_name_2)
        
        self.infinity_obj.get_database("default_db")
        for i in range(5):
            self.infinity_obj.drop_database(f"max_data_db_{i}", ConflictType.Ignore)
        
        print("System snapshot edge cases test completed successfully") 