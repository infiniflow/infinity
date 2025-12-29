import pandas as pd
import pytest
from common import common_values
import infinity
from numpy import dtype
from infinity.common import ConflictType, Array

from infinity.infinity_http import infinity_http

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
class TestInfinity:

    # test/sql/dql/unnest/test_unnest.slt
    @pytest.mark.usefixtures("skip_if_http")
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
        gt = pd.DataFrame({"unnest(c2)": [0, 0, 0, 1, 1, 2, 2, 2, 3, 3]}).astype('Int32')
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True), gt
        )

        res, extra_result = table_obj.output(["c1", "unnest(c2)"]).to_df()
        gt = pd.DataFrame(
            {
                "c1": [1, 1, 2, 2, 3, 3, 3, 4, 4, 4],
                "unnest(c2)": [0, 1, 2, 3, 0, 1, 2, 0, 2, 3],
            }
        ).astype({"c1": 'Int32', "unnest(c2)": 'Int32'})
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
        ).astype({"c1": 'Int32', "unnest(c2)": 'Int32'})
        pd.testing.assert_frame_equal(res, gt)

        res, extra_result = (
            table_obj.output(["c1", "unnest(c2) as uc2"]).filter("c1 > 2").to_df()
        )
        gt = pd.DataFrame(
            {
                "c1": [3, 3, 3, 4, 4, 4],
                "uc2": [0, 1, 2, 0, 2, 3],
            }
        ).astype({"c1": 'Int32', "uc2": 'Int32'})
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
        ).astype({"c1": 'Int32', "uc2": 'Int32'})
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
        ).astype({"uc2": 'Int32', "sum(c1)": 'Int64'})
        pd.testing.assert_frame_equal(
            res.sort_values(by=res.columns.tolist()).reset_index(drop=True), gt
        )

        db_obj.drop_table(table_name)
