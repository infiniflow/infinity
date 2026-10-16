#!/usr/bin/env python3
"""Probe the C++ BM25 ranker standalone, to pin the full-text scoring statistics.

The Gate A residual is dominated by `ROUND(SCORE(),4)` values. The formula is
already ported (`score = sum over matched terms of idf * tf_norm * weight`,
`idf = ln(1 + (N - df + 0.5)/(df + 0.5))`, `tf_norm = (k1+1)*tf /
(tf + k1*(1 - b + b*len/avg))`, k1=1.2, b=0.75 - `bm25_ranker_impl.cpp:25-34`),
but two *statistics* were unknown: whether `df`/`N` are per-segment or
whole-table, and how `avg_column_len`/`column_len` are derived.

This compiles the ranker's exact body (it is self-contained: `<cmath>` +
`<algorithm>` only) and lets you search for the (N, df, avg, len) that
reproduces a target score from a `.slt` file.

Usage:

    python3 rust_engine/tools/bm25_probe/gen.py            # build + demo search
    python3 rust_engine/tools/bm25_probe/gen.py --grid      # print a grid

Requires a C++23 compiler on PATH (CXX, default clang++).
"""

import argparse
import os
import subprocess
import sys
import tempfile

SRC = r"""
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <cstring>
using u64 = uint64_t;
using u32 = uint32_t;
namespace {
constexpr float k1 = 1.2F;
constexpr float b = 0.75F;
struct BM25Ranker {  // verbatim from src/storage/invertedindex/search/bm25_ranker_impl.cpp
    float score_{0};
    u64 total_df_{0};
    explicit BM25Ranker(u64 t) : total_df_(std::max(t, (u64)1)) {}
    void AddTermParam(u64 tf, u64 df, float avg_column_len, u32 column_len, float weight) {
        float smooth_idf = std::log(1.0F + (total_df_ - df + 0.5F) / (df + 0.5F));
        float smooth_tf = (k1 + 1.0F) * tf / (tf + k1 * (1.0F - b + b * column_len / avg_column_len));
        score_ += smooth_idf * smooth_tf * weight;
    }
    float GetScore() const { return score_; }
};
}
// one line per probe: "N tf:df:len:weight ..."
int main(int argc, char **argv) {
    if (argc >= 2 && std::strcmp(argv[1], "--grid") == 0) {
        for (u64 n : {3ull, 5ull})
            for (float avg : {2.0f, 2.1f, 3.0f})
                for (u32 len : {1u, 2u, 3u, 4u})
                    for (u64 df : {1ull, 2ull, 3ull, 4ull}) {
                        BM25Ranker r(n);
                        r.AddTermParam(1, df, avg, len, 1.0f);
                        std::printf("N=%llu df=%llu avg=%.2f len=%u -> %.4f\n", (unsigned long long)n,
                                    (unsigned long long)df, avg, len, r.GetScore());
                    }
        return 0;
    }
    // demo: the fulltext_minimum_should_match query 'second text' over 5 docs,
    // target row2 ('second text multiple') = 1.1863.
    for (u64 n : {3ull, 5ull}) {
        for (float avg : {2.0f, 2.05f, 2.1f, 2.15f}) {
            for (u32 len : {1u, 2u, 3u}) {
                BM25Ranker r(n);
                u64 df_second = (n == 3) ? 1 : 2;
                u64 df_text = (n == 3) ? 3 : 4;
                r.AddTermParam(1, df_second, avg, len, 1.0f);
                r.AddTermParam(1, df_text, avg, len, 1.0f);
                std::printf("N=%llu avg=%.2f len=%u -> %.4f\n", (unsigned long long)n, avg, len,
                            r.GetScore());
            }
        }
    }
    return 0;
}
"""


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--grid", action="store_true", help="print a (N, df, avg, len) grid")
    ap.add_argument("--cxx", default=os.environ.get("CXX", "clang++"))
    args = ap.parse_args()
    with tempfile.TemporaryDirectory() as td:
        src = os.path.join(td, "bm25.cpp")
        exe = os.path.join(td, "bm25")
        with open(src, "w") as f:
            f.write(SRC)
        subprocess.run([args.cxx, "-std=c++23", "-O2", "-o", exe, src], check=True)
        out = subprocess.run(
            [exe] + (["--grid"] if args.grid else []), check=True, capture_output=True, text=True
        )
    print(out.stdout, end="")
    return 0


if __name__ == "__main__":
    sys.exit(main())
