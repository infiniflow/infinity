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
import polars as pl

import common_values
import infinity


class TestShowTables:

    def test_show_tables(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        db = infinity_obj.get_database("default")

        with pl.Config(fmt_str_lengths=1000):
            res = db.show_tables()
            print(res)
            # check the polars dataframe
            assert res.columns == ["database", "table", "type", "column_count", "row_count", "segment_count",
                                   "block_count", "segment_capacity"]
