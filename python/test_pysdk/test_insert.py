import infinity
import numpy as np
import pandas as pd
import pytest
from infinity import index
from infinity.common import Array, ConflictType, InfinityException, SparseVector
from infinity.errors import ErrorCode
from infinity.infinity_http import infinity_http

from common import common_values


@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")


@pytest.fixture(scope="class")
def setup_class(request, http):
    if http:
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
@pytest.mark.ubsan
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

        db_obj.drop_table(table_name="table_2" + suffix,
                          conflict_type=ConflictType.Ignore)
        # infinity
        table_obj = db_obj.create_table(
            "table_2" + suffix, {
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

        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (0, 1, 2, 3, 4), 'c2': (0, 1, 2, 3, 4)})
                                      .astype({'c1': 'Int32', 'c2': 'Int32'}))

        res = db_obj.drop_table("table_2" + suffix)
        assert res.error_code == ErrorCode.OK

    def _test_insert_bool(self, suffix):
        """
        target: test insert bool column
        method: create table with bool column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("python_test_bool_insert" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("python_test_bool_insert" + suffix,
                                        {"c1": {"type": "float"}, "c2": {"type": "bool"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": -1, "c2": True}, {"c1": 2, "c2": False}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-1, 2), 'c2': (True, False)}).astype(
            {'c1': 'Float32', 'c2': 'boolean'}))
        res = db_obj.drop_table("python_test_bool_insert" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        db_obj.drop_table("python_test_bool_insert_default" + suffix, ConflictType.Ignore)
        table_instance = db_obj.create_table("python_test_bool_insert_default" + suffix, {
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
        res, extra_result = table_instance.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (1,), 'c2': (0,), 'c3': (0,), 'c4': (0,), 'c5': (0,), 'c6': (0,), 'c7': ("Tom",), 'c8': (1.0,),
             'c9': (1.0,), 'c10': (1.0,), 'c11': (1.0,), 'c12': (False,)}).astype(
            {'c1': 'Int8', 'c2': 'Int16', 'c3': 'Int32', 'c4': 'Int32',
             'c5': 'Int32', 'c6': 'Int64', 'c7': 'string', 'c8': 'Float32',
             'c9': 'Float32', 'c10': 'Float64', 'c11': 'Float64', 'c12': 'boolean'}))
        res = db_obj.drop_table("python_test_bool_insert_default" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_float16_bfloat16(self, suffix):
        """
        target: test insert float16 bfloat16 column
        method: create table with float16 bfloat16 column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("python_test_fp16_bf16" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("python_test_fp16_bf16" + suffix,
                                        {"c1": {"type": "float"}, "c2": {"type": "float16"},
                                         "c3": {"type": "bfloat16"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert(
            [{"c1": -1, "c2": 1, "c3": -1}, {"c1": 2, "c2": -2, "c3": 2}, {"c1": -3, "c2": 3, "c3": -3},
             {"c1": 4, "c2": -4, "c3": 4}, {"c1": -5, "c2": 5, "c3": -5}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (-1, 2, -3, 4, -5), 'c2': (1, -2, 3, -4, 5), 'c3': (-1, 2, -3, 4, -5)}).astype(
            {'c1': 'Float32', 'c2': 'Float32', 'c3': 'Float32'}))
        res = db_obj.drop_table("python_test_fp16_bf16" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_varchar(self, suffix):
        """
        target: test insert varchar column
        method: create table with varchar column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_varchar" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_varchar" + suffix, {"c1": {"type": "varchar"}}, ConflictType.Error)
        assert table_obj

        res = table_obj.insert([{"c1": "test_insert_varchar"}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": " test insert varchar "}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": "^789$ test insert varchar"}])
        assert res.error_code == ErrorCode.OK

        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ("test_insert_varchar", " test insert varchar ",
                                                                "^789$ test insert varchar")}, dtype='string'))
        res = db_obj.drop_table("test_insert_varchar" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_big_varchar(self, suffix):
        """
        target: test insert varchar with big length
        method: create table with varchar column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_big_varchar" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_varchar" + suffix, {"c1": {"type": "varchar"}},
                                        ConflictType.Error)
        assert table_obj
        for i in range(100):
            res = table_obj.insert([{"c1": "test_insert_big_varchar" * 1000}])
            assert res.error_code == ErrorCode.OK

        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ["test_insert_big_varchar" * 1000] * 100}, dtype='string'))

        res = db_obj.drop_table("test_insert_big_varchar" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_embedding(self, suffix):
        """
        target: test insert embedding column
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_embedding" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_embedding" + suffix, {"c1": {"type": "vector,3,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [4, 5, 6]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [7, 8, 9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9]}, {"c1": [-7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9],
                                                                [1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))

        res = db_obj.drop_table("test_insert_embedding" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        embedding_insert_float = [[1.1, 2.2, 3.3], [4.4, 5.5, 6.6], [7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]]
        db_obj.drop_table("test_insert_embedding_2" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_embedding_2" + suffix, {"c1": {"type": "vector,3,float"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_embedding_2" + suffix)
        assert table_obj
        res = table_obj.insert([{"c1": embedding_insert_float[0]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[1]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[2]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[3]}])
        assert res.error_code == ErrorCode.OK

        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': embedding_insert_float}))

        res = db_obj.drop_table("test_insert_embedding_2" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_embedding_3" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_embedding_3" + suffix, {"c1": {"type": "vector,3,float16"}},
                            ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_embedding_3" + suffix)
        assert table_obj
        res = table_obj.insert([{"c1": embedding_insert_float[0]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[1]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[2]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[3]}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': [np.array(x).astype(np.float16).tolist() for x in embedding_insert_float]}))
        res = db_obj.drop_table("test_insert_embedding_3" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_embedding_4" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_embedding_4" + suffix, {"c1": {"type": "vector,3,bfloat16"}},
                            ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_embedding_4" + suffix)
        assert table_obj
        res = table_obj.insert([{"c1": embedding_insert_float[0]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[1]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[2]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": embedding_insert_float[3]}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        tmp_bf16 = np.array(embedding_insert_float).astype('<f4')
        tmp_bf16.view('<i2')[:, ::2] = 0
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': tmp_bf16.tolist()}))
        res = db_obj.drop_table("test_insert_embedding_4" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_embedding_numpy(self, suffix):
        """
        target: test insert embedding column with numpy array values (#1253)
        method: insert a raw np.ndarray, and a plain list of numpy scalars
                (e.g. list(embedding_array), which isn't itself an ndarray)
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("test_insert_embedding_numpy_int" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_insert_embedding_numpy_int" + suffix, {"c1": {"type": "vector,3,int"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": np.array([1, 2, 3], dtype=np.int64)}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": list(np.array([4, 5, 6], dtype=np.int64))}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([1, 2, 3], [4, 5, 6])}))
        res = db_obj.drop_table("test_insert_embedding_numpy_int" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_embedding_numpy_float" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_insert_embedding_numpy_float" + suffix, {"c1": {"type": "vector,3,float"}}, ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": np.array([1.1, 2.2, 3.3], dtype=np.float32)}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": list(np.array([4.4, 5.5, 6.6], dtype=np.float32))}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (np.array([1.1, 2.2, 3.3], dtype=np.float32).tolist(),
                    np.array([4.4, 5.5, 6.6], dtype=np.float32).tolist())}))
        res = db_obj.drop_table("test_insert_embedding_numpy_float" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_big_embedding(self, suffix):
        """
        target: test insert embedding with big dimension
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_big_embedding" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_embedding" + suffix, {"c1": {"type": "vector,16384,int"}},
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
            "test_insert_big_embedding" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_big_embedding_float(self, suffix):
        """
        target: test insert embedding float with big dimension
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_big_embedding_float" + suffix,
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_embedding_float" + suffix,
                                        {"c1": {"type": "vector,16384,float"}},
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
            "test_insert_big_embedding_float" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_exceed_block_size(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_exceed_block_size" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_exceed_block_size" + suffix, {
            "c1": {"type": "float"}}, ConflictType.Error)
        assert table_obj
        values = [{"c1": 1} for _ in range(8193)]

        with pytest.raises(InfinityException) as exception:
            table_obj.insert(values)
            assert exception.type == InfinityException
            assert exception.value.args[0] == "Insert batch row limit shouldn\'t more than 8193"

        res = db_obj.drop_table("test_insert_exceed_block_size" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_data_into_non_existent_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_data_into_non_existent_table" + suffix, ConflictType.Ignore)

        # create and drop table
        table_obj = db_obj.create_table("test_insert_data_into_non_existent_table" + suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)
        res = db_obj.drop_table(
            "test_insert_data_into_non_existent_table" + suffix, ConflictType.Error)
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
            "test_insert_data_into_index_created_table" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_data_into_index_created_table" + suffix,
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

        table_obj.create_index("my_index_2",
                               index.IndexInfo("c1",
                                               index.IndexType.IVF,
                                               {"metric": "l2"}), ConflictType.Error)

        # insert
        values = [{"c1": [1.1 for _ in range(1024)]}]
        table_obj.insert(values)
        insert_res, extra_result = table_obj.output(["*"]).to_pl()
        print(insert_res)
        res, extra_result = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == 1

        res = table_obj.drop_index("my_index_1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index_2", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_data_into_index_created_table" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_table_with_10000_columns(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_table_with_10000_columns" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_table_with_10000_columns" + suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        for i in range(100):
            values = [{"c1": i * 100 + j, "c2": i * 100 + j + 1}
                      for j in range(100)]
            table_obj.insert(values)
        insert_res, extra_result = table_obj.output(["*"]).to_df()
        print(insert_res)

    def _test_read_after_shutdown(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("test_insert_table_with_10000_columns" + suffix)
        insert_res, extra_result = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_table_with_10000_columns" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_batch_insert(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_batch_insert" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_batch_insert" + suffix, {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8192)]
        table_obj.insert(values)
        insert_res, extra_result = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table("test_batch_insert" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_zero_column(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_zero_column" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_zero_column" + suffix, {
            "c1": {"type": "int"}}, ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            table_obj.insert([])
            insert_res, extra_result = table_obj.output(["*"]).to_df()
            print(insert_res)

        assert e.type == InfinityException
        # assert e.value.args[0] == ErrorCode.INSERT_WITHOUT_VALUES

        res = db_obj.drop_table("test_insert_zero_column" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_null(self, suffix):
        """
        target: test insert null values into nullable columns
        method:
        1. create table with nullable columns of different types
        2. insert rows with explicit None values
        3. verify null values are stored correctly
        4. drop table
        expected: all operations successfully, NULL values stored as expected
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_null" + suffix, ConflictType.Ignore)

        table_obj = db_obj.create_table(
            "test_insert_null" + suffix,
            {
                "c1": {"type": "int"},
                "c2": {"type": "varchar"},
                "c3": {"type": "float"},
            },
            ConflictType.Error,
        )
        assert table_obj

        # Insert explicit None values into nullable columns
        res = table_obj.insert([{"c1": 1, "c2": "hello", "c3": 1.5}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": None, "c2": "world", "c3": 2.5}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": 3, "c2": None, "c3": 3.5}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": 4, "c2": "test", "c3": None}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": None, "c2": None, "c3": None}])
        assert res.error_code == ErrorCode.OK

        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(
            res.sort_values("c1", na_position="last").reset_index(drop=True),
            pd.DataFrame({
                "c1": pd.array([1, 3, 4, pd.NA, pd.NA], dtype="Int32"),
                "c2": pd.array(["hello", pd.NA, "test", "world", pd.NA], dtype="string"),
                "c3": pd.array([1.5, 3.5, pd.NA, 2.5, pd.NA], dtype="Float32"),
            }),
        )

        res = db_obj.drop_table("test_insert_null" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_not_null(self, suffix):
        """
        target: test insert into NOT NULL columns
        method:
        1. Create table with a NOT NULL column (no default)
        2. Insert omitting the NOT NULL column — fails
        3. Insert with explicit None in the NOT NULL column — fails
        4. Insert with proper value in the NOT NULL column — succeeds
        5. Verify data
        6. Drop table
        expected: NOT NULL constraint enforced during insert
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_not_null" + suffix, ConflictType.Ignore)

        table_obj = db_obj.create_table(
            "test_insert_not_null" + suffix,
            {
                "c1": {"type": "int"},
                "c2": {"type": "varchar", "constraints": ["not null"]},
                "c3": {"type": "float", "constraints": ["not null"], "default": 0.0},
            },
            ConflictType.Error,
        )
        assert table_obj

        # Case 1: omitting c2 (NOT NULL without default) — fails
        with pytest.raises(Exception):
            table_obj.insert([{"c1": 1, "c3": 1.0}])

        # Case 2: c2=None (explicit NULL on NOT NULL column) — fails
        with pytest.raises(Exception):
            table_obj.insert([{"c1": 1, "c2": None, "c3": 1.0}])

        # Case 3: omitting c3 (NOT NULL with default) — succeeds, uses default
        res = table_obj.insert([{"c1": 1, "c2": "hello"}])
        assert res.error_code == ErrorCode.OK

        # Case 4: all values provided — succeeds
        res = table_obj.insert([{"c1": 2, "c2": "world", "c3": 2.5}])
        assert res.error_code == ErrorCode.OK

        res, _ = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(
            res.sort_values("c1").reset_index(drop=True),
            pd.DataFrame({
                "c1": pd.array([1, 2], dtype="Int32"),
                "c2": pd.array(["hello", "world"], dtype="string"),
                "c3": pd.array([0.0, 2.5], dtype="Float32"),
            }),
        )

        res = db_obj.drop_table("test_insert_not_null" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_sparse(self, suffix):
        """
        target: test insert sparse column
        method: create table with sparse column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_sparse" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_sparse" + suffix, {"c1": {"type": "sparse,100,float,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": SparseVector(indices=[10, 20, 30], values=[1.1, 2.2, 3.3])}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": SparseVector([40, 50, 60], [4.4, 5.5, 6.6])}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": {"70": 7.7, "80": 8.8, "90": 9.9}},
                                {"c1": {"70": -7.7, "80": -8.8, "90": -9.9}}])
        assert res.error_code == ErrorCode.OK
        print(table_obj.output(["*"]).to_pl())
        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (
                {"10": 1.1, "20": 2.2, "30": 3.3},
                {"40": 4.4, "50": 5.5, "60": 6.6},
                {"70": 7.7, "80": 8.8, "90": 9.9},
                {"70": -7.7, "80": -8.8, "90": -9.9})}))

        res = db_obj.drop_table("test_insert_sparse" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_multivector(self, suffix):
        """
        target: test insert multivector column
        method: create table with multivector column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_multivector" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_multivector" + suffix, {"c1": {"type": "multivector,3,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4, 5, 6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": np.array([[7, 8, 9], [-7, -8, -9]])}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9, -7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]],
                                                                [[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))

        res = db_obj.drop_table("test_insert_multivector" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_multivector_2" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_multivector_2" + suffix, {"c1": {"type": "multivector,3,float"}},
                            ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_multivector_2" + suffix)
        assert table_obj
        res = table_obj.insert([{"c1": [1.1, 2.2, 3.3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4.4, 5.5, 6.6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]]}])
        assert res.error_code == ErrorCode.OK

        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1.1, 2.2, 3.3]], [[4.4, 5.5, 6.6]], [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]])}))

        res = db_obj.drop_table("test_insert_multivector_2" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_tensor(self, suffix):
        """
        target: test insert tensor column
        method: create table with tensor column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_tensor" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_tensor" + suffix, {"c1": {"type": "tensor,3,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4, 5, 6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": np.array([[7, 8, 9], [-7, -8, -9]])}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9, -7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]],
                                                                [[1, 2, 3]], [[4, 5, 6]], [[7, 8, 9], [-7, -8, -9]])}))

        res = db_obj.drop_table("test_insert_tensor" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("test_insert_tensor_2" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_insert_tensor_2" + suffix, {"c1": {"type": "tensor,3,float"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_insert_tensor_2" + suffix)
        assert table_obj
        res = table_obj.insert([{"c1": [1.1, 2.2, 3.3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[4.4, 5.5, 6.6]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]]}])
        assert res.error_code == ErrorCode.OK

        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[1.1, 2.2, 3.3]], [[4.4, 5.5, 6.6]], [[7.7, 8.8, 9.9], [-7.7, -8.8, -9.9]])}))

        res = db_obj.drop_table("test_insert_tensor_2" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_tensor_array(self, suffix):
        """
        target: test insert tensor_array column
        method: create table with tensor_array column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_tensor_array" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_tensor_array" + suffix, {"c1": {"type": "tensorarray,2,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": [[[1, 2], [3, 4]], [[5, 6]]]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [np.array([[7, 8]]), np.array([[9, 10], [11, 12]])]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [[[13, 14], [15, 16], [17, 18]]]}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([[[1, 2], [3, 4]], [[5, 6]]], [[[7, 8]], [[9, 10], [11, 12]]], [[[13, 14], [15, 16], [17, 18]]])}))
        res = db_obj.drop_table("test_insert_tensor_array" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_array(self, suffix):
        """
        target: test insert array column
        method: create table with array column
        expected: ok
        """
        if suffix == '_http':
            pytest.skip("HTTP not support array type")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_array" + suffix, ConflictType.Ignore)
        with pytest.raises(InfinityException):
            db_obj.create_table("test_insert_array" + suffix, {"c1": {"type": "array,array"}}, ConflictType.Error)
        table_obj = db_obj.create_table("test_insert_array" + suffix, {"c1": {"type": "array,array,array,int"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert(
            [{"c1": Array(Array(Array(1, 2), Array(3, 4)), Array(Array(5, 6)), Array(Array()), Array())}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": Array(Array(Array()))}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": Array(Array())}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": Array()}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res,
                                      pd.DataFrame({'c1': ([[[1, 2], [3, 4]], [[5, 6]], [[]], []], [[[]]], [[]], [])}))
        res = db_obj.drop_table("test_insert_array" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_insert_array_varchar(self, suffix):
        """
        target: test insert array column
        method: create table with array column
        expected: ok
        """
        if suffix == '_http':
            pytest.skip("HTTP not support array type")
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_array_varchar" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_array_varchar" + suffix,
                                        {"c1": {"type": "array,array,array,varchar"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"c1": Array(Array(Array("hello", "world"), Array("!")), Array(
            Array("Hi! This is an example of long text! Hi! This is an example of long text!")), Array(Array()),
                                             Array())}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": Array(Array(Array()))}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": Array(Array())}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": Array()}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (
            [[["hello", "world"], ["!"]],
             [["Hi! This is an example of long text! Hi! This is an example of long text!"]],
             [[]], []], [[[]]], [[]], [])}))
        res = db_obj.drop_table("test_insert_array_varchar" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_insert(self, suffix):
        # self.test_infinity_obj._test_version()
        self._test_insert_basic(suffix)
        self._test_insert_bool(suffix)
        self._test_insert_float16_bfloat16(suffix)
        self._test_insert_varchar(suffix)
        self._test_insert_big_varchar(suffix)
        self._test_insert_embedding(suffix)
        self._test_insert_embedding_numpy(suffix)
        self._test_insert_big_embedding(suffix)
        self._test_insert_big_embedding_float(suffix)
        self._test_insert_exceed_block_size(suffix)
        self._test_insert_data_into_non_existent_table(suffix)
        self._test_insert_data_into_index_created_table(suffix)
        self._test_insert_table_with_10000_columns(suffix)
        self._test_read_after_shutdown(suffix)
        self._test_batch_insert(suffix)
        self._test_insert_zero_column(suffix)
        self._test_insert_null(suffix)
        self._test_insert_not_null(suffix)
        self._test_insert_sparse(suffix)
        self._test_insert_multivector(suffix)
        self._test_insert_tensor(suffix)
        self._test_insert_tensor_array(suffix)
        self._test_insert_array(suffix)
        self._test_insert_array_varchar(suffix)

    def test_insert_rows_mismatch(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("python_test_insert_rows_mismatch" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("python_test_insert_rows_mismatch" + suffix,
                                        {"num": {"type": "integer", "default": 33},
                                         "body": {"type": "varchar", "default": "ABC"},
                                         "vec": {"type": "vector,1,float"}},
                                        ConflictType.Error)
        assert table_obj
        res = table_obj.insert([{"body": "", "vec": [1.0]}, {"vec": [2.0], "body": "DEF"}, {"vec": [4.0]}])
        assert res.error_code == ErrorCode.OK
        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'num': (33, 33, 33), 'body': ("", "DEF", "ABC"), 'vec': ([1.0], [2.0], [4.0])}).astype(
            {'num': 'Int32', 'body': 'string'}))
        res = db_obj.drop_table("python_test_insert_rows_mismatch" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

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
            "test_insert_big_embedding_various_type" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_big_embedding_various_type" + suffix, {
            "c1": {"type": types}}, ConflictType.Error)
        res = table_obj.insert(types_examples)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_insert_big_embedding_various_type" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # insert primitive data type not aligned with table definition
    @pytest.mark.parametrize("types", ["vector,16384,float"])
    @pytest.mark.parametrize("types_example", [1, 1, 6, 'hello', True])
    def test_insert_data_not_aligned_with_table_definition(self, types, types_example, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_data_not_aligned_with_table_definition" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_data_not_aligned_with_table_definition" + suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": types}}, ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": types_example}]
        with pytest.raises(InfinityException) as e:
            table_obj.insert(values)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.NOT_SUPPORTED

        res = db_obj.drop_table(
            "test_insert_data_not_aligned_with_table_definition" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("types", common_values.types_array)
    def test_insert_empty_into_table(self, types, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert_empty_into_table" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_empty_into_table" + suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": types}}, ConflictType.Error)

        # insert
        with pytest.raises(InfinityException) as e:
            values = [{}]
            table_obj.insert(values)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.SYNTAX_ERROR

        insert_res, extra_result = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_empty_into_table" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("values", [[{"c1": 1}], [{"c1": 1, "c2": 1, "c3": 1}]])
    def test_insert_with_not_matched_columns(self, values, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_not_matched_columns" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_not_matched_columns" + suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        # insert
        if len(values[0]) == 1:
            # missing columns: allowed, fills NULL
            insert_res = table_obj.insert(values)
            assert insert_res.error_code == ErrorCode.OK
            insert_res, extra_result = table_obj.output(["*"]).to_df()
            print(insert_res)
        else:
            # extra columns: still raises exception
            with pytest.raises(Exception):
                table_obj.insert(values)

        res = db_obj.drop_table(
            "test_insert_with_not_matched_columns" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("values", [[{"c1": pow(2, 63) - 1, "c2": pow(2, 63) - 1}]])
    def test_insert_with_exceeding_invalid_value_range(self, values, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_exceeding_invalid_value_range" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_exceeding_invalid_value_range" + suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int32"}}, ConflictType.Error)

        # insert
        table_obj.insert(values)
        insert_res, extra_result = table_obj.output(["*"]).to_pl()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_exceeding_invalid_value_range" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # batch insert, within limit
    @pytest.mark.parametrize("batch", [10, 1024, 2048, 8192])
    def test_batch_insert_within_limit(self, batch, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_batch_insert_within_limit" + suffix,
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_batch_insert_within_limit" + suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(batch)]
        table_obj.insert(values)
        insert_res, extra_result = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_batch_insert_within_limit" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("batch", [10, 1024])
    @pytest.mark.parametrize("types", [(1, False), (1.1, False), ("1#$@!adf", False), ([1, 2, 3], True)])
    def test_insert_with_invalid_data_type(self, batch, types, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_invalid_data_type" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_invalid_data_type" + suffix,
                                        {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        # insert
        # Note: Use small batch for invalid types to avoid server crash with large batch insert
        effective_batch = batch if types[1] else min(batch, 10)
        for i in range(5):
            values = [{"c1": 1, "c2": types[0]} for _ in range(effective_batch)]
            if not types[1]:
                with pytest.raises(InfinityException) as e:
                    table_obj.insert(values)

                assert e.type == InfinityException
                assert e.value.args[0] == ErrorCode.NOT_SUPPORTED
            else:
                table_obj.insert(values)
        insert_res, extra_result = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_invalid_data_type" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("batch", [10, 1024])
    def test_batch_insert_with_invalid_column_count(self, batch, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(
            "test_insert_with_invalid_column_count" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_with_invalid_column_count" + suffix, {
            "c1": {"type": "int"}}, ConflictType.Error)

        # insert
        with pytest.raises(Exception):
            for i in range(5):
                values = [{"c1": 1, "c2": 1} for _ in range(batch)]
                table_obj.insert(values)
        insert_res, extra_result = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_insert_with_invalid_column_count" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize('column_types', ["varchar"])
    @pytest.mark.parametrize('column_types_example', [[1, 2, 3], [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]])
    def test_various_insert_types(self, column_types, column_types_example, suffix):
        # connect

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_insert_types" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_various_insert_types" + suffix, {
            "c1": {"type": column_types}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_various_insert_types" + suffix)

        values = [{"c1": column_types_example} for _ in range(5)]
        table_obj.insert(values)

        insert_res, extra_result = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = db_obj.drop_table(
            "test_various_insert_types" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

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
        db_obj.drop_table("test_insert_no_match_column" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_insert_no_match_column" + suffix, {
            "c1": {"type": "int"}}, ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            table_obj.insert([{column_name: 1}])
            insert_res, extra_result = table_obj.output(["*"]).to_df()
            print(insert_res)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.SYNTAX_ERROR

        res = db_obj.drop_table(
            "test_insert_no_match_column" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    