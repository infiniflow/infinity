#!/usr/bin/env python3

# This script extracts queries from the tantivy queries file:
# https://github.com/quickwit-oss/search-benchmark-game/blob/master/queries.txt

import sys
import json


def process_queries(tantivy_queries_file, output_queries_file):
    queries = []
    for line in open(tantivy_queries_file, "r"):
        obj = json.loads(line)
        if "union" in obj["tags"]:
            queries.append(obj["query"])
    sorted_queries = sorted(queries)
    with open(output_queries_file, "w") as file:
        for query in sorted_queries:
            file.write(query + "\n")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(
            "Usage: python preprocess_tantivy_queries <tantivy_queries_file> <output_queries_file>"
        )
        sys.exit(1)

    tantivy_queries_file = sys.argv[1]
    output_queries_file = sys.argv[2]
    process_queries(tantivy_queries_file, output_queries_file)
