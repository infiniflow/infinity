#!/usr/bin/env python3
"""Build a large synthetic full-text corpus out of a real one, for long-query
WAND measurements (the shape upstream #3535 measured: 1M docs, 10 x 100k,
vocabulary 20k, 40 words/doc).

Words come from an enwiki extract (the tokenizer here is the reference's own
LoadVocabulary rule: runs of alnum characters, at least 3 long, lower-cased),
so document frequencies follow real prose instead of a uniform distribution.

    python3 rust_engine/tools/gen_long_query_corpus.py \
        --corpus test/data/benchmark/enwiki/enwiki-10w.csv \
        --out .scratch/ft-m1/corpus-1m.jsonl \
        --vocab-out .scratch/ft-m1/vocab.txt \
        --docs 1000000 --words 40 --vocab 20000

Output is the same {"id","title","text"} JSONL that
`infinity fulltext-bench <dir> insert <corpus> <batch>` reads; `--batch` also
writes `<out>.part0..9` slices if you want to insert them separately.
"""

import argparse
import collections
import json
import os
import random
import re
import sys

TOKEN = re.compile(r"[A-Za-z0-9]{3,}")


def build_vocab(path, limit):
    """Document frequency over the source corpus, top-limit by tf then by word."""
    tf = collections.Counter()
    with open(path, encoding="utf-8", errors="replace") as src:
        for line in src:
            parts = line.rstrip("\n").split("\t")
            text = parts[2] if len(parts) >= 3 else parts[-1]
            for word in TOKEN.findall(text):
                tf[word.lower()] += 1
    ranked = tf.most_common()
    ranked.sort(key=lambda kv: (-kv[1], kv[0]))
    return ranked[:limit]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--corpus", required=True, help="source text extract (tsv/csv, text in the last column)")
    ap.add_argument("--out", required=True, help="JSONL to write")
    ap.add_argument("--vocab-out", default=None, help="one word per line, by descending frequency")
    ap.add_argument("--docs", type=int, default=1_000_000)
    ap.add_argument("--words", type=int, default=40, help="words per document")
    ap.add_argument("--vocab", type=int, default=20_000)
    ap.add_argument("--batch", type=int, default=0, help="also write <out>.partN slices of this size")
    ap.add_argument("--seed", type=int, default=20260601)
    args = ap.parse_args()

    vocab = build_vocab(args.corpus, args.vocab)
    if not vocab:
        sys.exit(f"no words in {args.corpus}")
    words = [w for w, _ in vocab]
    # Sample by frequency (zipf-shaped), the same way the source corpus is
    # distributed - a uniform vocabulary makes every term equally dense, which
    # is not a useful WAND workload.
    weights = [c for _, c in vocab]
    if args.vocab_out:
        os.makedirs(os.path.dirname(os.path.abspath(args.vocab_out)), exist_ok=True)
        with open(args.vocab_out, "w", encoding="utf-8") as out:
            out.write("\n".join(words) + "\n")
    print(f"vocabulary: {len(words)} words, top={words[0]}/{weights[0]}, tail={words[-1]}/{weights[-1]}", flush=True)

    rng = random.Random(args.seed)
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    part = None
    part_index = 0
    written_in_part = 0
    with open(args.out, "w", encoding="utf-8") as out:
        for i in range(args.docs):
            text = " ".join(rng.choices(words, weights=weights, k=args.words))
            line = json.dumps({"id": f"d{i}", "title": f"doc {i}", "text": text}, ensure_ascii=False)
            out.write(line + "\n")
            if args.batch:
                if part is None or written_in_part == args.batch:
                    if part:
                        part.close()
                    part = open(f"{args.out}.part{part_index}", "w", encoding="utf-8")
                    part_index += 1
                    written_in_part = 0
                part.write(line + "\n")
                written_in_part += 1
            if (i + 1) % 100_000 == 0:
                print(f"  {i + 1} docs", flush=True)
    if part:
        part.close()


if __name__ == "__main__":
    main()
