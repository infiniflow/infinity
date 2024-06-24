# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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
This example is to connect local infinity instance, create table, insert data, import file into a table, and export table's data
'''

import infinity
import os

import os

current_path = os.path.abspath(__file__)

current_directory = os.path.dirname(current_path)
project_directory = os.path.dirname(current_path)

print(f"project dir: {project_directory}")
print(os.getcwd())

try:
    # open a local directory to store the data
    infinity_instance = infinity.connect("/var/infinity")

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop my_table if it already exists
    db_instance.drop_table("my_table", infinity.common.ConflictType.Ignore)

    # Create a table named "my_table"
    table_instance = db_instance.create_table("my_table", {
        "num": {"type": "integer"},
        "doc": {"type": "varchar"},
    })

    table_instance.import_data(current_directory + "/../test/data/csv/fulltext_delete.csv", {"file_type": "csv", "delimiter": "\t"})

    result = table_instance.output(["num", "doc"]).to_pl()
    print(result)

    infinity_instance.disconnect()

except Exception as e:
    print(str(e))
