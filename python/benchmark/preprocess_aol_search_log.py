#!/usr/bin/env python3

# This script extracts queries from the wikipedia search log. The search log can be downloaded from:
# - https://en.wikipedia.org/wiki/AOL_search_log_release
# - https://www.kaggle.com/datasets/dineshydv/aol-user-session-collection-500k

import sys
import re


def load_terms(enwiki_terms_file):
    data = set()
    with open(enwiki_terms_file, "r", encoding="utf-8", errors="ignore") as file:
        for line in file:
            columns = line.split()
            data.add(columns[0])
    return data


def process_tsv(aol_search_log_file, enwiki_terms_file, output_aol_queries_file):
    terms = load_terms(enwiki_terms_file)
    unique_queries = set()
    with open(aol_search_log_file, "r") as file:
        next(file)  # skip title line
        for line in file:
            # split per tab, the second field is the query
            fields = line.strip().split("\t")
            if len(fields) <= 2:
                continue
            # find pure alphabet words and lowercase them
            words = re.findall(r"\b[a-zA-Z]+\b", fields[1])
            words = [x.lower() for x in words]
            # filter out words that are not in the terms file
            words = [x for x in words if x in terms]
            if words and len(words) <= 5:
                unique_queries.add(" ".join(words))
    sorted_queries = sorted(unique_queries)
    with open(output_aol_queries_file, "w") as file:
        for query in sorted_queries:
            file.write(query + "\n")


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(
            "Usage: python preprocess_aol_search_log.py <aol_search_log_file> <enwiki_terms_file> <output_aol_queries_file>"
        )
        sys.exit(1)

    aol_search_log_file = sys.argv[1]
    enwiki_terms_file = sys.argv[2]
    output_aol_queries_file = sys.argv[3]
    process_tsv(aol_search_log_file, enwiki_terms_file, output_aol_queries_file)
