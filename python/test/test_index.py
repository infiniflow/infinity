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

import numpy as np
import pytest

from common import common_values
import infinity.index as index
from infinity.errors import ErrorCode
from infinity.remote_thrift.infinity import RemoteThriftInfinityConnection
from infinity.remote_thrift.table import RemoteTable

TEST_DATA_DIR = "/test/data/"


class TestIndex:

    def test_create_index_IVFFlat(self, get_infinity_db):
        db_obj = get_infinity_db
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

    def test_create_index_HNSW(self, get_infinity_db):
        # CREATE INDEX idx1 ON test_hnsw (col1) USING Hnsw WITH (M = 16, ef_construction = 50, ef = 50, metric = l2);
        db_obj = get_infinity_db
        res = db_obj.drop_table("test_index_hnsw", True)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_hnsw", {"c1": "vector,1024,float"}, None)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.Hnsw,
                                                      [
                                                          index.InitParameter(
                                                              "M", "16"),
                                                          index.InitParameter(
                                                              "ef_construction", "50"),
                                                          index.InitParameter(
                                                              "ef", "50"),
                                                          index.InitParameter(
                                                              "metric", "l2")
                                                      ])], None)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

    def test_create_index_fulltext(self, get_infinity_db):
        # CREATE INDEX ft_index ON enwiki(body) USING FULLTEXT WITH(ANALYZER=segmentation);
        db_obj = get_infinity_db
        res = db_obj.drop_table("test_index_fulltext", if_exists=True)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_fulltext", {"doctitle": "varchar", "docdate": "varchar", "body": "varchar"}, None)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("body",
                                                      index.IndexType.FullText,
                                                      [index.InitParameter("ANALYZER", "segmentation")]),
                                      ], None)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

    # drop non-existent index
    def test_drop_non_existent_index(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        res = db_obj.drop_table("test_drop_non_existent_index", if_exists=True)

        table_obj = db_obj.create_table("test_drop_non_existent_index", {
            "c1": "vector,3,float"}, None)
        assert table_obj is not None

        # drop none index
        table_obj.drop_index("none_index")

    # create created index
    def test_create_created_index(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
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


    # create / drop index with invalid options
    @pytest.mark.parametrize("column_name",
                             [(1, False), (2.2, False), ((1, 2), False), ([1, 2, 3], False), ("c1", True)])
    @pytest.mark.parametrize("index_type", [
        (1, False),
        (2.2, False),
        ([1, 2], False),
        ("$#%dfva", False),
        ((1, 2), False),
        ({"1": 2}, False),
        (index.IndexType.Hnsw, False),
        (index.IndexType.IVFFlat, True)
    ])
    @pytest.mark.parametrize("params", [
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False), ((1, 2), False), ({"1": 2}, False),
        ([index.InitParameter("centroids_count", "128"),
          index.InitParameter("metric", "l2")], True)
    ])
    @pytest.mark.parametrize("types", ["vector, 3, float"])
    def test_create_drop_vector_index_invalid_options(self, get_infinity_db, column_name, index_type, params, types):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_drop_vector_index_invalid_options", if_exists=True)
        table_obj = db_obj.create_table("test_create_drop_vector_index_invalid_options", {
            "c1": types}, None)

        index_info = [index.IndexInfo(column_name[0], index_type[0], params[0])]
        if not column_name[1] or not index_type[1] or not params[1]:
            with pytest.raises(Exception):
                table_obj.create_index("my_index", index_info, None)
        else:
            table_obj.create_index("my_index", index_info, None)


    @pytest.mark.parametrize("column_name", [
        (1, False),
        (2.2, False),
        ([1, 2], False),
        ("$#%dfva", False),
        ((1, 2), False),
        ({"1": 2}, False),
        ("c1", True)])
    @pytest.mark.parametrize("index_type", [(index.IndexType.FullText, True)])
    @pytest.mark.parametrize("params", [
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False), ((1, 2), False), ({"1": 2}, False),
        ([index.InitParameter("ANALYZER", "segmentation")], True)
    ])
    @pytest.mark.parametrize("types", ["int", "int8", "int16", "int32", "int64", "integer",
                                       "float", "float32", "double", "float64",
                                       "varchar", "bool", "vector, 3, float"])
    def test_create_drop_different_fulltext_index_invalid_options(self, get_infinity_db, column_name, index_type, params, types):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_drop_different_fulltext_index_invalid_options", if_exists=True)
        table_obj = db_obj.create_table("test_create_drop_different_fulltext_index_invalid_options", {
            "c1": types}, None)

        index_info = [index.IndexInfo(column_name[0], index_type[0], params[0])]
        if types != "varchar" or not column_name[1] or not index_type[1] or not params[1]:
            with pytest.raises(Exception):
                table_obj.create_index("my_index", index_info, None)
        else:
            table_obj.create_index("my_index", index_info, None)

    # create index on dropped table instance
    def test_create_index_on_dropped_table(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table(
            "test_create_drop_index_invalid_options", if_exists=True)
        table_obj = db_obj.create_table("test_create_drop_index_invalid_options", {
            "c1": "vector,3,float"}, None)
        db_obj.drop_table(
            "test_create_drop_index_invalid_options", if_exists=True)

        # create created index
        with pytest.raises(Exception, match="ERROR:3022*"):
            table_obj.create_index("my_index",
                                   [index.IndexInfo("c1",
                                                    index.IndexType.IVFFlat,
                                                    [index.InitParameter("centroids_count", "128"),
                                                     index.InitParameter("metric", "l2")])], None)

    # create index then show index
    @pytest.mark.skip(reason="Not support for showing index yet.")
    def test_create_index_show_index(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_index_show_index", if_exists=True)
        table_obj = db_obj.create_table("test_create_index_show_index", {
            "c1": "vector,3,float"}, None)
        # create created index
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)

    # drop index then show index
    @pytest.mark.skip(reason="Not support for showing index yet.")
    def test_drop_index_show_index(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_index_show_index", if_exists=True)
        table_obj = db_obj.create_table("test_create_index_show_index", {
            "c1": "vector,3,float"}, None)
        # create created index
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)

        table_obj.drop_index("my_index")

    # create index on different type of column and show index
    @pytest.mark.parametrize("types", ["vector, 3, float"])
    @pytest.mark.parametrize("index_type", [
        (index.IndexType.Hnsw, False, "ERROR:3061*"),
        (index.IndexType.IVFFlat, True),
        (index.IndexType.FullText, False, "ERROR:3009*")
    ])
    def test_create_index_on_different_type_of_column(self, get_infinity_db, types, index_type):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table(
            "test_create_index_on_different_type_of_column", if_exists=True)
        table_obj = db_obj.create_table("test_create_index_on_different_type_of_column", {
            "c1": types}, None)
        # create created index
        if not index_type[1]:
            with pytest.raises(Exception, match=index_type[2]):
                table_obj.create_index("my_index",
                                       [index.IndexInfo("c1",
                                                        index_type[0],
                                                        [index.InitParameter("centroids_count", "128"),
                                                         index.InitParameter("metric", "l2")])], None)
        else:
            res = table_obj.create_index("my_index",
                                         [index.IndexInfo("c1",
                                                          index_type[0],
                                                          [index.InitParameter("centroids_count", "128"),
                                                           index.InitParameter("metric", "l2")])], None)
            assert res.error_code == ErrorCode.OK

    # insert data, then create index
    @pytest.mark.parametrize("index_type", [
        index.IndexType.IVFFlat
    ])
    def test_insert_data_create_index(self, get_infinity_db, index_type):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_insert_data_create_index", if_exists=True)
        table_obj = db_obj.create_table("test_insert_data_create_index", {
            "c1": "vector,1024,float"}, None)
        values = [{"c1": [1.1 for _ in range(1024)]}]
        table_obj.insert(values)

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index_type,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_type", [
        (index.IndexType.IVFFlat, True),
        (index.IndexType.FullText, False, "ERROR:3009*")
    ])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_import_data_create_index(self, get_infinity_db, index_type, file_format):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_import_data_create_index", if_exists=True)
        table_obj = db_obj.create_table("test_import_data_create_index", {
            "c1": "int",
            "c2": "vector,3,float"}, None)

        table_obj.import_data(os.getcwd() + TEST_DATA_DIR +
                              file_format + "/pysdk_test." + file_format)
        if (index_type[1]):
            res = table_obj.create_index("my_index",
                                         [index.IndexInfo("c2",
                                                          index_type[0],
                                                          [index.InitParameter("centroids_count", "128"),
                                                           index.InitParameter("metric", "l2")])], None)
            assert res.error_code == ErrorCode.OK
        else:
            with pytest.raises(Exception, match=index_type[2]):
                table_obj.create_index("my_index",
                                       [index.IndexInfo("c2",
                                                        index_type[0],
                                                        [index.InitParameter("centroids_count", "128"),
                                                         index.InitParameter("metric", "l2")])], None)

    # create index then insert / import data
    @pytest.mark.skip(reason="TODO")
    @pytest.mark.parametrize("index_type", [index.IndexType.IVFFlat, index.IndexType.FullText])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_create_index_import_data(self, get_infinity_db, index_type, file_format):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_import_data_create_index", if_exists=True)
        table_obj = db_obj.create_table("test_import_data_create_index", {
            "c1": "int",
            "c2": "vector,3,float"}, None)
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c2",
                                                      index_type,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)
        assert res.error_code == ErrorCode.OK
        table_obj.import_data(os.getcwd() + TEST_DATA_DIR + file_format + "/pysdk_test." + file_format)

    # create index on all data are deleted table.
    def test_create_index_on_deleted_table(self, get_infinity_db):
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_index_on_deleted_table", if_exists=True)

        table_obj = db_obj.create_table("test_create_index_on_deleted_table", {"c1": "vector,128,float"}, None)
        # insert data
        embedding_data = [i for i in range(128)]
        value = [{"c1": embedding_data} for _ in range(1024)]
        table_obj.insert(value)
        res = table_obj.output(["*"]).to_pl()
        print(res)

        # delete data
        table_obj.delete()
        res = table_obj.output(["*"]).to_pl()
        print(res)

        # create index
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], None)
        assert res.error_code == ErrorCode.OK

    # create index on all data are updated.
    @pytest.mark.skip(reason="Not support yet.")
    def test_create_index_on_update_table(self, get_infinity_db):
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_index_on_update_table")
        table_obj = db_obj.create_table("test_create_index_on_update_table", {"c1": "vector,128,float", "c2": "int"}, None)

        # insert data
        embedding_data = [i for i in range(128)]
        value = [{"c1": embedding_data, "c2": i} for i in range(10)]
        table_obj.insert(value)
        res = table_obj.output(["*"]).to_pl()
        print(res)
        # update data
        embedding_data = [i + 0.1 * i for i in range(128)]
        # embedding_data = [i for i in range(128)]

        value = [{"c1": embedding_data} for _ in range(10)]
        for i in range(10):
            table_obj.update("c2 = " + str(i), [{"c1": embedding_data}])
        res = table_obj.output(["*"]).to_pl()
        print(res)

    # create index on no date are deleted.
    # def test_create_index
    # create index on no date are update.
    # create index on table has multiple segments
    # create index on table has only one segment
    # create index on empty table

    # create / drop index repeatedly
    # create index on same column with different parameters

    # show index on table
    # show index on dropped table instance
