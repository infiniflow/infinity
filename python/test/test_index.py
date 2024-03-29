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
import pandas
import pytest
import sqlglot
import time

import infinity.index as index
from infinity.errors import ErrorCode
from infinity.common import ConflictType
from sqlglot import exp, parse_one

TEST_DATA_DIR = "/test/data/"


class TestIndex:

    def test_create_index_IVFFlat(self, get_infinity_db):
        db_obj = get_infinity_db
        res = db_obj.drop_table("test_index_ivfflat", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("test_index_ivfflat", {
            "c1": "vector,1024,float"}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

    def test_create_index_HNSW(self, get_infinity_db):
        # CREATE INDEX idx1 ON test_hnsw (col1) USING Hnsw WITH (M = 16, ef_construction = 50, ef = 50, metric = l2);
        db_obj = get_infinity_db
        res = db_obj.drop_table("test_index_hnsw", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_hnsw", {"c1": "vector,1024,float"}, ConflictType.Error)
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
                                                      ])], ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

    def test_create_index_fulltext(self, get_infinity_db):
        # CREATE INDEX ft_index ON enwiki(body) USING FULLTEXT;
        db_obj = get_infinity_db
        res = db_obj.drop_table("test_index_fulltext", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_fulltext", {"doctitle": "varchar", "docdate": "varchar", "body": "varchar"}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("body",
                                                      index.IndexType.FullText,
                                                      []),
                                      ], ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

    # drop non-existent index
    def test_drop_non_existent_index(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        res = db_obj.drop_table(
            "test_drop_non_existent_index", ConflictType.Ignore)

        table_obj = db_obj.create_table("test_drop_non_existent_index", {
            "c1": "vector,3,float"}, ConflictType.Error)
        assert table_obj is not None

        # drop none existent index
        with pytest.raises(Exception, match="ERROR:3023*"):
            table_obj.drop_index("none_index")

    # create created index
    def test_create_created_index(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_created_index", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_created_index", {
            "c1": "vector,3,float"}, ConflictType.Error)

        # create index
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # create created index
        with pytest.raises(Exception, match="ERROR:3018*"):
            res = table_obj.create_index("my_index",
                                         [index.IndexInfo("c1",
                                                          index.IndexType.IVFFlat,
                                                          [index.InitParameter("centroids_count", "128"),
                                                           index.InitParameter("metric", "l2")])], ConflictType.Error)
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
        db_obj.drop_table(
            "test_create_drop_vector_index_invalid_options", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_drop_vector_index_invalid_options", {
            "c1": types}, ConflictType.Error)

        index_info = [index.IndexInfo(
            column_name[0], index_type[0], params[0])]
        if not column_name[1] or not index_type[1] or not params[1]:
            with pytest.raises(Exception):
                table_obj.create_index(
                    "my_index", index_info, ConflictType.Error)
        else:
            table_obj.create_index("my_index", index_info, ConflictType.Error)

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
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False),
        ((1, 2), False), ({"1": 2}, False), ([], True)
    ])
    @pytest.mark.parametrize("types", ["int", "int8", "int16", "int32", "int64", "integer",
                                       "float", "float32", "double", "float64",
                                       "varchar", "bool", "vector, 3, float"])
    def test_create_drop_different_fulltext_index_invalid_options(self, get_infinity_db, column_name, index_type,
                                                                  params, types):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table(
            "test_create_drop_different_fulltext_index_invalid_options", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_drop_different_fulltext_index_invalid_options", {
            "c1": types}, ConflictType.Error)

        index_info = [index.IndexInfo(
            column_name[0], index_type[0], params[0])]
        if types != "varchar" or not column_name[1] or not index_type[1] or not params[1]:
            with pytest.raises(Exception):
                table_obj.create_index(
                    "my_index", index_info, ConflictType.Error)
        else:
            table_obj.create_index("my_index", index_info, ConflictType.Error)

    # create index on dropped table instance
    def test_create_index_on_dropped_table(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table(
            "test_create_drop_index_invalid_options", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_drop_index_invalid_options", {
            "c1": "vector,3,float"}, ConflictType.Error)
        db_obj.drop_table(
            "test_create_drop_index_invalid_options")

        # create created index
        with pytest.raises(Exception, match="ERROR:3022*"):
            table_obj.create_index("my_index",
                                   [index.IndexInfo("c1",
                                                    index.IndexType.IVFFlat,
                                                    [index.InitParameter("centroids_count", "128"),
                                                     index.InitParameter("metric", "l2")])], ConflictType.Error)

    # create index then show index
    def test_create_index_show_index(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_index_show_index", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_index_show_index", {
            "c1": "vector,3,float"}, ConflictType.Error)
        # create created index
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = table_obj.show_index("my_index")
        print(res)

    # drop index then show index
    def test_drop_index_show_index(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_index_show_index", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_index_show_index", {
            "c1": "vector,3,float"}, ConflictType.Error)
        # create created index
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.show_index("my_index")
        assert res.error_code == ErrorCode.OK

        res = table_obj.list_indexes()
        assert res.error_code == ErrorCode.OK

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
            "test_create_index_on_different_type_of_column", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_index_on_different_type_of_column", {
            "c1": types}, ConflictType.Error)
        # create created index
        if not index_type[1]:
            with pytest.raises(Exception, match=index_type[2]):
                table_obj.create_index("my_index",
                                       [index.IndexInfo("c1",
                                                        index_type[0],
                                                        [index.InitParameter("centroids_count", "128"),
                                                         index.InitParameter("metric", "l2")])], ConflictType.Error)
        else:
            res = table_obj.create_index("my_index",
                                         [index.IndexInfo("c1",
                                                          index_type[0],
                                                          [index.InitParameter("centroids_count", "128"),
                                                           index.InitParameter("metric", "l2")])], ConflictType.Error)
            assert res.error_code == ErrorCode.OK

    # insert data, then create index
    @pytest.mark.parametrize("index_type", [
        index.IndexType.IVFFlat
    ])
    def test_insert_data_create_index(self, get_infinity_db, index_type):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_insert_data_create_index", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_data_create_index", {
            "c1": "vector,1024,float"}, ConflictType.Error)
        values = [{"c1": [1.1 for _ in range(1024)]}]
        table_obj.insert(values)

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index_type,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_type", [
        (index.IndexType.IVFFlat, True),
        (index.IndexType.FullText, False, "ERROR:3009*")
    ])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_import_data_create_index(self, get_infinity_db, index_type, file_format):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_import_data_create_index", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_data_create_index", {
            "c1": "int",
            "c2": "vector,3,float"}, ConflictType.Error)

        table_obj.import_data(os.getcwd() + TEST_DATA_DIR +
                              file_format + "/pysdk_test." + file_format)
        if (index_type[1]):
            res = table_obj.create_index("my_index",
                                         [index.IndexInfo("c2",
                                                          index_type[0],
                                                          [index.InitParameter("centroids_count", "128"),
                                                           index.InitParameter("metric", "l2")])], ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        else:
            with pytest.raises(Exception, match=index_type[2]):
                table_obj.create_index("my_index",
                                       [index.IndexInfo("c2",
                                                        index_type[0],
                                                        [index.InitParameter("centroids_count", "128"),
                                                         index.InitParameter("metric", "l2")])], ConflictType.Error)

    # create index then insert / import data
    @pytest.mark.xfail(reason="ERROR:3009, Attempt to create IVFFLAT/full-text index on column: c2, data type")
    @pytest.mark.parametrize("index_type", [index.IndexType.IVFFlat, index.IndexType.FullText])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_create_index_import_data(self, get_infinity_db, index_type, file_format):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_import_data_create_index", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_import_data_create_index", {
            "c1": "int",
            "c2": "vector,3,float"}, ConflictType.Error)
        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c2",
                                                      index_type,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        table_obj.import_data(os.getcwd() + TEST_DATA_DIR +
                              file_format + "/pysdk_test." + file_format)

    @pytest.mark.parametrize("file_format", ["csv"])
    def test_insert_data_fulltext_index_search(self, get_infinity_db, file_format):
        # prepare data for insert
        column_names = ["doctitle", "docdate", "body"]
        df = pandas.read_csv(os.getcwd() + TEST_DATA_DIR + file_format + "/enwiki_99." + file_format,
                             delimiter="\t",
                             header=None,
                             names=column_names)
        data = {key: list(value.values()) for key, value in df.to_dict().items()}

        db_obj = get_infinity_db
        db_obj.drop_table("test_insert_data_fulltext_index_search", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_data_fulltext_index_search", {
            "doctitle": "varchar",
            "docdate": "varchar", "body": "varchar"}, ConflictType.Error)
        res = table_obj.create_index("body_index",
                                     [index.IndexInfo("body",
                                                      index.IndexType.FullText,
                                                      [])])
        assert res.error_code == ErrorCode.OK

        for i in range(len(data["doctitle"])):
            value = [{"doctitle": data["doctitle"][i], "docdate": data["docdate"][i], "body": data["body"][i]}]
            table_obj.insert(value)
        time.sleep(10)
        res = table_obj.output(["doctitle", "docdate", "_row_id", "_score"]).match(
            "body^5", "harmful chemical", "topn=3").to_pl()
        assert not res.is_empty()
        print(res)
        for i in range(len(data.items())):
            value = [{"doctitle": data["doctitle"][i], "docdate": data["docdate"][i], "body": data["body"][i]}]
            table_obj.insert(value)
        time.sleep(10)
        res = table_obj.output(["doctitle", "docdate", "_row_id", "_score"]).match(
            "body^5", "harmful chemical", "topn=3").to_pl()
        assert not res.is_empty()
        print(res)

        res = table_obj.create_index("doctitle_index",
                                     [index.IndexInfo("doctitle",
                                                      index.IndexType.FullText,
                                                      [])])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["doctitle", "docdate", "_row_id", "_score"]).match(
            "doctitle,body^5", "harmful chemical anarchism", "topn=3").to_pl()
        assert not res.is_empty()
        print(res)

    # create index on all data are deleted table.
    def test_create_index_on_deleted_table(self, get_infinity_db):
        db_obj = get_infinity_db
        db_obj.drop_table(
            "test_create_index_on_deleted_table", ConflictType.Ignore)

        table_obj = db_obj.create_table("test_create_index_on_deleted_table", {"c1": "vector,128,float"},
                                        ConflictType.Error)
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
                                                       index.InitParameter("metric", "l2")])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # create index on all data are updated.
    @pytest.mark.xfail(reason="Not support to convert Embedding to Embedding")
    def test_create_index_on_update_table(self, get_infinity_db):
        db_obj = get_infinity_db
        db_obj.drop_table("test_create_index_on_update_table",
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_index_on_update_table", {"c1": "vector,128,float", "c2": "int"},
                                        ConflictType.Error)

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

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               ConflictType.Error,
                                               ConflictType.Replace,
                                               0,
                                               1,
                                               2,
                                               pytest.param(
                                                   1.1, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   "#@$@!%string", marks=pytest.mark.xfail),
                                               pytest.param(
                                                   [], marks=pytest.mark.xfail),
                                               pytest.param(
                                                   {}, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   (), marks=pytest.mark.xfail),
                                               ])
    def test_create_index_with_various_options(self, get_infinity_db, conflict_type):
        db_obj = get_infinity_db
        db_obj.drop_table(
            "test_create_index_with_various_options", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_create_index_with_various_options",
            {"c1": "vector,1024,float"}, ConflictType.Error)
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
                                                      ])], conflict_type)

        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               0,
                                               pytest.param(
                                                   ConflictType.Error, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   ConflictType.Replace, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   1, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   2, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   1.1, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   "#@$@!%string", marks=pytest.mark.xfail),
                                               pytest.param(
                                                   [], marks=pytest.mark.xfail),
                                               pytest.param(
                                                   {}, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   (), marks=pytest.mark.xfail),
                                               ])
    def test_create_duplicated_index_with_various_options(self, get_infinity_db, conflict_type):
        db_obj = get_infinity_db
        db_obj.drop_table(
            "test_create_duplicated_index_with_various_options", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_create_duplicated_index_with_various_options",
            {"c1": "vector,1024,float"}, ConflictType.Error)
        assert table_obj is not None
        for i in range(10):
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
                                                          ])], conflict_type)

            assert res.error_code == ErrorCode.OK

    def test_show_index(self, get_infinity_db):
        db_obj = get_infinity_db
        db_obj.drop_table("test_show_index", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_show_index",
            {"c1": "vector,1024,float"}, ConflictType.Error)
        assert table_obj is not None
        for i in range(10):
            res = table_obj.create_index("my_index_" + str(i),
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
                                                          ])], ConflictType.Error)
            assert res.error_code == ErrorCode.OK
            res = table_obj.show_index("my_index_" + str(i))
            print(res)

    @pytest.mark.parametrize("index_name", [
        "my_index",
        pytest.param("Invalid name", marks=pytest.mark.xfail),
        pytest.param("not_exist_name", marks=pytest.mark.xfail),
        pytest.param(1, marks=pytest.mark.xfail),
        pytest.param(1.1, marks=pytest.mark.xfail),
        pytest.param(True, marks=pytest.mark.xfail),
        pytest.param([], marks=pytest.mark.xfail),
        pytest.param((), marks=pytest.mark.xfail),
        pytest.param({}, marks=pytest.mark.xfail),
    ])
    def test_show_various_name_index(self, get_infinity_db, index_name):
        db_obj = get_infinity_db
        db_obj.drop_table("test_show_various_name_index", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_show_various_name_index",
            {"c1": "vector,1024,float"}, ConflictType.Error)
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
                                                      ])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = table_obj.show_index(index_name)
        print(res)

    def test_list_index(self, get_infinity_db):
        db_obj = get_infinity_db
        db_obj.drop_table("test_list_index", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_list_index",
            {"c1": "vector,1024,float"}, ConflictType.Error)
        assert table_obj is not None
        for i in range(10):
            res = table_obj.create_index("my_index_" + str(i),
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
                                                          ])], ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = table_obj.list_indexes()
        print(res)

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               ConflictType.Error,
                                               0,
                                               1,
                                               pytest.param(
                                                   1.1, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   "#@$@!%string", marks=pytest.mark.xfail),
                                               pytest.param(
                                                   [], marks=pytest.mark.xfail),
                                               pytest.param(
                                                   {}, marks=pytest.mark.xfail),
                                               pytest.param(
                                                   (), marks=pytest.mark.xfail),
                                               ])
    def test_drop_index_with_various_options(self, get_infinity_db, conflict_type):
        db_obj = get_infinity_db
        db_obj.drop_table(
            "test_drop_index_with_various_options", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_drop_index_with_various_options",
            {"c1": "vector,1024,float"}, ConflictType.Error)
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
                                                      ])], ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", conflict_type)

        assert res.error_code == ErrorCode.OK

    # create index on same column with different parameters
    @pytest.mark.parametrize("index_distance_type",
                             ["l2", "ip",
                              pytest.param("cosine",
                                           marks=pytest.mark.xfail(reason="ERROR:3062, Lack index parameter")),
                              pytest.param("hamming",
                                           marks=pytest.mark.xfail(reason="ERROR:3062, Lack index parameter"))])
    def test_same_column_with_different_parameters(self, get_infinity_db, index_distance_type):
        db_obj = get_infinity_db
        db_obj.drop_table(
            "test_drop_index_with_various_options", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_drop_index_with_various_options",
            {"c1": "vector,1024,float"}, ConflictType.Error)
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
                                                              "metric", index_distance_type)
                                                      ])], ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", ConflictType.Ignore)

        assert res.error_code == ErrorCode.OK
