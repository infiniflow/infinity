//! A table restored from a snapshot can be dropped again: `DROP TABLE` has to
//! remove the restored catalog entry (and its rows), not only the original one.
//!
//! The two drops exercise different paths — the first removes a table the
//! engine created, the second one the restore just recreated — and the restore
//! must not leave a second incarnation behind for the second drop to miss.

use infinity_executor::Engine;

#[test]
fn drop_after_restore() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE tb1 (col1 INT);").unwrap();
    e.execute("INSERT INTO tb1 VALUES (1),(2),(3);").unwrap();
    e.execute("CREATE SNAPSHOT s1 ON TABLE default_db.tb1;")
        .unwrap();
    e.execute("DROP TABLE tb1;").unwrap();
    assert!(
        !e.table_exists("default_db.tb1"),
        "drop left the table behind (1)"
    );
    e.execute("RESTORE TABLE SNAPSHOT s1;").unwrap();
    assert!(e.table_exists("default_db.tb1"), "restore did not recreate it");
    e.execute("DROP TABLE tb1;").unwrap();
    assert!(
        !e.table_exists("default_db.tb1"),
        "drop left the table behind (2)"
    );
}
