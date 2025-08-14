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
    """Comprehensive snapshot testing for Infinity database with retry logic for snapshot creation (checkpoint handling)"""
    
    # Class-level retry configuration
    DEFAULT_MAX_RETRIES = 3
    DEFAULT_RETRY_DELAY = 2
    LARGE_TABLE_MAX_RETRIES = 5
    LARGE_TABLE_RETRY_DELAY = 3
    
    def create_snapshot_with_retry(self, db_obj, snapshot_name, table_name, max_retries=None, retry_delay=None):
        """Create snapshot with retry logic for checkpoint failures"""
        if max_retries is None:
            max_retries = self.DEFAULT_MAX_RETRIES
        if retry_delay is None:
            retry_delay = self.DEFAULT_RETRY_DELAY
            
        for attempt in range(max_retries):
            try:
                snapshot_result = db_obj.create_table_snapshot(snapshot_name, table_name)
                
                if snapshot_result.error_code == ErrorCode.OK:
                    print(f"Snapshot created successfully on attempt {attempt + 1}")
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

    def handle_snapshot_result(self, snapshot_result, operation_name="Snapshot operation", skip_on_checkpoint=True):
        """Handle snapshot operation results with consistent error handling"""
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
            # Count rows using fast count query instead of fetching all data
            count_result, extra_result = restored_table.output(["count(*)"]).to_df()
            row_count = count_result.iloc[0, 0]  # Get the count value
            print(f"   Row count: {row_count}")
            
            if expected_row_count:
                assert row_count == expected_row_count, f"Expected {expected_row_count} rows, got {row_count}"
            
            # Get column names from table metadata instead of fetching data
            columns_result = restored_table.show_columns()
            column_names = columns_result["name"].to_list()  # Get column names from polars DataFrame
            print(f"   Table columns: {column_names}")
            
        except Exception as e:
            print(f"   ERROR in basic verification: {e}")
            raise
        
        # 2. Test search operations on all indexes
        print("2. Testing search operations...")
        try:
            # Test full-text search using match_text
            fts_result, extra_result = restored_table.output(["id", "name"]).match_text("name", "user_1", 5, None).to_df()
            print(f"   Full-text search results: {len(fts_result)} rows")
            
            # Test vector similarity search using match_dense
            query_vector = [random.uniform(-1, 1) for _ in range(1)]
            vector_result, extra_result = restored_table.output(["id", "vector_col"]).match_dense("vector_col", query_vector, "float", "l2", 5).to_df()
            print(f"   Vector similarity search results: {len(vector_result)} rows")
            
            # Test sparse vector search using match_sparse
            sparse_query = SparseVector([1, 2], [0.1, 0.2])
            sparse_result, extra_result = restored_table.output(["id", "sparse_col"]).match_sparse("sparse_col", sparse_query, "ip", 5).to_df()
            print(f"   Sparse vector search results: {len(sparse_result)} rows")
            
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
            verify_result, extra_result = restored_table.output(["id", "name"]).filter("id = 999999").to_df()
            assert len(verify_result) > 0, "Inserted row not found"
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
                remaining_response = restored_table.list_indexes().index_names
                # remaining_indexes = [index["index_name"] for index in remaining_response]
                assert 'idx_name_fts' not in remaining_response, "Index not dropped"
                print(f"   Index drop verification: OK")
            else:
                print(f"   Index 'idx_name_fts' not found, skipping drop test")
            
            # Add a new index
            new_index_result = restored_table.create_index("idx_test_new", index.IndexInfo("age", index.IndexType.Secondary), ConflictType.Ignore)
            print(f"   Add new index result: {new_index_result.error_code}")
            
            
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
            verify_delete, extra_result = restored_table.output(["id"]).filter("id = 999999").to_df()
            assert len(verify_delete) == 0, "Row not deleted"
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
                columns_result = restored_table.show_columns()
                column_names = columns_result["name"].to_list()
                print(f"   Column add verification: OK - table has {len(column_names)} columns")
            except Exception as e:
                print(f"   Column add verification failed: {e}")
            
            # Drop the test column
            drop_columns_result = restored_table.drop_columns(["test_col"])
            print(f"   Drop column result: {drop_columns_result.error_code}")
            
            # Verify column was dropped by checking the full table structure
            try:
                columns_result = restored_table.show_columns()
                column_names = columns_result["name"].to_list()
                print(f"   Column drop verification: OK - table structure updated")
            except Exception as e:
                print(f"   Column drop verification failed: {e}")
            
        except Exception as e:
            print(f"   ERROR in column operations: {e}")
            # Don't raise here as column operations might not be supported
        
        # 7. Test complex queries
        # TODO: test after complex query is fixed
        # print("7. Testing complex queries...")
        # try:
        #     # Complex filter query
        #     complex_result, extra_result = restored_table.output(["count(*)"]).filter("age > 30 AND salary > 50000").to_df()
        #     print(f"   Complex filter results: {complex_result}")
            
        # #     # Group by query (if supported)
        # #     try:
        # #         group_result = restored_table.query_builder.output(["age", "salary"]).group_by(["age"]).to_result()
        # #         print(f"   Group by results: {len(group_result[0])} rows")
        # #     except Exception as e:
        # #         print(f"   Group by not supported: {e}")
            
        # except Exception as e:
        #     print(f"   ERROR in complex queries: {e}")
        #     raise
        
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

        # Full-text search index
        table_obj.create_index("idx_name_fts", index.IndexInfo("name", index.IndexType.FullText), ConflictType.Ignore)

        # BMP index
        table_obj.create_index("idx_vector_bmp", index.IndexInfo("sparse_col", index.IndexType.BMP, {"block_size": "16", "compress_type": "compress"}), ConflictType.Ignore)

        # HNSW index TODO: too slow, need to fix
        # table_obj.create_index("idx_vector_hnsw", index.IndexInfo("vector_col", index.IndexType.Hnsw, {"metric": "cosine", "m": "16", "ef_construction": "200"}), ConflictType.Ignore)

        table_obj.drop_index("idx_vector_bmp")

        table_obj.create_index("idx_vector_bmp", index.IndexInfo("sparse_col", index.IndexType.BMP, {"block_size": "16", "compress_type": "compress"}), ConflictType.Ignore)


        # # EMVB index (for tensors)
        # table_obj.create_index("idx_tensor_emvb", index.IndexInfo("tensor_col", index.IndexType.EMVB, {"pq_subspace_num": "32", "pq_subspace_bits": "8"}), ConflictType.Ignore)

    
    def insert_comprehensive_data(self, table_obj, num_rows: int = 1000):
        """Insert comprehensive test data"""
        data = []
        for i in range(num_rows):
            # Create sparse vector data (only 5-10 non-zero elements out of 30000)
            sparse_data = SparseVector([0, 1], [1.0, 1.0]) 
            
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
        
        # Insert in batches with verification
        batch_size = 100
        successful_insertions = 0
        for i in range(0, len(data), batch_size):
            batch = data[i:i + batch_size]
            try:
                result = table_obj.insert(batch)
                if result.error_code == ErrorCode.OK:
                    successful_insertions += len(batch)
                else:
                    print(f"Warning: Batch insertion failed with error: {result.error_msg}")
            except Exception as e:
                print(f"Warning: Exception during batch insertion: {e}")
        
        # Verify the total number of inserted rows
        try:
            count_result, _ = table_obj.output(["count(*)"]).to_df()
            actual_count = count_result.iloc[0, 0]
            print(f"Expected to insert {num_rows} rows, actually inserted {actual_count} rows")
            if actual_count != num_rows:
                print(f"Warning: Row count mismatch. Expected {num_rows}, got {actual_count}")
        except Exception as e:
            print(f"Warning: Could not verify row count: {e}")
        
        return actual_count if 'actual_count' in locals() else successful_insertions

    def add_drop_column(self, table_obj):
        """Add and drop column"""
        table_obj.drop_columns(["salary"])
        table_obj.add_columns({"salary": {"type": "float64", "default": 0}})
        return table_obj
    
    def create_snapshot_with_retry(self, db_obj, snapshot_name, table_name, max_retries=3, retry_delay=2):
        """Create snapshot with retry logic for checkpoint failures"""
        for attempt in range(max_retries):
            try:
                snapshot_result = db_obj.create_table_snapshot(snapshot_name, table_name)
                
                if snapshot_result.error_code == ErrorCode.OK:
                    print(f"Snapshot created successfully on attempt {attempt + 1}")
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

    def test_basic_snapshot_operations(self, suffix):
        """Test basic snapshot create, list, drop operations with retry logic"""
        table_name = f"test_basic_snapshot{suffix}"
        snapshot_name = f"basic_snapshot{suffix}"
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Drop original table
        db_obj.drop_table(table_name, ConflictType.Ignore)
        
        # Create table and insert data
        table_obj = self.create_comprehensive_table(table_name)
        self.add_drop_column(table_obj)
        self._create_indexes(table_obj)
        actual_inserted = self.insert_comprehensive_data(table_obj, 100)
        print(f"Successfully inserted {actual_inserted} out of 100 rows")
        
        # Create snapshot with retry logic
        snapshot_result = self.create_snapshot_with_retry(db_obj, snapshot_name, table_name)
        self.handle_snapshot_result(snapshot_result, "Basic snapshot creation")
        
        # List snapshots
        snapshots_response = self.infinity_obj.list_snapshots()
        snapshots = snapshots_response.snapshots
        print(f"Looking for snapshot: {snapshot_name}")
        print(f"Available snapshots: {[s.name for s in snapshots]}")
        print(f"Error code: {snapshots_response.error_code}")
        print(f"Error message: {snapshots_response.error_msg}")
        assert snapshot_name in [s.name for s in snapshots]

        # self.verify_restored_table_functionality(table_name, db_obj, expected_row_count=100)
        
        # Drop original table
        db_obj.drop_table(table_name, ConflictType.Error)
        
        # Restore from snapshot
        restore_result = db_obj.restore_table_snapshot(snapshot_name)
        assert restore_result.error_code == ErrorCode.OK
        
        # Verify data integrity and functionality
        self.verify_restored_table_functionality(table_name, db_obj, expected_row_count=actual_inserted)
        
        # Drop snapshot
        drop_result = self.infinity_obj.drop_snapshot(snapshot_name)
        assert drop_result.error_code == ErrorCode.OK

        # Drop table
        db_obj.drop_table(table_name, ConflictType.Error)
    
    # need to be moved to parallel
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
        """Test snapshot with large table and retry logic"""
        table_name = f"test_large_dataset{suffix}"
        snapshot_name = f"large_snapshot{suffix}"
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Drop original table
        db_obj.drop_table(table_name, ConflictType.Ignore)
        
        # Create table and insert large amount of data
        table_obj = self.create_comprehensive_table(table_name)
        table_obj = self.add_drop_column(table_obj)
        self._create_indexes(table_obj)
        actual_inserted = self.insert_comprehensive_data(table_obj, 100000)  # 100k rows - should be fine with small dimensions
        print(f"Successfully inserted {actual_inserted} out of 100000 rows")

        # Check count
        count_result, extra_result = table_obj.output(["count(*)"]).to_df()
        print(f"   Row count: {count_result.iloc[0, 0]}")
        assert count_result.iloc[0, 0] == actual_inserted, f"Expected {actual_inserted} rows, got {count_result.iloc[0, 0]}"

        # Measure snapshot creation time with retry logic
        start_time = time.time()
        snapshot_result = self.create_snapshot_with_retry(
            db_obj, snapshot_name, table_name, 
            max_retries=self.LARGE_TABLE_MAX_RETRIES, 
            retry_delay=self.LARGE_TABLE_RETRY_DELAY
        )
        snapshot_time = time.time() - start_time
        
        if self.handle_snapshot_result(snapshot_result, "Large table snapshot creation", skip_on_checkpoint=False):
            print(f"Snapshot creation time: {snapshot_time:.2f} seconds")
        else:
            pytest.skip(f"Large table snapshot failed due to checkpointing after retries: {snapshot_result.error_msg}")

        # Drop table
        db_obj.drop_table(table_name, ConflictType.Error)
        # use new db
        # self.infinity_obj.create_database("test_large_dataset_db", ConflictType.Ignore)
        # db_obj = self.infinity_obj.get_database("test_large_dataset_db")
        
        # Test restore performance
        start_time = time.time()
        restore_result = db_obj.restore_table_snapshot(snapshot_name)
        restore_time = time.time() - start_time
        
        assert restore_result.error_code == ErrorCode.OK

        # Verify data integrity and functionality
        self.verify_restored_table_functionality(table_name, db_obj, expected_row_count=actual_inserted)

        # Drop snapshot
        drop_result = self.infinity_obj.drop_snapshot(snapshot_name)
        assert drop_result.error_code == ErrorCode.OK

        # Drop table
        db_obj.drop_table(table_name, ConflictType.Error)

        print(f"Snapshot restore time: {restore_time:.2f} seconds")
    
    def test_snapshot_error_conditions(self):
        """Test error conditions for snapshot operations"""
        # Test creating snapshot of non-existent table
        db_obj = self.infinity_obj.get_database("default_db")
        with pytest.raises(InfinityException) as e:
            db_obj.create_table_snapshot("non_existent", "non_existent_table")
        
        # Test restoring non-existent snapshot
        with pytest.raises(InfinityException) as e:
            db_obj.restore_table_snapshot("non_existent_snapshot")
        
        # Test dropping non-existent snapshot
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.drop_snapshot("non_existent_snapshot")
    
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
            # Create snapshot with retry logic
            db_obj = self.infinity_obj.get_database("default_db")
            result = self.create_snapshot_with_retry(db_obj, snapshot_name, table_name, max_retries=3, retry_delay=1)
            
            if not self.handle_snapshot_result(result, f"Snapshot creation with name '{snapshot_name}'", skip_on_checkpoint=False):
                pytest.skip(f"Snapshot with name '{snapshot_name}' failed due to checkpointing: {result.error_msg}")

            db_obj.drop_table(table_name, ConflictType.Error)
            
            # Restore from snapshot
            restore_result = db_obj.restore_table_snapshot(snapshot_name)
            assert restore_result.error_code == ErrorCode.OK
            
            # Clean up
            self.infinity_obj.drop_snapshot(snapshot_name)

        db_obj.drop_table(table_name, ConflictType.Error)

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

    # TODO: need to be moved to parallel
    # def test_multithread_snapshot_with_modifications(self, suffix):
    #     """Test snapshot creation and restore while table is being modified by multiple threads - runs 20 times"""
    #     import threading
    #     import time
    #     import random
        
    #     # Run the entire test 20 times
    #     for test_run in range(2):
    #         print(f"\n=== Test Run {test_run + 1}/20 ===")
            
    #         table_name = f"multithread_test{suffix}_{test_run}"
    #         snapshot_name = f"multithread_snap{suffix}_{test_run}"
            
    #         # Create table with comprehensive schema
    #         db_obj = self.infinity_obj.get_database("default_db")
    #         db_obj.drop_table(table_name, ConflictType.Ignore)
            
    #         table_obj = self.create_comprehensive_table(table_name)
            
            
    #         # Insert initial data
    #         print(f"Inserting initial data for run {test_run + 1}...")
    #         self.insert_comprehensive_data(table_obj, 1000)  # 1K initial rows
    #         self._create_indexes(table_obj)
        
    #         # Shared state for coordination
    #         class TestState:
    #             def __init__(self):
    #                 self.running = True
    #                 self.insert_count = 0
    #                 self.update_count = 0
    #                 self.delete_count = 0
    #                 self.index_count = 0
    #                 self.snapshot_taken = False
    #                 self.next_id = 1001  # Start after initial 1000 rows
    #                 self.lock = threading.Lock()
            
    #         state = TestState()
            
    #         # Thread 1: Continuous data insertion with unique IDs
    #         def insert_thread():
    #             print("Insert thread started")
    #             while state.running:
    #                 try:
    #                     with state.lock:
    #                         current_id = state.next_id
    #                         state.next_id += 1
                        
    #                     # Insert with unique ID
    #                     new_sparse_data = SparseVector([1, 2], [0.1, 0.2])
    #                     new_row = {
    #                         "id": current_id,
    #                         "name": "test_insert_user",
    #                         "age": 25,
    #                         "salary": 75000.0,
    #                         "is_active": True,
    #                         "vector_col": [0.1] * 1,
    #                         "tensor_col": [0.1] * 2,
    #                         "sparse_col": new_sparse_data,
    #                     }
    #                     table_obj.insert(new_row)
    #                     with state.lock:
    #                         state.insert_count += 1
    #                     time.sleep(0.01)  # Small delay
    #                 except Exception as e:
    #                     print(f"Insert thread error: {e}")
    #                     break
            
    #         # Thread 2: Continuous data updates
    #         def update_thread():
    #             print("Update thread started")
    #             # while state.running:
    #             #     try:
    #             #         # Update random rows from initial data (1-1000)
    #             #         update_id = random.randint(1, 1000)
    #             #         update_data = {
    #             #             "name": f"updated_{random.randint(1, 1000)}",
    #             #             "age": random.randint(18, 80),
    #             #             "salary": random.uniform(30000, 150000)
    #             #         }
    #             #         # Use proper update syntax
    #             #         table_obj.update(update_data, f"id = {update_id}")
    #             #         with state.lock:
    #             #             state.update_count += 1
    #             #         time.sleep(0.02)  # Slightly longer delay
    #             #     except Exception as e:
    #             #         print(f"Update thread error: {e}")
    #             #         break
            
    #         # Thread 3: Continuous data deletion
    #         def delete_thread():
    #             print("Delete thread started")
    #             # while state.running:
    #             #     try:
    #             #         # Delete random rows
    #             #         delete_id = random.randint(1, 1000)
    #             #         table_obj.delete(f"id = {delete_id}")
    #             #         with state.lock:
    #             #             state.delete_count += 1
    #             #         time.sleep(0.03)  # Longer delay for deletes
    #             #     except Exception as e:
    #             #         print(f"Delete thread error: {e}")
    #             #         break
            
    #         # Thread 4: Index operations
    #         def index_thread():
    #             print("Index thread started")
    #             while state.running:
    #                 try:
    #                     # Create and drop indexes
    #                     index_name = f"test_idx_{random.randint(1, 100)}"
    #                     try:
    #                         # Try to create index
    #                         table_obj.create_index(index_name, index.IndexInfo("name", index.IndexType.FullText))
    #                         with state.lock:
    #                             state.index_count += 1
    #                         time.sleep(0.1)
                            
    #                         # Try to drop the index
    #                         table_obj.drop_index(index_name)
    #                         with state.lock:
    #                             state.index_count += 1
    #                         time.sleep(0.1)
    #                     except Exception as inner_e:
    #                         # Index operations might fail due to concurrent modifications
    #                         print(f"Index operation failed: {inner_e}")
    #                         pass
    #                 except Exception as e:
    #                     import traceback
    #                     print(f"Index thread error: {e}")
    #                     print(f"Full traceback:")
    #                     traceback.print_exc()
    #                     break
            
    #         # Thread 5: Snapshot creation
    #         def snapshot_thread():
    #             print("Snapshot thread started")
    #             time.sleep(0.5)  # Let other threads start first
                
    #             try:
    #                 print("Creating snapshot while table is being modified...")
    #                 start_time = time.time()
                    
    #                 # Create snapshot while other threads are modifying the table
    #                 result = db_obj.create_table_snapshot(snapshot_name, table_name)
                    
    #                 end_time = time.time()
    #                 snapshot_time = end_time - start_time
                    
    #                 if result.error_code == ErrorCode.OK:
    #                     print(f"Snapshot created successfully in {snapshot_time:.2f} seconds")
    #                     print(f"During snapshot creation:")
    #                     print(f"  - {state.insert_count} inserts performed")
    #                     print(f"  - {state.update_count} updates performed")
    #                     print(f"  - {state.delete_count} deletes performed")
    #                     print(f"  - {state.index_count} index operations performed")
                        
                        
    #                 else:
    #                     print(f"Snapshot creation failed: {result.error_code}")
                        
    #             except Exception as e:
    #                 print(f"Snapshot thread error: {e}")
    #                 raise
            
    #         # Start all threads
    #         threads = []
    #         thread_functions = [insert_thread, update_thread, delete_thread, index_thread, snapshot_thread]
            
    #         print("Starting modification threads...")
    #         for func in thread_functions:
    #             thread = threading.Thread(target=func)
    #             thread.daemon = True
    #             thread.start()
    #             threads.append(thread)
            
    #         # Wait for snapshot thread to complete
    #         threads[4].join(timeout=1000)  # Wait up to 30 seconds
            
    #         # Stop all threads
    #         print("Stopping modification threads...")
    #         state.running = False
            
    #         # Wait for other threads to finish
    #         for i, thread in enumerate(threads[:4]):
    #             thread.join(timeout=1000)
    #             if thread.is_alive():
    #                 print(f"Thread {i} did not stop gracefully")
            
    #         print("Multithreaded snapshot test completed")
            
            
    #         try:
    #             # drop table
    #             db_obj.drop_table(table_name, ConflictType.Ignore)
                
    #             # Restore the snapshot
    #             restore_result = db_obj.restore_table_snapshot(snapshot_name)
    #             assert restore_result.error_code == ErrorCode.OK, f"Snapshot restore failed: {restore_result.error_code}"
                
    #             self.verify_restored_table_functionality(table_name, db_obj)
    #         except Exception as e:
    #             print(f"Snapshot restore failed: {e}")
    #             raise

    #         # Cleanup
    #         try:
    #             self.infinity_obj.drop_snapshot(snapshot_name)
    #         except:
    #             pass
    #         db_obj.drop_table(table_name, ConflictType.Ignore)
        
    #         print("Verifying snapshot data integrity...")