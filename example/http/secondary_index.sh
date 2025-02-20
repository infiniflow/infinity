#!/bin/bash

# Define base URL and database name
BASE_URL="http://localhost:23820"
DATABASE_NAME="default_db"
TABLE_NAME="my_table"

# Drop table 'my_table' if it exists
echo -e '-- Drop table my_table if exists'
curl --request DELETE \
     --url "${BASE_URL}/databases/${DATABASE_NAME}/tables/${TABLE_NAME}" \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{"drop_option": "ignore_if_not_exists"}'
echo ""

# Create table 'my_table'
echo -e '\n-- Create table my_table'
curl --request POST \
     --url "${BASE_URL}/databases/${DATABASE_NAME}/tables/${TABLE_NAME}" \
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
echo ""

# Insert 10 rows into 'my_table'
echo -e '\n-- Insert 10 rows into my_table'
curl --request POST \
     --url "${BASE_URL}/databases/${DATABASE_NAME}/tables/${TABLE_NAME}/docs" \
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
echo ""

# Export data from 'my_table' to a JSONL file
echo -e '\n-- Export data from my_table to a JSONL file'
curl --request GET \
     --url "${BASE_URL}/databases/${DATABASE_NAME}/tables/${TABLE_NAME}" \
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
echo ""

echo -e '\n-- Test done'