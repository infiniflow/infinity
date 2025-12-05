#!/usr/bin/env python3

"""
This script should be run from the infinity project root directory
Usage:  uv run scripts/download_deps_infinity_builder_ubuntu20.py
"""
# PEP 723 metadata
# /// script
# requires-python = ">=3.10"
# dependencies = [
#   "nltk",
# ]
# ///

import os
import sys
import ssl
import pathlib
from urllib import request, parse, error
import nltk


def ensure_project_root() -> None:
    cwd = pathlib.Path.cwd()
    if not (cwd / ".git").is_dir() or not (cwd / "scripts").is_dir():
        print("Error: This script must be run from the infinity project root directory")
        print("Usage: ./scripts/download_deps_infinity_builder_ubuntu20.py")
        sys.exit(1)


def basename_from_url(url: str) -> str:
    path = parse.urlsplit(url).path
    name = os.path.basename(path)
    return name or "downloaded.file"


def file_exists_and_nonempty(path: pathlib.Path) -> bool:
    try:
        return path.is_file() and path.stat().st_size > 0
    except OSError:
        return False


def download(url: str, filename: str | None = None) -> int:
    print(f"download {url}" + (f" -> {filename}" if filename else ""))
    fn = filename or basename_from_url(url)
    out_path = pathlib.Path(fn)

    if file_exists_and_nonempty(out_path):
        return 0

    # Match curl --insecure by disabling certificate verification
    ctx = ssl._create_unverified_context()

    try:
        with request.urlopen(url, context=ctx) as resp, open(out_path, "wb") as f:
            # Stream to disk
            chunk_size = 1024 * 64
            while True:
                chunk = resp.read(chunk_size)
                if not chunk:
                    break
                f.write(chunk)
        return 0
    except error.HTTPError as e:
        print(f"HTTP error downloading {url}: {e}")
        return 1
    except error.URLError as e:
        print(f"URL error downloading {url}: {e}")
        return 1
    except Exception as e:
        print(f"Unexpected error downloading {url}: {e}")
        return 1


def main() -> int:
    ensure_project_root()

    urls = [
        "https://github.com/Kitware/CMake/releases/download/v4.1.0/cmake-4.1.0-linux-x86_64.tar.gz",
        "https://github.com/ninja-build/ninja/releases/download/v1.13.1/ninja-linux.zip",
        "https://www.python.org/ftp/python/3.11.9/Python-3.11.9.tar.xz",
        "https://github.com/gcc-mirror/gcc/archive/refs/tags/releases/gcc-15.2.0.tar.gz",
        "https://github.com/jemalloc/jemalloc/releases/download/5.3.0/jemalloc-5.3.0.tar.bz2",
        "https://github.com/gperftools/gperftools/releases/download/gperftools-2.17/gperftools-2.17.tar.gz",
        "https://github.com/risinglightdb/sqllogictest-rs/releases/download/v0.28.3/sqllogictest-bin-v0.28.3-x86_64-unknown-linux-musl.tar.gz",
        "https://github.com/rui314/mold/releases/download/v2.40.3/mold-2.40.3-x86_64-linux.tar.gz",
        "https://download.docker.com/linux/static/stable/x86_64/docker-28.3.3.tgz",
        "https://github.com/astral-sh/uv/releases/download/0.9.7/uv-x86_64-unknown-linux-gnu.tar.gz",
    ]

    for url in urls:
        rc = download(url)
        if rc != 0:
            return rc

    local_dir = os.path.abspath("nltk_data")
    for data in ["wordnet", "punkt", "punkt_tab"]:
        print(f"Downloading nltk {data}...")
        nltk.download(data, download_dir=local_dir)

    return 0


if __name__ == "__main__":
    sys.exit(main())
