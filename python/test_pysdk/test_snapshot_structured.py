import sys
import os
import concurrent.futures
import pytest
import polars as pl
from common import common_values
from infinity.common import ConflictType, InfinityException
import infinity
from infinity.errors import ErrorCode
from common.utils import trace_expected_exceptions
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
class TestSnapshotCreate:
    """Test snapshot creation operations"""
    
    @pytest.mark.usefixtures("setup_class")
    @pytest.mark.usefixtures("suffix")
    class TestTableSnapshot:
        """Test table-level snapshot operations"""
        
        def test_create_table_snapshot_basic(self, suffix):
            """Test basic table snapshot creation"""
            # Setup - use default_db database
            db_obj = self.infinity_obj.get_database("default_db")
            
            # Drop table if it exists
            try:
                db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)
            except:
                pass
            
            table_obj = db_obj.create_table(
                f"test_table_{suffix}",
                {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error
            )
            
            # Insert test data
            table_obj.insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])
            
            # Test
            res = db_obj.create_table_snapshot(f"table_snap_{suffix}", f"test_table_{suffix}")
            
            # Assert
            assert res.error_code == ErrorCode.OK, f"CreateTableSnapshot failed: {res.error_msg}"
            
            # Cleanup
            try:
                self.infinity_obj.drop_snapshot(f"table_snap_{suffix}")
            except:
                pass
            db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)
        
        def test_create_table_snapshot_empty_table(self, suffix):
            """Test creating snapshot of empty table"""
            db_obj = self.infinity_obj.get_database("default_db")
            
            # Drop table if it exists
            try:
                db_obj.drop_table(f"empty_table_{suffix}", ConflictType.Ignore)
            except:
                pass
            
            table_obj = db_obj.create_table(
                f"empty_table_{suffix}",
                {"c1": {"type": "int", "constraints": ["primary key"]}},
                ConflictType.Error
            )
            
            res = db_obj.create_table_snapshot(f"empty_snap_{suffix}", f"empty_table_{suffix}")
            assert res.error_code == ErrorCode.OK
            
            # Cleanup
            try:
                self.infinity_obj.drop_snapshot(f"empty_snap_{suffix}")
            except:
                pass
            db_obj.drop_table(f"empty_table_{suffix}", ConflictType.Ignore)
        
        def test_create_table_snapshot_invalid_table(self, suffix):
            """Test creating snapshot of non-existent table"""
            db_obj = self.infinity_obj.get_database("default_db")
            
            with pytest.raises(InfinityException):
                db_obj.create_table_snapshot(f"invalid_snap_{suffix}", "non_existent_table")
    
    # @pytest.mark.usefixtures("setup_class")
    # @pytest.mark.usefixtures("suffix")
    # class TestDatabaseSnapshot:
    #     """Test database-level snapshot operations"""
    #     
    #     def test_create_database_snapshot_basic(self, suffix):
    #         """Test basic database snapshot creation"""
    #         db_obj = self.infinity_obj.create_database(f"snapshot_db_{suffix}", ConflictType.Error)
    #         table_obj = db_obj.create_table(
    #             f"test_table_{suffix}",
    #             {"c1": {"type": "int", "constraints": ["primary key"]}},
    #             ConflictType.Error
    #         )
    #         table_obj.insert([{"c1": 1}])
    #         
    #         res = self.infinity_obj.create_database_snapshot(f"snapshot_db_{suffix}", f"db_snap_{suffix}")
    #         assert res.error_code == ErrorCode.OK
    #         
    #         self.infinity_obj.drop_database(f"snapshot_db_{suffix}", ConflictType.Error)
    #     
    #     def test_create_database_snapshot_empty(self, suffix):
    #         """Test creating snapshot of empty database"""
    #         db_obj = self.infinity_obj.create_database(f"empty_db_{suffix}", ConflictType.Error)
    #         
    #         res = self.infinity_obj.create_database_snapshot(f"empty_db_{suffix}", f"empty_db_snap_{suffix}")
    #         assert res.error_code == ErrorCode.OK
    #         
    #         self.infinity_obj.drop_database(f"empty_db_{suffix}", ConflictType.Error)
    
    # @pytest.mark.usefixtures("setup_class")
    # @pytest.mark.usefixtures("suffix")
    # class TestSystemSnapshot:
    #     """Test system-level snapshot operations"""
    #     
    #     def test_create_system_snapshot_basic(self, suffix):
    #         """Test basic system snapshot creation"""
    #         res = self.infinity_obj.create_system_snapshot(f"system_snap_{suffix}")
    #         assert res.error_code == ErrorCode.OK


