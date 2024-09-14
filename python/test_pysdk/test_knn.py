import importlib
import sys
import os
import pytest
from common import common_values
from common import common_index
import infinity
import infinity_embedded
from infinity.remote_thrift.infinity import RemoteThriftInfinityConnection
import infinity.index as index
from infinity.errors import ErrorCode
from infinity.common import ConflictType, InfinityException, SparseVector
from common.utils import copy_data, generate_commas_enwiki
import pandas as pd
from polars.testing import assert_frame_equal as pl_assert_frame_equal
from polars.testing import assert_frame_not_equal as pl_assert_frame_not_equal
from numpy import dtype
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
        module = importlib.import_module("infinity_embedded.index")
        globals()["index"] = module
        module = importlib.import_module("infinity_embedded.common")
        func = getattr(module, 'ConflictType')
        globals()['ConflictType'] = func
        func = getattr(module, 'InfinityException')
        globals()['InfinityException'] = func
        func = getattr(module, 'SparseVector')
        globals()['SparseVector'] = func
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
    # def test_version(self):
    #     self.test_infinity_obj._test_version()

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_knn(self, check_data, suffix):
        #
        # infinity
        #
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("fix_tmp_20240116"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("fix_tmp_20240116"+suffix, {
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
        # res = table_obj.output(["variant_id", "query_price"]).match_dense('gender_vector', [1.0] * 768, "float", "ip",
        #                                                                   10).to_pl()
        # print(res)

        # true
        res = table_obj.output(["variant_id", "_row_id"]).match_dense("gender_vector", [1.0] * 4, "float", "ip", 10).to_pl()
        print(res)

        # FIXME
        # res = table_obj.output(["variant_id", "query_is_recommend", "query_gender", "query_color", "query_price"]).match_dense(
        #     "gender_vector", [1.0] * 4, "float", "ip", 3).to_pl()

        # print(res)

        res = db_obj.drop_table("fix_tmp_20240116"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_knn_u8(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_knn_u8"+suffix, conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_knn_u8"+suffix, {
            "c1": {"type": "int"},
            "c2": {"type": "vector,3,uint8"}
        }, ConflictType.Error)
        test_csv_dir = "/var/infinity/test_data/embedding_int_dim3.csv"
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        print("import:", test_csv_dir, " start!")
        table_obj.import_data(test_csv_dir, None)
        table_obj.insert([{"c1": 11, "c2": [127, 128, 255]}])
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (1, 5, 9, 11), 'c2': ([2, 3, 4], [6, 7, 8], [10, 11, 12], [127, 128, 255])}).astype(
            {'c1': dtype('int32')}))
        res = table_obj.output(["c1", "_distance"]).match_dense('c2', [0, 0, 0], "uint8", "l2", 10).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (1, 5, 9, 11), 'DISTANCE': (29.0, 149.0, 365.0, 97538.0)}).astype(
            {'c1': dtype('int32'), 'DISTANCE': dtype('float32')}))
        with pytest.raises(InfinityException):
            table_obj.create_index("invalid_lvq", index.IndexInfo("c2", index.IndexType.Hnsw,
                                                                  {
                                                                      "M": "16",
                                                                      "ef_construction": "50",
                                                                      "metric": "l2",
                                                                      "encode": "lvq"
                                                                  }),
                                   ConflictType.Error)
        res = table_obj.create_index("valid_lvq", index.IndexInfo("c2", index.IndexType.Hnsw, {
            "M": "16",
            "ef_construction": "50",
            "metric": "l2"
        }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["c1", "_distance"]).match_dense('c2', [0, 0, 0], "uint8", "l2", 10).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (1, 5, 9, 11), 'DISTANCE': (29.0, 149.0, 365.0, 97538.0)}).astype(
            {'c1': dtype('int32'), 'DISTANCE': dtype('float32')}))
        with pytest.raises(InfinityException):
            table_obj.output(["c1", "_distance"]).match_dense('c2', [0, 0, 0], "int8", "l2", 10).to_result()
        res = db_obj.drop_table("test_knn_u8"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("save_elem_type", ["float", "float16", "bfloat16"])
    @pytest.mark.parametrize("query_elem_type", ["float", "float16", "bfloat16"])
    def test_knn_fp16_bf16(self, check_data, save_elem_type, query_elem_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_knn_fp16_bf16"+suffix, conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_knn_fp16_bf16"+suffix, {
            "c1": {"type": "int"},
            "c2": {"type": f"vector,3,{save_elem_type}"}
        }, ConflictType.Error)
        test_csv_dir = "/var/infinity/test_data/embedding_int_dim3.csv"
        if not check_data:
            copy_data("embedding_int_dim3.csv")
        print("import:", test_csv_dir, " start!")
        table_obj.import_data(test_csv_dir, None)
        table_obj.insert([{"c1": 11, "c2": [127, 128, 255]}])
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (1, 5, 9, 11), 'c2': ([2, 3, 4], [6, 7, 8], [10, 11, 12], [127, 128, 255])}).astype(
            {'c1': dtype('int32')}))
        res = table_obj.output(["c1", "_distance"]).match_dense('c2', [0, 0, 0], query_elem_type, "l2", 10).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (1, 5, 9, 11), 'DISTANCE': (29.0, 149.0, 365.0, 97538.0)}).astype(
            {'c1': dtype('int32'), 'DISTANCE': dtype('float32')}))
        res = db_obj.drop_table("test_knn_fp16_bf16"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_insert_multi_column(self, suffix):
        with pytest.raises(Exception, match=r".*value count mismatch*"):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_insert_multi_column"+suffix,
                              conflict_type=ConflictType.Ignore)
            table = db_obj.create_table("test_insert_multi_column"+suffix, {
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

        res = db_obj.drop_table("test_insert_multi_column"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("column_name", ["gender_vector",
                                             "color_vector"])
    def test_knn_on_vector_column(self, check_data, column_name, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_knn_on_vector_column"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_knn_on_vector_column"+suffix, {
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
        res = table_obj.output(["variant_id", "_row_id", "_similarity"]).match_dense(
            column_name, [1.0] * 4, "float", "ip", 2).to_pl()
        print(res)

        res = db_obj.drop_table(
            "test_knn_on_vector_column"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("column_name", [pytest.param("variant_id"),
                                             pytest.param("query_price"),
                                             # pytest.param(1, marks=pytest.mark.xfail),
                                             # pytest.param(2.2, marks=pytest.mark.xfail),
                                             # pytest.param("!@#/\#$ ## #$%  @#$^", marks=pytest.mark.xfail),
                                             ])
    def test_knn_on_non_vector_column(self, check_data, column_name, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_knn_on_non_vector_column"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_knn_on_non_vector_column"+suffix, {
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
            table_obj.output(["variant_id", "_row_id"]).match_dense(column_name, [1.0] * 4, "float", "ip", 2).to_pl()

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.SYNTAX_ERROR

        res = db_obj.drop_table(
            "test_knn_on_non_vector_column"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        (1, 2, 3, 4),
    ])
    def test_valid_embedding_data(self, check_data, embedding_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_valid_embedding_data"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_valid_embedding_data"+suffix, {
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
        res = table_obj.output(["variant_id", "_row_id"]).match_dense(
            "gender_vector", embedding_data, "float", "ip", 2).to_pl()
        print(res)

        res = db_obj.drop_table(
            "test_valid_embedding_data"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        pytest.param("variant_id"),
        pytest.param("gender_vector"),
        pytest.param(1),
        pytest.param(2.4),
        pytest.param([1] * 3),
        pytest.param((1, 2, 3)),
        pytest.param({"c": "12"}),
    ])
    def test_invalid_embedding_data(self, check_data, embedding_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_invalid_embedding_data"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_invalid_embedding_data"+suffix, {
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
            res = table_obj.output(["variant_id", "_row_id"]).match_dense(
                "gender_vector", embedding_data, "float", "ip", 2).to_pl()
            print(res)

        res = db_obj.drop_table(
            "test_invalid_embedding_data"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        [1.0] * 4,
    ])
    @pytest.mark.parametrize("embedding_data_type", [
        ("float", True),
    ])
    def test_valid_embedding_data_type(self, check_data, embedding_data, embedding_data_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_valid_embedding_data_type"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_valid_embedding_data_type"+suffix, {
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
            res = table_obj.output(["variant_id", "_distance"]).match_dense("gender_vector", embedding_data,
                                                                            embedding_data_type[0],
                                                                            "l2",
                                                                            2).to_pl()
            print(res)
        else:
            res = table_obj.output(["variant_id", "_similarity"]).match_dense("gender_vector", embedding_data,
                                                                              embedding_data_type[0],
                                                                              "ip",
                                                                              2).to_pl()

        res = db_obj.drop_table(
            "test_valid_embedding_data_type"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        [1.0] * 4,
    ])
    @pytest.mark.parametrize("embedding_data_type", [
        ("int", False),
        pytest.param(1),
        pytest.param(2.2),
        pytest.param("#@!$!@"),
    ])
    def test_invalid_embedding_data_type(self, check_data, embedding_data, embedding_data_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_invalid_embedding_data_type"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_invalid_embedding_data_type"+suffix, {
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
                res = table_obj.output(["variant_id"]).match_dense("gender_vector", embedding_data,
                                                                   embedding_data_type[0],
                                                                   "l2",
                                                                   2).to_pl()
                print(res)
            else:
                res = table_obj.output(["variant_id"]).match_dense("gender_vector", embedding_data,
                                                                   embedding_data_type[0],
                                                                   "ip",
                                                                   2).to_pl()
        res = db_obj.drop_table(
            "test_invalid_embedding_data_type"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        [1.0] * 4,
    ])
    @pytest.mark.parametrize("embedding_data_type", [
        # ("int", False),
        ("float", True),
    ])
    @pytest.mark.parametrize("distance_type", [
        ("l2", True),
        ("cosine", True),
        ("cos", True),
        ("ip", True),
        ("hamming", False),
    ])
    def test_various_distance_type(self, check_data, embedding_data, embedding_data_type,
                                   distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_distance_type"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_various_distance_type"+suffix, {
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
            res = table_obj.output(["variant_id"]).match_dense("gender_vector", embedding_data, embedding_data_type[0],
                                                               distance_type[0],
                                                               2).to_pl()
            print(res)
        else:
            with pytest.raises(InfinityException) as e:
                table_obj.output(["variant_id"]).match_dense("gender_vector", embedding_data, embedding_data_type[0],
                                                             distance_type[0],
                                                             2).to_pl()

            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.NOT_SUPPORTED

        res = db_obj.drop_table(
            "test_various_distance_type"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("topn", [
        (2, True),
        (10, True),
        (0, False, ErrorCode.INVALID_PARAMETER_VALUE),
        (-1, False, ErrorCode.INVALID_PARAMETER_VALUE),
        (1.1, False, ErrorCode.INVALID_TOPK_TYPE),
        ("test", False, ErrorCode.INVALID_TOPK_TYPE),
        ({}, False, ErrorCode.INVALID_TOPK_TYPE),
        ((), False, ErrorCode.INVALID_TOPK_TYPE),
        ([1] * 4, False, ErrorCode.INVALID_TOPK_TYPE),
    ])
    def test_various_topn(self, check_data, topn, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_topn"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_various_topn"+suffix, {
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
            res = table_obj.output(["variant_id"]).match_dense(
                "gender_vector", [1] * 4, "float", "l2", topn[0]).to_pl()
            print(res)
        else:
            with pytest.raises(InfinityException) as e:
                table_obj.output(["variant_id"]).match_dense(
                    "gender_vector", [1] * 4, "float", "l2", topn[0]).to_pl()

            assert e.type == InfinityException
            # assert e.value.args[0] == topn[2]

        res = db_obj.drop_table("test_various_topn"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("index_column_name", ["gender_vector",
                                                   "color_vector",
                                                   "category_vector",
                                                   "tag_vector",
                                                   "other_vector"])
    @pytest.mark.parametrize("knn_column_name", ["gender_vector",
                                                 "color_vector",
                                                 "category_vector",
                                                 "tag_vector",
                                                 "other_vector"])
    @pytest.mark.parametrize("index_distance_type", ["l2", "ip"])
    @pytest.mark.parametrize("knn_distance_type", ["l2", "ip"])
    def test_with_index_before(self, check_data, index_column_name, knn_column_name,
                               index_distance_type, knn_distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_index"+suffix, {
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
                                     index.IndexInfo(index_column_name,
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "M": "16",
                                                         "ef_construction": "50",
                                                         "metric": index_distance_type
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["variant_id"]).match_dense(
            knn_column_name, [1.0] * 4, "float", knn_distance_type, 5).to_pl()
        print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("index_column_name", ["gender_vector",
                                                   "color_vector",
                                                   "category_vector",
                                                   "tag_vector",
                                                   "other_vector"])
    @pytest.mark.parametrize("knn_column_name", ["gender_vector",
                                                 "color_vector",
                                                 "category_vector",
                                                 "tag_vector",
                                                 "other_vector"])
    @pytest.mark.parametrize("index_distance_type", ["l2", "ip"])
    @pytest.mark.parametrize("knn_distance_type", ["l2", "ip"])
    def test_with_index_after(self, check_data,
                              index_column_name, knn_column_name,
                              index_distance_type, knn_distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_index_after"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_index_after"+suffix, {
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
        res = table_obj.output(["variant_id"]).match_dense(
            knn_column_name, [1.0] * 4, "float", knn_distance_type, 5).to_pl()
        print(res)
        res = table_obj.create_index("my_index",
                                     index.IndexInfo(index_column_name,
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "M": "16",
                                                         "ef_construction": "50",
                                                         "metric": index_distance_type
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_with_index_after"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_99.csv", "data_dir": common_values.TEST_TMP_DIR}],
                             indirect=True)
    def test_fulltext_operator_option(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_fulltext_operator_option" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_fulltext_operator_option" + suffix,
                                        {"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"}})
        table_obj.create_index("my_index",
                               index.IndexInfo("body",
                                               index.IndexType.FullText,
                                               {"ANALYZER": "standard"}),
                               ConflictType.Error)
        if not check_data:
            copy_data("enwiki_99.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_99.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": "\t"})
        print('Test fulltext operator OR for query "TO BE OR NOT":')
        print(table_obj.output(["*", "_row_id", "_score"]).match_text("body^5", "TO BE OR NOT", 5,
                                                                      {"operator": "or"}).to_pl())
        print('Test fulltext operator AND for query "TO BE OR NOT":')
        print(table_obj.output(["*", "_row_id", "_score"]).match_text("body^5", "TO BE OR NOT", 5,
                                                                      {"operator": "and"}).to_pl())
        # expect throw
        print('No operator option for query "TO BE OR NOT", expect throw:')
        with pytest.raises(InfinityException) as e_info:
            table_obj.output(["*", "_row_id", "_score"]).match_text("body^5", "TO BE OR NOT", 5).to_pl()
        print(e_info.value.error_message)
        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_fulltext_operator_option" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("match_param_1", ["body^5"])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_valid_columns(self, check_data, match_param_1, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_valid_columns"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_valid_columns"+suffix,
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               index.IndexInfo("body",
                                               index.IndexType.FullText,
                                               {"ANALYZER": "standard"}),
                               ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj
               .output(["*"])
               .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
               .match_text(match_param_1, "black", 1)
               .fusion(method='rrf', topn=10)
               .to_pl())
        print(res)
        res_filter_1 = (table_obj
               .output(["*"])
               .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
               .match_text(match_param_1, "black", 1)
               .fusion(method='rrf', topn=10)
               .filter("num!=98 AND num != 12")
               .to_pl())
        print(res_filter_1)
        pl_assert_frame_not_equal(res, res_filter_1)
        res_filter_2 = (table_obj
               .output(["*"])
               .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1, {"filter": "num!=98 AND num != 12"})
               .match_text(match_param_1, "black", 1, {"filter": "num!=98 AND num != 12"})
               .fusion(method='rrf', topn=10)
               .to_pl())
        print(res_filter_2)
        pl_assert_frame_equal(res_filter_1, res_filter_2)
        with pytest.raises(InfinityException) as e_info:
            res_filter_3 = (table_obj
               .output(["*"])
               .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1, {"filter": "num!=98 AND num != 12"})
               .match_text(match_param_1, "black", 1, {"filter": "num!=98 AND num != 12"})
               .fusion(method='rrf', topn=10)
               .filter("num!=98 AND num != 12")
               .to_pl())
        print(e_info)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_valid_columns"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("match_param_1", [pytest.param(1),
                                               pytest.param(1.1),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               pytest.param("invalid column name")])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_invalid_columns(self, check_data, match_param_1, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_columns"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_invalid_columns"+suffix,
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               index.IndexInfo("body",
                                               index.IndexType.FullText,
                                               {"ANALYZER": "standard"}),
                               ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        with pytest.raises(Exception):
            res = (table_obj
                   .output(["*"])
                   .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
                   .match_text(match_param_1, "black", 1)
                   .fusion(method='rrf', topn=10)
                   .to_pl())
            print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_columns"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("match_param_2", ["a word a segment",
                                               "body=Greek"])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_valid_words(self, check_data, match_param_2, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_valid_words"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_valid_words"+suffix,
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               index.IndexInfo("body",
                                               index.IndexType.FullText,
                                               {"ANALYZER": "standard"}),
                               ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj
               .output(["*"])
               .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
               .match_text("body^5", match_param_2, 1)
               .fusion(method='rrf', topn=10)
               .to_pl())
        print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_valid_words"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("match_param_2", [pytest.param(1),
                                               pytest.param(1.1),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               pytest.param("@#$!#@$SDasdf3!@#$")])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_invalid_words(self, check_data, match_param_2, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_words"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_fulltext_match_with_invalid_words"+suffix,
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               index.IndexInfo("body",
                                               index.IndexType.FullText,
                                               {"ANALYZER": "standard"}),
                               ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})

        with pytest.raises(Exception):
            res = (table_obj
                   .output(["*"])
                   .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
                   .match_text("doctitle,num,body^5", match_param_2, 1)
                   .fusion(method='rrf', topn=10)
                   .to_pl())
            print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_fulltext_match_with_invalid_words"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("save_elem_t", ["float32", "float16", "bfloat16"])
    @pytest.mark.parametrize("query_elem_t", ["float32", "float16", "bfloat16"])
    def test_tensor_scan(self, check_data, save_elem_t, query_elem_t, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_tensor_scan"+suffix,
                                        {"title": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "t": {"type": f"tensor, 4, {save_elem_t}"},
                                         "body": {"type": "varchar"}})
        if not check_data:
            copy_data("tensor_maxsim.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "tensor_maxsim.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj
               .output(["*", "_row_id", "_score"])
               .match_tensor('t', [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]], query_elem_t, 3)
               .to_pl())
        print(res)
        with pytest.raises(InfinityException) as e:
            table_obj.output(["*", "_row_id", "_score"]).match_tensor('t',
                                                                      [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]],
                                                                      query_elem_t, 3, {"some_option": 222}).to_df()
        print(e)
        pd.testing.assert_frame_equal(
            table_obj.output(["title"]).match_tensor('t', [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]],
                                                     query_elem_t, 3).to_df(),
            pd.DataFrame({'title': ["test22", "test55", "test66"]}))
        res = db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_scan"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_sparse_scan"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int8"}},
                                        ConflictType.Error)
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj.output(["*", "_row_id", "_similarity"])
               .match_sparse("c2", SparseVector(**{"indices": [0, 20, 80], "values": [1.0, 2.0, 3.0]}), "ip", 3)
               .to_pl())
        print(res)

        res = db_obj.drop_table("test_sparse_scan"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_index(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_sparse_knn_with_index"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int8"}},
                                        ConflictType.Error)
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        table_obj.create_index("idx1",
                               index.IndexInfo("c2",
                                               index.IndexType.BMP,
                                               {"block_size": "8", "compress_type": "compress"}), ConflictType.Error)

        table_obj.optimize("idx1", {"topk": "3"})

        res = (table_obj
               .output(["*", "_row_id", "_similarity"])
               .match_sparse("c2", SparseVector(**{"indices": [0, 20, 80], "values": [1.0, 2.0, 3.0]}), "ip", 3,
                             {"alpha": "1.0", "beta": "1.0"})
               .to_pl())
        print(res)

        res = table_obj.drop_index("idx1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_multiple_fusion(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_multiple_fusion"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_multiple_fusion"+suffix,
                                        {"title": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "t": {"type": "tensor, 4, float"},
                                         "body": {"type": "varchar"}})
        table_obj.create_index("ft_index",
                               index.IndexInfo("body",
                                               index.IndexType.FullText,
                                               {"ANALYZER": "standard"}),
                               ConflictType.Error)
        if not check_data:
            copy_data("tensor_maxsim.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "tensor_maxsim.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj
               .output(["*", "_row_id", "_score"])
               .match_text('body', 'off', 4)
               .match_tensor('t', [[1.0, 0.0, 0.0, 0.0], [1.0, 0.0, 0.0, 0.0]], 'float', 2)
               .fusion(method='rrf', topn=10)
               .fusion(method='match_tensor', topn=2, fusion_params={'field': 't', 'data_type': 'float',
                                                                     'data': [[0.0, -10.0, 0.0, 0.7],
                                                                              [9.2, 45.6, -55.8, 3.5]]})
               .to_pl())
        print(res)

        res = db_obj.drop_table("test_with_multiple_fusion"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("index_column_name", ["gender_vector"])
    @pytest.mark.parametrize("knn_column_name", ["gender_vector"])
    @pytest.mark.parametrize("index_distance_type", ["l2", "ip", "cosine", "cos"])
    @pytest.mark.parametrize("knn_distance_type", ["l2", "ip", "cosine", "cos"])
    @pytest.mark.parametrize("index_type", [common_index.IndexType.Hnsw]) # Remove index.IndexType.IVFFlat, after IVFFlat support cosine metric
    def test_with_various_index_knn_distance_combination(self, check_data, index_column_name, knn_column_name,
                                                         index_distance_type, knn_distance_type, index_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_index"+suffix, {
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
        if index_type == common_index.IndexType.Hnsw:
            res = table_obj.create_index("my_index",
                                         index.IndexInfo(index_column_name,
                                                         index.IndexType.Hnsw,
                                                         {
                                                             "M": "16",
                                                             "ef_construction": "50",
                                                             "metric": index_distance_type
                                                         }), ConflictType.Error)
            assert res.error_code == ErrorCode.OK
            res = table_obj.output(["variant_id"]).match_dense(
                knn_column_name, [1.0] * 4, "float", knn_distance_type, 5).to_pl()
            print(res)
            res = table_obj.drop_index("my_index", ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.IVFFlat:
            if index_distance_type == "cosine" or index_distance_type == "cos":
                with pytest.raises(InfinityException) as e:
                    res = table_obj.create_index("my_index",
                                                 index.IndexInfo(index_column_name,
                                                                 index.IndexType.IVFFlat,
                                                                 {"centroids_count": "128", "metric": index_distance_type}),
                                                 ConflictType.Error)
                assert e.type == InfinityException
                assert e.value.args[0] == ErrorCode.NOT_SUPPORTED
            else:
                res = table_obj.create_index("my_index",
                                             index.IndexInfo(index_column_name,
                                                             index.IndexType.IVFFlat,
                                                             {"centroids_count": "128", "metric": index_distance_type}),
                                             ConflictType.Error)
                assert res.error_code == ErrorCode.OK
                # for IVFFlat, index_distance_type has to match knn_distance_type?
                res = table_obj.output(["variant_id"]).match_dense(
                    knn_column_name, [1.0] * 4, "float", index_distance_type, 5).to_pl()
                print(res)
                res = table_obj.drop_index("my_index", ConflictType.Error)
                assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_zero_dimension_vector(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_zero_dimension_vector"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_zero_dimension_vector"+suffix, {
            "zero_vector": {"type": "vector,0,float"},
        }, ConflictType.Error)

        # try to insert and search a non-zero dim vector
        with pytest.raises(InfinityException) as e:
            table_obj.insert([{"zero_vector": [0.0]}])
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DATA_TYPE_MISMATCH
        with pytest.raises(InfinityException) as e:
            res = table_obj.output(["_row_id"]).match_dense(
                "zero_vector", [0.0], "float", "l2", 5).to_pl()
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.SYNTAX_ERROR

        # try to insert and search a zero dim vector
        with pytest.raises(Exception):
            table_obj.insert([{"zero_vector": []}])
        try:
            res = table_obj.output(["_row_id"]).match_dense(
                "zero_vector", [], "float", "l2", 5).to_pl()
        except:
            print("Exception")

        res = db_obj.drop_table("test_zero_dimension_vector"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("dim", [1000, 16384])
    def test_big_dimension_vector(self, dim, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_big_dimension_vector"+suffix,
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table("test_big_dimension_vector"+suffix, {
            "big_vector": {"type": f"vector,{dim},float"},
        }, ConflictType.Error)
        table_obj.insert([{"big_vector": [1.0] * dim},
                          {"big_vector": [2.0] * dim},
                          {"big_vector": [3.0] * dim},
                          {"big_vector": [4.0] * dim},
                          {"big_vector": [5.0] * dim}])
        res = table_obj.output(["_row_id"]).match_dense(
            "big_vector", [0.0] * dim, "float", "l2", 5).to_pl()
        print(res)

        res = db_obj.drop_table("test_big_dimension_vector"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # "^5" indicates the point that column "body" get multipy by 5, default is multipy by 1
    # refer to https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-query-string-query.html
    @pytest.mark.parametrize("fields_and_matching_text", [
        ["body", "black"],
        ["body^5", "black"],
        ["", "body:black"],
        ["", "body:black^5"],
        ["", "'body':'(black)'"],
        ["", "body:'(black)^5'"],
        ["", "'body':'(black OR white)'"],
        ["", "'body':'(black AND white)'"],
        ["", "'body':'(black)^5 OR (white)'"],
        ["", "'body':'(black)^5 AND (white)'"],
        ["", "'body':'black - white'"],
        ["", "body:black OR body:black"],
        ["", "body:black AND body:black"],
        ["", "(body:black OR body:black) AND (body:white OR body:white)"],
        ["", "(body:black)^5 OR body:black"],
        ["", "(body:black)^5 AND body:black"],
        ["", "(body:black OR body:black)^5 AND (body:white OR body:white)"],
        # ["", "doc\*:back"] not support
    ])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_various_fulltext_match(self, check_data, fields_and_matching_text, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_with_various_fulltext_match"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_various_fulltext_match"+suffix,
                                        {"doctitle": {"type": "varchar"},
                                         "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "vec": {"type": "vector, 4, float"}})
        table_obj.create_index("my_index",
                               index.IndexInfo("body",
                                               index.IndexType.FullText,
                                               {"ANALYZER": "standard"}),
                               ConflictType.Error)

        if not check_data:
            generate_commas_enwiki(
                "enwiki_99.csv", "enwiki_embedding_99_commas.csv", 1)
            copy_data("enwiki_embedding_99_commas.csv")

        test_csv_dir = common_values.TEST_TMP_DIR + "enwiki_embedding_99_commas.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        res = (table_obj
               .output(["*"])
               .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 1)
               .match_text(fields_and_matching_text[0], fields_and_matching_text[1], 1)
               .fusion(method='rrf', topn=10)
               .to_pl())
        print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_with_various_fulltext_match"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("data_type", ['varchar',
                                           pytest.param(1),
                                           pytest.param(1.1),
                                           pytest.param([]),
                                           pytest.param({}),
                                           pytest.param(()),
                                           pytest.param("@#$!#@$SDasdf3!@#$")
                                           ])
    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_tensor_scan_with_invalid_data_type(self, check_data, data_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_tensor_scan"+suffix,
                                        {"title": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "t": {"type": "tensor, 4, float"},
                                         "body": {"type": "varchar"}})
        if not check_data:
            copy_data("tensor_maxsim.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "tensor_maxsim.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        with pytest.raises(Exception):
            res = (table_obj
                   .output(["*", "_row_id", "_score"])
                   .match_tensor('t', [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]], data_type, 2)
                   .to_pl())

        res = db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("extra_option", ['topn=-1',
                                              'topn=0',
                                              'topn=100000000',
                                              pytest.param(1),
                                              pytest.param(1.1),
                                              pytest.param([]),
                                              pytest.param(()),
                                              ])
    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_tensor_scan_with_invalid_extra_option(self, check_data, extra_option, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_tensor_scan"+suffix,
                                        {"title": {"type": "varchar"},
                                         "num": {"type": "int"},
                                         "t": {"type": "tensor, 4, float"},
                                         "body": {"type": "varchar"}})
        if not check_data:
            copy_data("tensor_maxsim.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "tensor_maxsim.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        with pytest.raises(Exception):
            table_obj.output(["*", "_row_id", "_score"]).match_tensor('t',
                                                                      [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]],
                                                                      'float', 3, extra_option).to_pl()

        res = db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    @pytest.mark.skip(
        reason="UnrecoverableException The tensor column basic embedding dimension should be greater than 0")
    def test_zero_dimension_tensor_scan(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_tensor_scan"+suffix,
                                        {"t": {"type": "tensor, 0, float"}})
        with pytest.raises(IndexError):
            table_obj.insert([{"t": [[], []]}])

        with pytest.raises(Exception):
            res = (table_obj
                   .output(["*", "_row_id", "_score"])
                   .match_tensor('t', [[], []], 'float', 2)
                   .to_pl())

        res = db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("dim", [1, 10, 100])  # 1^3, 10^3, 100^3
    def test_big_dimension_tensor_scan(self, dim, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_tensor_scan"+suffix,
                                        {"t": {"type": f"tensorarray, {dim}, float"}})

        table_obj.insert([{"t": [[[1.0] * dim] * dim] * dim},
                          {"t": [[[2.0] * dim] * dim] * dim},
                          {"t": [[[3.0] * dim] * dim] * dim},
                          {"t": [[[4.0] * dim] * dim] * dim},
                          {"t": [[[5.0] * dim] * dim] * dim}, ])

        res = (table_obj
               .output(["*", "_row_id", "_score"])
               .match_tensor('t', [[[0.0] * dim] * dim] * dim, 'float', 5)
               .to_pl())
        print(res)

        res = db_obj.drop_table("test_tensor_scan"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("table_params", [
        "vector,100,float,int8",
        "sparse,0,float,int8",
        "sparse,100,int,int8",
        "sparse,100,float,float",
        "int8,float,100,sparse",  # disorder
    ])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_with_invalid_table_params(self, check_data, table_params ,suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_scan"+suffix, ConflictType.Ignore)
        params = table_params.split(",")
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"

        if params[0] == "int8":
            with pytest.raises(InfinityException) as e:
                table_obj = db_obj.create_table("test_sparse_scan"+suffix,
                                                {"c1": {"type": "int"}, "c2": {"type": table_params}},
                                                ConflictType.Error)
            # assert e.value.args[0] == ErrorCode.INVALID_DATA_TYPE
        elif params[0] == "vector":
            table_obj = db_obj.create_table("test_sparse_scan"+suffix, {"c1": {"type": "int"}, "c2": {"type": table_params}},
                                            ConflictType.Error)
            table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})

            with pytest.raises(InfinityException) as e:
                res = (table_obj.output(["*", "_row_id", "_similarity"])
                       .match_sparse("c2", SparseVector(**{"indices": [0, 20, 80], "values": [1.0, 2.0, 3.0]}), "ip", 3)
                       .to_pl())
            assert e.value.args[0] == ErrorCode.SYNTAX_ERROR

            res = db_obj.drop_table("test_sparse_scan"+suffix, ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif params[1] == "0":
            table_obj = db_obj.create_table("test_sparse_scan"+suffix, {"c1": {"type": "int"}, "c2": {"type": table_params}},
                                            ConflictType.Error)
            table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})

            with pytest.raises(InfinityException) as e:
                res = (table_obj.output(["*", "_row_id", "_similarity"])
                       .match_sparse("c2", SparseVector(**{"indices": [0, 20, 80], "values": [1.0, 2.0, 3.0]}), "ip", 3)
                       .to_pl())
            assert e.value.args[0] == ErrorCode.DATA_TYPE_MISMATCH

            res = db_obj.drop_table("test_sparse_scan"+suffix, ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif params[2] == "int":
            table_obj = db_obj.create_table("test_sparse_scan"+suffix, {"c1": {"type": "int"}, "c2": {"type": table_params}},
                                            ConflictType.Error)
            with pytest.raises(InfinityException) as e:
                table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
            assert e.value.args[0] == ErrorCode.PARSER_ERROR
            res = db_obj.drop_table("test_sparse_scan" + suffix, ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif params[3] == "float":
            with pytest.raises(InfinityException) as e:
                table_obj = db_obj.create_table("test_sparse_scan"+suffix,
                                                {"c1": {"type": "int"}, "c2": {"type": table_params}},
                                                ConflictType.Error)
            assert e.value.args[0] == ErrorCode.INVALID_EMBEDDING_DATA_TYPE

    @pytest.mark.parametrize("index_type", [#common_index.IndexType.IVFFlat,
                                            common_index.IndexType.Hnsw,
                                            common_index.IndexType.EMVB,
                                            common_index.IndexType.FullText,
                                            common_index.IndexType.Secondary, ])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_invalid_index_type(self, check_data, index_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_sparse_knn_with_index"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int8"}},
                                        ConflictType.Error)
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})

        with pytest.raises(InfinityException) as e:
            if index_type == common_index.IndexType.IVFFlat:
                res = table_obj.create_index("my_index",
                                             index.IndexInfo("c2",
                                                             index.IndexType.IVFFlat,
                                                             {"centroids_count": "128", "metric": "L2"}),
                                             ConflictType.Error)
            elif index_type == common_index.IndexType.Hnsw:
                res = table_obj.create_index("my_index",
                                             index.IndexInfo("c2",
                                                             index.IndexType.Hnsw,
                                                             {
                                                                 "M": "16",
                                                                 "ef_construction": "50",
                                                                 "metric": "L2"
                                                             }), ConflictType.Error)
            elif index_type == common_index.IndexType.EMVB:
                res = table_obj.create_index("my_index",
                                             index.IndexInfo("c2",
                                                             index.IndexType.EMVB,
                                                             {"pq_subspace_num": "32", "pq_subspace_bits": "8"}),
                                             ConflictType.Error)
            elif index_type == common_index.IndexType.FullText:
                res = table_obj.create_index("my_index",
                                             index.IndexInfo("c2",
                                                             index.IndexType.FullText,
                                                             {"ANALYZER": "STANDARD"}),
                                             ConflictType.Error)
            elif index_type == common_index.IndexType.Secondary:
                res = table_obj.create_index("my_index",
                                             index.IndexInfo("c2",
                                                             index.IndexType.Secondary),
                                             ConflictType.Error)
        assert e.value.args[0] == ErrorCode.INVALID_INDEX_DEFINITION

        res = db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_params", [["0", "compress"],
                                              ["257", "compress"],
                                              ["16", "invalid compress type"]])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_invalid_index_params(self, check_data, index_params ,suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_sparse_knn_with_index"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int8"}},
                                        ConflictType.Error)
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        with pytest.raises(InfinityException) as e:
            table_obj.create_index("idx1",
                                   index.IndexInfo("c2",
                                                   index.IndexType.BMP,
                                                   {"block_size": index_params[0], "compress_type": index_params[1]}), ConflictType.Error)
        assert e.value.args[0] == ErrorCode.INVALID_INDEX_PARAM

        res = db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip(reason="invalid alpha and beta do not raise exception")
    @pytest.mark.parametrize("alpha", ["-1.0", "2.0"])
    @pytest.mark.parametrize("beta", ["-1.0", "2.0"])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_invalid_alpha_beta(self, check_data, alpha, beta, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_sparse_knn_with_index"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int8"}},
                                        ConflictType.Error)
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})
        table_obj.create_index("idx1",
                               index.IndexInfo("c2",
                                               index.IndexType.BMP,
                                               {"block_size": "8", "compress_type": "compress"}), ConflictType.Error)

        table_obj.optimize("idx1", {"topk": "3"})

        with pytest.raises(InfinityException) as e:
            res = (table_obj
                   .output(["*", "_row_id", "_similarity"])
                   .match_sparse("c2", SparseVector(**{"indices": [0, 20, 80], "values": [1.0, 2.0, 3.0]}), "ip", 3,
                                 {"alpha": alpha, "beta": beta})
                   .to_pl())

        res = table_obj.drop_index("idx1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip(reason="UnrecoverableException Sparse data size mismatch")
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_indices_values_mismatch(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_sparse_knn_with_index"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int8"}},
                                        ConflictType.Error)
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})

        res = (table_obj
               .output(["*", "_row_id", "_similarity"])
               .match_sparse("c2", SparseVector(**{"indices": [0, 20], "values": [1.0, 2.0, 3.0]}), "ip", 3,
                             {"alpha": "1.0", "beta": "1.0"})
               .to_pl())
        print(res)

        res = db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("distance_type", ["l2", "cosine", "hamming"])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_invalid_distance_type(self, check_data, distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_sparse_knn_with_index"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "sparse,100,float,int8"}},
                                        ConflictType.Error)
        if not check_data:
            copy_data("sparse_knn.csv")
        test_csv_dir = common_values.TEST_TMP_DIR + "sparse_knn.csv"
        table_obj.import_data(test_csv_dir, import_options={"delimiter": ","})

        with pytest.raises(Exception):
            res = (table_obj
                   .output(["*", "_row_id", "_similarity"])
                   .match_sparse("c2", SparseVector(**{"indices": [0, 20, 80], "values": [1.0, 2.0, 3.0]}), distance_type, 3,
                                 {"alpha": "1.0", "beta": "1.0"})
                   .to_pl())

        res = db_obj.drop_table("test_sparse_knn_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("knn_distance_type", ["l2", "ip"])
    def test_knn_with_given_index_name(self, check_data, knn_distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_index"+suffix, {
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
        res = table_obj.create_index("my_index_l2",
                                     index.IndexInfo("gender_vector",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "M": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.create_index("my_index_ip",
                                     index.IndexInfo("gender_vector",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "M": "16",
                                                         "ef_construction": "50",
                                                         "metric": "ip"
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["variant_id"]).match_dense(
            "gender_vector", [1.0] * 4, "float", knn_distance_type, 5, {"index_name":"my_index_l2"}).to_pl()
        print(res)

        res = table_obj.output(["variant_id"]).match_dense(
            "gender_vector", [1.0] * 4, "float", knn_distance_type, 5, {"index_name": "my_index_ip"}).to_pl()
        print(res)

        res = table_obj.drop_index("my_index_l2", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index_ip", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("knn_distance_type", ["l2", "ip"])
    def test_knn_with_ignore_index(self, check_data, knn_distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_index"+suffix, {
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
        res = table_obj.create_index("my_index_l2",
                                     index.IndexInfo("gender_vector",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "M": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["variant_id"]).match_dense(
            "gender_vector", [1.0] * 4, "float", knn_distance_type, 5, {"ignore_index":"true"}).to_pl()
        print(res)

        res = table_obj.drop_index("my_index_l2", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("knn_distance_type", ["l2", "ip"])
    def test_knn_with_given_invalid_index_name(self, check_data, knn_distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_index"+suffix, {
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
        res = table_obj.create_index("my_index_l2",
                                     index.IndexInfo("gender_vector",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "M": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        with  pytest.raises(InfinityException) as e:
            res = table_obj.output(["variant_id"]).match_dense(
                "gender_vector", [1.0] * 4, "float", knn_distance_type, 5, {"index_name": "my_index_ip"}).to_pl()
        assert e.value.args[0] == ErrorCode.INDEX_NOT_EXIST

        res = table_obj.drop_index("my_index_l2", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("knn_distance_type", ["l2", "ip"])
    def test_knn_with_given_index_name_and_ignore_index(self, check_data, knn_distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_index"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_index"+suffix, {
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
        res = table_obj.create_index("my_index_l2",
                                     index.IndexInfo("gender_vector",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "M": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["variant_id"]).match_dense(
                "gender_vector", [1.0] * 4, "float", knn_distance_type, 5, {"index_name": "my_index_l2", "ignore_index":"false"}).to_pl()
        print(res)

        with  pytest.raises(InfinityException) as e:
            res = table_obj.output(["variant_id"]).match_dense(
                    "gender_vector", [1.0] * 4, "float", knn_distance_type, 5, {"index_name": "my_index_l2", "ignore_index":"true"}).to_pl()
        assert e.value.args[0] == ErrorCode.SYNTAX_ERROR

        res = table_obj.drop_index("my_index_l2", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_with_index"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
