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
from infinity.remote_thrift.infinity import RemoteThriftInfinityConnection
import infinity.index as index
from infinity.errors import ErrorCode
from infinity.common import ConflictType, InfinityException
from infinity.remote_thrift.types import make_match_tensor_expr as remote_make_match_tensor_expr
from infinity.local_infinity.types import make_match_tensor_expr as local_make_match_tensor_expr
from internal.utils import copy_data, generate_commas_enwiki
from internal.test_sdkbase import TestSdk


class TestKnn(TestSdk):

    # def _test_version(self):
    #     print(infinity.__version__)

    def _test_knn(self, check_data):
        #
        # infinity
        #
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("fix_tmp_20240116",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("fix_tmp_20240116", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)

        test_csv_dir = "/var/infinity/test_data/tmp_20240116.csv"
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
        res = table_obj.output(["variant_id", "_row_id"]).knn("gender_vector", [1.0] * 4, "float", "ip", 10).to_pl()
        print(res)

        # FIXME
        # res = table_obj.output(["variant_id", "query_is_recommend", "query_gender", "query_color", "query_price"]).knn(
        #     "gender_vector", [1.0] * 4, "float", "ip", 3).to_pl()

        # print(res)

        res = db_obj.drop_table("fix_tmp_20240116", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_multi_column(self):
        with pytest.raises(Exception, match=r".*value count mismatch*"):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_insert_multi_column",
                              conflict_type=ConflictType.Ignore)
            table = db_obj.create_table("test_insert_multi_column", {
                "variant_id": {"type": "varchar"},
                "gender_vector": {"type": "vector,4,float"},
                "color_vector": {"type": "vector,4,float"},
                "category_vector": {"type": "vector,4,float"},
                "tag_vector": {"type": "vector,4,float"},
                "other_vector": {"type": "vector,4,float"},
                "query_is_recommend": {"type": "varchar"},
                "query_gender": {"type": "varchar"},
                "query_color": {"type": "varchar"},
                "query_price": {"type": "float"}
            }, ConflictType.Error)
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

        res = db_obj.drop_table("test_insert_multi_column", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # knn various column name
    def _test_knn_on_vector_column(self, check_data, column_name):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_knn_on_vector_column",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_knn_on_vector_column", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/var/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        res = table_obj.output(["variant_id", "_row_id", "_similarity"]).knn(
            column_name, [1.0] * 4, "float", "ip", 2).to_pl()
        print(res)

        res = db_obj.drop_table(
            "test_knn_on_vector_column", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_knn_on_non_vector_column(self, check_data, column_name):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_knn_on_non_vector_column",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_knn_on_non_vector_column", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/var/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        with pytest.raises(InfinityException) as e:
            table_obj.output(["variant_id", "_row_id"]).knn(column_name, [1.0] * 4, "float", "ip", 2).to_pl()

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.SYNTAX_ERROR

        res = db_obj.drop_table(
            "test_knn_on_non_vector_column", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_valid_embedding_data(self, check_data, embedding_data):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_valid_embedding_data",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_valid_embedding_data", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/var/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        res = table_obj.output(["variant_id", "_row_id"]).knn(
            "gender_vector", embedding_data, "float", "ip", 2).to_pl()
        print(res)

        res = db_obj.drop_table(
            "test_valid_embedding_data", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_invalid_embedding_data(self, check_data, embedding_data):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_invalid_embedding_data",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_invalid_embedding_data", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/var/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        with pytest.raises(Exception):
            res = table_obj.output(["variant_id", "_row_id"]).knn(
                "gender_vector", embedding_data, "float", "ip", 2).to_pl()
            print(res)

        res = db_obj.drop_table(
            "test_invalid_embedding_data", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_valid_embedding_data_type(self, check_data, embedding_data, embedding_data_type):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_valid_embedding_data_type",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_valid_embedding_data_type", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/var/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        if embedding_data_type[1]:
            res = table_obj.output(["variant_id", "_distance"]).knn("gender_vector", embedding_data, embedding_data_type[0],
                                                       "l2",
                                                       2).to_pl()
            print(res)
        else:
            res = table_obj.output(["variant_id", "_similarity"]).knn("gender_vector", embedding_data, embedding_data_type[0],
                                                       "ip",
                                                       2).to_pl()

        res = db_obj.drop_table(
            "test_valid_embedding_data_type", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_invalid_embedding_data_type(self, check_data, embedding_data, embedding_data_type):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_invalid_embedding_data_type",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_invalid_embedding_data_type", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/var/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        with pytest.raises(Exception):
            if embedding_data_type[1]:
                res = table_obj.output(["variant_id"]).knn("gender_vector", embedding_data, embedding_data_type[0],
                                                           "l2",
                                                           2).to_pl()
                print(res)
            else:
                res = table_obj.output(["variant_id"]).knn("gender_vector", embedding_data, embedding_data_type[0],
                                                           "ip",
                                                           2).to_pl()
        res = db_obj.drop_table(
            "test_invalid_embedding_data_type", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_various_distance_type(self, check_data, embedding_data, embedding_data_type,
                                   distance_type):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_distance_type",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_various_distance_type", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/var/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        if distance_type[1] and embedding_data_type[1]:
            res = table_obj.output(["variant_id"]).knn("gender_vector", embedding_data, embedding_data_type[0],
                                                       distance_type[0],
                                                       2).to_pl()
            print(res)
        else:
            with pytest.raises(InfinityException) as e:
                table_obj.output(["variant_id"]).knn("gender_vector", embedding_data, embedding_data_type[0],
                                                     distance_type[0],
                                                     2).to_pl()

            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.NOT_SUPPORTED

        res = db_obj.drop_table(
            "test_various_distance_type", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_various_topn(self, check_data, topn):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_topn",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_various_topn", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("tmp_20240116.csv")
        test_csv_dir = "/var/infinity/test_data/tmp_20240116.csv"
        table_obj.import_data(test_csv_dir, None)
        if topn[1]:
            res = table_obj.output(["variant_id"]).knn(
                "gender_vector", [1] * 4, "float", "l2", topn[0]).to_pl()
            print(res)
        else:
            with pytest.raises(InfinityException) as e:
                table_obj.output(["variant_id"]).knn(
                    "gender_vector", [1] * 4, "float", "l2", topn[0]).to_pl()

            assert e.type == InfinityException
            assert e.value.args[0] == topn[2]

        res = db_obj.drop_table("test_various_topn", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_with_index_before(self, check_data, index_column_name, knn_column_name,
                               index_distance_type, knn_distance_type):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_index", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_index", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("pysdk_test_knn.csv")
        test_csv_dir = "/var/infinity/test_data/pysdk_test_knn.csv"
        table_obj.import_data(test_csv_dir, None)
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo(index_column_name,
                                                      index.IndexType.Hnsw,
                                                      [
                                                          index.InitParameter(
                                                              "M", "16"),
                                                          index.InitParameter(
                                                              "ef_construction", "50"),
                                                          index.InitParameter(
                                                              "ef", "50"),
                                                          index.InitParameter(
                                                              "metric", index_distance_type)
                                                      ])], ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["variant_id"]).knn(
            knn_column_name, [1] * 4, "float", knn_distance_type, 5).to_pl()
        print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_with_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_with_index_after(self, check_data,
                              index_column_name, knn_column_name,
                              index_distance_type, knn_distance_type):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_index_after", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_index_after", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        if not check_data:
            copy_data("pysdk_test_knn.csv")
        test_csv_dir = "/var/infinity/test_data/pysdk_test_knn.csv"
        table_obj.import_data(test_csv_dir, None)
        res = table_obj.output(["variant_id"]).knn(
            knn_column_name, [1.0] * 4, "float", knn_distance_type, 5).to_pl()
        print(res)
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo(index_column_name,
                                                      index.IndexType.Hnsw,
                                                      [
                                                          index.InitParameter(
                                                              "M", "16"),
                                                          index.InitParameter(
                                                              "ef_construction", "50"),
                                                          index.InitParameter(
                                                              "ef", "50"),
                                                          index.InitParameter(
                                                              "metric", index_distance_type)
                                                      ])], ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_with_index_after", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_with_fulltext_match_with_valid_columns(self, check_data, match_param_1):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_valid_columns", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_valid_columns",
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               [index.IndexInfo("body",
                                                index.IndexType.FullText,
                                                [index.InitParameter("ANALYZER", "standard")]),
                                ], ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj
               .output(["*"])
               .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
               .match(match_param_1, "black", "topn=1")
               .fusion('rrf')
               .to_pl())
        print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_valid_columns", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_with_fulltext_match_with_invalid_columns(self, check_data, match_param_1):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_columns", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_invalid_columns",
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               [index.IndexInfo("body",
                                                index.IndexType.FullText,
                                                [index.InitParameter("ANALYZER", "standard")]),
                                ], ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        with pytest.raises(Exception):
            res = (table_obj
                   .output(["*"])
                   .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
                   .match(match_param_1, "black", "topn=1")
                   .fusion('rrf')
                   .to_pl())
            print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_columns", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_with_fulltext_match_with_valid_words(self, check_data, match_param_2):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_valid_words", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_valid_words",
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               [index.IndexInfo("body",
                                                index.IndexType.FullText,
                                                [index.InitParameter("ANALYZER", "standard")]),
                                ], ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj
               .output(["*"])
               .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
               .match("doctitle,num,body^5", match_param_2, "topn=1")
               .fusion('rrf')
               .to_pl())
        print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_valid_words", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_with_fulltext_match_with_invalid_words(self, check_data, match_param_2):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_words", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_invalid_words",
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               [index.IndexInfo("body",
                                                index.IndexType.FullText,
                                                [index.InitParameter("ANALYZER", "standard")]),
                                ], ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})

        with pytest.raises(Exception):
            res = (table_obj
                   .output(["*"])
                   .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
                   .match("doctitle,num,body^5", match_param_2, "topn=1")
                   .fusion('rrf')
                   .to_pl())
            print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_words", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_with_fulltext_match_with_options(self, check_data, match_param_3):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_options", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_options",
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               [index.IndexInfo("body",
                                                index.IndexType.FullText,
                                                [index.InitParameter("ANALYZER", "standard")]),
                                ], ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj
               .output(["*"])
               .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
               .match("doctitle,num,body^5", "word", match_param_3)
               .fusion('rrf')
               .to_pl())
        print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_options", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_with_fulltext_match_with_invalid_options(self, check_data, match_param_3):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_options", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_invalid_options",
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               [index.IndexInfo("body",
                                                index.IndexType.FullText,
                                                [index.InitParameter("ANALYZER", "standard")]),
                                ], ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})

        with pytest.raises(Exception):
            res = (table_obj
                   .output(["*"])
                   .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
                   .match("doctitle,num,body^5", "word", match_param_3)
                   .fusion('rrf')
                   .to_pl())
            print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_options", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_tensor_scan(self, check_data):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_tensor_scan", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_tensor_scan",
                                        {"title": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "t": {"type": "tensor, 4, float"},
                                         "body": {"type": "varchar"}})
        if not check_data:
            copy_data("tensor_maxsim.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "tensor_maxsim.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj
               .output(["*", "_row_id", "_score"])
               .match_tensor('t', [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]], 'float', 'maxsim', 'topn=2')
               .to_pl())
        print(res)

        res = db_obj.drop_table("test_tensor_scan", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_sparse_scan(self, check_data):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_scan", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_sparse_scan", {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int8"}}, ConflictType.Error)
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj.output(["*", "_row_id", "_similarity"])
                        .match_sparse("c2", {"indices": [0, 20, 80], "values": [1.0, 2.0, 3.0]}, "ip", 3)
                        .to_pl())
        print(res)

        res = db_obj.drop_table("test_sparse_scan", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_sparse_knn_with_index(self, check_data):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_knn_with_index", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_sparse_knn_with_index", {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int8"}}, ConflictType.Error)
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        table_obj.create_index("idx1",
                                     [index.IndexInfo("c2",
                                                      index.IndexType.BMP,
                                                      [
                                                          index.InitParameter(
                                                              "block_size", "8"),
                                                          index.InitParameter(
                                                              "compress_type", "compress")
                                                      ])], ConflictType.Error)

        table_obj.optimize("idx1", {"topk": "3"})

        res = (table_obj
                .output(["*", "_row_id", "_similarity"])
                .match_sparse("c2", {"indices": [0, 20, 80], "values": [1.0, 2.0, 3.0]}, "ip", 3,
                              {"alpha": "1.0", "beta": "1.0"})
                .to_pl())
        print(res)

        res = table_obj.drop_index("idx1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_sparse_knn_with_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_with_multiple_fusion(self, check_data):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_multiple_fusion", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_multiple_fusion",
                                        {"title": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "t": {"type": "tensor, 4, float"},
                                         "body": {"type": "varchar"}})
        if not check_data:
            copy_data("tensor_maxsim.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "tensor_maxsim.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        match_tensor_expr = None
        if isinstance(self.infinity_obj, RemoteThriftInfinityConnection):
            match_tensor_expr = remote_make_match_tensor_expr('t', [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]], 'float',
                                                              'maxsim')
        else:
            match_tensor_expr = local_make_match_tensor_expr('t', [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]], 'float',
                                                              'maxsim')
        res = (table_obj
               .output(["*", "_row_id", "_score"])
               .match('body', 'off', 'topn=4')
               .match_tensor('t', [[1.0, 0.0, 0.0, 0.0], [1.0, 0.0, 0.0, 0.0]], 'float', 'maxsim', 'topn=2')
               .fusion('rrf')
               .fusion('match_tensor', 'topn=2', match_tensor_expr)
               .to_pl())
        print(res)

        res = db_obj.drop_table("test_with_multiple_fusion", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
