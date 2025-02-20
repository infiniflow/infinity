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
         {"num": 1, "name": "Tom", "score": 90.5},
         {"num": 2, "name": "Henry", "score": 70.0},
         {"num": 3, "name": "James", "score": 75.0},
         {"num": 4, "name": "Toby", "score": 92.0},
         {"num": 5, "name": "Thomas", "score": 72.5},
         {"num": 6, "name": "Charlie", "score": 69.0},
         {"num": 7, "name": "Chris", "score": 88.0},
         {"num": 8, "name": "Bill", "score": 90.0},
         {"num": 9, "name": "Stefan", "score": 86.5},
         {"num": 10, "name": "Steve", "score": 86.0}
     ]'

# Query rows from 'my_table' with filter: score > 80.0 and score <= 90.0
echo -e '\n-- Query rows from my_table with filter: score > 80.0 and score <= 90.0'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "output": ["num", "name", "score"],
         "filter": "(score > 80.0) and (score <= 90.0)"
     }'

# Query rows from 'my_table' with filter: num <> 9
echo -e '\n-- Query rows from my_table with filter: num <> 9'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "output": ["num", "name", "score"],
         "filter": "num <> 9"
     }'

echo -e '\n-- Test done'