##!/bin/bash
#
## Define base URL and database name
#BASE_URL="http://127.0.0.1:23820"
#DATABASE_NAME="default_db"
#TABLE_NAME="my_table"
#
## Drop table 'my_table' if it exists
#echo -e '-- Drop table my_table if exists'
#curl --request DELETE \
#     --url "${BASE_URL}/databases/${DATABASE_NAME}/tables/${TABLE_NAME}" \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{"drop_option": "ignore_if_not_exists"}'
#echo ""
#
## Create table 'my_table'
#echo -e '\n-- Create table my_table'
#curl --request POST \
#     --url "${BASE_URL}/databases/${DATABASE_NAME}/tables/${TABLE_NAME}" \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{
#         "create_option": "ignore_if_exists",
#         "fields": [
#             {"name": "num", "type": "integer"},
#             {"name": "body", "type": "varchar"},
#             {"name": "vec", "type": "tensor,4,float"}
#         ]
#     }'
#echo ""
#
## Insert 4 rows of data into 'my_table'
#echo -e '\n-- Insert 4 rows into my_table'
#curl --request POST \
#     --url "${BASE_URL}/databases/${DATABASE_NAME}/tables/${TABLE_NAME}/docs" \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '[
#         {
#             "num": 1,
#             "body": "unnecessary and harmful",
#             "vec": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
#         },
#         {
#             "num": 2,
#             "body": "Office for Harmful Blooms",
#             "vec": [[4.0, 0.0, 4.3, 4.5], [4.0, 4.2, 4.4, 5.0]]
#         },
#         {
#             "num": 3,
#             "body": "A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
#             "vec": [[0.9, 0.1, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
#         },
#         {
#             "num": 4,
#             "body": "The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.",
#             "vec": [[4.0, 4.2, 4.3, 4.5], [4.0, 4.2, 4.3, 4.4]]
#         }
#     ]'
#echo ""
#
## Query data from 'my_table' with tensor similarity search
#echo -e '\n-- Query data from my_table with tensor similarity search'
#curl --request GET \
#     --url "${BASE_URL}/databases/${DATABASE_NAME}/tables/${TABLE_NAME}/docs" \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{
#         "output": ["num", "vec", "_score"],
#         "search": [
#             {
#                 "match_method": "tensor",
#                 "fields": "vec",
#                 "query_vector": [[0.9, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]],
#                 "element_type": "float",
#                 "topn": 2
#             }
#         ]
#     }'
#echo ""
#
#echo -e '\n-- Test done'