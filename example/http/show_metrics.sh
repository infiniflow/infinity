# Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

# create table 'my_table'
echo -e '-- create my_table'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data ' {
         "create_option": "ignore_if_exists",
         "fields": [
            {
                "name" : "num",
                "type" : "integer"
            },
            {
                "name" : "name",
                "type" : "varchar"
            },
            {
                "name" : "score",
                "type" : "float"
            }
        ]
    } '

echo -e '\n\n-- insert 5 rows into my_table'
curl --request POST \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     [
        {
            "num" : 1,
            "name" : "Tom",
            "score" : 90
        },
        {
            "num" : 2,
            "name" : "Henry",
            "score" : 70
        },
        {
            "num" : 3,
            "name" : "james",
            "score" : 70
        },
        {
            "num" : 4,
            "name" : "Toby",
            "score" : 92
        },
        {
            "num" : 5,
            "name" : "Tom",
            "score" : 90
        }
    ] '

# show all rows of 'my_table'
echo -e '\n\n-- select all rows of my_table'
curl --request GET \
     --url http://localhost:23820/databases/default_db/tables/my_table/docs \
     --header 'accept: application/json' \
     --header 'content-type: application/json' \
     --data '
     {
         "output":
         [
             "*"
         ]
     } '

# show buffer
echo -e '\n-- show buffer'
curl --request GET \
     --url http://localhost:23820/instance/buffer \
     --header 'accept: application/json'

# show profiles
echo -e '\n\n-- show profiles'
curl --request GET \
     --url http://localhost:23820/instance/profiles \
     --header 'accept: application/json'

# show memindex
echo -e '\n\n-- show memindex'
curl --request GET \
     --url http://localhost:23820/instance/memindex \
     --header 'accept: application/json'

# show queries
echo -e '\n\n-- show queries'
curl --request GET \
     --url http://localhost:23820/instance/queries \
     --header 'accept: application/json'

# show logs
echo -e '\n\n-- show logs'
curl --request GET \
     --url http://localhost:23820/instance/logs \
     --header 'accept: application/json'

# show delta checkpoints
echo -e '\n\n-- show delta checkpoint'
curl --request GET \
     --url http://localhost:23820/instance/delta_checkpoint \
     --header 'accept: application/json'

# show global checkpoints
echo -e '\n\n-- show global checkpoint'
curl --request GET \
     --url http://localhost:23820/instance/global_checkpoint \
     --header 'accept: application/json'

# show transactions
echo -e '\n\n-- show transactions'
curl --request GET \
     --url http://localhost:23820/instance/transactions \
     --header 'accept: application/json'

# show objects
echo -e '\n\n-- show objects'
curl --request GET \
     --url http://localhost:23820/instance/objects \
     --header 'accept: application/json'

# show files
echo -e '\n\n-- show files'
curl --request GET \
     --url http://localhost:23820/instance/files \
     --header 'accept: application/json'

# show memory
echo -e '\n\n-- show memory'
curl --request GET \
     --url http://localhost:23820/instance/memory \
     --header 'accept: application/json'

# show memory objects
echo -e '\n\n-- show memory objects'
curl --request GET \
     --url http://localhost:23820/instance/memory/objects \
     --header 'accept: application/json'

# show memory allocations
echo -e '\n\n-- show memory allocations'
curl --request GET \
     --url http://localhost:23820/instance/memory/allocations \
     --header 'accept: application/json'

# show memory allocations
echo -e '\n\n-- show memory allocations'
curl --request GET \
     --url http://localhost:23820/instance/memory/allocations \
     --header 'accept: application/json'

# force global checkpoints
echo -e '\n\n-- show memory allocations'
curl --request POST \
    --url http://localhost:23820/instance/flush \
    --header 'accept: application/json'

# compact table
curl --request POST \
    --url http://localhost:23820/instance/table/compact \
    --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' 
    {
        "db_name" : "default_db",
        "table_name" : "my_table"
    } '
