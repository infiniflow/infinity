import infinity
import pandas as pd
import pytest
from infinity.common import ConflictType
from infinity.errors import ErrorCode


@pytest.fixture(scope="class")
def setup_class(request, http):
    if http:
        from infinity.infinity_http import infinity_http
        request.cls.infinity_obj = infinity_http()
    else:
        request.cls.infinity_obj = infinity.connect()
    yield
    request.cls.infinity_obj.disconnect()


@pytest.mark.usefixtures("setup_class")
@pytest.mark.usefixtures("suffix")
class TestNull:
    def test_insert_null_omit_column(self, suffix):
        """
        Insert NULL by omitting a nullable column.
        """
        db = self.infinity_obj.get_database("default_db")
        db.drop_table("test_null_omit" + suffix, ConflictType.Ignore)

        db.create_table(
            "test_null_omit" + suffix,
            {"a": {"type": "integer"},
             "b": {"type": "varchar"},
             "c": {"type": "integer", "constraints": ["not null"]}},
            ConflictType.Error,
        )
        table = db.get_table("test_null_omit" + suffix)

        # Omit b (nullable varchar) and a (nullable integer by default).
        table.insert([{"c": 1}])
        table.insert([{"c": 2, "a": 100}])

        res, _ = table.output(["a", "b", "c"]).to_df()
        pd.testing.assert_frame_equal(
            res,
            pd.DataFrame({
                "a": pd.array([pd.NA, 100], dtype="Int32"),
                "b": pd.array([pd.NA, pd.NA], dtype="string"),
                "c": pd.array([1, 2], dtype="Int32"),
            }),
        )

        # IS NULL on integer column
        res, _ = table.output(["a", "b", "c"]).filter("a IS NULL").to_df()
        pd.testing.assert_frame_equal(
            res,
            pd.DataFrame({
                "a": pd.array([pd.NA], dtype="Int32"),
                "b": pd.array([pd.NA], dtype="string"),
                "c": pd.array([1], dtype="Int32"),
            }),
        )

        # IS NOT NULL on varchar column
        res, _ = table.output(["a", "b", "c"]).filter("b IS NOT NULL").to_df()
        assert len(res) == 0

        res = db.drop_table("test_null_omit" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_insert_null_single_value(self, suffix):
        """
        Insert NULL into nullable columns via explicit None values.
        """
        db = self.infinity_obj.get_database("default_db")
        db.drop_table("test_null_val" + suffix, ConflictType.Ignore)

        db.create_table(
            "test_null_val" + suffix,
            {"id": {"type": "integer"},
             "name": {"type": "varchar"},
             "score": {"type": "float"}},
            ConflictType.Error,
        )
        table = db.get_table("test_null_val" + suffix)

        table.insert([{"id": 1, "name": "Alice", "score": 90.5}])
        table.insert([{"id": 2}])  # name=NULL, score=NULL
        table.insert([{"id": 3, "name": "Bob"}])  # score=NULL
        table.insert([{"id": 4, "score": 60.0}])  # name=NULL

        res, _ = table.output(["*"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(
            res.sort_values("id").reset_index(drop=True),
            pd.DataFrame({
                "id": pd.array([1, 2, 3, 4], dtype="Int32"),
                "name": pd.array(["Alice", pd.NA, "Bob", pd.NA], dtype="string"),
                "score": pd.array([90.5, pd.NA, pd.NA, 60.0], dtype="Float32"),
            }),
        )

        res = db.drop_table("test_null_val" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_is_null_is_not_null(self, suffix):
        """
        Test IS NULL and IS NOT NULL filters.
        """
        db = self.infinity_obj.get_database("default_db")
        db.drop_table("test_null_pred" + suffix, ConflictType.Ignore)

        db.create_table(
            "test_null_pred" + suffix,
            {"id": {"type": "integer"},
             "name": {"type": "varchar"},
             "score": {"type": "integer"}},
            ConflictType.Error,
        )
        table = db.get_table("test_null_pred" + suffix)

        table.insert([{"id": 1}])
        table.insert([{"id": 2, "name": "Alice"}])
        table.insert([{"id": 3, "name": "Bob", "score": 100}])

        # IS NULL
        res, _ = table.output(["id"]).filter("name IS NULL").to_df()
        pd.testing.assert_frame_equal(
            res,
            pd.DataFrame({"id": pd.array([1], dtype="Int32")}),
        )

        # IS NOT NULL
        res, _ = table.output(["id"]).filter("name IS NOT NULL").to_df()
        pd.testing.assert_frame_equal(
            res.sort_values("id").reset_index(drop=True),
            pd.DataFrame({"id": pd.array([2, 3], dtype="Int32")}),
        )

        # IS NULL on integer column
        res, _ = table.output(["id"]).filter("score IS NULL").to_df()
        pd.testing.assert_frame_equal(
            res,
            pd.DataFrame({"id": pd.array([1, 2], dtype="Int32")}),
        )

        # IS NOT NULL on integer column
        res, _ = table.output(["id"]).filter("score IS NOT NULL").to_df()
        pd.testing.assert_frame_equal(
            res,
            pd.DataFrame({"id": pd.array([3], dtype="Int32")}),
        )

        res = db.drop_table("test_null_pred" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_null_count_aggregate(self, suffix):
        """
        Test COUNT with NULL filter.
        """
        db = self.infinity_obj.get_database("default_db")
        db.drop_table("test_null_agg" + suffix, ConflictType.Ignore)

        db.create_table(
            "test_null_agg" + suffix,
            {"a": {"type": "integer"}, "b": {"type": "varchar"}},
            ConflictType.Error,
        )
        table = db.get_table("test_null_agg" + suffix)

        table.insert([{"a": 1}])
        table.insert([{"a": 2, "b": "x"}])
        table.insert([{"a": 3}])

        res, _ = table.output(["count(*)"]).filter("b IS NULL").to_df()
        assert res.iloc[0, 0] == 2

        res, _ = table.output(["count(*)"]).filter("b IS NOT NULL").to_df()
        assert res.iloc[0, 0] == 1

        res = db.drop_table("test_null_agg" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_null_with_not_null_constraint(self, suffix):
        """
        Omitting a NOT NULL column without a default must error.
        """
        db = self.infinity_obj.get_database("default_db")
        db.drop_table("test_null_nn" + suffix, ConflictType.Ignore)

        db.create_table(
            "test_null_nn" + suffix,
            {"a": {"type": "integer"},
             "b": {"type": "integer", "constraints": ["not null"]}},
            ConflictType.Error,
        )
        table = db.get_table("test_null_nn" + suffix)

        # Omit NOT NULL column b — should fail.
        with pytest.raises(Exception):
            table.insert([{"a": 1}])

        # Provide b — should succeed.
        table.insert([{"a": 2, "b": 2}])
        res, _ = table.output(["*"]).to_df()
        pd.testing.assert_frame_equal(
            res,
            pd.DataFrame({
                "a": pd.array([2], dtype="Int32"),
                "b": pd.array([2], dtype="Int32"),
            }),
        )

        res = db.drop_table("test_null_nn" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_null_default(self, suffix):
        """
        Explicit DEFAULT NULL column: omitted columns store NULL.
        """
        db = self.infinity_obj.get_database("default_db")
        db.drop_table("test_null_default" + suffix, ConflictType.Ignore)

        db.create_table(
            "test_null_default" + suffix,
            {"id": {"type": "integer"},
             "val": {"type": "varchar", "default": None}},
            ConflictType.Error,
        )
        table = db.get_table("test_null_default" + suffix)

        table.insert([{"id": 1}])
        table.insert([{"id": 2, "val": "hello"}])

        res, _ = table.output(["*"]).to_df()
        pd.testing.assert_frame_equal(
            res.sort_values("id").reset_index(drop=True),
            pd.DataFrame({
                "id": pd.array([1, 2], dtype="Int32"),
                "val": pd.array([pd.NA, "hello"], dtype="string"),
            }),
        )

        res = db.drop_table("test_null_default" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
