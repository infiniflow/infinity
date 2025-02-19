##!/bin/bash
#
## 1. Drop 'my_table' if it already exists
#echo -e '-- drop my_table if it exists'
#curl --request DELETE \
#     --url http://localhost:23817/databases/default_db/tables/my_table \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{
#         "drop_option": "ignore"
#     }'
#
## 2. Create 'my_table'
#echo -e '\n-- create my_table'
#curl --request POST \
#     --url http://localhost:23817/databases/default_db/tables/my_table \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{
#         "fields": {
#             "num": {"type": "integer"},
#             "name": {"type": "varchar"},
#             "age": {"type": "integer"},
#             "score": {"type": "float"}
#         }
#     }'
#
## 3. Insert 10 rows of data into 'my_table'
#echo -e '\n-- insert data into my_table'
#curl --request POST \
#     --url http://localhost:23817/databases/default_db/tables/my_table/docs \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '[
#         {"num": 1, "name": "Tom", "age": 19, "score": 90.5},
#         {"num": 2, "name": "Henry", "age": 20, "score": 70.0},
#         {"num": 3, "name": "James", "age": 20, "score": 75.0},
#         {"num": 4, "name": "Toby", "age": 24, "score": 92.0},
#         {"num": 5, "name": "Thomas", "age": 20, "score": 72.5},
#         {"num": 6, "name": "Charlie", "age": 20, "score": 69.0},
#         {"num": 7, "name": "Chris", "age": 21, "score": 88.0},
#         {"num": 8, "name": "Bill", "age": 21, "score": 90.0},
#         {"num": 9, "name": "Stefan", "age": 25, "score": 86.5},
#         {"num": 10, "name": "Steve", "age": 20, "score": 86.0}
#     ]'
#
## 4. Export data from 'my_table' into a JSONL file
#echo -e '\n-- export data from my_table to export_data.jsonl'
#curl --request GET \
#     --url "http://localhost:23817/databases/default_db/tables/my_table/docs/export?file_type=jsonl&offset=2&limit=7&row_limit=2" \
#     --header 'accept: application/json' \
#     --output "./export_data.jsonl"
#
## 5. Drop the table after operation is complete
#echo -e '\n-- drop my_table'
#curl --request DELETE \
#     --url http://localhost:23817/databases/default_db/tables/my_table \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{
#         "drop_option": "error"
#     }'
#
## 6. List all tables to verify 'my_table' is dropped
#echo -e '\n-- list tables, my_table should be dropped'
#curl --request GET \
#     --url http://localhost:23817/databases/default_db/tables \
#     --header 'accept: application/json'
