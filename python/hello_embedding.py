import os
import sys

# remove local path, use the installed infinity sdk
current_path = os.path.abspath(os.path.dirname(__file__))
local_infinity_path = os.path.join(current_path, 'infinity')
if local_infinity_path in sys.path:
    sys.path.remove(local_infinity_path)
if current_path in sys.path:
    sys.path.remove(current_path)

import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST, ConflictType
import pandas as pds
import numpy as np

infinity_obj = infinity.connect(REMOTE_HOST)
db = infinity_obj.get_database("default_db")
# Drop my_table if it already exists
db.drop_table("my_table", ConflictType.Ignore)
# Create a table named "my_table"
table = db.create_table(
    "my_table",
    {
        "num": {"type": "integer"},
        "body": {"type": "varchar"},
        "vec": {"type": "vector, 4, float"},
    },
)

embedding=np.array([1.0, 1.2, 0.8, 0.9])
table.insert(
    [{"num": 1, "body": "unnecessary and harmful", "vec": embedding.astype('float32')}]
)
table.insert(
    [
        {
            "num": 2,
            "body": "Office for Harmful Blooms",
            "vec": [4.0, 4.2, 4.3, 4.5],
        }
    ]
)

res = (
    table.output(["num", "body"])
    .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2)
    .to_pl()
)

pds_df = pds.DataFrame(res)
json_data = pds_df.to_json()
print("------json-------")
print(json_data)