class TestSnapshotRestore:
    """Test snapshot restore operations"""
    
    @pytest.mark.usefixtures("setup_class")
    @pytest.mark.usefixtures("suffix")
    class TestTableRestore:
        """Test table-level restore operations"""
        
        def test_restore_table_snapshot_basic(self, suffix):
            """Test basic table snapshot restore"""
            # Setup: Create table and snapshot
            db_obj = self.infinity_obj.get_database("default_db")
            
            # Drop table if it exists
            try:
                db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)
            except:
                pass
            
            table_obj = db_obj.create_table(
                f"test_table_{suffix}",
                {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error
            )
            table_obj.insert([{"c1": 1, "c2": 1.1}])
            
            # Create snapshot
            res = db_obj.create_table_snapshot(f"restore_snap_{suffix}", f"test_table_{suffix}")
            assert res.error_code == ErrorCode.OK
            
            # Modify table
            table_obj.insert([{"c1": 2, "c2": 2.2}])
            
            # Drop original table
            db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)
            
            # Restore from snapshot
            res = db_obj.restore_table_snapshot(f"restore_snap_{suffix}")
            assert res.error_code == ErrorCode.OK
            
            # Verify restore
            restored_table = db_obj.get_table(f"test_table_{suffix}")
            result = restored_table.output(["c1", "c2"]).to_result()
            assert len(result[0]["c1"]) == 1  # Should only have original data
            assert result[0]["c1"][0] == 1
            
            # Cleanup
            db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)

        
        
    
    # @pytest.mark.usefixtures("setup_class")
    # @pytest.mark.usefixtures("suffix")
    # class TestDatabaseRestore:
    #     """Test database-level restore operations"""
    #     
    #     def test_restore_database_snapshot_basic(self, suffix):
    #         """Test basic database snapshot restore"""
    #         # Setup: Create database with table
    #         db_obj = self.infinity_obj.create_database(f"restore_db_{suffix}", ConflictType.Error)
    #         table_obj = db_obj.create_table(
    #             f"test_table_{suffix}",
    #             {"c1": {"type": "int", "constraints": ["primary key"]}},
    #             ConflictType.Error
    #         )
    #         table_obj.insert([{"c1": 1}])
    #         
    #         # Create snapshot
    #         res = self.infinity_obj.create_database_snapshot(f"restore_db_{suffix}", f"db_restore_snap_{suffix}")
    #         assert res.error_code == ErrorCode.OK
    #         
    #         # Drop original database
    #         self.infinity_obj.drop_database(f"restore_db_{suffix}", ConflictType.Error)
    #         
    #         # Restore from snapshot
    #         res = self.infinity_obj.restore_database_snapshot(f"db_restore_snap_{suffix}")
    #         assert res.error_code == ErrorCode.OK
    #         
    #         # Verify restore
    #         restored_db = self.infinity_obj.get_database(f"restore_db_{suffix}")
    #         restored_table = restored_db.get_table(f"test_table_{suffix}")
    #         result = restored_table.output(["c1"])
    #         assert len(result) == 1
    #         assert result[0]["c1"] == 1
    #         
    #         # Cleanup
    #         self.infinity_obj.drop_database(f"restore_db_{suffix}", ConflictType.Error)


