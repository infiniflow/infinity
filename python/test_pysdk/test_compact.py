import importlib
import sys
import os
import os
import pytest
from common import common_values
import infinity
from infinity.errors import ErrorCode
from infinity.common import ConflictType, InfinityException

from common.utils import generate_big_int_csv, copy_data, generate_big_rows_csv, generate_big_columns_csv, generate_fvecs, generate_commas_enwiki
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
class TestInfinity:
    @pytest.fixture
    def skip_setup_marker(self, request):
        request.node.skip_setup = True

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_compact(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        assert db_obj

        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj.drop_table("test_compact"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_compact"+suffix, {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        # Import to create 4 segments
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.show_segments()
        print(res)
        assert len(res) == 4

        res, extra_result = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == 12

        # Compact
        res = table_obj.compact()

        # If compact is conflicted with background compact, sleep to wait for background compact to finish.
        if res.error_code != ErrorCode.OK:
            sleep(2)

        res = table_obj.show_segments()
        assert len(res) == 1

        res, extra_result = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == 12

        res = db_obj.drop_table("test_compact"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
