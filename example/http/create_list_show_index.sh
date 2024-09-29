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

# create HNSW index on dense vector column: 'dense_column'
echo -e '\n\n-- create table index: hnsw_index'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/hnsw_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
          "fields":
          [
              "dense_column"
          ],
          "index":
          {
              "type": "Hnsw",
              "M": "16",
              "ef_construction": "50",
              "metric": "l2"
          },
          "create_option": "ignore_if_exists"
     } '


echo -e '\n\n-- create table index: hnsw_index on multi_vector column'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/multi_vector_hnsw_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
          "fields":
          [
              "multivector_column"
          ],
          "index":
          {
              "type": "Hnsw",
              "M": "16",
              "ef_construction": "50",
              "metric": "l2"
          },
          "create_option": "ignore_if_exists"
     } '

# create Full-text index on varchar column: 'fulltext_column'
echo -e '\n\n-- create table index: fulltext_index'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/fulltext_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
          "fields":
          [
              "fulltext_column"
          ],
          "index":
          {
              "type": "fulltext",
              "analyzer": "standard"
          },
          "create_option": "ignore_if_exists"
     } '

# create secondary index on varchar column: 'name'
echo -e '\n\n-- create table index: secondary_index'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/secondary_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
          "fields":
          [
              "name"
          ],
          "index":
          {
              "type": "secondary"
          },
          "create_option": "ignore_if_exists"
     } '

# create sparse vector index on sparse vector column: 'sparse_column'
echo -e '\n\n-- create table index: bmp_index'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/bmp_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
          "fields":
          [
              "sparse_column"
          ],
          "index":
          {
              "type": "bmp"
          },
          "create_option": "ignore_if_exists"
     } '



# list table indexes
echo -e '\n\n-- list table indexes: hnsw_index, sparse_index, fulltext_index, bmp_index should be included'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes \
     --header 'accept: application/json'

# show table tbl1 hnsw_index
echo -e '\n\n-- show tbl1 hnsw_index'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/hnsw_index \
     --header 'accept: application/json'

# show table tbl1 hnsw_index
echo -e '\n\n-- show tbl1 multi_vector_hnsw_index'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/multi_vector_hnsw_index \
     --header 'accept: application/json'

# show table tbl1 fulltext_index
echo -e '\n\n-- show tbl1 fulltext_index'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/fulltext_index \
     --header 'accept: application/json'

# show table tbl1 secondary_index
echo -e '\n\n-- show tbl1 secondary_index'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/secondary_index \
     --header 'accept: application/json'

# show table tbl1 bmp_index
echo -e '\n\n-- show tbl1 bmp_index'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/bmp_index \
     --header 'accept: application/json'

# drop tbl1 hnsw_index
echo -e '\n\n-- drop tbl1 hnsw_index'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/hnsw_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"drop_option": "ignore_if_not_exists"} '

# drop tbl1 hnsw_index
echo -e '\n\n-- drop tbl1 multi_vector_hnsw_index'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/multi_vector_hnsw_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"drop_option": "ignore_if_not_exists"} '

# drop tbl1 fulltext_index
echo -e '\n\n-- drop tbl1 fulltext_index'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/fulltext_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"drop_option": "ignore_if_not_exists"} '

# drop tbl1 secondary_index
echo -e '\n\n-- drop tbl1 secondary_index'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/secondary_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"drop_option": "ignore_if_not_exists"} '

# drop tbl1 bmp_index
echo -e '\n\n-- drop tbl1 bmp_index'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes/bmp_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"drop_option": "ignore_if_not_exists"} '

# list table indexes
echo -e '\n\n-- no indexes'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/indexes \
     --header 'accept: application/json'

# drop tbl1
echo -e '\n\n-- drop tbl1'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1 \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"drop_option": "error"} '
