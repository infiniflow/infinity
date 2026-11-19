#!/usr/bin/env bash
# Gate C, nightly: kill -9 soak on a table big enough to have more than one
# segment (8,388,609 rows = the reference's 8192 * 1024 + 1).
#
# What it proves, and why each part is there:
#
#   1. **A committed row is never lost.** `kill -9` lands in the middle of a
#      `COPY`, so the WAL has a prefix the engine never acknowledged finishing;
#      after the restart the rows that were answered are still there.
#   2. **The secondary index has no gap.** The first load publishes index chunks
#      (the memory index dumps every `mem_index_capacity` rows) and leaves a
#      memory tail; a restart must index the tail on top of the published chunks
#      instead of either dropping it or rebuilding everything. Point lookups for
#      three rows of the first load are the assertion.
#   3. **The vector index keeps its rows too.** A `SPANN` index is published as a
#      *chain* of chunks (§5.4): a checkpoint appends the pages it rewrote to the
#      chunk they came from, and a restart assembles the chain from the catalog.
#      A kill -9 in the middle of the vector insert loop must leave: every
#      acknowledged row, no gap in the id range, no row twice, and a self-query
#      for a row from the base chunk *and* one from a later delta answered.
#   4. **Starting on that much data stays within a budget**, measured the way a
#      supervisor sees it: process spawn -> the `serving on` line. A restart is
#      only cheap if the recovery path is (this is the nightly form of the
#      `test_mem_bmp`-style failures).
#   5. **The delta write window survives a crash.** A timed `kill -9` almost never
#      lands between "the delta chunk is written" and "the catalog records it",
#      so round 0.5 injects exactly that: `INFINITY_CRASH_SPANN_DELTA` aborts the
#      process there (the page-tree writer's `engine.rs` hook). The restart must
#      find every acknowledged row, the orphan chunk must be swept, and the next
#      checkpoint must publish those rows again.
#
# Everything speaks the HTTP front end (`POST /query`), so the soak needs no SDK
# and no pytest - just curl and python3.
#
#   rust_engine/scripts/soak_kill9.sh                       # nightly shape
#   rust_engine/scripts/soak_kill9.sh --rows=20000 --rounds=2   # local smoke
#
# Flags (defaults come from the environment of the same name):
#   --rows=N          rows the first COPY loads        (SOAK_ROWS, 8388609)
#   --extra=N         rows the killable COPY appends   (SOAK_EXTRA_ROWS, 262144)
#   --vectors=N       rows in the vector table         (SOAK_VECTOR_ROWS, 20000)
#   --vector-extra=N  vector rows a round appends      (SOAK_VECTOR_EXTRA, 2000)
#                     (round 0.5's delta, and each kill round's writer)
#   --rounds=N        kill -9 rounds                   (SOAK_ROUNDS, 5)
#   --startup-limit=N startup budget, seconds/restart  (SOAK_STARTUP_LIMIT_S, 120)
#   --dir=PATH        work directory                   (SOAK_DIR, .scratch/soak-kill9)
#   --port=N          Thrift port                      (SOAK_PORT, 23817)
#   --http-port=N     HTTP port                        (SOAK_HTTP_PORT, 23820)
#   --seed=N          kill-delay RNG seed              (SOAK_SEED, 20261116)
#   --bin=PATH        server binary               (INFINITY_SERVER_BIN, target/release/infinity)
set -uo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../.." && pwd)"

bin="${INFINITY_SERVER_BIN:-$root/rust_engine/target/release/infinity}"
rows="${SOAK_ROWS:-8388609}"
extra="${SOAK_EXTRA_ROWS:-262144}"
vectors="${SOAK_VECTOR_ROWS:-20000}"
vector_extra="${SOAK_VECTOR_EXTRA:-2000}"
rounds="${SOAK_ROUNDS:-5}"
limit="${SOAK_STARTUP_LIMIT_S:-120}"
dir="${SOAK_DIR:-$root/.scratch/soak-kill9}"
port="${SOAK_PORT:-23817}"
http="${SOAK_HTTP_PORT:-23820}"
seed="${SOAK_SEED:-20261116}"

while [ $# -gt 0 ]; do
  case "$1" in
    --rows=*) rows="${1#*=}" ;;
    --extra=*) extra="${1#*=}" ;;
    --vectors=*) vectors="${1#*=}" ;;
    --vector-extra=*) vector_extra="${1#*=}" ;;
    --rounds=*) rounds="${1#*=}" ;;
    --startup-limit=*) limit="${1#*=}" ;;
    --dir=*) dir="${1#*=}" ;;
    --port=*) port="${1#*=}" ;;
    --http-port=*) http="${1#*=}" ;;
    --seed=*) seed="${1#*=}" ;;
    --bin=*) bin="${1#*=}" ;;
    -h|--help) sed -n '2,50p' "$0"; exit 0 ;;
    *) echo "unknown argument: $1" >&2; exit 2 ;;
  esac
  shift
