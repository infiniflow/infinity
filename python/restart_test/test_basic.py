from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from common import common_values
from infinity.common import ConflictType
from infinity.errors import ErrorCode


def test_0(infinity_runner: InfinityRunner):
    infinity_runner.clear()
    uri = common_values.TEST_LOCAL_HOST

    decorator = infinity_runner_decorator_factory(None, uri, infinity_runner)

    @decorator
    def part(infinity_obj):
        db_obj = infinity_obj.create_database("my_db", ConflictType.Error)
        assert db_obj is not None
        res = infinity_obj.drop_database("my_db", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    for i in range(2):
        print(f"i: {i}")
        part()
