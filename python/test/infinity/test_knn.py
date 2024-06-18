import pytest
from common import common_values
from src.test_knn import TestKnn
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
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
        uri = common_values.TEST_REMOTE_HOST
    request.cls.uri = uri
    request.cls.test_infinity_obj = TestKnn(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    def test_version(self):
        self.test_infinity_obj._test_version()

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_knn(self, check_data):
        self.test_infinity_obj._test_knn(check_data)

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
        ("cosine", False),
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

    @pytest.mark.usefixtures("skip_if_local_infinity")
    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_tensor_scan(self, check_data):
        self.test_infinity_obj._test_tensor_scan(check_data)

    @pytest.mark.usefixtures("skip_if_local_infinity")
    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_with_multiple_fusion(self, check_data):
        self.test_infinity_obj._test_with_multiple_fusion(check_data)
