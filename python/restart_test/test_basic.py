from infinity_runner import InfinityRunner
from common import common_values
from infinity.common import ConflictType
from infinity.errors import ErrorCode


def test_0(infinity_runner: InfinityRunner):
    infinity_runner.clear()
    uri = common_values.TEST_LOCAL_HOST
    for i in range(2):
        print(f"i: {i}")
        infinity_runner.init(None)
        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.create_database("my_db", ConflictType.Error)
        assert db_obj is not None
        res = infinity_obj.drop_database("my_db", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        infinity_obj.disconnect()

        infinity_runner.uninit()
