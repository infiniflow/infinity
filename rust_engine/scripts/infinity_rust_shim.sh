#!/usr/bin/env bash
# Adapts the C++ `infinity` CLI to the Rust `infinity-server` so the unmodified
# harnesses under `python/restart_test` (Gate C) can drive the Rust engine.
#
# The C++ harness starts the engine as `<binary> --config=<toml>` and expects it
# to serve Thrift on 127.0.0.1:23817 with data under `/var/infinity`. The Rust
# server takes the data dir as its only positional argument and binds
# `INFINITY_BIND` (default `0.0.0.0:23817`); the `--config` value is accepted
# and ignored here so nothing in the harness has to change.
#
#   ./scripts/infinity_rust_shim.sh --config=/any/path
#
# Override the binary / data dir with INFINITY_SERVER_BIN / INFINITY_DATA (the
# latter is useful together with `bwrap`, which can give an unprivileged user a
# writable `/var/infinity`).
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
bin="${INFINITY_SERVER_BIN:-$here/../target/release/infinity-server}"
data_dir="${INFINITY_DATA:-/var/infinity}"
mkdir -p "$data_dir"
exec "$bin" "$data_dir"
