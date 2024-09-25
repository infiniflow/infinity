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

# create table 'tbl1'
echo -e '-- create tbl1'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1 \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {
         "create_option": "ignore_if_exists",
         "fields": [
             {
                  "name": "num",
                  "type": "integer"
             },
             {
                  "name": "body",
                  "type": "varchar"
             },
             {
                  "name": "vec",
                  "type": "vector, 4, float"
             },
             {
                  "name": "sparse_column",
                  "type": "sparse,100,float,int"
             },
             {
                  "name": "year",
                  "type": "integer"
             },
             {
                  "name": "tensor",
                  "type": "tensor,4,float"
             }
        ]
    } '

# insert 4 rows into 'tbl1'
echo -e '\n\n-- insert 4 rows into tbl1'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     [
         {
             "num": 1,
             "body": "unnecessary and harmful",
             "vec": [1.0, 1.2, 0.8, 0.9],
             "sparse_column": {"10":1.1, "20":2.2, "30": 3.3},
             "year": 2024,
             "tensor": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
         },
         {
             "num": 2,
             "body": "Office for Harmful Blooms",
             "vec": [4.0, 4.2, 4.3, 4.5],
             "sparse_column": {"40":4.4, "50":5.5, "60": 6.6},
             "year": 2023,
             "tensor": [[4.0, 0.0, 4.3, 4.5], [4.0, 4.2, 4.4, 5.0]]
         },
         {
             "num": 3,
             "body": "A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
             "vec": [4.0, 4.2, 4.3, 4.2],
             "sparse_column":  {"70":7.7, "80":8.8, "90": 9.9},
             "year": 2019,
             "tensor": [[0.9, 0.1, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
         },
         {
             "num": 4,
             "body": "The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.",
             "vec": [4.0, 4.2, 4.3, 4.5],
             "sparse_column":  {"20":7.7, "80":7.8, "90": 97.9},
             "year": 2018,
             "tensor": [[5.0, 4.2, 4.3, 4.5], [4.0, 4.2, 4.3, 4.4]]
         }
     ] '

# show all rows of 'tbl1'
echo -e '\n\n-- select all rows of tbl1'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
         "output":
         [
             "*"
         ]
     } '


# select num and year of 'tbl1' where num > 1 and year < 2023
echo -e '\n\n-- select num and year of tbl1 where num > 1 and year < 2023 offset 1 limit 1'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
         "output":
         [
             "num",
             "year"
         ],
         "filter": "num > 1 and year < 2024",
         "offset": "1",
         "limit": "1"
     } '

echo -e '\n\n-- search with dense vector'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
         "output":
         [
             "num",
             "year",
             "similarity()",
             "row_id()"
         ],
         "search":
         [
             {
                 "match_method": "dense",
                 "fields": "vec",
                 "query_vector": [5.0, 2.8, 2.7, 3.1],
                 "element_type": "float",
                 "metric_type": "cosine",
                 "topn": 4
             }
         ]
     } '

echo -e '\n\n-- search with sparse vector'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
         "output":
         [
             "num",
             "year",
             "similarity()",
             "row_id()"
         ],
         "search":
         [
             {
                 "match_method": "sparse",
                 "fields": "sparse_column",
                 "query_vector": {"0":1.0, "20":2.0, "80": 3.0},
                 "metric_type": "ip",
                 "topn": 3
             }
         ]
     } '

# Can't search without full-text index
echo -e '\n\n-- Fail to search with full text'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
         "output":
         [
             "num",
             "year",
             "score()",
             "row_id()"
         ],
         "search":
         [
             {
                "match_method": "text",
                "fields": "body",
                "matching_text": "bloom",
                "topn": 1,
                "params":
                {
                    "default_fields": "body",
                    "operator": "or"
                }
             }
         ]
     } '

# create Full-text index on varchar column: 'text'
echo -e '\n\n-- create table index: fulltext_index'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/fulltext_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
          "fields":
          [
              "body"
          ],
          "index":
          {
              "type": "fulltext",
              "analyzer": "standard"
          },
          "create_option": "ignore_if_exists"
     } '

echo -e '\n\n-- search with full text'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
         "output":
         [
             "num",
             "year",
             "score()",
             "row_id()"
         ],
         "search":
         [
             {
                "match_method": "text",
                "fields": "body",
                "matching_text": "bloom",
                "topn": 2,
                "params":
                {
                    "default_fields": "body",
                    "operator": "or"
                }
             }
         ]
     } '


# drop tbl1
echo -e '\n\n-- drop tbl1'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1 \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"drop_option": "error"} '
echo -e '\n'
