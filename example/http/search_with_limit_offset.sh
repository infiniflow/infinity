##!/bin/bash
#
## Connect to the local Infinity instance
#echo -e "\n-- Connect to the local Infinity instance"
#curl --request POST \
#     --url http://localhost:23820/databases/default_db \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{"create_option": "ignore_if_exists"}'
#
## Drop table 'my_table' if it exists
#echo -e "\n-- Drop table my_table if exists"
#curl --request DELETE \
#     --url http://localhost:23820/databases/default_db/tables/my_table \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{"drop_option": "ignore_if_not_exists"}'
#
## Create table 'my_table'
#echo -e "\n-- Create table my_table"
#curl --request POST \
#     --url http://localhost:23820/databases/default_db/tables/my_table \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{
#         "create_option": "ignore_if_exists",
#         "fields": [
#             {"name": "num", "type": "integer"},
#             {"name": "body", "type": "varchar"},
#             {"name": "vec", "type": "vector,4,float"}
#         ]
#     }'
#
## Insert 3 rows into 'my_table'
#echo -e "\n-- Insert 3 rows into my_table"
#curl --request POST \
#     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '[
#         {"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]},
#         {"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]},
#         {"num": 3, "body": "A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.", "vec": [4.0, 4.2, 4.3, 4.2]}
#     ]'
#
## Query data from 'my_table' with dense vector search and pagination
#echo -e "\n-- Query data from my_table with dense vector search and pagination"
#curl --request GET \
#     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{
#         "output": ["num", "vec", "_similarity"],
#         "search": [
#             {
#                 "match_method": "dense",
#                 "fields": "vec",
#                 "query_vector": [3.0, 2.8, 2.7, 3.1],
#                 "element_type": "float",
#                 "metric_type": "cosine",
#                 "topn": 3
#             }
#         ],
#         "limit": 2,
#         "offset": 1,
#         "option": {"total_hits_count": true}
#     }'
#
#echo -e "\n-- Test done"