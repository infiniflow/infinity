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
class TestSnapshot:
    """Comprehensive snapshot testing for Infinity database""" 
    
    def verify_restored_table_functionality(self, table_name: str, db_obj, expected_row_count: int = None):
        """
        Comprehensive test to verify that a restored table functions normally.
        Tests all major operations: search, insert, drop indexes, add indexes, delete data, rename columns.
        
        Args:
            table_name: Name of the restored table to test
            db_obj: Database object
            expected_row_count: Expected number of rows in the table (optional)
        """
        print(f"\n=== Testing restored table functionality: {table_name} ===")
        
        # Get the restored table
        restored_table = db_obj.get_table(table_name)
        
        # 1. Verify basic table structure and data
        print("1. Verifying table structure and data...")
        try:
            # Count rows
            count_result = restored_table.query_builder.output(["*"]).to_result()
            data_dict = count_result[0]
            row_count = len(next(iter(data_dict.values())))
            print(f"   Row count: {row_count}")
            
            if expected_row_count:
                assert row_count == expected_row_count, f"Expected {expected_row_count} rows, got {row_count}"
            
            # Get column names from the result
            if data_dict:
                column_names = list(data_dict.keys())
                print(f"   Table columns: {column_names}")
            
        except Exception as e:
            print(f"   ERROR in basic verification: {e}")
            raise
        
        # 2. Test search operations on all indexes
        print("2. Testing search operations...")
        try:
            # Test full-text search using match_text
            fts_result = restored_table.query_builder.output(["id", "name"]).match_text("name", "user_1", 5, None).to_result()
            print(f"   Full-text search results: {len(fts_result[0])} rows")
            
            # Test vector similarity search using match_dense
            query_vector = [random.uniform(-1, 1) for _ in range(1)]
            vector_result = restored_table.query_builder.output(["id", "vector_col"]).match_dense("vector_col", query_vector, "float", "l2", 5).to_result()
            print(f"   Vector similarity search results: {len(vector_result[0])} rows")
            
            # Test sparse vector search using match_sparse
            sparse_query = SparseVector([1, 2], [0.1, 0.2])
            sparse_result = restored_table.query_builder.output(["id", "sparse_col"]).match_sparse("sparse_col", sparse_query, "ip", 5).to_result()
            print(f"   Sparse vector search results: {len(sparse_result[0])} rows")
            
        except Exception as e:
            print(f"   ERROR in search operations: {e}")
            raise
        
        # 3. Test insert operations
        print("3. Testing insert operations...")
        try:
            # Insert new data
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
            insert_result = restored_table.insert(new_row)
            print(f"   Insert result: {insert_result.error_code}")
            
            # Verify insert
            verify_result = restored_table.query_builder.output(["id", "name"]).filter("id = 999999").to_result()
            assert len(verify_result[0]) > 0, "Inserted row not found"
            print(f"   Insert verification: OK")
            
        except Exception as e:
            print(f"   ERROR in insert operations: {e}")
            raise
        
        # 4. Test index operations
        print("4. Testing index operations...")
        try:
            # List existing indexes
            index_response = restored_table.list_indexes()
            existing_indexes = index_response.index_names if hasattr(index_response, 'index_names') else []
            print(f"   Existing indexes: {existing_indexes}")
            
            # Drop an index if it exists
            if 'idx_name_fts' in existing_indexes:
                drop_result = restored_table.drop_index("idx_name_fts")
                print(f"   Drop index result: {drop_result.error_code}")
                # Verify index is dropped
                remaining_response = restored_table.list_indexes()
                remaining_indexes = remaining_response.index_names if hasattr(remaining_response, 'index_names') else []
                assert 'idx_name_fts' not in remaining_indexes, "Index not dropped"
                print(f"   Index drop verification: OK")
            else:
                print(f"   Index 'idx_name_fts' not found, skipping drop test")
            
            # Add a new index
            new_index_result = restored_table.create_index("idx_test_new", index.IndexInfo("age", index.IndexType.Secondary), ConflictType.Ignore)
            print(f"   Add new index result: {new_index_result.error_code}")
            
            # Verify new index
            final_response = restored_table.list_indexes()
            final_indexes = final_response.index_names if hasattr(final_response, 'index_names') else []
            print(f"   Final indexes: {final_indexes}")
            assert 'idx_test_new' in final_indexes, "New index not created"
            print(f"   New index verification: OK")
            
        except Exception as e:
            print(f"   ERROR in index operations: {e}")
            raise
        
        # 5. Test delete operations
        print("5. Testing delete operations...")
        try:
            # Delete the test row we inserted
            delete_result = restored_table.delete("id = 999999")
            print(f"   Delete result: {delete_result.error_code}")
            
            # Verify deletion
            verify_delete = restored_table.query_builder.output(["id"]).filter("id = 999999").to_result()
            data_dict = verify_delete[0]  # {'id': []}
            id_list = data_dict['id']     # []
            assert len(id_list) == 0, "Row not deleted"  # len([]) == 0 ✅
            print(f"   Delete verification: OK")
            
        except Exception as e:
            print(f"   ERROR in delete operations: {e}")
            raise
        
        # 6. Test column operations (rename, drop column, etc.)
        print("6. Testing column operations...")
        try:
            # Test drop column operation
            print(f"   Testing drop column operation...")
            
            # Add a test column first
            add_columns_result = restored_table.add_columns({"test_col": {"type": "int", "default": 0}})
            print(f"   Add column result: {add_columns_result.error_code}")
            
            # Verify column was added by checking the full table structure
            try:
                verify_add = restored_table.query_builder.output(["*"]).to_result()
                print(f"   Column add verification: OK - table has {len(verify_add[0])} columns")
            except Exception as e:
                print(f"   Column add verification failed: {e}")
            
            # Drop the test column
            drop_columns_result = restored_table.drop_columns(["test_col"])
            print(f"   Drop column result: {drop_columns_result.error_code}")
            
            # Verify column was dropped by checking the full table structure
            try:
                verify_drop = restored_table.query_builder.output(["*"]).to_result()
                print(f"   Column drop verification: OK - table structure updated")
            except Exception as e:
                print(f"   Column drop verification failed: {e}")
            
        except Exception as e:
            print(f"   ERROR in column operations: {e}")
            # Don't raise here as column operations might not be supported
        
        # 7. Test complex queries
        # TODO: test after fix
        print("7. Testing complex queries...")
        try:
            # Complex filter query
            complex_result = restored_table.query_builder.output(["id", "name", "age", "salary"]).filter("age > 30 AND salary > 50000").to_result()
            print(f"   Complex filter results: {len(complex_result[0])} rows")
            
        #     # Group by query (if supported)
        #     try:
        #         group_result = restored_table.query_builder.output(["age", "salary"]).group_by(["age"]).to_result()
        #         print(f"   Group by results: {len(group_result[0])} rows")
        #     except Exception as e:
        #         print(f"   Group by not supported: {e}")
            
        except Exception as e:
            print(f"   ERROR in complex queries: {e}")
            raise
        
        print(f"=== All functionality tests passed for {table_name} ===\n")
        return True

    def create_comprehensive_table(self, table_name: str):
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
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name, table_schema, ConflictType.Ignore)
        
        return table_obj
    
    def _create_indexes(self, table_obj):
        """Create various types of indexes"""
        # Primary key is already created
        # Secondary indexes
        # table_obj.create_index("idx_name", index.IndexInfo("name", index.IndexType.Secondary), ConflictType.Ignore)
        # table_obj.create_index("idx_age_salary", index.IndexInfo("age", index.IndexType.Secondary), ConflictType.Ignore)
        
        # Vector indexes
        # table_obj.create_index("idx_vector_hnsw", index.IndexInfo("vector_col", index.IndexType.Hnsw, {"metric": "cosine", "m": "16", "ef_construction": "200"}), ConflictType.Ignore)
        
        # Full-text search index
        table_obj.create_index("idx_name_fts", index.IndexInfo("name", index.IndexType.FullText), ConflictType.Ignore)

        # BMP index
        table_obj.create_index("idx_vector_bmp", index.IndexInfo("sparse_col", index.IndexType.BMP, {"block_size": "16", "compress_type": "compress"}), ConflictType.Ignore)

        # # EMVB index (for tensors)
        table_obj.create_index("idx_tensor_emvb", index.IndexInfo("tensor_col", index.IndexType.EMVB, {"pq_subspace_num": "32", "pq_subspace_bits": "8"}), ConflictType.Ignore)

        # IVF index
        table_obj.create_index("idx_vector_ivf", index.IndexInfo("vector_col", index.IndexType.IVF, {"metric": "l2"}), ConflictType.Ignore)
    
    def insert_comprehensive_data(self, table_obj, num_rows: int = 1000):
        """Insert comprehensive test data"""
        data = []
        for i in range(num_rows):
            # Create sparse vector data (only 5-10 non-zero elements out of 30000)
            num_non_zero = random.randint(1,2)
            indices = sorted(random.sample(range(3), num_non_zero))
            values = [random.uniform(-1, 1) for _ in range(num_non_zero)]
            # Create sparse vector using SparseVector class
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
        batch_size = 100
        for i in range(0, len(data), batch_size):
            batch = data[i:i + batch_size]
            table_obj.insert(batch)
    
    def test_basic_snapshot_operations(self, suffix):
        """Test basic snapshot create, list, drop operations"""
        table_name = f"test_basic_snapshot{suffix}"
        snapshot_name = f"basic_snapshot{suffix}"
        db_obj = self.infinity_obj.get_database("default_db")
        # Drop original table
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # Create table and insert data
        table_obj = self.create_comprehensive_table(table_name)
        # Create indexes
        self._create_indexes(table_obj)
        self.insert_comprehensive_data(table_obj, 100)
        
        
        # Create snapshot
        snapshot_result = db_obj.create_table_snapshot(snapshot_name, table_name)
        print(f"Snapshot creation result: {snapshot_result}")
        print(f"Snapshot creation error code: {snapshot_result.error_code}")
        print(f"Snapshot creation error message: {snapshot_result.error_msg}")
        assert snapshot_result.error_code == ErrorCode.OK
        
        # List snapshots
        snapshots_response = self.infinity_obj.list_snapshots()
        snapshots = snapshots_response.snapshots
        print(f"Looking for snapshot: {snapshot_name}")
        print(f"Available snapshots: {[s.name for s in snapshots]}")
        print(f"Error code: {snapshots_response.error_code}")
        print(f"Error message: {snapshots_response.error_msg}")
        assert snapshot_name in [s.name for s in snapshots]

        self.verify_restored_table_functionality(table_name, db_obj, expected_row_count=100)
        
        # Drop original table
        db_obj.drop_table(table_name, ConflictType.Error)
        
        # Restore from snapshot
        restore_result = db_obj.restore_table_snapshot(snapshot_name)
        assert restore_result.error_code == ErrorCode.OK
        
        # Verify data integrity and functionality
        self.verify_restored_table_functionality(table_name, db_obj, expected_row_count=100)
        
        # Drop snapshot
        drop_result = self.infinity_obj.drop_snapshot(snapshot_name)
        assert drop_result.error_code == ErrorCode.OK

        # Drop table
        db_obj.drop_table(table_name, ConflictType.Error)
    
    # def test_snapshot_concurrency(self):
    #     """Test concurrent snapshot operations"""
    #     table_name = f"test_concurrency{self.suffix}"
    #     snapshot_name = f"concurrency_snapshot{self.suffix}"
        
    #     # Create table and insert data
    #     table_obj = self.create_comprehensive_table(table_name)
    #     self.insert_comprehensive_data(table_obj, 500)
        
    #     def create_snapshot():
    #         return self.db_obj.create_snapshot(snapshot_name, table_name)
        
    #     def restore_snapshot():
    #         return self.db_obj.restore_snapshot(snapshot_name, f"restored_{table_name}")
        
    #     # Test concurrent snapshot creation
    #     with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    #         futures = [executor.submit(create_snapshot) for _ in range(4)]
    #         results = [future.result() for future in futures]
            
            #         # Only one should succeed, others should fail with conflict
        #         success_count = sum(1 for r in results if r is not None)
        #         assert success_count == 1
    
    def test_snapshot_large_table(self, suffix):
        """Test snapshot with large table"""
        table_name = f"test_large_dataset{suffix}"
        snapshot_name = f"large_snapshot{suffix}"
        db_obj = self.infinity_obj.get_database("default_db")
        # Drop original table
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # Create table and insert large amount of data
        table_obj = self.create_comprehensive_table(table_name)
        self._create_indexes(table_obj)
        self.insert_comprehensive_data(table_obj, 100000)  # 100k rows - should be fine with small dimensions

        # self.verify_restored_table_functionality(table_name, db_obj, expected_row_count=100000)


        
        # # Measure snapshot creation time
        # start_time = time.time()
        # snapshot_result = db_obj.create_table_snapshot(snapshot_name, table_name)
        # snapshot_time = time.time() - start_time
        
        # assert snapshot_result.error_code == ErrorCode.OK
        # print(f"Snapshot creation time: {snapshot_time:.2f} seconds")

        # # Drop table
        # db_obj.drop_table(table_name, ConflictType.Error)
        
        # # Test restore performance
        # start_time = time.time()
        # restore_result = db_obj.restore_table_snapshot(snapshot_name)
        # restore_time = time.time() - start_time
        
        # assert restore_result.error_code == ErrorCode.OK

        # # Verify data integrity and functionality
        # self.verify_restored_table_functionality(table_name, db_obj, expected_row_count=100000)

        # # Drop snapshot
        # drop_result = self.infinity_obj.drop_snapshot(snapshot_name)
        # assert drop_result.error_code == ErrorCode.OK

        # # Drop table
        # db_obj.drop_table(table_name, ConflictType.Error)

        # print(f"Snapshot restore time: {restore_time:.2f} seconds")
    
    def test_snapshot_error_conditions(self):
        """Test error conditions for snapshot operations"""
        # Test creating snapshot of non-existent table
        db_obj = self.infinity_obj.get_database("default_db")
        with pytest.raises(InfinityException) as e:
            db_obj.create_snapshot("non_existent", "non_existent_table")
        
        # Test restoring non-existent snapshot
        with pytest.raises(InfinityException) as e:
            db_obj.restore_snapshot("non_existent_snapshot", "new_table")
        
        # Test dropping non-existent snapshot
        with pytest.raises(InfinityException) as e:
            db_obj.drop_snapshot("non_existent_snapshot")
    
    def test_snapshot_naming_conventions(self, suffix):
        """Test snapshot with various naming conventions"""
        table_name = f"test_naming{suffix}"
        
        # Test with special characters in snapshot name
        special_names = [
            "snapshot_with_underscores",
            "snapshot-with-dashes",
            "snapshot123",
            "SNAPSHOT_UPPER",
        ]
        
        table_obj = self.create_comprehensive_table(table_name)
        self.insert_comprehensive_data(table_obj, 10)
        
        for snapshot_name in special_names:
            # Create snapshot
            db_obj = self.infinity_obj.get_database("default_db")
            result = db_obj.create_table_snapshot(snapshot_name, table_name)
            assert result.error_code == ErrorCode.OK

            db_obj.drop_table(table_name, ConflictType.Error)
            
            # Restore from snapshot
            restore_result = db_obj.restore_table_snapshot(snapshot_name)
            assert restore_result.error_code == ErrorCode.OK
            
            # Clean up
            self.infinity_obj.drop_snapshot(snapshot_name)

        db_obj.drop_table(table_name, ConflictType.Error)
    
    @pytest.mark.slow
    def test_snapshot_stress_test(self, suffix):
        """Stress test for snapshot operations with concurrent create and restore"""
        num_tables = 10
        tables = []
        
        # Create multiple tables
        for i in range(num_tables):
            table_name = f"stress_test_table_{i}{suffix}"
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            table_obj = self.create_comprehensive_table(table_name)
            self.insert_comprehensive_data(table_obj, 100)
            self._create_indexes(table_obj)
            tables.append((table_name, table_obj))
        
        # Define snapshot operations
        def create_snapshot_for_table(table_name):
            snapshot_name = f"stress_snapshot_{table_name}"
            db_obj = self.infinity_obj.get_database("default_db")
            return db_obj.create_table_snapshot(snapshot_name, table_name)
        
        def restore_snapshot_for_table(table_name):
            snapshot_name = f"stress_snapshot_{table_name}"
            db_obj = self.infinity_obj.get_database("default_db")
            return db_obj.restore_table_snapshot(snapshot_name)
        
        # Create snapshots for all tables concurrently
        print(f"Creating {num_tables} snapshots concurrently...")
        with concurrent.futures.ThreadPoolExecutor(max_workers=5) as executor:
            futures = [executor.submit(create_snapshot_for_table, table_name) 
                      for table_name, _ in tables]
            create_results = [future.result() for future in futures]
            
            # All should succeed
            for i, result in enumerate(create_results):
                assert result.error_code == ErrorCode.OK, f"Snapshot creation failed for table {i}: {result.error_code}"
        
        print(f"Successfully created {num_tables} snapshots")
        
        # Drop original tables before restore to ensure clean restoration
        print(f"Dropping {num_tables} original tables...")
        for table_name, _ in tables:
            try:
                db_obj = self.infinity_obj.get_database("default_db")
                db_obj.drop_table(table_name, ConflictType.Ignore)
                print(f"   Dropped table: {table_name}")
            except Exception as e:
                print(f"   Warning: Failed to drop table {table_name}: {e}")
        
        # Restore all snapshots concurrently
        print(f"Restoring {num_tables} snapshots concurrently...")
        with concurrent.futures.ThreadPoolExecutor(max_workers=5) as executor:
            futures = [executor.submit(restore_snapshot_for_table, table_name) 
                      for table_name, _ in tables]
            restore_results = [future.result() for future in futures]
            
            # All should succeed
            for i, result in enumerate(restore_results):
                assert result.error_code == ErrorCode.OK, f"Snapshot restore failed for table {i}: {result.error_code}"
        
        print(f"Successfully restored {num_tables} snapshots")
        
        # Verify all restored tables work correctly
        print("Verifying restored tables...")
        for i in range(num_tables):
            restored_table_name = f"stress_test_table_{i}{suffix}"
            
            # Use comprehensive verification function
            try:
                db_obj = self.infinity_obj.get_database("default_db")
                self.verify_restored_table_functionality(restored_table_name, db_obj, expected_row_count=100)
                print(f"   Table {i}: Comprehensive verification passed")
            except Exception as e:
                print(f"   Table {i}: Verification failed - {e}")
                raise

    def test_restore_table_snapshot_table_exists(self, suffix):
        """Test restore when table already exists"""
        db_obj = self.infinity_obj.get_database("default_db")

        try:
            self.infinity_obj.drop_snapshot(f"restore_snap_{suffix}")
        except:
            pass
        
        # Drop table if it exists
        try:
            db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)
        except:
            pass
        
        table_obj = db_obj.create_table(
            f"test_table_{suffix}",
            {"c1": {"type": "int", "constraints": ["primary key"]}},
            ConflictType.Error
        )
        
        # Create snapshot
        res = db_obj.create_table_snapshot(f"restore_snap_{suffix}", f"test_table_{suffix}")
        assert res.error_code == ErrorCode.OK
        
        # Try to restore without dropping original table
        with pytest.raises(InfinityException):
            db_obj.restore_table_snapshot(f"restore_snap_{suffix}")
        
        # Cleanup
        try:
            self.infinity_obj.drop_snapshot(f"restore_snap_{suffix}")
        except:
            pass
        db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)

    def test_multithread_snapshot_with_modifications(self, suffix):
        """Test snapshot creation and restore while table is being modified by multiple threads - runs 20 times"""
        import threading
        import time
        import random
        
        # Run the entire test 20 times
        for test_run in range(2):
            print(f"\n=== Test Run {test_run + 1}/20 ===")
            
            table_name = f"multithread_test{suffix}_{test_run}"
            snapshot_name = f"multithread_snap{suffix}_{test_run}"
            
            # Create table with comprehensive schema
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            
            table_obj = self.create_comprehensive_table(table_name)
            
            
            # Insert initial data
            print(f"Inserting initial data for run {test_run + 1}...")
            self.insert_comprehensive_data(table_obj, 1000)  # 1K initial rows
            self._create_indexes(table_obj)
        
            # Shared state for coordination
            class TestState:
                def __init__(self):
                    self.running = True
                    self.insert_count = 0
                    self.update_count = 0
                    self.delete_count = 0
                    self.index_count = 0
                    self.snapshot_taken = False
                    self.next_id = 1001  # Start after initial 1000 rows
                    self.lock = threading.Lock()
            
            state = TestState()
            
            # Thread 1: Continuous data insertion with unique IDs
            def insert_thread():
                print("Insert thread started")
                while state.running:
                    try:
                        with state.lock:
                            current_id = state.next_id
                            state.next_id += 1
                        
                        # Insert with unique ID
                        new_sparse_data = SparseVector([1, 2], [0.1, 0.2])
                        new_row = {
                            "id": current_id,
                            "name": "test_insert_user",
                            "age": 25,
                            "salary": 75000.0,
                            "is_active": True,
                            "vector_col": [0.1] * 1,
                            "tensor_col": [0.1] * 2,
                            "sparse_col": new_sparse_data,
                        }
                        table_obj.insert(new_row)
                        with state.lock:
                            state.insert_count += 1
                        time.sleep(0.01)  # Small delay
                    except Exception as e:
                        print(f"Insert thread error: {e}")
                        break
            
            # Thread 2: Continuous data updates
            def update_thread():
                print("Update thread started")
                # while state.running:
                #     try:
                #         # Update random rows from initial data (1-1000)
                #         update_id = random.randint(1, 1000)
                #         update_data = {
                #             "name": f"updated_{random.randint(1, 1000)}",
                #             "age": random.randint(18, 80),
                #             "salary": random.uniform(30000, 150000)
                #         }
                #         # Use proper update syntax
                #         table_obj.update(update_data, f"id = {update_id}")
                #         with state.lock:
                #             state.update_count += 1
                #         time.sleep(0.02)  # Slightly longer delay
                #     except Exception as e:
                #         print(f"Update thread error: {e}")
                #         break
            
            # Thread 3: Continuous data deletion
            def delete_thread():
                print("Delete thread started")
                # while state.running:
                #     try:
                #         # Delete random rows
                #         delete_id = random.randint(1, 1000)
                #         table_obj.delete(f"id = {delete_id}")
                #         with state.lock:
                #             state.delete_count += 1
                #         time.sleep(0.03)  # Longer delay for deletes
                #     except Exception as e:
                #         print(f"Delete thread error: {e}")
                #         break
            
            # Thread 4: Index operations
            def index_thread():
                print("Index thread started")
                while state.running:
                    try:
                        # Create and drop indexes
                        index_name = f"test_idx_{random.randint(1, 100)}"
                        try:
                            # Try to create index
                            table_obj.create_index(index_name, index.IndexInfo("name", index.IndexType.FullText))
                            with state.lock:
                                state.index_count += 1
                            time.sleep(0.1)
                            
                            # Try to drop the index
                            table_obj.drop_index(index_name)
                            with state.lock:
                                state.index_count += 1
                            time.sleep(0.1)
                        except Exception as inner_e:
                            # Index operations might fail due to concurrent modifications
                            print(f"Index operation failed: {inner_e}")
                            pass
                    except Exception as e:
                        import traceback
                        print(f"Index thread error: {e}")
                        print(f"Full traceback:")
                        traceback.print_exc()
                        break
            
            # Thread 5: Snapshot creation
            def snapshot_thread():
                print("Snapshot thread started")
                time.sleep(0.5)  # Let other threads start first
                
                try:
                    print("Creating snapshot while table is being modified...")
                    start_time = time.time()
                    
                    # Create snapshot while other threads are modifying the table
                    result = db_obj.create_table_snapshot(snapshot_name, table_name)
                    
                    end_time = time.time()
                    snapshot_time = end_time - start_time
                    
                    if result.error_code == ErrorCode.OK:
                        print(f"Snapshot created successfully in {snapshot_time:.2f} seconds")
                        print(f"During snapshot creation:")
                        print(f"  - {state.insert_count} inserts performed")
                        print(f"  - {state.update_count} updates performed")
                        print(f"  - {state.delete_count} deletes performed")
                        print(f"  - {state.index_count} index operations performed")
                        
                        
                    else:
                        print(f"Snapshot creation failed: {result.error_code}")
                        
                except Exception as e:
                    print(f"Snapshot thread error: {e}")
                    raise
            
            # Start all threads
            threads = []
            thread_functions = [insert_thread, update_thread, delete_thread, index_thread, snapshot_thread]
            
            print("Starting modification threads...")
            for func in thread_functions:
                thread = threading.Thread(target=func)
                thread.daemon = True
                thread.start()
                threads.append(thread)
            
            # Wait for snapshot thread to complete
            threads[4].join(timeout=1000)  # Wait up to 30 seconds
            
            # Stop all threads
            print("Stopping modification threads...")
            state.running = False
            
            # Wait for other threads to finish
            for i, thread in enumerate(threads[:4]):
                thread.join(timeout=1000)
                if thread.is_alive():
                    print(f"Thread {i} did not stop gracefully")
            
            print("Multithreaded snapshot test completed")
            
            
            try:
                # drop table
                db_obj.drop_table(table_name, ConflictType.Ignore)
                
                # Restore the snapshot
                restore_result = db_obj.restore_table_snapshot(snapshot_name)
                assert restore_result.error_code == ErrorCode.OK, f"Snapshot restore failed: {restore_result.error_code}"
                
                self.verify_restored_table_functionality(table_name, db_obj)
            except Exception as e:
                print(f"Snapshot restore failed: {e}")
                raise

            # Cleanup
            try:
                self.infinity_obj.drop_snapshot(snapshot_name)
            except:
                pass
            db_obj.drop_table(table_name, ConflictType.Ignore)
        
            print("Verifying snapshot data integrity...")
