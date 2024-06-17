import pytest
from common import common_values
from src.test_knn import TestKnn
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
from infinity.errors import ErrorCode
import infinity.index as index


@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, request, local_infinity):
        if 'skip_setup' in request.keywords:
            yield
        else:
            if local_infinity:
                self.uri = common_values.TEST_LOCAL_PATH
            else:
                self.uri = common_values.TEST_REMOTE_HOST
            self.test_infinity_obj = TestKnn(self.uri)
            yield
            self.teardown()

    def teardown(self):
        if hasattr(self, 'test_infinity_obj'):
            self.test_infinity_obj.disconnect()

    @pytest.fixture
    def skip_setup_marker(self, request):
        request.node.skip_setup = True

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
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_knn_on_vector_column(self, get_infinity_db, skip_setup_marker, check_data, column_name):
        self.test_infinity_obj._test_knn_on_vector_column(get_infinity_db, check_data, column_name)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("column_name", [pytest.param("variant_id"),
                                             pytest.param("query_price"),
                                             # pytest.param(1, marks=pytest.mark.xfail),
                                             # pytest.param(2.2, marks=pytest.mark.xfail),
                                             # pytest.param("!@#/\#$ ## #$%  @#$^", marks=pytest.mark.xfail),
                                             ])
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_knn_on_non_vector_column(self, get_infinity_db, skip_setup_marker, check_data, column_name):
        self.test_infinity_obj._test_knn_on_non_vector_column(get_infinity_db, check_data, column_name)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        (1, 2, 3, 4),
    ])
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_valid_embedding_data(self, get_infinity_db, skip_setup_marker, check_data, embedding_data):
        self.test_infinity_obj._test_valid_embedding_data(get_infinity_db, check_data, embedding_data)

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
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_invalid_embedding_data(self, get_infinity_db, skip_setup_marker, check_data, embedding_data):
        self.test_infinity_obj._test_invalid_embedding_data(get_infinity_db, check_data, embedding_data)

    @pytest.mark.parametrize("check_data", [{"file_name": "tmp_20240116.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("embedding_data", [
        [1] * 4,
        [1.0] * 4,
    ])
    @pytest.mark.parametrize("embedding_data_type", [
        ("float", True),
    ])
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_valid_embedding_data_type(self, get_infinity_db, skip_setup_marker, check_data, embedding_data, embedding_data_type):
        self.test_infinity_obj._test_valid_embedding_data_type(get_infinity_db, check_data, embedding_data,
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
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_invalid_embedding_data_type(self, get_infinity_db, skip_setup_marker, check_data, embedding_data, embedding_data_type):
        self.test_infinity_obj._test_invalid_embedding_data_type(get_infinity_db, check_data, embedding_data,
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
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_various_distance_type(self, get_infinity_db, skip_setup_marker, check_data, embedding_data, embedding_data_type,
                                   distance_type):
        self.test_infinity_obj._test_various_distance_type(get_infinity_db, check_data, embedding_data, embedding_data_type,
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
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_various_topn(self, get_infinity_db, skip_setup_marker, check_data, topn):
        self.test_infinity_obj._test_various_topn(get_infinity_db, check_data, topn)

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
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_with_index_before(self, get_infinity_db, skip_setup_marker, check_data, index_column_name, knn_column_name,
                               index_distance_type, knn_distance_type):
        self.test_infinity_obj._test_with_index_before(get_infinity_db, check_data, index_column_name, knn_column_name,
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
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_with_index_after(self, get_infinity_db, skip_setup_marker, check_data,
                              index_column_name, knn_column_name,
                              index_distance_type, knn_distance_type):
        self.test_infinity_obj._test_with_index_after(get_infinity_db, check_data, index_column_name, knn_column_name,
                                                 index_distance_type, knn_distance_type)

    @pytest.mark.parametrize("match_param_1", ["doctitle,num,body^5"])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_with_fulltext_match_with_valid_columns(self, get_infinity_db, skip_setup_marker, check_data, match_param_1):
        self.test_infinity_obj._test_with_fulltext_match_with_valid_columns(get_infinity_db, check_data, match_param_1)

    @pytest.mark.parametrize("match_param_1", [pytest.param(1),
                                               pytest.param(1.1),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               pytest.param("invalid column name")])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_with_fulltext_match_with_invalid_columns(self, get_infinity_db, skip_setup_marker, check_data, match_param_1):
        self.test_infinity_obj._test_with_fulltext_match_with_invalid_columns(get_infinity_db, check_data, match_param_1)

    @pytest.mark.parametrize("match_param_2", ["a word a segment",
                                               "body=Greek"])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_with_fulltext_match_with_valid_words(self, get_infinity_db, skip_setup_marker, check_data, match_param_2):
        self.test_infinity_obj._test_with_fulltext_match_with_valid_words(get_infinity_db, check_data, match_param_2)

    @pytest.mark.parametrize("match_param_2", [pytest.param(1),
                                               pytest.param(1.1),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               pytest.param("@#$!#@$SDasdf3!@#$")])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_with_fulltext_match_with_invalid_words(self, get_infinity_db, skip_setup_marker, check_data, match_param_2):
        self.test_infinity_obj._test_with_fulltext_match_with_invalid_words(get_infinity_db, check_data, match_param_2)

    @pytest.mark.parametrize("match_param_3", [pytest.param("@#$!#@$SDa^sdf3!@#$"),
                                               "topn=1",
                                               "1"])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_with_fulltext_match_with_options(self, get_infinity_db, skip_setup_marker, check_data, match_param_3):
        self.test_infinity_obj._test_with_fulltext_match_with_options(get_infinity_db, check_data, match_param_3)

    @pytest.mark.parametrize("match_param_3", [pytest.param(1),
                                               pytest.param(1.1),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()), ])
    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_embedding_99_commas.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_with_fulltext_match_with_invalid_options(self, get_infinity_db, skip_setup_marker, check_data, match_param_3):
        self.test_infinity_obj._test_with_fulltext_match_with_invalid_options(get_infinity_db, check_data, match_param_3)

    @pytest.mark.usefixtures("skip_if_local_infinity")
    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_tensor_scan(self, get_infinity_db, skip_setup_marker, check_data):
        self.test_infinity_obj._test_tensor_scan(get_infinity_db, check_data)

    @pytest.mark.usefixtures("skip_if_local_infinity")
    @pytest.mark.parametrize("check_data", [{"file_name": "tensor_maxsim.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_with_multiple_fusion(self, get_infinity_db, skip_setup_marker, check_data):
        self.test_infinity_obj._test_with_multiple_fusion(get_infinity_db, check_data)
