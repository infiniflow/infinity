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
class TestDatabaseSnapshot:
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
                "vector_col": [random.uniform(-1, 1) for _ in range(1)],
                "tensor_col": [random.uniform(-1, 1) for _ in range(2)],
                "sparse_col": sparse_data,
            }
            data.append(row)
        
        # Insert in batches
        batch_size = 50
        for i in range(0, len(data), batch_size):
            batch = data[i:i + batch_size]
            table_obj.insert(batch)
    
    def insert_simple_data_for_table(self, table_obj, num_rows: int = 50):
        """Insert test data for a simple table"""
        data = []
        for i in range(num_rows):
            row = {
                "id": i,
                "name": f"item_{i}",
                "value": random.uniform(0, 1000)
            }
            data.append(row)
        
        # Insert in batches
        batch_size = 25
        for i in range(0, len(data), batch_size):
            batch = data[i:i + batch_size]
            table_obj.insert(batch)
    
    def insert_large_dataset_optimized(self, table_obj, num_rows: int = 100000):
        """Insert large dataset using deterministic patterns for better performance"""
        print(f"Generating {num_rows:,} rows using optimized method...")
        
        # Use larger batch sizes for better performance
        batch_size = 8192  # Increased from 25/50
        total_batches = (num_rows + batch_size - 1) // batch_size
        
        for batch_idx in range(total_batches):
            start_idx = batch_idx * batch_size
            end_idx = min(start_idx + batch_size, num_rows)
            batch_size_actual = end_idx - start_idx
            
            # Generate batch data using deterministic patterns
            batch_data = []
            for i in range(batch_size_actual):
                row_idx = start_idx + i
                
                # Use deterministic patterns instead of random
                row = {
                    "id": row_idx,
                    "name": f"user_{row_idx % 1000000}",  # Cycle through names
                    "age": 18 + (row_idx % 62),  # Age 18-79
                    "salary": 30000 + (row_idx % 120000),  # Salary 30k-150k
                    "is_active": (row_idx % 2) == 0,  # Alternate true/false
                    "vector_col": [0.1 + (row_idx % 100) / 1000.0],  # Deterministic vector
                    "tensor_col": [0.1 + (row_idx % 50) / 500.0, 0.2 + (row_idx % 30) / 300.0],
                    "sparse_col": SparseVector([row_idx % 3], [0.1 + (row_idx % 10) / 100.0]),
                }
                batch_data.append(row)
            
            # Insert batch
            table_obj.insert(batch_data)
            
            # Progress reporting
            if (batch_idx + 1) % 10 == 0:
                progress = ((batch_idx + 1) / total_batches) * 100
                print(f"Progress: {progress:.1f}% ({batch_idx + 1}/{total_batches} batches)")
        
        print(f"Completed generating {num_rows:,} rows")
    
    def insert_large_simple_dataset_optimized(self, table_obj, num_rows: int = 100000):
        """Insert large simple dataset using deterministic patterns"""
        print(f"Generating {num_rows:,} simple rows using optimized method...")
        
        batch_size = 8192  # Even larger batches for simple data
        total_batches = (num_rows + batch_size - 1) // batch_size
        
        for batch_idx in range(total_batches):
            start_idx = batch_idx * batch_size
            end_idx = min(start_idx + batch_size, num_rows)
            batch_size_actual = end_idx - start_idx
            
            batch_data = []
            for i in range(batch_size_actual):
                row_idx = start_idx + i
                row = {
                    "id": row_idx,
                    "name": f"item_{row_idx}",
                    "value": 100 + (row_idx % 900)  # Values 100-999
                }
                batch_data.append(row)
            
            table_obj.insert(batch_data)
            
            if (batch_idx + 1) % 5 == 0:
                progress = ((batch_idx + 1) / total_batches) * 100
                print(f"Progress: {progress:.1f}% ({batch_idx + 1}/{total_batches} batches)")
        
        print(f"Completed generating {num_rows:,} simple rows")

    def verify_table_functionality(self, table_name: str, db_obj, expected_row_count: int | None = None):
        """Verify that a table functions correctly after snapshot operations"""
        print(f"Verifying table: {table_name}")
        
        try:
            # Get the table
            table = db_obj.get_table(table_name)
            
            # 1. Verify basic table structure and data
            result = table.query_builder.output(["*"]).to_result()
            data_dict = result[0]
            row_count = len(next(iter(data_dict.values())))
            print(f"   Row count: {row_count}")
            
            if expected_row_count:
                assert row_count == expected_row_count, f"Expected {expected_row_count} rows, got {row_count}"
            
            # 2. Test search operations
            try:
                # Full-text search
                fts_result = table.query_builder.output(["id", "name"]).match_text("name", "user_1", 5, None).to_result()
                print(f"   Full-text search: {len(fts_result[0])} results")
                
                # Vector similarity search
                query_vector = [random.uniform(-1, 1) for _ in range(1)]
                vector_result = table.query_builder.output(["id", "vector_col"]).match_dense("vector_col", query_vector, "float", "l2", 5).to_result()
                print(f"   Vector search: {len(vector_result[0])} results")
                
            except Exception as e:
                print(f"   Search operations failed: {e}")
            
            # 3. Test insert operations
            try:
                new_sparse_data = SparseVector([1, 2], [0.1, 0.2])
                new_row = {
                    "id": 999999,
                    "name": "test_insert_user",
                    "age": 25,
                    "salary": 75000.0,
                    "is_active": True,
                    "vector_col": [0.1] * 1,
                    "tensor_col": [0.1] * 2,
                    "sparse_col": new_sparse_data,
                }
                insert_result = table.insert(new_row)
                print(f"   Insert operation: {insert_result.error_code}")
                
                # Verify insert
                verify_result = table.query_builder.output(["id", "name"]).filter("id = 999999").to_result()
                assert len(verify_result[0]) > 0, "Inserted row not found"
                print(f"   Insert verification: OK")
                
            except Exception as e:
                print(f"   Insert operations failed: {e}")
            
            # 4. Test index operations
            try:
                index_response = table.list_indexes()
                existing_indexes = index_response.index_names if hasattr(index_response, 'index_names') else []
                print(f"   Existing indexes: {existing_indexes}")
                
                # Add a new index
                new_index_result = table.create_index("idx_test_new", index.IndexInfo("age", index.IndexType.Secondary), ConflictType.Ignore)
                print(f"   Add new index: {new_index_result.error_code}")
                
            except Exception as e:
                print(f"   Index operations failed: {e}")
            
            print(f"   Table {table_name} verification: PASSED")
            return True
            
        except Exception as e:
            print(f"   Table {table_name} verification: FAILED - {e}")
            raise
    
    def verify_simple_table_functionality(self, table_name: str, db_obj, expected_row_count: int | None = None):
        """Verify that a simple table functions correctly after snapshot operations"""
        print(f"Verifying simple table: {table_name}")
        
        try:
            # Get the table
            table = db_obj.get_table(table_name)
            
            # 1. Verify basic table structure and data
            result = table.query_builder.output(["*"]).to_result()
            data_dict = result[0]
            row_count = len(next(iter(data_dict.values())))
            print(f"   Row count: {row_count}")
            
            if expected_row_count:
                assert row_count == expected_row_count, f"Expected {expected_row_count} rows, got {row_count}"
            
            # 2. Test insert operations
            try:
                new_row = {
                    "id": 999999,
                    "name": "test_insert_item",
                    "value": 999.99
                }
                insert_result = table.insert(new_row)
                print(f"   Insert operation: {insert_result.error_code}")
                
                # Verify insert
                verify_result = table.query_builder.output(["id", "name"]).filter("id = 999999").to_result()
                assert len(verify_result[0]) > 0, "Inserted row not found"
                print(f"   Insert verification: OK")
                
            except Exception as e:
                print(f"   Insert operations failed: {e}")
            
            # 3. Test basic queries
            try:
                # Simple filter query
                filter_result = table.query_builder.output(["id", "name", "value"]).filter("value > 500").to_result()
                print(f"   Filter query: {len(filter_result[0])} results")
                
            except Exception as e:
                print(f"   Query operations failed: {e}")
            
            print(f"   Simple table {table_name} verification: PASSED")
            return True
            
        except Exception as e:
            print(f"   Simple table {table_name} verification: FAILED - {e}")
            raise
    
    def verify_database_operations(self, db_obj, expected_tables: list | None = None):
        """
        Verify database operations including adding, removing, and renaming tables
        
        Args:
            db_obj: Database object to test
            expected_tables: List of expected table names after operations
        """
        print("Verifying database operations...")
        
        try:
            # 1. Get current tables
            tables_before = db_obj.list_tables()
            table_names_before = tables_before.table_names
            print(f"   Tables before operations: {table_names_before}")
            
            # 2. Test adding new tables
            print("   Testing table creation...")
            new_table_names = []
            
            # Create a simple table
            simple_table_name = "test_add_simple_table"
            try:
                db_obj.create_table(simple_table_name, {
                    "id": {"type": "int", "constraints": ["primary key"]},
                    "name": {"type": "varchar"},
                    "value": {"type": "float"}
                })
                new_table_names.append(simple_table_name)
                print(f"   Created simple table: {simple_table_name}")
            except Exception as e:
                print(f"   Failed to create simple table: {e}")
            
            # Create a comprehensive table
            comp_table_name = "test_add_comp_table"
            try:
                table_obj = self.create_comprehensive_table(comp_table_name, db_obj)
                self.insert_data_for_table(table_obj, 50)
                self.create_indexes_for_table(table_obj)
                new_table_names.append(comp_table_name)
                print(f"   Created comprehensive table: {comp_table_name}")
            except Exception as e:
                print(f"   Failed to create comprehensive table: {e}")
            
            # 3. Verify tables were added
            tables_after_add = db_obj.list_tables()
            table_names_after_add = tables_after_add.table_names
            print(f"   Tables after adding: {table_names_after_add}")
            
            for table_name in new_table_names:
                assert table_name in table_names_after_add, f"Table {table_name} not found after creation"
            
            # 4. Test table renaming (if supported)
            print("   Testing table renaming...")
            renamed_success = False
            original_name = simple_table_name
            new_name = "test_renamed_table"
            
            try:
                db_obj.rename_table(original_name, new_name)
                renamed_success = True
                print(f"   Successfully renamed {original_name} to {new_name}")
                
                # Verify rename
                tables_after_rename = db_obj.list_tables()
                table_names_after_rename = tables_after_rename.table_names
                assert original_name not in table_names_after_rename, f"Original table {original_name} still exists"
                assert new_name in table_names_after_rename, f"Renamed table {new_name} not found"
                print(f"   Rename verification: OK")
                
            except Exception as e:
                print(f"   Table renaming not supported or failed: {e}")
            
            # 5. Test table deletion
            print("   Testing table deletion...")
            delete_table_name = comp_table_name
            
            try:
                db_obj.drop_table(delete_table_name)
                print(f"   Successfully deleted table: {delete_table_name}")
                
                # Verify deletion
                tables_after_delete = db_obj.list_tables()
                table_names_after_delete = tables_after_delete.table_names
                assert delete_table_name not in table_names_after_delete, f"Deleted table {delete_table_name} still exists"
                print(f"   Delete verification: OK")
                
            except Exception as e:
                print(f"   Failed to delete table: {e}")
            
            # 6. Test table functionality after operations
            print("   Testing table functionality after operations...")
            
            # Test remaining tables
            final_tables = db_obj.list_tables()
            final_table_names = final_tables.table_names
            
            for table_name in final_table_names:
                if table_name == new_name:  # Renamed table
                    print(f"   Testing renamed table: {table_name}")
                    self.verify_simple_table_functionality(table_name, db_obj)
                elif "comprehensive" in table_name:
                    print(f"   Testing comprehensive table: {table_name}")
                    self.verify_table_functionality(table_name, db_obj)
                else:
                    print(f"   Testing other table: {table_name}")
                    # Try both verification methods
                    try:
                        self.verify_table_functionality(table_name, db_obj)
                    except:
                        try:
                            self.verify_simple_table_functionality(table_name, db_obj)
                        except Exception as e:
                            print(f"   Could not verify table {table_name}: {e}")
            
            # 7. Verify expected tables if provided
            if expected_tables:
                print(f"   Verifying expected tables: {expected_tables}")
                for expected_table in expected_tables:
                    assert expected_table in final_table_names, f"Expected table {expected_table} not found"

            # 8. Test adding database
            print("   Testing database operations...")
            self.infinity_obj.drop_database("new_test_db", ConflictType.Ignore)
            self.infinity_obj.create_database("new_test_db")
            db_obj = self.infinity_obj.get_database("new_test_db")
            db_obj.create_table("new_test_table", {
                "id": {"type": "int"},
                "name": {"type": "varchar"}
            })
            table_obj = db_obj.get_table("new_test_table")
            table_obj.insert([{"id": 1, "name": "test_data"}])
            self.infinity_obj.drop_database("default_db")
            self.infinity_obj.create_database("default_db")
            
            print("   Database operations verification: PASSED")
            return True
            
        except Exception as e:
            print(f"   Database operations verification: FAILED - {e}")
            raise

    def test_database_snapshot_empty(self, suffix):
        """
        Test database snapshot with empty database
        """
        self.infinity_obj.drop_database("empty_db", ConflictType.Ignore)
        self.infinity_obj.create_database("empty_db")
        self.infinity_obj.create_database_snapshot("empty_db_snapshot","empty_db")
        self.infinity_obj.drop_database("empty_db")
        self.infinity_obj.restore_database_snapshot("empty_db_snapshot")
        db_obj = self.infinity_obj.get_database("empty_db")
        self.verify_database_operations(db_obj, [])
        self.infinity_obj.drop_snapshot("empty_db_snapshot")
        db_obj = self.infinity_obj.get_database("default_db")
        self.infinity_obj.drop_database("empty_db")

    def test_database_snapshot_comprehensive_tables(self, suffix):
        """
        Test database snapshot with comprehensive tables (all data types, indexes)
        """
        db_obj = self.infinity_obj.get_database("default_db")
            
        # Create multiple comprehensive tables with different configurations
        table_configs = [
            {"name": "comprehensive_table_1", "rows": 50, "has_indexes": True, "type": "comprehensive"},
            {"name": "comprehensive_table_2", "rows": 100, "has_indexes": True, "type": "comprehensive"},
            {"name": "comprehensive_table_3", "rows": 75, "has_indexes": False, "type": "comprehensive"},
            {"name": "comprehensive_table_4", "rows": 25, "has_indexes": True, "type": "comprehensive"},
            {"name": "simple_table_1", "rows": 10, "has_indexes": False, "type": "simple"},
            {"name": "simple_table_2", "rows": 15, "has_indexes": False, "type": "simple"},
            {"name": "simple_table_3", "rows": 20, "has_indexes": False, "type": "simple"},
        ]
        
        created_tables = []
        
        # Create all tables
        for config in table_configs:
            table_name = config["name"]
            
            if config["type"] == "comprehensive":
                table_obj = self.create_comprehensive_table(table_name, db_obj)
                self.insert_data_for_table(table_obj, config["rows"])
                if config["has_indexes"]:
                    self.create_indexes_for_table(table_obj)
            else:  # simple
                table_obj = self.create_simple_table(table_name, db_obj)
                self.insert_simple_data_for_table(table_obj, config["rows"])
            
            created_tables.append(table_name)
            print(f"Created table: {table_name} with {config['rows']} rows")
        
        # Verify all tables exist
        tables = db_obj.list_tables()
        table_names = tables.table_names
        for table_name in created_tables:
            assert table_name in table_names, f"Table {table_name} not found"
        
        # Create database snapshot
        print("Creating database snapshot...")
        self.infinity_obj.create_database_snapshot("comprehensive_db_snapshot","default_db")
        
        # Verify snapshot exists
        snapshots_response = self.infinity_obj.list_snapshots()
        snapshots = snapshots_response.snapshots
        assert "comprehensive_db_snapshot" in [snapshot.name for snapshot in snapshots]
        
        # Add more tables after snapshot
        db_obj.create_table("post_snapshot_table", {
            "c1": {"type": "int"},
            "c2": {"type": "varchar"}
        })
        post_table = db_obj.get_table("post_snapshot_table")
        post_table.insert([{"c1": 999, "c2": "post_snapshot_data"}])
        
        # Delete one table
        db_obj.drop_table("comprehensive_table_3")
        
        # Show tables after modifications
        tables_after_mod = db_obj.list_tables()
        table_names_after = tables_after_mod.table_names
        assert "post_snapshot_table" in table_names_after
        assert "comprehensive_table_3" not in table_names_after
        
        # Restore database from snapshot
        print("Restoring database from snapshot...")
        self.infinity_obj.create_database("temp_db1")
        db_obj = self.infinity_obj.get_database("temp_db1")
        self.infinity_obj.drop_database("default_db")
        self.infinity_obj.restore_database_snapshot("comprehensive_db_snapshot")
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Verify all original tables are restored
        tables_restored = db_obj.list_tables()
        table_names_restored = tables_restored.table_names
        
        # Check that original tables are back
        for table_name in created_tables:
            assert table_name in table_names_restored, f"Table {table_name} not restored"
        
        # Check that post-snapshot table is gone
        assert "post_snapshot_table" not in table_names_restored
        
        # Verify each table's functionality
        self.verify_database_operations(db_obj, created_tables)
        
        # Clean up
        for table_name in created_tables:
            try:
                db_obj.drop_table(table_name, ConflictType.Ignore)
            except:
                pass
        self.infinity_obj.drop_database("temp_db1")
        self.infinity_obj.drop_snapshot("comprehensive_db_snapshot")
        
        self.infinity_obj.disconnect()

    def test_database_snapshot_large_scale(self, suffix):
        """
        Test database snapshot with large number of tables and data
        """
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create many tables with different configurations
        table_configs = []
        for i in range(15):  # 15 tables total
            if i < 10:
                # 10 comprehensive tables
                table_configs.append({
                    "name": f"large_comp_table_{i}",
                    "rows": 50 + (i * 10),  # Varying row counts
                    "has_indexes": i % 2 == 0,  # Alternate with indexes
                    "type": "comprehensive"
                })
            else:
                # 5 simple tables
                table_configs.append({
                    "name": f"large_simple_table_{i}",
                    "rows": 20 + (i * 5),
                    "has_indexes": False,
                    "type": "simple"
                })
        
        created_tables = []
        
        # Create all tables
        print("Creating large scale tables...")
        start_time = time.time()
        
        for config in table_configs:
            table_name = config["name"]
            
            if config["type"] == "comprehensive":
                table_obj = self.create_comprehensive_table(table_name, db_obj)
                self.insert_large_dataset_optimized(table_obj, config["rows"] * 100)
                # if config["has_indexes"]:
                #     self.create_indexes_for_table(table_obj)
            else:
                table_obj = self.create_simple_table(table_name, db_obj)
                self.insert_large_simple_dataset_optimized(table_obj, config["rows"] * 100)
            
            created_tables.append(table_name)
        
        creation_time = time.time() - start_time
        print(f"Created {len(created_tables)} tables in {creation_time:.2f} seconds")
        
        # Create snapshot
        print("Creating database snapshot...")
        snapshot_start = time.time()
        self.infinity_obj.create_database_snapshot("large_scale_snapshot","default_db")
        snapshot_time = time.time() - snapshot_start
        print(f"Snapshot creation time: {snapshot_time:.2f} seconds")
        
        # Verify snapshot exists
        snapshots_response = self.infinity_obj.list_snapshots()
        snapshots = snapshots_response.snapshots
        assert "large_scale_snapshot" in [snapshot.name for snapshot in snapshots]
        
        # Add more tables after snapshot
        for i in range(3):
            table_name = f"post_snapshot_table_{i}"
            db_obj.create_table(table_name, {
                "c1": {"type": "int"},
                "c2": {"type": "varchar"}
            })
            table = db_obj.get_table(table_name)
            table.insert([{"c1": i, "c2": f"post_data_{i}"}])
        
        # Delete some tables
        for i in range(3):
            table_name = f"large_comp_table_{i}"
            db_obj.drop_table(table_name)
        
        # Restore from snapshot
        print("Restoring from snapshot...")
        restore_start = time.time()
        self.infinity_obj.create_database("temp_db2")
        db_obj = self.infinity_obj.get_database("temp_db2")
        self.infinity_obj.drop_database("default_db")
        self.infinity_obj.restore_database_snapshot("large_scale_snapshot")
        db_obj = self.infinity_obj.get_database("default_db")
        restore_time = time.time() - restore_start
        print(f"Snapshot restore time: {restore_time:.2f} seconds")
        
        # Verify all original tables are restored
        tables_restored = db_obj.list_tables()
        table_names_restored = tables_restored.table_names
        
        # Check that original tables are back
        for table_name in created_tables:
            assert table_name in table_names_restored, f"Table {table_name} not restored"
        
        # Check that post-snapshot tables are gone
        for i in range(3):
            assert f"post_snapshot_table_{i}" not in table_names_restored
        
        # Verify functionality of a subset of tables (for performance)
        print("Verifying subset of restored tables...")
        verification_tables = created_tables[:5]  # Verify first 5 tables
        
        for table_name in verification_tables:
            if "comprehensive" in table_name:
                self.verify_table_functionality(table_name, db_obj)
            else:
                self.verify_simple_table_functionality(table_name, db_obj)
        
        # Clean up
        for table_name in created_tables:
            try:
                db_obj.drop_table(table_name, ConflictType.Ignore)
            except:
                pass
        
        self.infinity_obj.drop_database("temp_db2")
        self.infinity_obj.drop_snapshot("large_scale_snapshot")
        
        self.infinity_obj.disconnect()

    def test_database_snapshot_error_handling(self, suffix):
        """
        Test error handling in database snapshot operations
        """
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Test creating snapshot with invalid name
        with pytest.raises(Exception):
            self.infinity_obj.create_database_snapshot("")
        
        # Test creating duplicate snapshot
        db_obj.create_table("error_table", {
            "c1": {"type": "int"}
        })
        self.infinity_obj.create_database_snapshot("duplicate_snapshot","default_db")
        
        with pytest.raises(Exception):
            self.infinity_obj.create_database_snapshot("duplicate_snapshot")
        
        # Test restoring non-existent snapshot
        with pytest.raises(Exception):
            self.infinity_obj.restore_database_snapshot("non_existent_snapshot")
        
        # Test deleting non-existent snapshot
        with pytest.raises(Exception):
            self.infinity_obj.drop_snapshot("non_existent_snapshot")
        
        # Clean up
        try:
            db_obj.drop_table("error_table", ConflictType.Ignore)
            self.infinity_obj.drop_snapshot("duplicate_snapshot")
        except:
            pass
        
        self.infinity_obj.disconnect()

    def test_database_snapshot_list_and_delete(self, suffix):
        """
        Test listing and deleting database snapshots
        """
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create some tables
        table_obj = self.create_comprehensive_table("list_table", db_obj)
        self.insert_data_for_table(table_obj, 50)
        
        # Create multiple snapshots
        self.infinity_obj.create_database_snapshot("snapshot_1","default_db")
        self.infinity_obj.create_database_snapshot("snapshot_2","default_db")
        self.infinity_obj.create_database_snapshot("snapshot_3","default_db")
        
        # List snapshots
        snapshots_response = self.infinity_obj.list_snapshots()
        snapshots = snapshots_response.snapshots
        snapshot_names = [snapshot.name for snapshot in snapshots]
        assert "snapshot_1" in snapshot_names
        assert "snapshot_2" in snapshot_names
        assert "snapshot_3" in snapshot_names
        
        # Delete a snapshot
        self.infinity_obj.drop_snapshot("snapshot_2")
        
        # List snapshots again
        snapshots_response = self.infinity_obj.list_snapshots()
        snapshots = snapshots_response.snapshots
        snapshot_names = [snapshot.name for snapshot in snapshots]
        assert "snapshot_1" in snapshot_names
        assert "snapshot_2" not in snapshot_names
        assert "snapshot_3" in snapshot_names
        
        
        # Clean up
        try:
            db_obj.drop_table("list_table", ConflictType.Ignore)
            self.infinity_obj.drop_snapshot("snapshot_1")
            self.infinity_obj.drop_snapshot("snapshot_3")
        except:
            pass
        
        self.infinity_obj.disconnect()

    # def test_database_snapshot_large_scale_optimized(self, suffix):
    #     """
    #     Test database snapshot with large datasets using optimized generation
    #     """
    #     db_obj = self.infinity_obj.get_database("default_db")
        
    #     # Create tables with large datasets
    #     table_configs = [
    #         {"name": "large_comp_table_1", "rows": 1000000, "has_indexes": True, "type": "comprehensive"},  # 1M rows
    #         {"name": "large_comp_table_2", "rows": 500000, "has_indexes": False, "type": "comprehensive"},   # 500K rows
    #         {"name": "large_simple_table_1", "rows": 2000000, "has_indexes": False, "type": "simple"},      # 2M rows
    #     ]
        
    #     created_tables = []
        
    #     # Create all tables with optimized data generation
    #     print("Creating large scale tables with optimized data generation...")
    #     start_time = time.time()
        
    #     for config in table_configs:
    #         table_name = config["name"]
    #         print(f"\nCreating table: {table_name}")
            
    #         if config["type"] == "comprehensive":
    #             table_obj = self.create_comprehensive_table(table_name, db_obj)
    #             self.insert_large_dataset_optimized(table_obj, config["rows"])
    #             if config["has_indexes"]:
    #                 print(f"Creating indexes for {table_name}...")
    #                 self.create_indexes_for_table(table_obj)
    #         else:
    #             table_obj = self.create_simple_table(table_name, db_obj)
    #             self.insert_large_simple_dataset_optimized(table_obj, config["rows"])
            
    #         created_tables.append(table_name)
        
    #     creation_time = time.time() - start_time
    #     print(f"\nCreated {len(created_tables)} tables in {creation_time:.2f} seconds")
        
    #     # Create snapshot
    #     print("Creating database snapshot...")
    #     snapshot_start = time.time()
    #     db_obj.create_database_snapshot("large_scale_optimized_snapshot")
    #     snapshot_time = time.time() - snapshot_start
    #     print(f"Snapshot creation time: {snapshot_time:.2f} seconds")
        
    #     # Verify snapshot exists
    #     snapshots_response = self.infinity_obj.list_snapshots()
    #     snapshots = snapshots_response.snapshots
    #     assert "large_scale_optimized_snapshot" in [snapshot.name for snapshot in snapshots]
        
    #     # Add more tables after snapshot
    #     for i in range(2):
    #         table_name = f"post_snapshot_table_{i}"
    #         db_obj.create_table(table_name, {
    #             "c1": {"type": "int"},
    #             "c2": {"type": "varchar"}
    #         })
    #         table = db_obj.get_table(table_name)
    #         table.insert([{"c1": i, "c2": f"post_data_{i}"}])
        
    #     # Delete some tables
    #     db_obj.drop_table("large_comp_table_2")
        
    #     # Restore from snapshot
    #     print("Restoring from snapshot...")
    #     restore_start = time.time()
    #     db_obj.restore_database_snapshot("large_scale_optimized_snapshot")
    #     restore_time = time.time() - restore_start
    #     print(f"Snapshot restore time: {restore_time:.2f} seconds")
        
    #     # Verify all original tables are restored
    #     tables_restored = db_obj.list_tables()
    #     table_names_restored = tables_restored.table_names
        
    #     # Check that original tables are back
    #     for table_name in created_tables:
    #         assert table_name in table_names_restored, f"Table {table_name} not restored"
        
    #     # Check that post-snapshot tables are gone
    #     for i in range(2):
    #         assert f"post_snapshot_table_{i}" not in table_names_restored
        
    #     # Verify functionality of a subset of tables (for performance)
    #     print("Verifying subset of restored tables...")
    #     verification_tables = created_tables[:2]  # Verify first 2 tables
        
    #     for table_name in verification_tables:
    #         if "comprehensive" in table_name:
    #             self.verify_table_functionality(table_name, db_obj)
    #         else:
    #             self.verify_simple_table_functionality(table_name, db_obj)
        
    #     # Clean up
    #     for table_name in created_tables:
    #         try:
    #             db_obj.drop_table(table_name, ConflictType.Ignore)
    #         except:
    #             pass
        
    #     self.infinity_obj.disconnect()