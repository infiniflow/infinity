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

    def test_snapshot_not_implemented_yet(self, suffix):
        """
        target: test that snapshot operations return "not implemented yet" error
        method: call various snapshot operations using the correct Thrift interface
        expect: all operations return ErrorCode.NOT_SUPPORTED with appropriate error message
        """
        db_obj = self.infinity_obj.create_database("snapshot_test_db" + suffix, ConflictType.Error)
        table_obj = db_obj.create_table(
            "test_table" + suffix,
            {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error
        )

        # Helper to check for correct exception or response
        def expect_not_implemented(call):
            try:
                res = call()
                assert res.error_code == ErrorCode.NOT_SUPPORTED
                assert "not implemented yet" in res.error_msg.lower()
            except infinity.common.InfinityException as e:
                assert e.args[0] == ErrorCode.NOT_SUPPORTED or e.args[0] == 3032
                assert "not implemented yet" in str(e.args[1]).lower()
            except AttributeError:
                pytest.skip("Method not yet implemented in Python SDK")

        # Test CreateTableSnapshot
        expect_not_implemented(lambda: db_obj.create_table_snapshot("test_snapshot", "test_table" + suffix))
        # Test CreateDatabaseSnapshot
        expect_not_implemented(lambda: self.infinity_obj.create_database_snapshot("test_db_snapshot", "test_db" + suffix))
        # Test CreateSystemSnapshot
        expect_not_implemented(lambda: self.infinity_obj.create_system_snapshot("test_system_snapshot"))
        # Test RestoreSystemSnapshot
        expect_not_implemented(lambda: self.infinity_obj.restore_system_snapshot("test_snapshot"))
        # Test RestoreTableSnapshot
        expect_not_implemented(lambda: db_obj.restore_table_snapshot("test_snapshot"))
        # Test ShowSnapshot
        expect_not_implemented(lambda: self.infinity_obj.show_snapshot("test_snapshot"))
        # Test ListSnapshots
        expect_not_implemented(lambda: self.infinity_obj.list_snapshots())
        # Test DropSnapshot
        expect_not_implemented(lambda: self.infinity_obj.drop_snapshot("test_snapshot"))

        # Cleanup
        res = db_obj.drop_table("test_table" + suffix)
        assert res.error_code == ErrorCode.OK
        res = self.infinity_obj.drop_database("snapshot_test_db" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
