#!/bin/bash

# Connect to the local Infinity instance
echo -e "\n-- Connect to the local Infinity instance"
curl --request POST \
               --url http://localhost:23820/databases/default_db \
                                      --header 'accept: application/json' \
                                               --header 'content-type: application/json' \
                                                        --data '{"create_option": "ignore_if_exists"}'

# Create databases db1, db2, db3
echo -e "\n-- Create databases db1, db2, db3"
curl --request POST \
               --url http://localhost:23820/databases/db1 \
                                      --header 'accept: application/json' \
                                               --header 'content-type: application/json' \
                                                        --data '{"create_option": "ignore_if_exists"}'

curl --request POST \
               --url http://localhost:23820/databases/db2 \
                                      --header 'accept: application/json' \
                                               --header 'content-type: application/json' \
                                                        --data '{"create_option": "ignore_if_exists"}'

curl --request POST \
               --url http://localhost:23820/databases/db3 \
                                      --header 'accept: application/json' \
                                               --header 'content-type: application/json' \
                                                        --data '{"create_option": "ignore_if_exists"}'

# List all databases
echo -e "\n-- List all databases"
curl --request GET \
               --url http://localhost:23820/databases \
                                      --header 'accept: application/json'

# Create tables in db1
echo -e "\n-- Create tables in db1"
curl --request POST \
               --url http://localhost:23820/databases/db1/tables/table1 \
                                      --header 'accept: application/json' \
                                               --header 'content-type: application/json' \
                                                        --data '{
"create_option": "ignore_if_exists",
"fields": [
    {"name": "num", "type": "integer", "constraints": ["PRIMARY KEY"], "comment": "number column"},
    {"name": "body", "type": "varchar", "comment": "body column"},
    {"name": "vec", "type": "vector,4,float", "comment": "vec column"}
]
}'

curl --request POST \
               --url http://localhost:23820/databases/db1/tables/table2 \
                                      --header 'accept: application/json' \
                                               --header 'content-type: application/json' \
                                                        --data '{
                                                               "create_option": "ignore_if_exists",
"fields": [
{"name": "num", "type": "integer", "constraints": ["PRIMARY KEY"]},
{"name": "body", "type": "varchar"},
{"name": "vec", "type": "vector,4,float"}
]
}'

curl --request POST \
               --url http://localhost:23820/databases/db1/tables/table3 \
                                      --header 'accept: application/json' \
                                               --header 'content-type: application/json' \
                                                        --data '{
                                                               "create_option": "ignore_if_exists",
"fields": [
    {"name": "num", "type": "integer", "constraints": ["PRIMARY KEY"]},
    {"name": "body", "type": "varchar"},
    {"name": "vec", "type": "vector,4,float"}
]
}'

# List all tables in db1
echo -e "\n-- List all tables in db1"
curl --request GET \
               --url http://localhost:23820/databases/db1/tables \
                                      --header 'accept: application/json'

# Show columns of table1
echo -e "\n-- Show columns of table1"
curl --request GET \
               --url http://localhost:23820/databases/db1/tables/table1/columns \
                                      --header 'accept: application/json'

# Insert 3 rows into table1
echo -e "\n-- Insert 3 rows into table1"
curl --request POST \
               --url http://localhost:23820/databases/db1/tables/table1/docs \
                                      --header 'accept: application/json' \
                                               --header 'content-type: application/json' \
                                                        --data '[
{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]},
{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]},
{"num": 3, "body": "A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.", "vec": [4.0, 4.2, 4.3, 4.5]}
]'

# Query data from table1
echo -e "\n-- Query data from table1"
curl --request GET \
               --url http://localhost:23820/databases/db1/tables/table1/docs \
                                      --header 'accept: application/json' \
                                               --header 'content-type: application/json' \
                                                        --data '{
                                                               "output": ["num", "body", "vec"]
}'

echo -e "\n-- Test done"