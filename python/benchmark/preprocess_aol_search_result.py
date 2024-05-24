#!/usr/bin/env python3

# This script extracts queries from the wikipedia search log. The search log can be downloaded from:
# - https://en.wikipedia.org/wiki/AOL_search_log_release
# - https://www.kaggle.com/datasets/dineshydv/aol-user-session-collection-500k

import sys
import json


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(
            "Usage: python preprocess_aol_search_result.py <aol_queries_file> <aol_results_file> <output_select_aol_queries_file>"
        )
        sys.exit(1)

    aol_queries_file = sys.argv[1]
    aol_results_file = sys.argv[2]
    output_file = sys.argv[3]
    queries = open(aol_queries_file, "r").readlines()
    selected_queries = []
    results = []
    with open(aol_results_file, "r") as f:
        for line in f:
            result = json.loads(line)
            results.append(result)
    results.sort(key=lambda x: x[-1][1], reverse=True)
    i = 0
    for result in results:
        query = queries[result[0][0]]
        terms = query.split()
        if len(terms) > 5:
            continue
        selected_queries.append(query)
        i += 1
        if i >= 100000:
            break
    open(output_file, "w").writelines(selected_queries)
