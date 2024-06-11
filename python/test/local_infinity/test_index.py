import pytest
from common import common_values
from src.test_index import TestIndex
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
from infinity.errors import ErrorCode
import infinity.index as index


class TestLocalInfinity():
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_IVFFlat(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_IVFFlat(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_HNSW(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_HNSW(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_fulltext(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_fulltext(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_drop_non_existent_index(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_drop_non_existent_index(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_created_index(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_created_index(get_infinity_db)

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
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_drop_vector_index_invalid_options(self, get_infinity_db, column_name, index_type, params, types):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_drop_vector_index_invalid_options(get_infinity_db, column_name, index_type,
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
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_drop_different_fulltext_index_invalid_options(self, get_infinity_db, column_name, index_type,
                                                                  params, types):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_drop_different_fulltext_index_invalid_options(get_infinity_db, column_name,
                                                                                     index_type,
                                                                                     params, types)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_on_dropped_table(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_on_dropped_table(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_show_index(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_show_index(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_drop_index_show_index(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_drop_index_show_index(get_infinity_db)

    @pytest.mark.parametrize("types", ["vector, 3, float"])
    @pytest.mark.parametrize("index_type", [
        (index.IndexType.Hnsw, False, ErrorCode.INVALID_INDEX_PARAM),
        (index.IndexType.IVFFlat, True),
        (index.IndexType.FullText, False, ErrorCode.INVALID_INDEX_DEFINITION)
    ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_on_different_type_of_column(self, get_infinity_db, types, index_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_on_different_type_of_column(get_infinity_db, types, index_type)

    @pytest.mark.parametrize("index_type", [
        index.IndexType.IVFFlat
    ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_insert_data_create_index(self, get_infinity_db, index_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_insert_data_create_index(get_infinity_db, index_type)

    @pytest.mark.parametrize("index_type", [
        (index.IndexType.IVFFlat, True),
        (index.IndexType.FullText, False, ErrorCode.INVALID_INDEX_DEFINITION)
    ])
    @pytest.mark.parametrize("file_format", ["csv"])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_import_data_create_index(self, get_infinity_db, index_type, file_format):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_import_data_create_index(get_infinity_db, index_type, file_format)

    @pytest.mark.parametrize("index_type", [index.IndexType.IVFFlat])
    @pytest.mark.parametrize("file_format", ["csv"])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_vector_index_import_data(self, get_infinity_db, index_type, file_format):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_vector_index_import_data(get_infinity_db, index_type, file_format)

    @pytest.mark.parametrize("index_type", [
        pytest.param(index.IndexType.FullText)
    ])
    @pytest.mark.parametrize("file_format", ["csv"])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_import_data(self, get_infinity_db, index_type, file_format):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_import_data(get_infinity_db, index_type, file_format)

    @pytest.mark.parametrize("file_format", ["csv"])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_insert_data_fulltext_index_search(self, get_infinity_db, file_format):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_insert_data_fulltext_index_search(get_infinity_db, file_format)

    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_9.csv", "data_dir": common_values.TEST_TMP_DIR, }],
                             indirect=True)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_fulltext_match_with_invalid_analyzer(self, get_infinity_db, check_data):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_fulltext_match_with_invalid_analyzer(get_infinity_db, check_data)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_on_deleted_table(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_on_deleted_table(get_infinity_db)

    @pytest.mark.skip
    @pytest.mark.xfail(reason="Not support to convert Embedding to Embedding")
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_on_update_table(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_on_update_table(get_infinity_db)

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               ConflictType.Error,
                                               ConflictType.Replace,
                                               0,
                                               1,
                                               2,
                                               ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_with_valid_options(self, get_infinity_db, conflict_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_with_valid_options(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_index_with_invalid_options(self, get_infinity_db, conflict_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_index_with_invalid_options(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               0])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_duplicated_index_with_valid_options(self, get_infinity_db, conflict_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_duplicated_index_with_valid_options(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("conflict_type", [pytest.param(ConflictType.Error),
                                               pytest.param(
                                                   ConflictType.Replace),
                                               pytest.param(1),
                                               pytest.param(2),
                                               ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_duplicated_index_with_valid_error_options(self, get_infinity_db, conflict_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_duplicated_index_with_valid_error_options(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_create_duplicated_index_with_invalid_options(self, get_infinity_db, conflict_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_create_duplicated_index_with_invalid_options(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_show_index(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_show_index(get_infinity_db)

    @pytest.mark.parametrize("index_name", ["my_index"])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_show_valid_name_index(self, get_infinity_db, index_name):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_show_valid_name_index(get_infinity_db, index_name)

    @pytest.mark.parametrize("index_name", [pytest.param("Invalid name"),
                                            pytest.param("not_exist_name"),
                                            pytest.param(1),
                                            pytest.param(1.1),
                                            pytest.param(True),
                                            pytest.param([]),
                                            pytest.param(()),
                                            pytest.param({}),
                                            ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_show_invalid_name_index(self, get_infinity_db, index_name):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_show_invalid_name_index(get_infinity_db, index_name)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_list_index(self, get_infinity_db):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_list_index(get_infinity_db)

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               ConflictType.Error,
                                               0,
                                               1])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_drop_index_with_valid_options(self, get_infinity_db, conflict_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_drop_index_with_valid_options(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_drop_index_with_invalid_options(self, get_infinity_db, conflict_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_drop_index_with_invalid_options(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("index_distance_type",
                             ["l2", "ip"])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_supported_vector_index(self, get_infinity_db, index_distance_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_supported_vector_index(get_infinity_db, index_distance_type)

    @pytest.mark.parametrize("index_distance_type", ["cosine", "hamming"])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_unsupported_vector_index(self, get_infinity_db, index_distance_type):
        test_infinity_obj = TestIndex(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_unsupported_vector_index(get_infinity_db, index_distance_type)