done

fail=0
srv=""
startup_ms=""

# `serve` takes the Thrift address on the command line but the HTTP port from
# the environment, and a bind failure on either is fatal - so a run cannot
# silently talk to somebody else's server on the default port.
export INFINITY_HTTP_BIND="127.0.0.1:${http}"

say() { echo "== $*"; }
bad() {
  echo "FAIL: $*" >&2
  fail=1
}
now_ms() { python3 -c 'import time; print(int(time.time() * 1000))'; }

cleanup() { kill -9 "$srv" 2>/dev/null; }
trap cleanup EXIT

# The first cell of the first row, or `ERR:<reason>` - the HTTP envelope for a
# failed statement carries `error`, not a row set.
q() {
  local payload
  payload=$(python3 -c 'import json,sys; print(json.dumps({"sql": sys.argv[1]}))' "$1")
  curl -sS -m 3600 -X POST "http://127.0.0.1:${http}/query" --data-binary "$payload" |
    python3 -c '
import json, sys
raw = sys.stdin.read()
try:
    doc = json.loads(raw)
except Exception:
    print("ERR:unparseable reply: " + raw[:200])
    raise SystemExit(0)
if "error" in doc:
    print("ERR:" + str(doc["error"]))
else:
    rows = doc.get("rows") or []
    print("" if not rows or not rows[0] else str(rows[0][0]))
'
}

run() {
  local out
  out=$(q "$1")
  case "$out" in ERR:*) bad "statement failed: $1 -> ${out#ERR:}" ;; esac
}

count() {
  local out
  out=$(q "$1")
  case "$out" in
    ERR:*)
      bad "count failed: $1 -> ${out#ERR:}"
      echo -1
      ;;
    *) echo "$out" ;;
  esac
}

# start <tag> — spawn the server and wait for the readiness line, timing it.
start() {
  local tag="$1"
  local log="$dir/server-$tag.log"
  local begin deadline
  begin=$(now_ms)
  "$bin" serve "$dir/db" "127.0.0.1:$port" >"$log" 2>&1 &
  srv=$!
  deadline=$(( $(date +%s) + limit ))
  while ! grep -q "serving on" "$log" 2>/dev/null; do
    if ! kill -0 "$srv" 2>/dev/null; then
      bad "$tag: the server exited before it was ready: $(tail -n 3 "$log" | tr '\n' ' ')"
      srv=""
      return 1
    fi
    if [ "$(date +%s)" -ge "$deadline" ]; then
      bad "$tag: started in less than ${limit}s? the readiness line never appeared: $log"
      return 1
    fi
    sleep 0.2
  done
  startup_ms=$(( $(now_ms) - begin ))
  echo "   $tag: startup ${startup_ms} ms"
  if [ "$startup_ms" -gt $(( limit * 1000 )) ]; then
    bad "$tag: startup ${startup_ms} ms is over the ${limit} ms budget"
  fi
  return 0
}

stop() {
  kill -9 "$srv" 2>/dev/null
  wait "$srv" 2>/dev/null
  srv=""
}

# vec_gen <path> <count> — `(id, [v...])` tuples, one per line, for `INSERT INTO
# vec VALUES`. Deterministic per id, so `vec_literal` can reproduce a row's
# vector without reading the table: a self-query must come back as that row.
vec_gen() {
  python3 - "$1" "$2" <<'PY'
import random, sys

path, count = sys.argv[1], int(sys.argv[2])
with open(path, "w", buffering=1 << 20) as out:
    for i in range(count):
        rng = random.Random(i)
        body = ",".join(f"{rng.uniform(-1.0, 1.0):.6f}" for _ in range(8))
        out.write(f"({i}, [{body}])\n")
PY
}

# vec_literal <id> — the vector `vec_gen` gave that row.
vec_literal() {
  python3 - "$1" <<'PY'
import random, sys

rng = random.Random(int(sys.argv[1]))
print("[" + ",".join(f"{rng.uniform(-1.0, 1.0):.6f}" for _ in range(8)) + "]")
PY
}

# vec_insert <first> <count> — one `INSERT`, i.e. one transaction of the loop
# the kill interrupts. Committed batches are what the checks after a restart
# have to find.
vec_insert() {
  python3 - "$dir/vec-values.txt" "$1" "$2" <<'PY'
import sys

path, first, count = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
with open(path) as src:
    lines = src.read().splitlines()
print("INSERT INTO vec VALUES " + ",".join(lines[first:first + count]) + ";")
PY
}

