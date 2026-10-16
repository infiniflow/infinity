//! C++-compatible **catalog key space** for the RocksDB meta store
//! (RUST_REWRITE_PLAN.md §2.6.1, task #69).
//!
//! Frozen in `rust_engine/docs/CPP_FORMAT_SPEC.md` §8.2 from
//! `src/storage/common/kv_code_impl.cpp`. Every key is plain ASCII with `|` as
//! the separator and **decimal** ids, so a Rust writer produces byte-identical
//! keys to the C++ engine's `KeyEncode::*` helpers.
//!
//! Two families exist in the C++ tree and both are reproduced here:
//!   * the **new catalog** (`catalog|…`, `tbl|…`, `blk|…`, `idx|…`) used by
//!     `src/storage/catalog/meta/*`;
//!   * the **legacy txn** keys (`db|…`, `tbl|{db_name}|…`) used by
//!     `src/storage/txn/*`.
//!
//! Only the key encoding is implemented: the values (ColumnDef / IndexBase JSON,
//! the merge-operator semantics) and swapping the substrate for RocksDB remain.

/// `catalog|db|{db_name}|{ts}` → the database id.
pub fn catalog_db_key(db_name: &str, ts: u64) -> String {
    format!("catalog|db|{db_name}|{ts}")
}

/// Prefix of every [`catalog_db_key`] for a database name.
pub fn catalog_db_prefix(db_name: &str) -> String {
    format!("catalog|db|{db_name}|")
}

/// `db|{db_id}|{tag}` → a database-level tag (`comment`, `next_table_id`, …).
pub fn catalog_db_tag_key(db_id: &str, tag: &str) -> String {
    format!("db|{db_id}|{tag}")
}

/// `catalog|tbl|{db_id}|{table_name}|{ts}` → the table id.
pub fn catalog_table_key(db_id: &str, table_name: &str, ts: u64) -> String {
    format!("catalog|tbl|{db_id}|{table_name}|{ts}")
}

/// Prefix of every [`catalog_table_key`] for a table name.
pub fn catalog_table_prefix(db_id: &str, table_name: &str) -> String {
    format!("catalog|tbl|{db_id}|{table_name}|")
}

/// Prefix of every table in a database.
pub fn catalog_db_table_prefix(db_id: &str) -> String {
    format!("catalog|tbl|{db_id}|")
}

/// `tbl|{db_id}|{table_id}|{tag}` → a table-level tag
/// (`comment`, `next_segment_id`, `next_column_id`, `next_index_id`,
/// `unsealed_segment_id`, `ft_index_cache`).
pub fn catalog_table_tag_key(db_id: &str, table_id: &str, tag: &str) -> String {
    format!("tbl|{db_id}|{table_id}|{tag}")
}

/// `tbl|col|{db_id}|{table_id}|{column_name}|{ts}` → the ColumnDef JSON.
pub fn table_column_key(db_id: &str, table_id: &str, column_name: &str, ts: u64) -> String {
    format!("tbl|col|{db_id}|{table_id}|{column_name}|{ts}")
}

/// Prefix of every column record of a table.
pub fn table_column_prefix(db_id: &str, table_id: &str) -> String {
    format!("tbl|col|{db_id}|{table_id}|")
}

/// `col|{db_id}|{table_id}|{column_id}|{tag}` → a per-column tag.
pub fn column_tag_key(db_id: &str, table_id: &str, column_id: &str, tag: &str) -> String {
    format!("col|{db_id}|{table_id}|{column_id}|{tag}")
}

/// `catalog|seg|{db_id}|{table_id}|{segment_id}` → the segment's commit ts.
pub fn catalog_segment_key(db_id: &str, table_id: &str, segment_id: u32) -> String {
    format!("catalog|seg|{db_id}|{table_id}|{segment_id}")
}

/// Prefix of every segment of a table.
pub fn catalog_segment_prefix(db_id: &str, table_id: &str) -> String {
    format!("catalog|seg|{db_id}|{table_id}|")
}

