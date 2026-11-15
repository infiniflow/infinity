//! Mapping lifecycle: a block maps only its own files, and dropping the last
//! handle takes its mappings off the process-wide counter.
//!
//! This is an integration test rather than a module unit test on purpose. It
//! asserts on `storage::mapped::mapping_count()` / `mapped_bytes()`, which are
//! **process-wide**, while `cargo test -p storage --lib` runs its ~500 unit
//! tests in parallel threads: any of them mapping a file at the same moment
//! shifts those counters, so the exact `before + n` / `before` comparison is a
//! race there (observed on a many-core Linux amd64 runner as `left: 1,
//! right: 0` - another test's mapping opened between the "held" assertion and
//! the "released" one). A test binary of its own runs nothing else, so the
//! counters here are quiet and the numbers are exact.

use storage::block_index::BlockIndex;
use storage::format::ColumnBody;
use storage::{BlockColumn, BlockColumns, BlockId};

fn id(segment: u32, block: u16) -> BlockId {
    BlockId {
        db_id: 1,
        table_id: 2,
        segment_id: segment,
        block_id: block,
    }
}

fn column(column_id: u64, values: &[i32]) -> BlockColumn {
    let data: Vec<u8> = values.iter().flat_map(|v| v.to_le_bytes()).collect();
    let valid = vec![true; values.len()];
    let body = ColumnBody::build(values.len(), 4, &data, &valid).unwrap();
    BlockColumn {
        column_id,
        body,
        outline: Vec::new(),
    }
}

fn column_file_len(root: &std::path::Path, id: BlockId, column_id: u64) -> u64 {
    let dir = root.join(storage::block_dir(
        id.db_id,
        id.table_id,
        id.segment_id,
        id.block_id,
    ));
    std::fs::metadata(dir.join(storage::column_file(column_id)))
        .unwrap()
        .len()
}

#[test]
fn every_block_maps_only_its_own_files() {
    let dir = tempfile::tempdir().unwrap();
    let before = storage::mapped::mapping_count();
    let before_bytes = storage::mapped::mapped_bytes();
    let mut index = BlockIndex::new(dir.path());
    for b in 0..4u16 {
        index
            .create_block(id(0, b), b as usize, 4, &[column(0, &[b as i32; 4])], 1, 1)
            .unwrap();
    }
    let file_lens: Vec<u64> = (0..4u16)
        .map(|b| column_file_len(dir.path(), id(0, b), 0))
        .collect();
    {
        let mut mapped = Vec::new();
        for b in 0..4u16 {
            let (_, block) = index.get_block(&id(0, b)).unwrap();
            let (body, _) = block.column(0).unwrap();
            assert_eq!(i32::from_le_bytes(body[0..4].try_into().unwrap()), b as i32);
            // The mapping covers this block's column file and nothing else.
            assert_eq!(block.bytes() as u64, file_lens[b as usize]);
            mapped.push(block);
        }
        assert_eq!(
            storage::mapped::mapping_count(),
            before + 4,
            "one mapping per block, shared while it is held"
        );
        assert_eq!(
            storage::mapped::mapped_bytes(),
            before_bytes + file_lens.iter().sum::<u64>() as usize,
            "the counter covers the mapped files, not the address space"
        );
    }
    assert_eq!(
        storage::mapped::mapping_count(),
        before,
        "dropping the last handle unmaps the block"
    );
    assert_eq!(storage::mapped::mapped_bytes(), before_bytes);
}
