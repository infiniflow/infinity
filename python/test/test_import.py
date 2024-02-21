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
import common_values
import infinity

TEST_DATA_DIR = "/test/data/"




class TestImport:

    def test_version(self):
        print(infinity.__version__)

    def test_import(self):
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
        db_obj.drop_table("test_import", True)
        table_obj = db_obj.create_table(
            "test_import", {"c1": "int", "c2": "vector,3,int"}, None)

        test_dir = "/tmp/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir, None)
        assert res.success

        # search
        res = table_obj.output(["c1"]).filter("c1 > 1").to_df()
        print(res)
        res = db_obj.drop_table("test_import")
        assert res.success

    # import different file format data
    @pytest.mark.skip(reason="Python sdk not support.")
    @pytest.mark.parametrize("file_format", ["csv", "json", "fvecs"])
    def test_import_different_file_format_data(self, file_format):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")

        db_obj.drop_table("test_import_different_file_format_data")
        table_obj = db_obj.create_table("test_import_different_file_format_data",
                                        {"c1": "int", "c2": "vector,3,int"}, None)

        if file_format == "fvecs":
            db_obj.drop_table("test_import_different_file_format_data_fvecs")
            table_obj = db_obj.create_table("test_import_different_file_format_data_fvecs",
                                            {"c1": "vector,128,float"}, None)
            table_obj.import_data(TEST_DATA_DIR + file_format + "/pysdk_test." + file_format)
            res = table_obj.output(["*"]).to_df()
            print(res)

        print(TEST_DATA_DIR + file_format + "/pysdk_test." + file_format)
        table_obj.import_data(TEST_DATA_DIR + file_format + "/pysdk_test." + file_format, None)
        res = table_obj.output(["*"]).to_df()
        print(res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res

    # import empty FVECS file
    @pytest.mark.parametrize("file_format", ["fvecs", "fvecs", "fvecs"])
    def test_import_empty_file_fvecs(self, file_format):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_import_empty_file_fvecs")
        table_obj = db_obj.create_table("test_import_empty_file_fvecs",
                                        {"c1": "vector,128,float"}, None)
        table_obj.import_data(os.getcwd() + TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res

    # import empty CSV file
    @pytest.mark.parametrize("file_format", ["csv", "csv", "csv"])
    def test_import_empty_file_csv(self, file_format):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_import_empty_file_csv")
        table_obj = db_obj.create_table("test_import_empty_file_csv",
                                        {"c1": "int", "c2": "vector,3,int"}, None)
        table_obj.import_data(os.getcwd() + TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res

    # import empty JSONL file
    @pytest.mark.parametrize("file_format", ["jsonl", "jsonl", "jsonl"])
    def test_import_empty_file_jsonl(self, file_format):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_import_empty_file_jsonl")
        table_obj = db_obj.create_table("test_import_empty_file_jsonl",
                                        {"c1": "int", "c2": "vector,3,int"}, None)
        table_obj.import_data(os.getcwd() + TEST_DATA_DIR + file_format + "/test_empty." + file_format)

        res = table_obj.output(["*"]).to_df()
        print(res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res

    # import format unrecognized data
    # import csv with different delimiter
    # import csv with delimiter more than one charactor
    # import csv with headers
    # import csv without headers
    # import fvecs, when table with more columns
    # import json, json has nested structure
    # import file into non-existent table, dropped table
    # import table with embedding not match with the table definition.
    # import table with varchar not match with the table definition.
    # import table with 10000 columns.
    # import table with columns isn't matched (more and less)
    # import table with column value exceeding invalid value range
