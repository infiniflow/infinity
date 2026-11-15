//! PoC G (RUST_REWRITE_PLAN.md 附录 B): does `jieba-rs` reproduce `cppjieba`?
//!
//! Both segmenters are loaded with the *same* dictionaries that Infinity ships
//! (`/usr/share/infinity/resource/jieba/dict/`), so this isolates the algorithm
//! port from dictionary differences. Two modes are compared:
//!   * `mp`   : dictionary-only (cppjieba `Cut(...,hmm=false)` vs jieba-rs `cut(s,false)`)
//!   * `hmm`  : dictionary + HMM (cppjieba `Cut(...,hmm=true)`  vs jieba-rs `cut(s,true)`)
//!
//! Output is tab-separated so it can be diffed against the C++ side:
//!   `<mode>\t<line_no>\t<space-joined-words>`
//!
//! Run:  cargo run -p jieba-consistency -- <dict_dir> [sentences.txt]

use std::fs;
use std::io::BufReader;

fn main() {
    let mut args = std::env::args().skip(1);
    let dict_dir = args
        .next()
        .unwrap_or_else(|| "/usr/share/infinity/resource/jieba/dict".to_string());
    let sentences_file = args.next().unwrap_or_else(|| "sentences.txt".to_string());

    let dict = format!("{dict_dir}/jieba.dict.utf8");
    let user = format!("{dict_dir}/user.dict.utf8");

    let mut jieba = {
        let f = fs::File::open(&dict).expect("open jieba.dict.utf8");
        jieba_rs::Jieba::with_dict(&mut BufReader::new(f)).expect("load dict")
    };
    if let Ok(f) = fs::File::open(&user) {
        let _ = jieba.load_dict(&mut BufReader::new(f));
    }

    let text = fs::read_to_string(&sentences_file).expect("read sentences");
    for (i, line) in text.lines().enumerate() {
        if line.trim().is_empty() {
            continue;
        }
        for (mode, hmm) in [("mp", false), ("hmm", true)] {
            let words: Vec<&str> = jieba
                .cut(line, hmm)
                .into_iter()
                .filter(|w| !w.trim().is_empty())
                .collect();
            println!("{mode}\t{i}\t{}", words.join(" "));
        }
    }
}
