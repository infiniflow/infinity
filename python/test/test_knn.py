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

import pytest

import common_values
import infinity


class TestKnn:

    def test_version(self):
        print(infinity.__version__)

    def test_knn(self):
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
