# Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

'''
This example is about connecting to the local infinity instance, creating table, inserting data, creating bmp index and searching data
'''

import infinity
import sys
import time
from infinity.common import SparseVector

try:
    # Use infinity_embedded module to open a local directory
    # infinity_instance = infinity.connect("/var/infinity")

    #  Use infinity module to connect a remote server
    infinity_instance = infinity.connect(infinity.common.NetworkAddress("127.0.0.1", 23817))

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop my_table if it already exists
    db_instance.drop_table("my_table", infinity.common.ConflictType.Ignore)

    # Create a table named "my_table"
    # "CREATE TABLE my_table (c1 INT, c2 SPARSE(FLOAT, 100))"
    table_instance = db_instance.create_table("my_table", {
        "c1": {"type": "integer"},
        "c2": {"type": "sparse,100,float,int8"},
    })

    # Insert 4 rows
    # "INSERT INTO my_table VALUES (1,[0:1.0,10:1.0,20:1.0,30:1.0,40:1.0,50:1.0,60:1.0,70:1.0,80:1.0,90:1.0])"
    table_instance.insert([
        {
            "c1": 1,
            "c2": SparseVector(**{"indices": [0, 10, 20, 30, 40, 50, 60, 70, 80, 90],
                                  "values": [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]})
        }]
    )

    table_instance.insert([
        {
            "c1": 2,
            "c2": SparseVector([0, 20, 40, 60, 80], [2.0, 2.0, 2.0, 2.0, 2.0])
        }]
    )

    table_instance.insert([
        {
            "c1": 3,
            "c2": SparseVector(**{"indices": [0, 30, 60, 90], "values": [3.0, 3.0, 3.0, 3.0]})
        }]
    )

    table_instance.insert([
        {
            "c1": 4,
            "c2": {"0": 4.0, "40": 4.0, "80": 4.0}
        }]
    )

    # Create a bmp index on the table
    # "CREATE INDEX index1 ON my_table (c2) USING BMP WITH (block_size = 8, compress_type = compress)"
    table_instance.create_index("index1",
                                infinity.index.IndexInfo("c2",
                                                         infinity.index.IndexType.BMP,
                                                         {
                                                             "block_size": "8",
                                                             "compress_type": "compress"
                                                         }), infinity.common.ConflictType.Error)

    # Search with bmp index
    # "SELECT c1 FROM my_table SEARCH MATCH SPARSE (c2, [0:1.0,20:2.0,80:3.0], 'ip', 3)"
    res, extra_result = (table_instance.output(["c1"])
                         .match_sparse("c2", SparseVector(**{"indices": [0, 20, 80], "values": [1.0, 2.0, 3.0]}), "ip",
                                       3)
                         .to_pl())
    print(res)
    if extra_result is not None:
        print(extra_result)

    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)
