# Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

# Drop table 'my_table' if it exists
echo -e '-- Drop table my_table if exists'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/my_table \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{"drop_option": "ignore_if_not_exists"}'

# Create table 'my_table'
echo -e '\n-- Create table my_table'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "create_option": "ignore_if_exists",
         "fields": [
             {"name": "num", "type": "integer"},
             {"name": "name", "type": "varchar"},
             {"name": "age", "type": "integer"},
             {"name": "score", "type": "float"}
         ]
     }'

# Insert 10 rows into 'my_table'
echo -e '\n-- Insert 10 rows into my_table'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '[
         {"num": 1, "name": "Tom", "age": 19, "score": 90.5},
         {"num": 2, "name": "Henry", "age": 20, "score": 70.0},
         {"num": 3, "name": "James", "age": 20, "score": 75.0},
         {"num": 4, "name": "Toby", "age": 24, "score": 92.0},
         {"num": 5, "name": "Thomas", "age": 20, "score": 72.5},
         {"num": 6, "name": "Charlie", "age": 20, "score": 69.0},
         {"num": 7, "name": "Chris", "age": 21, "score": 88.0},
         {"num": 8, "name": "Bill", "age": 21, "score": 90.0},
         {"num": 9, "name": "Stefan", "age": 25, "score": 86.5},
         {"num": 10, "name": "Steve", "age": 20, "score": 86.0}
     ]'

# Export data from 'my_table' to a JSONL file
echo -e '\n-- Export data from my_table to a JSONL file'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "file_path": "'$(pwd)'/export_data.jsonl",
         "file_type": "jsonl",
         "offset": 2,
         "limit": 7,
         "row_limit": 2,
         "output": ["num", "name", "score"]
     }'

echo -e '\n-- Test done'