#!/bin/bash

# Create table 'my_table'
echo -e '-- create my_table'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "create_option": "ignore_if_exists",
         "fields": [
             {
                 "name": "num",
                 "type": "integer"
             },
             {
                 "name": "body",
                 "type": "varchar"
             },
             {
                 "name": "vec",
                 "type": "vector, 4, float"
             }
         ]
     }'

# Insert data into 'my_table'
echo -e '\n\n-- insert data into my_table'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '[
         {
             "num": 1,
             "body": "unnecessary and harmful",
             "vec": [1.0, 1.2, 0.8, 0.9]
         },
         {
             "num": 2,
             "body": "Office for Harmful Blooms",
             "vec": [4.0, 4.2, 4.3, 4.5]
         },
         {
             "num": 3,
             "body": "A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
             "vec": [4.0, 4.2, 4.3, 4.2]
         }
     ]'

# Query the data from 'my_table'
echo -e '\n-- query data from my_table'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json'

# Delete data from 'my_table' (Fix: Ensure correct filter format)
echo -e '\n-- delete data from my_table'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "filter": {"num": 2}
     }'

# Insert data again into 'my_table'
echo -e '\n-- insert data again into my_table'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '[
         {
             "num": 2,
             "body": "Office for Harmful Blooms",
             "vec": [4.0, 4.2, 4.3, 4.5]
         },
         {
             "num": 2,
             "body": "Office for Harmful Blooms",
             "vec": [4.0, 4.2, 4.3, 4.5]
         }
     ]'

# Query data again from 'my_table'
echo -e '\n-- query data again from my_table'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json'

# Update data in 'my_table' (Fix: Use POST for update as PATCH isn't recognized)
echo -e '\n-- update data in my_table'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "filter": {"num": 2},
         "set": {
             "body": "unnecessary and harmful",
             "vec": [14.0, 7.2, 0.8, 10.9]
         }
     }'

# Query all data after update from 'my_table'
echo -e '\n-- query all data after update from my_table'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json'

# Drop table 'my_table'
echo -e '\n-- drop my_table'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/my_table \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "drop_option": "error"
     }'

# List all tables to verify 'my_table' is dropped
echo -e '\n-- list tables, my_table should be dropped'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables \
     --header 'accept: application/json'
