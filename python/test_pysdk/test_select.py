import importlib
import sys
import os
import os
import pandas as pd
import pytest
from common import common_values
import infinity
import infinity.index as index
import infinity_embedded
from numpy import dtype
from infinity.errors import ErrorCode
from infinity.common import ConflictType, SortType

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http
from common.utils import copy_data
from datetime import date, time, datetime


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
            - select * from test_select where (c1 in (1, 2, 3))
            - select * from test_select where (c1 in (1, 2, 3) and c2 in (1, 2, 3))
            - select * from test_select where (c1 not in (1, 2, 3))
            - select * from test_select where ((c2 + 1) in (8, 9, 10))
            - select * from test_select where ((-c1) in (1, 2, 3))
        4. drop tables
            - 'test_select'
        expect: all operations successfully
        """
        db_obj = self.infinity_obj.get_database("default_db")

        # infinity
        db_obj.drop_table("test_select" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_select" + suffix, {
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

        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                         'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["c1", "c2"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9),
                                                         'c2': (-3, -2, -1, 0, 1, 2, 3, -8, -7, -6, 7, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_result = table_obj.output(
            ["c1 + c2"]).filter("c1 = 3").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'(c1 + c2)': (6,)})
                                      .astype({'(c1 + c2)': dtype('int32')}))

        res, extra_result = table_obj.output(
            ["c1"]).filter("c1 > 2 and c2 < 4").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (3,)})
                                      .astype({'c1': dtype('int32')}))

        res, extra_result = table_obj.output(["c2"]).filter("(-7 < c1 or 9 <= c1) and (c1 = 3)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (3,)})
                                      .astype({'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["c2"]).filter("(-8 < c1 and c1 <= -7) or (c1 >= 1 and 2 > c1)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["c2"]).filter(
            "((c1 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c1)) and ((c1 > 0 and c1 <= 1) or (c1 > -8 and c1 < -6))").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["c2"]).filter(
            "(-7 < c1 or 9 <= c1) and (c2 = 3)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (3,)})
                                      .astype({'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["c2"]).filter(
            "(-8 < c1 and c2 <= -7) or (c1 >= 1 and 2 > c2)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["c2"]).filter(
            "((c2 >= -8 and -4 >= c1) or (c1 >= 0 and 5 > c2)) and ((c2 > 0 and c1 <= 1) or (c1 > -8 and c2 < -6))").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["c2"]).filter(
            "(not(c2 < -8 or -4 < c1) or not(c1 < 0 or 5 <= c2)) and not((c2 <= 0 or c1 > 1) and (c1 <= -8 or c2 >= -6))").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c2': (1, -7)})
                                      .astype({'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["*"]).filter("c1 in (1, 2, 3)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (1, 2, 3),
                                                         'c2': (1, 2, 3)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["*"]).filter("c1 in (1, 2, 3) and c2 in (1, 2, 3)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (1, 2, 3),
                                                         'c2': (1, 2, 3)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["*"]).filter("c1 not in (1, 2, 3)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1, 0, -8, -7, -6, 7, 8, 9),
                                                         'c2': (-3, -2, -1, 0, -8, -7, -6, 7, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_result = table_obj.output(["*"]).filter("(c2 + 1) in (8, 9, 10)").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (7, 8, 9),
                                                         'c2': (7, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        # res = table_obj.output(["*"]).filter("(-c1) in (1, 2, 3)").to_df()
        # pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2, -1),
        #                                                  'c2': (-3, -2, -1)})
        #                               .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = db_obj.drop_table("test_select" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_select_datetime(self, suffix):
        """
        target: test table select apis
        methods:
        1. create tables
            - 'test_select_datetime'
                - c1 date
                - c2 time
                - c3 datetime
        2. insert
            - ('2024-09-23', '20:45:11', '2024-09-23 20:45:11')
            - ('2022-05-26', '21:44:33', '2022-05-26 21:44:33')
            - ('2021-03-04', '20:58:59', '2021-03-04 20:58:59')
        3. select
            - select * from test_select_datetime
            - select c1, c2 from test_select_datetime where c1=DATE '2024-09-23'
            - select * from test_select_datetime where c2=TIME '21:44:33'
            - select * from test_select_datetime where c3=DATETIME '2021-03-04 20:58:59'

        """

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_datetime" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_select_datetime" + suffix, {
                "c1": {"type": "date"},
                "c2": {"type": "time"},
                "c3": {"type": "datetime"},
                "c4": {"type": "timestamp"}},
            ConflictType.Error)

        assert table_obj is not None

        d_list = list()
        t_list = list()
        dt_list = list()
        ts_list = list()

        d_list.append("2024-09-23")
        t_list.append("20:45:11")
        dt_list.append("2024-09-23 20:45:11")
        ts_list.append("2024-09-23 20:45:11")
        res = table_obj.insert(
            {"c1": d_list[0], "c2": t_list[0], "c3": dt_list[0], "c4": ts_list[0]}
        )
        assert res.error_code == ErrorCode.OK

        d_list.append("2022-05-26")
        t_list.append("21:44:33")
        dt_list.append("2022-05-26 21:44:33")
        ts_list.append("2022-05-26 21:44:33")
        res = table_obj.insert(
            {"c1": d_list[1], "c2": t_list[1], "c3": dt_list[1], "c4": ts_list[1]}
        )
        assert res.error_code == ErrorCode.OK

        d_list.append("2021-03-04")
        t_list.append("20:58:59")
        dt_list.append("2021-03-04 20:58:59")
        ts_list.append("2021-03-04 20:58:59")
        res = table_obj.insert(
            {"c1": d_list[2], "c2": t_list[2], "c3": dt_list[2], "c4": ts_list[2]}
        )
        assert res.error_code == ErrorCode.OK

        res, extra_result = table_obj.output(["*"]).to_pl()
        for i in range(3):
            assert res.item(i, 0) == d_list[i] and res.item(i, 1) == t_list[i] and res.item(i, 2) == dt_list[
                i] and res.item(i, 3) == ts_list[i]

        res, extra_result = table_obj.output(["c1", "c2"]).filter("c1='2024-09-23'").to_pl()
        assert res.item(0, 0) == d_list[0] and res.item(0, 1) == t_list[0]

        res, extra_result = table_obj.output(["*"]).filter("c2='21:44:33'").to_pl()
        assert res.item(0, 0) == d_list[1] and res.item(0, 1) == t_list[1] and res.item(0, 2) == dt_list[
            1] and res.item(0, 3) == ts_list[1]

        res, extra_result = table_obj.output(["*"]).filter("c3='2021-03-04 20:58:59'").to_pl()
        assert res.item(0, 0) == d_list[2] and res.item(0, 1) == t_list[2] and res.item(0, 2) == dt_list[
            2] and res.item(0, 3) == ts_list[2]

        res, extra_result = table_obj.output(["*"]).filter("c4='2021-03-04 20:58:59'").to_pl()
        assert res.item(0, 0) == d_list[2] and res.item(0, 1) == t_list[2] and res.item(0, 2) == dt_list[
            2] and res.item(0, 3) == ts_list[2]

        res = db_obj.drop_table("test_select_datetime" + suffix, ConflictType.Ignore)
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
        db_obj.drop_table("test_select_aggregate" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_select_aggregate" + suffix, {
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

        res, extra_result = table_obj.output(["count(*)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == 13
        res, extra_result = table_obj.output(["max(c1)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == 90
        res, extra_result = table_obj.output(["min(c2)"]).to_pl()
        assert res.height == 1 and res.width == 1 and res.item(0, 0) == -19
        res, extra_result = table_obj.output(["min(c1) + max(c2)"]).to_pl()
        print(res)
        res, extra_result = table_obj.output(["sum(c1)"]).to_pl()
        print(res)
        res, extra_result = table_obj.output(["avg(c2)"]).to_pl()
        print(res)

        res = db_obj.drop_table("test_select_aggregate" + suffix, ConflictType.Error)
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
        db_obj.drop_table("test_select_varchar" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_varchar" + suffix,
                            {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_varchar" + suffix)
        table_obj.insert(
            [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'},
             {"c1": 'e', "c2": 'e'}, {"c1": 'f', "c2": 'f'}, {
                 "c1": 'g', "c2": 'g'}, {"c1": 'h', "c2": 'h'},
             {"c1": 'i', "c2": 'i'}, {"c1": 'j', "c2": 'j'}, {
                 "c1": 'k', "c2": 'k'}, {"c1": 'l', "c2": 'l'},
             {"c1": 'm', "c2": 'm'}])
        res, extra_result = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                                'l', 'm'),
                                                         'c2': ('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                                'l', 'm')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res, extra_result = table_obj.output(
            ["c1", "c2"]).filter("c1 = 'a'").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('a',), 'c2': ('a',)}).astype(
            {'c1': dtype('O'), 'c2': dtype('O')}))

        # TODO NotImplement Error: Not implement: varchar > varchar
        # res, extra_result = table_obj.output(["c1"]).filter("c1 > 'a' and c2 < 'c'").to_df()
        # pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('b',)}).astype({'c1': dtype('O')}))
        res = db_obj.drop_table("test_select_varchar" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_big(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_select_big" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_big" + suffix, {
            "c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
            "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_big" + suffix)

        for i in range(1000):
            table_obj.insert(
                [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'}])

        res = db_obj.drop_table("test_select_big" + suffix, ConflictType.Error)
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

        db_obj.drop_table("test_select_embedding" + suffix, ConflictType.Ignore)

        res = db_obj.create_table("test_select_embedding" + suffix, {
            "c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_embedding" + suffix)

        if not check_data:
            copy_data("embedding_int_dim3.csv")
        test_dir = "/var/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir, None)
        assert res.error_code == ErrorCode.OK

        res, extra_result = table_obj.output(["c2"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c2': ([2, 3, 4], [6, 7, 8], [10, 11, 12])}))

        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (1, 5, 9), 'c2': ([2, 3, 4], [6, 7, 8], [10, 11, 12])})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('O')}))

        res = db_obj.drop_table("test_select_embedding" + suffix, ConflictType.Error)
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

        db_obj.drop_table("test_select_embedding_float" + suffix, ConflictType.Ignore)

        res = db_obj.create_table("test_select_embedding_float" + suffix, {
            "c1": {"type": "float"}, "c2": {"type": "vector,4,float"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_embedding_float" + suffix)

        test_dir = "/var/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_float_dim4.csv"
        if not check_data:
            copy_data("embedding_float_dim4.csv")
        assert os.path.exists(test_csv_dir)

        res = table_obj.import_data(test_csv_dir, None)
        assert res.error_code == ErrorCode.OK

        res, extra_result = table_obj.output(["c2"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c2': ([0.1, 0.2, 0.3, -0.2], [0.2, 0.1, 0.3, 0.4],
                    [0.3, 0.2, 0.1, 0.4], [0.4, 0.3, 0.2, 0.1])}))

        res, extra_result = table_obj.output(["*"]).to_df()
        print(res)

        pd.testing.assert_frame_equal(res,
                                      pd.DataFrame({'c1': (2, 4, 6, 8), 'c2': (
                                          [0.1, 0.2, 0.3, -0.2], [0.2,
                                                                  0.1, 0.3, 0.4],
                                          [0.3, 0.2, 0.1, 0.4], [0.4, 0.3, 0.2, 0.1])})
                                      .astype({'c1': dtype('float32'), 'c2': dtype('O')}))

        res = db_obj.drop_table(
            "test_select_embedding_float" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip(reason = "import conflict with compact")
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

        db_obj.drop_table("test_select_big_embedding" + suffix, ConflictType.Ignore)

        db_obj.create_table("test_select_big_embedding" + suffix, {
            "c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_big_embedding" + suffix)

        if not check_data:
            copy_data("embedding_int_dim3.csv")
        test_dir = "/var/infinity/test_data/"
        test_csv_dir = test_dir + "embedding_int_dim3.csv"
        assert os.path.exists(test_csv_dir)

        for i in range(1000):
            res = table_obj.import_data(test_csv_dir, None)
            assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(
            "test_select_big_embedding" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    def test_select_same_output(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_same_output" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_same_output" + suffix, {
            "c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_select_same_output" + suffix)
        table_obj.insert([{"c1": 1, "c2": 2}])
        print()
        res, extra_result = table_obj.output(["c1", "c2"]).to_df()
        print(res)
        res, extra_result = table_obj.output(["c1", "c1"]).to_df()
        print(res)
        res, extra_result = table_obj.output(["c1", "c2", "c1"]).to_df()
        print(res)

        res = db_obj.drop_table("test_select_same_output" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    def test_empty_table(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_empty_table" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_empty_table" + suffix, {
            "c1": {"type": "int"}, "c2": {"type": "int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_empty_table" + suffix)
        print()
        res, extra_result = table_obj.output(["c1", "c2"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c2': ()}).astype(
            {'c1': dtype('int32'), 'c2': dtype('int32')}))
        res, extra_result = table_obj.output(["c1", "c1"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c1_2': ()}).astype(
            {'c1': dtype('int32'), 'c1_2': dtype('int32')}))
        res, extra_result = table_obj.output(["c1", "c2", "c1"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c2': (), 'c1_2': ()}).astype(
            {'c1': dtype('int32'), 'c2': dtype('int32'), 'c1_2': dtype('int32')}))

        res = db_obj.drop_table("test_empty_table" + suffix, ConflictType.Error)
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
        db_obj.drop_table("test_valid_filter_expression" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_valid_filter_expression" + suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])
        # TODO add more filter function
        res, extra_result = table_obj.output(["*"]).filter(filter_list).to_pl()
        print(str(res))

        res = db_obj.drop_table(
            "test_valid_filter_expression" + suffix, ConflictType.Error)
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
        db_obj.drop_table("test_invalid_filter_expression" + suffix,
                          ConflictType.Ignore)
        table_obj = db_obj.create_table("test_invalid_filter_expression" + suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])
        # TODO add more filter function
        with pytest.raises(Exception):
            res, extra_result = table_obj.output(["*"]).filter(filter_list).to_pl()
            print(str(res))

        res = db_obj.drop_table(
            "test_invalid_filter_expression" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_filter_fulltext(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_filter_fulltext" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_filter_fulltext" + suffix,
                                        {"num": {"type": "int"}, "doc": {"type": "varchar"}}, ConflictType.Error)
        table_obj.insert([{"num": 1, "doc": "first text"}, {"num": 2, "doc": "second text multiple"},
                          {"num": 3, "doc": "third text many words"}])
        table_obj.create_index("my_ft_index", index.IndexInfo("doc", index.IndexType.FullText), ConflictType.Error)

        def test_func():
            expect_result = pd.DataFrame({'num': (1,), "doc": "first text"}).astype({'num': dtype('int32')})
            res, extra_result = table_obj.output(["*"]).filter(
                "filter_text('doc', 'first text', 'minimum_should_match=100%')").to_df()
            pd.testing.assert_frame_equal(expect_result, res)

            res, extra_result = table_obj.output(["*"]).filter(
                "filter_text('', 'first second', 'default_field=doc;minimum_should_match=99%') and not num = 2").to_df()
            pd.testing.assert_frame_equal(expect_result, res)

            res, extra_result = table_obj.output(["*"]).filter(
                "filter_text('doc', 'first OR second') and (num < 2 or num > 2)").to_df()
            pd.testing.assert_frame_equal(expect_result, res)

            res, extra_result = table_obj.output(["*"]).filter(
                "(filter_text('doc', 'first') or filter_fulltext('doc', 'second')) and (num < 2 or num > 2)").to_df()
            pd.testing.assert_frame_equal(expect_result, res)
            expect_result = pd.DataFrame(
                {'num': (1, 2, 3), "doc": ("first text", "second text multiple", "third text many words")}).astype(
                {'num': dtype('int32')})

            res, extra_result = table_obj.output(["*"]).filter(
                "filter_text('doc', 'first') or num >= 2").to_df()
            pd.testing.assert_frame_equal(expect_result, res)

            res, extra_result = table_obj.output(["*"]).filter(
                "filter_fulltext('doc', 'second') or (num < 2 or num > 2)").to_df()
            pd.testing.assert_frame_equal(expect_result, res)

            res, extra_result = table_obj.output(
                ["filter_text('doc', 'second') or num > 2", "filter_text('doc', 'second')"]).to_df()
            pd.testing.assert_frame_equal(pd.DataFrame({
                "(FILTER_FULLTEXT('doc', 'second') OR (num > 2))": (False, True, True),
                "FILTER_FULLTEXT('doc', 'second')": (False, True, False)}),
                res)

        test_func()
        table_obj.create_index("my_sc_index", index.IndexInfo("num", index.IndexType.Secondary), ConflictType.Error)
        test_func()
        res = db_obj.drop_table("test_filter_fulltext" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_neg_func(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_neg_func" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_neg_func" + suffix, {"num": {"type": "float64"}}, ConflictType.Error)
        table_obj.insert([{"num": 1.0}, {"num": 2.0}, {"num": 3.0}])

        res, extra_result = table_obj.output(["-abs(num) - 1"]).filter("-abs(num) >= -2").to_df()
        pd.testing.assert_frame_equal(res,
                                      pd.DataFrame({"(-(ABS(num)) - 1)": (-2.0, -3.0)}))
        res = db_obj.drop_table("test_neg_func" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_sort(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        # infinity
        db_obj.drop_table("test_sort" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_sort" + suffix, {
                "c1": {"type": "int", "constraints": ["primary key", "not null"]},
                "c2": {"type": "int", "constraints": ["not null"]}}, ConflictType.Error)

        assert table_obj is not None

        res = table_obj.insert(
            [{"c1": -3, "c2": 3}, {"c1": -2, "c2": 2}, {"c1": -1, "c2": 1}, {"c1": 0, "c2": 0}, {"c1": 1, "c2": 1},
             {"c1": 2, "c2": 2}, {"c1": 3, "c2": 3}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert(
            [{"c1": -8, "c2": 8}, {"c1": -7, "c2": 7}, {"c1": -6, "c2": 6}, {"c1": 7, "c2": 7}, {"c1": 8, "c2": 8},
             {"c1": 9, "c2": 9}])
        assert res.error_code == ErrorCode.OK

        res, extra_res = table_obj.output(["c1", "c2"]).sort([["c2", SortType.Asc], ["c1", SortType.Desc]]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (0, 1, -1, 2, -2, 3, -3, -6, 7, -7, 8, -8, 9),
                                                         'c2': (0, 1, 1, 2, 2, 3, 3, 6, 7, 7, 8, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_res = table_obj.output(["c1", "c2"]).sort([["c2", SortType.Asc], ["c1", SortType.Asc]]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (0, -1, 1, -2, 2, -3, 3, -6, -7, 7, -8, 8, 9),
                                                         'c2': (0, 1, 1, 2, 2, 3, 3, 6, 7, 7, 8, 8, 9)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_res = table_obj.output(["_row_id"]).sort([["_row_id", SortType.Desc]]).to_df()
        # pd.testing.assert_frame_equal(res, pd.DataFrame({'ROW_ID': (12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)})
        #                              .astype({'ROW_ID': dtype('int64')}))
        print(res)

        res = db_obj.drop_table("test_sort" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_select_varchar_length(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_varchar_length" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_varchar_length" + suffix,
                            {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_varchar_length" + suffix)
        table_obj.insert(
            [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'},
             {"c1": 'abc', "c2": 'abc'}, {"c1": 'bbc', "c2": 'bbc'}, {"c1": 'cbc', "c2": 'cbc'},
             {"c1": 'dbc', "c2": 'dbc'}])

        res, extra_res = table_obj.output(["*"]).filter("char_length(c1) = 1").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('a', 'b', 'c', 'd'),
                                                         'c2': ('a', 'b', 'c', 'd')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res, extra_res = table_obj.output(["*"]).filter("char_length(c1) = 3").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('abc', 'bbc', 'cbc', 'dbc'),
                                                         'c2': ('abc', 'bbc', 'cbc', 'dbc')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res = db_obj.drop_table("test_select_varchar_length" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_regex(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_regex" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_regex" + suffix,
                            {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_regex" + suffix)
        table_obj.insert(
            [{"c1": 'a', "c2": 'a'}, {"c1": 'b', "c2": 'b'}, {"c1": 'c', "c2": 'c'}, {"c1": 'd', "c2": 'd'},
             {"c1": 'abc', "c2": 'abc'}, {"c1": 'bbc', "c2": 'bbc'}, {"c1": 'cbc', "c2": 'cbc'},
             {"c1": 'dbc', "c2": 'dbc'}, ])

        res, extra_res = table_obj.output(["*"]).filter("regex(c1, 'bc')").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('abc', 'bbc', 'cbc', 'dbc'),
                                                         'c2': ('abc', 'bbc', 'cbc', 'dbc')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res = db_obj.drop_table("test_select_regex" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_upper_lower(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_upper_lower" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_upper_lower" + suffix,
                            {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_upper_lower" + suffix)
        table_obj.insert(
            [{"c1": 'a', "c2": 'A'}, {"c1": 'b', "c2": 'B'}, {"c1": 'c', "c2": 'C'}, {"c1": 'd', "c2": 'D'},
             {"c1": 'abc', "c2": 'ABC'}, {"c1": 'bbc', "c2": 'bbc'}, {"c1": 'cbc', "c2": 'cbc'},
             {"c1": 'dbc', "c2": 'dbc'}, ])

        res, extra_res = table_obj.output(["*"]).filter("upper(c1) = c2").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('a', 'b', 'c', 'd', 'abc'),
                                                         'c2': ('A', 'B', 'C', 'D', 'ABC')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res = db_obj.drop_table("test_select_upper_lower" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_substring(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_substring" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_substring" + suffix,
                            {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_substring" + suffix)
        table_obj.insert(
            [{"c1": 'a', "c2": 'A'}, {"c1": 'b', "c2": 'B'}, {"c1": 'c', "c2": 'C'}, {"c1": 'd', "c2": 'D'},
             {"c1": 'abc', "c2": 'ABC'}, {"c1": 'bbcc', "c2": 'bbc'}, {"c1": 'cbcc', "c2": 'cbc'},
             {"c1": 'dbcc', "c2": 'dbc'}, ])

        res, extra_res = table_obj.output(["*"]).filter("substring(c1, 0, 3) = c2").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('bbcc', 'cbcc', 'dbcc'),
                                                         'c2': ('bbc', 'cbc', 'dbc')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res = db_obj.drop_table("test_select_substring" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_trim(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_trim" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_trim" + suffix,
                            {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_trim" + suffix)
        table_obj.insert(
            [{"c1": ' a', "c2": 'a'}, {"c1": ' b', "c2": 'b'}, {"c1": ' c', "c2": 'c'},
             {"c1": 'ab ', "c2": 'ab'}, {"c1": 'bcc ', "c2": 'bcc'}, {"c1": 'cbc ', "c2": 'cbc'},
             {"c1": ' dbc ', "c2": 'dbc'}, ])

        res, extra_res = table_obj.output(["*"]).filter("ltrim(c1) = c2").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (' a', ' b', ' c'),
                                                         'c2': ('a', 'b', 'c')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res, extra_res = table_obj.output(["*"]).filter("rtrim(c1) = c2").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('ab ', 'bcc ', 'cbc '),
                                                         'c2': ('ab', 'bcc', 'cbc')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res, extra_res = table_obj.output(["*"]).filter("trim(c1) = c2").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (' a', ' b', ' c', 'ab ', 'bcc ', 'cbc ', ' dbc '),
                                                         'c2': ('a', 'b', 'c', 'ab', 'bcc', 'cbc', 'dbc')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res = db_obj.drop_table("test_select_trim" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_position(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_position" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_position" + suffix,
                            {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_position" + suffix)
        table_obj.insert(
            [{"c1": 'a', "c2": 'A'}, {"c1": 'b', "c2": 'B'}, {"c1": 'c', "c2": 'C'}, {"c1": 'd', "c2": 'D'},
             {"c1": 'abc', "c2": 'ABC'}, {"c1": 'bbcc', "c2": 'bbc'}, {"c1": 'cbcc', "c2": 'cbc'},
             {"c1": 'dbcc', "c2": 'dbc'}, ])

        res, extra_res = table_obj.output(["*"]).filter("char_position(c1, c2) <> 0").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ('bbcc', 'cbcc', 'dbcc'),
                                                         'c2': ('bbc', 'cbc', 'dbc')})
                                      .astype({'c1': dtype('O'), 'c2': dtype('O')}))

        res = db_obj.drop_table("test_select_position" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_sqrt(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_sqrt" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_sqrt" + suffix,
                            {"c1": {"type": "integer"},
                             "c2": {"type": "double"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_sqrt" + suffix)
        table_obj.insert(
            [{"c1": '1', "c2": '2'}, {"c1": '4', "c2": '5'}, {"c1": '9', "c2": '10'}, {"c1": '16', "c2": '17'}])

        res, extra_res = table_obj.output(["*", "sqrt(c1)", "sqrt(c2)"]).to_df()
        print(res)

        res, extra_res = table_obj.output(["*"]).filter("sqrt(c1) = 2").to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (4,),
                                                         'c2': (5,)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('double')}))

        res = db_obj.drop_table("test_select_sqrt" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_round(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_round" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_round" + suffix,
                            {"c1": {"type": "integer"},
                             "c2": {"type": "double"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_round" + suffix)
        table_obj.insert(
            [{"c1": '1', "c2": '2.4'}, {"c1": '4', "c2": '-2.4'}, {"c1": '9', "c2": '2.5'}, {"c1": '16', "c2": '-2.5'}])

        res, extra_res = table_obj.output(["c1", "round(c2)"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (1, 4, 9, 16),
                                                         'round(c2)': (2, -2, 3, -3)})
                                      .astype({'c1': dtype('int32'), 'round(c2)': dtype('double')}))

        res, extra_res = table_obj.output(["c1", "ceil(c2)"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (1, 4, 9, 16),
                                                         'ceil(c2)': (3, -2, 3, -2)})
                                      .astype({'c1': dtype('int32'), 'ceil(c2)': dtype('double')}))

        res, extra_res = table_obj.output(["c1", "floor(c2)"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (1, 4, 9, 16),
                                                         'floor(c2)': (2, -3, 2, -3)})
                                      .astype({'c1': dtype('int32'), 'floor(c2)': dtype('double')}))

        res = db_obj.drop_table("test_select_round" + suffix)
        assert res.error_code == ErrorCode.OK
    
    def test_select_truncate(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_truncate" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_truncate" + suffix,
                            {"c1": {"type": "double"},
                             "c2": {"type": "double"},
                             "c3": {"type": "float"}
                             }, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_truncate" + suffix)
        table_obj.insert(
            [{"c1": "2.123", "c2": "2.123", "c3": "2.123"}, {"c1": "-2.123", "c2": "-2.123", "c3": "-2.123"}, {"c1": "2", "c2": "2", "c3": "2"}, {"c1": "2.1", "c2":" 2.1", "c3": "2.1"}])

        res, extra_res = table_obj.output(["trunc(c1, 14)",  "trunc(c2, 2)", "trunc(c3, 2)"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'(c1 trunc 14)': ("2.12300000000000", "-2.12300000000000", "2.00000000000000", "2.10000000000000"),
                                                         '(c2 trunc 2)': ("2.12", "-2.12", "2.00", "2.10"),
                                                         '(c3 trunc 2)': ("2.12", "-2.12", "2.00", "2.10")
                                                         })
                                      .astype({'(c1 trunc 14)': dtype('object'), '(c2 trunc 2)': dtype('object'), '(c3 trunc 2)': dtype('object')}))


        res = db_obj.drop_table("test_select_truncate" + suffix)
        assert res.error_code == ErrorCode.OK


    def test_select_reverse(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_reverse" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_reverse" + suffix,
                            {"c1": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c2": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_reverse" + suffix)
        table_obj.insert(
            [{"c1": 'abc', "c2": 'ABC'}, {"c1": 'a123', "c2": 'a123'}, {"c1": 'c', "c2": 'C'}, {"c1": 'abcdefghijklmn', "c2": 'ABCDEFGHIJKLMN'}])

        res, extra_res = table_obj.output(["reverse(c1)", "reverse(c2)"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'reverse(c1)': ('cba', '321a', 'c', 'nmlkjihgfedcba'),
                                                         'reverse(c2)': ('CBA', '321a', 'C', 'NMLKJIHGFEDCBA')})
                                      .astype({'reverse(c1)': dtype('str_'), 'reverse(c2)': dtype('str_')}))

        res = db_obj.drop_table("test_select_reverse" + suffix)
        assert res.error_code == ErrorCode.OK


    def test_select_year(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_year" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_select_year" + suffix, {
                "c1": {"type": "date"},
                "c2": {"type": "datetime"},
                "c3": {"type": "timestamp"}},
            ConflictType.Error)
        table_obj = db_obj.get_table("test_select_year" + suffix)
        table_obj.insert(
            [{"c1": "2024-09-23", "c2":  "2022-05-26 21:44:33", "c3":"2024-09-23 20:45:11"}])

        res, extra_res = table_obj.output(["year(c1)", "year(c2)", "year(c3)"]).to_pl()
        print(res)
        assert res['year(c1)'][0] == 2024, "The value of year(c1) should be 2024"
        assert res['year(c2)'][0] == 2022, "The value of year(c2) should be 2022"
        assert res['year(c3)'][0] == 2024, "The value of year(c3) should be 2024"

        res = db_obj.drop_table("test_select_year" + suffix)
        assert res.error_code == ErrorCode.OK


    def test_select_month(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_month" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_select_month" + suffix, {
                "c1": {"type": "date"},
                "c2": {"type": "datetime"},
                "c3": {"type": "timestamp"}},
            ConflictType.Error)
        table_obj = db_obj.get_table("test_select_month" + suffix)
        table_obj.insert(
            [{"c1": "2024-09-23", "c2":  "2022-05-26 21:44:33", "c3":"2024-09-23 20:45:11"}])

        res, extra_res = table_obj.output(["month(c1)", "month(c2)", "month(c3)"]).to_pl()
        print(res)
        assert res['month(c1)'][0] == 9, "The value of month(c1) should be 9"
        assert res['month(c2)'][0] == 5, "The value of month(c2) should be 5"
        assert res['month(c3)'][0] == 9, "The value of month(c3) should be 9"
        res = db_obj.drop_table("test_select_month" + suffix)
        assert res.error_code == ErrorCode.OK


    def test_select_day(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_day" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_day" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_day" + suffix)
        table_obj.insert(
            [{"c1": "2024-09-23", "c2":  "2022-05-26 21:44:33", "c3":"2024-09-23 20:45:11"}])

        res, extra_res = table_obj.output(["day(c1)", "day(c2)", "day(c3)"]).to_pl()
        print(res)
        assert res['day(c1)'][0] == 23, "The value of day(c1) should be 23"
        assert res['day(c2)'][0] == 26, "The value of day(c2) should be 26"
        assert res['day(c3)'][0] == 23, "The value of day(c3) should be 23"
        res = db_obj.drop_table("test_select_day" + suffix)
        assert res.error_code == ErrorCode.OK

    
    def test_select_hour(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_hour" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_hour" + suffix,
                            {"c1": {"type": "time"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_hour" + suffix)
        table_obj.insert(
            [{"c1":"0:0:0", "c2":  "2022-05-26 21:44:33", "c3":"2024-09-23 20:45:11"}])

        res, extra_res = table_obj.output(["hour(c1)" ,"hour(c2)", "hour(c3)"]).to_pl()
        print(res)
        assert res['hour(c1)'][0] == 0, "The value of hour(c1) should be 0"
        assert res['hour(c2)'][0] == 21, "The value of hour(c2) should be 21"
        assert res['hour(c3)'][0] == 20, "The value of hour(c3) should be 20"

        res = db_obj.drop_table("test_select_hour" + suffix)
        assert res.error_code == ErrorCode.OK
    
    def test_select_minute(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_minute" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_minute" + suffix,
                            {"c1": {"type": "time"},
                             "c2": {"type": "datetime"},"c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_minute" + suffix)
        table_obj.insert(
            [{"c1":"0:0:0", "c2":  "2022-05-26 21:44:33", "c3":"2024-09-23 20:45:11"}])

        res, extra_res = table_obj.output(["minute(c1)", "minute(c2)", "minute(c3)"]).to_pl()
        print(res)
        assert res['minute(c1)'][0] == 0, "The value of minute(c1) should be 0"
        assert res['minute(c2)'][0] == 44, "The value of minute(c2) should be 44"
        assert res['minute(c3)'][0] == 45, "The value of minute(c3) should be 45"
        
        res = db_obj.drop_table("test_select_minute" + suffix)
        assert res.error_code == ErrorCode.OK
    
    def test_select_second(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_second" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_second" + suffix,
                            {"c1": {"type": "time"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_second" + suffix)
        table_obj.insert(
            [{"c1":"0:0:0", "c2":  "2022-05-26 21:44:33", "c3":"2024-09-23 20:45:11"}])

        res, extra_res = table_obj.output(["second(c1)", "second(c2)", "second(c3)"]).to_pl()
        print(res)
        assert res['second(c1)'][0] == 0, "The value of second(c1) should be 0"
        assert res['second(c2)'][0] == 33, "The value of second(c2) should be 33"
        assert res['second(c3)'][0] == 11, "The value of second(c3) should be 11"

        res = db_obj.drop_table("test_select_second" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_day_of_month(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_day_of_month" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_day_of_month" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_day_of_month" + suffix)
        table_obj.insert(
            [{"c1":"2024-09-23", "c2":  "2022-05-26 21:44:33", "c3":"2024-09-23 20:45:11"}])

        res, extra_res = table_obj.output(["dayofmonth(c1)", "dayofmonth(c2)", "dayofmonth(c3)"]).to_pl()
        print(res)
        assert res['dayofmonth(c1)'][0] == 23, "The value of day_of_month(c1) should be 23"
        assert res['dayofmonth(c2)'][0] == 26, "The value of day_of_monthc2) should be 26"
        assert res['dayofmonth(c3)'][0] == 23, "The value of day_of_month(c3) should be 23"

        res = db_obj.drop_table("test_select_day_of_month" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_day_of_year(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_day_of_year" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_day_of_year" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_day_of_year" + suffix)
        table_obj.insert(
            [{"c1":"2024-09-23", "c2":  "2022-05-26 21:44:33", "c3":"2024-09-23 20:45:11"}])

        res, extra_res = table_obj.output(["dayofyear(c1)", "dayofyear(c2)", "dayofyear(c3)"]).to_pl()
        print(res)
        assert res['dayofyear(c1)'][0] == 267, "The value of day_of_year(c1) should be 267"
        assert res['dayofyear(c2)'][0] == 146, "The value of day_of_yearc2) should be 146"
        assert res['dayofyear(c3)'][0] == 267, "The value of day_of_year(c3) should be 267"

        res = db_obj.drop_table("test_select_day_of_year" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_day_of_week(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_day_of_week" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_day_of_week" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_day_of_week" + suffix)
        table_obj.insert(
            [{"c1":"2025-01-16", "c2":  "2025-01-16 21:44:33", "c3":"2025-01-16 20:45:11"}])

        res, extra_res = table_obj.output(["dayofweek(c1)", "dayofweek(c2)", "dayofweek(c3)"]).to_pl()
        print(res)
        assert res['dayofweek(c1)'][0] == 4, "The value of day_of_week(c1) should be 4"
        assert res['dayofweek(c2)'][0] == 4, "The value of day_of_week(c2) should be 4"
        assert res['dayofweek(c3)'][0] == 4, "The value of day_of_week(c3) should be 4"

        res = db_obj.drop_table("test_select_day_of_week" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_weekday(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_weekday" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_weekday" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_weekday" + suffix)
        table_obj.insert(
            [{"c1":"2025-01-16", "c2":  "2025-01-31 21:44:33", "c3":"2025-01-26 20:45:11"}])

        res, extra_res = table_obj.output(["weekday(c1)", "weekday(c2)", "weekday(c3)"]).to_pl()
        print(res)
        assert res['weekday(c1)'][0] == 4, "The value of weekday(c1) should be 4"
        assert res['weekday(c2)'][0] == 5, "The value of weekday(c2) should be 5"
        assert res['weekday(c3)'][0] == 0, "The value of weekday(c3) should be 0"

        res = db_obj.drop_table("test_select_weekday" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_era(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_era" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_era" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_era" + suffix)
        table_obj.insert(
            [{"c1":"2025-01-16", "c2":  "44-01-16 21:44:33", "c3":"-25-01-16 20:45:11"}])

        res, extra_res = table_obj.output(["era(c1)", "era(c2)", "era(c3)"]).to_pl()
        print(res)
        assert res['era(c1)'][0] == 1, "The value of era(c1) should be 1"
        assert res['era(c2)'][0] == 1, "The value of era(c2) should be 1"
        assert res['era(c3)'][0] == 0, "The value of era(c3) should be 0"

        res = db_obj.drop_table("test_select_era" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_epoch(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_epoch" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_epoch" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_epoch" + suffix)
        table_obj.insert(
            [{"c1":"1970-01-01", "c2":  "1970-01-01 21:44:33", "c3":"1970-01-01 20:45:11"}])

        res, extra_res = table_obj.output(["epoch(c1)", "epoch(c2)", "epoch(c3)"]).to_pl()
        print(res)
        assert res['epoch(c1)'][0] == 0, "The value of epoch(c1) should be 0"
        assert res['epoch(c2)'][0] == 78273, "The value of epoch(c2) should be 78273"
        assert res['epoch(c3)'][0] == 74711 , "The value of epoch(c3) should be 74711"

        res = db_obj.drop_table("test_select_epoch" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_quarter(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_quarter" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_quarter" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_quarter" + suffix)
        table_obj.insert(
            [{"c1":"2025-01-16", "c2":  "2025-04-16 21:44:33", "c3":"2025-09-16 20:45:11"}])

        res, extra_res = table_obj.output(["quarter(c1)", "quarter(c2)", "quarter(c3)"]).to_pl()
        print(res)
        assert res['quarter(c1)'][0] == 1, "The value of quarter(c1) should be 1"
        assert res['quarter(c2)'][0] == 2, "The value of quarter(c2) should be 2"
        assert res['quarter(c3)'][0] == 3, "The value of quarter(c3) should be 3"

        res = db_obj.drop_table("test_select_quarter" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_century(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_century" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_century" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_century" + suffix)
        table_obj.insert(
            [{"c1":"2025-01-16", "c2":  "1925-01-16 21:44:33", "c3":"25-01-16 20:45:11"}])

        res, extra_res = table_obj.output(["century(c1)", "century(c2)", "century(c3)"]).to_pl()
        print(res)
        assert res['century(c1)'][0] == 21, "The value of century(c1) should be 21"
        assert res['century(c2)'][0] == 20, "The value of century(c2) should be 20"
        assert res['century(c3)'][0] == 1, "The value of century(c3) should be 1"

        res = db_obj.drop_table("test_select_century" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_week_of_year(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_week_of_year" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_week_of_year" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_week_of_year" + suffix)
        table_obj.insert(
            [{"c1":"2025-01-16", "c2":  "2025-01-01 21:44:33", "c3":"2017-12-16 20:45:11"}])

        res, extra_res = table_obj.output(["weekofyear(c1)", "weekofyear(c2)", "weekofyear(c3)"]).to_pl()
        print(res)
        assert res['weekofyear(c1)'][0] == 3, "The value of weekofyear(c1) should be 3"
        assert res['weekofyear(c2)'][0] == 1, "The value of weekofyear(c2) should be 1"
        assert res['weekofyear(c3)'][0] == 50, "The value of weekofyear(c3) should be 50"

        res = db_obj.drop_table("test_select_week_of_year" + suffix)
        assert res.error_code == ErrorCode.OK

    def test_select_date_part(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_date_part" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_select_date_part" + suffix,
                            {"c1": {"type": "date"},
                             "c2": {"type": "datetime"},
                             "c3": {"type": "timestamp"},
                             "c4": {"type": "varchar", "constraints": ["primary key", "not null"]},
                             "c5": {"type": "varchar", "constraints": ["not null"]}}, ConflictType.Error)
        table_obj = db_obj.get_table("test_select_date_part" + suffix)
        table_obj.insert(
            [{"c1":"2025-01-16", "c2":  "2025-01-16 21:44:33", "c3":"2025-01-16 20:45:11", "c4":"year", "c5":"month"}])

        res, extra_res = table_obj.output(["datepart(c4, c1)", "datepart(c4, c2)", "datepart(c5, c3)"]).to_pl()
        print(res)
        assert res['(c4 datepart c1)'][0] == 2025, "The value of c4 datepart c1 should be 2025"
        assert res['(c4 datepart c2)'][0] == 2025, "The value of c4 datepart c2 should be 2025"
        assert res['(c5 datepart c3)'][0] == 1, "The value of c5 datepart c3 should be 1"

        res = db_obj.drop_table("test_select_date_part" + suffix)
        assert res.error_code == ErrorCode.OK
