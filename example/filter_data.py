# Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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


import infinity_embedded as infinity
#import infinity
import sys

try:
    # open a local directory(default = "/var/infinity") or connect to server(default = NetworkAddress("127.0.0.1", 23817)) to store the data
    infinity_instance = infinity.connect()

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop my_table if it already exists
    db_instance.drop_table("my_table", infinity.common.ConflictType.Ignore)

    # Create a table named "my_table"
    table_instance = db_instance.create_table("my_table", {
        "num": {"type": "integer"},
        "name": {"type": "varchar"},
        "score": {"type": "float"},
    })

    # Insert 10 rows of data into the 'my_table'
    table_instance.insert(
        [
            {
                "num": 1,
                "name": "Tom",
                "score": 90.5,
            },
            {
                "num": 2,
                "name": "Henry",
                "score": 70.0,
            },
            {
                "num": 3,
                "name": "James",
                "score": 75.0,
            },
            {
                "num": 4,
                "name": "Toby",
                "score": 92.0,
            },
            {
                "num": 5,
                "name": "Thomas",
                "score": 72.5,
            },
            {
                "num": 6,
                "name": "Charlie",
                "score": 69.0,
            },
            {
                "num": 7,
                "body": "Chris",
                "score": 88.0,
            },
            {
                "num": 8,
                "name": "Bill",
                "score": 90.0,
            },
            {
                "num": 9,
                "name": "Stefan",
                "score": 86.5,
            },
            {
                "num": 10,
                "name": "Steve",
                "score": 86.0,
            },
        ]
    )

    result = table_instance.output(["num", "name", "score"]).filter("(score > 80.0) and (score <= 90.0)").to_pl()
    print(result)

    # result = table_instance.output(["num", "name", "score"]).filter("not (score > 80.0)").to_pl()
    # print(result)

    result = table_instance.output(["num", "name", "score"]).filter("num <> 9").to_pl()
    print(result)
    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)