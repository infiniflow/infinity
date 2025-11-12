import importlib
import sys
import os
import os
import pandas as pd
import pytest
from common import common_values
import infinity
import infinity.index as index
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
    def test_limit(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        # infinity
        db_obj.drop_table("test_limit" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_limit" + suffix, {
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

        res, extra_res = table_obj.output(["c1", "c2"]).limit(2).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-3, -2),
                                                         'c2': (3, 2)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_res = table_obj.output(["c1", "c2"]).limit(2).offset(2).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-1, 0),
                                                         'c2': (1, 0)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_res = table_obj.output(["c1", "c2"]).limit(2).offset(2).option({"total_hits_count": True}).to_df()
        assert extra_res['total_hits_count'] == 13

        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (-1, 0),
                                                         'c2': (1, 0)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res, extra_res = table_obj.output(["c1", "c2"]).sort([["c2", SortType.Asc], ["c1", SortType.Desc]]).limit(3).option({"total_hits_count": True}).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (0, 1, -1),
                                                         'c2': (0, 1, 1)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))
        assert extra_res['total_hits_count'] == 13

        res = db_obj.drop_table("test_limit" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_limit_2block(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        table_name = "ragspace_kb_bcc87f1cd48711ef8ed510ffe02aa993" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            table_name,
            {
                "doc_id": {"type": "varchar"},
                "knowledge_graph_kwd": {"type": "varchar", "default": ""},
                "page_num_int": {"type": "int"},
                "top_int": {"type": "int"},
                "create_timestamp_flt": {"type": "float"},
            },
        )
        for j in range(20):
            row_count = 8192
            batch_size = 128
            for i in range(0, row_count, batch_size):
                table_obj.insert(
                    [
                        {
                            "doc_id": "f294ba5cd48711efaaa610ffe02aa993",
                            "knowledge_graph_kwd": (
                                ""
                                if j != 9
                                else ("graph" if i % 2 == 0 else "mind_map")
                            ),
                            "page_num_int": i,
                            "top_int": i,
                            "create_timestamp_flt": 0.0,
                        }
                        for _ in range(batch_size)
                    ]
                )

        table_obj = db_obj.get_table(table_name)

        res, extra_result = (
            table_obj.output(["doc_id"])
            .option({"total_hits_count": True})
            .filter(
                "(doc_id IN ('f294ba5cd48711efaaa610ffe02aa993')) AND (knowledge_graph_kwd IN ('graph', 'mind_map'))"
            )
            .offset(0)
            .limit(30)
            .to_df()
        )
        gt = pd.DataFrame(
            {"doc_id": ["f294ba5cd48711efaaa610ffe02aa993" for i in range(30)]}
        ).astype({"doc_id": dtype("object")})
        pd.testing.assert_frame_equal(res, gt)

        res, extra_result = (
            table_obj.output(["doc_id"])
            .option({"total_hits_count": True})
            .filter(
                "(doc_id IN ('f294ba5cd48711efaaa610ffe02aa993')) AND (knowledge_graph_kwd IN ('graph', 'mind_map'))"
            )
            .offset(0)
            .limit(30)
            .sort(
                [
                    ["page_num_int", SortType.Asc],
                    ["top_int", SortType.Asc],
                    ["create_timestamp_flt", SortType.Desc],
                ]
            )
            .to_df()
        )
        assert extra_result['total_hits_count'] == 8192

        gt = pd.DataFrame(
            {"doc_id": ["f294ba5cd48711efaaa610ffe02aa993" for i in range(30)]}
        ).astype({"doc_id": dtype("object")})
        pd.testing.assert_frame_equal(res, gt)

        db_obj.drop_table(table_name)
