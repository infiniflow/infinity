import importlib
import sys
import os
import infinity
import infinity_embedded
from infinity.errors import ErrorCode
from infinity.common import ConflictType, InfinityException
from infinity import NetworkAddress

def test_import_exceeding_rows():
    infinity_obj = infinity.connect(NetworkAddress("127.0.0.1", 23817))
    db_obj = infinity_obj.get_database("default_db")
    db_obj.drop_table("test_import_exceeding_rows", ConflictType.Ignore)
    table_obj = db_obj.create_table("test_import_exceeding_rows",
                                    {"c1": {"type": "int"}, "c2": {"type": "varchar"}})

    test_csv_dir = "/var/infinity/test_data/" + "pysdk_test_big_varchar_rows.csv"
    res = table_obj.import_data(test_csv_dir)
    assert res.error_code == ErrorCode.OK

    res = table_obj.output(["count(*)"]).to_pl()
    assert res.height == 1 and res.width == 1 and res.item(0, 0) == 1024 * 8192
    db_obj.drop_table("test_import_exceeding_rows", ConflictType.Error)

    infinity_obj.disconnect()

test_import_exceeding_rows()