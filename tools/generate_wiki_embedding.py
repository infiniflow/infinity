#!/usr/bin/env python3


src_fp = "./test/data/csv/enwiki_9999.csv"
dst_fp = "./test/data/csv/enwiki_embedding_9999.csv"


def generate():
    with open(src_fp) as src:
        with open(dst_fp, 'w') as dst:
            i = 0
            for line in src:
                suffix = '\t{}\t"[{}, {}, {}, {}]"\n'.format(
                    i, i, i, i, i)
                dst.write(line.rstrip() + suffix)
                i += 1


if __name__ == "__main__":
    generate()
