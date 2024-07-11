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
from infinity.common import ConflictType, InfinityException
from internal.test_sdkbase import TestSdk

from internal.utils import generate_big_int_csv, copy_data, generate_big_rows_csv, generate_big_columns_csv, generate_fvecs, \
    generate_commas_enwiki


class TestImport(TestSdk):

    # def _test_version(self):
    #     print(infinity.__version__)

    def _test_import(self, check_data):
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
        db_obj.drop_table("test_import", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_import", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        # search
        res = table_obj.output(["c1"]).filter("c1 > 1").to_df()
        print(res)
        res = db_obj.drop_table("test_import", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # import different file format data
    def _test_import_different_file_format_data(self, file_format, check_data):
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("test_import_different_file_format_data", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_different_file_format_data",
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        if file_format == "fvecs":
            file_name = "pysdk_test.fvecs"
            if not check_data:
                generate_fvecs(100, 128, file_name)
                copy_data(file_name)
            db_obj.drop_table("test_import_different_file_format_data")
            table_obj = db_obj.create_table("test_import_different_file_format_data",
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

        res = db_obj.drop_table("test_import_different_file_format_data", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # import empty FVECS file
    def _test_import_empty_file_fvecs(self, file_format):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_empty_file_fvecs", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_empty_file_fvecs",
                                        {"c1": {"type": "vector,128,float"}}, ConflictType.Error)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_empty_file_fvecs", ConflictType.Error)

    # import empty CSV file
    def _test_import_empty_file_csv(self, file_format):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_empty_file_csv", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_empty_file_csv",
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_empty_file_csv", ConflictType.Error)

    # import empty JSONL file
    def _test_import_empty_file_jsonl(self, file_format):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_empty_file_jsonl", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_empty_file_jsonl",
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)
        table_obj.import_data(os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_empty_file_jsonl", ConflictType.Error)


    # import format unrecognized data
    def _test_import_format_unrecognized_data(self, file_format):
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("test_import_format_unrecognized_data", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_format_unrecognized_data",
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                                        ConflictType.Error)

        with pytest.raises(Exception):
            table_obj.import_data(
                os.getcwd() + common_values.TEST_DATA_DIR + file_format + "/pysdk_test." + file_format,
                {"file_type": file_format})

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_format_unrecognized_data", ConflictType.Error)

    # import csv with different delimiter
    def _test_csv_with_different_delimiter(self, check_data, delimiter, types):
        if not check_data:
            copy_data("pysdk_test_" + delimiter[0] + ".csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_csv_with_different_delimiter", ConflictType.Ignore)
        if not isinstance(types, tuple):
            table_obj = db_obj.create_table("test_csv_with_different_delimiter",
                                            {"c1": {"type": types}, "c2": {"type": types}},
                                            ConflictType.Error)
            table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test_" + delimiter[0] + ".csv",
                                  import_options={
                                      "delimiter": delimiter[1]
                                  })
            res = table_obj.output(["*"]).to_df()
            print(res)
            db_obj.drop_table("test_csv_with_different_delimiter", ConflictType.Error)
        else:
            table_obj = db_obj.create_table("test_csv_with_different_delimiter", {
                "c1": {"type": types[0]}, "c2": {"type": types[0]}}, ConflictType.Error)
            with pytest.raises(InfinityException) as e:
                table_obj.import_data(common_values.TEST_TMP_DIR + "/pysdk_test_" + delimiter[0] + ".csv",
                                      import_options={
                                          "delimiter": delimiter[1]
                                      })

            assert e.type == InfinityException
            assert e.value.args[0] == types[1]
            db_obj.drop_table("test_csv_with_different_delimiter", ConflictType.Error)

    # import csv with delimiter more than one character
    def _test_csv_with_different_delimiter_more_than_one_character(self, check_data, delimiter):
        if not check_data:
            copy_data("pysdk_test_" + delimiter + ".csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_csv_with_different_delimiter_more_than_one_character", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_csv_with_different_delimiter_more_than_one_character",
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)
        table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test_" + delimiter + ".csv",
                              import_options={"delimiter": " "})

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_csv_with_different_delimiter_more_than_one_character", ConflictType.Error)

    def _test_import_csv_with_headers(self, check_data, has_header):
        if not check_data:
            copy_data("pysdk_test_commas.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_csv_with_headers", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_csv_with_headers", {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)
        table_obj.import_data(common_values.TEST_TMP_DIR + "pysdk_test_commas.csv",
                              import_options={"header": has_header})
        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_csv_with_headers", ConflictType.Error)

    # import fvecs, when table with more columns
    def _test_import_fvecs_table_with_more_columns(self, check_data):
        file_name = "pysdk_test.fvecs"
        if not check_data:
            generate_fvecs(100, 128, file_name)
            copy_data(file_name)

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_fvecs_table_with_more_columns", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_fvecs_table_with_more_columns",
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,128,float"}})

        with pytest.raises(InfinityException) as e:
            test_csv_dir = common_values.TEST_TMP_DIR + file_name
            table_obj.import_data(test_csv_dir, import_options={"file_type": "fvecs"})

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.IMPORT_FILE_FORMAT_ERROR

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_fvecs_table_with_more_columns", ConflictType.Error)

    # TODO import json, json has nested structure

    # import table with embedding not match with the table definition.
    def _test_import_embedding_with_not_match_definition(self, check_data, types):
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_embedding_with_not_match_definition", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_embedding_with_not_match_definition",
                                        {"c1": {"type": "int"}, "c2": {"type": types}})

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_embedding_with_not_match_definition", ConflictType.Error)

    def _test_import_embedding_with_dimension_unmatch(self, check_data, types):
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_embedding_with_not_match_definition", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_embedding_with_not_match_definition",
                                        {"c1": {"type": "int"}, "c2": {"type": types}})

        test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"

        with pytest.raises(InfinityException) as e:
            table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.IMPORT_FILE_FORMAT_ERROR

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_embedding_with_not_match_definition", ConflictType.Error)

    def _test_import_embedding_with_unmatched_elem_type(self, check_data, types):
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_embedding_with_not_match_definition", ConflictType.Ignore)
        with pytest.raises(Exception):
            table_obj = db_obj.create_table("test_import_embedding_with_not_match_definition",
                                            {"c1": {"type": "int"}, "c2": {"type": types}})
            test_csv_dir = common_values.TEST_TMP_DIR + "embedding_int_dim3.csv"
            res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})
            assert res.error_code == ErrorCode.OK
            res = table_obj.output(["*"]).to_df()
            print(res)
        db_obj.drop_table("test_import_embedding_with_not_match_definition", ConflictType.Error)

    # import table with varchar not match with the table definition.
    def _test_import_varchar_with_not_match_definition(self, check_data):
        if not check_data:
            copy_data("pysdk_test_varchar.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_varchar_with_not_match_definition", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_varchar_with_not_match_definition",
                                        {"c1": {"type": "int"}, "c2": {"type": "varchar"}})

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_varchar.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_varchar_with_not_match_definition", ConflictType.Error)

    # import table with 10000 columns.
    def _test_import_10000_columns(self, check_data):
        file_name = "pysdk_test_big_int.csv"
        if not check_data:
            generate_big_int_csv(10000, file_name)
            copy_data(file_name)

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_10000_columns", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_10000_columns", {"c1": {"type": "int"}, "c2": {"type": "int"}})

        test_csv_dir = common_values.TEST_TMP_DIR + file_name
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_10000_columns", ConflictType.Error)

    # import table with columns isn't matched (more and less)
    def _test_table_with_not_matched_columns(self, columns, check_data):
        if not check_data:
            copy_data("pysdk_test_commas.csv")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_not_matched_columns", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_table_with_not_matched_columns", columns)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_commas.csv"
        with pytest.raises(InfinityException) as e:
            table_obj.import_data(test_csv_dir)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.COLUMN_COUNT_MISMATCH or e.value.args[0] == ErrorCode.IMPORT_FILE_FORMAT_ERROR

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_table_with_not_matched_columns", ConflictType.Error)

    def _test_import_with_different_size(self, check_data, data_size):
        generate_big_rows_csv(data_size, "pysdk_test_import_with_different_size.csv")
        copy_data("pysdk_test_import_with_different_size.csv")

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_with_different_size", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_with_different_size",
                                        {"c1": {"type": "int"}, "c2": {"type": "varchar"}})

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_import_with_different_size.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == data_size
        db_obj.drop_table("test_import_with_different_size", ConflictType.Ignore)

    # import table with column value exceeding invalid value range
    def _test_import_exceeding_rows(self, check_data):
        if not check_data:
            generate_big_rows_csv(1024 * 8192, "pysdk_test_big_varchar_rows.csv")
            copy_data("pysdk_test_big_varchar_rows.csv")

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_exceeding_rows", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_exceeding_rows",
                                        {"c1": {"type": "int"}, "c2": {"type": "varchar"}})

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_big_varchar_rows.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == 1024 * 8192
        db_obj.drop_table("test_import_exceeding_rows", ConflictType.Error)

    def _test_import_exceeding_columns(self, check_data):
        generate_big_columns_csv(1024, "pysdk_test_big_columns.csv")
        if not check_data:
            copy_data("pysdk_test_big_columns.csv")

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_exceeding_columns", ConflictType.Ignore)
        columns = {"c" + str(i): {"type": "int"} for i in range(1024)}
        table_obj = db_obj.create_table("test_import_exceeding_columns", columns, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_big_columns.csv"
        res = table_obj.import_data(test_csv_dir)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        print(res)
        db_obj.drop_table("test_import_exceeding_columns", ConflictType.Error)

    def _test_import_jsonl_file_with_default(self, check_data):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_jsonl_file_with_default", ConflictType.Ignore)
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
        table_obj = db_obj.create_table("test_import_jsonl_file_with_default", columns, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "test_default.jsonl"
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "jsonl"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_jsonl_file_with_default", ConflictType.Error)

    def _test_import_csv_file_with_default(self, check_data):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_csv_file_with_default", ConflictType.Ignore)
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
        table_obj = db_obj.create_table("test_import_csv_file_with_default", columns, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_import_default.csv"
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "csv"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_csv_file_with_default", ConflictType.Error)

    def _test_import_json_file_with_default(self, check_data):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_import_json_file_with_default", ConflictType.Ignore)
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
        table_obj = db_obj.create_table("test_import_json_file_with_default", columns, ConflictType.Error)

        test_csv_dir = common_values.TEST_TMP_DIR + "pysdk_test_default.json"
        res = table_obj.import_data(test_csv_dir, import_options={"file_type": "json"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_pl()
        print(res)
        db_obj.drop_table("test_import_json_file_with_default", ConflictType.Error)
