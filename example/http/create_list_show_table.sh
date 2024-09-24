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
                  "name": "name",
                  "type": "varchar"
             },
             {
                  "name": "score",
                  "type": "float",
                  "default": 3.0
             },
             {
                  "name": "dense_column",
                  "type": "vector,8,float",
                  "default": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]
             },
             {
                  "name": "fulltext_column",
                  "type": "varchar",
                  "default": ""
             },
             {
                  "name": "sparse_column",
                  "type": "sparse,128,float,int",
                  "default": {"10":1.1, "20":2.2, "30": 3.3}
             },
             {
                  "name": "tensor_column",
                  "type": "tensor,4,float",
                  "default": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
             },
             {
                  "name": "multivector_column",
                  "type": "multivector,4,float",
                  "default": [[0.0, 0.0, 0.0, 0.0], [0.0, 0.0, 0.0, 0.0]]
             },
             {
                  "name": "tensorarray_column",
                  "type": "tensorarray,2,float",
                  "default": [[[1.0, 1.0], [1.0, 1.0]], [[1.0, 1.0]]]
             }
        ]
    } '

# list tables, there are 'tbl1' at least.
echo -e '\n-- list tables, tbl1 should be included'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables \
     --header 'accept: application/json'

# show table tb1
echo -e '\n-- show tbl1'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1 \
     --header 'accept: application/json'

# show table tb1 columns
echo -e '\n-- show tbl1 columns'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/columns \
     --header 'accept: application/json'

# drop tbl1
echo -e '\n-- drop tbl1'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1 \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"drop_option": "error"} '

# list tables, 'tbl1' is dropped.
echo -e '\n-- list tables, tbl1 is dropped'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables \
     --header 'accept: application/json'
echo -e '\n'