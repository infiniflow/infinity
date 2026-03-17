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

import infinity
import infinity.index as index
import pytest
from common import common_values
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.infinity_http import infinity_http


@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")


@pytest.fixture(scope="class")
def setup_class(request, http):
    if http:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity_http()
    else:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity.connect(uri)
    request.cls.uri = uri
    yield
    request.cls.infinity_obj.disconnect()


@pytest.mark.usefixtures("setup_class")
@pytest.mark.usefixtures("suffix")
class TestSecondaryIndex:
    """Test secondary index functionality, especially low cardinality scenarios"""

    def test_secondary_index_low_cardinality_int(self, suffix):
        """
        Test secondary index with low cardinality integer data.
        This tests the fix for bitmap size calculation in low cardinality scenarios.
        """
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_secondary_low_card_int" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table(
            "test_secondary_low_card_int" + suffix,
            {"c1": {"type": "int"}, "c2": {"type": "varchar"}},
            ConflictType.Error
        )
        assert table_obj is not None

        # Insert data with low cardinality (only 3 distinct values: 1, 2, 3)
        # But many rows to test the bitmap handling
        for i in range(30):
            value = (i % 3) + 1  # Values: 1, 2, 3, 1, 2, 3, ...
            table_obj.insert([{"c1": value, "c2": f"text_{i}"}])

        # Create secondary index AFTER inserting data (to avoid dump issues)
        res = table_obj.create_index(
            "idx_c1",
            index.IndexInfo("c1", index.IndexType.Secondary, {"cardinality": "low"}),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # Test queries with filter on the indexed column
        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = 1").to_pl()
        assert len(res) == 10  # Should have 10 rows with c1=1

        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = 2").to_pl()
        assert len(res) == 10  # Should have 10 rows with c1=2

        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = 3").to_pl()
        assert len(res) == 10  # Should have 10 rows with c1=3

        # Clean up
        res = table_obj.drop_index("idx_c1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_secondary_low_card_int" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_secondary_index_low_cardinality_bool(self, suffix):
        """
        Test secondary index with boolean column (extreme low cardinality - only 2 values).
        This specifically tests the low cardinality optimization path.
        """
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_secondary_low_card_bool" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table(
            "test_secondary_low_card_bool" + suffix,
            {"c1": {"type": "bool"}, "c2": {"type": "int"}},
            ConflictType.Error
        )
        assert table_obj is not None

        # Insert data with boolean values
        for i in range(20):
            table_obj.insert([{"c1": i % 2 == 0, "c2": i}])

        # Create secondary index AFTER inserting data (to avoid dump issues)
        res = table_obj.create_index(
            "idx_c1",
            index.IndexInfo("c1", index.IndexType.Secondary, {"cardinality": "low"}),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # Query for true values
        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = true").to_pl()
        assert len(res) == 10  # Even indices: 0, 2, 4, ..., 18

        # Query for false values
        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = false").to_pl()
        assert len(res) == 10  # Odd indices: 1, 3, 5, ..., 19

        # Clean up
        res = table_obj.drop_index("idx_c1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_secondary_low_card_bool" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_secondary_index_low_cardinality_varchar(self, suffix):
        """
        Test secondary index with low cardinality varchar data.
        """
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_secondary_low_card_varchar" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table(
            "test_secondary_low_card_varchar" + suffix,
            {"c1": {"type": "varchar"}, "c2": {"type": "int"}},
            ConflictType.Error
        )
        assert table_obj is not None

        # Insert data with few distinct varchar values
        categories = ["A", "B", "C"]
        for i in range(30):
            category = categories[i % 3]
            table_obj.insert([{"c1": category, "c2": i}])

        # Create secondary index AFTER inserting data (to avoid dump issues)
        res = table_obj.create_index(
            "idx_c1",
            index.IndexInfo("c1", index.IndexType.Secondary, {"cardinality": "low"}),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # Query for each category
        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = 'A'").to_pl()
        assert len(res) == 10  # i % 3 == 0

        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = 'B'").to_pl()
        assert len(res) == 10  # i % 3 == 1

        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = 'C'").to_pl()
        assert len(res) == 10  # i % 3 == 2

        # Clean up
        res = table_obj.drop_index("idx_c1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_secondary_low_card_varchar" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_secondary_index_high_cardinality(self, suffix):
        """
        Test secondary index with high cardinality data (traditional path).
        """
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_secondary_high_card" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table(
            "test_secondary_high_card" + suffix,
            {"c1": {"type": "int"}, "c2": {"type": "varchar"}},
            ConflictType.Error
        )
        assert table_obj is not None

        # Insert data with high cardinality (all values are unique)
        for i in range(50):
            table_obj.insert([{"c1": i, "c2": f"text_{i}"}])

        # Create secondary index AFTER inserting data
        res = table_obj.create_index(
            "idx_c1",
            index.IndexInfo("c1", index.IndexType.Secondary),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # Query for specific values
        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = 25").to_pl()
        assert len(res) == 1

        res, extra = table_obj.output(["c1", "c2"]).filter("c1 >= 40").to_pl()
        assert len(res) == 10

        # Clean up
        res = table_obj.drop_index("idx_c1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_secondary_high_card" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_secondary_index_insert_after_index_creation(self, suffix):
        """
        Test inserting data after index creation and verify correctness.
        """
        db_obj = self.infinity_obj.get_database("default_db")
        res = db_obj.drop_table("test_secondary_insert_after" + suffix, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table(
            "test_secondary_insert_after" + suffix,
            {"c1": {"type": "int"}, "c2": {"type": "varchar"}},
            ConflictType.Error
        )
        assert table_obj is not None

        # Insert first batch
        for i in range(15):
            table_obj.insert([{"c1": i % 5, "c2": f"batch1_{i}"}])

        # Create secondary index AFTER inserting first batch
        res = table_obj.create_index(
            "idx_c1",
            index.IndexInfo("c1", index.IndexType.Secondary),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # Verify first batch
        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = 0").to_pl()
        assert len(res) == 3

        # Insert second batch
        for i in range(15, 30):
            table_obj.insert([{"c1": i % 5, "c2": f"batch2_{i}"}])

        # Verify both batches
        res, extra = table_obj.output(["c1", "c2"]).filter("c1 = 0").to_pl()
        assert len(res) == 6

        # Clean up
        res = table_obj.drop_index("idx_c1", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_secondary_insert_after" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
