import pytest
from common import common_values
from internal.test_knn import TestKnn
from infinity.common import ConflictType
from internal.utils import trace_expected_exceptions
from infinity.errors import ErrorCode
import infinity.index as index

@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def setup_class(request, local_infinity):
    if local_infinity:
        uri = common_values.TEST_LOCAL_PATH
    else:
        uri = common_values.TEST_LOCAL_HOST
    request.cls.uri = uri
    request.cls.test_infinity_obj = TestKnn(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    # def test_version(self):
    #     self.test_infinity_obj._test_version()

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_knn(self, check_data):
        self.test_infinity_obj._test_knn(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_knn_u8(self, check_data):
        self.test_infinity_obj._test_knn_u8(check_data)

    def test_insert_multi_column(self):
        self.test_infinity_obj._test_insert_multi_column()

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("column_name", ["gender_vector",
                                             "color_vector"])
    def test_knn_on_vector_column(self, check_data, column_name):
        self.test_infinity_obj._test_knn_on_vector_column(check_data, column_name)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("column_name", [pytest.param("variant_id"),
                                             pytest.param("query_price"),
                                             # pytest.param(1, marks=pytest.mark.xfail),
                                             # pytest.param(2.2, marks=pytest.mark.xfail),
                                             # pytest.param("!@#/\#$ ## #$%  @#$^", marks=pytest.mark.xfail),
                                             ])
    def test_knn_on_non_vector_column(self, check_data, column_name):
        self.test_infinity_obj._test_knn_on_non_vector_column(check_data, column_name)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        (1, 2, 3, 4),
    ])
    def test_valid_embedding_data(self, check_data, embedding_data):
        self.test_infinity_obj._test_valid_embedding_data(check_data, embedding_data)

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
    def test_invalid_embedding_data(self, check_data, embedding_data):
        self.test_infinity_obj._test_invalid_embedding_data(check_data, embedding_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        [1.0] * 4,
    ])
    @pytest.mark.parametrize("embedding_data_type", [
        ("float", True),
    ])
    def test_valid_embedding_data_type(self, check_data, embedding_data, embedding_data_type):
        self.test_infinity_obj._test_valid_embedding_data_type(check_data, embedding_data,
                                                          embedding_data_type)

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
    def test_invalid_embedding_data_type(self, check_data, embedding_data, embedding_data_type):
        self.test_infinity_obj._test_invalid_embedding_data_type(check_data, embedding_data,
                                                            embedding_data_type)

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
        ("ip", True),
        ("hamming", False),
    ])
    def test_various_distance_type(self, check_data, embedding_data, embedding_data_type,
                                   distance_type):
        self.test_infinity_obj._test_various_distance_type(check_data, embedding_data, embedding_data_type,
                                                      distance_type)

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
    def test_various_topn(self, check_data, topn):
        self.test_infinity_obj._test_various_topn(check_data, topn)

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
                               index_distance_type, knn_distance_type):
        self.test_infinity_obj._test_with_index_before(check_data, index_column_name, knn_column_name,
                                                  index_distance_type, knn_distance_type)

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
                              index_distance_type, knn_distance_type):
        self.test_infinity_obj._test_with_index_after(check_data, index_column_name, knn_column_name,
                                                 index_distance_type, knn_distance_type)

    @pytest.mark.parametrize("match_param_1", ["doctitle,num,body^5"])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_valid_columns(self, check_data, match_param_1):
        self.test_infinity_obj._test_with_fulltext_match_with_valid_columns(check_data, match_param_1)

    @pytest.mark.parametrize("match_param_1", [pytest.param(1),
                                               pytest.param(1.1),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               pytest.param("invalid column name")])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_invalid_columns(self, check_data, match_param_1):
        self.test_infinity_obj._test_with_fulltext_match_with_invalid_columns(check_data, match_param_1)

    @pytest.mark.parametrize("match_param_2", ["a word a segment",
                                               "body=Greek"])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_valid_words(self, check_data, match_param_2):
        self.test_infinity_obj._test_with_fulltext_match_with_valid_words(check_data, match_param_2)

    @pytest.mark.parametrize("match_param_2", [pytest.param(1),
                                               pytest.param(1.1),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               pytest.param("@#$!#@$SDasdf3!@#$")])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_invalid_words(self, check_data, match_param_2):
        self.test_infinity_obj._test_with_fulltext_match_with_invalid_words(check_data, match_param_2)

    @pytest.mark.parametrize("match_param_3", [pytest.param("@#$!#@$SDa^sdf3!@#$"),
                                               "topn=1",
                                               "1"])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_options(self, check_data, match_param_3):
        self.test_infinity_obj._test_with_fulltext_match_with_options(check_data, match_param_3)

    @pytest.mark.parametrize("match_param_3", [pytest.param(1),
                                               pytest.param(1.1),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()), ])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_fulltext_match_with_invalid_options(self, check_data, match_param_3):
        self.test_infinity_obj._test_with_fulltext_match_with_invalid_options(check_data, match_param_3)

    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_tensor_scan(self, check_data):
        self.test_infinity_obj._test_tensor_scan(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                            "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn(self, check_data):
        self.test_infinity_obj._test_sparse_scan(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_index(self, check_data):
        self.test_infinity_obj._test_sparse_knn_with_index(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_multiple_fusion(self, check_data):
        self.test_infinity_obj._test_with_multiple_fusion(check_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "pysdk_test_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("index_column_name", ["gender_vector"])
    @pytest.mark.parametrize("knn_column_name", ["gender_vector"])
    @pytest.mark.parametrize("index_distance_type", ["l2","ip", "cosine"])
    @pytest.mark.parametrize("knn_distance_type", ["l2", "ip", "cosine"])
    @pytest.mark.parametrize("index_type", [index.IndexType.Hnsw, index.IndexType.IVFFlat])
    def test_with_various_index_knn_distance_combination(self, check_data, index_column_name, knn_column_name,
                               index_distance_type, knn_distance_type, index_type):
        self.test_infinity_obj._test_with_various_index_knn_distance_combination(check_data, index_column_name, knn_column_name,
                                                  index_distance_type, knn_distance_type, index_type)

    def test_zero_dimension_vector(self):
        self.test_infinity_obj._test_zero_dimension_vector()

    @pytest.mark.parametrize("dim", [1000, 16384])
    def test_big_dimension_vector(self, dim):
        self.test_infinity_obj._test_big_dimension_vector(dim)

    # "^5" indicates the point that column "body" get multipy by 5, default is multipy by 1
    # refer to https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-query-string-query.html
    @pytest.mark.parametrize("fields_and_matching_text", [
                                        ["body","black"],
                                        ["doctitle,num,body", "black"],
                                        ["doctitle,num,body^5", "black"],
                                        ["", "body:black"],
                                        ["", "body:black^5"],
                                        ["", "'body':'(black)'"],
                                        ["", "body:'(black)^5'"],
                                        ["", "'body':'(black OR white)'"],
                                        ["", "'body':'(black AND white)'"],
                                        ["", "'body':'(black)^5 OR (white)'"],
                                        ["", "'body':'(black)^5 AND (white)'"],
                                        ["", "'body':'black - white'"],
                                        ["", "body:black OR doctitle:black"],
                                        ["", "body:black AND doctitle:black"],
                                        ["", "(body:black OR doctitle:black) AND (body:white OR doctitle:white)"],
                                        ["", "(body:black)^5 OR doctitle:black"],
                                        ["", "(body:black)^5 AND doctitle:black"],
                                        ["", "(body:black OR doctitle:black)^5 AND (body:white OR doctitle:white)"],
                                        #["", "doc\*:back"] not support
                                        ])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_various_fulltext_match(self, check_data, fields_and_matching_text):
        self.test_infinity_obj._test_with_various_fulltext_match(check_data, fields_and_matching_text)

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
    def test_tensor_scan_with_invalid_data_type(self, check_data, data_type):
        self.test_infinity_obj._test_tensor_scan_with_invalid_data_type(check_data, data_type)

    @pytest.mark.parametrize("method_type", ['invalid method type',
                                            pytest.param(1),
                                            pytest.param(1.1),
                                            pytest.param([]),
                                            pytest.param({}),
                                            pytest.param(()),
                                            pytest.param("@#$!#@$SDasdf3!@#$")
                                            ])
    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                                "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_tensor_scan_with_invalid_method_type(self, check_data, method_type):
        self.test_infinity_obj._test_tensor_scan_with_invalid_method_type(check_data, method_type)

    @pytest.mark.parametrize("extra_option", ['topn=-1',
                                              'topn=0',
                                              'topn=100000000',
                                            pytest.param(1),
                                            pytest.param(1.1),
                                            pytest.param([]),
                                            pytest.param({}),
                                            pytest.param(()),
                                            ])
    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                                "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_tensor_scan_with_invalid_extra_option(self, check_data, extra_option):
        self.test_infinity_obj._test_tensor_scan_with_invalid_extra_option(check_data, extra_option)

    @pytest.mark.skip(reason = "UnrecoverableException The tensor column basic embedding dimension should be greater than 0")
    def test_zero_dimension_tensor_scan(self):
        self.test_infinity_obj._test_zero_dimension_tensor_scan()

    @pytest.mark.parametrize("dim", [1, 10, 100]) #1^3, 10^3, 100^3
    def test_big_dimension_tensor_scan(self, dim):
        self.test_infinity_obj._test_big_dimension_tensor_scan(dim)

    @pytest.mark.parametrize("table_params", [
        "vector,100,float,int8",
        "sparse,0,float,int8",
        "sparse,100,int,int8",
        "sparse,100,float,float",
        "int8,float,100,sparse", #disorder
    ])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                            "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_with_invalid_table_params(self, check_data, table_params):
        self.test_infinity_obj._test_sparse_with_invalid_table_params(check_data, table_params)

    @pytest.mark.parametrize("index_type", [index.IndexType.IVFFlat,
                                            index.IndexType.Hnsw,
                                            index.IndexType.EMVB,
                                            index.IndexType.FullText,
                                            index.IndexType.Secondary,])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_invalid_index_type(self, check_data, index_type):
        self.test_infinity_obj._test_sparse_knn_with_invalid_index_type(check_data, index_type)

    @pytest.mark.parametrize("index_params", [["0", "compress"],
                                              ["257", "compress"],
                                              ["16", "invalid compress type"]])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_invalid_index_params(self, check_data, index_params):
        self.test_infinity_obj._test_sparse_knn_with_invalid_index_params(check_data, index_params)


    @pytest.mark.skip(reason = "invalid alpha and beta do not raise exception")
    @pytest.mark.parametrize("alpha", ["-1.0", "2.0"])
    @pytest.mark.parametrize("beta", ["-1.0", "2.0"])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_invalid_alpha_beta(self, check_data, alpha, beta):
        self.test_infinity_obj._test_sparse_knn_with_invalid_alpha_beta(check_data, alpha, beta)

    @pytest.mark.skip(reason = "UnrecoverableException Sparse data size mismatch")
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_indices_values_mismatch(self, check_data):
        self.test_infinity_obj._test_sparse_knn_with_indices_values_mismatch(check_data)

    @pytest.mark.parametrize("distance_type", ["l2", "cosine", "hamming"])
    @pytest.mark.parametrize("check_data", [{"file_name": "sparse_knn.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_sparse_knn_with_invalid_distance_type(self, check_data, distance_type):
        self.test_infinity_obj._test_sparse_knn_with_invalid_distance_type(check_data, distance_type)