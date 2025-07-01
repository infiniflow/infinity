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
This example is about connecting to the local infinity instance, creating table, creating emvb index, inserting data, and searching data
'''

import infinity
import sys
import time

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
    # "CREATE TABLE my_table (c1 integer, c2 TENSOR(FLOAT, 4))"
    table_instance = db_instance.create_table("my_table", {
        "c1": {"type": "integer"},
        "c2": {"type": "tensor,4,float"},
    })

    # Create an emvb index on the table
    # "CREATE INDEX index1 ON my_table (c2) USING EMVB WITH (pq_subspace_num = 32, pq_subspace_bits = 8)"
    table_instance.create_index("index1",
                                infinity.index.IndexInfo("c2",
                                                         infinity.index.IndexType.EMVB,
                                                         {
                                                             "pq_subspace_num": "32",
                                                             "pq_subspace_bits": "8"
                                                         }), infinity.common.ConflictType.Error)

    # Insert 4 rows
    table_instance.insert([
        {
            "c1": 2,
            "c2": [0.1, 0.2, 0.3, -0.2, 0.3, -0.2, 0.2, 0.3]
        }]
    )

    table_instance.insert([
        {
            "c1": 4,
            "c2": [0.2, 0.1, 0.3, 0.4]
        }]
    )

    table_instance.insert([
        {
            "c1": 6,
            "c2": [0.3, 0.2, -11.1, 0.4, 0.4, 0.3, 0.2, -88.5, 0.1, -0.4, 9.4, 0.3]
        }]
    )

    table_instance.insert([
        {
            "c1": 8,
            "c2": [0.4, 0.3, 0.2, 0.1]
        }]
    )

    # Search with emvb index
    # "SELECT c1 FROM my_table SEARCH MATCH TENSOR (c2, [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]], 'float', 'maxsim', 'topn=3')"
    res, extra_result = table_instance.output(["c1"]).match_tensor("c2",
                                                                   [[0.0, -10.0, 0.0, 0.7], [9.2, 45.6, -55.8, 3.5]],
                                                                   "float", 3).to_pl()
    print(res)
    if extra_result is not None:
        print(extra_result)

    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)
