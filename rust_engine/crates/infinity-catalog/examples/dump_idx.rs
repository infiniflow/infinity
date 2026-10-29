fn main() {
    let root = std::env::args().nth(1).expect("catalog dir");
    let store = infinity_storage::open_store(std::path::Path::new(&root)).unwrap();
    let root = std::path::Path::new(&root);
    if let Some(parent) = root.parent() {
        let pstore = infinity_storage::open_store(&parent.join("persistence")).unwrap();
        for (k, v) in pstore.scan_prefix(b"").unwrap() {
            let key = String::from_utf8_lossy(&k).to_string();
            if key.contains("idx") || key.contains("index") {
                println!("PKEY {key} -> {}", String::from_utf8_lossy(&v));
            }
        }
    }
    for (k, v) in store.scan_prefix(b"idx|").unwrap() {
        let key = String::from_utf8_lossy(&k).to_string();
        if key.ends_with("index_base") {
            println!("BASE {key} -> {} bytes", v.len());
        } else {
            println!("IDXK {key}");
        }
    }
    for (k, _) in store.scan_prefix(b"catalog|").unwrap() {
        let key = String::from_utf8_lossy(&k).to_string();
        if key.contains("idx") || key.contains("index") {
            println!("ALL {key}");
        }
    }
    for (k, v) in store.scan_prefix(b"catalog|idx|").unwrap() {
        println!(
            "KEY {} -> {}",
            String::from_utf8_lossy(&k),
            String::from_utf8_lossy(&v)
        );
    }
    for (k, v) in store.scan_prefix(b"idx|").unwrap() {
        let key = String::from_utf8_lossy(&k).to_string();
        if key.ends_with("index_base") {
            println!("BASE {key} -> {} bytes", v.len());
        } else {
            println!("IDXK {key}");
        }
    }
    for (k, _) in store.scan_prefix(b"catalog|").unwrap() {
        let key = String::from_utf8_lossy(&k).to_string();
        if key.contains("idx") || key.contains("index") {
            println!("ALL {key}");
        }
    }
    for (k, v) in store.scan_prefix(b"catalog|idx|").unwrap() {
        let key = String::from_utf8_lossy(&k).to_string();
        let f: Vec<&str> = key.split('|').collect();
        if f.len() == 6 {
            let id = String::from_utf8_lossy(&v).to_string();
            let tag = format!("catalog|idx|{}|{}|{}|index_base", f[2], f[3], id);
            println!(
                "TAG {tag} -> {}",
                store.get(tag.as_bytes()).unwrap().is_some()
            );
        }
    }
}
