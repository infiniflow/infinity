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

import infinity
import pandas

try:
    # open a local directory to store the data
    infinity_instance = infinity.connect("/var/infinity")

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop my_table if it already exists
    db_instance.drop_table("my_table", infinity.common.ConflictType.Ignore)

    # Create a table named "my_table"
    table_instance = db_instance.create_table("my_table", {
            "num": {"type": "integer"},
            "body": {"type": "varchar"},
            "vec": {"type": "vector, 4, float"},
    })

    # Insert 3 rows of data into the 'my_table'
    table_instance.insert(
        [
            {
                "num": 1,
                "body": r"unnecessary and harmful",
                "vec": [1.0, 1.2, 0.8, 0.9],
            },
            {
                "num": 2,
                "body": r"Office for Harmful Blooms",
                "vec": [4.0, 4.2, 4.3, 4.5],
            },
            {
                "num": 3,
                "body": r"A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
                "vec": [4.0, 4.2, 4.3, 4.5],
            },
        ]
    )

    res = table_instance.output(["num", "body", "vec"]).to_pl()
    print(res)

    infinity_instance.disconnect()
    #
    # # `create_index()` is required before match() or fusion()
    # # Create index on varchar column for full-text search and multiple way fusion.
    # res = table_instance.create_index(
    #     "my_index",
    #     [
    #         infinity.index.IndexInfo("body", infinity.index.IndexType.FullText, []),
    #     ],
    #     infinity.ConflictType.Error,
    # )
    #
    #
    #
    # pds_df = pds.DataFrame(res)
    # json_data = pds_df.to_json()
    # print(json_data)
    #
    # print("------tabular -------")
    # print("------vector-------")
    # table_obj = db.get_table("my_table")
    # qb_result = (
    #     table_obj.output(["num", "body"])
    #         .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3)
    #         .to_pl()
    # )
    # print(qb_result)
    #
    # print("------fulltext-------")
    # questions = [
    #     r"blooms",  # single term
    #     r"Bloom filter",  # OR multiple terms
    #     r'"Bloom filter"',  # phrase: adjacent multiple terms
    #     r"space efficient",  # OR multiple terms
    #     # r"space-efficient",      # Error 3013: Invalid query statement: OrQueryNode should not have both not child and non-not child
    #     r"space\-efficient",  # Escape reserved character '-', equivalent to: `space efficient`
    #     r'"space\-efficient"',  # phrase and escape reserved character, equivalent to: `"space efficient"`
    # ]
    # for question in questions:
    #     qb_result = (
    #         table_obj.output(["num", "body", "_score"])
    #             .match("body", question, "topn=10")
    #             .to_pl()
    #     )
    #     print(f"question: {question}")
    #     print(qb_result)
    #
    # print("------vector+fulltext-------")
    # qb_result2 = (
    #     table_obj.output(["num", "body"])
    #         .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3)
    #         .match("body", "blooms", "topn=1")
    #         .fusion("rrf")
    #         .to_pl()
    # )
    # print(qb_result2)

except Exception as e:
    print(str(e))