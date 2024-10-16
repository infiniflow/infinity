# create table 'tbl1'
echo -e '-- create tbl1'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1 \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {
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
             },
             {
                  "name": "sparse_column",
                  "type": "sparse,100,float,int"
             },
             {
                  "name": "year",
                  "type": "integer"
             },
             {
                  "name": "tensor",
                  "type": "tensor,4,float"
             }
        ]
    } '

# insert 4 rows into 'tbl1'
echo -e '\n\n-- insert 4 rows into tbl1'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/tbl1/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     [
         {
             "num": 1,
             "body": "unnecessary and harmful",
             "vec": [1.0, 1.2, 0.8, 0.9],
             "sparse_column": {"10":1.1, "20":2.2, "30": 3.3},
             "year": 2024,
             "tensor": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
         },
         {
             "num": 2,
             "body": "Office for Harmful Blooms",
             "vec": [4.0, 4.2, 4.3, 4.5],
             "sparse_column": {"40":4.4, "50":5.5, "60": 6.6},
             "year": 2023,
             "tensor": [[4.0, 0.0, 4.3, 4.5], [4.0, 4.2, 4.4, 5.0]]
         },
         {
             "num": 3,
             "body": "A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
             "vec": [4.0, 4.2, 4.3, 4.2],
             "sparse_column":  {"70":7.7, "80":8.8, "90": 9.9},
             "year": 2019,
             "tensor": [[0.9, 0.1, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
         },
         {
             "num": 4,
             "body": "The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.",
             "vec": [4.0, 4.2, 4.3, 4.5],
             "sparse_column":  {"20":7.7, "80":7.8, "90": 97.9},
             "year": 2018,
             "tensor": [[5.0, 4.2, 4.3, 4.5], [4.0, 4.2, 4.3, 4.4]]
         }
     ] '

# list tables, there are 'tbl1' at least.
echo -e '\n-- list tables, tbl1 should be included'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables \
     --header 'accept: application/json'

# show table tb1
echo -e '\n-- show tbl1'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1 \
     --header 'accept: application/json'

# show table tbl1 segments
echo -e '\n-- show tbl1 segments'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/segments \
     --header 'accept: application/json'

# show table tbl1 blocks
echo -e '\n-- show tbl1 blocks'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/segments/0/blocks \
     --header 'accept: application/json'

# show table tb1 columns
echo -e '\n-- show tbl1 columns'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/columns \
     --header 'accept: application/json'

# drop table column tbl1.year
echo -e '\n-- drop table column tbl1.year'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1/columns \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"column_names": ["year"]} '

# show table tbl1 columns again, tbl1.year will be gone
echo -e '\n-- show tbl1 columns, tbl1.year will be gone'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/tbl1/columns \
     --header 'accept: application/json'

# drop tbl1
echo -e '\n-- drop tbl1'
curl --request DELETE \
     --url http://localhost:23820/databases/default_db/tables/tbl1 \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {"drop_option": "error"} '

# list tables, 'tbl1' is dropped.
echo -e '\n-- list tables, tbl1 is dropped'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables \
     --header 'accept: application/json'
echo -e '\n'
