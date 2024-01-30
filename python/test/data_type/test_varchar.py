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

import pandas as pd
from numpy import dtype

import infinity
from infinity.common import NetworkAddress, REMOTE_HOST

# create a table, one column data are all inline varchar, output correctly.
# create a table, one column data are all inline varchar, and more than one block, output correctly.
# create a table, one column data are all outline varchar, output correctly.
# create a table, one column data are all outline varchar, and more than one block, output correctly.
# where clause with varchar column ==, <>, >, >=, <, <= operator.
# where clause with varchar column AND OR NOT, not supported.
# where clause with varchar column IS NULL, IS NOT NULL expression. supported?
# aggregate with varchar column
# group by on varchar column
# sort by on varchar column