# vec_check <tag> <low> — the vector index after a restart: contiguous ids,
# every sampled row exactly once, and a SPANN self-query answered for a row from
# the base chunk and one from the newest delta.
vec_check() {
  local tag="$1" low="$2" total maxid n key got
  total=$(count "SELECT COUNT(*) FROM vec;")
  maxid=$(count "SELECT MAX(id) FROM vec;")
  [ "$total" -ge "$low" ] 2>/dev/null || bad "$tag: vector rows lost ($total < $low)"
  [ "$total" = "$((maxid + 1))" ] 2>/dev/null ||
    bad "$tag: the vector ids have a gap (count $total, max id $maxid)"
  for key in 0 $((low / 2)) $((low - 1)); do
    [ "$key" -ge 0 ] 2>/dev/null || continue
    n=$(count "SELECT COUNT(*) FROM vec WHERE id = $key;")
    [ "$n" = "1" ] || bad "$tag: vector row $key appears $n times"
    got=$(q "SELECT id FROM vec SEARCH MATCH VECTOR (v, $(vec_literal "$key"), 'float', 'l2', 1);")
    [ "$got" = "$key" ] || bad "$tag: the SPANN index answered '$got' for row $key"
  done
  echo "   $tag: $total vector rows, max id $maxid, self-queries answered"
}

# gen <path> <count> <first value> — one integer per line, the shape
# `tools/generate_large_import.py` produces for the segment-boundary fixtures.
gen() {
  python3 - "$1" "$2" "$3" <<'PY'
import sys

path, count, first = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
with open(path, "w", buffering=1 << 20) as out:
    for value in range(first, first + count):
        out.write(f"{value}\n")
PY
}

