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

import os
import sys

# remove local path, use the installed infinity sdk
current_path = os.path.abspath(os.path.dirname(__file__))
local_infinity_path = os.path.join(current_path, "infinity")
if local_infinity_path in sys.path:
    sys.path.remove(local_infinity_path)
if current_path in sys.path:
    sys.path.remove(current_path)

import infinity
import infinity.index as index
from infinity.common import LOCAL_HOST, ConflictType
import pandas as pds


def test_search():
    try:
        infinity_obj = infinity.connect(LOCAL_HOST)
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
        table.insert(
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
                {
                    "num": 4,
                    "body": r"The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.",
                    "vec": [4.0, 4.2, 4.3, 4.5],
                },
            ]
        )

        # `create_index()` is required before match() or fusion()
        res = table.create_index(
            "my_index",
            [
                index.IndexInfo("body", index.IndexType.FullText, []),
            ],
            ConflictType.Error,
        )
        # assert res.success

        print("------json-------")
        res = (
            table.output(["num", "body"])
            .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2)
            .to_pl()
        )
        pds_df = pds.DataFrame(res)
        json_data = pds_df.to_json()
        print(json_data)

        print("------tabular -------")
        print("------vector-------")
        table_obj = db.get_table("my_table")
        qb_result = (
            table_obj.output(["num", "body"])
            .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3)
            .to_pl()
        )
        print(qb_result)

        print("------fulltext-------")
        questions = [
            r"blooms",  # single term
            r"Bloom filter",  # OR multiple terms
            r'"Bloom filter"',  # phrase: adjacent multiple terms
            r"space efficient",  # OR multiple terms
            # r"space-efficient",      # Error 3013: Invalid query statement: OrQueryNode should not have both not child and non-not child
            r"space\-efficient",  # Escape reserved character '-', equivalent to: `space efficient`
            r'"space\-efficient"',  # phrase and escape reserved character, equivalent to: `"space efficient"`
            r'"harmful chemical"~10',  # sloppy phrase, refers to https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-match-query-phrase.html
        ]
        for question in questions:
            qb_result = (
                table_obj.output(["num", "body", "_score"])
                .match("body", question, "topn=10")
                .to_pl()
            )
            print(f"question: {question}")
            print(qb_result)

        print("------vector+fulltext-------")
        qb_result2 = (
            table_obj.output(["num", "body"])
            .knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 3)
            .match("body", "blooms", "topn=1")
            .fusion("rrf")
            .to_pl()
        )
        print(qb_result2)

    except Exception as e:
        print(str(e))


if __name__ == "__main__":
    test_search()
