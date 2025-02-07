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
from infinity.common import ConflictType, SortType, Array

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

    # test/sql/dql/unnest/test_unnest.slt
    def test_unnest(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        table_name = "test_unnest" + suffix
        db_obj.drop_table(table_name, conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table(
            table_name, {"c1": {"type": "int"}, "c2": {"type": "array, int"}}
        )
        table_obj.insert(
            [
                {"c1": 1, "c2": Array(0, 1)},
                {"c1": 2, "c2": Array(2, 3)},
                {"c1": 3, "c2": Array(0, 1, 2)},
                {"c1": 4, "c2": Array(0, 2, 3)},
                {"c1": 5, "c2": Array()},
            ]
        )

        res, extra_result = table_obj.output(["unnest(c2)"]).to_df()
        gt = pd.DataFrame({"unnest(c2)": [0, 0, 0, 1, 1, 2, 2, 2, 3, 3]}).astype(
            dtype("int32")
        )
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True), gt
        )

        res, extra_result = table_obj.output(["c1", "unnest(c2)"]).to_df()
        gt = pd.DataFrame(
            {
                "c1": [1, 1, 2, 2, 3, 3, 3, 4, 4, 4],
                "unnest(c2)": [0, 1, 2, 3, 0, 1, 2, 0, 2, 3],
            }
        ).astype({"c1": dtype("int32"), "unnest(c2)": dtype("int32")})
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True), gt
        )

        res, extra_result = table_obj.output(["c1", "c2", "unnest(c2)"]).to_df()
        gt = pd.DataFrame(
            {
                "c1": [1, 1, 2, 2, 3, 3, 3, 4, 4, 4],
                "c2": [
                    [0, 1],
                    [0, 1],
                    [2, 3],
                    [2, 3],
                    [0, 1, 2],
                    [0, 1, 2],
                    [0, 1, 2],
                    [0, 2, 3],
                    [0, 2, 3],
                    [0, 2, 3],
                ],
                "unnest(c2)": [0, 1, 2, 3, 0, 1, 2, 0, 2, 3],
            }
        ).astype({"c1": dtype("int32"), "unnest(c2)": dtype("int32")})
        pd.testing.assert_frame_equal(res, gt)

        res, extra_result = (
            table_obj.output(["c1", "unnest(c2) as uc2"]).filter("c1 > 2").to_df()
        )
        gt = pd.DataFrame(
            {
                "c1": [3, 3, 3, 4, 4, 4],
                "uc2": [0, 1, 2, 0, 2, 3],
            }
        ).astype({"c1": dtype("int32"), "uc2": dtype("int32")})
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True), gt
        )

        res, extra_result = (
            table_obj.output(["c1", "unnest(c2) as uc2"]).filter("uc2 > 1").to_df()
        )
        gt = pd.DataFrame(
            {
                "c1": [2, 2, 3, 4, 4],
                "uc2": [2, 3, 2, 2, 3],
            }
        ).astype({"c1": dtype("int32"), "uc2": dtype("int32")})
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True), gt
        )

        res, extra_result = (
            table_obj.output(["unnest(c2) as uc2", "sum(c1)"]).group_by("uc2").to_df()
        )
        gt = pd.DataFrame(
            {
                "uc2": [0, 1, 2, 3],
                "sum(c1)": [8, 4, 9, 6],
            }
        ).astype({"uc2": dtype("int32"), "sum(c1)": dtype("int64")})
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True), gt
        )

        db_obj.drop_table(table_name)
