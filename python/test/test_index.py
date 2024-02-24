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
import infinity.index as index
from infinity.errors import ErrorCode

class TestIndex:

    def test_create_index_IVFFlat(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        res = db_obj.drop_table("test_index_ivfflat", True)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("test_index_ivfflat", {
            "c1": "vector,1024,float"}, None)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

    def test_create_index_HNSW(self):
        # CREATE INDEX idx1 ON test_hnsw (col1) USING Hnsw WITH (M = 16, ef_construction = 50, ef = 50, metric = l2);
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        res = db_obj.drop_table("test_index_hnsw", True)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_hnsw", {"c1": "vector,1024,float"}, None)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.Hnsw,
                                                      [
                                                          index.InitParameter("M", "16"),
                                                          index.InitParameter("ef_construction", "50"),
                                                          index.InitParameter("ef", "50"),
                                                          index.InitParameter("metric", "l2")
                                                      ])], None)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

    def test_create_index_fulltext(self):
        # CREATE INDEX ft_index ON enwiki(body) USING FULLTEXT WITH(ANALYZER=segmentation) (doctitle, docdate) USING FULLTEXT;
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        res = db_obj.drop_table("test_index_fulltext", if_exists=True)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_fulltext", {"doctitle": "varchar", "docdate": "varchar", "body": "varchar"}, None)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("body",
                                                      index.IndexType.FullText,
                                                      [index.InitParameter("ANALYZER", "segmentation")]),
                                      index.IndexInfo("doctitle",
                                                      index.IndexType.FullText,
                                                      []),
                                      index.IndexInfo("docdate",
                                                      index.IndexType.FullText,
                                                      []),
                                      ], None)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

    # drop non-existent index
    def test_drop_non_existent_index(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        res = db_obj.drop_table("test_drop_non_existent_index", if_exists=True)

        table_obj = db_obj.create_table("test_drop_non_existent_index", {
            "c1": "vector,3,float"}, None)
        assert table_obj is not None

        # drop none index
        table_obj.drop_index("none_index")

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create created index
    def test_create_created_index(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_create_created_index", if_exists=True)
        table_obj = db_obj.create_table("test_create_created_index", {
            "c1": "vector,3,float"}, None)

        # create index
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)
        assert res.error_code == ErrorCode.OK

        # create created index
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # create / drop index with invalid options
    @pytest.mark.skip(reason="Expected error.")
    @pytest.mark.parametrize("cl_name", [1, 2.2, (1, 2), "c1", [1, 2, 3]])
    @pytest.mark.parametrize("index_type", [
        1, 2.2, [1, 2], "$#%dfva", (1, 2), {"1": 2},
        index.IndexType.Hnsw, index.IndexType.IVFFlat, index.IndexType.FullText
    ])
    @pytest.mark.parametrize("params", [
        1, 2.2, [1, 2], "$#%dfva", (1, 2), {"1": 2},
        [index.InitParameter("centroids_count", "128"),
         index.InitParameter("metric", "l2")]
    ])
    def test_create_drop_index_invalid_options(self, cl_name, index_type, params):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_create_drop_index_invalid_options", if_exists=True)
        table_obj = db_obj.create_table("test_create_drop_index_invalid_options", {
            "c1": "vector,3,float"}, None)

        index_info = [cl_name, index_type, params]
        table_obj.create_index("my_index", index_info, None)

        # disconnect
        res = infinity_obj.disconnect()
        assert res

    # create index on dropped table instance
    @pytest.mark.skip(reason="Cause core dumped.")
    def test_create_index_on_dropped_table(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_create_drop_index_invalid_options", if_exists=True)
        table_obj = db_obj.create_table("test_create_drop_index_invalid_options", {
            "c1": "vector,3,float"}, None)
        db_obj.drop_table("test_create_drop_index_invalid_options", if_exists=True)

        # create created index
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)

        # disconnect
        res = infinity_obj.disconnect()
        assert res

    # create index then show index
    # drop index then show index
    # create index on different type of column and show index

    # insert / import data, then create index
    # create index then insert / import data
    # create index on all data are deleted table.
    # create index on all data are updated.
    # create index on no date are deleted.
    # create index on no date are update.
    # create index on table has multiple segments
    # create index on table has only one segment
    # create index on empty table

    # create / drop index repeatedly
    # create index on same column with different parameters

    # show index on table
    # show index on dropped table instance
