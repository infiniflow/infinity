#!/bin/bash
# Prints what the engine binary needs and what this CPU has, before the engine
# is started.
#
# The binary is built on one runner and executed on another, so a `target-cpu`
# the executor lacks shows up as a bare SIGILL with an empty log: the process
# dies before it can write a line, and `wait_for_infinity.sh` only reports
# "unhealthy in 60s". This runs the binary for five seconds up front so the
# failure names itself instead of looking like a slow start.
#
# Usage: check_engine_binary.sh <binary> <config>
set -uo pipefail

bin="${1:-./bin/infinity}"
config="${2:-conf/infinity_conf.toml}"

echo "=== binary ==="
file "${bin}" || true
ldd "${bin}" 2>&1 | head -10 || true

echo "=== this CPU ==="
grep -m1 'model name' /proc/cpuinfo || true
echo -n "simd flags: "
grep -m1 '^flags' /proc/cpuinfo \
  | tr ' ' '\n' \
  | grep -Ex 'avx|avx2|avx512f|fma|sse4_2|bmi1|bmi2|pclmulqdq' \
  | sort \
  | tr '\n' ' ' || true
echo
# The whole list, because the one that matters is the one nobody thought to
# check: `x86-64-v3` needs BMI1, BMI2, F16C, FMA, LZCNT and MOVBE besides AVX2,
# and a VM can expose a subset.
echo "all flags:"
grep -m1 '^flags' /proc/cpuinfo | cut -d: -f2- || true

echo "=== required vs available ==="
# What `x86-64-v3` needs, checked one by one so a missing feature names itself
# instead of showing up as a SIGILL with no explanation. `lzcnt` is reported as
# `abm` on Intel, and some kernels list only that spelling.
has_flag() {
  grep -qw "$1" /proc/cpuinfo && return 0
  grep -qw "$2" /proc/cpuinfo 2>/dev/null && return 0
  return 1
}
for pair in "avx:avx" "avx2:avx2" "bmi1:bmi1" "bmi2:bmi2" "f16c:f16c" "fma:fma" "lzcnt:abm" "movbe:movbe"; do
  want="${pair%%:*}"
  alias="${pair##*:}"
  if ! has_flag "${want}" "${alias}"; then
    echo "::error::this CPU lacks '${want}', which x86-64-v3 requires"
  fi
done

# `x86-64-v3` is a *level*, and a level is not the only thing a compiler can
# emit. `-march=native` on an AMD build host picks `znver3`/`znver4`, and those
# profiles include SSE4a - `insertq`/`extrq` - which no Intel part implements.
# A build that leaked one of those dies on Intel in a library we did not write
# (RocksDB's C++ in `rocksdb::ImmutableDBOptions`), so grep the binary itself
# and name the instruction rather than waiting for a bare SIGILL.
echo "=== instructions the CPU cannot run ==="
if command -v objdump >/dev/null 2>&1; then
  # Disassemble once: the binary is ~55 MB and two passes double the cost.
  disasm="$(objdump -d "${bin}" 2>/dev/null)"
  sse4a_count=$(printf '%s\n' "${disasm}" | grep -cE '\b(insertq|extrq)\b' || true)
  avx512_count=$(printf '%s\n' "${disasm}" | grep -cE '%zmm|%k[0-7]\b' || true)
  echo "sse4a (insertq/extrq): ${sse4a_count}"
  echo "avx-512 (zmm/mask):    ${avx512_count}"
  if [ "${sse4a_count}" != "0" ] && ! has_flag sse4a sse4a; then
    echo "::error::the binary has ${sse4a_count} SSE4a instructions (AMD-only) and this CPU lacks sse4a"
  fi
  if [ "${avx512_count}" != "0" ] && ! has_flag avx512f avx512f; then
    echo "::error::the binary has ${avx512_count} AVX-512 instructions and this CPU lacks avx512f"
  fi
else
  echo "objdump is unavailable; skipping the instruction scan"
fi

echo "=== direct execution (5s) ==="
# Run against a throwaway data directory and its own ports. Starting the real
# one here and killing it left `/var/infinity` locked, so the "Start the engine"
# step that follows timed out at 60 s with a log that looked empty - the probe
# was the thing breaking the start it was meant to diagnose.
probe_dir="$(mktemp -d)"
probe_conf="${probe_dir}/engine.toml"
sed -e "s#/var/infinity#${probe_dir}#g" \
    -e 's/^postgres_port.*/postgres_port = 25432/' \
    -e 's/^http_port.*/http_port = 25433/' \
    -e 's/^client_port.*/client_port = 25434/' \
    -e 's/^peer_port.*/peer_port = 25435/' \
    "${config}" > "${probe_conf}"
# `timeout` sends SIGTERM, and the engine's shutdown is graceful enough that it
# can outlive the signal - a probe process left running holds the global
# resources the real start needs, which made that start hang with an empty log.
# Kill by the probe's own config path so nothing else can be hit.
cleanup_probe() {
  pkill -9 -f "${probe_conf}" 2>/dev/null || true
  rm -rf "${probe_dir}"
}
trap cleanup_probe EXIT
if command -v gdb >/dev/null 2>&1; then
  # Under gdb a SIGILL stops instead of killing the process, so the faulting
  # instruction can be named. This is the whole reason the probe exists: a bare
  # "exit 132" says the CPU rejected an instruction but not which one.
  timeout -s KILL 20 gdb -batch \
    -ex 'run' \
    -ex 'printf "\n--- faulting instruction ---\n"' \
    -ex 'x/i $pc' \
    -ex 'info registers rip' \
    -ex 'bt 8' \
    --args "${bin}" --config="${probe_conf}" > /tmp/engine_probe.log 2>&1
  # gdb's own exit status is not the program's; the log says what happened.
  if grep -q "SIGILL\|Illegal instruction" /tmp/engine_probe.log 2>/dev/null; then
    echo "::error::the binary died on an illegal instruction"
  else
    echo "no SIGILL in the probe log"
  fi
else
  timeout -s KILL 5 "${bin}" --config="${probe_conf}" > /tmp/engine_probe.log 2>&1
  status=$?
  if [ "${status}" -eq 124 ]; then
    echo "still running at the timeout, which is healthy"
  else
    echo "::error::the binary exited with ${status} before the timeout"
  fi
fi
tail -40 /tmp/engine_probe.log || true

# Never fail the job here: the point is to leave a readable trace, and the
# "Start the engine" step that follows is what decides pass or fail.
exit 0
