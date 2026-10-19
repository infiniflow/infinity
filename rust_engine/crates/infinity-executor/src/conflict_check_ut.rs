//! Port of `ConflictCheckTest.conflict_check_delete`
//! (`src/unit_test/storage/txn/conflict_check_ut.cpp`).
//!
//! Five blocks of concurrent deletes: three transactions delete the same row
//! (the first committer wins, the others get `kTxnConflict`), then two
//! transactions with overlapping delete sets. After each block the live row
//! count drops by exactly the number of rows the winner deleted.

use crate::{del_key, Engine};
use infinity_common::Error;
use infinity_expression::Value;
use infinity_txn::Txn;
use tempfile::tempdir;

fn expect_conflict(e: &Engine, table: &str, seqs: &[usize], txn: Txn) {
    assert!(matches!(
        e.commit_with_deletes(table, seqs, txn),
        Err(Error::TxnConflict(_))
    ));
}

#[test]
fn conflict_check_delete() {
    let dir = tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE table1 (col1 INT);").unwrap();
    e.execute("INSERT INTO table1 VALUES (0),(1),(2),(3),(4),(5),(6),(7),(8),(9);")
        .unwrap();

    let table = "default_db.table1";
    let rows = e.fetch_rows_with_ids(table).unwrap();
    let seq_of: Vec<usize> = (0..10)
        .map(|row| {
            rows.iter()
                .find(|(_, values)| {
                    matches!(values.first(), Some(Value::Int(value)) if *value == row)
                })
                .unwrap_or_else(|| panic!("row {row} not found"))
                .0
        })
        .collect();

    let delete_row = |seqs: &[usize]| {
        let mut txn = e.txn.begin();
        for seq in seqs {
            txn.delete(del_key(table, *seq));
        }
        txn
    };

    {
        let txn1 = delete_row(&[seq_of[0]]);
        let txn2 = delete_row(&[seq_of[0]]);
        let txn3 = delete_row(&[seq_of[0]]);

        e.commit_with_deletes(table, &[seq_of[0]], txn1).unwrap();
        expect_conflict(&e, table, &[seq_of[0]], txn2);
        expect_conflict(&e, table, &[seq_of[0]], txn3);
        assert_eq!(e.row_count(table).unwrap(), 9);
    }
    {
        let txn1 = delete_row(&[seq_of[1]]);
        let txn2 = delete_row(&[seq_of[1]]);
        let txn3 = delete_row(&[seq_of[1]]);

        e.commit_with_deletes(table, &[seq_of[1]], txn2).unwrap();
        expect_conflict(&e, table, &[seq_of[1]], txn1);
        expect_conflict(&e, table, &[seq_of[1]], txn3);
        assert_eq!(e.row_count(table).unwrap(), 8);
    }
    {
        let txn1 = delete_row(&[seq_of[2]]);
        let txn2 = delete_row(&[seq_of[2]]);
        let txn3 = delete_row(&[seq_of[2]]);

        e.commit_with_deletes(table, &[seq_of[2]], txn3).unwrap();
        expect_conflict(&e, table, &[seq_of[2]], txn2);
        expect_conflict(&e, table, &[seq_of[2]], txn1);
        assert_eq!(e.row_count(table).unwrap(), 7);
    }
    {
        let txn1 = delete_row(&[seq_of[3]]);
        let txn2 = delete_row(&[seq_of[3], seq_of[4]]);

        e.commit_with_deletes(table, &[seq_of[3]], txn1).unwrap();
        expect_conflict(&e, table, &[seq_of[3], seq_of[4]], txn2);
        assert_eq!(e.row_count(table).unwrap(), 6);
    }
    {
        let txn1 = delete_row(&[seq_of[5], seq_of[6]]);
        let txn2 = delete_row(&[seq_of[5]]);

        e.commit_with_deletes(table, &[seq_of[5], seq_of[6]], txn1)
            .unwrap();
        expect_conflict(&e, table, &[seq_of[5]], txn2);
        assert_eq!(e.row_count(table).unwrap(), 4);
    }
}
