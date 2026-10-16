#!/usr/bin/env python3
"""Regenerate the golden C++ FST fixtures used by the Rust FST decoder tests.

The C++ full-text index stores its term dictionary as a self-owned FST
(`src/storage/invertedindex/fst/`). Its node state machine is byte-exact and
hard to verify from source alone, so we compile the *real* C++ sources into a
throwaway binary and print the FST bytes for a few small key sets. Those bytes
are pasted into `infinity-storage/src/inverted_format.rs`
(`golden_fsts_from_the_cpp_builder_verify`, `fst_lookup_decodes_the_cpp_built_fsts`),
where the Rust decoder is checked against them.

Why an amalgamation: the sources are C++20 modules that `import third_party`
(pulling in the whole engine). Flattening them into one translation unit with
tiny stubs for `infinity_type`/`crc`/`fmt`/`UnrecoverableError` lets us build
just the FST, with no vcpkg or engine build.

Usage (from anywhere):

    python3 rust_engine/tools/fst_golden/gen.py [--check]

`--check` also runs `cargo test -p infinity-storage fst_` to confirm the Rust
decoder still agrees with the freshly built fixtures.

Requires: a `clang++` new enough for C++23 (tested with clang 23) and, for
`--check`, a working cargo.
"""

import argparse
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))  # repo root
FST_DIR = os.path.join(ROOT, "src", "storage", "invertedindex", "fst")

# Flatten order matters: each file only needs what precedes it.
ORDER = ["common_inputs", "writer", "bytes", "node", "registry", "build"]
# `build_impl.cpp` holds FstBuilder's out-of-line definitions.
IMPL = os.path.join(FST_DIR, "build_impl.cpp")

STUBS = r"""
#include <cassert>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <optional>
#include <utility>
#include <memory>
#include <limits>
namespace fmt {
template <class... A> std::string format(const char *f, A &&...a) {
    std::ostringstream os; const char *p = f;
    auto emit = [&](auto &&v) {
        std::ostringstream t; t << v; std::string s = t.str();
        const char *b = p; while (*p && *p != '{') ++p; os.write(b, p - b);
        if (*p == '{') { ++p; while (*p && *p != '}') ++p; if (*p == '}') ++p; }
        os << s;
    };
    (emit(a), ...); os << p; return os.str();
}
}
namespace infinity {
using u8 = uint8_t; using u16 = uint16_t; using u32 = uint32_t; using u64 = uint64_t;
using i8 = int8_t; using i16 = int16_t; using i32 = int32_t; using i64 = int64_t;
using idx_t = u64; using f32 = float; using f64 = double;
[[noreturn]] inline void UnrecoverableError(const std::string &m) {
    std::cerr << "UnrecoverableError: " << m << "\n"; std::abort();
}
// CRC32/IEEE (fst/writer.cppm's CountingWriter uses `update`/`finalize`).
template <class T> struct CRCBase {
    T tab[256];
    explicit CRCBase(T p) {
        for (size_t i = 0; i < 256; ++i) {
            T c = (T)i;
            for (size_t j = 0; j < 8; ++j) c = (c & 1) ? p ^ (c >> 1) : (c >> 1);
            tab[i] = c;
        }
    }
};
template <class T, T poly, T init, T fin> struct CRCImpl {
    static CRCBase<T> base;
    void update(const unsigned char *b, size_t n) {
        for (size_t i = 0; i < n; ++i) crc = base.tab[(crc ^ b[i]) & 0xff] ^ (crc >> 8);
    }
    T finalize() { return crc ^ fin; }
    T crc = init;
};
template <class T, T p, T i, T f> CRCBase<T> CRCImpl<T, p, i, f>::base{p};
constexpr u32 CRC32_IEEE = 0xEDB88320;
using CRC32IEEE = CRCImpl<u32, CRC32_IEEE, 0xFFFFFFFF, 0xFFFFFFFF>;
}
"""

MAIN = r"""
// ===== harness =====
#include <cstdio>
static const char *g_keys[6];
static int g_nkeys;
static void dump(const char *label) {
    std::vector<infinity::u8> buf;
    infinity::FstBufferWriter w(buf);
    infinity::FstBuilder b(w);
    for (int i = 0; i < g_nkeys; i++) {
        std::string k(g_keys[i]);
        b.Insert((infinity::u8 *)k.data(), k.size(), (infinity::u64)(i + 1) * 100);
    }
    b.Finish();
    printf("FST %s len=%zu bytes=", label, buf.size());
    for (auto c : buf) printf("%02x", (unsigned)c);
    printf("\n");
}
static void run(const char *label, std::initializer_list<const char *> ks) {
    g_nkeys = 0;
    for (const char *k : ks) g_keys[g_nkeys++] = k;
    dump(label);
}
int main() {
    run("empty", {});
    run("one", {"a"});
    run("two", {"a", "b"});
    run("words", {"app", "apple", "apply", "banana", "band", "cat"});
    run("shared", {"aaaa", "aaab", "aabb", "abbb", "baaa"});
    return 0;
}
"""


def strip_module_lines(text: str) -> str:
    out = []
    for line in text.split("\n"):
        s = line.strip()
        if re.match(r"^(export )?module\b.*;$", s):
            continue
        if re.match(r"^import\b.*;$", s):
            continue
        out.append(re.sub(r"^(\s*)export\s+", r"\1", line))
    return "\n".join(out)


def build_source() -> str:
    parts = [STUBS]
    for name in ORDER:
        path = os.path.join(FST_DIR, name + ".cppm")
        with open(path) as f:
            parts.append(f"\n// ===== {name} =====\n" + strip_module_lines(f.read()))
    with open(IMPL) as f:
        parts.append("\n// ===== build_impl =====\n" + strip_module_lines(f.read()))
    parts.append(MAIN)
    return "".join(parts)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="run the Rust FST tests after building")
    ap.add_argument("--cxx", default=os.environ.get("CXX", "clang++"))
    args = ap.parse_args()

    with tempfile.TemporaryDirectory() as td:
        src = os.path.join(td, "amalg.cpp")
        exe = os.path.join(td, "amalg")
        with open(src, "w") as f:
            f.write(build_source())
        subprocess.run([args.cxx, "-std=c++23", "-O0", "-o", exe, src], check=True)
        out = subprocess.run([exe], check=True, capture_output=True, text=True).stdout
    print(out, end="")
    if args.check:
        subprocess.run(
            ["cargo", "test", "-p", "infinity-storage", "fst_", "--", "--nocapture"],
            cwd=os.path.join(ROOT, "rust_engine"),
            check=True,
        )
    return 0


if __name__ == "__main__":
    sys.exit(main())
