## Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##      https://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
#
## Drop table 'my_table' if it exists
#echo -e '-- Drop table my_table if exists'
#curl --request DELETE \
#     --url http://localhost:23820/databases/default_db/tables/my_table \
#     --header "accept: application/json" \
#     --header "content-type: application/json" \
#     --data '{"drop_option": "ignore_if_not_exists"}'
#
## Create table 'my_table'
#echo -e '\n-- Create table my_table'
#curl --request POST \
#     --url http://localhost:23820/databases/default_db/tables/my_table \
#     --header "accept: application/json" \
#     --header "content-type: application/json" \
#     --data '{
#         "create_option": "ignore_if_exists",
#         "fields": [
#             {"name": "num", "type": "integer"},
#             {"name": "uuid", "type": "varchar"},
#             {"name": "score", "type": "float"}
#         ]
#     }'
#
## Insert 10 rows into 'my_table'
#echo -e '\n-- Insert 10 rows into my_table'
#curl --request POST \
#     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
#     --header "accept: application/json" \
#     --header "content-type: application/json" \
#     --data '[
#         {"num": 1, "uuid": "UUID-1-1", "score": 90.5},
#         {"num": 2, "uuid": "UUID-1-2", "score": 70.0},
#         {"num": 3, "uuid": "UUID-1-3", "score": 75.0},
#         {"num": 4, "uuid": "UUID-1-4", "score": 92.0},
#         {"num": 5, "uuid": "UUID-1-5", "score": 72.5},
#         {"num": 6, "uuid": "UUID-2-1", "score": 69.0},
#         {"num": 7, "uuid": "UUID-2-2", "score": 88.0},
#         {"num": 8, "uuid": "UUID-2-3", "score": 90.0},
#         {"num": 9, "uuid": "UUID-2-4", "score": 86.5},
#         {"num": 10, "uuid": "UUID-2-5", "score": 86.0}
#     ]'
#
## Create fulltext index on 'uuid' column
#echo -e '\n-- Create fulltext index on uuid column'
#curl --request POST \
#     --url http://localhost:23820/databases/default_db/tables/my_table/indexes/my_index \
#     --header "accept: application/json" \
#     --header "content-type: application/json" \
#     --data '{
#         "create_option": "error",
#         "fields": ["uuid"],
#         "index": {
#             "type": "fulltext",
#             "analyzer": "keyword"
#         }
#     }'
#
## Query rows with filter: score > 80.0 and score <= 90.0
#echo -e "\n-- Query rows with filter: score > 80.0 and score <= 90.0"
#curl --request GET \
#     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
#     --header "accept: application/json" \
#     --header "content-type: application/json" \
#     --data '{"output": ["*"], "filter": "(score > 80.0) and (score <= 90.0)"}'
#
## Query rows with fulltext filter: filter_fulltext('uuid', 'UUID-2-1 UUID-2-3')
#echo -e "\n-- Query rows with fulltext filter: filter_fulltext('uuid', 'UUID-2-1 UUID-2-3')"
#curl --request GET \
#     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
#     --header "accept: application/json" \
#     --header "content-type: application/json" \
#     --data "$(printf '{"output": ["*"], "filter": "filter_fulltext(\\"uuid\\", \\"UUID-2-1 UUID-2-3\\")"}')"
#
## Query rows with combined filter: score > 80.0 and score <= 90.0 and filter_fulltext('uuid', 'UUID-2-1 UUID-2-3')
#echo -e "\n-- Query rows with combined filter: score > 80.0 and score <= 90.0 and filter_fulltext('uuid', 'UUID-2-1 UUID-2-3')"
#curl --request GET \
#     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
#     --header "accept: application/json" \
#     --header "content-type: application/json" \
#     --data "$(printf '{"output": ["*"], "filter": "(score > 80.0) and (score <= 90.0) and filter_fulltext(\\"uuid\\", \\"UUID-2-1 UUID-2-3\\")"}')"
#
## Drop table 'my_table'
#echo -e "\n-- Drop table my_table"
#curl --request DELETE \
#     --url http://localhost:23820/databases/default_db/tables/my_table \
#     --header "accept: application/json" \
#     --header "content-type: application/json" \
#     --data '{"drop_option": "error"}'
#
#echo -e "\n-- Test done"