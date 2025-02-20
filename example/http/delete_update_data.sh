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
             {"name": "body", "type": "varchar"},
             {"name": "vec", "type": "vector,4,float"}
         ]
     }'

# Insert 3 rows into 'my_table'
echo -e '\n-- Insert 3 rows into my_table'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '[
         {"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]},
         {"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]},
         {"num": 3, "body": "A Bloom filter is a space-efficient probabilistic data structure...", "vec": [4.0, 4.2, 4.3, 4.2]}
     ]'

# Query all rows from 'my_table'
echo -e '\n-- Query all rows from my_table'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{"output": ["num", "body"]}'

# Delete rows from 'my_table' where num = 2
echo -e '\n-- Delete rows from my_table where num = 2'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{"filter": "num = 2"}'

# Insert 2 rows into 'my_table' again
echo -e '\n-- Insert 2 rows into my_table again'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '[
         {"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]},
         {"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}
     ]'

# Update rows in 'my_table' where num = 2
echo -e '\n-- Update rows in my_table where num = 2'
curl --request PUT \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "update": {"body": "unnecessary and harmful", "vec": [14.0, 7.2, 0.8, 10.9]},
         "filter": "num = 2"
     }'

# Query all rows from 'my_table' after updates
echo -e '\n-- Query all rows from my_table after updates'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{"output": ["*"]}'