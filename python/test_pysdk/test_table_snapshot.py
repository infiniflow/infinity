import sys
import os
import concurrent.futures
import pytest
import polars as pl
import time
import random
import json
import base64
from typing import Dict, List, Any
from common import common_values
from infinity.common import ConflictType, InfinityException
import infinity
from infinity.errors import ErrorCode
from common.utils import trace_expected_exceptions

class TestSnapshot:
    """Comprehensive snapshot testing for Infinity database"""
    
    @pytest.fixture(scope="class")
    def setup_class(self, request, http):
        if http:
            uri = common_values.TEST_LOCAL_HOST
            request.cls.infinity_obj = infinity_http()
        else:
            uri = common_values.TEST_LOCAL_HOST
            request.cls.infinity_obj = infinity.connect(uri)
        request.cls.uri = uri
        yield
        request.cls.infinity_obj.disconnect()
    
    def setup_method(self):
        """Setup for each test method"""
        self.db_obj = self.infinity_obj.get_database("default_db")
        self.suffix = f"_{int(time.time())}_{random.randint(1000, 9999)}"
        
    def teardown_method(self):
        """Cleanup after each test"""
        # Clean up any remaining tables/snapshots
        pass
    
    def create_comprehensive_table(self, table_name: str) -> Dict[str, Any]:
        """Create a table with all data types and indexes"""
        table_schema = {
            "id": {"type": "int", "constraints": ["primary key"]},
            "name": {"type": "varchar"},
            "age": {"type": "int8"},
            "salary": {"type": "float64"},
            "is_active": {"type": "bool"},
            "vector_col": {"type": "vector,128,float"},
            "tensor_col": {"type": "tensor,64,float"},
            "json_col": {"type": "varchar"},
            "timestamp_col": {"type": "int64"},
            "blob_col": {"type": "varchar"}
        }
        
        # Create table
        table_obj = self.db_obj.create_table(table_name, table_schema, ConflictType.Error)
        
        # Create indexes
        self._create_indexes(table_obj)
        
        return table_obj
    
    def _create_indexes(self, table_obj):
        """Create various types of indexes"""
        # Primary key is already created
        # Secondary indexes
        table_obj.create_index("idx_name", ["name"], ConflictType.Error)
        table_obj.create_index("idx_age_salary", ["age", "salary"], ConflictType.Error)
        
        # Vector indexes
        table_obj.create_index("idx_vector_hnsw", ["vector_col"], {
            "type": "hnsw",
            "metric": "cosine",
            "m": 16,
            "ef_construction": 200
        }, ConflictType.Error)
        
        # Full-text search index
        table_obj.create_index("idx_name_fts", ["name"], {
            "type": "fulltext"
        }, ConflictType.Error)
    
    def insert_comprehensive_data(self, table_obj, num_rows: int = 1000):
        """Insert comprehensive test data"""
        data = []
        for i in range(num_rows):
            row = {
                "id": i,
                "name": f"user_{i}",
                "age": random.randint(18, 80),
                "salary": random.uniform(30000, 150000),
                "is_active": random.choice([True, False]),
                "vector_col": [random.uniform(-1, 1) for _ in range(128)],
                "tensor_col": [random.uniform(-1, 1) for _ in range(64)],
                "json_col": json.dumps({"key": f"value_{i}", "nested": {"data": i}}),
                "timestamp_col": int(time.time()) + i,
                "blob_col": base64.b64encode(f"blob_data_{i}".encode()).decode()
            }
            data.append(row)
        
        # Insert in batches
        batch_size = 100
        for i in range(0, len(data), batch_size):
            batch = data[i:i + batch_size]
            table_obj.insert(batch)
    
    def test_basic_snapshot_operations(self):
        """Test basic snapshot create, list, drop operations"""
        table_name = f"test_basic_snapshot{self.suffix}"
        snapshot_name = f"basic_snapshot{self.suffix}"
        
        # Create table and insert data
        table_obj = self.create_comprehensive_table(table_name)
        self.insert_comprehensive_data(table_obj, 100)
        
        # Create snapshot
        snapshot_result = self.db_obj.create_snapshot(snapshot_name, table_name)
        assert snapshot_result.error_code == ErrorCode.OK
        
        # List snapshots
        snapshots = self.db_obj.list_snapshots()
        assert snapshot_name in [s["name"] for s in snapshots]
        
        # Drop original table
        self.db_obj.drop_table(table_name, ConflictType.Error)
        
        # Restore from snapshot
        restore_result = self.db_obj.restore_snapshot(snapshot_name, table_name)
        assert restore_result.error_code == ErrorCode.OK
        
        # Verify data integrity
        restored_table = self.db_obj.get_table(table_name)
        count_result = restored_table.count()
        assert count_result == 100
        
        # Drop snapshot
        drop_result = self.db_obj.drop_snapshot(snapshot_name)
        assert drop_result.error_code == ErrorCode.OK
    
    def test_snapshot_concurrency(self):
        """Test concurrent snapshot operations"""
        table_name = f"test_concurrency{self.suffix}"
        snapshot_name = f"concurrency_snapshot{self.suffix}"
        
        # Create table and insert data
        table_obj = self.create_comprehensive_table(table_name)
        self.insert_comprehensive_data(table_obj, 500)
        
        def create_snapshot():
            return self.db_obj.create_snapshot(snapshot_name, table_name)
        
        def restore_snapshot():
            return self.db_obj.restore_snapshot(snapshot_name, f"restored_{table_name}")
        
        # Test concurrent snapshot creation
        with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
            futures = [executor.submit(create_snapshot) for _ in range(4)]
            results = [future.result() for future in futures]
            
            # Only one should succeed, others should fail with conflict
            success_count = sum(1 for r in results if r.error_code == ErrorCode.OK)
            assert success_count == 1
    
    def test_snapshot_large_dataset(self):
        """Test snapshot with large dataset"""
        table_name = f"test_large_dataset{self.suffix}"
        snapshot_name = f"large_snapshot{self.suffix}"
        
        # Create table and insert large amount of data
        table_obj = self.create_comprehensive_table(table_name)
        self.insert_comprehensive_data(table_obj, 10000)  # 10K rows
        
        # Measure snapshot creation time
        start_time = time.time()
        snapshot_result = self.db_obj.create_snapshot(snapshot_name, table_name)
        snapshot_time = time.time() - start_time
        
        assert snapshot_result.error_code == ErrorCode.OK
        print(f"Snapshot creation time: {snapshot_time:.2f} seconds")
        
        # Test restore performance
        start_time = time.time()
        restore_result = self.db_obj.restore_snapshot(snapshot_name, f"restored_{table_name}")
        restore_time = time.time() - start_time
        
        assert restore_result.error_code == ErrorCode.OK
        print(f"Snapshot restore time: {restore_time:.2f} seconds")
    
    def test_snapshot_data_integrity(self):
        """Test data integrity after snapshot restore"""
        table_name = f"test_integrity{self.suffix}"
        snapshot_name = f"integrity_snapshot{self.suffix}"
        
        # Create table and insert data
        table_obj = self.create_comprehensive_table(table_name)
        self.insert_comprehensive_data(table_obj, 100)
        
        # Create snapshot
        self.db_obj.create_snapshot(snapshot_name, table_name)
        
        # Modify original table
        table_obj.insert({"id": 999, "name": "modified", "age": 25, "salary": 50000.0, 
                         "is_active": True, "vector_col": [0.1] * 128, "tensor_col": [0.1] * 64,
                         "json_col": "{}", "timestamp_col": 0, "blob_col": ""})
        
        # Restore from snapshot
        self.db_obj.restore_snapshot(snapshot_name, f"restored_{table_name}")
        
        # Verify restored table doesn't have the modified data
        restored_table = self.db_obj.get_table(f"restored_{table_name}")
        count_result = restored_table.count()
        assert count_result == 100  # Should not include the modified row
    
    def test_snapshot_index_integrity(self):
        """Test that indexes are properly restored"""
        table_name = f"test_index_integrity{self.suffix}"
        snapshot_name = f"index_snapshot{self.suffix}"
        
        # Create table with indexes
        table_obj = self.create_comprehensive_table(table_name)
        self.insert_comprehensive_data(table_obj, 100)
        
        # Create snapshot
        self.db_obj.create_snapshot(snapshot_name, table_name)
        
        # Restore from snapshot
        self.db_obj.restore_snapshot(snapshot_name, f"restored_{table_name}")
        
        # Verify indexes exist on restored table
        restored_table = self.db_obj.get_table(f"restored_{table_name}")
        indexes = restored_table.list_indexes()
        
        # Check that all expected indexes are present
        expected_indexes = ["idx_name", "idx_age_salary", "idx_vector_hnsw", "idx_name_fts"]
        for expected_idx in expected_indexes:
            assert any(idx["name"] == expected_idx for idx in indexes)
    
    def test_snapshot_error_conditions(self):
        """Test error conditions for snapshot operations"""
        # Test creating snapshot of non-existent table
        with pytest.raises(InfinityException) as e:
            self.db_obj.create_snapshot("non_existent", "non_existent_table")
        
        # Test restoring non-existent snapshot
        with pytest.raises(InfinityException) as e:
            self.db_obj.restore_snapshot("non_existent_snapshot", "new_table")
        
        # Test dropping non-existent snapshot
        with pytest.raises(InfinityException) as e:
            self.db_obj.drop_snapshot("non_existent_snapshot")
    
    def test_snapshot_naming_conventions(self):
        """Test snapshot with various naming conventions"""
        table_name = f"test_naming{self.suffix}"
        
        # Test with special characters in snapshot name
        special_names = [
            "snapshot_with_underscores",
            "snapshot-with-dashes",
            "snapshot123",
            "SNAPSHOT_UPPER",
            "snapshot.lower"
        ]
        
        table_obj = self.create_comprehensive_table(table_name)
        self.insert_comprehensive_data(table_obj, 10)
        
        for snapshot_name in special_names:
            # Create snapshot
            result = self.db_obj.create_snapshot(snapshot_name, table_name)
            assert result.error_code == ErrorCode.OK
            
            # Restore from snapshot
            restore_result = self.db_obj.restore_snapshot(snapshot_name, f"restored_{snapshot_name}")
            assert restore_result.error_code == ErrorCode.OK
            
            # Clean up
            self.db_obj.drop_snapshot(snapshot_name)
    
    @pytest.mark.slow
    def test_snapshot_stress_test(self):
        """Stress test for snapshot operations"""
        num_tables = 10
        tables = []
        
        # Create multiple tables
        for i in range(num_tables):
            table_name = f"stress_test_table_{i}{self.suffix}"
            table_obj = self.create_comprehensive_table(table_name)
            self.insert_comprehensive_data(table_obj, 100)
            tables.append((table_name, table_obj))
        
        # Create snapshots for all tables concurrently
        def create_snapshot_for_table(table_name):
            snapshot_name = f"stress_snapshot_{table_name}"
            return self.db_obj.create_snapshot(snapshot_name, table_name)
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=5) as executor:
            futures = [executor.submit(create_snapshot_for_table, table_name) 
                      for table_name, _ in tables]
            results = [future.result() for future in futures]
            
            # All should succeed
            for result in results:
                assert result.error_code == ErrorCode.OK
        
        # Restore all snapshots concurrently
        def restore_snapshot_for_table(table_name):
            snapshot_name = f"stress_snapshot_{table_name}"
            return self.db_obj.restore_snapshot(snapshot_name, f"restored_{table_name}")
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=5) as executor:
            futures = [executor.submit(restore_snapshot_for_table, table_name) 
                      for table_name, _ in tables]
            results = [future.result() for future in futures]
            
            # All should succeed
            for result in results:
                assert result.error_code == ErrorCode.OK