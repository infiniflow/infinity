import pytest
from common import common_values
from internal.test_index import TestIndex
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
    request.cls.test_infinity_obj = TestIndex(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    def test_create_index_IVFFlat(self):
        self.test_infinity_obj._test_create_index_IVFFlat()

    def test_create_index_HNSW(self):
        self.test_infinity_obj._test_create_index_HNSW()

    def test_create_index_DiskAnn(self):
        self.test_infinity_obj._test_create_index_DiskAnn()

    @pytest.mark.parametrize("block_size", [8, 128])
    @pytest.mark.parametrize("compress_type", ["compress", "raww"])
    def test_create_index_BMP(self, block_size, compress_type):
        self.test_infinity_obj._test_create_index_BMP(block_size, compress_type)

    def test_create_index_fulltext(self):
        self.test_infinity_obj._test_create_index_fulltext()

    def test_create_index_secondary(self):
        self.test_infinity_obj._test_create_index_secondary()

    def test_create_index_emvb(self):
        self.test_infinity_obj._test_create_index_emvb()

    def test_drop_non_existent_index(self):
        self.test_infinity_obj._test_drop_non_existent_index()

    def test_create_created_index(self):
        self.test_infinity_obj._test_create_created_index()

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
        (index.IndexType.IVFFlat, True),
        (index.IndexType.DiskAnn, False)
    ])
    @pytest.mark.parametrize("params", [
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False), ((1, 2), False), ({"1": 2}, False),
        ([index.InitParameter("centroids_count", "128"),
          index.InitParameter("metric", "l2")], True)
    ])
    @pytest.mark.parametrize("types", ["vector, 3, float"])
    def test_create_drop_vector_index_invalid_options(self, column_name, index_type, params, types):
        self.test_infinity_obj._test_create_drop_vector_index_invalid_options(column_name, index_type,
                                                                         params, types)

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
    def test_create_drop_different_fulltext_index_invalid_options(self, column_name, index_type,
                                                                  params, types):
        self.test_infinity_obj._test_create_drop_different_fulltext_index_invalid_options(column_name,
                                                                                     index_type,
                                                                                     params, types)

    def test_create_index_on_dropped_table(self):
        self.test_infinity_obj._test_create_index_on_dropped_table()

    def test_create_index_show_index(self):
        self.test_infinity_obj._test_create_index_show_index()

    def test_drop_index_show_index(self):
        self.test_infinity_obj._test_drop_index_show_index()

    @pytest.mark.parametrize("types", ["vector, 3, float"])
    @pytest.mark.parametrize("index_type", [
        (index.IndexType.Hnsw, False, ErrorCode.INVALID_INDEX_PARAM),
        (index.IndexType.IVFFlat, True),
        (index.IndexType.FullText, False, ErrorCode.INVALID_INDEX_DEFINITION),
        (index.IndexType.DiskAnn, False, ErrorCode.INVALID_INDEX_PARAM)
    ])
    def test_create_index_on_different_type_of_column(self, types, index_type):
        self.test_infinity_obj._test_create_index_on_different_type_of_column(types, index_type)

    @pytest.mark.parametrize("index_type", [
        index.IndexType.IVFFlat
    ])
    def test_insert_data_create_index(self, index_type):
        self.test_infinity_obj._test_insert_data_create_index(index_type)

    @pytest.mark.parametrize("index_type", [
        (index.IndexType.IVFFlat, True),
        (index.IndexType.FullText, False, ErrorCode.INVALID_INDEX_DEFINITION)
    ])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_import_data_create_index(self, index_type, file_format):
        self.test_infinity_obj._test_import_data_create_index(index_type, file_format)

    @pytest.mark.parametrize("index_type", [index.IndexType.IVFFlat])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_create_vector_index_import_data(self, index_type, file_format):
        self.test_infinity_obj._test_create_vector_index_import_data(index_type, file_format)

    @pytest.mark.parametrize("index_type", [
        pytest.param(index.IndexType.FullText)
    ])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_create_index_import_data(self, index_type, file_format):
        self.test_infinity_obj._test_create_index_import_data(index_type, file_format)

    @pytest.mark.parametrize("file_format", ["csv"])
    def test_insert_data_fulltext_index_search(self, file_format):
        self.test_infinity_obj._test_insert_data_fulltext_index_search(file_format)

    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_9.csv", "data_dir": common_values.TEST_TMP_DIR, }],
                             indirect=True)
    def test_fulltext_match_with_invalid_analyzer(self, check_data):
        self.test_infinity_obj._test_fulltext_match_with_invalid_analyzer(check_data)

    def test_create_index_on_deleted_table(self):
        self.test_infinity_obj._test_create_index_on_deleted_table()

    @pytest.mark.skip
    @pytest.mark.xfail(reason="Not support to convert Embedding to Embedding")
    def test_create_index_on_update_table(self):
        self.test_infinity_obj._test_create_index_on_update_table()

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               ConflictType.Error,
                                               ConflictType.Replace,
                                               0,
                                               1,
                                               2,
                                               ])
    def test_create_index_with_valid_options(self, conflict_type):
        self.test_infinity_obj._test_create_index_with_valid_options(conflict_type)

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def test_create_index_with_invalid_options(self, conflict_type):
        self.test_infinity_obj._test_create_index_with_invalid_options(conflict_type)

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               0])
    def test_create_duplicated_index_with_valid_options(self, conflict_type):
        self.test_infinity_obj._test_create_duplicated_index_with_valid_options(conflict_type)

    @pytest.mark.parametrize("conflict_type", [pytest.param(ConflictType.Error),
                                               pytest.param(
                                                   ConflictType.Replace),
                                               pytest.param(1),
                                               pytest.param(2),
                                               ])
    def test_create_duplicated_index_with_valid_error_options(self, conflict_type):
        self.test_infinity_obj._test_create_duplicated_index_with_valid_error_options(conflict_type)

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def test_create_duplicated_index_with_invalid_options(self, conflict_type):
        self.test_infinity_obj._test_create_duplicated_index_with_invalid_options(conflict_type)

    def test_show_index(self):
        self.test_infinity_obj._test_show_index()

    @pytest.mark.parametrize("index_name", ["my_index"])
    def test_show_valid_name_index(self, index_name):
        self.test_infinity_obj._test_show_valid_name_index(index_name)

    @pytest.mark.parametrize("index_name", [pytest.param("Invalid name"),
                                            pytest.param("not_exist_name"),
                                            pytest.param(1),
                                            pytest.param(1.1),
                                            pytest.param(True),
                                            pytest.param([]),
                                            pytest.param(()),
                                            pytest.param({}),
                                            ])
    def test_show_invalid_name_index(self, index_name):
        self.test_infinity_obj._test_show_invalid_name_index(index_name)

    def test_list_index(self):
        self.test_infinity_obj._test_list_index()

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               ConflictType.Error,
                                               0,
                                               1])
    def test_drop_index_with_valid_options(self, conflict_type):
        self.test_infinity_obj._test_drop_index_with_valid_options(conflict_type)

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def test_drop_index_with_invalid_options(self, conflict_type):
        self.test_infinity_obj._test_drop_index_with_invalid_options(conflict_type)

    @pytest.mark.parametrize("index_distance_type",
                             ["l2", "ip"])
    def test_supported_vector_index(self, index_distance_type):
        self.test_infinity_obj._test_supported_vector_index(index_distance_type)

    @pytest.mark.parametrize("index_distance_type", ["cosine", "hamming"])
    def test_unsupported_vector_index(self, index_distance_type):
        self.test_infinity_obj._test_unsupported_vector_index(index_distance_type)


    def test_create_upper_name_index(self):
        self.test_infinity_obj._test_create_upper_name_index()

    @pytest.mark.parametrize("index_type", [
        index.IndexType.IVFFlat,
        index.IndexType.Hnsw,
        index.IndexType.BMP,
        index.IndexType.FullText,
        index.IndexType.EMVB,
        index.IndexType.Secondary,
    ])
    def test_create_index_with_converse_param_name(self, index_type):
        self.test_infinity_obj._test_create_index_with_converse_param_name(index_type)

    @pytest.mark.parametrize("index_type", [
        index.IndexType.IVFFlat,
        index.IndexType.Hnsw,
        index.IndexType.BMP,
        index.IndexType.FullText,
        index.IndexType.EMVB,
        index.IndexType.Secondary,
    ])
    def test_create_index_with_converse_param_value(self, index_type):
        self.test_infinity_obj._test_create_index_with_converse_param_value(index_type)

