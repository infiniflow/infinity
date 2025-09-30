import infinity
from infinity.table import ExplainType
from infinity import index
import sys
import os
import time
# from common import common_values
from infinity.common import ConflictType
import pathlib
from infinity.errors import ErrorCode

table_name = "test_alter2"
infinity_obj = infinity.connect(infinity.common.NetworkAddress("127.0.0.1", 23817))
db_obj = infinity_obj.get_database("default_db")
db_obj.drop_table(table_name, ConflictType.Ignore)
table_obj = db_obj.create_table(
    table_name,
    {
        "c1": {"type": "int"},
        "c2": {"type": "int"},
        "c3": {"type": "varchar"},
    },
)
res = table_obj.create_index(
    "index1", index.IndexInfo("c3", index.IndexType.FullText)
)
assert res.error_code == ErrorCode.OK

table_obj.insert([{"c1": 1, "c2": 2, "c3": "test"}])

res = table_obj.add_columns({"c4": {"type": "varchar", "default": "tttt"}})
assert res.error_code == ErrorCode.OK

res = table_obj.drop_columns(["c2"])
assert res.error_code == ErrorCode.OK

res = table_obj.add_columns({"c2": {"type": "float", "default": 0.0}})
assert res.error_code == ErrorCode.OK

table_obj.insert([{"c1": 2, "c3": "test2", "c4": "tttt2", "c2": 1.0}])

res = table_obj.add_columns({"c5": {"type": "int", "default": 0}})
assert res.error_code == ErrorCode.OK

infinity_obj.flush_data()
