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

echo "=== required vs available ==="
# `x86-64-v3` needs AVX2. Without it the process dies on the first vector
# instruction, which is why the log stays empty.
if ! grep -q ' avx2 ' /proc/cpuinfo; then
  echo "::error::this CPU has no AVX2, but the binary was built for x86-64-v3"
fi

echo "=== direct execution (5s) ==="
timeout 5 "${bin}" --config="${config}" > /tmp/engine_probe.log 2>&1
status=$?
if [ "${status}" -eq 124 ]; then
  echo "still running at the timeout, which is healthy"
else
  echo "::error::the binary exited with ${status} before the timeout"
fi
tail -20 /tmp/engine_probe.log || true

# Never fail the job here: the point is to leave a readable trace, and the
# "Start the engine" step that follows is what decides pass or fail.
exit 0
