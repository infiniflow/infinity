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
This example is to connect local infinity instance, create table, insert data, search the data
'''

# import infinity_embedded as infinity
import infinity
import sys

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
    table_instance = db_instance.create_table("my_table", {
        "num": {"type": "integer"},
        "body": {"type": "varchar"},
        "vec": {"type": "sparse,100,float,int"}
    })

    # Insert 3 rows of data into the 'my_table'
    table_instance.insert(
        [
            {
                "num": 1,
                "body": r"unnecessary and harmful",
                "vec":  infinity.common.SparseVector([10, 20, 30], [1.1, 2.2, 3.3])
            },
            {
                "num": 2,
                "body": r"Office for Harmful Blooms",
                "vec": infinity.common.SparseVector([40, 50, 60], [4.4, 5.5, 6.6])
            },
            {
                "num": 3,
                "body": r"A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
                "vec":  infinity.common.SparseVector([70, 80, 90], [7.7, 8.8, 9.9])
            },
        ]
    )

    result = table_instance.output(["num", "vec", "_similarity"]).match_sparse("vec", infinity.common.SparseVector([0, 20, 80], [1.0, 2.0, 3.0]), "ip", 3).to_pl()
    print(result)
    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)