/// `seg|{db_id}|{table_id}|{segment_id}|{tag}` → a segment tag
/// (`first_delete_ts`, `fast_rough_filter`).
pub fn catalog_segment_tag_key(db_id: &str, table_id: &str, segment_id: u32, tag: &str) -> String {
    format!("seg|{db_id}|{table_id}|{segment_id}|{tag}")
}

/// `catalog|blk|{db_id}|{table_id}|{segment_id}|{block_id}` → the block's ts.
pub fn catalog_block_key(db_id: &str, table_id: &str, segment_id: u32, block_id: u16) -> String {
    format!("catalog|blk|{db_id}|{table_id}|{segment_id}|{block_id}")
}

/// Prefix of every block of a segment.
pub fn catalog_block_prefix(db_id: &str, table_id: &str, segment_id: u32) -> String {
    format!("catalog|blk|{db_id}|{table_id}|{segment_id}|")
}

/// `blk|{db_id}|{table_id}|{segment_id}|{block_id}|{tag}` → a block tag
/// (`lock`, `fast_rough_filter`).
pub fn catalog_block_tag_key(
    db_id: &str,
    table_id: &str,
    segment_id: u32,
    block_id: u16,
    tag: &str,
) -> String {
    format!("blk|{db_id}|{table_id}|{segment_id}|{block_id}|{tag}")
}

/// `blk_col|{db_id}|{table_id}|{segment_id}|{block_id}|{column_id}|{tag}`.
pub fn catalog_block_column_tag_key(
    db_id: &str,
    table_id: &str,
    segment_id: u32,
    block_id: u16,
    column_id: &str,
    tag: &str,
) -> String {
    format!("blk_col|{db_id}|{table_id}|{segment_id}|{block_id}|{column_id}|{tag}")
}

/// `catalog|idx|{db_id}|{table_id}|{index_name}|{ts}` → the index id.
pub fn catalog_index_key(db_id: &str, table_id: &str, index_name: &str, ts: u64) -> String {
    format!("catalog|idx|{db_id}|{table_id}|{index_name}|{ts}")
}

/// Prefix of every index name of a table.
pub fn catalog_table_index_prefix(db_id: &str, table_id: &str) -> String {
    format!("catalog|idx|{db_id}|{table_id}|")
}

/// `idx|{db_id}|{table_id}|{index_id}|{tag}` → `index_base` JSON or
/// `segment_update_ts`.
pub fn catalog_index_tag_key(db_id: &str, table_id: &str, index_id: &str, tag: &str) -> String {
    format!("idx|{db_id}|{table_id}|{index_id}|{tag}")
}

/// `idx_seg|{db_id}|{table_id}|{index_id}|{segment_id}` → the commit ts.
pub fn catalog_index_segment_key(
    db_id: &str,
    table_id: &str,
    index_id: &str,
    segment_id: u32,
) -> String {
    format!("idx_seg|{db_id}|{table_id}|{index_id}|{segment_id}")
}

/// Prefix of every segment of an index.
pub fn catalog_index_segment_prefix(db_id: &str, table_id: &str, index_id: &str) -> String {
    format!("idx_seg|{db_id}|{table_id}|{index_id}|")
}

/// `idx_seg|…|{segment_id}|{tag}` → `next_chunk_id`, `mem_index`, `ft_info`.
pub fn catalog_index_segment_tag_key(
    db_id: &str,
    table_id: &str,
    index_id: &str,
    segment_id: u32,
    tag: &str,
) -> String {
    format!("idx_seg|{db_id}|{table_id}|{index_id}|{segment_id}|{tag}")
}

/// `idx_chunk|{db_id}|{table_id}|{index_id}|{segment_id}|{chunk_id}`.
pub fn catalog_index_chunk_key(
    db_id: &str,
    table_id: &str,
    index_id: &str,
    segment_id: u32,
    chunk_id: u64,
) -> String {
    format!("idx_chunk|{db_id}|{table_id}|{index_id}|{segment_id}|{chunk_id}")
}

