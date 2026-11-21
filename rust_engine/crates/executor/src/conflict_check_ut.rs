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

//! Port of `ConflictCheckTest.conflict_check_delete`
//! (`src/unit_test/storage/txn/conflict_check_ut.cpp`).
//!
//! Five blocks of concurrent deletes: three transactions delete the same row
//! (the first committer wins, the others get `kTxnConflict`), then two
//! transactions with overlapping delete sets. After each block the live row
//! count drops by exactly the number of rows the winner deleted.

use crate::Engine;
use ::common::Error;
use expression::Value;
use tempfile::tempdir;
use txn::Txn;

async fn expect_conflict(e: &Engine, table: &str, seqs: &[usize], txn: Txn) {
    assert!(matches!(
        e.commit_with_deletes(table, seqs, &[], txn).await,
        Err(Error::TxnConflict(_))
    ));
}

#[tokio::test]
async fn conflict_check_delete() {
    let dir = tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE table1 (col1 INT);").await.unwrap();
    e.execute("INSERT INTO table1 VALUES (0),(1),(2),(3),(4),(5),(6),(7),(8),(9);")
        .await
        .unwrap();

    let table = "default_db.table1";
    let rows = e.fetch_rows_with_ids(table).await.unwrap();
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

    // `commit_with_deletes` stages the `DeleteV2` tombstones itself; the
    // transactions here only have to exist, as `NewTxn::Delete` begins them.
    let delete_row = |_seqs: &[usize]| e.txn.begin();

    {
        let txn1 = delete_row(&[seq_of[0]]);
        let txn2 = delete_row(&[seq_of[0]]);
        let txn3 = delete_row(&[seq_of[0]]);

        e.commit_with_deletes(table, &[seq_of[0]], &[], txn1)
            .await
            .unwrap();
        expect_conflict(&e, table, &[seq_of[0]], txn2).await;
        expect_conflict(&e, table, &[seq_of[0]], txn3).await;
        assert_eq!(e.row_count(table).unwrap(), 9);
    }
    {
        let txn1 = delete_row(&[seq_of[1]]);
        let txn2 = delete_row(&[seq_of[1]]);
        let txn3 = delete_row(&[seq_of[1]]);

        e.commit_with_deletes(table, &[seq_of[1]], &[], txn2)
            .await
            .unwrap();
        expect_conflict(&e, table, &[seq_of[1]], txn1).await;
        expect_conflict(&e, table, &[seq_of[1]], txn3).await;
        assert_eq!(e.row_count(table).unwrap(), 8);
    }
    {
        let txn1 = delete_row(&[seq_of[2]]);
        let txn2 = delete_row(&[seq_of[2]]);
        let txn3 = delete_row(&[seq_of[2]]);

        e.commit_with_deletes(table, &[seq_of[2]], &[], txn3)
            .await
            .unwrap();
        expect_conflict(&e, table, &[seq_of[2]], txn2).await;
        expect_conflict(&e, table, &[seq_of[2]], txn1).await;
        assert_eq!(e.row_count(table).unwrap(), 7);
    }
    {
        let txn1 = delete_row(&[seq_of[3]]);
        let txn2 = delete_row(&[seq_of[3], seq_of[4]]);

        e.commit_with_deletes(table, &[seq_of[3]], &[], txn1)
            .await
            .unwrap();
        expect_conflict(&e, table, &[seq_of[3], seq_of[4]], txn2).await;
        assert_eq!(e.row_count(table).unwrap(), 6);
    }
    {
        let txn1 = delete_row(&[seq_of[5], seq_of[6]]);
        let txn2 = delete_row(&[seq_of[5]]);

        e.commit_with_deletes(table, &[seq_of[5], seq_of[6]], &[], txn1)
            .await
            .unwrap();
        expect_conflict(&e, table, &[seq_of[5]], txn2).await;
        assert_eq!(e.row_count(table).unwrap(), 4);
    }
}
