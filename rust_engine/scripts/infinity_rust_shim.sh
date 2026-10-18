#!/usr/bin/env bash
# Adapts the C++ `infinity` CLI to the Rust engine so the unmodified harnesses
# under `python/restart_test` (Gate C) can drive it.
#
# The C++ harness starts the engine as `<binary> --config=<toml>` and expects it
# to serve Thrift on 127.0.0.1:23817 with data under `/var/infinity`. The Rust
# CLI accepts the same `--config=` form: it reads `persistence_dir`,
# `memindex_memory_quota`, `checkpoint_interval`, `optimize_interval` and the
# MinIO settings, then serves Thrift. Without a config it serves `$INFINITY_DATA`.
#
#   ./scripts/infinity_rust_shim.sh --config=/any/path
#
# Override the binary / data dir with INFINITY_SERVER_BIN / INFINITY_DATA (the
# latter is useful together with `bwrap`, which can give an unprivileged user a
# writable `/var/infinity`).
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
bin="${INFINITY_SERVER_BIN:-$here/../target/release/infinity}"
data_dir="${INFINITY_DATA:-/var/infinity}"
mkdir -p "$data_dir"
# Long imports allocate large per-batch buffers; two glibc arenas keep the
# allocator from retaining freed pages across threads (Gate C RSS).
export MALLOC_ARENA_MAX="${MALLOC_ARENA_MAX:-2}"
# The harness only speaks Thrift on 23817; keep a foreign server already holding
# 23820 from affecting this run.
export INFINITY_HTTP_BIND="${INFINITY_HTTP_BIND:-127.0.0.1:0}"
if [[ "${1:-}" == --config=* ]]; then
  cfg="${1#--config=}"
  # The harness runs from `python/` but passes repo-relative config paths.
  if [[ ! -f "$cfg" && -f "$here/../../$cfg" ]]; then
    set -- "--config=$here/../../$cfg"
  fi
  "$bin" "$@"
  code=$?
  echo "$(date '+%F %T') exit=$code args=$*" >> "${INFINITY_SHIM_LOG:-/tmp/infinity_shim.log}"
  exit $code
fi
exec "$bin" serve "$data_dir"
