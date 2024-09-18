import importlib
import sys
import os
import os
import pytest
from common import common_values
import infinity
import infinity_embedded
from infinity.errors import ErrorCode
from infinity.common import ConflictType, InfinityException

from common.utils import generate_big_int_csv, copy_data, generate_big_rows_csv, generate_big_columns_csv, generate_fvecs, generate_commas_enwiki
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http

@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")
@pytest.fixture(scope="class")
def setup_class(request, local_infinity, http):
    if local_infinity:
        module = importlib.import_module("infinity_embedded.common")
        func = getattr(module, 'ConflictType')
        globals()['ConflictType'] = func
        func = getattr(module, 'InfinityException')
        globals()['InfinityException'] = func
        uri = common_values.TEST_LOCAL_PATH
        request.cls.infinity_obj = infinity_embedded.connect(uri)
    elif http:
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

    def test_py_import(self):
        print("test_sdk")

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import(self, check_data, suffix):
        """
        target: test import data to remote server
        method: connect server, create table, import data, search, drop table, disconnect
        expect: all operations successfully
        """
        # infinity

        db_obj = self.infinity_obj.get_database("default_db")
        assert db_obj

        # import
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj.drop_table("test_import"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_import"+suffix, {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        # search
        res = table_obj.output(["c1"]).filter("c1 > 1").to_df()
        print(res)
        res = db_obj.drop_table("test_import"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test.fvecs",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("file_format", ["csv",
                                             "json",
                                             "jsonl",
                                             "fvecs"])
    def test_import_different_file_format_data(self, file_format, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("test_import_different_file_format_data"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_different_file_format_data"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        if file_format == "fvecs":
            file_name = "pysdk_test.fvecs"
            if not check_data:
                generate_fvecs(100, 128, file_name)
                copy_data(file_name)
            db_obj.drop_table("test_import_different_file_format_data"+suffix)
            table_obj = db_obj.create_table("test_import_different_file_format_data"+suffix,
                                            {"c1": {"type": "vector,128,float"}}, ConflictType.Error)
            table_obj.import_data(common_values.TEST_TMP_DIR + file_name, {"file_type": file_format})
            res = table_obj.output(["*"]).to_df()
            print(res)
        else:
            print(common_values.TEST_DATA_DIR + file_format + "/pysdk_test." + file_format)
            table_obj.import_data(
                os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/pysdk_test." + file_format,
                {"file_type": file_format})
            res = table_obj.output(["*"]).to_df()
            print(res)

        res = db_obj.drop_table("test_import_different_file_format_data"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("file_format", ["fvecs", "fvecs", "fvecs"])
    def test_import_empty_file_fvecs(self, file_format, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_empty_file_fvecs"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_empty_file_fvecs"+suffix,
                                        {"c1": {"type": "vector,128,float"}}, ConflictType.Error)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_empty_file_fvecs"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("file_format", ["csv", "csv", "csv"])
    def test_import_empty_file_csv(self, file_format, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_empty_file_csv"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_empty_file_csv"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_empty_file_csv"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("file_format", ["jsonl", "jsonl", "jsonl"])
    def test_import_empty_file_jsonl(self, file_format, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_empty_file_jsonl"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_empty_file_jsonl"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_empty_file_jsonl"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("file_format", [pytest.param("txt")])
    def test_import_format_unrecognized_data(self, file_format, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("test_import_format_unrecognized_data"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_format_unrecognized_data"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                                        ConflictType.Error)

        with pytest.raises(Exception):
            table_obj.import_data(
                os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/pysdk_test." + file_format,
                {"file_type": file_format})

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_format_unrecognized_data"+suffix, ConflictType.Error)

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
    def test_csv_with_different_delimiter(self, check_data, delimiter, types, suffix):
        if not check_data:
            copy_data("pysdk_test_" + delimiter[0] + ".csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_csv_with_different_delimiter"+suffix, ConflictType.Ignore)
        if not isinstance(types, tuple):
            table_obj = db_obj.create_table("test_csv_with_different_delimiter"+suffix,
                                            {"c1": {"type": types}, "c2": {"type": types}},
                                            ConflictType.Error)
            table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test_" + delimiter[0] + ".csv",
                                  import_options={
                                      "delimiter": delimiter[1]
                                  })
            res = table_obj.output(["*"]).to_df()
            print(res)
            db_obj.drop_table("test_csv_with_different_delimiter"+suffix, ConflictType.Error)
        else:
            table_obj = db_obj.create_table("test_csv_with_different_delimiter"+suffix, {
                "c1": {"type": types[0]}, "c2": {"type": types[0]}}, ConflictType.Error)
            with pytest.raises(InfinityException) as e:
                table_obj.import_data(common_values.TEST_TMP_DIR + "/pysdk_test_" + delimiter[0] + ".csv",
                                      import_options={
                                          "delimiter": delimiter[1]
                                      })

            assert e.type == InfinityException
            assert e.value.args[0] == types[1]
            db_obj.drop_table("test_csv_with_different_delimiter"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("delimiter", ["blankspace"])
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_blankspace.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_csv_with_different_delimiter_more_than_one_character(self, check_data, delimiter, suffix):
        if not check_data:
            copy_data("pysdk_test_" + delimiter + ".csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_csv_with_different_delimiter_more_than_one_character"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_csv_with_different_delimiter_more_than_one_character"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)
        table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test_" + delimiter + ".csv",
                              import_options={"delimiter": " "})

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_csv_with_different_delimiter_more_than_one_character"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("has_header", [True, False])
    def test_import_csv_with_headers(self, check_data, has_header, suffix):
        if not check_data:
            copy_data("pysdk_test_commas.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_csv_with_headers"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_csv_with_headers"+suffix, {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)
        table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test_commas.csv",
                              import_options={"header": has_header})
        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_csv_with_headers"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test.fvecs",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_fvecs_table_with_more_columns(self, check_data, suffix):
        file_name = "pysdk_test.fvecs"
        if not check_data:
            generate_fvecs(100, 128, file_name)
            copy_data(file_name)

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_fvecs_table_with_more_columns"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_fvecs_table_with_more_columns"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,128,float"}})

        with pytest.raises(InfinityException) as e:
            test_csv_dir = common_values.TEST_TMP_DIR + file_name
            table_obj.import_data(test_csv_dir, import_options={"file_type": "fvecs"})

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.IMPORT_FILE_FORMAT_ERROR

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_fvecs_table_with_more_columns"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", ["vector, 3, int",
                                       "vector, 128, int",
                                       "vector, 3, float",
                                       "vector, 128, float",
                                       "vector, 3, double"])
    def test_import_embedding_with_not_match_definition(self, check_data, types, suffix):
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_embedding_with_not_match_definition"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_embedding_with_not_match_definition"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": types}})

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_embedding_with_not_match_definition"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", [pytest.param("vector, 2, int")])
    def test_import_embedding_with_dimension_unmatch(self, check_data, types, suffix):
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_embedding_with_not_match_definition"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_embedding_with_not_match_definition"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": types}})

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"

        with pytest.raises(InfinityException) as e:
            table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.IMPORT_FILE_FORMAT_ERROR

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_embedding_with_not_match_definition"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", [pytest.param("vector, 3, bool"),
                                       pytest.param("vector, 3, varchar")
                                       ])
    def test_import_embedding_with_unmatched_elem_type(self, check_data, types, suffix):
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_embedding_with_not_match_definition"+suffix, ConflictType.Ignore)
        with pytest.raises(Exception):
            table_obj = db_obj.create_table("test_import_embedding_with_not_match_definition"+suffix,
                                            {"c1": {"type": "int"}, "c2": {"type": types}})
            test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
            res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})
            assert res.error_code == ErrorCode.OK
            res = table_obj.output(["*"]).to_df()
            print(res)
        db_obj.drop_table("test_import_embedding_with_not_match_definition"+suffix, ConflictType.Ignore)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_varchar.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_varchar_with_not_match_definition(self, check_data, suffix):
        if not check_data:
            copy_data("pysdk_test_varchar.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_varchar_with_not_match_definition"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_varchar_with_not_match_definition"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "varchar"}})

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_varchar.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_varchar_with_not_match_definition"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_int.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_10000_columns(self, check_data, suffix):
        file_name = "pysdk_test_big_int.csv"
        if not check_data:
            generate_big_int_csv(10000, file_name)
            copy_data(file_name)

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_10000_columns"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_10000_columns"+suffix, {"c1": {"type": "int"}, "c2": {"type": "int"}})

        test_csv_dir = common_values.TEST_TMP_DIR + file_name
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_10000_columns"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("columns", [
        pytest.param({"c1": {"type": "int"}}),
        pytest.param({"c1": {"type": "int"}, "c2": {"type": "int"}, "c3": {"type": "int"}})
    ])
    def test_table_with_not_matched_columns(self, columns, check_data, suffix):
        if not check_data:
            copy_data("pysdk_test_commas.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_not_matched_columns"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_table_with_not_matched_columns"+suffix, columns)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_commas.csv"
        with pytest.raises(InfinityException) as e:
            table_obj.import_data(test_csv_dir)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.COLUMN_COUNT_MISMATCH or e.value.args[0] == ErrorCode.IMPORT_FILE_FORMAT_ERROR

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_table_with_not_matched_columns"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_import_with_different_size.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("data_size", [1, 8191, 8192, 8193, 16 * 8192])
    def test_import_with_different_size(self, check_data, data_size, suffix):
        generate_big_rows_csv(data_size, "pysdk_test_import_with_different_size.csv")
        copy_data("pysdk_test_import_with_different_size.csv")

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_with_different_size"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_with_different_size"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "varchar"}})

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_import_with_different_size.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == data_size
        db_obj.drop_table("test_import_with_different_size"+suffix, ConflictType.Ignore)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_varchar_rows.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_exceeding_rows(self, check_data, suffix):
        if not check_data:
            generate_big_rows_csv(1024 * 8192, "pysdk_test_big_varchar_rows.csv")
            copy_data("pysdk_test_big_varchar_rows.csv")

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_exceeding_rows"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_exceeding_rows"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "varchar"}})

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_big_varchar_rows.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == 1024 * 8192
        db_obj.drop_table("test_import_exceeding_rows"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_columns.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_exceeding_columns(self, check_data, suffix):
        generate_big_columns_csv(1024, "pysdk_test_big_columns.csv")
        if not check_data:
            copy_data("pysdk_test_big_columns.csv")

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_exceeding_columns"+suffix, ConflictType.Ignore)
        columns = {"c" + str(i): {"type": "int"} for i in range(1024)}
        table_obj = db_obj.create_table("test_import_exceeding_columns"+suffix, columns, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_big_columns.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_exceeding_columns"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "test_default.jsonl",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_jsonl_file_with_default(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_jsonl_file_with_default"+suffix, ConflictType.Ignore)
        if not check_data:
            copy_data("test_default.jsonl")

        columns = {
            "c1": {
                "type": "int",
                "default": 1,
            },
            "c2": {
                "type": "int",
                "default": 4,
            },
            "c3": {
                "type": "int",
                "default": 7,
            },
            "c4": {
                "type": "vector, 3, float",
                "constraints": ["primary key"],
                "default": [1.0, 2.0, 3.0],
            }
        }
        table_obj = db_obj.create_table("test_import_jsonl_file_with_default"+suffix, columns, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "test_default.jsonl"
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "jsonl"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_jsonl_file_with_default"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_import_default.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_csv_file_with_default(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_csv_file_with_default"+suffix, ConflictType.Ignore)
        if not check_data:
            copy_data("pysdk_test_import_default.csv")

        columns = {
            "c1": {
                "type": "int",
                "default": 1,
            },
            "c2": {
                "type": "int",
                "default": 4,
            },
            "c4": {
                "type": "vector, 3, float",
                "constraints": ["primary key"],
                "default": [1.0, 2.0, 3.0],
            }
        }
        table_obj = db_obj.create_table("test_import_csv_file_with_default"+suffix, columns, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_import_default.csv"
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_csv_file_with_default"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_default.json",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_json_file_with_default(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_json_file_with_default"+suffix, ConflictType.Ignore)
        if not check_data:
            copy_data("pysdk_test_default.json")

        columns = {
            "c1": {
                "type": "int",
                "default": 1,
            },
            "c2": {
                "type": "int",
                "default": 4,
            },
            "c3": {
                "type": "int",
                "default": 7,
            },
            "c4": {
                "type": "vector, 3, float",
                "default": [1.1, 2.5, 3.0],
            }
        }
        table_obj = db_obj.create_table("test_import_json_file_with_default"+suffix, columns, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_default.json"
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "json"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_json_file_with_default"+suffix, ConflictType.Error)
