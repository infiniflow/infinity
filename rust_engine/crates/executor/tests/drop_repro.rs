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

//! A table restored from a snapshot can be dropped again: `DROP TABLE` has to
//! remove the restored catalog entry (and its rows), not only the original one.
//!
//! The two drops exercise different paths — the first removes a table the
//! engine created, the second one the restore just recreated — and the restore
//! must not leave a second incarnation behind for the second drop to miss.

use executor::Engine;

#[tokio::test]
async fn drop_after_restore() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open_async(dir.path(), Default::default())
        .await
        .unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT);").await.unwrap();
    e.execute("INSERT INTO tb1 VALUES (1),(2),(3);")
        .await
        .unwrap();
    e.execute("CREATE SNAPSHOT s1 ON TABLE default_db.tb1;")
        .await
        .unwrap();
    e.execute("DROP TABLE tb1;").await.unwrap();
    assert!(
        !e.table_exists("default_db.tb1"),
        "drop left the table behind (1)"
    );
    e.execute("RESTORE TABLE SNAPSHOT s1;").await.unwrap();
    assert!(
        e.table_exists("default_db.tb1"),
        "restore did not recreate it"
    );
    e.execute("DROP TABLE tb1;").await.unwrap();
    assert!(
        !e.table_exists("default_db.tb1"),
        "drop left the table behind (2)"
    );
}
