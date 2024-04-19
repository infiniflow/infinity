# how to run slt test

## directory structure

* **tools/sqllogicaltest.py** is the main script to run slt test. It will call other python script to generate test data and call sqllogictest cli tool to run the test cases.  
* **test/data/sql** contains slt files. slt file records the test process used by sqllogictest cli tool.
* **test/data/csv** contains csv files. some slt files may use csv files to create table.

## prerequisite

1. Install [rust](https://www.rust-lang.org/tools/install).
2. Install [sqllogictest cli tool](https://github.com/risinglightdb/sqllogictest-rs). Run `cargo install sqllogictest-bin` so that `sqllogictest` executable is available inside `PATH`.

## start the test

1. run `sudo rm -rf /var/infinity/*` to remove all previous data.
2. run `infinity`, so the database is ready for test.
3. run `python3 ./tools/sqllogictest.py` under **the root path of infinity project**.  
   This python script will first call `generate_big.py`, `generate_fvecs.py` and so on to generate big test file under coresponding directory, then call sqllogictest cli tool to run the test cases. The cli tools will compare the actual result of infinity and result in slt file. Difference will be marked **RED**. Only when all slt test is passed, the test is passed.
