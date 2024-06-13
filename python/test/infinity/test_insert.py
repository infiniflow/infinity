import pytest
from common import common_values
from src.test_insert import TestInsert
from infinity.common import ConflictType

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_REMOTE_HOST
    def test_insert(self):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_version()
        test_infinity_obj._test_insert_basic()
        test_infinity_obj._test_insert_varchar()
        test_infinity_obj._test_insert_big_varchar()
        test_infinity_obj._test_insert_embedding()
        test_infinity_obj._test_insert_big_embedding()
        test_infinity_obj._test_insert_big_embedding_float()
        test_infinity_obj._test_insert_exceed_block_size()
        test_infinity_obj._test_insert_data_into_non_existent_table()
        test_infinity_obj._test_insert_data_into_index_created_table()
        test_infinity_obj._test_insert_table_with_10000_columns()
        test_infinity_obj._test_read_after_shutdown()
        test_infinity_obj._test_batch_insert()
        test_infinity_obj._test_insert_zero_column()

        # todo support tensor
        # test_infinity_obj._test_insert_tensor()
        # test_infinity_obj._test_insert_tensor_array()


    @pytest.mark.parametrize("types", ["vector,65535,int", "vector,65535,float"])
    @pytest.mark.parametrize("types_examples", [[{"c1": [1] * 65535}],
                                                [{"c1": [4] * 65535}],
                                                [{"c1": [-9999999] * 65535}],
                                                [{"c1": [1.1] * 65535}],
                                                [{"c1": [-9999999.988] * 65535}],
                                                ])
    def test_insert_big_embedding_various_type(self, types, types_examples):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_insert_big_embedding_various_type(types, types_examples)


    # insert primitive data type not aligned with table definition
    @pytest.mark.parametrize("types", common_values.types_array)
    @pytest.mark.parametrize("types_example", common_values.types_example_array)
    def test_insert_data_not_aligned_with_table_definition(self, types, types_example):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_insert_data_not_aligned_with_table_definition(types, types_example)

    @pytest.mark.parametrize("types", common_values.types_array)
    def test_insert_empty_into_table(self, types):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_insert_empty_into_table(types)

    @pytest.mark.parametrize("values", [[{"c1": 1}], [{"c1": 1, "c2": 1, "c3": 1}]])
    def test_insert_with_not_matched_columns(self, values):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_insert_with_not_matched_columns(values)
    @pytest.mark.parametrize("values", [[{"c1": pow(2, 63) - 1, "c2": pow(2, 63) - 1}]])
    def test_insert_with_exceeding_invalid_value_range(self, values):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_insert_with_exceeding_invalid_value_range(values)

    # batch insert, within limit
    @pytest.mark.parametrize("batch", [10, 1024, 2048])
    def test_batch_insert_within_limit(self, batch):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_batch_insert_within_limit(batch)

    @pytest.mark.parametrize("batch", [10, 1024])
    @pytest.mark.parametrize("types", [(1, False), (1.1, False), ("1#$@!adf", False), ([1, 2, 3], True)])
    def test_insert_with_invalid_data_type(self, batch, types):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_insert_with_invalid_data_type(batch, types)

    @pytest.mark.parametrize("batch", [10, 1024])
    def test_batch_insert_with_invalid_column_count(self, batch):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_batch_insert_with_invalid_column_count(batch)
    @pytest.mark.parametrize('column_types', ["varchar"])
    @pytest.mark.parametrize('column_types_example', [[1, 2, 3], [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]])
    def test_various_insert_types(self, column_types, column_types_example):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_various_insert_types(column_types, column_types_example)

    @pytest.mark.complex
    @pytest.mark.skip(reason="TODO")
    def test_insert_with_invalid_column_name(self):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_insert_with_invalid_column_name()

    @pytest.mark.parametrize("column_name", [
        "c2",
        "$%#$sadf",
        # 1,
        # 2.2,
        # [1],
        # (1, "adsf"),
        # {"1": 1}
    ])
    def test_insert_no_match_column(self, column_name):
        test_infinity_obj = TestInsert(self.uri)
        test_infinity_obj._test_insert_no_match_column(column_name)
