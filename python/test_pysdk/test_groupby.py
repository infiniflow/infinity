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
        func = getattr(module, "ConflictType")
        globals()["ConflictType"] = func
        func = getattr(module, "InfinityException")
        globals()["InfinityException"] = func
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

    # test/sql/dql/aggregate/test_groupby_aggtype.slt
    def test_groupby_aggtype(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        table_name = "test_simple_groupby" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            table_name,
            {
                "c1": {"type": "int"},
                "c2": {"type": "float"},
            },
        )
        table_obj.insert(
            [
                {"c1": 1, "c2": 1.0},
                {"c1": 2, "c2": 2.0},
                {"c1": 1, "c2": 3.0},
                {"c1": 2, "c2": 4.0},
                {"c1": 1, "c2": 5.0},
            ]
        )

        # test agg sum
        res, extra_result = table_obj.output(["c1", "sum(c2)"]).group_by("c1").to_df()
        gt = pd.DataFrame({"c1": (1, 2), "sum(c2)": (9.0, 6.0)}).astype(
            {"c1": dtype("int32"), "sum(c2)": dtype("float64")}
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        # test agg min
        res, extra_result = table_obj.output(["c1", "min(c2)"]).group_by("c1").to_df()
        gt = pd.DataFrame({"c1": (1, 2), "min(c2)": (1.0, 2.0)}).astype(
            {"c1": dtype("int32"), "min(c2)": dtype("float32")}
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        # test agg max
        res, extra_result = table_obj.output(["c1", "max(c2)"]).group_by("c1").to_df()
        gt = pd.DataFrame({"c1": (1, 2), "max(c2)": (5.0, 4.0)}).astype(
            {"c1": dtype("int32"), "max(c2)": dtype("float32")}
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        # test agg count
        res, extra_result = table_obj.output(["c1", "count(*)"]).group_by("c1").to_df()
        gt = pd.DataFrame({"c1": (1, 2), "count(star)": (3, 2)}).astype(
            {"c1": dtype("int32"), "count(star)": dtype("int64")}
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        # test agg avg
        res, extra_result = table_obj.output(["c1", "avg(c2)"]).group_by("c1").to_df()
        gt = pd.DataFrame({"c1": (1, 2), "(sum(c2) / count(c2))": (3.0, 3.0)}).astype(
            {"c1": dtype("int32"), "(sum(c2) / count(c2))": dtype("float64")}
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

    # test/sql/dql/aggregate/test_groupby_datatype.slt
    def test_groupby_datatype(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        table_name = "test_simple_groupby" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            table_name,
            {
                "c1": {"type": "int"},
                "c2": {"type": "float"},
                "c3": {"type": "varchar"},
            },
        )
        table_obj.insert(
            [
                {"c1": 1, "c2": 1.0, "c3": "abc"},
                {"c1": 2, "c2": 2.0, "c3": "abcdef"},
                {"c1": 3, "c2": 3.0, "c3": "abcdefghi"},
                {"c1": 1, "c2": 4.0, "c3": "abcdefghijkl"},
                {"c1": 2, "c2": 5.0, "c3": "abcdefghijklmno"},
                {"c1": 3, "c2": 6.0, "c3": "abcdefghijklmnopqr"},
                {"c1": 1, "c2": 1.0, "c3": "abcdefghijklmnopqrstu"},
                {"c1": 2, "c2": 2.0, "c3": "abcdefghijklmnopqrstuvwx"},
                {"c1": 3, "c2": 3.0, "c3": "abcdefghijklmnopqrstuvwxyz"},
                {"c1": 1, "c2": 4.0, "c3": "abc"},
                {"c1": 2, "c2": 5.0, "c3": "abcdef"},
                {"c1": 3, "c2": 6.0, "c3": "abcdefghi"},
                {"c1": 1, "c2": 1.0, "c3": "abcdefghijkl"},
                {"c1": 2, "c2": 2.0, "c3": "abcdefghijklmno"},
                {"c1": 3, "c2": 3.0, "c3": "abcdefghijklmnopqr"},
                {"c1": 1, "c2": 4.0, "c3": "abcdefghijklmnopqrstu"},
                {"c1": 2, "c2": 5.0, "c3": "abcdefghijklmnopqrstuvwx"},
                {"c1": 3, "c2": 6.0, "c3": "abcdefghijklmnopqrstuvwxyz"},
            ]
        )

        res, extra_result = (
            table_obj.output(["c3", "sum(c1)", "sum(c2)"]).group_by("c3").to_df()
        )
        gt = pd.DataFrame(
            {
                "c3": [
                    "abc",
                    "abcdef",
                    "abcdefghi",
                    "abcdefghijkl",
                    "abcdefghijklmno",
                    "abcdefghijklmnopqr",
                    "abcdefghijklmnopqrstu",
                    "abcdefghijklmnopqrstuvwx",
                    "abcdefghijklmnopqrstuvwxyz",
                ],
                "sum(c1)": [2, 4, 6, 2, 4, 6, 2, 4, 6],
                "sum(c2)": [5.0, 7.0, 9.0, 5.0, 7.0, 9.0, 5.0, 7.0, 9.0],
            }
        ).astype(
            {
                "c3": dtype("object"),
                "sum(c1)": dtype("int64"),
                "sum(c2)": dtype("float64"),
            }
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = (
            table_obj.output(["c3", "char_length(c3)", "sum(c1)"])
            .group_by("c3")
            .to_df()
        )
        gt = pd.DataFrame(
            {
                "c3": [
                    "abc",
                    "abcdef",
                    "abcdefghi",
                    "abcdefghijkl",
                    "abcdefghijklmno",
                    "abcdefghijklmnopqr",
                    "abcdefghijklmnopqrstu",
                    "abcdefghijklmnopqrstuvwx",
                    "abcdefghijklmnopqrstuvwxyz",
                ],
                "char_length(c3)": [3, 6, 9, 12, 15, 18, 21, 24, 26],
                "sum(c1)": [2, 4, 6, 2, 4, 6, 2, 4, 6],
            }
        ).astype({"char_length(c3)": dtype("int32"), "sum(c1)": dtype("int64")})
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table(
            table_name,
            {
                "c1": {"type": "int"},
                "d": {"type": "date"},
                "dt": {"type": "datetime"},
                "t": {"type": "time"},
                "ts": {"type": "timestamp"},
            },
        )
        table_obj.insert(
            [
                {
                    "c1": 1,
                    "d": "1970-01-01",
                    "dt": "1970-01-01 00:00:00",
                    "t": "00:00:00",
                    "ts": "1970-01-01 00:00:00",
                },
                {
                    "c1": 2,
                    "d": "1970-01-01",
                    "dt": "1970-01-01 00:00:00",
                    "t": "11:59:59",
                    "ts": "1970-01-01 11:59:59",
                },
                {
                    "c1": 3,
                    "d": "1970-01-01",
                    "dt": "1970-01-01 00:00:00",
                    "t": "12:00:00",
                    "ts": "1970-01-01 12:00:00",
                },
                {
                    "c1": 4,
                    "d": "1970-01-01",
                    "dt": "1970-01-01 00:00:00",
                    "t": "23:59:59",
                    "ts": "1970-01-01 23:59:59",
                },
                {
                    "c1": 5,
                    "d": "1970-01-02",
                    "dt": "1970-01-02 00:00:00",
                    "t": "00:00:00",
                    "ts": "1970-01-01 00:00:00",
                },
                {
                    "c1": 6,
                    "d": "1970-01-02",
                    "dt": "1970-01-02 00:00:00",
                    "t": "11:59:59",
                    "ts": "1970-01-01 11:59:59",
                },
                {
                    "c1": 7,
                    "d": "1970-01-02",
                    "dt": "1970-01-02 00:00:00",
                    "t": "12:00:00",
                    "ts": "1970-01-01 12:00:00",
                },
                {
                    "c1": 8,
                    "d": "1970-01-02",
                    "dt": "1970-01-02 00:00:00",
                    "t": "23:59:59",
                    "ts": "1970-01-01 23:59:59",
                },
                {
                    "c1": 9,
                    "d": "1970-01-03",
                    "dt": "1970-01-03 00:00:00",
                    "t": "00:00:00",
                    "ts": "1970-01-01 00:00:00",
                },
                {
                    "c1": 10,
                    "d": "1970-01-03",
                    "dt": "1970-01-03 00:00:00",
                    "t": "11:59:59",
                    "ts": "1970-01-01 11:59:59",
                },
                {
                    "c1": 11,
                    "d": "1970-01-03",
                    "dt": "1970-01-03 00:00:00",
                    "t": "12:00:00",
                    "ts": "1970-01-01 12:00:00",
                },
                {
                    "c1": 12,
                    "d": "1970-01-03",
                    "dt": "1970-01-03 00:00:00",
                    "t": "23:59:59",
                    "ts": "1970-01-01 23:59:59",
                },
            ]
        )

        res, extra_result = table_obj.output(["d", "sum(c1)"]).group_by("d").to_df()
        gt = pd.DataFrame(
            {
                "d": [
                    "1970-01-01",
                    "1970-01-02",
                    "1970-01-03",
                ],
                "sum(c1)": [10, 26, 42],
            }
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = table_obj.output(["t", "sum(c1)"]).group_by("t").to_df()
        gt = pd.DataFrame(
            {
                "t": [
                    "00:00:00",
                    "11:59:59",
                    "12:00:00",
                    "23:59:59",
                ],
                "sum(c1)": [15, 18, 21, 24],
            }
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = table_obj.output(["dt", "sum(c1)"]).group_by("dt").to_df()
        gt = pd.DataFrame(
            {
                "dt": [
                    "1970-01-01 00:00:00",
                    "1970-01-02 00:00:00",
                    "1970-01-03 00:00:00",
                ],
                "sum(c1)": [10, 26, 42],
            }
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = table_obj.output(["ts", "sum(c1)"]).group_by("ts").to_df()
        gt = pd.DataFrame(
            {
                "ts": [
                    "1970-01-01 00:00:00",
                    "1970-01-01 11:59:59",
                    "1970-01-01 12:00:00",
                    "1970-01-01 23:59:59",
                ],
                "sum(c1)": [15, 18, 21, 24],
            }
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

    # test/sql/dql/aggregate/test_groupby_complex.slt
    def test_groupby_complex(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        table_name = "test_simple_groupby" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            table_name,
            {
                "c1": {"type": "int"},
                "c2": {"type": "int"},
                "c3": {"type": "float"},
            },
        )
        table_obj.insert(
            [
                {"c1": 1, "c2": 1, "c3": 1.0},
                {"c1": 2, "c2": 2, "c3": 2.0},
                {"c1": 1, "c2": 3, "c3": 3.0},
                {"c1": 2, "c2": 1, "c3": 4.0},
                {"c1": 1, "c2": 2, "c3": 5.0},
                {"c1": 2, "c2": 3, "c3": 6.0},
                {"c1": 1, "c2": 1, "c3": 7.0},
                {"c1": 2, "c2": 2, "c3": 8.0},
                {"c1": 1, "c2": 3, "c3": 1.0},
                {"c1": 2, "c2": 1, "c3": 2.0},
                {"c1": 1, "c2": 2, "c3": 3.0},
                {"c1": 2, "c2": 3, "c3": 4.0},
            ]
        )

        res, extra_result = (
            table_obj.output(["c1", "c2", "sum(c3)"]).group_by(["c1", "c2"]).to_df()
        )
        gt = pd.DataFrame(
            {
                "c1": [1, 1, 1, 2, 2, 2],
                "c2": [1, 2, 3, 1, 2, 3],
                "sum(c3)": [8.0, 8.0, 4.0, 6.0, 10.0, 10.0],
            }
        ).astype(
            {"c1": dtype("int32"), "c2": dtype("int32"), "sum(c3)": dtype("float64")}
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = (
            table_obj.output(["c1+c2", "sum(c3)"]).group_by(["c1+c2"]).to_df()
        )
        gt = pd.DataFrame(
            {
                "(c1 + c2)": [2, 3, 4, 5],
                "sum(c3)": [8.0, 14.0, 14.0, 10.0],
            }
        ).astype({"(c1 + c2)": dtype("int32"), "sum(c3)": dtype("float64")})
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = (
            table_obj.output(["abs(c1-c2)", "c1+c2", "sum(c3)"])
            .group_by(["abs(c1-c2)", "c1+c2"])
            .to_df()
        )
        gt = pd.DataFrame(
            {
                "abs((c1 - c2))": [0, 0, 1, 1, 2],
                "(c1 + c2)": [2, 4, 3, 5, 4],
                "sum(c3)": [8.0, 10.0, 14.0, 10.0, 4.0],
            }
        ).astype(
            {
                "abs((c1 - c2))": dtype("int32"),
                "(c1 + c2)": dtype("int32"),
                "sum(c3)": dtype("float64"),
            }
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = (
            table_obj.output(["c1", "c2", "sum(c3)"])
            .filter("c1 > 1")
            .group_by(["c1", "c2"])
            .to_df()
        )
        gt = pd.DataFrame(
            {
                "c1": [2, 2, 2],
                "c2": [1, 2, 3],
                "sum(c3)": [6.0, 10.0, 10.0],
            }
        ).astype(
            {"c1": dtype("int32"), "c2": dtype("int32"), "sum(c3)": dtype("float64")}
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = (
            table_obj.output(["c1", "c3", "sum(c2)"]).group_by(["c1", "c3"]).to_df()
        )
        gt = pd.DataFrame(
            {
                "c1": [1, 1, 1, 1, 2, 2, 2, 2],
                "c3": [1.0, 3.0, 5.0, 7.0, 2.0, 4.0, 6.0, 8.0],
                "sum(c2)": [4, 5, 2, 1, 3, 4, 3, 2],
            }
        ).astype(
            {"c1": dtype("int32"), "c3": dtype("float32"), "sum(c2)": dtype("int64")}
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = (
            table_obj.output(["c3", "sum(c1)", "sum(c2)"]).group_by("c3").to_df()
        )
        gt = pd.DataFrame(
            {
                "c3": [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0],
                "sum(c1)": [2, 4, 2, 4, 1, 2, 1, 2],
                "sum(c2)": [4, 3, 5, 4, 2, 3, 1, 2],
            }
        ).astype(
            {
                "c3": dtype("float32"),
                "sum(c1)": dtype("int64"),
                "sum(c2)": dtype("int64"),
            }
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = (
            table_obj.output(["c3", "count(c3)", "sum(c1)", "sum(c2)"])
            .group_by("c3")
            .to_df()
        )
        gt = pd.DataFrame(
            {
                "c3": [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0],
                "count(c3)": [2, 2, 2, 2, 1, 1, 1, 1],
                "sum(c1)": [2, 4, 2, 4, 1, 2, 1, 2],
                "sum(c2)": [4, 3, 5, 4, 2, 3, 1, 2],
            }
        ).astype(
            {
                "c3": dtype("float32"),
                "count(c3)": dtype("int64"),
                "sum(c1)": dtype("int64"),
                "sum(c2)": dtype("int64"),
            }
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res, extra_result = (
            table_obj.output(["c3", "sum(c1)", "sum(c2)"])
            .group_by("c3")
            .sort([["c3", SortType.Desc]])
            .to_df()
        )
        gt = pd.DataFrame(
            {
                "c3": [8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0],
                "sum(c1)": [2, 1, 2, 1, 4, 2, 4, 2],
                "sum(c2)": [2, 1, 3, 2, 4, 5, 3, 4],
            }
        ).astype(
            {
                "c3": dtype("float32"),
                "sum(c1)": dtype("int64"),
                "sum(c2)": dtype("int64"),
            }
        )
        pd.testing.assert_frame_equal(res, gt)

        res, extra_result = (
            table_obj.output(["c3", "sum(c1) as sum1", "sum(c2) as sum2"])
            .group_by("c3")
            .sort([["sum1", SortType.Asc], ["sum2", SortType.Asc]])
            .to_df()
        )
        gt = pd.DataFrame(
            {
                "c3": [7.0, 5.0, 8.0, 6.0, 1.0, 3.0, 2.0, 4.0],
                "sum1": [1, 1, 2, 2, 2, 2, 4, 4],
                "sum2": [1, 2, 2, 3, 4, 5, 3, 4],
            }
        ).astype(
            {
                "c3": dtype("float32"),
                "sum1": dtype("int64"),
                "sum2": dtype("int64"),
            }
        )
        pd.testing.assert_frame_equal(res, gt)

        table_obj.delete("c1 <= 1")

        res, extra_result = (
            table_obj.output(["c1", "c2", "sum(c3)"])
            .group_by(["c1", "c2"])
            .sort([["c1", SortType.Asc], ["c2", SortType.Asc]])
            .to_df()
        )
        gt = pd.DataFrame(
            {
                "c1": [2, 2, 2],
                "c2": [1, 2, 3],
                "sum(c3)": [6.0, 10.0, 10.0],
            }
        ).astype(
            {"c1": dtype("int32"), "c2": dtype("int32"), "sum(c3)": dtype("float64")}
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True),
            gt.sort_values(by=gt.columns.tolist()).reset_index(drop=True),
        )

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
