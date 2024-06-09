import pytest
from common import common_values
from src.test_py_import import TestImport
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
from infinity.errors import ErrorCode

class TestLocalInfinity():
    def test_py_import(self):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj.test_version()

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import(self, check_data):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import(check_data)
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test.fvecs",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("file_format", ["csv",
                                             "json",
                                             "jsonl",
                                             "fvecs"])
    def test_import_different_file_format_data(self, file_format, check_data):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_different_file_format_data(file_format, check_data)

    @pytest.mark.parametrize("file_format", ["fvecs", "fvecs", "fvecs"])
    def test_import_empty_file_fvecs(self, file_format):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_empty_file_fvecs(file_format)

    @pytest.mark.parametrize("file_format", ["csv", "csv", "csv"])
    def test_import_empty_file_csv(self, file_format):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_empty_file_csv(file_format)

    @pytest.mark.parametrize("file_format", ["jsonl", "jsonl", "jsonl"])
    def test_import_empty_file_jsonl(self, file_format):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_empty_file_jsonl(file_format)
    @pytest.mark.parametrize("file_format", [
        pytest.param("txt")])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_format_unrecognized_data(self, get_infinity_db, file_format):
        test_inf_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_inf_obj._test_import_format_unrecognized_data(get_infinity_db, file_format)

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
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_csv_with_different_delimiter(self, get_infinity_db, check_data, delimiter, types):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_csv_with_different_delimiter(get_infinity_db, check_data, delimiter, types)

    @pytest.mark.parametrize("delimiter", ["blankspace"])
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_blankspace.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_csv_with_different_delimiter_more_than_one_character(self, get_infinity_db, check_data, delimiter):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_csv_with_different_delimiter_more_than_one_character(get_infinity_db, check_data, delimiter)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("has_header", [True, False])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_csv_with_headers(self, get_infinity_db, check_data, has_header):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_csv_with_headers(get_infinity_db, check_data, has_header)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test.fvecs",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_fvecs_table_with_more_columns(self, get_infinity_db, check_data):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_fvecs_table_with_more_columns(get_infinity_db, check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", ["vector, 3, int",
                                       "vector, 128, int",
                                       "vector, 3, float",
                                       "vector, 128, float",
                                       "vector, 3, double"])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_embedding_with_not_match_definition(self, get_infinity_db, check_data, types):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_embedding_with_not_match_definition(get_infinity_db, check_data, types)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", [pytest.param("vector, 2, int")])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_embedding_with_dimension_unmatch(self, get_infinity_db, check_data, types):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_embedding_with_dimension_unmatch(get_infinity_db, check_data, types)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", [pytest.param("vector, 3, bool"),
                                       pytest.param("vector, 3, varchar")
                                       ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_embedding_with_unmatched_elem_type(self, get_infinity_db, check_data, types):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_embedding_with_unmatched_elem_type(get_infinity_db, check_data, types)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_varchar.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_varchar_with_not_match_definition(self, get_infinity_db, check_data):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_varchar_with_not_match_definition(get_infinity_db, check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_int.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_10000_columns(self, get_infinity_db, check_data):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_10000_columns(get_infinity_db, check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("columns", [
        pytest.param({"c1": {"type": "int"}}),
        pytest.param({"c1": {"type": "int"}, "c2": {"type": "int"}, "c3": {"type": "int"}})
    ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_table_with_not_matched_columns(self, get_infinity_db, columns, check_data):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_table_with_not_matched_columns(get_infinity_db, columns, check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_import_with_different_size.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("data_size", [1, 8191, 8192, 8193, 16 * 8192])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_with_different_size(self, get_infinity_db, check_data, data_size):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_with_different_size(get_infinity_db, check_data, data_size)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_varchar_rows.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_exceeding_rows(self, get_infinity_db, check_data):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_exceeding_rows(get_infinity_db, check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_columns.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_exceeding_columns(self, get_infinity_db, check_data):
        test_infinity_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_exceeding_columns(get_infinity_db, check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "test_default.jsonl",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_jsonl_file_with_default(self, get_infinity_db, check_data):
        test_inf_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_inf_obj._test_import_jsonl_file_with_default(get_infinity_db, check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_import_default.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_csv_file_with_default(self, get_infinity_db, check_data):
        test_inf_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_inf_obj._test_import_csv_file_with_default(get_infinity_db, check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_default.json",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_json_file_with_default(self, get_infinity_db, check_data):
        test_inf_obj = TestImport(common_values.TEST_LOCAL_PATH)
        test_inf_obj._test_import_json_file_with_default(get_infinity_db, check_data)