class TestSnapshotManagement:
    """Test snapshot management operations (list, show, drop)"""
    
    @pytest.mark.usefixtures("setup_class")
    @pytest.mark.usefixtures("suffix")
    class TestSnapshotList:
        """Test listing snapshots"""
        
        def test_list_snapshots_empty(self, suffix):
            """Test listing snapshots when none exist"""
            res = self.infinity_obj.list_snapshots()
            assert res.error_code == ErrorCode.OK
        
        def test_list_snapshots_with_data(self, suffix):
            """Test listing snapshots after creating some"""
            # Create some snapshots
            db_obj = self.infinity_obj.get_database("default_db")
            
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
            
            # Create multiple snapshots
            db_obj.create_table_snapshot(f"snap1_{suffix}", f"test_table_{suffix}")
            db_obj.create_table_snapshot(f"snap2_{suffix}", f"test_table_{suffix}")
            # self.infinity_obj.create_database_snapshot(f"list_db_{suffix}", f"db_snap_{suffix}")  # Not implemented yet
            
            # List snapshots
            res = self.infinity_obj.list_snapshots()
            assert res.error_code == ErrorCode.OK
            
            # Cleanup
            try:
                self.infinity_obj.drop_snapshot(f"snap1_{suffix}")
            except:
                pass
            try:
                self.infinity_obj.drop_snapshot(f"snap2_{suffix}")
            except:
                pass
            db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)
    
    @pytest.mark.usefixtures("setup_class")
    @pytest.mark.usefixtures("suffix")
    class TestSnapshotShow:
        """Test showing snapshot details"""
        
        def test_show_snapshot_exists(self, suffix):
            """Test showing details of existing snapshot"""
            # Create snapshot
            db_obj = self.infinity_obj.get_database("default_db")
            
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
            
            snapshot_name = f"show_snap_{suffix}"
            db_obj.create_table_snapshot(snapshot_name, f"test_table_{suffix}")
            
            # Show snapshot
            res = self.infinity_obj.show_snapshot(snapshot_name)
            assert res.error_code == ErrorCode.OK
            
            # Cleanup
            try:
                self.infinity_obj.drop_snapshot(snapshot_name)
            except:
                pass
            db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)
        
        def test_show_snapshot_not_exists(self, suffix):
            """Test showing details of non-existent snapshot"""
            with pytest.raises(InfinityException):
                self.infinity_obj.show_snapshot("non_existent_snapshot")
    
    @pytest.mark.usefixtures("setup_class")
    @pytest.mark.usefixtures("suffix")
    class TestSnapshotDrop:
        """Test dropping snapshots"""
        
        def test_drop_snapshot_exists(self, suffix):
            """Test dropping existing snapshot"""
            # Create snapshot
            db_obj = self.infinity_obj.get_database("default_db")
            
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
            
            snapshot_name = f"drop_snap_{suffix}"
            db_obj.create_table_snapshot(snapshot_name, f"test_table_{suffix}")
            
            # Drop snapshot
            res = self.infinity_obj.drop_snapshot(snapshot_name)
            assert res.error_code == ErrorCode.OK
            
            # Verify it's gone
            with pytest.raises(InfinityException):
                self.infinity_obj.show_snapshot(snapshot_name)
            
            # Cleanup
            db_obj.drop_table(f"test_table_{suffix}", ConflictType.Ignore)
        
        def test_drop_snapshot_not_exists(self, suffix):
            """Test dropping non-existent snapshot"""
            with pytest.raises(InfinityException):
                self.infinity_obj.drop_snapshot("non_existent_snapshot")


