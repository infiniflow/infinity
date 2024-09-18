import importlib
import sys
import os
import infinity
import infinity_embedded
import os
import time
import infinity.index as index
import pandas
import pytest
from common import common_values
from common import common_index
from infinity.common import ConflictType, InfinityException
from infinity.errors import ErrorCode
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http

TEST_DATA_DIR = "/test/data/"


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

    @pytest.mark.skip(reason="Disable IVF index")
    def test_create_index_IVFFlat(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_index_ivfflat" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("test_index_ivfflat" + suffix, {
            "c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.IVFFlat,
                                                     {"centroids_count": "128", "metric": "l2"}),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_index_ivfflat" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_index_HNSW(self, suffix):
        # CREATE INDEX idx1 ON test_hnsw (col1) USING Hnsw WITH (M = 16, ef_construction = 50, ef = 50, metric = l2);
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_index_hnsw" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_hnsw" + suffix, {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_index_hnsw" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("block_size", [8, 128])
    @pytest.mark.parametrize("compress_type", ["compress", "raww"])
    def test_create_index_BMP(self, block_size, compress_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_bmp" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_bmp" + suffix, {"col1": {"type": "int"}, "col2": {"type": "sparse,30000,float,int16"}},
            ConflictType.Error)
        assert table_obj is not None

        # CREATE INDEX idx1 ON test_bmp (col2) USING Bmp WITH (block_size = 16, compress_type = compress);
        res = table_obj.create_index("idx1",
                                     index.IndexInfo("col2",
                                                     index.IndexType.BMP,
                                                     {"block_size": str(block_size), "compress_type": compress_type}),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("idx1")
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_bmp" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_index_fulltext(self, suffix):
        # CREATE INDEX ft_index ON enwiki(body) USING FULLTEXT;
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_index_fulltext" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_fulltext" + suffix, {
                "doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}
            }, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("body",
                                                     index.IndexType.FullText),
                                     ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_index_fulltext" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_drop_index_fulltext(self, suffix):
        # CREATE INDEX ft_index ON enwiki(body) USING FULLTEXT;
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_index_fulltext_drop" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("test_index_fulltext_drop" + suffix,
                                        {"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"},
                                         "body": {"type": "varchar"}}, ConflictType.Error)
        assert table_obj is not None
        # fulltext search when index is not created: expect error
        with pytest.raises(InfinityException) as e:
            table_obj.output(["doctitle", "_score"]).match_text("body^5", "harmful chemical", 3).to_pl()
        print(e)
        res = table_obj.create_index("my_index", index.IndexInfo("body", index.IndexType.FullText), ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        # fulltext search when index is created: expect success
        res = table_obj.output(["doctitle", "_score"]).match_text("body^5", "harmful chemical", 3).to_pl()
        print(res)
        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK
        # fulltext search when index is dropped: expect error
        with pytest.raises(InfinityException) as e:
            table_obj.output(["doctitle", "_score"]).match_text("body^5", "harmful chemical", 3).to_pl()
        print(e)
        res = db_obj.drop_table("test_index_fulltext_drop" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_index_secondary(self, suffix):
        # CREATE INDEX idx_secondary ON t(c1);
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_index_secondary" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_secondary" + suffix, {
                "c1": {"type": "int"}, "body": {"type": "varchar"}
            }, ConflictType.Error)
        assert table_obj is not None
        res = table_obj.create_index("my_index_1",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Secondary),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = table_obj.create_index("my_index_2",
                                     index.IndexInfo("body",
                                                     index.IndexType.Secondary),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = table_obj.drop_index("my_index_1")
        assert res.error_code == ErrorCode.OK
        res = table_obj.drop_index("my_index_2")
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_index_secondary" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # drop non-existent index

    def test_create_index_emvb(self, suffix):
        # CREATE INDEX idx_emvb ON t(c2) USING EMVB;
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_index_emvb" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "test_index_emvb" + suffix, {
                "c1": {"type": "int"}, "c2": {"type": "tensor, 128, float"}
            }, ConflictType.Error)
        assert table_obj is not None
        res = table_obj.create_index("my_index_1",
                                     index.IndexInfo("c2",
                                                     index.IndexType.EMVB,
                                                     {"pq_subspace_num": "32", "pq_subspace_bits": "8"}),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = table_obj.drop_index("my_index_1")
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_index_emvb" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_drop_non_existent_index(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_drop_non_existent_index" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_drop_non_existent_index" + suffix, {
            "c1": {"type": "vector,3,float"}}, ConflictType.Error)
        assert table_obj is not None

        # drop none existent index
        with pytest.raises(InfinityException) as e:
            table_obj.drop_index("none_index")

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INDEX_NOT_EXIST

        res = db_obj.drop_table(
            "test_drop_non_existent_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_created_index(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_drop_non_existent_index" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_drop_non_existent_index" + suffix, {
            "c1": {"type": "vector,3,float"}}, ConflictType.Error)
        assert table_obj is not None

        # drop none existent index
        with pytest.raises(InfinityException) as e:
            table_obj.drop_index("none_index")

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INDEX_NOT_EXIST

        res = db_obj.drop_table(
            "test_drop_non_existent_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("column_name",
                             [(1, False), (2.2, False), ((1, 2), False), ([1, 2, 3], False), ("c1", True)])
    @pytest.mark.parametrize("index_type", [
        (1, False),
        (2.2, False),
        ([1, 2], False),
        ("$#%dfva", False),
        ((1, 2), False),
        ({"1": 2}, False),
        (common_index.IndexType.Hnsw, False),
        # (index.IndexType.IVFFlat, False)
    ])
    @pytest.mark.parametrize("params", [
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False), ((1, 2), False), ({"1": 2}, False),
        ({"centroids_count": "128", "metric": "l2"}, True)
    ])
    @pytest.mark.parametrize("types", ["vector, 3, float"])
    def test_create_drop_vector_index_invalid_options(self, column_name, index_type, params, types, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_create_drop_vector_index_invalid_options" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("test_create_drop_vector_index_invalid_options" + suffix,
                                        {"c1": {"type": types}},
                                        ConflictType.Error)

        if not column_name[1] or not index_type[1] or not params[1]:
            with pytest.raises(Exception):
                index_info = index.IndexInfo(column_name[0], index_type[0], params[0])
                table_obj.create_index(
                    "my_index", index_info, ConflictType.Error)
        else:
            index_info = index.IndexInfo(column_name[0], index_type[0], params[0])
            table_obj.create_index("my_index", index_info, ConflictType.Error)

        res = db_obj.drop_table(
            "test_create_drop_vector_index_invalid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("column_name", [
        (1, False),
        (2.2, False),
        ([1, 2], False),
        ("$#%dfva", False),
        ((1, 2), False),
        ({"1": 2}, False),
        ("c1", True)])
    @pytest.mark.parametrize("index_type", [(common_index.IndexType.FullText, True)])
    @pytest.mark.parametrize("params", [
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False),
        ((1, 2), False), ({"1": 2}, False), (None, True)
    ])
    @pytest.mark.parametrize("types", ["int", "int8", "int16", "int32", "int64", "integer",
                                       "float", "float32", "double", "float64",
                                       "varchar", "bool", "vector, 3, float"])
    def test_create_drop_different_fulltext_index_invalid_options(self, column_name, index_type,
                                                                  params, types, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_create_drop_different_fulltext_index_invalid_options" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_create_drop_different_fulltext_index_invalid_options" + suffix, {
            "c1": {"type": types}}, ConflictType.Error)

        if types != "varchar" or not column_name[1] or not index_type[1] or not params[1]:
            with pytest.raises(Exception):
                index_info = index.IndexInfo(column_name[0], index_type[0], params[0])
                table_obj.create_index("my_index", index_info, ConflictType.Error)
        else:
            index_info = index.IndexInfo(column_name[0], index_type[0], params[0])
            table_obj.create_index("my_index", index_info, ConflictType.Error)

        res = db_obj.drop_table(
            "test_create_drop_different_fulltext_index_invalid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_index_on_dropped_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_create_drop_index_invalid_options" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_create_drop_index_invalid_options" + suffix, {
            "c1": {"type": "vector,3,float"}}, ConflictType.Error)
        db_obj.drop_table(
            "test_create_drop_index_invalid_options" + suffix)

        # create created index
        with pytest.raises(InfinityException) as e:
            table_obj.create_index("my_index",
                                   index.IndexInfo("c1",
                                                   index.IndexType.Hnsw,
                                                   {
                                                       "m": "16",
                                                       "ef_construction": "200",
                                                       "metric": "l2",
                                                       "encode": "lvq"
                                                   }),
                                   ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

    def test_create_index_show_index(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_create_index_show_index" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_create_index_show_index" + suffix, {
            "c1": {"type": "vector,3,float"}}, ConflictType.Error)
        # create created index
        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "200",
                                                         "metric": "l2",
                                                         "encode": "lvq"
                                                     }),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = table_obj.show_index("my_index")
        print(res)
        res = db_obj.drop_table(
            "test_create_index_show_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_drop_index_show_index(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_drop_index_show_index" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_drop_index_show_index" + suffix, {
            "c1": {"type": "vector,3,float"}}, ConflictType.Error)
        # create created index
        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "200",
                                                         "metric": "l2",
                                                         "encode": "lvq"
                                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.show_index("my_index")
        assert res.error_code == ErrorCode.OK

        res = table_obj.list_indexes()
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_drop_index_show_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("types", ["vector, 3, float"])
    @pytest.mark.parametrize("index_type", [
        (common_index.IndexType.Hnsw, False, ErrorCode.INVALID_INDEX_PARAM),
        # (index.IndexType.IVFFlat, True),
        (common_index.IndexType.FullText, False, ErrorCode.INVALID_INDEX_DEFINITION)
    ])
    def test_create_index_on_different_type_of_column(self, types, index_type, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_create_index_on_different_type_of_column" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_create_index_on_different_type_of_column" + suffix,
                                        {"c1": {"type": types}},
                                        ConflictType.Error)

        # create created index
        if not index_type[1]:
            with pytest.raises(InfinityException) as e:
                table_obj.create_index("my_index",
                                       index.IndexInfo("c1",
                                                       index_type[0],
                                                       {"centroids_count": "128", "metric": "l2"}), ConflictType.Error)

            assert e.type == InfinityException
            assert e.value.args[0] == index_type[2]
        else:
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c1",
                                                         index_type[0],
                                                         {"centroids_count": "128", "metric": "l2"}),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_create_index_on_different_type_of_column" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_type", [
        common_index.IndexType.Hnsw
    ])
    def test_insert_data_create_index(self, index_type, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_data_create_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_data_create_index" + suffix, {
            "c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        values = [{"c1": [1.1 for _ in range(1024)]}]
        table_obj.insert(values)

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index_type,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "200",
                                                         "metric": "l2",
                                                         "encode": "lvq"
                                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_insert_data_create_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_type", [
        (common_index.IndexType.Hnsw, True),
        (common_index.IndexType.FullText, False, ErrorCode.INVALID_INDEX_DEFINITION)
    ])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_import_data_create_index(self, index_type, file_format, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_import_data_create_index" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_import_data_create_index" + suffix, {
            "c1": {"type": "int"},
            "c2": {"type": "vector,3,float"}}, ConflictType.Error)

        table_obj.import_data(os.getcwd() + TEST_DATA_DIR +
                              file_format + "/pysdk_test." + file_format)
        if (index_type[1]):
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c2",
                                                         index_type[0],
                                                         {
                                                             "m": "16",
                                                             "ef_construction": "200",
                                                             "metric": "l2",
                                                             "encode": "lvq"
                                                         }),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        else:
            with pytest.raises(InfinityException) as e:
                table_obj.create_index("my_index",
                                       index.IndexInfo("c2",
                                                       index_type[0],
                                                       {"centroids_count": "128", "metric": "l2"}), ConflictType.Error)

            assert e.type == InfinityException
            assert e.value.args[0] == index_type[2]

        res = db_obj.drop_table(
            "test_import_data_create_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_type", [common_index.IndexType.Hnsw])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_create_vector_index_import_data(self, index_type, file_format, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_create_vector_index_import_data" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_create_vector_index_import_data" + suffix, {
            "c1": {"type": "int"},
            "c2": {"type": "vector,3,float"}}, ConflictType.Error)
        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c2",
                                                     index_type,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "200",
                                                         "metric": "l2",
                                                         "encode": "lvq"
                                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        table_obj.import_data(os.getcwd() + TEST_DATA_DIR +
                              file_format + "/pysdk_test." + file_format)

        res = db_obj.drop_table(
            "test_create_vector_index_import_data" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_type", [
        pytest.param(common_index.IndexType.FullText)
    ])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_create_index_import_data(self, index_type, file_format, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_create_index_import_data" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("test_create_index_import_data" + suffix, {
            "c1": {"type": "int"},
            "c2": {"type": "vector,3,float"}}, ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c2",
                                                         index_type,
                                                         {"centroids_count": "128", "metric": "l2"}),
                                         ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_INDEX_DEFINITION

        assert res.error_code == ErrorCode.OK
        table_obj.import_data(os.getcwd() + TEST_DATA_DIR +
                              file_format + "/pysdk_test." + file_format)

        res = db_obj.drop_table(
            "test_create_index_import_data" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("file_format", ["csv"])
    def test_insert_data_fulltext_index_search(self, file_format, suffix):
        # prepare data for insert
        column_names = ["doctitle", "docdate", "body"]
        df = pandas.read_csv(os.getcwd() + TEST_DATA_DIR + file_format + "/enwiki_99." + file_format,
                             delimiter="\t",
                             header=None,
                             names=column_names)
        data = {key: list(value.values())
                for key, value in df.to_dict().items()}

        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_insert_data_fulltext_index_search" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_insert_data_fulltext_index_search" + suffix, {
            "doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}
        }, ConflictType.Error)
        res = table_obj.create_index("body_index",
                                     index.IndexInfo("body",
                                                     index.IndexType.FullText))
        assert res.error_code == ErrorCode.OK

        # Create 99*300/8192 = 3.6 BlockEntry to test MemIndexRecover and OptimizeIndex
        for it in range(300):
            value = []
            for i in range(len(data["doctitle"])):
                value.append({"doctitle": data["doctitle"][i],
                              "docdate": data["docdate"][i], "body": data["body"][i]})
            table_obj.insert(value)
        time.sleep(5)
        res = table_obj.output(["doctitle", "docdate", "_row_id", "_score"]).match_text(
            "body^5", "harmful chemical", 3).to_pl()
        assert not res.is_empty()
        print(res)

        res = db_obj.drop_table(
            "test_insert_data_fulltext_index_search" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "enwiki_9.csv", "data_dir": common_values.TEST_TMP_DIR, }],
                             indirect=True)
    def test_fulltext_match_with_invalid_analyzer(self, check_data, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_fulltext_match_with_invalid_analyzer" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_fulltext_match_with_invalid_analyzer" + suffix, {
            "doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}})
        assert res.error_code == ErrorCode.OK

        with pytest.raises(InfinityException) as e:
            table_obj.create_index("my_index",
                                   index.IndexInfo("body",
                                                   index.IndexType.FullText,
                                                   {"ANALYZER": "segmentation"}),
                                   ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.ANALYZER_NOT_FOUND

        res = db_obj.drop_table("test_fulltext_match_with_invalid_analyzer" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

    def test_create_index_on_deleted_table(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_create_index_on_deleted_table" + suffix, ConflictType.Ignore)

        table_obj = db_obj.create_table("test_create_index_on_deleted_table" + suffix,
                                        {"c1": {"type": "vector,128,float"}},
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
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "200",
                                                         "metric": "l2",
                                                         "encode": "lvq"
                                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_create_index_on_deleted_table" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip
    @pytest.mark.xfail(reason="Not support to convert Embedding to Embedding")
    def test_create_index_on_update_table(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_index_on_update_table" + suffix,
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_index_on_update_table" + suffix, {
            "c1": {"type": "vector,128,float"},
            "c2": {"type": "int"}
        }, ConflictType.Error)
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
            table_obj.update("c2 = " + str(i), {"c1": embedding_data})
        res = table_obj.output(["*"]).to_pl()
        print(res)
        res = db_obj.drop_table(
            "test_create_index_on_update_table" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               ConflictType.Error,
                                               ConflictType.Replace,
                                               0,
                                               1,
                                               2,
                                               ])
    def test_create_index_with_valid_options(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_create_index_with_valid_options" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_create_index_with_valid_options" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), conflict_type)

        assert res.error_code == ErrorCode.OK
        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_create_index_with_valid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def test_create_index_with_invalid_options(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_create_index_with_invalid_options" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_create_index_with_invalid_options" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        with pytest.raises(InfinityException) as e:
            table_obj.create_index("my_index",
                                   index.IndexInfo("c1",
                                                   index.IndexType.Hnsw,
                                                   {
                                                       "m": "16",
                                                       "ef_construction": "50",
                                                       "metric": "l2"
                                                   }), conflict_type)

        assert e.type == InfinityException
        # assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        res = db_obj.drop_table(
            "test_create_index_with_invalid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               0])
    def test_create_duplicated_index_with_valid_options(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_create_duplicated_index_with_valid_options" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_create_duplicated_index_with_valid_options" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None
        for i in range(10):
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c1",
                                                         index.IndexType.Hnsw,
                                                         {
                                                             "m": "16",
                                                             "ef_construction": "50",
                                                             "metric": "l2"
                                                         }), conflict_type)

            assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_create_duplicated_index_with_valid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [pytest.param(ConflictType.Error),
                                               pytest.param(
                                                   ConflictType.Replace),
                                               pytest.param(1),
                                               pytest.param(2),
                                               ])
    def test_create_duplicated_index_with_valid_error_options(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_create_duplicated_index_with_valid_error_options" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_create_duplicated_index_with_valid_error_options" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), conflict_type)
        assert res.error_code == ErrorCode.OK

        for i in range(10):
            with pytest.raises(InfinityException) as e:
                table_obj.create_index("my_index",
                                       index.IndexInfo("c1",
                                                       index.IndexType.Hnsw,
                                                       {
                                                           "m": "16",
                                                           "ef_construction": "50",
                                                           "metric": "l2"
                                                       }), conflict_type)

            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.DUPLICATE_INDEX_NAME

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_create_duplicated_index_with_valid_error_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def test_create_duplicated_index_with_invalid_options(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_create_duplicated_index_with_invalid_options" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_create_duplicated_index_with_invalid_options" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        table_obj.drop_index("my_index", ConflictType.Ignore)
        with pytest.raises(InfinityException) as e:
            table_obj.create_index("my_index",
                                   index.IndexInfo("c1",
                                                   index.IndexType.Hnsw,
                                                   {
                                                       "m": "16",
                                                       "ef_construction": "50",
                                                       "metric": "l2"
                                                   }), conflict_type)

        assert e.type == InfinityException
        # assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        for i in range(10):
            with pytest.raises(InfinityException) as e:
                table_obj.create_index("my_index",
                                       index.IndexInfo("c1",
                                                       index.IndexType.Hnsw,
                                                       {
                                                           "m": "16",
                                                           "ef_construction": "50",
                                                           "metric": "l2"
                                                       }), conflict_type)

            assert e.type == InfinityException
            # assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        res = db_obj.drop_table(
            "test_create_duplicated_index_with_invalid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_show_index(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_show_index" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None
        for i in range(10):
            res = table_obj.create_index("my_index_" + str(i),
                                         index.IndexInfo("c1",
                                                         index.IndexType.Hnsw,
                                                         {
                                                             "m": "16",
                                                             "ef_construction": "50",
                                                             "metric": "l2"
                                                         }), ConflictType.Error)
            assert res.error_code == ErrorCode.OK
            res = table_obj.show_index("my_index_" + str(i))
            print(res)

        for i in range(10):
            res = table_obj.drop_index(
                "my_index_" + str(i), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_show_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_name", ["my_index"])
    def test_show_valid_name_index(self, index_name, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_various_name_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_show_various_name_index" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None
        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = table_obj.show_index(index_name)
        print(res)

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_show_various_name_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_name", [pytest.param("Invalid name"),
                                            pytest.param("not_exist_name"),
                                            pytest.param(1),
                                            pytest.param(1.1),
                                            pytest.param(True),
                                            pytest.param([]),
                                            pytest.param(()),
                                            pytest.param({}),
                                            ])
    def test_show_invalid_name_index(self, index_name, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_invalid_name_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_show_invalid_name_index" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        with pytest.raises(Exception):
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c1",
                                                         index.IndexType.Hnsw,
                                                         {
                                                             "m": "16",
                                                             "ef_construction": "50",
                                                             "metric": "l2"
                                                         }), ConflictType.Error)
            assert res.error_code == ErrorCode.OK
            res = table_obj.show_index(index_name)
            print(res)

        res = db_obj.drop_table(
            "test_show_invalid_name_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_list_index(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_list_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_list_index" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None
        for i in range(10):
            res = table_obj.create_index("my_index_" + str(i),
                                         index.IndexInfo("c1",
                                                         index.IndexType.Hnsw,
                                                         {
                                                             "m": "16",
                                                             "ef_construction": "50",
                                                             "metric": "l2"
                                                         }), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = table_obj.list_indexes()
        print(vars(res))

        for i in range(10):
            res = table_obj.drop_index(
                "my_index_" + str(i), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_list_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [ConflictType.Ignore,
                                               ConflictType.Error,
                                               0,
                                               1])
    def test_drop_index_with_valid_options(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_drop_index_with_valid_options" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_drop_index_with_valid_options" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", conflict_type)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_drop_index_with_valid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def test_drop_index_with_invalid_options(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_drop_index_with_invalid_options" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_drop_index_with_invalid_options" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        with pytest.raises(InfinityException) as e:
            table_obj.drop_index("my_index", conflict_type)

        assert e.type == InfinityException
        # assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        res = db_obj.drop_table(
            "test_drop_index_with_invalid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_distance_type",
                             ["l2", "ip"])
    def test_supported_vector_index(self, index_distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_supported_vector_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_supported_vector_index" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)

        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", ConflictType.Ignore)

        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_supported_vector_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_distance_type", ["hamming"])
    def test_unsupported_vector_index(self, index_distance_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_unsupported_vector_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_unsupported_vector_index" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        with pytest.raises(InfinityException) as e:
            table_obj.create_index("my_index",
                                   index.IndexInfo("c1",
                                                   index.IndexType.Hnsw,
                                                   {
                                                       "m": "16",
                                                       "ef_construction": "50",
                                                       "metric": index_distance_type
                                                   }), ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_INDEX_PARAM

        res = table_obj.drop_index("my_index", ConflictType.Ignore)

        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_unsupported_vector_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_upper_name_index(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_upper_name_index" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("test_upper_name_index" + suffix, {
            "c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        upper_name_index = "MY_INDEX"
        lower_name_index = "my_index"
        res = table_obj.create_index(upper_name_index,
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "200",
                                                         "metric": "l2",
                                                         "encode": "lvq"
                                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.show_index(lower_name_index)
        assert res.error_code == ErrorCode.OK
        res = table_obj.show_index(upper_name_index)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index(lower_name_index, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_upper_name_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_type", [
        # index.IndexType.IVFFlat,
        common_index.IndexType.Hnsw,
        common_index.IndexType.BMP,
        common_index.IndexType.FullText,
        common_index.IndexType.EMVB,
        common_index.IndexType.Secondary,
    ])
    def test_create_index_with_converse_param_name(self, index_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_index" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        if index_type == common_index.IndexType.IVFFlat:
            table_obj = db_obj.create_table("test_index" + suffix, {
                "c1": {"type": "vector,1024,float"}}, ConflictType.Error)
            assert table_obj is not None

            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c1",
                                                         index.IndexType.IVFFlat,
                                                         {"CENTROIDS_COUNT": "128", "METRIC": "l2"}),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.Hnsw:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
            assert table_obj is not None

            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c1",
                                                         index.IndexType.Hnsw,
                                                         {
                                                             "m": "16",
                                                             "EF_CONSTRUCTION": "50",
                                                             "METRIC": "l2"
                                                         }), ConflictType.Error)

            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.BMP:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {"col1": {"type": "int"}, "col2": {"type": "sparse,30000,float,int16"}},
                ConflictType.Error)
            assert table_obj is not None

            # CREATE INDEX idx1 ON test_bmp (col2) USING Bmp WITH (block_size = 16, compress_type = compress);
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("col2",
                                                         index.IndexType.BMP,
                                                         {"BLOCK_SIZE": "8", "COMPRESS_TYPE": "compress"}),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.FullText:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {
                    "doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}
                }, ConflictType.Error)
            assert table_obj is not None

            res = table_obj.create_index("my_index",
                                         index.IndexInfo("body",
                                                         index.IndexType.FullText,
                                                         {"analyzer": "standard"}),
                                         ConflictType.Error)

            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.EMVB:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {
                    "c1": {"type": "int"}, "c2": {"type": "tensor, 128, float"}
                }, ConflictType.Error)
            assert table_obj is not None
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c2",
                                                         index.IndexType.EMVB,
                                                         {"PQ_SUBSPACE_NUM": "32", "PQ_SUBSPACE_BITS": "8"}),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.Secondary:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {
                    "c1": {"type": "int"}, "body": {"type": "varchar"}
                }, ConflictType.Error)
            assert table_obj is not None
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c1",
                                                         index.IndexType.Secondary),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = table_obj.show_index("my_index")
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_type", [
        # index.IndexType.IVFFlat,
        common_index.IndexType.Hnsw,
        common_index.IndexType.BMP,
        common_index.IndexType.FullText,
        common_index.IndexType.EMVB,
        common_index.IndexType.Secondary,
    ])
    def test_create_index_with_converse_param_value(self, index_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_index" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        if index_type == common_index.IndexType.IVFFlat:
            table_obj = db_obj.create_table("test_index" + suffix, {
                "c1": {"type": "vector,1024,float"}}, ConflictType.Error)
            assert table_obj is not None

            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c1",
                                                         index.IndexType.IVFFlat,
                                                         {"centroids_count": "128", "metric": "l2"}),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.Hnsw:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
            assert table_obj is not None

            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c1",
                                                         index.IndexType.Hnsw,
                                                         {
                                                             "M": "16",
                                                             "ef_construction": "50",
                                                             "metric": "L2"
                                                         }), ConflictType.Error)

            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.BMP:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {"col1": {"type": "int"}, "col2": {"type": "sparse,30000,float,int16"}},
                ConflictType.Error)
            assert table_obj is not None

            # CREATE INDEX idx1 ON test_bmp (col2) USING Bmp WITH (block_size = 16, compress_type = compress);
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("col2",
                                                         index.IndexType.BMP,
                                                         {"block_size": "8", "compress_type": "COMPRESS"}),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.FullText:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {
                    "doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}
                }, ConflictType.Error)
            assert table_obj is not None

            res = table_obj.create_index("my_index",
                                         index.IndexInfo("body",
                                                         index.IndexType.FullText,
                                                         {"ANALYZER": "STANDARD"}),
                                         ConflictType.Error)

            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.EMVB:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {
                    "c1": {"type": "int"}, "c2": {"type": "tensor, 128, float"}
                }, ConflictType.Error)
            assert table_obj is not None
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c2",
                                                         index.IndexType.EMVB,
                                                         {"pq_subspace_num": "32", "pq_subspace_bits": "8"}),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK
        elif index_type == common_index.IndexType.Secondary:
            table_obj = db_obj.create_table(
                "test_index" + suffix, {
                    "c1": {"type": "int"}, "body": {"type": "varchar"}
                }, ConflictType.Error)
            assert table_obj is not None
            res = table_obj.create_index("my_index",
                                         index.IndexInfo("c1",
                                                         index.IndexType.Secondary),
                                         ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = table_obj.show_index("my_index")
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
