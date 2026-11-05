//! Builds the long-query full-text corpus out of a real text extract, for the
//! WAND measurements in `docs/FULLTEXT_BENCH.md` (the shape upstream #3535
//! used: 1M documents, vocabulary 20k, 40 words per document).
//!
//! Words come from the source corpus with the reference's own
//! `LoadVocabulary` rule - runs of alphanumeric characters, at least 3 long,
//! lower-cased - so document frequencies follow real prose instead of a
//! uniform vocabulary.
//!
//! ```text
//! cargo run --release --bin gen-corpus -- \
//!     --corpus ../../test/data/benchmark/enwiki/enwiki-10w.csv \
//!     --out ../../.scratch/ft-m1/corpus-1m.jsonl \
//!     --vocab-out ../../.scratch/ft-m1/vocab.txt \
//!     --docs 1000000 --words 40 --vocab 20000
//! ```
//!
//! The output is the `{"id","title","text"}` JSONL that
//! `infinity fulltext-bench <dir> insert <corpus> <batch>` reads.

use std::collections::HashMap;
use std::fs::File;
use std::io::{BufRead, BufReader, BufWriter, Write};

fn usage() -> ! {
    eprintln!(
        "usage: gen-corpus --corpus <text.tsv> --out <jsonl> [--vocab-out <txt>]\n\
         \x20                 [--docs 1000000] [--words 40] [--vocab 20000]\n\
         \x20                 [--batch 0] [--seed 20260601]"
    );
    std::process::exit(2)
}

fn arg(args: &[String], name: &str) -> Option<String> {
    args.iter()
        .position(|a| a == name)
        .and_then(|i| args.get(i + 1))
        .cloned()
}

fn number(args: &[String], name: &str, default: u64) -> u64 {
    arg(args, name)
        .map(|v| {
            v.parse()
                .unwrap_or_else(|_| panic!("{name} must be a number, got {v}"))
        })
        .unwrap_or(default)
}

/// The reference's LoadVocabulary tokenizer: alphanumeric runs of at least
/// three characters. The caller lower-cases.
fn tokens(text: &str) -> impl Iterator<Item = &str> + '_ {
    let bytes = text.as_bytes();
    let mut i = 0usize;
    std::iter::from_fn(move || loop {
        while i < bytes.len() && !bytes[i].is_ascii_alphanumeric() {
            i += 1;
        }
        let start = i;
        while i < bytes.len() && bytes[i].is_ascii_alphanumeric() {
            i += 1;
        }
        if i == start {
            return None;
        }
        let word = &text[start..i];
        if word.len() >= 3 {
            return Some(word);
        }
    })
}

/// xorshift64*: the generator only has to be fast and reproducible.
struct Rng(u64);

impl Rng {
    fn next(&mut self) -> u64 {
        let mut x = self.0;
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        self.0 = x;
        x.wrapping_mul(0x2545_f491_4f6c_dd1d)
    }
}

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    let corpus = arg(&args, "--corpus").unwrap_or_else(|| usage());
    let out_path = arg(&args, "--out").unwrap_or_else(|| usage());
    let vocab_out = arg(&args, "--vocab-out");
    let docs = number(&args, "--docs", 1_000_000) as usize;
    let words_per_doc = number(&args, "--words", 40) as usize;
    let vocab_size = number(&args, "--vocab", 20_000) as usize;
    let batch = number(&args, "--batch", 0) as usize;
    let seed = number(&args, "--seed", 20_260_601);

    // 1. Vocabulary: term frequencies over the source text.
    let file = File::open(&corpus).unwrap_or_else(|e| panic!("{corpus}: {e}"));
    let mut counts: HashMap<Vec<u8>, u64> = HashMap::new();
    for line in BufReader::new(file).lines() {
        let line = line.unwrap_or_default();
        // The extract is tab-separated with the article text in the last column.
        let text = line.rsplit('\t').next().unwrap_or("");
        for token in tokens(text) {
            *counts
                .entry(token.as_bytes().to_ascii_lowercase())
                .or_default() += 1;
        }
    }
    let mut ranked: Vec<(Vec<u8>, u64)> = counts.into_iter().collect();
    ranked.sort_by(|a, b| b.1.cmp(&a.1).then_with(|| a.0.cmp(&b.0)));
    ranked.truncate(vocab_size);
    let words: Vec<String> = ranked
        .iter()
        .map(|(w, _)| String::from_utf8_lossy(w).into_owned())
        .collect();
    let mut cumulative = Vec::with_capacity(words.len());
    let mut total = 0u64;
    for (_, count) in &ranked {
        total += count;
        cumulative.push(total);
    }
    eprintln!(
        "vocabulary: {} words, top={}/{}, tail={}/{}",
        words.len(),
        words[0],
        ranked[0].1,
        words[words.len() - 1],
        ranked[ranked.len() - 1].1
    );
    if let Some(path) = &vocab_out {
        let mut out = BufWriter::new(File::create(path).unwrap_or_else(|e| panic!("{path}: {e}")));
        for w in &words {
            writeln!(out, "{w}").unwrap();
        }
    }

    // 2. Documents: one draw per word, weighted by frequency.
    let mut out = BufWriter::with_capacity(
        1 << 22,
        File::create(&out_path).unwrap_or_else(|e| panic!("{out_path}: {e}")),
    );
    let mut part: Option<BufWriter<File>> = None;
    let mut part_index = 0usize;
    let mut in_part = 0usize;
    let mut rng = Rng(seed | 1);
    for doc in 0..docs {
        let mut text = String::with_capacity(words_per_doc * 8);
        for w in 0..words_per_doc {
            let draw = rng.next() % total;
            let idx = cumulative.partition_point(|&c| c <= draw);
            if w > 0 {
                text.push(' ');
            }
            text.push_str(&words[idx.min(words.len() - 1)]);
        }
        let line =
            format!("{{\"id\": \"d{doc}\", \"title\": \"doc {doc}\", \"text\": \"{text}\"}}");
        writeln!(out, "{line}").unwrap();
        if batch > 0 {
            if part.is_none() || in_part == batch {
                part = Some(BufWriter::new(
                    File::create(format!("{out_path}.part{part_index}")).unwrap(),
                ));
                part_index += 1;
                in_part = 0;
            }
            let p = part.as_mut().unwrap();
            writeln!(p, "{line}").unwrap();
            in_part += 1;
        }
        if (doc + 1) % 100_000 == 0 {
            eprintln!("  {} docs", doc + 1);
        }
    }
}