class TestSnapshotIntegration:
    """Integration tests for snapshot operations"""
    
    @pytest.mark.usefixtures("setup_class")
    @pytest.mark.usefixtures("suffix")
    class TestSnapshotWorkflow:
        """Test complete snapshot workflows"""
        
        def test_complete_table_snapshot_workflow(self, suffix):
            """Test complete table snapshot workflow: create -> modify -> restore"""
            # 1. Setup initial state
            db_obj = self.infinity_obj.get_database("default_db")
            
            # Drop table if it exists
            try:
                db_obj.drop_table(f"workflow_table_{suffix}", ConflictType.Ignore)
            except:
                pass
            
            table_obj = db_obj.create_table(
                f"workflow_table_{suffix}",
                {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "varchar"}},
                ConflictType.Error
            )
            
            # Insert initial data
            table_obj.insert([
                {"c1": 1, "c2": "original_data_1"},
                {"c1": 2, "c2": "original_data_2"}
            ])
            
            # 2. Create snapshot
            snapshot_name = f"workflow_snap_{suffix}"
            res = db_obj.create_table_snapshot(snapshot_name, f"workflow_table_{suffix}")
            assert res.error_code == ErrorCode.OK
            
            # 3. Modify table (add more data)
            table_obj.insert([
                {"c1": 3, "c2": "new_data_3"},
                {"c1": 4, "c2": "new_data_4"}
            ])
            
            # Verify current state has 4 rows
            result = table_obj.output(["c1", "c2"]).to_result()
            assert len(result[0]["c1"]) == 4
            
            # 4. Drop table
            db_obj.drop_table(f"workflow_table_{suffix}", ConflictType.Ignore)
            
            # 5. Restore from snapshot
            res = db_obj.restore_table_snapshot(snapshot_name)
            assert res.error_code == ErrorCode.OK
            
            # 6. Verify restored state (should only have original 2 rows)
            restored_table = db_obj.get_table(f"workflow_table_{suffix}")
            result = restored_table.output(["c1", "c2"]).to_result()
            assert len(result[0]["c1"]) == 2
            assert result[0]["c1"][0] == 1
            assert result[0]["c2"][0] == "original_data_1"
            assert result[0]["c1"][1] == 2
            assert result[0]["c2"][1] == "original_data_2"
            
            # 7. Cleanup
            res = self.infinity_obj.drop_snapshot(snapshot_name)
            assert res.error_code == ErrorCode.OK
            db_obj.drop_table(f"workflow_table_{suffix}", ConflictType.Ignore)
        
        def test_multiple_snapshots_same_table(self, suffix):
            """Test creating multiple snapshots of the same table"""
            db_obj = self.infinity_obj.get_database("default_db")
            
            # Drop table if it exists
            try:
                db_obj.drop_table(f"multi_table_{suffix}", ConflictType.Ignore)
            except:
                pass
            
            table_obj = db_obj.create_table(
                f"multi_table_{suffix}",
                {"c1": {"type": "int", "constraints": ["primary key"]}},
                ConflictType.Error
            )
            
            # Create multiple snapshots at different states
            table_obj.insert([{"c1": 1}])
            res = db_obj.create_table_snapshot(f"snap1_{suffix}", f"multi_table_{suffix}")
            assert res.error_code == ErrorCode.OK
            
            table_obj.insert([{"c1": 2}])
            res = db_obj.create_table_snapshot(f"snap2_{suffix}", f"multi_table_{suffix}")
            assert res.error_code == ErrorCode.OK
            
            table_obj.insert([{"c1": 3}])
            res = db_obj.create_table_snapshot(f"snap3_{suffix}", f"multi_table_{suffix}")
            assert res.error_code == ErrorCode.OK
            
            # Verify all snapshots exist
            res = self.infinity_obj.list_snapshots()
            assert res.error_code == ErrorCode.OK
            
            # Cleanup
            self.infinity_obj.drop_snapshot(f"snap1_{suffix}")
            self.infinity_obj.drop_snapshot(f"snap2_{suffix}")
            self.infinity_obj.drop_snapshot(f"snap3_{suffix}")
            db_obj.drop_table(f"multi_table_{suffix}", ConflictType.Ignore) 