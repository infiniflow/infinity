import pytest
from common import common_values
from src.test_py_import import TestImport
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
from infinity.errors import ErrorCode

@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")
@pytest.fixture(scope="class")
def setup_class(request, local_infinity):
    if local_infinity:
        uri = common_values.TEST_LOCAL_PATH
    else:
        uri = common_values.TEST_LOCAL_HOST
    request.cls.uri = uri
    request.cls.test_infinity_obj = TestImport(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    @pytest.fixture
    def skip_setup_marker(self, request):
        request.node.skip_setup = True

    def test_py_import(self):
        self.test_infinity_obj.test_version()

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import(self, check_data):
        self.test_infinity_obj._test_import(check_data)
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test.fvecs",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("file_format", ["csv",
                                             "json",
                                             "jsonl",
                                             "fvecs"])
    def test_import_different_file_format_data(self, file_format, check_data):
        self.test_infinity_obj._test_import_different_file_format_data(file_format, check_data)

    @pytest.mark.parametrize("file_format", ["fvecs", "fvecs", "fvecs"])
    def test_import_empty_file_fvecs(self, file_format):
        self.test_infinity_obj._test_import_empty_file_fvecs(file_format)

    @pytest.mark.parametrize("file_format", ["csv", "csv", "csv"])
    def test_import_empty_file_csv(self, file_format):
        self.test_infinity_obj._test_import_empty_file_csv(file_format)

    @pytest.mark.parametrize("file_format", ["jsonl", "jsonl", "jsonl"])
    def test_import_empty_file_jsonl(self, file_format):
        self.test_infinity_obj._test_import_empty_file_jsonl(file_format)
    @pytest.mark.parametrize("file_format", [pytest.param("txt")])
    def test_import_format_unrecognized_data(self, file_format):
        self.test_infinity_obj._test_import_format_unrecognized_data(file_format)

    @pytest.mark.parametrize("delimiter", [["blankspace", " "],
                                           ["commas", ","],
                                           ["semicolons", ";"],
                                           pytest.param(["tabular", "\t"])
                                           ])
    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        ("bool", ErrorCode.PARSER_ERROR)
    ])
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_blankspace.csv",
                                             "data_dir": common_values.TEST_TMP_DIR},
                                            {"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR},
                                            {"file_name": "pysdk_test_semicolons.csv",
                                             "data_dir": common_values.TEST_TMP_DIR},
                                            {"file_name": "pysdk_test_tabular.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_csv_with_different_delimiter(self, check_data, delimiter, types):
        self.test_infinity_obj._test_csv_with_different_delimiter(check_data, delimiter, types)

    @pytest.mark.parametrize("delimiter", ["blankspace"])
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_blankspace.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_csv_with_different_delimiter_more_than_one_character(self, check_data, delimiter):
        self.test_infinity_obj._test_csv_with_different_delimiter_more_than_one_character(check_data, delimiter)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("has_header", [True, False])
    def test_import_csv_with_headers(self, check_data, has_header):
        self.test_infinity_obj._test_import_csv_with_headers(check_data, has_header)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test.fvecs",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_fvecs_table_with_more_columns(self, check_data):
        self.test_infinity_obj._test_import_fvecs_table_with_more_columns(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", ["vector, 3, int",
                                       "vector, 128, int",
                                       "vector, 3, float",
                                       "vector, 128, float",
                                       "vector, 3, double"])
    def test_import_embedding_with_not_match_definition(self, check_data, types):
        self.test_infinity_obj._test_import_embedding_with_not_match_definition(check_data, types)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", [pytest.param("vector, 2, int")])
    def test_import_embedding_with_dimension_unmatch(self, check_data, types):
        self.test_infinity_obj._test_import_embedding_with_dimension_unmatch(check_data, types)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", [pytest.param("vector, 3, bool"),
                                       pytest.param("vector, 3, varchar")
                                       ])
    def test_import_embedding_with_unmatched_elem_type(self, check_data, types):
        self.test_infinity_obj._test_import_embedding_with_unmatched_elem_type(check_data, types)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_varchar.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_varchar_with_not_match_definition(self, check_data):
        self.test_infinity_obj._test_import_varchar_with_not_match_definition(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_int.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_10000_columns(self, check_data):
        self.test_infinity_obj._test_import_10000_columns(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("columns", [
        pytest.param({"c1": {"type": "int"}}),
        pytest.param({"c1": {"type": "int"}, "c2": {"type": "int"}, "c3": {"type": "int"}})
    ])
    def test_table_with_not_matched_columns(self, columns, check_data):
        self.test_infinity_obj._test_table_with_not_matched_columns(columns, check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_import_with_different_size.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("data_size", [1, 8191, 8192, 8193, 16 * 8192])
    def test_import_with_different_size(self, check_data, data_size):
        self.test_infinity_obj._test_import_with_different_size(check_data, data_size)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_varchar_rows.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_exceeding_rows(self, check_data):
        self.test_infinity_obj._test_import_exceeding_rows(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_columns.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_exceeding_columns(self, check_data):
        self.test_infinity_obj._test_import_exceeding_columns(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "test_default.jsonl",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_jsonl_file_with_default(self, check_data):
        self.test_infinity_obj._test_import_jsonl_file_with_default(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_import_default.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_csv_file_with_default(self, check_data):
        self.test_infinity_obj._test_import_csv_file_with_default(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_default.json",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_json_file_with_default(self, check_data):
        self.test_infinity_obj._test_import_json_file_with_default(check_data)
