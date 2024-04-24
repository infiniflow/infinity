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

import os
import pytest 

def run_sdk(): 
    os.system("cd test")
    os.system("python3 -m pytest -m 'not complex and not slow' test/test_database.py")

def run_http():
    os.system("cd test_http_api")
    os.system("python3 -m pytest -m 'not complex and not slow' test_http_api/test_index.py")   

if __name__ == '__main__':
    run_http()