/// Prefix of every chunk of a segment index.
pub fn catalog_index_chunk_prefix(
    db_id: &str,
    table_id: &str,
    index_id: &str,
    segment_id: u32,
) -> String {
    format!("idx_chunk|{db_id}|{table_id}|{index_id}|{segment_id}|")
}

/// `idx_chunk|…|{chunk_id}|{tag}` → the `chunk_info` JSON.
pub fn catalog_index_chunk_tag_key(
    db_id: &str,
    table_id: &str,
    index_id: &str,
    segment_id: u32,
    chunk_id: u64,
    tag: &str,
) -> String {
    format!("idx_chunk|{db_id}|{table_id}|{index_id}|{segment_id}|{chunk_id}|{tag}")
}

/// Global counters kept as bare keys (`src/storage/new_txn/new_txn_impl.cpp`).
pub const NEXT_DATABASE_ID: &str = "next_database_id";
/// Global next table id (bare key).
pub const NEXT_TABLE_ID: &str = "next_table_id";
/// `meta|version` — the catalog format version.
pub const META_VERSION: &str = "meta|version";

/// Drop tombstone for a table: `tbl|{db_name}|{tbl_name}|{ts}`.
pub fn drop_table_marker(db_name: &str, table_name: &str, ts: u64) -> String {
    format!("tbl|{db_name}|{table_name}|{ts}")
}

/// Drop tombstone for a segment: `seg|{db_name}|{tbl_name}|{segment_id}|{ts}`.
pub fn drop_segment_marker(db_name: &str, table_name: &str, segment_id: u32, ts: u64) -> String {
    format!("seg|{db_name}|{table_name}|{segment_id}|{ts}")
}

/// Drop tombstone for a block:
/// `blk|{db_name}|{tbl_name}|{segment_id}|{block_id}|{ts}`.
pub fn drop_block_marker(
    db_name: &str,
    table_name: &str,
    segment_id: u32,
    block_id: u16,
    ts: u64,
) -> String {
    format!("blk|{db_name}|{table_name}|{segment_id}|{block_id}|{ts}")
}

