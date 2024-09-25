import pathlib
from numpy import dtype
import pytest
import os
import sys
from common import common_values
import infinity_embedded
import infinity
from infinity.common import ConflictType
import infinity.index as index

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
import importlib
from infinity_http import infinity_http


class TestCleanup:
    @pytest.fixture(autouse=True)
    def setup_and_teardown(self, local_infinity, http, suffix):
        if local_infinity:
            module = importlib.import_module("infinity_embedded.index")
            globals()["index"] = module
            self.uri = common_values.TEST_LOCAL_PATH
            self.infinity_obj = infinity_embedded.connect(self.uri)
        elif http:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity_http()
        else:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity.connect(self.uri)

        self.suffix = suffix
        yield

        res = self.infinity_obj.disconnect()
        assert res.error_code == infinity.ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_invalidate_fulltext_cache(self):
        table_name = "test_add_column" + self.suffix
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            table_name,
            {
                "c1": {"type": "varchar"},
                "c2": {"type": "varchar"},
            },
        )
        table_obj.insert([{"c1": "text1", "c2": "text2"}])

        drop_index_name = "idx1_todrop"
        table_obj.create_index(drop_index_name, index.IndexInfo("c1", index.IndexType.FullText))
        table_obj.create_index("idx2", index.IndexInfo("c2", index.IndexType.FullText))

        res = (
            table_obj.output(["c1"])
            .match_text(fields="c1", matching_text="text1", topn=1)
            .to_result()
        )

        table_obj.drop_index(drop_index_name)

        self.infinity_obj.cleanup()
        dropped_index_dirs = pathlib.Path("/var/infinity/data").rglob(f"*{drop_index_name}*")
        assert len(list(dropped_index_dirs)) == 0

        db_obj.drop_table(table_name)