# ---------------------------------------------------------------------------
# Round 0: the dataset every later round recovers.
# ---------------------------------------------------------------------------
mkdir -p "$dir"
rm -rf "$dir/db" "$dir"/*.csv "$dir"/*.log "$dir"/*.out

say "soak: rows=$rows extra=$extra rounds=$rounds startup budget=${limit}s"
say "binary: $bin"
say "work:   $dir"

say "generating the first load (${rows} rows)"
gen "$dir/rows.csv" "$rows" 0 || { bad "could not write the CSV"; exit 1; }

say "round 0: load, index, checkpoint"
if start "round-0"; then
  run "CREATE TABLE big (c1 INTEGER);"
  run "COPY big FROM '$dir/rows.csv' WITH (DELIMITER ',', FORMAT CSV);"
  got=$(count "SELECT COUNT(*) FROM big;")
  [ "$got" = "$rows" ] || bad "the first COPY loaded $got rows, expected $rows"
  run "CREATE INDEX idx_big ON big (c1);"
  # A checkpoint bounds the replay each restart pays for. The memory index keeps
  # its own chunks (dump at `mem_index_capacity`), so the restart still has a
  # published part plus a tail - the gap case.
  run "FLUSH DATA;"
  # Committed before any kill: every later round must still find these through
  # the index, or the index lost the rows it was supposed to carry.
  for key in 0 $((rows / 2)) $((rows - 1)); do
    n=$(count "SELECT COUNT(*) FROM big WHERE c1 = $key;")
    [ "$n" = "1" ] || bad "pre-kill index lookup for $key answered $n rows"
  done
  stop
fi

# The vector table: filled in small transactions, because the kill has to land
# between two of them - a single statement is either committed or not, while the
# point of the soak is the boundary (mem index, checkpoint, chunk chain).
if [ "$vectors" -gt 0 ]; then
  say "round 0: load $vectors vectors, build the SPANN chain"
  # One batch more than the kill rounds use: the injected-crash round below
  # consumes it, and the kill rounds then start after it.
  vec_gen "$dir/vec-values.txt" $((vectors + (rounds + 1) * vector_extra)) ||
    bad "cannot write the vector values"
  if start "vectors-0"; then
    run "CREATE TABLE vec (id INTEGER, v EMBEDDING(FLOAT,8));"
    batch=200
    first=0
    while [ "$first" -lt "$vectors" ]; do
      n=$(( vectors - first < batch ? vectors - first : batch ))
      run "$(vec_insert "$first" "$n")"
      first=$((first + n))
    done
    run "CREATE INDEX idx_vec ON vec (v) USING SPANN WITH \
         (min_rows = 1, metric='l2', max_partition_size=32, num_centroids=64);"
    run "FLUSH DATA;"
    vec_check "round 0" "$vectors"
    stop
  fi
fi

# ---------------------------------------------------------------------------
# Round 0.5: the delta checkpoint's write window, injected instead of timed.
#
# `INFINITY_CRASH_SPANN_DELTA` aborts the process between "the delta chunk is
# written" and "the catalog names it" - the one window of the page-tree writer a
# random `kill -9` almost never lands in. Afterwards: every acknowledged row,
# contiguous ids, no row twice (the orphan chunk is swept because no record lists
# it), and the next checkpoint publishing those rows again.
# ---------------------------------------------------------------------------
if [ "$vectors" -gt 0 ] && [ "$vector_extra" -gt 0 ]; then
  say "round 0.5: crash between writing a delta chunk and recording it"
  export INFINITY_CRASH_SPANN_DELTA=1
  if start "crash-delta"; then
    run "$(vec_insert "$vectors" "$vector_extra")"
    out=$(q "FLUSH DATA;")
    case "$out" in
      ERR:* | "") echo "   the server died in the delta write window" ;;
      *) bad "the injected crash did not fire (FLUSH answered '$out')" ;;
    esac
    stop
  fi
  unset INFINITY_CRASH_SPANN_DELTA
  vectors_seen[0]=$((vectors + vector_extra))
  if start "crash-delta-restart"; then
    vec_check "crash-delta" "${vectors_seen[0]}"
    # The rows the orphan chunk described are published by the next pass.
    run "FLUSH DATA;"
    vec_check "crash-delta re-published" "${vectors_seen[0]}"
    stop
  fi
fi

# ---------------------------------------------------------------------------
# Rounds 1..n: restart, verify, then kill -9 in the middle of a COPY.
# ---------------------------------------------------------------------------
i=1
while [ "$i" -le "$rounds" ]; do
  say "round $i: restart, verify, kill -9 during a COPY"
  if ! start "round-$i"; then
    break
  fi

  total=$(count "SELECT COUNT(*) FROM big;")
  if [ "$total" -lt "$rows" ] 2>/dev/null; then
    bad "round $i: committed rows lost ($total < $rows)"
  fi
  for key in 0 $((rows / 2)) $((rows - 1)); do
    n=$(count "SELECT COUNT(*) FROM big WHERE c1 = $key;")
    [ "$n" = "1" ] || bad "round $i: the index no longer answers $key ($n rows)"
  done
  if [ "$vectors" -gt 0 ]; then
    vec_check "round $i" "${vectors_seen[$((i - 1))]:-$vectors}"
  fi

  if [ "$vectors" -gt 0 ]; then
    vec_check "round $i" "$vectors"
  fi

  # The COPY the kill interrupts. Its values start above the first load, so it
  # can never make one of the checks above answer twice.
  gen "$dir/extra-$i.csv" "$extra" $(( rows * (i + 1) )) || bad "round $i: cannot write the extra CSV"
  payload=$(python3 -c 'import json,sys; print(json.dumps({"sql": sys.argv[1]}))' \
    "COPY big FROM '$dir/extra-$i.csv' WITH (DELIMITER ',', FORMAT CSV);")
  ( curl -sS -m 3600 -X POST "http://127.0.0.1:${http}/query" --data-binary "$payload" \
      >"$dir/copy-$i.out" 2>&1 ) &
  copier=$!
  # The vector loop the same kill interrupts: one `INSERT` per transaction, each
  # acknowledged batch recorded, so the checks after the restart know what was
  # committed.
  vin="$vectors"
  # One batch past the injected-crash round, which consumed `vectors`..+extra.
  vfirst=$(( vectors + i * vector_extra ))
  progress="$dir/vec-progress-$i"
  echo "$vfirst" >"$progress"
  (
    at=$vfirst
    last=$(( vfirst + vector_extra ))
    while [ "$at" -lt "$last" ]; do
      n=$(( last - at < 100 ? last - at : 100 ))
      case "$(q "$(vec_insert "$at" "$n")" 2>/dev/null)" in
        # The server is gone: nothing after this was acknowledged.
        ERR:*) break ;;
      esac
      at=$(( at + n ))
      echo "$at" >"$progress"
    done
  ) &
  vwriter=$!
  delay=$(python3 -c "import random; random.seed($seed + $i); print(round(random.uniform(0.5, 4.0), 2))")
  sleep "$delay"
  echo "   killing after ${delay}s of COPY"
  stop
  wait "$copier" 2>/dev/null
  wait "$vwriter" 2>/dev/null
  committed=$(cat "$progress" 2>/dev/null || echo "$vfirst")
  echo "   $committed vector rows were acknowledged"
  vectors_seen[$i]=$committed
  i=$((i + 1))
done

say "result: $([ "$fail" -eq 0 ] && echo PASS || echo FAIL) (logs in $dir)"
exit "$fail"