/// Drop tombstone for a block column:
/// `col|{db_name}|{tbl_name}|{segment_id}|{block_id}|{column_id}|{ts}`.
pub fn drop_column_marker(
    db_name: &str,
    table_name: &str,
    segment_id: u32,
    block_id: u16,
    column_id: &str,
    ts: u64,
) -> String {
    format!("col|{db_name}|{table_name}|{segment_id}|{block_id}|{column_id}|{ts}")
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn new_catalog_keys_match_the_cpp_encoders() {
        assert_eq!(catalog_db_key("db1", 7), "catalog|db|db1|7");
        assert_eq!(catalog_db_prefix("db1"), "catalog|db|db1|");
        assert_eq!(
            catalog_db_tag_key("1", "next_table_id"),
            "db|1|next_table_id"
        );
        assert_eq!(catalog_table_key("1", "t", 7), "catalog|tbl|1|t|7");
        assert_eq!(catalog_table_prefix("1", "t"), "catalog|tbl|1|t|");
        assert_eq!(catalog_db_table_prefix("1"), "catalog|tbl|1|");
        assert_eq!(
            catalog_table_tag_key("1", "2", "next_segment_id"),
            "tbl|1|2|next_segment_id"
        );
        assert_eq!(table_column_key("1", "2", "c", 7), "tbl|col|1|2|c|7");
        assert_eq!(table_column_prefix("1", "2"), "tbl|col|1|2|");
        assert_eq!(column_tag_key("1", "2", "3", "x"), "col|1|2|3|x");
    }

    #[test]
    fn segment_and_block_keys() {
        assert_eq!(catalog_segment_key("1", "2", 3), "catalog|seg|1|2|3");
        assert_eq!(catalog_segment_prefix("1", "2"), "catalog|seg|1|2|");
        assert_eq!(
            catalog_segment_tag_key("1", "2", 3, "first_delete_ts"),
            "seg|1|2|3|first_delete_ts"
        );
        assert_eq!(catalog_block_key("1", "2", 3, 4), "catalog|blk|1|2|3|4");
        assert_eq!(catalog_block_prefix("1", "2", 3), "catalog|blk|1|2|3|");
        assert_eq!(
            catalog_block_tag_key("1", "2", 3, 4, "lock"),
            "blk|1|2|3|4|lock"
        );
        assert_eq!(
            catalog_block_column_tag_key("1", "2", 3, 4, "5", "t"),
            "blk_col|1|2|3|4|5|t"
        );
    }

    #[test]
    fn index_keys() {
        assert_eq!(
            catalog_index_key("1", "2", "idx", 7),
            "catalog|idx|1|2|idx|7"
        );
        assert_eq!(catalog_table_index_prefix("1", "2"), "catalog|idx|1|2|");
        assert_eq!(
            catalog_index_tag_key("1", "2", "3", "index_base"),
            "idx|1|2|3|index_base"
        );
        assert_eq!(
            catalog_index_segment_key("1", "2", "3", 4),
            "idx_seg|1|2|3|4"
        );
        assert_eq!(
            catalog_index_segment_prefix("1", "2", "3"),
            "idx_seg|1|2|3|"
        );
        assert_eq!(
            catalog_index_segment_tag_key("1", "2", "3", 4, "next_chunk_id"),
            "idx_seg|1|2|3|4|next_chunk_id"
        );
        assert_eq!(
            catalog_index_chunk_key("1", "2", "3", 4, 5),
            "idx_chunk|1|2|3|4|5"
        );
        assert_eq!(
            catalog_index_chunk_prefix("1", "2", "3", 4),
            "idx_chunk|1|2|3|4|"
        );
        assert_eq!(
            catalog_index_chunk_tag_key("1", "2", "3", 4, 5, "chunk_info"),
            "idx_chunk|1|2|3|4|5|chunk_info"
        );
    }

    #[test]
    fn global_keys_and_drop_markers() {
        assert_eq!(NEXT_DATABASE_ID, "next_database_id");
        assert_eq!(NEXT_TABLE_ID, "next_table_id");
        assert_eq!(META_VERSION, "meta|version");

        assert_eq!(drop_table_marker("db", "t", 9), "tbl|db|t|9");
        assert_eq!(drop_segment_marker("db", "t", 1, 9), "seg|db|t|1|9");
        assert_eq!(drop_block_marker("db", "t", 1, 2, 9), "blk|db|t|1|2|9");
        assert_eq!(
            drop_column_marker("db", "t", 1, 2, "3", 9),
            "col|db|t|1|2|3|9"
        );
    }

    #[test]
    fn prefixes_are_true_prefixes_of_their_keys() {
        assert!(catalog_db_key("db1", 7).starts_with(&catalog_db_prefix("db1")));
        assert!(catalog_table_key("1", "t", 7).starts_with(&catalog_table_prefix("1", "t")));
        assert!(catalog_segment_key("1", "2", 3).starts_with(&catalog_segment_prefix("1", "2")));
        assert!(catalog_block_key("1", "2", 3, 4).starts_with(&catalog_block_prefix("1", "2", 3)));
        assert!(
            catalog_index_key("1", "2", "i", 7).starts_with(&catalog_table_index_prefix("1", "2"))
        );
        assert!(catalog_index_segment_key("1", "2", "3", 4)
            .starts_with(&catalog_index_segment_prefix("1", "2", "3")));
        assert!(catalog_index_chunk_key("1", "2", "3", 4, 5)
            .starts_with(&catalog_index_chunk_prefix("1", "2", "3", 4)));
    }
}
