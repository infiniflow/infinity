import importlib
import sys
import os
import pandas as pd
import pytest
import numpy as np
from numpy import dtype
from common import common_values
import infinity
import infinity_embedded
import infinity.index as index
from infinity.common import ConflictType, InfinityException, SparseVector
from infinity.errors import ErrorCode
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
    def _test_insert_basic(self, suffix):
        """
        target: test table insert apis
        method:
        1. create tables
            - 'table_2'
                - c1 int primary key
                - c2 int null
        2. insert
            - insert into table_2 (c1, c2) values(1, 2)     √
            - insert into table_2 (c2, c1) values(1, 2)     √
            - insert into table_2 (c1) values(3)            √
        3. select all
            - 1, 2
            - 2, 1
            - 3, null
        4. drop tables
            - 'table_2'
        expect: all operations successfully
        """
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table(table_name="table_2"+suffix,
                          conflict_type=ConflictType.Ignore)
        # infinity
        table_obj = db_obj.create_table(
            "table_2"+suffix, {
                "c1": {"type": "int", "constraints": ["primary key", "not null"]},
                "c2": {"type": "int", "constraints": ["not null"]}
            },
            ConflictType.Error)
        assert table_obj is not None

        res = table_obj.insert([{"c1": 0, "c2": 0}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert([{"c1": 1, "c2": 1}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert({"c2": 2, "c1": 2})
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert([{"c2": 3, "c1": 3}, {"c1": 4, "c2": 4}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (0, 1, 2, 3, 4), 'c2': (0, 1, 2, 3, 4)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = db_obj.drop_table("table_2"+suffix)
        assert res.error_code == ErrorCode.OK

    def _test_insert_bool(self, suffix):
        """
        target: test insert bool column
        method: create table with bool column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("python_test_bool_insert"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("python_test_bool_insert"+suffix, {"c1": {"type": "float"}, "c2": {"type": "bool"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": -1, "c2": True}, {"c1": 2, "c2": False}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-1, 2), 'c2': (True, False)}).astype(
            {'c1': dtype('float32'), 'c2': dtype('bool')}))
        res = db_obj.drop_table("python_test_bool_insert"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        db_obj.drop_table("python_test_bool_insert_default"+suffix, ConflictType.Ignore)
        table_instance = db_obj.create_table("python_test_bool_insert_default"+suffix, {
            "c1": {"type": "int8", "default": 0},
            "c2": {"type": "int16", "default": 0},
            "c3": {"type": "int", "default": 0},
            "c4": {"type": "int32", "default": 0},  # Same as int
            "c5": {"type": "integer", "default": 0},  # Same as int
            "c6": {"type": "int64", "default": 0},
            "c7": {"type": "varchar"},
            "c8": {"type": "float", "default": 1.0},
            "c9": {"type": "float32", "default": 1.0},  # Same as float
            "c10": {"type": "double", "default": 1.0},
            "c11": {"type": "float64", "default": 1.0},  # Same as double
            "c12": {"type": "bool", "default": False}
        })
        assert table_instance
        res = table_instance.insert({"c1": 1, "c7": "Tom"})
        assert res.error_code == ErrorCode.OK
        res = table_instance.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (1,), 'c2': (0,), 'c3': (0,), 'c4': (0,), 'c5': (0,), 'c6': (0,), 'c7': ("Tom",), 'c8': (1.0,),
             'c9': (1.0,), 'c10': (1.0,), 'c11': (1.0,), 'c12': (False,)}).astype(
            {'c1': dtype('int8'), 'c2': dtype('int16'), 'c3': dtype('int32'), 'c4': dtype('int32'),
             'c5': dtype('int32'), 'c6': dtype('int64'), 'c7': dtype('object'), 'c8': dtype('float32'),
             'c9': dtype('float32'), 'c10': dtype('float64'), 'c11': dtype('float64'), 'c12': dtype('bool')}))
        res = db_obj.drop_table("python_test_bool_insert_default"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_float16_bfloat16(self, suffix):
        """
        target: test insert float16 bfloat16 column
        method: create table with float16 bfloat16 column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("python_test_fp16_bf16"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("python_test_fp16_bf16"+suffix, {"c1": {"type": "float"}, "c2": {"type": "float16"},
                                                                  "c3": {"type": "bfloat16"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert(
            [{"c1": -1, "c2": 1, "c3": -1}, {"c1": 2, "c2": -2, "c3": 2}, {"c1": -3, "c2": 3, "c3": -3},
             {"c1": 4, "c2": -4, "c3": 4}, {"c1": -5, "c2": 5, "c3": -5}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (-1, 2, -3, 4, -5), 'c2': (1, -2, 3, -4, 5), 'c3': (-1, 2, -3, 4, -5)}).astype(
            {'c1': dtype('float32'), 'c2': dtype('float32'), 'c3': dtype('float32')}))
        res = db_obj.drop_table("python_test_fp16_bf16"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_varchar(self, suffix):
        """
        target: test insert varchar column
        method: create table with varchar column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_varchar"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_varchar"+suffix, {"c1": {"type": "varchar"}}, ConflictType.Error)
        assert table_obj

        res = table_obj.insert([{"c1": "test_insert_varchar"}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": " test insert varchar "}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": "^789$ test insert varchar"}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ("test_insert_varchar", " test insert varchar ",
                                                                "^789$ test insert varchar")}))
        res = db_obj.drop_table("test_insert_varchar"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_big_varchar(self, suffix):
        """
        target: test insert varchar with big length
        method: create table with varchar column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_big_varchar"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_varchar"+suffix, {"c1": {"type": "varchar"}}, ConflictType.Error)
        assert table_obj
        for i in range(100):
            res = table_obj.insert([{"c1": "test_insert_big_varchar" * 1000}])
            assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ["test_insert_big_varchar" * 1000] * 100}))

        res = db_obj.drop_table("test_insert_big_varchar"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_embedding(self, suffix):
        """
        target: test insert embedding column
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_embedding"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_embedding"+suffix, {"c1": {"type": "vector,3,int"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [4, 5, 6]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [7, 8, 9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9]}, {"c1": [-7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9],
                                                                [1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))

        res = db_obj.drop_table("test_insert_embedding"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        embedding_insert_float = [[1.1, 2.2, 3.3], [4.4, 5.5, 6.6], [7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]]
        db_obj.drop_table("test_insert_embedding_2"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_embedding_2"+suffix, {"c1": {"type": "vector,3,float"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_embedding_2"+suffix)
        assert table_obj
        res = table_obj.insert([{"c1": embedding_insert_float[0]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[1]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[2]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[3]}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': embedding_insert_float}))

        res = db_obj.drop_table("test_insert_embedding_2"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_embedding_3"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_embedding_3"+suffix, {"c1": {"type": "vector,3,float16"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_embedding_3"+suffix)
        assert table_obj
        res = table_obj.insert([{"c1": embedding_insert_float[0]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[1]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[2]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': [np.array(x).astype(np.float16).tolist() for x in embedding_insert_float]}))
        res = db_obj.drop_table("test_insert_embedding_3"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_embedding_4"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_embedding_4"+suffix, {"c1": {"type": "vector,3,bfloat16"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_embedding_4"+suffix)
        assert table_obj
        res = table_obj.insert([{"c1": embedding_insert_float[0]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[1]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[2]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        print(res)
        tmp_bf16 = np.array(embedding_insert_float).astype('<f4')
        tmp_bf16.view('<i2')[:, ::2] = 0
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': tmp_bf16.tolist()}))
        res = db_obj.drop_table("test_insert_embedding_4"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_big_embedding(self, suffix):
        """
        target: test insert embedding with big dimension
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_big_embedding"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_embedding"+suffix, {"c1": {"type": "vector,16384,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [4] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [7] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-9999999] * 16384}])
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_big_embedding"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_big_embedding_float(self, suffix):
        """
        target: test insert embedding float with big dimension
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_big_embedding_float"+suffix,
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_embedding_float"+suffix, {"c1": {"type": "vector,16384,float"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-9999999] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [1.1] * 16384}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-9999999.988] * 16384}])
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_big_embedding_float"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_exceed_block_size(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_exceed_block_size"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_exceed_block_size"+suffix, {
            "c1": {"type": "float"}}, ConflictType.Error)
        assert table_obj
        values = [{"c1": 1} for _ in range(8193)]

        with pytest.raises(InfinityException) as exception:
            table_obj.insert(values)
            assert exception.type == InfinityException
            assert exception.value.args[0] == "Insert batch row limit shouldn\'t more than 8193"

        res = db_obj.drop_table("test_insert_exceed_block_size"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_data_into_non_existent_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_data_into_non_existent_table"+suffix, ConflictType.Ignore)

        # create and drop table
        table_obj = db_obj.create_table("test_insert_data_into_non_existent_table"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)
        res = db_obj.drop_table(
            "test_insert_data_into_non_existent_table"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # insert
        values = [{"c1": 1, "c2": 1}]
        # check whether throw exception TABLE_NOT_EXIST
        with pytest.raises(InfinityException) as e:
            table_obj.insert(values)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

    def _test_insert_data_into_index_created_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_data_into_index_created_table"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_data_into_index_created_table"+suffix,
                                        {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)

        # create index
        table_obj.create_index("my_index_1",
                               index.IndexInfo("c1",
                                               index.IndexType.Hnsw,
                                               {
                                                   "M": "16",
                                                   "ef_construction": "50",
                                                   "metric": "l2"
                                               }), ConflictType.Error)

        # table_obj.create_index("my_index_2",
        #                        index.IndexInfo("c1",
        #                                        index.IndexType.IVFFlat,
        #                                        [index.InitParameter("centroids_count", "128"),
        #                                         index.InitParameter("metric", "l2")]), ConflictType.Error)

        # insert
        values = [{"c1": [1.1 for _ in range(1024)]}]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_pl()
        print(insert_res)

        res = table_obj.drop_index("my_index_1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # res = table_obj.drop_index("my_index_2", ConflictType.Error)
        # assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_data_into_index_created_table"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_table_with_10000_columns(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_table_with_10000_columns"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_table_with_10000_columns"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        for i in range(100):
            values = [{"c1": i * 100 + j, "c2": i * 100 + j + 1}
                      for j in range(100)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

    def _test_read_after_shutdown(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("test_insert_table_with_10000_columns"+suffix)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_table_with_10000_columns"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_batch_insert(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_batch_insert"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_batch_insert"+suffix, {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8192)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table("test_batch_insert"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_zero_column(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_zero_column"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_zero_column"+suffix, {
            "c1": {"type": "int"}}, ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            table_obj.insert([])
            insert_res = table_obj.output(["*"]).to_df()
            print(insert_res)

        assert e.type == InfinityException
        # assert e.value.args[0] == ErrorCode.INSERT_WITHOUT_VALUES

        res = db_obj.drop_table("test_insert_zero_column"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_sparse(self, suffix):
        """
        target: test insert sparse column
        method: create table with sparse column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_sparse"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_sparse"+suffix, {"c1": {"type": "sparse,100,float,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": SparseVector(**{"indices": [10, 20, 30], "values": [1.1, 2.2, 3.3]})}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": SparseVector([40, 50, 60], [4.4, 5.5, 6.6])}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": {"70": 7.7, "80": 8.8, "90": 9.9}},
                                {"c1": {"70": -7.7, "80": -8.8, "90": -9.9}}])
        assert res.error_code == ErrorCode.OK
        print(table_obj.output(["*"]).to_pl())
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (
                {"10": 1.1, "20": 2.2, "30": 3.3},
                {"40": 4.4, "50": 5.5, "60": 6.6},
                {"70": 7.7, "80": 8.8, "90": 9.9},
                {"70": -7.7, "80": -8.8, "90": -9.9})}))

        res = db_obj.drop_table("test_insert_sparse"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        pass

    def _test_insert_multivector(self, suffix):
        """
        target: test insert multivector column
        method: create table with multivector column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_multivector"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_multivector"+suffix, {"c1": {"type": "multivector,3,int"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4, 5, 6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": np.array([[7, 8, 9], [-7, -8, -9]])}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9, -7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]],
                                                                [[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))

        res = db_obj.drop_table("test_insert_multivector"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_multivector_2"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_multivector_2"+suffix, {"c1": {"type": "multivector,3,float"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_multivector_2"+suffix)
        assert table_obj
        res = table_obj.insert([{"c1": [1.1, 2.2, 3.3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4.4, 5.5, 6.6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]]}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1.1, 2.2, 3.3]], [[4.4, 5.5, 6.6]], [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]])}))

        res = db_obj.drop_table("test_insert_multivector_2"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_tensor(self, suffix):
        """
        target: test insert tensor column
        method: create table with tensor column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_tensor"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_tensor"+suffix, {"c1": {"type": "tensor,3,int"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4, 5, 6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": np.array([[7, 8, 9], [-7, -8, -9]])}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9, -7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]],
                                                                [[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))

        res = db_obj.drop_table("test_insert_tensor"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_tensor_2"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_tensor_2"+suffix, {"c1": {"type": "tensor,3,float"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_tensor_2"+suffix)
        assert table_obj
        res = table_obj.insert([{"c1": [1.1, 2.2, 3.3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4.4, 5.5, 6.6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]]}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1.1, 2.2, 3.3]], [[4.4, 5.5, 6.6]], [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]])}))

        res = db_obj.drop_table("test_insert_tensor_2"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_tensor_array(self, suffix):
        """
        target: test insert tensor_array column
        method: create table with tensor_array column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_tensor_array"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_tensor_array"+suffix, {"c1": {"type": "tensorarray,2,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [[[1, 2], [3, 4]], [[5, 6]]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [np.array([[7, 8]]), np.array([[9, 10], [11, 12]])]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[[13, 14], [15, 16], [17, 18]]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[[1, 2], [3, 4]], [[5, 6]]], [[[7, 8]], [[9, 10], [11, 12]]], [[[13, 14], [15, 16], [17, 18]]])}))
        res = db_obj.drop_table("test_insert_tensor_array"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_insert(self, suffix):
        # self.test_infinity_obj._test_version()
        self._test_insert_basic(suffix)
        self._test_insert_bool(suffix)
        self._test_insert_float16_bfloat16(suffix)
        self._test_insert_varchar(suffix)
        self._test_insert_big_varchar(suffix)
        self._test_insert_embedding(suffix)
        self._test_insert_big_embedding(suffix)
        self._test_insert_big_embedding_float(suffix)
        self._test_insert_exceed_block_size(suffix)
        self._test_insert_data_into_non_existent_table(suffix)
        self._test_insert_data_into_index_created_table(suffix)
        self._test_insert_table_with_10000_columns(suffix)
        self._test_read_after_shutdown(suffix)
        self._test_batch_insert(suffix)
        self._test_insert_zero_column(suffix)
        self._test_insert_sparse(suffix)
        self._test_insert_multivector(suffix)
        self._test_insert_tensor(suffix)
        self._test_insert_tensor_array(suffix)

    @pytest.mark.parametrize("types", ["vector,16384,int", "vector,16384,float"])
    @pytest.mark.parametrize("types_examples", [[{"c1": [1] * 16384}],
                                                [{"c1": [4] * 16384}],
                                                [{"c1": [-9999999] * 16384}],
                                                [{"c1": [1.1] * 16384}],
                                                [{"c1": [-9999999.988] * 16384}],
                                                ])
    def test_insert_big_embedding_various_type(self, types, types_examples, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_big_embedding_various_type"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_embedding_various_type"+suffix, {
            "c1": {"type": types}}, ConflictType.Error)
        res = table_obj.insert(types_examples)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_big_embedding_various_type"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # insert primitive data type not aligned with table definition
    @pytest.mark.parametrize("types", common_values.types_array)
    @pytest.mark.parametrize("types_example", common_values.types_example_array)
    def test_insert_data_not_aligned_with_table_definition(self, types, types_example, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_data_not_aligned_with_table_definition"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_data_not_aligned_with_table_definition"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": types}}, ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": types_example}]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_data_not_aligned_with_table_definition"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("types", common_values.types_array)
    def test_insert_empty_into_table(self, types, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_empty_into_table"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_empty_into_table"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": types}}, ConflictType.Error)

        # insert
        with pytest.raises(InfinityException) as e:
            values = [{}]
            table_obj.insert(values)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.SYNTAX_ERROR

        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_empty_into_table"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("values", [[{"c1": 1}], [{"c1": 1, "c2": 1, "c3": 1}]])
    def test_insert_with_not_matched_columns(self, values, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_not_matched_columns"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_not_matched_columns"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        # insert
        with pytest.raises(Exception):
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_not_matched_columns"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("values", [[{"c1": pow(2, 63) - 1, "c2": pow(2, 63) - 1}]])
    def test_insert_with_exceeding_invalid_value_range(self, values, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_exceeding_invalid_value_range"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_exceeding_invalid_value_range"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int32"}}, ConflictType.Error)

        # insert
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_pl()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_exceeding_invalid_value_range"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # batch insert, within limit
    @pytest.mark.parametrize("batch", [10, 1024, 2048, 8192])
    def test_batch_insert_within_limit(self, batch, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_batch_insert_within_limit"+suffix,
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_batch_insert_within_limit"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(batch)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_batch_insert_within_limit"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("batch", [10, 1024])
    @pytest.mark.parametrize("types", [(1, False), (1.1, False), ("1#$@!adf", False), ([1, 2, 3], True)])
    def test_insert_with_invalid_data_type(self, batch, types, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_invalid_data_type"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_invalid_data_type"+suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        # insert
        for i in range(5):
            values = [{"c1": 1, "c2": types[0]} for _ in range(batch)]
            if not types[1]:
                with pytest.raises(InfinityException) as e:
                    table_obj.insert(values)

                assert e.type == InfinityException
                assert e.value.args[0] == ErrorCode.NOT_SUPPORTED
            else:
                table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_invalid_data_type"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("batch", [10, 1024])
    def test_batch_insert_with_invalid_column_count(self, batch, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_invalid_column_count"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_invalid_column_count"+suffix, {
            "c1": {"type": "int"}}, ConflictType.Error)

        # insert
        with pytest.raises(Exception):
            for i in range(5):
                values = [{"c1": 1, "c2": 1} for _ in range(batch)]
                table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_invalid_column_count"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize('column_types', ["varchar"])
    @pytest.mark.parametrize('column_types_example', [[1, 2, 3], [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]])
    def test_various_insert_types(self, column_types, column_types_example, suffix):
        # connect

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_insert_types"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_various_insert_types"+suffix, {
            "c1": {"type": column_types}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_various_insert_types"+suffix)

        values = [{"c1": column_types_example} for _ in range(5)]
        table_obj.insert(values)

        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_various_insert_types"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.complex
    @pytest.mark.skip(reason="TODO")
    def test_insert_with_invalid_column_name(self):
        print("TODO")

    @pytest.mark.parametrize("column_name", [
        "c2",
        "$%#$sadf",
        # 1,
        # 2.2,
        # [1],
        # (1, "adsf"),
        # {"1": 1}
    ])
    def test_insert_no_match_column(self, column_name, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_no_match_column"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_no_match_column"+suffix, {
            "c1": {"type": "int"}}, ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            table_obj.insert([{column_name: 1}])
            insert_res = table_obj.output(["*"]).to_df()
            print(insert_res)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.COLUMN_NOT_EXIST

        res = db_obj.drop_table(
            "test_insert_no_match_column"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.slow
    def test_insert_with_large_data(self, suffix):
        total_row_count = 1000000

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("hr_data_mix"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("hr_data_mix" + suffix, {
            "id": {"type": "varchar"},
            "content": {"type": "varchar"},
            "dense_vec": {"type": "vector, 1024, float"},
            "sparse_vec": {"type": "sparse,250002,float,int"},
        })

        import json
        import time
        def read_jsonl(file_path):
            data = []
            with open(file_path, 'r') as file:
                for line in file:
                    data.append(json.loads(line))
            return data
        data_array = read_jsonl("./test/data/jsonl/test_table.jsonl")
        loop_count: int = total_row_count // len(data_array)

        start = time.time()
        for global_idx in range(loop_count):
            insert_data = []
            for local_idx, data in enumerate(data_array):

                # each 1000, a duplicated row is generated
                if local_idx == 9 and global_idx % 1000 == 0:
                    end = time.time()
                    print(f"ID: {global_idx}@{local_idx}, cost: {end - start}s")
                    data = data_array[0]
                    local_idx = 0

                indices = []
                values = []
                for key, value in data['sparse_vec'].items():
                    indices.append(int(key))
                    values.append(value)

                insert_data.append(
                    {
                        "id": f'{global_idx}@{local_idx}',
                        "content": data['content'],
                        "dense_vec": data['dense_vec'],
                        "sparse_vec": SparseVector(indices, values)
                    }
                )
            table_obj.insert(insert_data)
        res = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == total_row_count

        db_obj.drop_table("hr_data_mix"+suffix, ConflictType.Error)
