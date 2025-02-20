#!/bin/bash

# Connect to the local Infinity instance and use the default database
echo -e "\n-- Connect to Infinity and use default_db"
curl --request POST \
     --url http://localhost:23820/databases/default_db \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{"create_option": "ignore_if_exists"}'

# Drop table 'my_table' if it exists
echo -e "\n-- Drop table my_table if exists"
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/my_table \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{"drop_option": "ignore_if_not_exists"}'

# Create table 'my_table'
echo -e "\n-- Create table my_table"
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "create_option": "ignore_if_exists",
         "fields": [
             {"name": "num", "type": "integer"},
             {"name": "body", "type": "varchar"},
             {"name": "vec", "type": "vector,4,float"},
             {"name": "sparse", "type": "sparse,100,float,int"},
             {"name": "year", "type": "integer"},
             {"name": "tensor", "type": "tensor,4,float"}
         ]
     }'

# Insert 4 rows into 'my_table'
echo -e "\n-- Insert 4 rows into my_table"
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '[
         {
             "num": 1,
             "body": "unnecessary and harmful",
             "vec": [1.0, 1.2, 0.8, 0.9],
             "sparse": {"10": 1.1, "20": 2.2, "30": 3.3},
             "year": 2024,
             "tensor": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
         },
         {
             "num": 2,
             "body": "Office for Harmful Blooms",
             "vec": [4.0, 4.2, 4.3, 4.5],
             "sparse": {"40": 4.4, "50": 5.5, "60": 6.6},
             "year": 2023,
             "tensor": [[4.0, 0.0, 4.3, 4.5], [4.0, 4.2, 4.4, 5.0]]
         },
         {
             "num": 3,
             "body": "A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
             "vec": [4.0, 4.2, 4.3, 4.2],
             "sparse": {"70": 7.7, "80": 8.8, "90": 9.9},
             "year": 2019,
             "tensor": [[0.9, 0.1, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
         },
         {
             "num": 4,
             "body": "The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.",
             "vec": [4.0, 4.2, 4.3, 4.5],
             "sparse": {"20": 7.7, "80": 7.8, "90": 97.9},
             "year": 2018,
             "tensor": [[4.0, 4.2, 4.3, 4.5], [4.0, 4.2, 4.3, 4.4]]
         }
     ]'

# Create index on 'body' column
echo -e "\n-- Create index on body column"
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table/indexes/my_index \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "fields": ["body"],
         "index": {"type": "fulltext"},
         "create_option": "error"
     }'

# Query with dense vector, sparse vector, full-text search, and tensor reranking
echo -e "\n-- Query with dense vector, sparse vector, full-text search, and tensor reranking"
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '{
         "output": ["num", "body", "vec", "sparse", "year", "tensor", "_score"],
         "search": [
             {
                 "match_method": "dense",
                 "fields": "vec",
                 "query_vector": [3.0, 2.8, 2.7, 3.1],
                 "element_type": "float",
                 "metric_type": "cosine",
                 "topn": 3
             },
             {
                 "match_method": "sparse",
                 "fields": "sparse",
                 "query_vector": {"0": 1.0, "20": 2.0, "80": 3.0},
                 "metric_type": "ip",
                 "topn": 3
             },
             {
                 "match_method": "text",
                 "fields": "body",
                 "matching_text": "blooms",
                 "topn": 10
             },
             {
                 "fusion_method": "match_tensor",
                 "topn": 3,
                 "params": {
                     "field": "tensor",
                     "element_type": "float",
                     "query_tensor": [[0.9, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
                 }
             }
         ],
         "filter": "year < 2024"
     }'

echo -e "\n-- Test done"