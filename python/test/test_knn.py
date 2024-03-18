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

from common import common_values
import infinity

from utils import copy_data


class TestKnn:

    def test_version(self):
        print(infinity.__version__)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_knn(self, check_data):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        # assert infinity_obj
        #
        # infinity
        #
        db_obj = infinity_obj.get_database("default")

        db_obj.drop_table("fix_tmp_20240116", if_exists=True)
        table_obj = db_obj.create_table("fix_tmp_20240116", {
            "variant_id": "varchar",
            "gender_vector": "vector,4,float",
            "color_vector": "vector,4,float",
            "category_vector": "vector,4,float",
            "tag_vector": "vector,4,float",
            "other_vector": "vector,4,float",
            "query_is_recommend": "varchar",
            "query_gender": "varchar",
            "query_color": "varchar",
            "query_price": "float"
        }, None)

        test_csv_dir = "/tmp/infinity/test_data/tmp_20240116.csv"
        if not check_data:
            copy_data("tmp_20240116.csv")
        print("import:", test_csv_dir, " start!")
        table_obj.import_data(test_csv_dir, None)

        # true
        # search
        # table_obj = db_obj.get_table("fix_tmp_20240116")
        # res = table_obj.output(["variant_id"]).to_df()

        # true
        # res = table_obj.output(["variant_id", "query_price"]).knn('gender_vector', [1.0] * 768, "float", "ip",
        #                                                           10).to_pl()
        # print(res)

        # true
        res = table_obj.output(["variant_id", "_row_id"]).knn(
            "gender_vector", [1.0] * 4, "float", "ip", 10).to_pl()
        print(res)

        # FIXME
        # res = table_obj.output(["variant_id", "query_is_recommend", "query_gender", "query_color", "query_price"]).knn(
        #     "gender_vector", [1.0] * 4, "float", "ip", 3).to_pl()

        # print(res)

    def test_insert_multi_column(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        with pytest.raises(Exception, match=r".*value count mismatch*"):
            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("test_insert_multi_column", if_exists=True)
            table = db_obj.create_table("test_insert_multi_column", {
                "variant_id": "varchar",
                "gender_vector": "vector,4,float",
                "color_vector": "vector,4,float",
                "category_vector": "vector,4,float",
                "tag_vector": "vector,4,float",
                "other_vector": "vector,4,float",
                "query_is_recommend": "varchar",
                "query_gender": "varchar",
                "query_color": "varchar",
                "query_price": "float"
            }, None)
            table.insert([{"variant_id": "123",
                           "gender_vector": [1.0] * 4,
                           "color_vector": [2.0] * 4,
                           "category_vector": [3.0] * 4,
                           "tag_vector": [4.0] * 4,
                           "other_vector": [5.0] * 4,
                           "query_is_recommend": "ok",
                           "query_gender": "varchar",
                           # "query_color": "red",
                           "query_price": 1.0
                           }])

    # knn various column name
    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("column_name", [pytest.param("variant_id", marks=pytest.mark.xfail),
                                             "gender_vector",
                                             "color_vector",
                                             pytest.param("query_price", marks=pytest.mark.xfail),
                                             # pytest.param(1, marks=pytest.mark.xfail),
                                             # pytest.param(2.2, marks=pytest.mark.xfail),
                                             # pytest.param("!@#/\#$ ## #$%  @#$^", marks=pytest.mark.xfail),
                                             ])
    def test_various_vector_column_name(self, get_infinity_db, check_data, column_name):
        db_obj = get_infinity_db
        db_obj.drop_table("test_various_vector_column_name", if_exists=True)
        table_obj = db_obj.create_table("test_various_vector_column_name", {
            "variant_id": "varchar",
            "gender_vector": "vector,4,float",
            "color_vector": "vector,4,float",
            "category_vector": "vector,4,float",
            "tag_vector": "vector,4,float",
            "other_vector": "vector,4,float",
            "query_is_recommend": "varchar",
            "query_gender": "varchar",
            "query_color": "varchar",
            "query_price": "float"
        }, None)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/tmp/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        res = table_obj.output(["variant_id", "_row_id"]).knn(column_name, [1.0] * 4, "float", "ip", 2).to_pl()
        print(res)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        pytest.param("variant_id", marks=pytest.mark.skip(reason="struct.error: required argument is not a float")),
        pytest.param("gender_vector", marks=pytest.mark.skip(reason="struct.error: required argument is not a float")),
        pytest.param(1, marks=pytest.mark.skip(reason="TypeError: object of type 'int' has no len()")),
        pytest.param(2.4, marks=pytest.mark.skip(reason="TypeError: object of type 'float' has no len()")),
        pytest.param([1] * 3, marks=pytest.mark.xfail),
        [1] * 4,
        pytest.param((1, 2, 3), marks=pytest.mark.xfail),
        (1, 2, 3, 4),
        pytest.param({"c": "12"}, marks=pytest.mark.skip(reason="struct.error: required argument is not a float")),
    ])
    def test_various_embedding_data(self, get_infinity_db, check_data, embedding_data):
        db_obj = get_infinity_db
        db_obj.drop_table("test_various_embedding_data", if_exists=True)
        table_obj = db_obj.create_table("test_various_embedding_data", {
            "variant_id": "varchar",
            "gender_vector": "vector,4,float",
            "color_vector": "vector,4,float",
            "category_vector": "vector,4,float",
            "tag_vector": "vector,4,float",
            "other_vector": "vector,4,float",
            "query_is_recommend": "varchar",
            "query_gender": "varchar",
            "query_color": "varchar",
            "query_price": "float"
        }, None)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/tmp/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        res = table_obj.output(["variant_id", "_row_id"]).knn("gender_vector", embedding_data, "float", "ip", 2).to_pl()
        print(res)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        [1.0] * 4,
    ])
    @pytest.mark.parametrize("embedding_data_type", [
        # FIXME "int",
        "float",
        pytest.param(1, marks=pytest.mark.xfail(reason="Invalid embedding 1.0 type")),
        pytest.param(2.2, marks=pytest.mark.xfail(reason="Invalid embedding 1.0 type")),
        pytest.param("#@!$!@", marks=pytest.mark.xfail(reason="Invalid embedding 1.0 type")),
    ])
    def test_various_embedding_data_type(self, get_infinity_db, check_data, embedding_data, embedding_data_type):
        db_obj = get_infinity_db
        db_obj.drop_table("test_various_embedding_data_type", if_exists=True)
        table_obj = db_obj.create_table("test_various_embedding_data_type", {
            "variant_id": "varchar",
            "gender_vector": "vector,4,float",
            "color_vector": "vector,4,float",
            "category_vector": "vector,4,float",
            "tag_vector": "vector,4,float",
            "other_vector": "vector,4,float",
            "query_is_recommend": "varchar",
            "query_gender": "varchar",
            "query_color": "varchar",
            "query_price": "float"
        }, None)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/tmp/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        res = table_obj.output(["variant_id"]).knn("gender_vector", embedding_data, embedding_data_type, "ip",
                                                   2).to_pl()
        print(res)

    @pytest.mark.skip(reason="Core dumped.")
    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        [1.0] * 4,
    ])
    @pytest.mark.parametrize("embedding_data_type", [
        # FIXME "int",
        "float",
    ])
    @pytest.mark.parametrize("distance_type", [
        "l2",
        # FIXME "cosine",
        "ip",
        # FIXME "hamming",
    ])
    def test_various_distance_type(self, get_infinity_db, check_data, embedding_data, embedding_data_type,
                                   distance_type):
        db_obj = get_infinity_db
        db_obj.drop_table("test_various_distance_type", if_exists=True)
        table_obj = db_obj.create_table("test_various_distance_type", {
            "variant_id": "varchar",
            "gender_vector": "vector,4,float",
            "color_vector": "vector,4,float",
            "category_vector": "vector,4,float",
            "tag_vector": "vector,4,float",
            "other_vector": "vector,4,float",
            "query_is_recommend": "varchar",
            "query_gender": "varchar",
            "query_color": "varchar",
            "query_price": "float"
        }, None)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/tmp/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        res = table_obj.output(["variant_id"]).knn("gender_vector", embedding_data, embedding_data_type, distance_type,
                                                   2).to_pl()
        print(res)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("topn", [
        (2, True),
        (10, True),
        (0, False),
        (-1, False),
        pytest.param("word", marks=pytest.mark.skip(reason="struct.error: required argument is not an integer")),
        pytest.param({}, marks=pytest.mark.skip(reason="struct.error: required argument is not an integer")),
        pytest.param((), marks=pytest.mark.skip(reason="struct.error: required argument is not an integer")),
        pytest.param([1] * 4, marks=pytest.mark.skip(reason="struct.error: required argument is not an integer")),
    ])
    def test_various_topn(self, get_infinity_db, check_data, topn):
        db_obj = get_infinity_db
        db_obj.drop_table("test_various_topn", if_exists=True)
        table_obj = db_obj.create_table("test_various_topn", {
            "variant_id": "varchar",
            "gender_vector": "vector,4,float",
            "color_vector": "vector,4,float",
            "category_vector": "vector,4,float",
            "tag_vector": "vector,4,float",
            "other_vector": "vector,4,float",
            "query_is_recommend": "varchar",
            "query_gender": "varchar",
            "query_color": "varchar",
            "query_price": "float"
        }, None)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/tmp/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        if topn[1]:
            res = table_obj.output(["variant_id"]).knn("gender_vector", [1] * 4, "float", "pl", topn[0]).to_pl()
            print(res)
        else:
            with pytest.raises(Exception, match="ERROR:3014*"):
                res = table_obj.output(["variant_id"]).knn("gender_vector", [1] * 4, "float", "pl", topn[0]).to_pl()
