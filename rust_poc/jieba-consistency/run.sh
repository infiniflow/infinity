#!/usr/bin/env bash
# PoC G: compare cppjieba (C++) vs jieba-rs (Rust) on Infinity's own dictionaries.
#
# Usage: bash rust_poc/jieba-consistency/run.sh [dict_dir]
# Default dict_dir = /usr/share/infinity/resource/jieba/dict
#
# Notes:
#   * jieba-rs's dict parser is stricter than cppjieba's; one line in Infinity's
#     jieba.dict.utf8 is rejected. We sanitize a *copy* and run BOTH sides on it
#     (idf/hmm/stop_words copied verbatim). This is itself a PoC finding.
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$HERE/../.." && pwd)"
SRC="${1:-/usr/share/infinity/resource/jieba/dict}"
DST="/tmp/jieba-dict"
WORK="/tmp/jieba-g"

rm -rf "$DST" "$WORK"; mkdir -p "$DST" "$WORK"

# sanitize only the dicts jieba-rs loads
python3 - "$SRC" "$DST" <<'PY'
import sys, shutil, os
src, dst = sys.argv[1], sys.argv[2]
sanitize = {'jieba.dict.utf8', 'user.dict.utf8'}
for name in os.listdir(src):
    s, d = os.path.join(src, name), os.path.join(dst, name)
    if not os.path.isfile(s):
        continue
    if name in sanitize:
        kept = drop = 0
        with open(s, encoding='utf-8', errors='replace') as f, open(d, 'w', encoding='utf-8') as g:
            for line in f:
                parts = line.rstrip('\n').split()
                if len(parts) >= 2:
                    try: int(parts[1])
                    except ValueError: drop += 1; continue
                g.write(line if line.endswith('\n') else line + '\n'); kept += 1
        print(f"  sanitized {name}: kept={kept} dropped={drop}")
    else:
        shutil.copy2(s, d)
PY

echo "  building cpp_side ..."
clang++ -std=c++17 -O2 \
  -I "$REPO/third_party/cppjieba/include" \
  -I "$REPO/third_party/cppjieba/deps/limonp/include" \
  "$HERE/cpp_side.cpp" -o "$WORK/cpp_side" -lpthread

echo "  building rust side ..."
cargo build -q --manifest-path "$REPO/rust_poc/Cargo.toml" -p jieba-consistency
"$REPO/rust_poc/target/debug/jieba-consistency" "$DST" "$HERE/sentences.txt" > "$WORK/rust.txt"
"$WORK/cpp_side" "$DST" "$HERE/sentences.txt" > "$WORK/cpp.txt"

echo "=== cppjieba vs jieba-rs ==="
for m in mp hmm; do
  grep "^$m" "$WORK/cpp.txt"  | sed 's/^[^\t]*\t//' | sort > "$WORK/c_$m"
  grep "^$m" "$WORK/rust.txt" | sed 's/^[^\t]*\t//' | sort > "$WORK/r_$m"
  n=$(wc -l < "$WORK/c_$m"); s=$(comm -12 "$WORK/c_$m" "$WORK/r_$m" | wc -l)
  printf "  %-4s : %d/%d identical %s\n" "$m" "$s" "$n" "$([ "$n" = "$s" ] && echo '(MATCH)' || echo '(DIFFER)')"
done
echo "--- differing lines (mp) ---"; diff "$WORK/c_mp" "$WORK/r_mp" || true
echo "--- differing lines (hmm) ---"; diff "$WORK/c_hmm" "$WORK/r_hmm" || true
