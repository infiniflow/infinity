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
    def test_select_group_by_aggtype(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        table_name = "test_select_simple_groupby" + suffix
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
        res, extra_result = (
            table_obj.output(["c1", "sum(c2)"]).group_by("c1")
            # .sort([["c1", SortType.Asc]])
            .to_df()
        )
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
