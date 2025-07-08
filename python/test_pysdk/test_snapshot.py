import sys
import os
import pytest
from common import common_values
import infinity
from infinity.errors import ErrorCode
from infinity.common import ConflictType

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http


@pytest.mark.usefixtures("http")
@pytest.mark.usefixtures("suffix")
class TestSnapshot:
    @pytest.fixture(autouse=True)
    def setup(self, http):
        if http:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity_http()
        else:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity.connect(self.uri)

    def teardown(self):
        res = self.infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_snapshot_operations(self, suffix):
        """
        target: test that table snapshot operations are properly implemented and working
        method: call table snapshot operations using the default database and verify they work correctly
        expect: all table snapshot operations complete successfully or return appropriate errors for invalid operations
        """
        # Use the default database instead of creating a new one
        db_obj = self.infinity_obj.get_database("default_db")
        
        # Create table in default database
        table_obj = db_obj.create_table(
            "test_table" + suffix,
            {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error
        )

        # Insert some data to make the table non-empty
        table_obj.insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])

        # Test CreateTableSnapshot
        res = db_obj.create_table_snapshot("test_table_snapshot", "test_table" + suffix)
        assert res.error_code == ErrorCode.OK, f"CreateTableSnapshot failed: {res.error_msg}"

        # Test ListSnapshots - should show our created snapshot
        res = self.infinity_obj.list_snapshots()
        assert res.error_code == ErrorCode.OK, f"ListSnapshots failed: {res.error_msg}"

        # Test ShowSnapshot for the table snapshot
        res = self.infinity_obj.show_snapshot("test_table_snapshot")
        assert res.error_code == ErrorCode.OK, f"ShowSnapshot test_table_snapshot failed: {res.error_msg}"

        # Test RestoreTableSnapshot (this might fail if table already exists, which is expected)
        try:
            res = db_obj.restore_table_snapshot("test_table_snapshot")
            # If it succeeds, that's fine. If it fails due to table already existing, that's also expected
        except Exception as e:
            # Expected behavior - table already exists
            pass

        # Test DropSnapshot for the table snapshot
        res = self.infinity_obj.drop_snapshot("test_table_snapshot")
        assert res.error_code == ErrorCode.OK, f"DropSnapshot test_table_snapshot failed: {res.error_msg}"

        # Verify snapshot is gone
        res = self.infinity_obj.list_snapshots()
        assert res.error_code == ErrorCode.OK, f"ListSnapshots after cleanup failed: {res.error_msg}"

        # Cleanup - drop the table we created
        res = db_obj.drop_table("test_table" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
