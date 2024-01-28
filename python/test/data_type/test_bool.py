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

# create table with bool column
# insert table 0, 1, true, false to check correctness
# select all data from the table
# select with filter to check the result
# delete with bool filter
# update to set bool column a new value
# select with where
# test IS NULL, IS NOT NULL expression
# test in and not in expression
# test, select function(bool column)
# test, select 'not', 'or', 'and' on bool column
# select * from where (a AND 1) = 0 related cases
# select aggregate function on bool column
# select group by on bool column
# select sort by bool column