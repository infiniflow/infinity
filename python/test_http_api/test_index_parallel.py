import os
import sys
import pytest
import time
import random
from httpapibase import HttpTest
from threading import Thread
from common.common_values import *
import infinity.index as index
import httputils
from infinity.errors import ErrorCode
from infinity.common import ConflictType


class TestIndexParallel(HttpTest):
    @pytest.mark.skip(reason="segfault")
    def test_http_fulltext_index_rw_parallel(self):
        def write_worker(data, file_path, end_time, thread_id):
            db_name = "default"
            table_name = "test_fulltext_index_parallel"
            self.show_database(db_name)

            while time.time() < end_time:
                operation = random.randint(0, 1)
                if operation == 0:
                    value = []
                    for i in range(len(data["doctitle"])):
                        value.append({"doctitle": data["doctitle"][i],
                                      "docdate": data["docdate"][i], "body": data["body"][i]})
                    self.insert(db_name, table_name, value)
                    print(f"thread {thread_id}: insert complete")
                if operation == 1:
                    print(f"thread {thread_id}: begin import")
                    self.import_data(file_path, {"delimiter": "\t"})
                    print(f"thread {thread_id}: import complete")

        def read_worker(end_time):
            db_name = "default"
            table_name = "test_fulltext_index_parallel"
            while time.time() < end_time:
                self.select(db_name, table_name, ["doctitle", "docdate", "_row_id", "_score"], "", {
                    "match":
                        {
                            "fields": "body^5",
                            "query": "harmful chemical",
                            "operator": "topn=3"
                        }
                })
                time.sleep(0.1)
