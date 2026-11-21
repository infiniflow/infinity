// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! Engine-level ports of `src/unit_test/storage/buffer/buffer_obj_ut.cpp`.
//!
//! The reference drives these cases through `CREATE TABLE` / `CREATE INDEX` /
//! `Append` / `DropTable` and then asserts catalog metadata: the segment and
//! block ids, each block's visible range and row count, the HNSW memory index,
//! and the buffer manager's configured limit. The engine exposes those through
//! `segment_ids`, `block_ids`, `block_visible_range`, `mem_index_len` and the
//! buffer-manager accessors, so the assertions are real rather than
//! re-implementing the buffer layer in the test.

use crate::Engine;
use parser::Literal;
use tempfile::tempdir;

/// C++ `BufferObjTest.test_hnsw_index_buffer_obj_shutdown`
/// (`buffer_obj_ut.cpp:297`).
#[tokio::test]
async fn test_hnsw_index_buffer_obj_shutdown() {
    const INSERT_N: usize = 2;
    const IMPORT_SIZE: usize = 8192;
    const DIM: usize = 128;

    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    // `test_buffer_obj_2.toml`: 8 * 4 * 128 * 8192 = 32 MiB.
    let memory_limit = 8 * 4 * 128 * 8192;
    e.set_buffer_memory_limit(memory_limit);
    assert_eq!(e.buffer_memory_limit(), memory_limit);

    e.execute("CREATE TABLE test_hnsw (col1 EMBEDDING(FLOAT,128));")
        .await
        .unwrap();
    e.execute(
        "CREATE INDEX hnsw_index ON test_hnsw(col1) USING HNSW \
         WITH (metric='l2', encode='plain', m=16, ef_construction=200);",
    )
    .await
    .unwrap();

    // `txn->Append`: two blocks of 8192 rows, row j = [j * 1000 + k, k<128].
    for _ in 0..INSERT_N {
        let rows: Vec<Vec<Literal>> = (0..IMPORT_SIZE)
            .map(|j| {
                vec![Literal::Vector(
                    (0..DIM).map(|k| (j * 1000 + k) as f64).collect(),
                )]
            })
            .collect();
        e.insert_literals("default_db.test_hnsw", None, rows)
            .await
            .unwrap();
    }

    // `txn->ListIndex` / `GetTableIndexMeta` succeed for the HNSW index.
    e.execute("SHOW TABLE test_hnsw INDEX hnsw_index;")
        .await
        .unwrap();

    // `table_meta->GetSegmentIDs1() == {0}`.
    assert_eq!(e.segment_ids("test_hnsw").await, vec![0]);
    // `SegmentIndexMeta(0).GetMemIndex() != nullptr`: both appended blocks are
    // in the memory segment (dumps happen only at `memindex_capacity`).
    assert_eq!(
        e.mem_index_len("test_hnsw", "col1"),
        Some(INSERT_N * IMPORT_SIZE)
    );

    // Drop table + cleanup (`CleanupTask`): the table entry, memory index and
    // block metadata are released.
    e.execute("DROP TABLE test_hnsw;").await.unwrap();
    assert!(!e.table_exists("default_db.test_hnsw"));
    assert_eq!(e.mem_index_len("test_hnsw", "col1"), None);
    assert!(e.block_ids("test_hnsw", 0).is_empty());
    // The block mappings went with the table (the reference frees the
    // `BufferObject`s a dropped table owned).
    assert!(e.buffer_objects().is_empty());
    assert_eq!(e.buffer_used_bytes(), 0);
}

/// C++ `BufferObjTest.test_big_with_gc_and_cleanup`
/// (`buffer_obj_ut.cpp:500`).
#[tokio::test]
async fn test_big_with_gc_and_cleanup() {
    const INSERT_N: usize = 256;
    const IMPORT_SIZE: usize = 8192;

    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    // `test_buffer_obj.toml`: 512 KiB.
    let memory_limit = 512 * 1024;
    e.set_buffer_memory_limit(memory_limit);
    assert_eq!(e.buffer_memory_limit(), memory_limit);

    e.execute("CREATE TABLE table1 (col1 BIGINT);")
        .await
        .unwrap();
    for i in 0..INSERT_N {
        let rows: Vec<Vec<Literal>> = (0..IMPORT_SIZE)
            .map(|j| vec![Literal::Int((i * 1000 + j) as i64)])
            .collect();
        e.insert_literals("default_db.table1", None, rows)
            .await
            .unwrap();
    }

    // `GetTableMeta`: one segment; its block list has `kInsertN` entries.
    assert_eq!(e.segment_ids("table1").await, vec![0]);

    // Materialise the C++ block layout (the reference's flushed blocks).
    assert_eq!(
        e.write_blocks("table1").await.unwrap(),
        INSERT_N * IMPORT_SIZE
    );
    let block_ids = e.block_ids("table1", 0);
    assert_eq!(
        block_ids,
        (0..INSERT_N as u32).collect::<Vec<_>>(),
        "block ids are 0..kInsertN"
    );

    for (idx, block_id) in block_ids.iter().enumerate() {
        let block_id = *block_id as u16;
        // `NewCatalog::GetBlockVisibleRange`: one full range, then `Next`
        // returns false; `block_offset_end()` is the block capacity.
        assert_eq!(
            e.block_visible_range("table1", 0, block_id),
            Some((0, IMPORT_SIZE)),
            "visible range of block {idx}"
        );
        assert_eq!(
            e.block_row_count("table1", 0, block_id),
            Some(IMPORT_SIZE),
            "row count of block {idx}"
        );
    }

    // Scan every block through the bounded buffer manager: each row reads
    // back as `idx * 1000 + row_id`, and the pool never exceeds its budget
    // (the GC/eviction the reference exercises with 64 KiB blocks under a
    // 512 KiB budget).
    let (_, rows) = e.read_blocks_pruned("table1", &[], &[]).await.unwrap();
    assert_eq!(rows.len(), INSERT_N * IMPORT_SIZE);
    // Reading a block maps it instead of copying it, and the shared map is an
    // LRU over those mappings: a block beyond the budget evicts the least
    // recently used one, so a table larger than the budget does not leave one
    // mapping per block in the process (what the reference's GC does here).
    assert!(
        e.buffer_used_bytes() <= memory_limit,
        "{} bytes mapped against a {memory_limit}-byte budget",
        e.buffer_used_bytes()
    );
    assert!(
        e.buffer_objects().len() < INSERT_N,
        "a table larger than the budget does not keep every mapping"
    );
    for idx in 0..INSERT_N {
        for row_id in 0..IMPORT_SIZE {
            assert_eq!(
                rows[idx * IMPORT_SIZE + row_id].1[0],
                (idx * 1000 + row_id).to_string(),
                "block {idx} row {row_id}"
            );
        }
    }

    // `DROP TABLE` then cleanup: no block metadata survives.
    e.execute("DROP TABLE table1;").await.unwrap();
    assert!(!e.table_exists("default_db.table1"));
    assert!(e.block_ids("table1", 0).is_empty());
}
