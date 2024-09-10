# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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
This example is about connecting to the local infinity instance, creating table, inserting data, and searching data
'''

import infinity_embedded as infinity
#import infinity
import sys

try:
    # open a local directory(default = "/var/infinity") or connect to server(default = NetworkAddress("127.0.0.1", 23817)) to store the data
    infinity_instance = infinity.connect()

    infinity_instance.create_database("db1", infinity.common.ConflictType.Ignore)
    infinity_instance.create_database("db2", infinity.common.ConflictType.Ignore)
    infinity_instance.create_database("db3", infinity.common.ConflictType.Ignore)

    res = infinity_instance.list_databases()
    print(res.db_names)

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("db1")

    db_instance.create_table("table1", {
        "num": {"type": "integer", "constraints": ["PRIMARY KEY"]},
        "body": {"type": "varchar"},
        "vec": {"type": "vector, 4, float"},
    }, infinity.common.ConflictType.Ignore)

    db_instance.create_table("table2", {
        "num": {"type": "integer", "constraints": ["PRIMARY KEY"]},
        "body": {"type": "varchar"},
        "vec": {"type": "vector, 4, float"},
    }, infinity.common.ConflictType.Ignore)

    db_instance.create_table("table3", {
        "num": {"type": "integer", "constraints": ["PRIMARY KEY"]},
        "body": {"type": "varchar"},
        "vec": {"type": "vector, 4, float"},
    }, infinity.common.ConflictType.Ignore)

    res = db_instance.list_tables()
    print(res.table_names)
    # # Insert 3 rows of data into the 'my_table'
    # table_instance.insert(
    #     [
    #         {
    #             "num": 1,
    #             "body": r"unnecessary and harmful",
    #             "vec": [1.0, 1.2, 0.8, 0.9],
    #         },
    #         {
    #             "num": 2,
    #             "body": r"Office for Harmful Blooms",
    #             "vec": [4.0, 4.2, 4.3, 4.5],
    #         },
    #         {
    #             "num": 3,
    #             "body": r"A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
    #             "vec": [4.0, 4.2, 4.3, 4.5],
    #         },
    #     ]
    # )
    #
    # res = table_instance.output(["num", "body", "vec"]).to_pl()
    # print(res)

    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)
