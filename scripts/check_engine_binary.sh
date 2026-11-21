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

echo "=== direct execution (5s) ==="
if command -v gdb >/dev/null 2>&1; then
  # Under gdb a SIGILL stops instead of killing the process, so the faulting
  # instruction can be named. This is the whole reason the probe exists: a bare
  # "exit 132" says the CPU rejected an instruction but not which one.
  timeout 20 gdb -batch \
    -ex 'run' \
    -ex 'printf "\n--- faulting instruction ---\n"' \
    -ex 'x/i $pc' \
    -ex 'info registers rip' \
    -ex 'bt 8' \
    --args "${bin}" --config="${config}" > /tmp/engine_probe.log 2>&1
  # gdb's own exit status is not the program's; the log says what happened.
  if grep -q "SIGILL\|Illegal instruction" /tmp/engine_probe.log 2>/dev/null; then
    echo "::error::the binary died on an illegal instruction"
  else
    echo "no SIGILL in the probe log"
  fi
else
  timeout 5 "${bin}" --config="${config}" > /tmp/engine_probe.log 2>&1
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
