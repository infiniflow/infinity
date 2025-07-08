import sys
import os
import concurrent.futures
import pytest
import polars as pl
import time
import random
from common import common_values
from common import common_index
from infinity.common import ConflictType, InfinityException
import infinity
from infinity.errors import ErrorCode
from common.utils import trace_expected_exceptions
import index


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
        yield
        request.cls.infinity_obj.disconnect()

    @pytest.fixture(autouse=True)
    def setup(self, request, setup_class):
        # Setup before each test
        pass

    def teardown(self, request, setup_class):
        # Cleanup after each test
        pass

    # ==================== BASIC SNAPSHOT OPERATIONS ====================

    def test_create_snapshot_basic(self, suffix):
        """Test basic snapshot creation"""
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create table with various data types
        table_obj = db_obj.create_table("test_snapshot_basic" + suffix, {
            "c1": {"type": "int"},
            "c2": {"type": "varchar"},
            "c3": {"type": "float"},
            "c4": {"type": "bool"},
            "c5": {"type": "vector,128,float"},
            "c6": {"type": "tensor,64,float"},
            "c7": {"type": "sparse,1000,float,int16"}
        }, ConflictType.Error)
        assert table_obj is not None

        # Insert sample data
        values = [
            [1, "hello", 3.14, True, [0.1] * 128, [0.2] * 64, {"indices": [1, 3, 5], "values": [1.0, 2.0, 3.0]}],
            [2, "world", 2.71, False, [0.3] * 128, [0.4] * 64, {"indices": [2, 4, 6], "values": [4.0, 5.0, 6.0]}],
        ]
        res = table_obj.insert(values)
        assert res.error_code == ErrorCode.OK

        # Create snapshot
        res = db_obj.create_snapshot("test_snapshot_basic" + suffix)
        assert res.error_code == ErrorCode.OK

        # Verify snapshot exists
        res = db_obj.show_snapshots()
        assert res.error_code == ErrorCode.OK
        assert any(snapshot.snapshot_name == "test_snapshot_basic" + suffix for snapshot in res.snapshots)

        # Cleanup
        res = db_obj.drop_snapshot("test_snapshot_basic" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_snapshot_basic" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_restore_snapshot_basic(self, suffix):
        """Test basic snapshot restoration"""
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create original table
        table_obj = db_obj.create_table("test_restore_basic" + suffix, {
            "c1": {"type": "int"},
            "c2": {"type": "varchar"}
        }, ConflictType.Error)
        assert table_obj is not None

        # Insert data
        values = [[1, "original"], [2, "data"]]
        res = table_obj.insert(values)
        assert res.error_code == ErrorCode.OK

        # Create snapshot
        res = db_obj.create_snapshot("test_restore_basic" + suffix)
        assert res.error_code == ErrorCode.OK

        # Modify table after snapshot
        values = [[3, "modified"], [4, "data"]]
        res = table_obj.insert(values)
        assert res.error_code == ErrorCode.OK

        # Restore from snapshot
        res = db_obj.restore_snapshot("test_restore_basic" + suffix)
        assert res.error_code == ErrorCode.OK

        # Verify only original data exists
        res = table_obj.output(["c1", "c2"]).to_pl()
        assert res.error_code == ErrorCode.OK
        assert len(res.result) == 2
        assert res.result[0][0] == 1 and res.result[0][1] == "original"

        # Cleanup
        res = db_obj.drop_snapshot("test_restore_basic" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_restore_basic" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # ==================== SNAPSHOT WITH INDEXES ====================

    @pytest.mark.parametrize("index_type", [
        common_index.IndexType.IVF,
        common_index.IndexType.Hnsw,
        common_index.IndexType.BMP,
        common_index.IndexType.FullText,
        common_index.IndexType.EMVB,
        common_index.IndexType.Secondary,
    ])
    def test_snapshot_with_index(self, index_type, suffix):
        """Test snapshot creation and restoration with different index types"""
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = f"test_snapshot_index_{index_type.value}" + suffix
        
        # Create table based on index type
        if index_type == common_index.IndexType.IVF:
            table_obj = db_obj.create_table(table_name, {
                "c1": {"type": "int"},
                "c2": {"type": "vector,1024,float"}
            }, ConflictType.Error)
            assert table_obj is not None
            
            # Create IVF index
            res = table_obj.create_index("my_ivf_index",
                                        index.IndexInfo("c2", index.IndexType.IVF, {"metric": "l2"}),
                                        ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        elif index_type == common_index.IndexType.Hnsw:
            table_obj = db_obj.create_table(table_name, {
                "c1": {"type": "int"},
                "c2": {"type": "vector,1024,float"}
            }, ConflictType.Error)
            assert table_obj is not None
            
            # Create HNSW index
            res = table_obj.create_index("my_hnsw_index",
                                        index.IndexInfo("c2", index.IndexType.Hnsw, {
                                            "M": "16",
                                            "ef_construction": "50",
                                            "metric": "L2"
                                        }), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        elif index_type == common_index.IndexType.BMP:
            table_obj = db_obj.create_table(table_name, {
                "c1": {"type": "int"},
                "c2": {"type": "sparse,30000,float,int16"}
            }, ConflictType.Error)
            assert table_obj is not None
            
            # Create BMP index
            res = table_obj.create_index("my_bmp_index",
                                        index.IndexInfo("c2", index.IndexType.BMP, {
                                            "block_size": "8",
                                            "compress_type": "COMPRESS"
                                        }), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        elif index_type == common_index.IndexType.FullText:
            table_obj = db_obj.create_table(table_name, {
                "c1": {"type": "int"},
                "c2": {"type": "varchar"}
            }, ConflictType.Error)
            assert table_obj is not None
            
            # Create FullText index
            res = table_obj.create_index("my_fulltext_index",
                                        index.IndexInfo("c2", index.IndexType.FullText, {"ANALYZER": "STANDARD"}),
                                        ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        elif index_type == common_index.IndexType.EMVB:
            table_obj = db_obj.create_table(table_name, {
                "c1": {"type": "int"},
                "c2": {"type": "tensor,128,float"}
            }, ConflictType.Error)
            assert table_obj is not None
            
            # Create EMVB index
            res = table_obj.create_index("my_emvb_index",
                                        index.IndexInfo("c2", index.IndexType.EMVB, {
                                            "pq_subspace_num": "32",
                                            "pq_subspace_bits": "8"
                                        }), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        elif index_type == common_index.IndexType.Secondary:
            table_obj = db_obj.create_table(table_name, {
                "c1": {"type": "int"},
                "c2": {"type": "varchar"}
            }, ConflictType.Error)
            assert table_obj is not None
            
            # Create Secondary index
            res = table_obj.create_index("my_secondary_index",
                                        index.IndexInfo("c1", index.IndexType.Secondary),
                                        ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        # Insert data
        if index_type in [common_index.IndexType.IVF, common_index.IndexType.Hnsw]:
            values = [[1, [0.1] * 1024], [2, [0.2] * 1024]]
        elif index_type == common_index.IndexType.BMP:
            values = [[1, {"indices": [1, 3, 5], "values": [1.0, 2.0, 3.0]}],
                     [2, {"indices": [2, 4, 6], "values": [4.0, 5.0, 6.0]}]]
        elif index_type == common_index.IndexType.FullText:
            values = [[1, "hello world"], [2, "test document"]]
        elif index_type == common_index.IndexType.EMVB:
            values = [[1, [0.1] * 128], [2, [0.2] * 128]]
        else:  # Secondary
            values = [[1, "value1"], [2, "value2"]]

        res = table_obj.insert(values)
        assert res.error_code == ErrorCode.OK

        # Create snapshot
        snapshot_name = f"test_snapshot_{index_type.value}" + suffix
        res = db_obj.create_snapshot(snapshot_name)
        assert res.error_code == ErrorCode.OK

        # Verify snapshot contains indexes
        res = db_obj.show_snapshots()
        assert res.error_code == ErrorCode.OK
        assert any(snapshot.snapshot_name == snapshot_name for snapshot in res.snapshots)

        # Restore snapshot
        res = db_obj.restore_snapshot(snapshot_name)
        assert res.error_code == ErrorCode.OK

        # Verify indexes still exist after restore
        res = table_obj.show_indexes()
        assert res.error_code == ErrorCode.OK
        assert len(res.indexes) > 0

        # Cleanup
        res = db_obj.drop_snapshot(snapshot_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # ==================== CONCURRENT SNAPSHOT OPERATIONS ====================

    def test_concurrent_snapshot_creation(self, suffix):
        """Test multiple threads creating snapshots simultaneously"""
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create table
        table_obj = db_obj.create_table("test_concurrent_snapshot" + suffix, {
            "c1": {"type": "int"},
            "c2": {"type": "varchar"}
        }, ConflictType.Error)
        assert table_obj is not None

        # Insert data
        values = [[i, f"value_{i}"] for i in range(100)]
        res = table_obj.insert(values)
        assert res.error_code == ErrorCode.OK

        # Create snapshots concurrently
        def create_snapshot(thread_id):
            snapshot_name = f"concurrent_snapshot_{thread_id}" + suffix
            res = db_obj.create_snapshot(snapshot_name)
            return res.error_code == ErrorCode.OK

        with concurrent.futures.ThreadPoolExecutor(max_workers=5) as executor:
            futures = [executor.submit(create_snapshot, i) for i in range(5)]
            results = [future.result() for future in futures]

        # All snapshots should be created successfully
        assert all(results)

        # Verify all snapshots exist
        res = db_obj.show_snapshots()
        assert res.error_code == ErrorCode.OK
        for i in range(5):
            snapshot_name = f"concurrent_snapshot_{i}" + suffix
            assert any(snapshot.snapshot_name == snapshot_name for snapshot in res.snapshots)

        # Cleanup
        for i in range(5):
            snapshot_name = f"concurrent_snapshot_{i}" + suffix
            res = db_obj.drop_snapshot(snapshot_name, ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_concurrent_snapshot" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_concurrent_snapshot_restore(self, suffix):
        """Test multiple threads trying to restore the same snapshot"""
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create table and snapshot
        table_obj = db_obj.create_table("test_concurrent_restore" + suffix, {
            "c1": {"type": "int"},
            "c2": {"type": "varchar"}
        }, ConflictType.Error)
        assert table_obj is not None

        values = [[1, "original"], [2, "data"]]
        res = table_obj.insert(values)
        assert res.error_code == ErrorCode.OK

        snapshot_name = "test_concurrent_restore" + suffix
        res = db_obj.create_snapshot(snapshot_name)
        assert res.error_code == ErrorCode.OK

        # Try to restore concurrently
        def restore_snapshot(thread_id):
            try:
                res = db_obj.restore_snapshot(snapshot_name)
                return res.error_code == ErrorCode.OK
            except Exception as e:
                return False

        with concurrent.futures.ThreadPoolExecutor(max_workers=3) as executor:
            futures = [executor.submit(restore_snapshot, i) for i in range(3)]
            results = [future.result() for future in futures]

        # At least one restore should succeed
        assert any(results)

        # Cleanup
        res = db_obj.drop_snapshot(snapshot_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_concurrent_restore" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # ==================== SNAPSHOT ERROR HANDLING ====================

    def test_snapshot_not_found(self, suffix):
        """Test error handling when snapshot doesn't exist"""
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Try to restore non-existent snapshot
        with pytest.raises(InfinityException) as e:
            db_obj.restore_snapshot("non_existent_snapshot" + suffix)
        assert e.value.error_code == ErrorCode.SNAPSHOT_NOT_FOUND

    def test_snapshot_already_exists(self, suffix):
        """Test error handling when creating duplicate snapshot"""
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create table
        table_obj = db_obj.create_table("test_duplicate_snapshot" + suffix, {
            "c1": {"type": "int"}
        }, ConflictType.Error)
        assert table_obj is not None

        # Create snapshot
        snapshot_name = "test_duplicate" + suffix
        res = db_obj.create_snapshot(snapshot_name)
        assert res.error_code == ErrorCode.OK

        # Try to create same snapshot again
        with pytest.raises(InfinityException) as e:
            db_obj.create_snapshot(snapshot_name)
        assert e.value.error_code == ErrorCode.SNAPSHOT_ALREADY_EXISTS

        # Cleanup
        res = db_obj.drop_snapshot(snapshot_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_duplicate_snapshot" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # ==================== COMPLEX SNAPSHOT SCENARIOS ====================

    def test_snapshot_with_multiple_indexes(self, suffix):
        """Test snapshot with multiple different index types on same table"""
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create table with multiple data types
        table_obj = db_obj.create_table("test_multi_index_snapshot" + suffix, {
            "id": {"type": "int"},
            "text": {"type": "varchar"},
            "vector": {"type": "vector,128,float"},
            "tensor": {"type": "tensor,64,float"},
            "sparse": {"type": "sparse,1000,float,int16"}
        }, ConflictType.Error)
        assert table_obj is not None

        # Create multiple indexes
        indexes = [
            ("id_idx", "id", common_index.IndexType.Secondary, {}),
            ("text_idx", "text", common_index.IndexType.FullText, {"ANALYZER": "STANDARD"}),
            ("vector_idx", "vector", common_index.IndexType.Hnsw, {"M": "16", "ef_construction": "50", "metric": "L2"}),
            ("tensor_idx", "tensor", common_index.IndexType.EMVB, {"pq_subspace_num": "32", "pq_subspace_bits": "8"}),
            ("sparse_idx", "sparse", common_index.IndexType.BMP, {"block_size": "8", "compress_type": "COMPRESS"})
        ]

        for idx_name, col_name, idx_type, params in indexes:
            res = table_obj.create_index(idx_name,
                                       index.IndexInfo(col_name, idx_type, params),
                                       ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        # Insert data
        values = [
            [1, "hello world", [0.1] * 128, [0.2] * 64, {"indices": [1, 3, 5], "values": [1.0, 2.0, 3.0]}],
            [2, "test document", [0.3] * 128, [0.4] * 64, {"indices": [2, 4, 6], "values": [4.0, 5.0, 6.0]}],
        ]
        res = table_obj.insert(values)
        assert res.error_code == ErrorCode.OK

        # Create snapshot
        snapshot_name = "test_multi_index" + suffix
        res = db_obj.create_snapshot(snapshot_name)
        assert res.error_code == ErrorCode.OK

        # Restore snapshot
        res = db_obj.restore_snapshot(snapshot_name)
        assert res.error_code == ErrorCode.OK

        # Verify all indexes still exist
        res = table_obj.show_indexes()
        assert res.error_code == ErrorCode.OK
        assert len(res.indexes) == 5

        # Cleanup
        res = db_obj.drop_snapshot(snapshot_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_multi_index_snapshot" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_snapshot_large_dataset(self, suffix):
        """Test snapshot with large dataset"""
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create table
        table_obj = db_obj.create_table("test_large_snapshot" + suffix, {
            "id": {"type": "int"},
            "data": {"type": "varchar"},
            "vector": {"type": "vector,256,float"}
        }, ConflictType.Error)
        assert table_obj is not None

        # Insert large amount of data
        values = []
        for i in range(1000):
            values.append([i, f"data_{i}", [float(j) / 1000 for j in range(256)]])
        
        res = table_obj.insert(values)
        assert res.error_code == ErrorCode.OK

        # Create index
        res = table_obj.create_index("vector_idx",
                                   index.IndexInfo("vector", common_index.IndexType.Hnsw, {
                                       "M": "16",
                                       "ef_construction": "50",
                                       "metric": "L2"
                                   }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # Create snapshot
        snapshot_name = "test_large_dataset" + suffix
        res = db_obj.create_snapshot(snapshot_name)
        assert res.error_code == ErrorCode.OK

        # Restore snapshot
        res = db_obj.restore_snapshot(snapshot_name)
        assert res.error_code == ErrorCode.OK

        # Verify data integrity
        res = table_obj.output(["count(*)"], "").to_pl()
        assert res.error_code == ErrorCode.OK
        assert res.result[0][0] == 1000

        # Cleanup
        res = db_obj.drop_snapshot(snapshot_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_large_snapshot" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK 