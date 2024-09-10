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

import infinity_embedded as infinity
#import infinity
import os
import sys

current_path = os.path.abspath(__file__)
project_directory = os.path.dirname(current_path)

try:
    # open a local directory(default = "/var/infinity") or connect to server(default = NetworkAddress("127.0.0.1", 23817)) to store the data
    infinity_instance = infinity.connect()

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop my_table if it already exists
    db_instance.drop_table("my_table", infinity.common.ConflictType.Ignore)

    # Create a table named "my_table"
    table_instance = db_instance.create_table("my_table", {
        "num": {"type": "integer"},
        "doc": {"type": "varchar"},
    })

    # TODO also show how to import other type of file
    table_instance.import_data(project_directory + "/../test/data/csv/fulltext_delete.csv",
                               {"header": True, "file_type": "csv", "delimiter": "\t"})

    result = table_instance.output(["num", "doc"]).to_pl()
    print(result)

    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)
