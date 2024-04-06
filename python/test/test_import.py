# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import pytest
from infinity import index

from common import common_values
import infinity
from infinity.errors import ErrorCode
from infinity.common import ConflictType

from utils import generate_big_int_csv, copy_data, generate_big_rows_csv, generate_big_columns_csv, generate_fvecs, \
    generate_commas_enwiki


class TestImport:

    def test_version(self):
        print(infinity.__version__)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import(self, check_data):
        """
        target: test import data to remote server
        method: connect server, create table, import data, search, drop table, disconnect
        expect: all operations successfully
        """

        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        assert infinity_obj

        # infinity

        db_obj = infinity_obj.get_database("default")
        assert db_obj

        # import
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj.drop_table("test_import", True)
        table_obj = db_obj.create_table(
            "test_import", {"c1": "int", "c2": "vector,3,int"}, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        # search
        res = table_obj.output(["c1"]).filter("c1 > 1").to_df()
        print(res)
        res = db_obj.drop_table("test_import")
        assert res.error_code == ErrorCode.OK

    # import different file format data
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test.fvecs",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("file_format", ["csv",
                                             pytest.param("json", marks=pytest.mark.xfail(
                                                 reason="ERROR:3032, Import JSON is not implemented yet")),
                                             "fvecs"])
    def test_import_different_file_format_data(self, file_format, check_data):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")

        db_obj.drop_table("test_import_different_file_format_data")
        table_obj = db_obj.create_table("test_import_different_file_format_data",
                                        {"c1": "int", "c2": "vector,3,int"}, ConflictType.Error)

        if file_format == "fvecs":
            if not check_data:
                generate_fvecs(100, 128, "pysdk_test.fvecs")
                copy_data("pysdk_test.fvecs")
            db_obj.drop_table("test_import_different_file_format_data_fvecs")
            table_obj = db_obj.create_table("test_import_different_file_format_data_fvecs",
                                            {"c1": "vector,128,float"}, ConflictType.Error)
            table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test.fvecs", {"file_type": file_format})
            res = table_obj.output(["*"]).to_df()
            print(res)

        print(common_values.TEST_DATA_DIR + file_format + "/pysdk_test." + file_format)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/pysdk_test." + file_format,
                              {"file_type": file_format})
        res = table_obj.output(["*"]).to_df()
        print(res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # import empty FVECS file
    @pytest.mark.parametrize("file_format", ["fvecs", "fvecs", "fvecs"])
    def test_import_empty_file_fvecs(self, file_format):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_import_empty_file_fvecs")
        table_obj = db_obj.create_table("test_import_empty_file_fvecs",
                                        {"c1": "vector,128,float"}, ConflictType.Error)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # import empty CSV file
    @pytest.mark.parametrize("file_format", ["csv", "csv", "csv"])
    def test_import_empty_file_csv(self, file_format):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_import_empty_file_csv")
        table_obj = db_obj.create_table("test_import_empty_file_csv",
                                        {"c1": "int", "c2": "vector,3,int"}, ConflictType.Error)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # import empty JSONL file
    @pytest.mark.parametrize("file_format", ["jsonl", "jsonl", "jsonl"])
    def test_import_empty_file_jsonl(self, file_format):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_import_empty_file_jsonl")
        table_obj = db_obj.create_table("test_import_empty_file_jsonl",
                                        {"c1": "int", "c2": "vector,3,int"}, ConflictType.Error)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # import format unrecognized data
    @pytest.mark.parametrize("file_format", [
        pytest.param("json"),
        pytest.param("txt")])
    def test_import_format_unrecognized_data(self, get_infinity_db, file_format):
        db_obj = get_infinity_db

        db_obj.drop_table("test_import_format_unrecognized_data")
        table_obj = db_obj.create_table("test_import_format_unrecognized_data", {"c1": "int", "c2": "vector,3,int"},
                                        ConflictType.Error)

        with pytest.raises(Exception):
            table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/pysdk_test." + file_format,
                                  {"file_type": file_format})

        res = table_obj.output(["*"]).to_df()
        print(res)

    # import csv with different delimiter
    @pytest.mark.parametrize("delimiter", [["blankspace", " "],
                                           ["commas", ","],
                                           ["semicolons", ";"],
                                           pytest.param(["tabular", "\t"])
                                           ])
    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        ("bool", 7012)
    ])
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_blankspace.csv",
                                             "data_dir": common_values.TEST_TMP_DIR},
                                            {"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR},
                                            {"file_name": "pysdk_test_semicolons.csv",
                                             "data_dir": common_values.TEST_TMP_DIR},
                                            {"file_name": "pysdk_test_tabular.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_csv_with_different_delimiter(self, get_infinity_db, check_data, delimiter, types):
        if not check_data:
            copy_data("pysdk_test_" + delimiter[0] + ".csv")
        db_obj = get_infinity_db
        db_obj.drop_table("test_csv_with_different_delimiter")
        if not isinstance(types, tuple):
            table_obj = db_obj.create_table("test_csv_with_different_delimiter", {"c1": types, "c2": types},
                                            ConflictType.Error)
            table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test_" + delimiter[0] + ".csv",
                                import_options={
                                    "delimiter": delimiter[1]
                                })
            res = table_obj.output(["*"]).to_df()
            print(res)
        else:
            table_obj = db_obj.create_table("test_csv_with_different_delimiter", {"c1": types[0], "c2": types[0]},
                                            ConflictType.Error)
            with pytest.raises(Exception, match=f"ERROR:{types[1]}*"):
                table_obj.import_data(common_values.TEST_TMP_DIR + "/pysdk_test_" + delimiter[0] + ".csv",
                                    import_options={
                                        "delimiter": delimiter[1]
                                    })

    # import csv with delimiter more than one character
    @pytest.mark.parametrize("delimiter", ["blankspace"])
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_blankspace.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_csv_with_different_delimiter_more_than_one_character(self, get_infinity_db, check_data, delimiter):
        if not check_data:
            copy_data("pysdk_test_" + delimiter + ".csv")
        db_obj = get_infinity_db
        db_obj.drop_table("test_csv_with_different_delimiter_more_than_one_character")
        table_obj = db_obj.create_table("test_csv_with_different_delimiter_more_than_one_character",
                                        {"c1": "int", "c2": "int"}, ConflictType.Error)
        table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test_" + delimiter + ".csv",
                              import_options={"delimiter": " "})

        res = table_obj.output(["*"]).to_df()
        print(res)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("has_header", [True, False])
    def test_import_csv_with_headers(self, get_infinity_db, check_data, has_header):
        if not check_data:
            copy_data("pysdk_test_commas.csv")
        db_obj = get_infinity_db
        db_obj.drop_table("test_import_csv_with_headers", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_csv_with_headers", {"c1": "int", "c2": "int"}, ConflictType.Error)
        table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test_commas.csv",
                              import_options={"header": has_header})
        res = table_obj.output(["*"]).to_pl()
        print(res)

    # import fvecs, when table with more columns
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test.fvecs",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_fvecs_table_with_more_columns(self, get_infinity_db, check_data):
        if not check_data:
            generate_fvecs(100, 128, "pysdk_test.fvecs")
            copy_data("pysdk_test.fvecs")

        db_obj = get_infinity_db
        db_obj.drop_table("test_import_fvecs_table_with_more_columns")
        table_obj = db_obj.create_table("test_import_fvecs_table_with_more_columns",
                                        {"c1": "int", "c2": "vector,128,float"})
        with pytest.raises(Exception, match="ERROR:3037*"):
            test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test.fvecs"
            res = table_obj.import_data(test_csv_dir, import_options={"file_type": "fvecs"})
            assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)

    # TODO import json, json has nested structure

    # import table with embedding not match with the table definition.
    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", ["vector, 3, int",
                                       "vector, 128, int",
                                       "vector, 3, float",
                                       "vector, 128, float",
                                       "vector, 3, double"])
    def test_import_embedding_with_not_match_definition(self, get_infinity_db, check_data, types):
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj = get_infinity_db
        db_obj.drop_table("test_import_embedding_with_not_match_definition")
        table_obj = db_obj.create_table("test_import_embedding_with_not_match_definition", {"c1": "int", "c2": types})

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", [pytest.param("vector, 2, int")])
    def test_import_embedding_with_dimension_unmatch(self, get_infinity_db, check_data, types):
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj = get_infinity_db
        db_obj.drop_table("test_import_embedding_with_not_match_definition")
        table_obj = db_obj.create_table("test_import_embedding_with_not_match_definition", {"c1": "int", "c2": types})

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
        with pytest.raises(Exception, match="ERROR:3037, Import file format error:*"):
            res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})
            assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("types", [pytest.param("vector, 3, bool"),
                                       pytest.param("vector, 3, varchar")
                                       ])
    def test_import_embedding_with_unmatched_elem_type(self, get_infinity_db, check_data, types):
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj = get_infinity_db
        db_obj.drop_table("test_import_embedding_with_not_match_definition")
        with pytest.raises(Exception):
            table_obj = db_obj.create_table("test_import_embedding_with_not_match_definition", {"c1": "int", "c2": types})
            test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
            res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})
            assert res.error_code == ErrorCode.OK
            res = table_obj.output(["*"]).to_df()
            print(res)

    # import table with varchar not match with the table definition.
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_varchar.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_varchar_with_not_match_definition(self, get_infinity_db, check_data):
        if not check_data:
            copy_data("pysdk_test_varchar.csv")
        db_obj = get_infinity_db
        db_obj.drop_table("test_import_varchar_with_not_match_definition")
        table_obj = db_obj.create_table("test_import_varchar_with_not_match_definition", {"c1": "int", "c2": "varchar"})

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_varchar.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_pl()
        print(res)

    # import table with 10000 columns.
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_int.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_10000_columns(self, get_infinity_db, check_data):
        if not check_data:
            generate_big_int_csv(10000, "pysdk_test_big_int.csv")
            copy_data("pysdk_test_big_int.csv")

        db_obj = get_infinity_db
        db_obj.drop_table("test_import_10000_columns")
        table_obj = db_obj.create_table("test_import_10000_columns", {"c1": "int", "c2": "int"})

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_big_int.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)

    # import table with columns isn't matched (more and less)
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("columns", [
        pytest.param({"c1": "int"}),
        pytest.param({"c1": "int", "c2": "int", "c3": "int"})
    ])
    def test_table_with_not_matched_columns(self, get_infinity_db, columns, check_data):
        if not check_data:
            copy_data("pysdk_test_commas.csv")
        db_obj = get_infinity_db
        db_obj.drop_table("test_table_with_not_matched_columns")
        table_obj = db_obj.create_table("test_table_with_not_matched_columns", columns)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_commas.csv"
        with pytest.raises(Exception, match="ERROR:3039, Column count mismatch: CSV file row count isn't match with table schema*"):
            res = table_obj.import_data(test_csv_dir)
            assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)

    # import table with column value exceeding invalid value range
    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_varchar_rows.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_exceeding_rows(self, get_infinity_db, check_data):
        if not check_data:
            generate_big_rows_csv(1024 * 8192, "pysdk_test_big_varchar_rows.csv")
            copy_data("pysdk_test_big_varchar_rows.csv")

        db_obj = get_infinity_db
        db_obj.drop_table("test_import_exceeding_rows")
        table_obj = db_obj.create_table("test_import_exceeding_rows", {"c1": "int", "c2": "varchar"})

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_big_varchar_rows.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_big_columns.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_import_exceeding_columns(self, get_infinity_db, check_data):
        generate_big_columns_csv(1024, "pysdk_test_big_columns.csv")
        if not check_data:
            copy_data("pysdk_test_big_columns.csv")

        db_obj = get_infinity_db
        db_obj.drop_table("test_import_exceeding_columns")
        columns = {"c" + str(i): "int" for i in range(1024)}
        table_obj = db_obj.create_table("test_import_exceeding_columns", columns)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_big_columns.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)

    # TODO: JSON file type import test