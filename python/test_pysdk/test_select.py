import importlib
import sys
import os
import os
import pandas as pd
import pytest
from common import common_values
import infinity
import infinity_embedded
from numpy import dtype
from infinity.errors import ErrorCode
from infinity.common import ConflictType
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http
from common.utils import copy_data

@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")

@pytest.fixture(scope="class")
def setup_class(request, local_infinity, http):
    if local_infinity:
        module = importlib.import_module("infinity_embedded.common")
        func = getattr(module, 'ConflictType')
        globals()['ConflictType'] = func
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
    def test_select(self, suffix):
        """
        target: test table select apis
        method:
        1. create tables
            - 'test_select'
                - c1 int primary key
                - c2 int not null
        2. insert
            - (-3, -3)
            - (-2, -2)
            - (-1, -1)
            - (0, 0)
            - (1, 1)
            - (2, 2)
            - (3, 3)
            - (-8, -8)
            - (-7, -7)
            - (-6, -6)
            - (7, 7)
            - (8, 8)
            - (9, 9)
        3. select
            - select * from test_select
                - Consistent with 2. Insert
            - select c1, c2 from test_select
                - Consistent with 2. Insert
            - select c1 + c2 from test_select where (c1 = 3)
                - 6
            - select c1 from test_select where c1 > 2 and c2 < 4
                - 3
            - select c2 from test_select where (-7 < c1 or 9 <= c1) and (c1 = 3)
                - 3
            - select c2 from test_select where (-8 < c1 and c1 <= -7) or (c1 >= 1 and 2 > c1)
                - -7
                - 1
            - select c2 from test_select where ((c1 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c1)) and ((c1 > 0 and c1 <= 1) or (c1 > -8 and c1 < -6))
                - -7
                - 1
            - select c2 from test_select where (-7 < c1 or 9 <= c1) and (c2 = 3)
                - 3
            - select c2 from test_select where (-8 < c1 and c2 <= -7) or (c1 >= 1 and 2 > c2)
                - -7
                - 1
            - select c2 from test_select where ((c2 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c2)) and ((c2 > 0 and c1 <= 1) or (c1 > -8 and c2 < -6))
                - -7
                - 1
            - select c2 from test_select where (not(c2 < -8 or -4 < c1) or not(c1 < 0 or 5 <= c2)) and not((c2 <= 0 or c1 > 1) and (c1 <= -8 or c2 >= -6))
                - -7
                - 1
        4. drop tables
            - 'test_select'
        expect: all operations successfully
        """
        db_obj = self.infinity_obj.get_database("default_db")

        # infinity
        db_obj.drop_table("test_select"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_select"+suffix, {
                "c1": {"type": "int", "constraints": ["primary key", "not null"]},
                "c2": {"type": "int", "constraints": ["not null"]}}, ConflictType.Error)

        assert table_obj is not None

        res = table_obj.insert(
            [{"c1": -3, "c2": -3}, {"c1": -2, "c2": -2}, {"c1": -1, "c2": -1}, {"c1": 0, "c2": 0}, {"c1": 1, "c2": 1},
             {"c1": 2, "c2": 2}, {"c1": 3, "c2": 3}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert(
            [{"c1": -8, "c2": -8}, {"c1": -7, "c2": -7}, {"c1": -6, "c2": -6}, {"c1": 7, "c2": 7}, {"c1": 8, "c2": 8},
             {"c1": 9, "c2": 9}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                         'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = table_obj.output(["c1", "c2"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                         'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = table_obj.output(
            ["c1 + c2"]).filter("c1 = 3").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'(c1 + c2)': (6,)})
                                      .astype({'(c1 + c2)': dtype('int32')}))

        res = table_obj.output(
            ["c1"]).filter("c1 > 2 and c2 < 4").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (3,)})
                                      .astype({'c1': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "(-7 < c1 or 9 <= c1) and (c1 = 3)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (3,)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "(-8 < c1 and c1 <= -7) or (c1 >= 1 and 2 > c1)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "((c1 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c1)) and ((c1 > 0 and c1 <= 1) or (c1 > -8 and c1 < -6))").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "(-7 < c1 or 9 <= c1) and (c2 = 3)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (3,)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "(-8 < c1 and c2 <= -7) or (c1 >= 1 and 2 > c2)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "((c2 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c2)) and ((c2 > 0 and c1 <= 1) or (c1 > -8 and c2 < -6))").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res = table_obj.output(["c2"]).filter(
            "(not(c2 < -8 or -4 < c1) or not(c1 < 0 or 5 <= c2)) and not((c2 <= 0 or c1 > 1) and (c1 <= -8 or c2 >= -6))").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res = db_obj.drop_table("test_select"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_select_aggregate(self, suffix):
        """
        target: test table select apis
        methods:
        1. create tables
            - 'test_select_aggregate'
                - c1 int primary key
                - c2 float not null
        2. insert
            - (-30, -1.43)
            - (-2, 2.5)
            - (42, -5.1)
            - (0, 0)
            - (1, 1)
            - (2, 2)
            - (3, 3)
            - (-8, -8)
            - (-13, -7.5)
            - (-6.3, 1.1)
            - (17, 0.47)
            - (85, 62)
            - (90, -19)
        3. select
            - select count(*) from test_select_aggregate
                - 13
            - select max(c1) from test_select_aggregate
                - 90
            - select min(c2) from test_select_aggregate
                - -19
            - select min(c1) + max(c2) from test_select_aggregate
                - 32'
            - select sum(c1) from test_select_aggregate
                - -203.3
            - select avg(c2) from test_select_aggregate
                - 2.387692308
        """
        db_obj = self.infinity_obj.get_database("default_db")

        # infinity
        db_obj.drop_table("test_select_aggregate"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_select_aggregate"+suffix, {
                "c1": {"type": "int", "constraints": ["primary key", "not null"]},
                "c2": {"type": "float", "constraints": ["not null"]}}, ConflictType.Error)

        assert table_obj is not None

        res = table_obj.insert(
            [{"c1": -30, "c2": -1.43}, {"c1": -2, "c2": -2.5}, {"c1": 42, "c2": -5.1}, {"c1": 0, "c2": 0},
             {"c1": 1, "c2": 1},
             {"c1": 2, "c2": 2}, {"c1": 3, "c2": 3}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert(
            [{"c1": -8, "c2": -8}, {"c1": -13, "c2": -7.5}, {"c1": -6.3, "c2": 1.1}, {"c1": 17, "c2": 0.47},
             {"c1": 85, "c2": 62},
             {"c1": 90, "c2": -19}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == 13
        res = table_obj.output(["max(c1)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == 90
        res = table_obj.output(["min(c2)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == -19
        res = table_obj.output(["min(c1) + max(c2)"]).to_pl()
        print(res)
        res = table_obj.output(["sum(c1)"]).to_pl()
        print(res)
        res = table_obj.output(["avg(c2)"]).to_pl()
        print(res)

        res = db_obj.drop_table("test_select_aggregate"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_select_varchar(self, suffix):
        """
        target: test table select apis
        method:
        1. create tables
            - 'test_select_varchar'
                - c1 varchar primary key
                - c2 varchar not null
        2. insert
            - ('a', 'a')
            - ('b', 'b')
            - ('c', 'c')
            - ('d', 'd')
            - ('e', 'e')
            - ('f', 'f')
            - ('g', 'g')
            - ('h', 'h')
            - ('i', 'i')
            - ('j', 'j')
            - ('k', 'k')
            - ('l', 'l')
            - ('m', 'm')
        3. select
            - select * from test_select_varchar
                - Consistent with 2. Insert
            - select c1, c2 from test_select_varchar
                - Consistent with 2. Insert
            - select c1 + c2 from test_select_varchar where (c1 = 'a')
                - 'aa'
            - select c1 from test_select_varchar where c1 > 'a' and c2 < 'c'
                - 'b'
            - select c2 from test_select_varchar where ('a' < c1 or 'm' <= c1) and (c1 = 'a')
                - 'a'
            - select c2 from test_select_varchar where ('a' < c1 and c1 <= 'b') or (c1 >= 'c' and 'd' > c1)
                - 'b'
                - 'c'
            - select c2 from test_select_varchar where ((c1 >= 'a' and 'd' >= c1) or (c1 >= 'e' and 'j' > c1)) and ((c1 > 'e' and c1 <= 'f') or (c1 > 'a' and c1 < 'c'))
                - 'b'
                - 'c'
            - select c2 from test_select_varchar where ('a' < c1 or 'm' <= c1) and (c2 = 'a')
                - 'a'
            - select c2 from test_select_varchar where ('a' < c1 and c2 <= 'b') or (c1 >= 'c' and 'd' > c2)`
                - 'b'
                - 'c'
        4. drop tables
               - 'test_select_varchar'
        expect: all operations successfully

        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_varchar"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_varchar"+suffix,
                            {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_varchar"+suffix)
        table_obj.insert(
            [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'},
             {"c1": 'e', "c2": 'e'}, {"c1": 'f', "c2": 'f'}, {
                 "c1": 'g', "c2": 'g'}, {"c1": 'h', "c2": 'h'},
             {"c1": 'i', "c2": 'i'}, {"c1": 'j', "c2": 'j'}, {
                 "c1": 'k', "c2": 'k'}, {"c1": 'l', "c2": 'l'},
             {"c1": 'm', "c2": 'm'}])
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                                'l', 'm'),
                                                         'c2': ('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                                'l', 'm')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res = table_obj.output(
            ["c1", "c2"]).filter("c1 = 'a'").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('a',), 'c2': ('a',)}).astype(
            {'c1': dtype('O'), 'c2': dtype('O')}))

        # TODO NotImplement Error: Not implement: varchar > varchar
        # res = table_obj.output(["c1"]).filter("c1 > 'a' and c2 < 'c'").to_df()
        # pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('b',)}).astype({'c1': dtype('O')}))
        res = db_obj.drop_table("test_select_varchar"+suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_big(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_select_big"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_big"+suffix, {
            "c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
            "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_big"+suffix)

        for i in range(1000):
            table_obj.insert(
                [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'}])

        res = db_obj.drop_table("test_select_big"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_select_embedding_int32(self, check_data, suffix):
        """
        TestSelect.test_select_embedding()
        This method tests the functionality of selecting embeddings from a table in the database.

        Parameters:
        None

        Return Type:
        None

        Example Usage:
        test_obj = TestSelect()
        test_obj.test_select_embedding()

        """
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("test_select_embedding"+suffix, ConflictType.Ignore)

        res = db_obj.create_table("test_select_embedding"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_embedding"+suffix)

        if not check_data:
            copy_data("embedding_int_dim3.csv")
        test_dir = "/var/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir, None)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["c2"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c2': ([2, 3, 4], [6, 7, 8], [10, 11, 12])}))

        res = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (1, 5, 9), 'c2': ([2, 3, 4], [6, 7, 8], [10, 11, 12])})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('O')}))

        res = db_obj.drop_table("test_select_embedding"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_float_dim4.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_select_embedding_float(self, check_data, suffix):
        """
        Method: test_select_embedding_float
        This method performs a series of tests on the `test_select_embedding_float` table in the Infinity database.

        Parameters:
        None

        Returns:
        None

        Example Usage:
        test_select_embedding_float()

        """
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("test_select_embedding_float"+suffix, ConflictType.Ignore)

        res = db_obj.create_table("test_select_embedding_float"+suffix, {
            "c1": {"type": "float"}, "c2": {"type": "vector,4,float"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_embedding_float"+suffix)

        test_dir = "/var/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_float_dim4.csv"
        if not check_data:
            copy_data("embedding_float_dim4.csv")
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir, None)
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["c2"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c2': ([0.1, 0.2, 0.3, -0.2], [0.2, 0.1, 0.3, 0.4],
                    [0.3, 0.2, 0.1, 0.4], [0.4, 0.3, 0.2, 0.1])}))

        res = table_obj.output(["*"]).to_df()
        print(res)

        pd.testing.assert_frame_equal(res,
                                      pd.DataFrame({'c1': (2, 4, 6, 8), 'c2': (
                                          [0.1, 0.2, 0.3, -0.2], [0.2,
                                                                  0.1, 0.3, 0.4],
                                          [0.3, 0.2, 0.1, 0.4], [0.4, 0.3, 0.2, 0.1])})
                                      .astype({'c1': dtype('float32'), 'c2': dtype('O')}))

        res = db_obj.drop_table(
            "test_select_embedding_float"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_select_big_embedding(self, check_data, suffix):
        """
        Method: test_select_big_embedding

        Description:
        This method performs a series of operations to test the selection of a large embedding from a table. It imports
        data from a CSV file, creates a table, imports the data into the table,
        and then searches for and retrieves a specific column from the table.

        Parameters:
        None

        Return Type:
        None

        Example Usage:
        test_select_big_embedding()
        """
        db_obj = self.infinity_obj.get_database("default_db")

        db_obj.drop_table("test_select_big_embedding"+suffix, ConflictType.Ignore)

        db_obj.create_table("test_select_big_embedding"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_big_embedding"+suffix)

        if not check_data:
            copy_data("embedding_int_dim3.csv")
        test_dir = "/var/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        for i in range(1000):
            res = table_obj.import_data(test_csv_dir, None)
            assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_select_big_embedding"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    def test_select_same_output(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_same_output"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_same_output"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_same_output"+suffix)
        table_obj.insert([{"c1": 1, "c2": 2}])
        print()
        res = table_obj.output(["c1", "c2"]).to_df()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_df()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_df()
        print(res)

        res = db_obj.drop_table("test_select_same_output"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    def test_empty_table(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_empty_table"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_empty_table"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_empty_table"+suffix)
        print()
        res = table_obj.output(["c1", "c2"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c2': ()}).astype(
            {'c1': dtype('int32'), 'c2': dtype('int32')}))
        res = table_obj.output(["c1", "c1"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c1_2': ()}).astype(
            {'c1': dtype('int32'), 'c1_2': dtype('int32')}))
        res = table_obj.output(["c1", "c2", "c1"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c2': (), 'c1_2': ()}).astype(
            {'c1': dtype('int32'), 'c2': dtype('int32'), 'c1_2': dtype('int32')}))

        res = db_obj.drop_table("test_empty_table"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1 = 0",
    ])
    def test_valid_filter_expression(self, filter_list, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_valid_filter_expression"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_valid_filter_expression"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])
        # TODO add more filter function
        select_res_df = table_obj.output(["*"]).filter(filter_list).to_pl()
        print(str(select_res_df))

        res = db_obj.drop_table(
            "test_valid_filter_expression"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("filter_list", [
        pytest.param("c1"),
        pytest.param("_row_id"),
        pytest.param("*"),
        pytest.param("#@$%@#f"),
        pytest.param("c1 + 0.1 and c2 - 1.0"),
        pytest.param("c1 * 0.1 and c2 / 1.0"),
        pytest.param("c1 > 0.1 %@#$sf c2 < 1.0"),
    ])
    def test_invalid_filter_expression(self, filter_list, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_invalid_filter_expression"+suffix,
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_invalid_filter_expression"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])
        # TODO add more filter function
        with pytest.raises(Exception):
            select_res_df = table_obj.output(["*"]).filter(filter_list).to_pl()
            print(str(select_res_df))

        res = db_obj.drop_table(
            "test_invalid_filter_expression"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
