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

table_name = "test_invalid_fulltext_cache2"
index_name = "body_index"

data_dir = "/var/infinity/data"
import_file = "test/data/csv/enwiki_9.csv"
abs_import_file = os.path.abspath(import_file)

infinity_obj = infinity.connect(infinity.common.NetworkAddress("127.0.0.1", 23817))

db_obj = infinity_obj.get_database("default_db")
db_obj.drop_table(table_name, ConflictType.Ignore)
table_obj = db_obj.create_table(
    table_name,
    {
        "doctitle": {"type": "varchar"},
        "docdate": {"type": "varchar"},
        "body": {"type": "varchar"},
    },
    ConflictType.Error,
)
res = table_obj.create_index(
    index_name, index.IndexInfo("body", index.IndexType.FullText)
)
assert res.error_code == ErrorCode.OK

# compaction need 4 segment in default config
for i in range(4):
    table_obj.import_data(abs_import_file, {"delimiter": "\t"})

res = table_obj.output(["*"]).match_text("body^5", "America", 3).to_pl()

# wait for compaction
time.sleep(3)
infinity_obj.flush_data()
infinity_obj.cleanup()
time.sleep(1)

index_id = 1
dropped_index_dirs = []
dropped_index_dirs_gen = pathlib.Path(data_dir).rglob(f"*idx_{index_id}*")
for dir in dropped_index_dirs_gen:
    dropped_index_dirs.append(dir)

db_obj.drop_table(table_name)
