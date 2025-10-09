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
This example is about connecting to the local infinity instance, creating table, inserting data, creating ivf index, and searching data
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

    try:
        table_obj = db_instance.get_table("test_mem_ivf")
    except infinity.common.InfinityException as e:
        # # Drop my_table if it already exists
        # db_instance.drop_table("test_mem_ivf", infinity.common.ConflictType.Ignore)

        # Create a table named "test_mem_ivf"
        # "CREATE TABLE test_mem_ivf (c1 INT, c2 EMBEDDING(FLOAT, 4))"
        table_obj = db_instance.create_table(
            "test_mem_ivf",
            {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}},
        )
        res = table_obj.create_index(
            "idx1",
            infinity.index.IndexInfo(
                "c2",
                infinity.index.IndexType.IVF,
                {
                    "metric": "l2",
                },
            ),
        )

        table_obj.insert([{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(51)])
        # trigger the dump by 52th record
        table_obj.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]}])
        # table_obj.insert([{"c1": 2, "c2": [0.1, 0.2, 0.3, -0.2]} for i in range(2)])
        time.sleep(5)
        table_obj.insert([{"c1": 4, "c2": [0.2, 0.1, 0.3, 0.4]} for i in range(4)])

    data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
    print(data_dict)
    assert data_dict["count(star)"] == [56]

    # Search with ivf index
    # SELECT c1 FROM my_table SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 3)
    data_dict, data_type_dict, _ = (
        table_obj.output(["c1"])
        .match_dense("c2", [0.3, 0.3, 0.2, 0.2], "float", "l2", 6, {"nprobe" : "100"})
        .to_result()
    )
    print(data_dict["c1"])
    assert data_dict["c1"] == [4, 4, 4, 4, 4, 2]

    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)
