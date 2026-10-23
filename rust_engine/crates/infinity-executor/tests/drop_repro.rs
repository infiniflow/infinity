use infinity_executor::Engine;

fn dump(e: &Engine, where_: &str) {
    println!("--- store after {where_} ---");
    let store = e.storage.catalog.store();
    for (k, v) in store.scan_prefix(b"").unwrap() {
        let k = String::from_utf8_lossy(&k).to_string();
        if k.contains("tb1") || k.starts_with("tbl:") || k.starts_with("name:") {
            println!("{k} => {:?}", String::from_utf8_lossy(&v));
        }
    }
    println!("--- tables: {:?}", e.execute("SHOW TABLES;"));
}

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
    dump(&e, "restore");
    let r = e.execute("DROP TABLE tb1;");
    println!("second drop: {r:?}");
    dump(&e, "second drop");
    assert!(
        !e.table_exists("default_db.tb1"),
        "drop left the table behind (2)"
    );
}
