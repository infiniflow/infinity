# Run slt test

## directory structure

* **tools/sqllogicaltest.py** is the main script to run slt test. It will call other python script to generate test data and call sqllogictest cli tool to run the test cases.
* **test/data/sql** contains slt files. slt file records the test process used by sqllogictest cli tool.
* **test/data/csv** contains csv files. some slt files may use csv files to create table.

## prerequisite

1. Install [rust](https://www.rust-lang.org/tools/install).
2. Install [sqllogictest cli tool](https://github.com/risinglightdb/sqllogictest-rs). Run `cargo install sqllogictest-bin` so that `sqllogictest` executable is available inside `PATH`.

## run the sql test

1. run `sudo rm -rf /var/infinity/*` to remove all previous data.
2. run `infinity` server, so the database is ready for test.
3. run `python3 ./tools/sqllogictest.py` under **the root path of infinity project**.
   This python script will first call `generate_big.py`, `generate_fvecs.py` and so on to generate big test file under corresponding directory, then call sqllogictest cli tool to run the test cases. The cli tools will compare the actual result of infinity and result in slt file. Difference will be marked **RED**. Only when all slt test is passed, the test is passed.


## per-index performance benchmarks

Build and query cost of each index kind, reference vs port, from one driver each:
`tools/run_index_bench.sh <index-types|multivector|secondary-lookup|secondary-maint|sdk> [rows] [queries]`.
Each engine runs in its own network namespace, so the port is private to the run.

| workload | what it measures | report |
|---|---|---|
| `index-types` | fulltext / hnsw / secondary / sparse: build + query | `rust_engine/docs/FULLTEXT_BENCH.md`, `HNSW_BENCH.md` |
| `multivector` | EMVB / PLAID / SMVE on a tensor column | `rust_engine/docs/TENSOR_INDEX_BENCH.md` |
| `secondary-lookup` | secondary index: build, point/range lookups, DML | `rust_engine/docs/SECONDARY_INDEX_BENCH.md` |
| `secondary-maint` | how much each live index costs per row to keep current | same |
| `sdk` | aggregate insert / select / point (the Gate E driver) | `rust_engine/scripts/run_gate_e.sh` |

`rust_engine/docs/INDEX_PERF.md` is the index of all of the above (what each one answers,
which numbers are current, and the caveats).

## run specific slt test, repeatly
1. Start infinity server.
2. bash tools/run_cmd_repeatly.sh 'sqllogictest test/sql/dml/insert/test_insert_tensor.slt && sqllogictest test/sql/dml/insert/test_insert.slt'

## start Python SDK test

### Run Infinity server test
1. Start infinity server.
2. Install infinity-sdk from PYPI or compiled from source code.
3. run 'python3 ./tools/run_pysdk_remote_infinity_test.py'

### Run parallel test
1. Start infinity server.
2. Install infinity-sdk from PYPI or compiled from source code.
3. run 'python3 ./tools/run_parallel_test.py'

### Run parallel test
1. Install infinity-sdk from PYPI or compiled from source code.
2. run 'python3 tools/run_restart_test.py --infinity_path=./cmake-build-debug/src/infinity'
