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

import json
import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST
from infinity.common import ConflictType
import pandas as pds


def main():
    infinity_obj = infinity.connect(REMOTE_HOST)
    db = infinity_obj.get_database("default_db")
    # Drop my_table if it already exists
    db.drop_table("my_table", ConflictType.Ignore)
    # Create a table named "my_table"
    table = db.create_table(
        "my_table", {
            "num": {"type": "integer"}, 
            "body": {"type": "varchar"},
            "vec": {"type": "vector, 4, float"}
        })
    table.insert(
        [{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
    table.insert(
        [{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])

    res = table.output(["*"]).knn("vec", [3.0, 2.8, 2.7,
                                          3.1], "float", "ip", 2).to_pl()
    print("------all rows-------")
    print(res)


def test():
    try:
        infinity_obj = infinity.connect(REMOTE_HOST)
        db = infinity_obj.get_database("default_db")
        # Drop my_table if it already exists
        db.drop_table("my_table", ConflictType.Ignore)
        # Create a table named "my_table"
        table = db.create_table(
            "my_table", {
                "num": {"type": "integer"}, 
                "body": {"type": "varchar"},
                "vec": {"type": "vector, 4, float"}
            })
        table.insert(
            [{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
        table.insert(
            [{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])

        # `create_index()` is required before match() or fusion()
        res = table.create_index("my_index",
                                 [index.IndexInfo("body",
                                                  index.IndexType.FullText,
                                                  []),
                                  ], ConflictType.Error)
        # assert res.success

        res = table.output(["num", "body"]).knn(
            "vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2).to_pl()

        pds_df = pds.DataFrame(res)
        json_data = pds_df.to_json()
        print("------json-------")
        print(json_data)

        table_obj = db.get_table("my_table")
        qb_result = table_obj.output(["num", "body"]).knn(
            "vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3).to_pl()
        print("------tabular -------")
        print("------knn-------")
        print(qb_result)

        qb_result1 = table_obj.match(
            "body", "blooms", "topn=1").output(["num", "body"]).to_pl()
        print("------match-------")
        print(qb_result1)

        qb_result2 = table_obj.output(["num", "body"]).knn(
            "vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3).match(
            "body", "blooms", "topn=1").fusion('rrf').to_pl()
        print("------knn+match-------")
        print(qb_result2)

    except Exception as e:
        print(str(e))

    # Print the JSON data
    # print("------json-------")
    # print(json_data)
    # print("------tabular - querybuilder-------")
    # print(qb_result)


if __name__ == '__main__':
    main()
    test()
