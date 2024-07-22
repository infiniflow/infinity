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

'''
This example is about connecting local infinity instance, creating table, ing data, importing file into a table, and exporting table's data
'''

import infinity
import os

current_path = os.path.abspath(__file__)
project_directory = os.path.dirname(current_path)

try:
    # open a local directory to store the data
    infinity_instance = infinity.connect("/var/infinity")

    # connect to server with 127.0.0.1
    # infinity_instance = infinity.connect(infinity.common.LOCAL_HOST)

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop my_table if it already exists
    db_instance.drop_table("my_table", infinity.common.ConflictType.Ignore)

    # Create a table named "my_table"
    table_instance = db_instance.create_table("my_table", {
        "num": {"type": "integer"},
        "name": {"type": "varchar"},
        "age": {"type": "integer"},
        "score": {"type": "float"},
    })

    # Insert 10 rows of data into the 'my_table'
    table_instance.insert(
        [
            {
                "num": 1,
                "name": "Tom",
                "age": 19, 
                "score": 90.5,
            },
            {
                "num": 2,
                "name": "Henry",
                "age": 20, 
                "score": 70.0,
            },
            {
                "num": 3,
                "name": "James",
                "age": 20,
                "score": 75.0,
            },
            {
                "num": 4,
                "name": "Toby",
                "age": 24,
                "score": 92.0,
            },
            {
                "num": 5,
                "name": "Thomas",
                "age": 20,
                "score": 72.5,
            },
            {
                "num": 6,
                "name": "Charlie",
                "age": 20,
                "score": 69.0,
            },
            {
                "num": 7,
                "body": "Chris",
                "age": 21,
                "score": 88.0,
            },
            {
                "num": 8,
                "name": "Bill",
                "age": 21,
                "score": 90.0,
            },
            {
                "num": 9,
                "name": "Stefan",
                "age": 25,
                "score": 86.5,
            },
            {
                "num": 10,
                "name": "Steven",
                "age": 20,
                "score": 86.0,
            },
        ]
    )

    # TODO also show how to export other type of file
    table_instance.export_data(project_directory + "/../test/data/csv/export_data.csv",
                               {"header": True, "delimiter": ",", "row_limit": 2}, ["num", "name", "score"])
    
    print("Your data is exported to /../test/data/csv/export_data.csv!")


    infinity_instance.disconnect()

except Exception as e:
    print(str(e))