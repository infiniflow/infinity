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
## Drop table 'my_table' if it already exists
#echo -e '-- Drop table my_table if exists'
#curl --request DELETE \
#     --url http://localhost:23820/databases/default_db/tables/my_table \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{"drop_option": "ignore_if_not_exists"}'
#
## Create table 'my_table'
#echo -e '\n-- Create table my_table'
#curl --request POST \
#     --url http://localhost:23820/databases/default_db/tables/my_table \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{
#         "create_option": "ignore_if_exists",
#         "fields": [
#             {"name": "num", "type": "integer"},
#             {"name": "body", "type": "varchar"},
#             {"name": "vec", "type": "vector, 4, float"}
#         ]
#     }'
#
## Insert 3 rows into 'my_table'
#echo -e '\n-- Insert 3 rows into my_table'
#curl --request POST \
#     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '[
#         {"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]},
#         {"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]},
#         {"num": 3, "body": "A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.", "vec": [4.0, 4.2, 4.3, 4.5]},
#         {"num": 4, "body": "The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.", "vec": [4.0, 4.2, 4.3, 4.5]}
#     ]'
#
## Create fulltext index on 'body' column
#echo -e '\n-- Create fulltext index on body column'
#curl --request POST \
#     --url http://localhost:23820/databases/default_db/tables/my_table/indexes/my_index \
#     --header 'accept: application/json' \
#     --header 'content-type: application/json' \
#     --data '{
#         "create_option": "error",
#         "fields": ["body"],
#         "index": {
#             "type": "fulltext"
#         }
#     }'
#
## Define search queries
#questions=(
#    "blooms"  # single term
#    "Bloom filter"  # OR multiple terms
#    '"Bloom filter"'  # phrase: adjacent multiple terms
#    "space efficient"  # OR multiple terms
#    "space:efficient"  # Escape reserved character ':', equivalent to: `space efficient`
#    '"space:efficient"'  # phrase and escape reserved character, equivalent to: `"space efficient"`
#    '"harmful chemical"~10'  # sloppy phrase
#)
#
## Execute search queries
#for question in "${questions[@]}"; do
#    echo -e "\n-- Query: $question"
#    curl --request GET \
#         --url http://localhost:23820/databases/default_db/tables/my_table/docs \
#         --header 'accept: application/json' \
#         --header 'content-type: application/json' \
#         --data "$(printf '{
#             "output": ["num", "body", "_score"],
#             "highlight": ["body"],
#             "search": [
#                 {
#                     "match_method": "text",
#                     "fields": "body",
#                     "matching_text": "%s",
#                     "topn": 10
#                 }
#             ]
#         }' "$question")"
#done
#
#echo -e "\n-- Test done"